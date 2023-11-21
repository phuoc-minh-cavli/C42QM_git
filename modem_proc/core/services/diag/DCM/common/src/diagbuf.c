/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             Diagnostic Subsystem Buffer Allocation Manager

General Description
  This file contains routines that manage the diag output buffer.
  This allocation scheme is modeled as a circular buffer.

  Different types of data have different priorities.  These types and
  priorities are listed below in descending order of priority:

  1.  Event reports
  2.  Explicit diag responses
  3.  Streamed data (LOG, MSG, etc)

  Events are recorded at the head of the buffer, and will be accumulated
  until terminated (by event reporting code in event.c, etc)  A specified amount
  of space is reserved for event reporting.  If the buffer gets full (up to
  specified amount of space for events), only events will be allowed in the buffer.

  The diag task has the ability to block other tasks from accessing buffer
  space.  If diag is unable to get space, it will block other tasks (except events)
  and retry until it can get space.  Only event reports will be able to allocate space
  while diag is blocking.

Initialization and Sequencing Requirements
  diagbuf_init() must be called before using this service.

Copyright (c) 1999-2019 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.10/services/diag/DCM/common/src/diagbuf.c#11 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/18   ph      Separation of data and DCI send buffer macros.
02/20/16   gn      Fixed log drop issue in NRT mode
01/07/16   ph      Diag log packet support for ULog
07/01/16   rs      checking for diagcomm_cmd_status() instead of diagcomm_status() in diagbuf_flush_timeout()
                   before sending a response.
03/11/16   nk      Passed correct buffer pointer to diagcomm_send in diagbuf_send_pkt_single_proc_dci
02/17/16   ph      chained_extended_pkt is incorrectly reset in diagbuf_drain().
01/28/16   nk      Update the status correctly if we are flow controlled
01/05/16   ph      Flow control handled for buffering mode switch.
11/23/15   ph      WP logging stopped issue fixes.
09/01/15   gn      Fixed issue of missing Subscription ID and Proc ID for Extended logs
08/26/15   ph      Buffering mode revamp to make it functional.
08/19/15   gn      Added check for misalignment of diagbuf_tail.
08/07/15   ph      updated the send buffer pad length for NRT mode.
08/04/15   ph      Updated the threshold to send the data over DCI to 16kb.
08/06/15   ph      Feature mask check ignored for standalone configuration.
07/13/15   sa      Update the diagbuf_tail only if the packet is drained successfully.
07/01/15   ph      Updated the condition to drain the send buffer.
06/11/15   sa      Fixed Compiler Warnings.
05/25/15   ph      Fixed the issue of invalid length appearing for DCI channel.
05/20/15   xy      Added support for QShrink 4.0
05/15/15   sr      Changed the SMDL FIFO size to 8k
05/21/15   ph      Send the outbound buffer first before draining extended packets.
05/12/15   xy      Fixed issue that diagbuf drain only first part of the chained pkt before checking for signal
04/08/15   ps      Diag over sockets and dsm removal
01/12/15   sr      Added support for DLL clients
11/25/14   ph      Mainlined the feature DIAG_CONSUMER_API.
06/19/14   sa      Added support to send an event to debug diag drops.
05/23/14   ph      Remove diag_suspend for optimizing the drops.
05/01/14   ph      EVENT_POWERUP now appears on boot up.
04/24/14   tbg     Fixed issues with extended event, log and F3 pkts
04/18/14   ph      Resolve delayed response before immediate response issue.
03/20/14   is      Resolve delayed responses out of order issue
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
01/27/14   rh      Added registration to set immediate diagbuf drain during shutdown.
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
11/21/13   xy      Change DIAGCOMM_PORT_1 to DIAGCOMM_PORT_2 in single_proc_dci()
10/14/13   sr      Added support for extended event, log and F3 pkts
09/13/13   ph      Updated drain logic for TX packets of size greater than 4k
                   to send the current dsm item and then start the chain.
09/20/13   tbg     Added Support for Diag over STM
08/31/13   is      Support for peripheral buffering mode
05/24/13   sr      Resolved compiler warnings
05/11/13   is      Move drain timer from client conext to diag context
04/09/13   sr      Fixed the compiler warnings
04/03/13   sr      Replace memcpy() and memmove() with memscpy() and memsmove()
03/13/13   rh      diagbuf_flush() now handles both TX and TX Timer sigs
02/26/13   sr      Added support to turn off HDLC encoding on modem
02/15/13   sr      Moved to new Dog heartbeat implementation
01/16/13   rh      In diagbuf_drain(), we no longer flush our packet when a
                   signal comes in if we do not set the Diag drain timer.
01/03/13   sg      Command Response Channel separation
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush.
11/16/12   ph      Klocwork warning fixes
11/15/12   is      Support for preset masks
11/08/12   sr      Support for DCI Override feature(OCH) and
                   On Board Diagnostics(OBD)- Secure Sim feature
09/13/12   sa      Increased the size of DIAGBUF_PKT_HANDLER_TBL_CNT by 1
08/31/12   ra      Use accessor macros instead of accessing private task
                   info directly; accomodated changes for User space
                   task priority changes
08/20/12   ra      Renamed ambiguous static symbols in
                   diagbuf_send_pkt_single_proc_ctrl()
05/03/12   rs      Moved delayed response from diag buffer to AMSS heap.
03/07/12   rh      Fixed Klockwork Errors
12/01/11   is      Fix buffer corruption checks
11/09/11   hm      Fixed compiler warnings and enable DCI traffic even when
                           QPST is not connected
11/08/11   hm      Added NULL pointer check in diag_non_hdlc_encode()
11/07/11   is      Fixed compiler warnings
11/04/11   hm      Increased diagbuf_outbound_pkt_handler_tbl size to accommodate
                   for delayed response handler
10/14/11   hm      Diag consumer API - modem side changes
04/27/11   is      Resolve compiler warnings
04/18/11   is      Ensure CTRL msg sizes are constant
03/01/11   is      Removed unused AEEstd.h
02/10/11   vs      Fixed case of incorrect drops if invalid state is read
                     in alloc_internal
12/20/10   mad     Changes for dual mask diag
01/18/11   vs      removed inline for diagbuf_clear
01/03/11   vs      calling digbuf_clear for case of DIAGBUF_HOLE_S
12/10/10   vs      Removed error fatal in internal_alloc. Check invalid
12/10/10   vs      Clear status bit in diagbuf_drain, revert changes to
                    diagbuf_shorten
11/23/10   vs      Fix corner case in encode function where a partial f3
                    message was being sent out incorrectly.
11/22/10   vs      Fix for corner case with lock-less implementation
11/05/10   vs      diagbuf returns 8 byte aligned addresses for logs on q6
10/28/10   vs      Added lock-less implementation for diagbuf_alloc_internal
10/25/10   vs      Added implementation for faster encoding
10/25/10   is      Diag feature flag cleanup
09/22/10   mad     Added diagbuf_ctrl_tx_sleep_parameters
09/14/10   is      Remove duplicate inclusion of diagcomm_v.h
09/13/10   is      Add flow control count and DSM chaining count to Diag Health
07/22/10   sg      Added changes to support dsm chaining for  rx response packets
07/15/10   sg      Check whether diag is connected while setting the drain timer
06/22/10   sg      Merging back DCM rtos and WM files
06/20/10   sg      check whether diag is initialized before allocating space from
                   diagbuf when clients call diag apis
05/07/10   is      Mainlined DIAGBUF_HEAP
04/29/10   is      Removed support for DIAG_DEBUG_FS_PKT_LOG
04/15/10   JV      Fix for klocwork errors
03/14/10   vs      Added sleep based on threshold for a case in
                    diagbuf_send_pkt_single_proc.
03/14/10   vs      Added check for full buffer case in single_proc function
                    This is sent out without chaining now.
03/06/10   JV      When bailing out of diagbuf_drain(), set the drain timer
                   if diagbuf in not empty.
03/06/10   JV      Revert setting the TX sig in diagbuf_alloc_internal.
03/05/10   JV      Set the TX sig to start draining if diagbuf_alloc fails
                   because diagbuf is full.
03/02/10   JV      Moved DIAG_TX_SLEEP_TIME_DEFAULT and
                   DIAG_TX_SLEEP_THRESHOLD_DEFAULT to diagbuf_v.h since these
                   are required even in diagdiag_common.c.
02/19/10   vs      Fixed compiler warning in diagbuf_init
02/02/10   JV      Do not poll for events at every packet boundary
01/28/10   vs      Flush responses also in diagbuf_flush
01/18/10   JV      Consolidate OSAL APIs
01/08/10   vs      Modified diagbuf_send_pkt_single_proc to handle packet
                   boundaries.
12/28/09   ps      Fixed Klocwork errors & compiler warnings.
12/16/09   JV      Including diagnv.h causes a compilation error on the audio Q6
12/10/09   sg      Added flow control mechanism for DSM_DIAG_SIO_TX_ITEM_POOL
                   at item level granularity
12/07/09   cahn    Added Delayed Response to DIAG Health.
11/30/09   ps      Fixed compiler warnings.
11/18/09   ps      Added qsr feature support.
11/04/09   cahn    DIAG Health
10/21/09   JV      Putting in a new feature so that we can now use the
                   diagbuf_send_pkt_multi_proc() function to send data out
                   if we require the response to be prioritized.
09/28/09   JV      Remove mutexes from diag task
09/28/09   JV      Include diag_cfg.h for the config related info
09/22/09   vs      Use OSAL for all rex calls on qdsp6.
09/09/09   JV      Changed signature of osal mutex calls.
07/13/09   vs      In diagbuf_commit - set the diag signal after a
                   certain number of bytes have been commited.
08/05/09   JV      Removed nested INTLOCKS (osal mutexes).
08/05/09   JV      Removed the CUST_H featurization around the inclusion of
                   customer.h.
08/05/09   JV      task.h is now included in osal.
07/31/09   JV      Merged Q6 diag code back to mainline
07/23/09   JV      Including diagdsm_v.h instead of diagdsm.h
07/10/09   as      Decoupled FEATURE_DIAG_NON_STREAMING & FEATURE_REX_CORE_DUMP
05/12/09   JV      Introduced the OS abstraction layer for rex.
05/07/09   mad     Clean-up: moved DIAG_TX_SLEEP_TIME_DEFAULT and
                   DIAG_TX_SLEEP_THRESHOLD_DEFAULT from diagbuf_v.h to
                   diagbuf.c, since these are required only in diagbuf.c
05/07/09   vk      changed includes from external to internal headers
04/22/09   mad     Modified diagbuf_tx_sleep_time_set() and
                   diagbuf_tx_sleep_threshold_set(), combined these into one
                   function, diagbuf_tx_sleep_parameters_set()
04/03/09   mad     now DIAGBUF_SIO_TX_ITEM_SIZ mirrors
                   DSMI_DIAG_SIO_TX_ITEM_SIZ
03/20/09   mad     change on behalf of vsharma: Change DIAGBUF_SIO_TX_ITEM_SIZ
                   to match increased DSM item size
03/20/09   vg      Update diag_alloc_internel to be more thread safe.
03/04/09   vg      Merge Q6 diag buffer code to be thread safe access
01/08/09   as      Update diag buffer code to be thread safe access.
10/03/08   vg      Update code to use PACK() vs. PACKED
02/05/08   pc      Added support for dynamic threshold and sleep values
12/16/07   pc      Added support for diag_dtr_enable and diag_dtr_changed_cb.
02/07/02   as      Made diagbuf_tail volatile. Moved sanity checks within
                   INTLOCKS. Check for min DSM items before filling them.
12/01/07   as      Moved diagbuf_write_overrun_check_pattern inside INTLOCK().
                   Added sanity checks in alloc and diagbuf_drain().
12/21/06   as      Fixed boundary cases in multiple Diag packets.
12/15/06   as      Fixed compiler warnings.
12/05/06   as      Support to include multiple Diag packets into one DSM item.
08/21/06   bfc     Removed a MSG_HIGH that was exacerbating an dsm_item
                   deficiency
10/31/05   as      Fixed lint errors.
10/19/05   as      Fixed CRC update in hdlc encode.
02/15/05   as      Removed PLATFORM_LTK feature to allow data send to be split
                   into different packets.
01/12/05   as      Fixed hdlc encode bug in CRC
01/07/05   as      Modified the location of check for empty diag buffer. This
                   fixes the invalid check for empty diag heap.
12/06/04   as      Included dog.h to prevent compiler warnings
09/21/04   as      Changed hdlc encode to fill dsm items with 128 bytes. But,
                   if the last byte is an escape character (0x7D or 0x7E),then
                   only 127 bytes are filled in dsm item.
03/30/03   as      Added ASSERTS in functions that manipulate the diag buffer
                   and intlocks in diagbuf wrap and move macros.
10/28/03   as      Changes to fix warnings on ads12 compiler
12/02/02   lad     Removed blocking alloc (to diagpkt.c), enhacned packet
                   bounadary processing to diagbuf_drain() to give priority
                   to diagpkt rsp packets, and added periodic sleep logic with
                   inert defaults to avoid starvation of lower priority tasks.
07/30/02   as      Updated diagbuf_get_log and diag_send_tx_packet to support
                   FEATURE_DIAG_NON_STREAMING
06/25/02   as      Changed diagbuf_internal_alloc, diagbuf_shorten, diagbuf_commit,
                   and diagbuf_drain to obtain byte allignment.
10/24/01   jal     Changed diagbuf_flush() to wait for SIO to finish
                   flushing UART before returning--guarantees last
                   response packet gets sent.
06/27/01   lad     Made private data and functions static.
                   Removed "code" from diagbuf_blocking_alloc() - this is
                   done in diagpkt_alloc().
                   Updated usage of log_config().
06/22/01   lad     Fixed diagbuf_drain to check for committed packet after wrap.
                   Changed diagbuf_buf to be aligned on 32-bit boundary.
04/17/01   lad     Modified diagbuf_alloc(), diagbuf_shorten(), and
                   diagbuf_commit() to detect buffer overrun by users.
                   Changed call to diag_do_escaping() to remove const args.
02/23/01   lad     Made changes for newly architected diagnostics service.
                   Introduced the communications layer and the use of diagtune.h
                   to configure this code.
                   Many cosmetic changes were made as well.
11/28/00   lad     Changed diagbuf_drain() to return correct status when no
                   open communications stream exists.
11/10/00   jal     Killed third call to event_drain() in diagbuf_drain().
                   It was causing an endless loop when there are no events
                   and only un-committed data in the ring buffer.  It isn't
                   needed anyways.
11/07/00   jal     Bugfixes to diagbuf_drain().  Moved handling
                   DIAG_EVENT_TIMER_SIG out of diagpkt_blocking_alloc() into
                   diag.c/diag_wait() per Larry.  Removed diag_set/clr_sigs
                   debug hack.
10/17/00   jal     Moved diagbuf_OKTX, diagbuf_start/stop_flow() to diag.c
                   Moved event_stale_timer over to event.c.  Mods to
                   support new event accumulation mechanism.
09/25/00   jal     Rewrote diagbuf_drain() to a) use the new DSM item
                   interface, and b) to use DSM items more efficiently.
                   Instead of using one item for each response packet
                   (wasteful!) we fill each item until we have nothing
                   more to send.
09/12/00   lad     Fixed load balancing check in diagbuf_alloc_internal.
08/28/00   lad     Optimized the routines in this module.
08/25/00   rc      Changed dsm_new_buffer() calls to take pool id as input
                   parameter.
07/06/00   jal     Changed KICK_WATCHDOG to DIAG_KICK_WATCHDOG to avoid
                   conflict with other (FS) VUs
06/23/00   lad     Removed obsolete diag_sleep reference.
06/19/00   lad     Added MSM5000_IRAM_FWD aroung diagbuf_alloc_internal()
06/09/00   lad     Added flags for MSM5000_IRAM_FWD
05/15/00   lad     Updated load balancing.
05/10/00   lad     Added load balancing.  Changed API to give diag_rsp_type*
                   instead of void*
03/07/00   lad     Fixed flow control for FEATURE_DIAG_NON_STREAMING.
02/28/00   lad     Created file.

===========================================================================*/
#include "core_variation.h"
#include "feature.h"
#include "comdef.h"
#include "diag_v.h"
#include "diagbuf_v.h"
#include "diagcomm_v.h"
#include "diagcomm_smd.h"
#include "diagdiag_v.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "diagtarget.h"
#include "diagtune.h"
#include "err.h"
#include "event.h"
#include "log.h"
#include "msg.h"
#include "msgi.h" /* For msg_set_all_rt_masks_adv() */
#include "stringl.h"
#include "diag_mode_v.h"
#include "ULog_Diag.h"

#include <stdio.h>
#include "diagdsm_v.h"
#include "crc.h"
#include "osal.h"
#include "diag_cfg.h"
#include "diagcomm_cmd.h"

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h"
#include "diagnv.h"
#if !defined (FEATURE_WINCE)
#include "osal.h"
#endif
#endif /* defined (DIAG_QDSP6_APPS_PROC) */

#if defined FEATURE_WINCE
#error code not present
#endif

#include "diagcomm_ctrl.h" /* For DIAG_CTRL_MSG_DIAGDATA etc */
#include "time_svc.h"

#include "err.h"
static osal_mutex_arg_t diagbuf_buf_cs;

/*lint -esym(751, diagbuf_header_status_enum_type) */

/* Supress local typedef 'diagbuf_header_status_enum_type' not referenced */

/*---------------------------------------------------------------------------
  The following are indices for the ring buffer used for streaming diag
  output from the DMSS.  Each packet will be stored in the following format.
  Numbers are expressed as bytes.

  -----------------------------
  |                           |
  |  Length (2)               |  <-- Length of entire packet
  |                           |      (Data + CRC + Length field)
  -----------------------------
  |                           |
  |                           |
  |  Output Data (Length - 4) |  <-- Buffer given to caller (ptr to this)
  |                           |
  |                           |
  -----------------------------
  |                           |
  |  CRC (2)                  |  <-- CRC required for transport protocol
  |                           |
  -----------------------------

---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  The following variables are used to manage the ring buffer allocation scheme.
  The circular model is composed of a linear buffer of size 2*N where N is the
  size of the buffer.  The index to the buffer = idx % N;  This handles buffer
  wrap since head - tail will always yield the amount of allocated space.

  diagbuf_tail (T) - The end of the used buffer.  Data is drained to the SIO
                     queues from here.

  diagbuf_head (H) - The front of buffer (where writers allocate)

  With this scheme, the following calculations hold true:

  Empty condition:        H == T
  Full condition:         H - T == N
  Amnt used:              H - T

  Since 'int2' operations are atomic, the only critical section is
  moving the head.

----------------------------------------------------------------------------*/


/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */

/*===========================================================================

MACRO DIAGBUF_EMPTY
MACRO DIAGBUF_FULL

DESCRIPTION
  Conditional macros for diag streaming buffer.

RETURN VALUE
  Conditional values.

===========================================================================*/
#define DIAGBUF_EMPTY(h, t) (h == t)


/*===========================================================================

MACRO DIAGBUF_USED
MACRO DIAGBUF_FREE

DESCRIPTION
  Status calculating macros for the diag ring buffer.

PARAMETERS
  h - head index
  t - tail index

RETURN VALUE
  Number of bytes.

===========================================================================*/
#define DIAGBUF_USED(h, t) ((h >= t) ? (h - (t)) : (h - (t) + 2*DIAGBUF_SIZE))

#define DIAGBUF_FREE(h, t) (DIAGBUF_SIZE - DIAGBUF_USED(h, t))



/*===========================================================================

MACRO DIAGBUF_CALC_IDX

DESCRIPTION
  Calculates index in DIag output buffer.

PARAMETERS
  Abstract buffer index

DEPENDENCIES
  0 <= x < (2 * DIAGBUF_SIZE)

RETURN VALUE
  Index in diag ring buffer.

===========================================================================*/
#define DIAGBUF_CALC_IDX(x) ((x >= DIAGBUF_SIZE) ? (x - DIAGBUF_SIZE) : x)



/*===========================================================================

MACRO DIAGBUF_WRAP

DESCRIPTION
  Wraps abstract buffer index.

PARAMETERS
  Abstract buffer index.

DEPENDENCIES
  0 <= x < (2 * DIAGBUF_SIZE)

===========================================================================*/
#define DIAGBUF_WRAP(x)                    \
do {                                       \
  x += DIAGBUF_SIZE - DIAGBUF_CALC_IDX(x); \
  if (x >= 2 * DIAGBUF_SIZE)               \
    x -= 2 * DIAGBUF_SIZE;                 \
/*lint -save -e{717} */                      \
} while(0)                                 \
/*lint -restore */



/*===========================================================================

MACRO DIAGBUF_MOVE

DESCRIPTION
  Moves the buffer index.

PARAMETERS
  Buffer index.

DEPENDENCIES
  0 <= x < (2 * DIAGBUF_SIZE)

===========================================================================*/
#define DIAGBUF_MOVE(i, l)   \
do {                         \
  i += l;                    \
  if (i >= 2 * DIAGBUF_SIZE) \
   i -= 2 * DIAGBUF_SIZE;   \
/*lint -save -e{717} */        \
} while(0)                   \
/*lint -restore */


/* To fill in diagbuf header for a hole, alignement need not be 8 byte aligned
   on q6, macro below aligns on 32 bit boundary */
#define DIAGBUF_NEXT_ALIGNED_BYTE_4(p) (((p) + 3) & ~3)

#define DIAGBUF_ALIGNED_BYTE_4(p) ((p) & ~3)
/* Diag's output buffer.  The ADS 1.0 compiler will not align a byte array on
 * 32-bit boundary.  The ARM 2.5 compiler had a setting for this, but not ADS
 * 1.0.  Therefore, we must declare it to be a array of 32-bit values and
 * assign a byte* to point to it. */

#define DIAGBUF_HEADER_PATTERN 0xDEAD


static inline void diagbuf_clear(int start, int offset);
	
static uint32 diagbuf_buf_aligned[(DIAGBUF_SIZE + 3)/4];
static uint8 *diagbuf_buf = (uint8 *) diagbuf_buf_aligned;

/* Head and tail of circular buffer.
*/
static volatile int diagbuf_head = 0;
static volatile int diagbuf_tail = 0;

extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];

uint8 diagbuf_send_cmd[NUM_SMD_PORTS][DIAG_SEND_BUF_SIZE]; /*Buffer used to send out cmd rsp */
#ifdef DIAG_CONSUMER_API
  static uint16 diag_send_buf_data_index[NUM_SMD_PORTS] = {0, 0}; /*Current data buffer index*/
  static uint16 diag_send_buf_cmd_index[NUM_SMD_PORTS] = {0, 0}; /*Current cmd buffer index*/
#else
  static uint16 diag_send_buf_cmd_index[NUM_SMD_PORTS] = {0};
#endif /* DIAG_CONSUMER_API */
extern uint32 diag_send_buf_threshold; /*Threshold to buffer up data in send buffer*/
extern uint32 diag_max_send_buf_len; /*Maximum send size*/
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /*From diagcomm_cfg.c*/

/*Maps each version of the diagbuf packet to the length of the first packet in the chain*/
uint8 diagbuf_ver_len[DIAGBUF_MAX_VERSION_NUM];

#ifdef FEATURE_DIAG_SWITCH
 extern int diag_on_single_proc;
#endif

#ifndef DIAGBUF_PKT_HANDLER_TBL_CNT
/* DIAG_QSR_EXT_MSG_TERSE_F, DIAG_EXT_MSG_F and DIAG_LOG_F use this feature */
    #define DIAGBUF_PKT_HANDLER_TBL_CNT 6
 #endif

#define DIAG_MODEM_HDLC_TURNED_OFF 1
#define DIAG_MODEM_HDLC_TURNED_ONN 0

static struct {
  uint8 cmd_code;
  diagbuf_outbound_pkt_handler_type fp;
} diagbuf_outbound_pkt_handler_tbl [DIAGBUF_PKT_HANDLER_TBL_CNT];

typedef struct
{
  void *dest;
  void *dest_last;

  /* Below: internal use only */
  uint16 crc;
} diag_hdlc_dest_type;

/* External declrations */
extern boolean diagpkt_rsp_send (void);
extern boolean diag_tx_pending;
extern boolean diag_drain_timer_pending;
extern diag_tx_mode_type diag_tx_mode[DIAG_MAX_STREAM_ID];


/* Forward prototype declarations for local routines. */
void diag_add_block_header(diag_send_buf_type* send_buf, uint16 length);

void diag_hdlc_encode (diag_send_desc_type *src_desc, diag_hdlc_dest_type *enc);

void diag_non_hdlc_encode (diag_send_desc_type *src_desc, diag_hdlc_dest_type *enc);

static void diagbuf_write_overrun_check_pattern (void *ptr);

static diagbuf_outbound_pkt_handler_type
  diagbuf_get_custom_pkt_handler (uint8 cmd_code);

/* Input buffer used for encoded transmit data. For packets
   that exceed DSM item size, this buffer is used as output buffer */
#define DIAG_MAX_TX_PKG_PKT_SIZ (12288)

/* DIAG_MAX_RX_PKT_SIZ is the biggest packet.
   that can be allocated, times 2 is worst case scenario where every
   character is escape. Addition of two is for CRC
   (DIAG_MAX_RX_PKT_SIZ/DIAGBUF_SIO_TX_ITEM_SIZ)*2 + 2 */
#define DIAGBUF_MIN_DSM_ITEM_CNT   5

/* Count for packets in a DSM item */
uint32 diagbuf_num_pkts_per_dsm_item = 0;
uint32 diagbuf_num_bytes_per_dsm_item = 0;
uint32 diagbuf_num_bytes_per_dsm_item_sent = 0;

#if defined DEBUG_DIAG_TEST
#define DIAGBUF_MAX_DEBUG_COUNT 8192
typedef struct
{
  uint32 num_diag_pkts;
  uint32 num_bytes_pkged;
  uint32 num_bytes_sent;
} diag_pkg_type;

uint32 diag_pkg_count = 0;

diag_pkg_type diag_pkg[DIAGBUF_MAX_DEBUG_COUNT];

#define diag_pkg_append(x, y, z) \
   diag_pkg[diag_pkg_count].num_diag_pkts = x;\
   diag_pkg[diag_pkg_count].num_bytes_pkged = y;\
   diag_pkg[diag_pkg_count].num_bytes_sent = z;\
   diag_pkg_count++; \
   if (diag_pkg_count >= DIAGBUF_MAX_DEBUG_COUNT ) \
      diag_pkg_count = 0;
#endif

#define DIAG_HDLC_CRC_PAD (3) /* 2 bytes CRC. 1 byte 7E terminator. */
#define DIAG_NHDLC_PAD (5) /* 1 byte 7E start character, 1 byte version, 2 bytes length, 1 byte 7E terminator. */
#define DIAG_NHDLC_TERMINATE_PAD (1) /* 1 byte 7E terminator */

static uint8 diagbuf_hdlc_pad_len = DIAG_HDLC_PAD_LEN;

/* Variables to store drop count for DIAG Health */
uint32 drop_count_log = 0;
uint32 drop_count_f3 = 0;

/* Variables to store allocation requests for DIAG Health */
uint32 alloc_count_log = 0;
uint32 alloc_count_f3 = 0;

/* Diag Percentage Drop Count Variables */
extern uint8 diag_per_count_dropped; /* Diag Percentage Drop Count  from diag.c */
boolean bCountReset = FALSE; /* To track if the log/drop counts are reset */

static uint32 diagbuf_dsm_chained_count = 0; /* Diag Health counter - Keeps track of # of times DSM chaining occurs */
static uint32 diagbuf_dsm_count = 0;         /* Diag Health counter - Keeps track of # of times DSMs sent */

#ifdef DIAG_CONSUMER_API
  uint8 diagbuf_send_data_dci[DIAG_SEND_BUF_SIZE_DATA_DCI]; /*Buffer used to send out F3s/logs/events over DCI */
#endif /* DIAG_CONSUMER_API */
uint8 diagbuf_send_data[DIAG_SEND_BUF_SIZE_DATA]; /*Buffer used to send out F3s/logs/events */

/* Quick and dirty way to have all future messages get flushed out immediately.
   Assumes that more messages will be generated or a timer ticks over. */
static void diagbuf_immediate_delivery(void)
{
  diagbuf_ctrl_commit_threshold(0);
  diag_tx_notify();
}

extern boolean g_is_cmd_rsp_type;

extern uint32 diag_buffered_bytes(uint8 stream_index); /*From diagcomm_smd.c */

/* This variable holds the length of any type of packet (Log, Msg, Event, User PD packets), 
   update this before calling diagbuf_send_pkt. No need to reset this variable as the calculation is
   done for every invokation of diagbuf_send_pkt */
uint32 diag_pkt_len;

/*===========================================================================

FUNCTION DIAGBUF_INIT

DESCRIPTION
   This function initializes the diag allocation buffer at startup.
===========================================================================*/
void
diagbuf_init (void)
{
  diagbuf_header_type *pkt_hdr; /* pointer to the first header */


  int i, return_val = 0;

  diagbuf_head = 0;
  diagbuf_tail = 0;


  /* Setting the first status field to DIAGBUF_UNCOMMIT_S is all the
  ** initizalization needed
  */
  pkt_hdr = ((diagbuf_header_type *) &diagbuf_buf[0]);
  pkt_hdr->status = DIAGBUF_UNUSED_S;

  for (i = 0; i < DIAGBUF_PKT_HANDLER_TBL_CNT; i++)
  {
    diagbuf_outbound_pkt_handler_tbl[i].cmd_code = 0xFF;
    diagbuf_outbound_pkt_handler_tbl[i].fp = NULL;
  }


  /*Initialize the array used to store the length of first pkt in
    the chain in diagbuf*/
  memset(diagbuf_ver_len, 0, DIAGBUF_MAX_VERSION_NUM);

  /* Initialize diagbuf mutex */
  diagbuf_buf_cs.name = "MUTEX_DIAGBUF_BUF_CS";
  return_val = osal_init_mutex(&diagbuf_buf_cs);
  ASSERT(OSAL_SUCCESS == return_val);

  /* Set up callback for rapid delivery during shutdown
     Group 7 is the first shutdown group */
  rcinit_register_termfn_group(RCINIT_GROUP_7, diagbuf_immediate_delivery);

} /* diagbuf_init */

/*===========================================================================
FUNCTION DIAGBUF_RESET_DROP_COUNT_LOG

DESCRIPTION
    This function resets the internal variables drop_count_log and
    alloc_count_log.
===========================================================================*/
void diagbuf_reset_drop_count_log( void ){

  drop_count_log = 0;
  alloc_count_log = 0;
  bCountReset = TRUE;

}
/*===========================================================================
FUNCTION DIAGBUF_GET_DROP_COUNT_LOG

DESCRIPTION
    This function returns the internal variable drop_count_log.
===========================================================================*/
uint32 diagbuf_get_drop_count_log( void ){
    return drop_count_log;
}

/*===========================================================================
FUNCTION DIAGBUF_GET_ALLOC_COUNT_LOG

DESCRIPTION
    This function returns the internal variable alloc_count_log.
===========================================================================*/
uint32 diagbuf_get_alloc_count_log( void ){
    return alloc_count_log;
}

/*===========================================================================
FUNCTION DIAGBUF_RESET_DROP_COUNT_F3

DESCRIPTION
    This function resets the internal variables drop_count_f3 and
    alloc_count_f3.
===========================================================================*/
void diagbuf_reset_drop_count_f3( void ){
  drop_count_f3 = 0;
  alloc_count_f3 = 0;
  bCountReset = TRUE;
}
/*===========================================================================
FUNCTION DIAGBUF_GET_DROP_COUNT_F3

DESCRIPTION
    This function returns the internal variable drop_count_f3.
===========================================================================*/
uint32 diagbuf_get_drop_count_f3( void ){
    return drop_count_f3;
}

/*===========================================================================
FUNCTION DIAGBUF_GET_ALLOC_COUNT_F3

DESCRIPTION
    This function returns the internal variable alloc_count_f3.
===========================================================================*/
uint32 diagbuf_get_alloc_count_f3( void ){
    return alloc_count_f3;
}

/*===========================================================================
FUNCTION DIAGBUF_RESET_DSM_CHAINED_COUNT

DESCRIPTION
    Resets the internal DSM chaining counts for Diag Health.

===========================================================================*/
void diagbuf_reset_dsm_chained_count( void )
{
  diagbuf_dsm_chained_count = 0;
  diagbuf_dsm_count = 0;
}

/*===========================================================================
FUNCTION DIAGBUF_GET_DSM_CHAINED_COUNT

DESCRIPTION
    Returns the internal variable diagbuf_dsm_chained_count for Diag Health.

===========================================================================*/
uint32 diagbuf_get_dsm_chained_count( void )
{
  return diagbuf_dsm_chained_count;
}

/*===========================================================================
FUNCTION DIAGBUF_GET_DSM_COUNT

DESCRIPTION
    Returns the internal variable diagbuf_dsm_count for Diag Health.

===========================================================================*/
uint32 diagbuf_get_dsm_count( void )
{
  return diagbuf_dsm_count;
}

/*===========================================================================
FUNCTION DIAGBUF_INCR_DSM_CHAINED_COUNT

DESCRIPTION
    Increments diagbuf_dsm_chained_count for Diag Health.

===========================================================================*/
void diagbuf_incr_dsm_chained_count( void )
{
  if( diagbuf_dsm_chained_count == MAX_VALUE_UINT32 )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagbuf_dsm_chained_count has reached max value");
  }
  else
  {
    diagbuf_dsm_chained_count++;
  }
}

/*===========================================================================
FUNCTION DIAGBUF_INCR_DSM_COUNT

DESCRIPTION
    Increments diagbuf_dsm_count for Diag Health.

===========================================================================*/
void diagbuf_incr_dsm_count( void )
{
  if( diagbuf_dsm_count == MAX_VALUE_UINT32 )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagbuf_dsm_count has reached max value");
  }
  else
  {
    diagbuf_dsm_count++;
  }
}

/*===========================================================================
FUNCTION DIAGBUF_GET_DROP_PERCENTAGE
DESCRIPTION
    This function returns the percentage total percentage drop
    from the diag buffer (f3 and log percentage drops)
===========================================================================*/
uint8 diagbuf_get_drop_percentage(void){
  return diag_per_count_dropped;
}

/*===========================================================================
FUNCTION DIAGBUF_ALLOC_CHAIN

DESCRIPTION
    Allocates chained(consecutive) packets in Diag buffer.
    This function should be called only for extended log pkts

PARAMETERS
   length                    Length of requested buffer
   nice                      Niceness padding value
   diagbuf_alloc_request     Required for DIAG Health
   chain                     0 - not chained
                             1 - first pkt in the chain
                             2 - second pkt in the chain
   version                   0 - non extended pkts
                             1 or 2 - extended pkts
   id                        Passed in by diag client while
                             calling log APIs and F3 macros

DEPENDENCIES
  This function must only be called from within this file by externalized
  alloc functions.

  diagbuf_commit() must be called to commit a buffer to the diag pool.  This
  must happen reasonably soon after this call completes.  If the buffer is
  needed to accumulate data over time, then you should use your own buffer
  use this just long enough to copy the data.

  diagbuf_commit() should be called seperately for the two chained pkts in
  diagbuf.

RETURN VALUE
  Pointer to the allocated space in diagbuf
  NULL if there is no space available in diagbuf

===========================================================================*/
PACK (void *) diagbuf_alloc_chain(
   uint8 code,
   uint32 length,
   int nice,
   uint8 diagbuf_alloc_request,
   uint8 chain,
   uint8 version,
   void* parameter,
   byte stream_id )
{
  uint32 buf_used = 0;
  uint32 buf_used_first_pkt = 0;
  uint32 buf_used_second_pkt = 0;
  uint8 pad_len1 = 0;
  uint8 pad_len2 = 0;
  diagbuf_header_type *pkt_hdr;
  uint32 ver_len = 0;
  byte * ptr_start = NULL;
  void * ptr1 = NULL;
  diag_cmd_ext_v1_type *ptr_payload = NULL;
  diag_cmd_ext_v2_type *ptr_payload2 = NULL;
  char temp_buf[ULOG_MAX_NAME_SIZE];
  uint32 name_len=0;
  time_type toffset;
  boolean is_offset_negative = FALSE;
  uint64 hi;
  uint32 lo;

  /*Sanity checks*/
  if ((version > DIAGBUF_MAX_VERSION_NUM)  ||  /*currently support only version 0 and 1 */
       (chain > DIAGBUF_LAST_PKT_CHAIN) ||     /* allow only two packets to be chained in diagbuf*/
      (code != DIAG_LOG_F)) /*allow only log extended pkts to be chained in diagbuf*/
  {
    return NULL;
  }

  if( (version > DIAGBUF_VERSION_0 ) && (parameter == NULL) )
  {
      return (NULL);
  }

  /* Currently, we support only version 0,1 and 2 pkts */
  switch(version)
  {
    case DIAGBUF_VERSION_0:
    {
        return (diagbuf_alloc(code, length, nice, diagbuf_alloc_request, chain, version));
    }
    case DIAGBUF_VERSION_1:
    case DIAGBUF_VERSION_2:
    {

        if(version == DIAGBUF_VERSION_1)
        {
            /*version 1 pkt is of the following format:
            |cmd_code-DIAG_LOG_F (1 byte)|version (1 byte)|proc_id (2 bytes)|ID (4 bytes)|*/
            ver_len = sizeof(diag_cmd_ext_v1_type);
        }
        else
        {
            ver_len = sizeof(diag_cmd_ext_v2_type);

            if(ULogCore_NameGet( parameter, &temp_buf[0]) == DAL_SUCCESS)
            {
                /*Match found, get length*/
                name_len= strlen(temp_buf);

                if(name_len > ULOG_MAX_NAME_SIZE )
                {
                    ERR_FATAL("Invalid ULog ", 0, 0, 0);
                }
            }
            else
            {
               MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "ULogCore_NameGet returned fail");
               return NULL;
            }
        }
        /*Calculate the length of the first packet*/
        buf_used_first_pkt = ver_len +   /* payload of the first packet*/
            sizeof(uint16);   /* CRC - to account for DEAD pattern for the first packet in the chain*/

        /*Make the first packet byte aligned*/
        buf_used_first_pkt = DIAGBUF_NEXT_ALIGNED_BYTE(buf_used_first_pkt);

        /*Calculate the length of the second pkt;
        Account for the diag buffer header for the second pkt here*/
        buf_used_second_pkt = length +
            sizeof(diagbuf_header_type);  /* Diagbuf header for the second packet */

        /*Total length required for both the chained pkts;
            Call diagbuf_alloc() only once since we cant gaurantee if
            the chained pkts are consecutive if we call diagbuf_alloc() twice
        */
        buf_used = buf_used_first_pkt + buf_used_second_pkt;

    diagbuf_ver_len[version - 1] = buf_used_first_pkt / 4;

        ptr_start = (byte *)diagbuf_alloc(code, buf_used, nice, diagbuf_alloc_request, chain, version);

    if (ptr_start == NULL)
        {
      return NULL;
        }

        /*Point to the first diagbuf header*/
        pkt_hdr = (diagbuf_header_type *)(ptr_start - sizeof(diagbuf_header_type));

        /*Set the length in the first diagbuf header pkt*/
        pkt_hdr->length = ver_len;
        pkt_hdr->stream_id = stream_id;
        pad_len2 = pkt_hdr->pad_len;
        pad_len1 = ((uint32)buf_used_first_pkt - ver_len - sizeof(uint16));
        ASSERT(pad_len2 < 4);
        ASSERT(pad_len1 < 4);
        pkt_hdr->pad_len = pad_len1;

        if(version == DIAGBUF_VERSION_1)
        {
            /*Assign the version and ID field*/
            ptr1 = (void *)ptr_start;
            ptr_payload = (diag_cmd_ext_v1_type *)ptr1;
            ptr_payload->version = DIAGBUF_VERSION_1;
            ptr_payload->proc_id = DIAG_MY_PROC_ID;
            ptr_payload->id = *((uint32 *)parameter);
        }
        else
        {
            /*Assign the version and other fields*/
            ptr1 = (void *)ptr_start;
            ptr_payload2 = (diag_cmd_ext_v2_type *)ptr1;
            ptr_payload2->version = DIAGBUF_VERSION_2;
            ptr_payload2->proc_id = DIAG_MY_PROC_ID;
            ptr_payload2->name_len = name_len;
            ptr_payload2->time_offset_type = DIAG_ULOG_SYNC_OFFSET_TYPE;
            is_offset_negative= time_get_sum_genoff_ts(toffset);

            hi = (uint64)qw_hi(toffset); /*Most significant 32 Bits*/
            lo = qw_lo(toffset);         /*Least Significant 32 Bits*/

            ptr_payload2->time_offset= ((hi<<32) | lo );

            if(is_offset_negative)
            {
                /*Set MSB in the 64bit value if offset is negative*/
                ptr_payload2->time_offset|= 0x8000000000000000;
            }

            strlcpy(ptr_payload2->ulog_name, temp_buf, name_len + 1 );
        }

        diagbuf_write_overrun_check_pattern(ptr_start - 4);

        /*Fill out the second header*/
        ptr1 = (void *)(ptr_start + buf_used_first_pkt);
        pkt_hdr = (diagbuf_header_type *)ptr1;
        // pkt_hdr->stream_id = stream_id; /* Will be filled out by the calling function, as per standard log allocations */
        pkt_hdr->status = DIAGBUF_UNCOMMIT_S;
        pkt_hdr->pad_len = pad_len2;
        pkt_hdr->length = (uint16)length;
        pkt_hdr->chain = DIAGBUF_SECOND_PKT_CHAIN;
        pkt_hdr->version = version;

        /* Set 'ptr' to the data following pkt_hdr */
        pkt_hdr += 1;
        return (pkt_hdr);
    }
    default:
      return NULL;
  }
}
/*===========================================================================

FUNCTION DIAGBUF_ALLOC_INTERNAL

DESCRIPTION
  This function allocates the specified amount of space in the diag
  output buffer.

DEPENDENCIES
  This function must only be called from within this file by externalized
  alloc functions.

  diagbuf_commit() must be called to commit a buffer to the diag pool.  This
  must happen reasonably soon after this call completes.  If the buffer is
  needed to accumulate data over time, then you should use your own buffer
  use this just long enough to copy the data.

============================================================================*/
static PACK(void *) diagbuf_alloc_internal (
   uint32 length,                 /* Length of requested buffer */
   int nice,                      /* Niceness padding value     */
   uint8 diagbuf_alloc_request,   /* Required for DIAG Health */
  uint8 chain, /*0: no chain; 1: 1st packet in the chain; 2nd packet in the chain*/
   uint8 version
)
{
  PACK(void *) ptr = NULL;  /* Pointer to return.        */
  uint32 buffer_used;          /* Length to allocate        */
  diagbuf_header_type *pkt_hdr;
  diagbuf_header_type *prev_pkt_hdr = NULL;


  /* Local indices to manipulate buffer */
  int iHead;
  int iHead_old;
  int iHeadIdx;
  int iTail;
  int iTailIdx;

  if (!diag_services_initialized)
    return NULL;
  if (length > DIAG_MAX_TX_PKT_SIZ) {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Attempt to alloc too much: %d", (dword) length);
    return NULL;
  }


  buffer_used =
             sizeof(diagbuf_header_type) +  /* Diagbuf header    */
                (uint32)length +  /* Number of bytes requested buffer */
                sizeof(uint16);       /* CRC                          */

  buffer_used = DIAGBUF_NEXT_ALIGNED_BYTE(buffer_used);

  if(buffer_used > DIAGBUF_SIZE )
    ERR_FATAL("diagbuf_alloc_internal: buffer_used (%d) > DIAGBUF_SIZE (%d)", buffer_used, DIAGBUF_SIZE, 0);

retry:
  prev_pkt_hdr = NULL;

  iHead_old = diagbuf_head;

  iTail = diagbuf_tail;
  iTailIdx = DIAGBUF_CALC_IDX(iTail);

  iHead = diagbuf_head;
  iHeadIdx = DIAGBUF_CALC_IDX(iHead);

  /* Check if valid state is read, if not re-read */
  if (iHead_old != iHead) {
    goto retry;
  }

  ASSERT (iHead < 2 * DIAGBUF_SIZE);
  ASSERT (iTail < 2 * DIAGBUF_SIZE);
  ASSERT (iHeadIdx < DIAGBUF_SIZE);
  ASSERT (iTailIdx < DIAGBUF_SIZE);

  /* Set a local pointer to the header */
  pkt_hdr = ((diagbuf_header_type *) &diagbuf_buf[iHeadIdx]);

  /* First check if the nice pad is available.  If OK, then alloc */
  if (DIAGBUF_BALANCE_PAD + nice < DIAGBUF_FREE(iHead, iTail)) {
    /* if not wrapped and no room here. */
    if (iTailIdx <= iHeadIdx          &&       /* Not wrapped &&  */
         iHead - iTail < DIAGBUF_SIZE &&       /* Not full &&     */
         /* No room here!   */
      (uint32)iHeadIdx + buffer_used >= DIAGBUF_SIZE) {



      prev_pkt_hdr = pkt_hdr;
      /* Mark header for wrap - do this after it is known that we
          have got a valid diagbuf_head */

      /* Wrap */
      DIAGBUF_WRAP(iHead);
      ASSERT ((iHead == 0) || (iHead == DIAGBUF_SIZE));

      /* Recalculate head index */
      iHeadIdx = DIAGBUF_CALC_IDX(iHead);
      ASSERT (iHeadIdx == 0);
    }

    if (buffer_used <= (unsigned int) DIAGBUF_FREE(iHead, iTail)) {
      /* Move head */
      DIAGBUF_MOVE(iHead, (int)buffer_used);
      ASSERT (iHead < 2 * DIAGBUF_SIZE);

      /* Update value of global head index */
      /* If atomic compare and set fails, implies some other thread wrote
         a new diagbuf_head before this could be written :
         this requires a retry */

      if (!osal_atomic_compare_and_set((osal_atomic_word_t *)&diagbuf_head, iHead_old, iHead)) {
        goto retry;
      }

      if (prev_pkt_hdr) {
        prev_pkt_hdr->status = DIAGBUF_WRAP_S;
      }

      /* Set header field of buffer */
      pkt_hdr = ((diagbuf_header_type *) &diagbuf_buf[iHeadIdx]);

      pkt_hdr->status = DIAGBUF_UNCOMMIT_S;
      pkt_hdr->pad_len = (byte)(buffer_used - (uint32)length - sizeof(diagbuf_header_type)
                                - sizeof(uint16));
      pkt_hdr->length = (uint16)length;
      pkt_hdr->chain = chain;
      pkt_hdr->version = version;

      /* Set 'ptr' to pkt_hdr +1 = data following pkt_hdr */
      ptr = pkt_hdr + 1;

      /* Logic to set diagbuf_head_read here */

    }
  } /* If requested length + nice pad is free */

  if (ptr) {
    /* Increment allocation requests counter */
    switch ( diagbuf_alloc_request ) {
    case DIAGBUF_ALLOC_REQUEST_LOG:
          if ( alloc_count_log == MAX_VALUE_UINT32 ) {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable alloc_count_log has reached maximum value.");
          } else {
        alloc_count_log++;
      }
      break;
    case DIAGBUF_ALLOC_REQUEST_MSG:
          if ( alloc_count_f3 == MAX_VALUE_UINT32 ) {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable alloc_count_f3 has reached maximum value.");
          } else {
        alloc_count_f3++;
      }
      break;
    default: /* DIAGBUF_ALLOC_REQUEST_OTHER */
      /* No need to update any variables */
      break;
    }
  } else {

      switch ( diagbuf_alloc_request ) {
    case DIAGBUF_ALLOC_REQUEST_LOG:
          if ( drop_count_log == MAX_VALUE_UINT32 ) {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable drop_count_log has reached maximum value.");
          } else {
        drop_count_log++;
      }
      break;
    case DIAGBUF_ALLOC_REQUEST_MSG:
          if ( drop_count_f3 == MAX_VALUE_UINT32 ) {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable drop_count_f3 has reached maximum value.");
          } else {
        drop_count_f3++;
      }
      break;
    default: /* DIAGBUF_ALLOC_REQUEST_OTHER */
      /* No need to update any variables */
      break;
    }
  }


  if (ptr) {
    /* Sanity check */

    byte stream_id = DIAG_STREAM_1;
    if (DIAGBUF_USED(iHead, iTail) > DIAGBUF_SIZE)
    {
       MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Possbile Ring buffer corrupt!");
    }
    diagbuf_write_overrun_check_pattern(pkt_hdr);
    diagbuf_write_stream_id(ptr, stream_id); /* write default stream ID for all packets allocated in diagbuf  */
  }

  return ptr;
} /* diagbuf_alloc_internal */


/*===========================================================================

FUNCTION DIAGBUF_ALLOC

DESCRIPTION
  This function allocates the specified amount of space in the diag output
  buffer.

DEPENDENCIES
  diagbuf_commit() must be called to commit a buffer to the diag pool.  This
  must happen reasonably soon after this call completes.  If the buffer is
  needed to accumulate data over time, then you should use your own buffer
  use this just long enough to copy the data.

============================================================================*/
PACK(void *) diagbuf_alloc (uint8 code, uint32 length, int nice, uint8 diagbuf_alloc_request, uint8 chain, uint8 version)
{
  PACK(void *) ptr = NULL; /* Pointer to return.        */

  //TODO: Move this function here.
   ptr = diagbuf_alloc_internal (length, nice, diagbuf_alloc_request, chain, version);

  if (ptr)
  {
    diagpkt_set_cmd_code (ptr, code);
  }

  return ptr;
} /* diagbuf_alloc */



/*===========================================================================

FUNCTION DIAGBUF_SHORTEN

DESCRIPTION
  This function reduces the length field of a previously allocated buffer.

  'ptr' must point to the same address that was returned by a prior call to
  diagpkt_alloc().

  Allocating too much and using this to shorten the packet is ideal for
  situations in which the length of the packet is not know prior to
  allocation.  Using this scheme does, however, consume resources that would
  otherwise be used to buffer outbound diagnostics data.  Please use this
  scheme sparingly.

============================================================================*/
/* mutex inclusion in this function needs to be reviewed again */
void
diagbuf_shorten (
  PACK(void *) ptr,       /* Pointer returned from diagbuf_alloc() */
   unsigned int new_length /* New length of buffer                  */
)
{
  diagbuf_header_type *pkt_hdr; /* Points to packet header */
  diagbuf_header_type *hole; /* This will point to new header field. */

  unsigned int curr_length;
  unsigned int hole_length;
  byte stream_id;
  uint32 temp_length = 0;

  if (!diag_services_initialized)
    return;
  /* Sanity check.  Make sure it is within boundaries of ring buffer */
  if ((byte *) ptr <  diagbuf_buf ||
      (byte *) ptr >= (&diagbuf_buf[DIAGBUF_SIZE] - sizeof(uint16))) {
    /* This is bad.  Incorrect usage. Report error and do nothing.*/
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Invalid packet address: ptr:0x%x, l:%d", ptr, new_length);
    return;
  }

  osal_lock_mutex(&diagbuf_buf_cs);
  stream_id = diagbuf_read_stream_id(ptr);
  /* Cast pointer to do arithmetic with diagbuf_header_type */
  pkt_hdr = (diagbuf_header_type *) ptr;

  /* Subtract 1 diagbuf_header_type (4 bytes) to get header */
  pkt_hdr -= 1;

  curr_length = pkt_hdr->length;

  if (new_length == 0) {
    /* Special case: shorten to 0.
     * The entire packet is just changed to be a hole
     */
    hole = pkt_hdr;

    /* length of the hole is current length + CRC + pad + streamID */
    temp_length = DIAGBUF_NEXT_ALIGNED_BYTE (curr_length +
                                            sizeof(uint16));

    if( temp_length > DIAGBUF_SIZE )
      ERR_FATAL("diagbuf_shorten: temp_length (%d) > DIAGBUF_SIZE (%d)", temp_length, DIAGBUF_SIZE, 0);

    /* Ensure temp_length can be stored in a uint16 */
    if( temp_length > 0xFFFF )
      ERR_FATAL("diagbuf_shorten: temp_length (%d) > 0xFFFF", temp_length, 0, 0);

    hole->length = (uint16)temp_length;
    if( (pkt_hdr->chain == DIAGBUF_SECOND_PKT_CHAIN)
        && (pkt_hdr->version >= DIAGBUF_VERSION_1)
        && (pkt_hdr->version <= DIAGBUF_MAX_VERSION_NUM))
    {
      /*Mark the first pkt in diagbuf as a hole*/
      pkt_hdr = ((diagbuf_header_type *)pkt_hdr) - diagbuf_ver_len[pkt_hdr->version -1];
      pkt_hdr = ((diagbuf_header_type *)pkt_hdr) - 1;
      pkt_hdr->status = DIAGBUF_HOLE_S;
    }
    hole->pad_len = 0;
    hole->status = DIAGBUF_HOLE_S;

    /* Since this essentially commits the packet as a hole, notify DIAG
       that the caller is ready to TX. */
    diag_tx_notify ();

  } else {

    /* If new length is longer or the same, we do nothing. */
    if (curr_length > new_length) {
      /* Compute hole size and record new packet length */

      pkt_hdr->length = (uint16)new_length;

      /* Adjust length fields to include CRC fields and streamID */
      new_length += sizeof (uint16);
      curr_length += sizeof (uint16);

      /* Calcualte pad for the packet */
      pkt_hdr->pad_len = (byte)(DIAGBUF_NEXT_ALIGNED_BYTE (new_length) - new_length);

      /* Now change length fields to be the length of entire buffer used */
      new_length += pkt_hdr->pad_len;
      curr_length = DIAGBUF_NEXT_ALIGNED_BYTE (curr_length);

      ASSERT (curr_length < DIAGBUF_SIZE);

      hole = (diagbuf_header_type *) ptr;

      /* Move to next aligned address past the new packet length */
      hole += new_length / sizeof (*hole);

      hole_length = curr_length - new_length;

      if (hole_length >= sizeof (diagbuf_header_type)) {
        hole->status = DIAGBUF_HOLE_S;
        hole->length = (uint16)hole_length - sizeof (diagbuf_header_type);
        hole->pad_len = 0;
      }

      diagbuf_write_overrun_check_pattern (pkt_hdr);
      diagbuf_write_stream_id(ptr, stream_id);

    } else if (new_length > curr_length) {
        MSG_2(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Attempted shorten %d to %d ignored.",
            curr_length, new_length);
    } else {
      /* If shortening to same size, do nothing. */
    }
  }

  osal_unlock_mutex(&diagbuf_buf_cs);

} /* diagbuf_shorten */


/*===========================================================================

FUNCTION DIAGBUF_CHECK_OVERRUN_PATTERN

DESCRIPTION
  Check for overrun pattern in the buffer provided.
============================================================================*/
boolean diagbuf_check_overrun_pattern (diagbuf_header_type *hdr)
{
  uint8 *pattern = NULL;
  boolean success = TRUE;

  /* The 2 byte CRC field at the end of the allocated buffer is written
   * with a mask to detect buffer overrun.  Check to make sure that value
   * wasn't corrupted.
   */
  pattern = (((byte *) hdr) + hdr->length +
             sizeof(diagbuf_header_type));

  if ((*pattern != 0xDE) || ((*(pattern+1) != 0xAD)))
  {
    diag_log_rsp_type *logpkt;
    char culprit[40];

    memset(culprit,0,40);
    success = FALSE;

    /* This detects a memory scribble.  There is no way to know how far
       past the allocated buffer the caller scribbled.  If this is
       determined to be a log, print the log code in lieu of the file
       name on the LCD.  Next, disable interrupts to avoid user of
       corrupt memory.  Reset diagbuf_head/tail so panic mode won't
       attempt to flush it, then enter panic mode as usual. */

    /* Disable all interrupts to avoid any more diagnostics client
       activitiy.  */
    osal_disable_interrupts();

    /* Reset diagbuf to avoid processing scribbled memory. */
    diagbuf_tail = diagbuf_head;

/*lint -save -e{740} Unusual pointer cast*/
    logpkt = (diag_log_rsp_type *) hdr + 1;
/*lint -restore */

    switch (diagpkt_get_cmd_code (logpkt))
    {
    case DIAG_LOG_F:
      /* Display that a log is the culprit in lieu of filename. */
        ERR_FATAL ("Log %04X : pri %d",log_get_code (logpkt->log),osal_thread_get_pri(),0);
      break;

    default:
        ERR_FATAL ("error fatal in diagbuf_check_overrun_pattern",0,0,0);
      break;
    }
    osal_enable_interrupts();
  }
  return success;
}

/* For now, this feature is inert.  tx_sleep_time = 0 means no sleep will
occur.  It will remain in this state until proper values are established.
However, the implemenatation is tested, allowing testers to poke values to
avoid watchdog timeouts. */

uint64 diagbuf_tx_sleep_threshold = DIAG_TX_SLEEP_THRESHOLD_DEFAULT;
osal_timer_cnt_t diagbuf_tx_sleep_time = DIAG_TX_SLEEP_TIME_DEFAULT;

int diagbuf_commit_threshold = DIAGBUF_COMMIT_THRESHOLD;

static int diagbuf_commit_size = 0;
/*===========================================================================

FUNCTION DIAGBUF_COMMIT

DESCRIPTION
  This function commits previously allocated space in the diag output buffer.

  'ptr' must be the address that was returned by a prior call to
  diagbuf_alloc().

  This function signals the diag task and may cause a context switch.

============================================================================*/
void diagbuf_commit (
  PACK(void *) ptr /* Buffer to commit */
)
{
  diagbuf_header_type *pkt_hdr; /* Pointer to the header of the buffer. */

  /* Cast pointer to do arithmetic with diagbuf_header_type */
  pkt_hdr = (diagbuf_header_type *) ptr;

  /* Subtract 1 diagbuf_header_type (4 bytes) to get header field */
  pkt_hdr -= 1;

  /* Sanity checks */
  if (ptr == NULL                                ||
      (byte*) ptr < &diagbuf_buf[sizeof(uint16)] ||
      (byte*) ptr >= &diagbuf_buf[DIAGBUF_SIZE]
     ) {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Invalid commit address 0x%x.", (dword) ptr);
    return;
  }

  if (pkt_hdr->status != DIAGBUF_HOLE_S)
  {
    if (diagbuf_check_overrun_pattern (pkt_hdr))
    {
      /* Set status to "committed" */
      pkt_hdr->status = DIAGBUF_COMMIT_S;
    }
  }

  diagbuf_commit_size += pkt_hdr->length;
  
  /*Set signal to drain if commit threshold is reached or if free space in diag buffer is less than max pkt size to prevent next large packet 
  not fitting even though commit threshold is not reached*/
  if ((diagbuf_commit_size >= diagbuf_commit_threshold) || (DIAGBUF_FREE(diagbuf_head,diagbuf_tail) < DIAG_MAX_TX_PKT_SIZ))
  {
    diagbuf_commit_size = 0;
	/* Clear the drain timer to avoid force flush */
    diag_clr_drain_timer();
    diag_tx_notify();
  }
  else
  {
    diag_set_drain_timer();
  }


} /* diagbuf_commit */


/* Private functions */


/*===========================================================================

FUNCTION DIAGBUF_WRITE_OVERRUN_CHECK_PATTERN

DESCRIPTION
  Writes the overrun check pattern in the CRC field at the end of the bufer
  pointed to by 'ptr'.  diagbuf_commit() checks this field to determine if
  buffer overrun has occured.

  The work to check this is done in diagbuf_commit() rather than a seperate
  function to save the cost of calculations and the function call.

  'ptr' points to the length field before the packet, not the beginning of
  the packet itself.

===========================================================================*/
static void diagbuf_write_overrun_check_pattern (void *ptr)
{
  unsigned int length;

  if (ptr)
  {
    length = ((diagbuf_header_type *) ptr)->length;

    /* The reserved space for the CRC is be used as a sanity check.
     * A pattern is written to those bytes in an effort to detect
     * buffer overrun.
     * The Head index is pointing to the length field.  We want to write
     * the CRC, which is 'length - sizeof(crc)' bytes ahead.
     */
    ptr = ((byte *) ptr) + length + sizeof(diagbuf_header_type);
    *((byte *) ptr) = 0xDE;
    *((byte *)ptr+1) = 0xAD;
  }
} /* diagbuf_write_overrun_check_pattern */

/*===========================================================================
FUNCTION diagbuf_write_stream_id

DESCRIPTION
  Writes the stream ID into diagbuf.
  Packet looks like this:
  diagbuf_header(4bytes) diag_packet(variable) streamID(1byte) over-run_check_pattern(2 bytes, 0xDEAD)
===========================================================================*/
void diagbuf_write_stream_id (PACK(void *)ptr, byte stream_id)
{
  if (ptr)
  {
     ((diagbuf_header_type *)((diagbuf_header_type *) ptr - 1))->stream_id = stream_id;
  }
}

/*===========================================================================
FUNCTION diagbuf_read_stream_id

DESCRIPTION
  Reads the stream ID for the diag packet.
  Packet looks like this:
  diagbuf_header(4bytes) diag_packet(variable) streamID(1byte) over-run_check_pattern(2 bytes, 0xDEAD)
===========================================================================*/
byte diagbuf_read_stream_id (const PACK(void *)ptr)
{
  byte stream_id = 0;

  if (ptr)
  {
     stream_id = ((diagbuf_header_type *)((diagbuf_header_type *) ptr - 1))->stream_id;
  }
  return stream_id;
}

/*===========================================================================
FUNCTION diagbuf_set_bit_outbound_pkt_handler

DESCRIPTION
  Set the most significant bit in the stream id to indicate that we should not
  call the fp associated with F3/log in diagbuf_drain()
===========================================================================*/
void diagbuf_set_bit_outbound_pkt_handler( void *ptr ) //MPD ToDo: check if these functions conflict with STM or entended APIs
{

  if ( ptr )
  {
    ptr = ((diagbuf_header_type *)((diagbuf_header_type *)ptr - 1));
    ((diagbuf_header_type *)ptr)->stream_id = ((diagbuf_header_type *)ptr)->stream_id | DIAGBUF_OUTBOUND_PKT_HANDLER_BIT_MASK;
  }
} /* diagbuf_set_bit_outbound_pkt_handler */

/*===========================================================================
FUNCTION diagbuf_clear_bit_outbound_pkt_handler

DESCRIPTION
  Clear the most significant bit in the stream id to indicate that we should
  call the fp associated with F3/log in diagbuf_drain()
===========================================================================*/
void diagbuf_clear_bit_outbound_pkt_handler( void *ptr )
{
  if ( ptr )
  {
    ptr = ((diagbuf_header_type *)((diagbuf_header_type *)ptr - 1));
    ((diagbuf_header_type *)ptr)->stream_id = ((diagbuf_header_type *)ptr)->stream_id & (~(DIAGBUF_OUTBOUND_PKT_HANDLER_BIT_MASK));
  }
} /* diagbuf_clear_bit_outbound_pkt_handler */

/*===========================================================================
FUNCTION diagbuf_clear_index

DESCRIPTION
  Clears the index of used bytes in the send buffer for command and data channel.
  For peripheral buffering and data channel on port1, the tail and head pointers are updated.
===========================================================================*/
void diagbuf_clear_index (diagcomm_io_conn_type * conn, diagcomm_io_tx_params_type * tx_param )
{
  uint8 pad = 0;
  diag_send_buf_header_type * buf_header = NULL;
  diagcomm_enum_port_type port_num = conn->port_num;
  diagcomm_io_channel_type  ch_type = conn->channel_type;

#ifndef DIAG_MP
  uint32 dsm_pkt_length = 0;
#endif

  if (ch_type == DIAGCOMM_IO_CMD)
  {
    diag_send_buf_cmd_index[port_num]  = 0;
    memset(diagbuf_send_cmd[port_num] ,0,DIAG_SEND_BUF_SIZE);
  }
  else if (ch_type == DIAGCOMM_IO_DATA)
  {
    if(port_num == DIAGCOMM_PORT_1)
    {
#ifndef DIAG_MP
    if(g_is_cmd_rsp_type)
    {
      diag_send_buf_cmd_index[port_num]  = 0;
      memset(diagbuf_send_cmd[port_num] ,0,DIAG_SEND_BUF_SIZE);
    }
    else
    {
       /* dsm_ptr->used  */
      /*Check to see if we have sent data less than used bytes in the buffer*/
     dsm_pkt_length = dsm_length_packet(tx_param->dsm_ptr);
      ASSERT(diag_tx_mode[port_num].buf_used >= dsm_pkt_length);
#else

      /*Check to see if we have sent data less than used bytes in the buffer*/
      ASSERT(diag_tx_mode[port_num].buf_used >= tx_param->params.len);
#endif

      //Reset pattern and length to avoid errors when reading this data again.
      buf_header = (diag_send_buf_header_type *)(diag_tx_mode[port_num].buf_ptr + diag_tx_mode[port_num].buf_tail);
      buf_header->pattern = 0x00;
      buf_header->length = 0x0;

      /*Move the tail pointer by the number of bytes that we sent*/
#ifndef DIAG_MP
      diag_tx_mode[port_num].buf_tail += dsm_pkt_length + sizeof(diag_send_buf_header_type);
#else
      diag_tx_mode[port_num].buf_tail += tx_param->params.len + sizeof(diag_send_buf_header_type);
#endif

      if(diag_tx_mode[port_num].buf_tail == diag_tx_mode[port_num].buf_head)
      {
        diag_tx_mode[port_num].buf_head = sizeof(diag_send_buf_header_type);
        diag_tx_mode[port_num].buf_tail = 0;
        buf_header = (diag_send_buf_header_type *)(diag_tx_mode[port_num].buf_ptr + diag_tx_mode[port_num].buf_tail);
        buf_header->pattern = 0xDEAD;
        buf_header->length = 0;
    diag_tx_mode[port_num].curr_data_block_header = buf_header;
        diag_tx_mode[port_num].buf_used = sizeof(diag_send_buf_header_type);
        diag_tx_mode[port_num].curr_threshold=diag_max_send_buf_len;

      }
      else
      {
        pad = DIAG_WORD_ALIGN(diag_tx_mode[port_num].buf_tail) - diag_tx_mode[port_num].buf_tail ;
        diag_tx_mode[port_num].buf_tail += pad;

#ifndef DIAG_MP
        diag_tx_mode[port_num].buf_used -= (pad + dsm_pkt_length + sizeof(diag_send_buf_header_type));
#else
        diag_tx_mode[port_num].buf_used -= (pad + tx_param->params.len + sizeof(diag_send_buf_header_type));
#endif

        buf_header = (diag_send_buf_header_type *)(diag_tx_mode[port_num].buf_ptr + diag_tx_mode[port_num].buf_tail);

        /*Check if the new tail is pointing to a data block or not; If not, then move the tail to curr data block*/
        if((buf_header->pattern != 0xDEAD) || ((buf_header->length + diag_tx_mode[port_num].buf_tail) > diag_tx_mode[port_num].buf_size))
        {
          /*Reached end of buffer*/
           diag_tx_mode[port_num].buf_tail = 0;
           diag_tx_mode[port_num].buf_used = diag_tx_mode[port_num].buf_head;
           buf_header = (diag_send_buf_header_type *)(diag_tx_mode[port_num].buf_ptr + diag_tx_mode[port_num].buf_tail);

          if((buf_header->pattern != 0xDEAD) || (buf_header->length == 0))
          {
            diag_tx_mode[port_num].buf_tail = (((uint8 *)diag_tx_mode[port_num].curr_data_block_header) - diag_tx_mode[port_num].buf_ptr);
            diag_tx_mode[port_num].buf_used = diag_tx_mode[port_num].curr_data_block_header->length + sizeof(diag_send_buf_header_type);
          }
        }
      }

      if(diag_tx_mode[port_num].buf_tail > diag_tx_mode[port_num].buf_size)
      {
        diag_tx_mode[port_num].buf_tail = 0;
      }

#ifndef DIAG_MP
    }
#endif
     }
#ifdef DIAG_CONSUMER_API
     else if(port_num == DIAGCOMM_PORT_2)
     {
        diag_send_buf_data_index[port_num]  = 0;
     }
#endif /* DIAG_CONSUMER_API */
    }
}

void diag_get_sendbuf_params(diag_send_buf_type *send_buf, boolean is_cmd_rsp)
{
  if(is_cmd_rsp)
  {
    send_buf->ptr = &diagbuf_send_cmd[DIAGCOMM_PORT_1][0];
    send_buf->head = diag_send_buf_cmd_index[DIAGCOMM_PORT_1];
    send_buf->tail = 0;
    send_buf->size = DIAG_SEND_BUF_SIZE;
    send_buf->threshold = DIAG_SEND_BUF_SIZE;
    send_buf->wrap = 0;
    send_buf->used = send_buf->head;
    send_buf->curr_data_block_header = NULL;
  }
  else
  {
    /*For data use the parameters from diag_tx_mode*/
    send_buf->ptr = diag_tx_mode[DIAGCOMM_PORT_1].buf_ptr;
    send_buf->size = diag_tx_mode[DIAGCOMM_PORT_1].buf_size;
    send_buf->wrap = diag_tx_mode[DIAGCOMM_PORT_1].buf_wrap;

    send_buf->threshold = diag_tx_mode[DIAGCOMM_PORT_1].curr_threshold;

      send_buf->curr_data_block_header = diag_tx_mode[DIAGCOMM_PORT_1].curr_data_block_header;

    send_buf->used = diag_tx_mode[DIAGCOMM_PORT_1].buf_used;
    send_buf->tail = diag_tx_mode[DIAGCOMM_PORT_1].buf_tail;
    send_buf->head = diag_tx_mode[DIAGCOMM_PORT_1].buf_head;
      }
}

void diag_update_sendbuf_params(diag_send_buf_type *send_buf, boolean is_cmd_rsp)
{
  if(is_cmd_rsp)
  {
  /*Update the index for command flat buffer*/
    diag_send_buf_cmd_index[DIAGCOMM_PORT_1] = send_buf->head;
  }
  else
  {
  if((send_buf->used > send_buf->size) || (send_buf->head > send_buf->size))
  {
    ASSERT(0);
  }
    /*Update the head, tail and wrap flags for data channel*/
    diag_tx_mode[DIAGCOMM_PORT_1].buf_head = send_buf->head;
    diag_tx_mode[DIAGCOMM_PORT_1].buf_tail = send_buf->tail;
    diag_tx_mode[DIAGCOMM_PORT_1].buf_wrap = send_buf->wrap;
    diag_tx_mode[DIAGCOMM_PORT_1].buf_used = send_buf->used;
    diag_tx_mode[DIAGCOMM_PORT_1].curr_data_block_header = send_buf->curr_data_block_header;
  }
}

uint8 diag_get_padlen( diag_send_desc_type *desc )
{
   uint8 pad_len = 0;

    if ((DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].feature_mask, F_DIAG_HDLC_ENCODE_IN_APPS))\
          && (DIAG_INT_FEATURE_MASK_CHECK(F_DIAG_HDLC_ENCODE_IN_APPS)))
    {
       /*If peripheral is not HDLC encoding the data, we have to account for 4 bytes of header and 1 byte of terminating character
       pad_len is 5 bytes for first part of chained packet and 1 byte for the next part */
       pad_len = (desc->state == DIAG_SEND_STATE_START)? (DIAG_NHDLC_PAD):(DIAG_NHDLC_TERMINATE_PAD);
    }
    else
    {
      if (desc->terminate)
      {
         /* If terminal packet, allow for minimal padding. */
         pad_len = MIN(((int)desc->last - (int)desc->pkt) + DIAG_HDLC_CRC_PAD, diagbuf_hdlc_pad_len);
      }
      else
      {
        /* If non-terminal packet, add more padding to account for the rest of the packet needing to fit in
        same DSM chain. */
        pad_len = diagbuf_hdlc_pad_len;
      }
    }
    return pad_len;
}

uint32 diag_get_send_size(diagcomm_enum_port_type port_type)
{
  diag_send_buf_header_type *diag_send_buf_header = (diag_send_buf_header_type *)(diag_tx_mode[port_type].buf_ptr + diag_tx_mode[port_type].buf_tail);

  if(diag_send_buf_header->pattern != DIAGBUF_HEADER_PATTERN)
  {
    return 0;
  }

  return diag_send_buf_header->length;
}

/*===========================================================================
FUNCTION DIAG_ADD_BLOCK_HEADER

DESCRIPTION
  This functions adds a new block in the send buffer and
  currently invoked only in buffering modes.

DEPENDENCIES

===========================================================================*/
void diag_add_block_header(diag_send_buf_type* send_buf, uint16 length)
{
  int pad = 0;
  uint8* ptr = NULL;
  uint32 new_head=0;
  uint32 free_size=0;
  diag_send_buf_header_type *tail_data_block_header=NULL;
  diag_send_buf_header_type *head_data_block_header=NULL;
  uint8 stream_index=0;

    stream_index = DIAG_ID_TO_INDEX(DIAG_STREAM_1);

    /* Make the Head 4 byte aligned before making any calculations */
     pad = (uint32)DIAG_WORD_ALIGN((uint32)send_buf->head) - (uint32)send_buf->head;
     send_buf->head += pad;

    if( (send_buf->head + length + sizeof(diag_send_buf_header_type)) >= send_buf->size )
    {
        /*Head is at the end of buffer and incoming log cannot fit here, Wrap here*/
        new_head=0;

        if(send_buf->head <= send_buf->tail)
        {
            tail_data_block_header = (diag_send_buf_header_type *)(send_buf->ptr + send_buf->tail);
            tail_data_block_header->pattern=0;
            tail_data_block_header->length=0;
            send_buf->tail=0;

        }
        /* Clear pattern and length only if head is less than size to avoid overwriting out of buffer memory */
        if(send_buf->head < send_buf->size)
        {
          head_data_block_header=(diag_send_buf_header_type *)(send_buf->ptr + send_buf->head );
          head_data_block_header->pattern=0;
          head_data_block_header->length=0;
        }
    }
    else
    {
        new_head = send_buf->head;
    }

    /*Calculate free size available for new block being created
    If head is greater than tail, maximum available is till the end of buffer
    or if tail is greater than head, then maximum available will be up to tail
    index, in such case we move tail such that we have enough pad which is
    maximum TX size*/

    if(new_head >send_buf->tail)
    {
        free_size = send_buf->size - new_head ;
    }
    else if (new_head < send_buf->tail)
    {
        free_size = send_buf->tail - new_head;
    }
    else
    {
        if( diag_tx_mode[DIAGCOMM_PORT_1].buf_used > sizeof(diag_send_buf_header_type))
        {
            free_size=0;
        }
        else
        {
            free_size = send_buf->size - new_head;
        }
    }
    /* Consider size of block header while calculating free_size */
    free_size -= sizeof(diag_send_buf_header_type);
    /* Move the tail till we have the free size available which is equal to
     maximum block pad size. */
    while(free_size < length)
    {
        /*Move Tail */
        tail_data_block_header = (diag_send_buf_header_type *)(send_buf->ptr + send_buf->tail);

        if((send_buf->tail >send_buf->size) || (tail_data_block_header->pattern != 0xDEAD) || (tail_data_block_header->length > diag_max_send_buf_len))
        {
            tail_data_block_header->pattern=0;
            tail_data_block_header->length=0;
            send_buf->tail=0;
            free_size =0;
            tail_data_block_header = (diag_send_buf_header_type *)(send_buf->ptr + send_buf->tail);
        }

        send_buf->tail+= tail_data_block_header->length + sizeof(diag_send_buf_header_type);

        pad = (uint32)DIAG_WORD_ALIGN((uint32)send_buf->tail) - (uint32)send_buf->tail;
        send_buf->tail+=pad;
        tail_data_block_header->pattern=0;
        tail_data_block_header->length=0;

        tail_data_block_header = (diag_send_buf_header_type *)(send_buf->ptr + send_buf->tail);

        if((send_buf->tail >send_buf->size) || (tail_data_block_header->pattern != 0xDEAD) || (tail_data_block_header->length > diag_max_send_buf_len))
        {
            tail_data_block_header = (diag_send_buf_header_type *)(send_buf->ptr + send_buf->tail);
            tail_data_block_header->pattern=0;
            tail_data_block_header->length=0;
            send_buf->tail=0;
            free_size =0;
        }

        /* Tail moved so check for free size again*/
        if((new_head + sizeof(diag_send_buf_header_type)) >send_buf->tail)
        {
            free_size = send_buf->size - new_head;
        }
        else
        {
            free_size = send_buf->tail - new_head;
        }
       /* Consider size of block header while calculating free_size */
       free_size -= sizeof(diag_send_buf_header_type);
    }   
    /*Threshold indicates the maximum index that head can write up to
    and this threshold is used in diagbuf_send_pkt_single_proc() to add a new block */
    send_buf->threshold= MIN(free_size, diag_max_send_buf_len);

    send_buf->curr_data_block_header = (diag_send_buf_header_type *)(send_buf->ptr + new_head);
  pad = (uint32)DIAG_WORD_ALIGN((uint32)send_buf->curr_data_block_header) - (uint32)send_buf->curr_data_block_header;
  ptr = (uint8 *)send_buf->curr_data_block_header;
  ptr += pad;
  send_buf->curr_data_block_header = (diag_send_buf_header_type *)ptr;
  diag_tx_mode[DIAGCOMM_PORT_1].curr_data_block_header = send_buf->curr_data_block_header;
  diag_tx_mode[DIAGCOMM_PORT_1].curr_data_block_header->pattern = 0xDEAD;
  diag_tx_mode[DIAGCOMM_PORT_1].curr_data_block_header->length = 0;
    send_buf->head = pad + (uint32)(new_head) + sizeof(diag_send_buf_header_type);

    /*Update global parameters*/
    diag_tx_mode[DIAGCOMM_PORT_1].buf_head = send_buf->head;
    diag_tx_mode[DIAGCOMM_PORT_1].buf_tail = send_buf->tail;
    diag_tx_mode[DIAGCOMM_PORT_1].buf_wrap = send_buf->wrap;
    diag_tx_mode[DIAGCOMM_PORT_1].buf_used = diag_buffered_bytes(stream_index);
    diag_tx_mode[DIAGCOMM_PORT_1].curr_threshold = send_buf->threshold;

    send_buf->used= diag_tx_mode[DIAGCOMM_PORT_1].buf_used;

    if((send_buf->used > send_buf->size) || (send_buf->head > send_buf->size))
   {
        ULogFront_RealTimePrintf(diag_debug_ulog_handle,3,"send_buf->head : %d send_buf->size : %d send_buf->used : %d",send_buf->head,send_buf->size,send_buf->used);
        ASSERT(0);
   }
}
/*===========================================================================
FUNCTION DIAGBUF_SEND_PKT_SINGLE_PROC

DESCRIPTION
  This function sends a packet, or a fragment of a packet, through the comm
  layer.  It handles HDLC encoding as well as flow control.

  If 'pkt' is NULL, the buffer in progress is flushed to the comm layer,
  making no assumptions about HDLC, packet termination, etc.

DEPENDENCIES
  This routine maintains state due to flow control requirements.
  The caller is responsible for maintaining integrity of the data stream.

===========================================================================*/
diagbuf_status_enum_type diagbuf_send_pkt_single_proc (diag_send_desc_type *desc)
{
   diagbuf_status_enum_type status = DIAGBUF_READY_S;
   static diag_hdlc_dest_type enc = { NULL, NULL, 0 };
   uint8 pad_len = 0;
   uint16 send_size = 0;
   boolean pre_send = FALSE;
   diag_send_buf_type send_buf;
   uint32 pkt_len = 0;
   uint32 curr_block_len = 0;   

   if (!(diagcomm_status(DIAGCOMM_PORT_1) && DIAG_CHECK_MASK(diag_cx_state, DIAG_CX_COMM_S)
#if defined (DIAG_MP) && !defined(DIAG_MP_MASTER)
         && (diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].feature_mask[0] > 0)
#endif
        ))
   {

      /* DIAG is not connected to the comm layer.  Listeners may still be
         active, though.  Throw away any buffer that may be lingering. */
      if (diag_tx_mode[DIAGCOMM_PORT_1].buf_used > sizeof(diag_send_buf_header_type)) {
         diag_tx_mode[DIAGCOMM_PORT_1].buf_used = sizeof(diag_send_buf_header_type);
      }

      /* Now discard inbound data by adjusting the descriptor to indicate
         that all data was sent.  Return status that we are ready for more.  */
      if (desc) {
         uint8 *ptr = (uint8 *)desc->last;
         ptr++;
         desc->pkt = (void *)ptr;
         desc->state = DIAG_SEND_STATE_COMPLETE;
         status = DIAGBUF_READY_S;
      }
      /* Commit local copies of persistent data back to static variables and return */
      return status;
   }

/*Logic to check if we have to flush bufferred data*/

   if (desc == NULL) {
      /* Flush the buffer to the comm layer. */
      if (diag_tx_mode[DIAGCOMM_PORT_1].buf_used > sizeof(diag_send_buf_header_type)) {
         diag_get_sendbuf_params(&send_buf, FALSE);
         pre_send = TRUE;
      }

      if (diag_tx_mode[DIAGCOMM_PORT_1].mode == DIAG_TX_MODE_BUFFERED_CIR) {
         pre_send = FALSE;
      }

   } else {
      if (status != DIAGBUF_READY_S && desc->state >= DIAG_SEND_STATE_COMPLETE) {
         return status;
      }

      /*Get the send buffer parameters*/
      diag_get_sendbuf_params(&send_buf, desc->is_cmd_rsp_type);
      if (!desc->is_cmd_rsp_type) {
         /*Calculate the pad length required*/
         pad_len = diag_get_padlen(desc);

         /* calculate the length of the packet */
         pkt_len = MAX((((int)desc->last - (int)desc->pkt + 1) + pad_len), (diag_pkt_len + pad_len));
         curr_block_len = send_buf.curr_data_block_header->length + sizeof(diag_send_buf_header_type);
         /*Process incoming desc*/
        if (((curr_block_len + pkt_len) >= send_buf.threshold) && (desc->state == DIAG_SEND_STATE_START))
         {
            pre_send = TRUE;


            if ( diag_tx_mode[DIAGCOMM_PORT_1].mode == DIAG_TX_MODE_BUFFERED_CIR)
            {
               pre_send = FALSE;
            }
         }
      }
   }
   /*Flush the buffered data before processing the incoming data*/
   if (pre_send == TRUE) {
      send_size = diag_get_send_size(DIAGCOMM_PORT_1);
      if (0 == send_size || send_size > diag_max_send_buf_len) {
         MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "invalid send_size %d", send_size);
         if(send_size > diag_max_send_buf_len)
         {
            ASSERT(0);
         }

      } else {
         diagcomm_send((void *)(send_buf.ptr + send_buf.tail + sizeof(diag_send_buf_header_type)), send_size, FALSE, DIAGCOMM_PORT_1);
         diag_get_sendbuf_params(&send_buf, FALSE);	
         /* Calculate the length of current block including block header */
         curr_block_len = send_buf.curr_data_block_header->length + sizeof(diag_send_buf_header_type);
      }
   }

   /*Drop the incoming data if we get flow controlled during the previous send
   or if desc is NULL*/
   if (desc == NULL) return status;

   /* if not command response and channel is flow controlled, drop data and return*/
   if ((!desc->is_cmd_rsp_type) && (FALSE == diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].allow_flow)) {
     status = DIAGBUF_NOT_READY_S;
      return status;
   }

   if (!desc->is_cmd_rsp_type)
   {
      if (((curr_block_len + pkt_len) >= send_buf.threshold) && (desc->state == DIAG_SEND_STATE_START))
      {
       /* Add a header here to mark the start of a data block;
          The flat buffer is divided into data blocks
          header pattern is: |0xDEAD<2 bytes> | length <2 bytes>| */
         diag_add_block_header(&send_buf, pkt_len);
		 
       /*Update the local copy of buffer parameters*/
       diag_get_sendbuf_params(&send_buf, desc->is_cmd_rsp_type);

      }
   }

   enc.dest = (void *)((uint32)send_buf.ptr + send_buf.head);

   if (send_buf.head >= send_buf.tail) {
      enc.dest_last = (void *)(send_buf.ptr + send_buf.size - 1);
   } else {
      enc.dest_last = (void *)(send_buf.ptr + send_buf.tail - 1);
   }

   if ((DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].feature_mask, F_DIAG_HDLC_ENCODE_IN_APPS))\
          && (DIAG_INT_FEATURE_MASK_CHECK(F_DIAG_HDLC_ENCODE_IN_APPS))) {
      diag_non_hdlc_encode(desc, &enc);
   } else {
      diag_hdlc_encode(desc, &enc);
   }

   /*Update the used bytes*/
   send_buf.used += (uint32)enc.dest - (uint32)((uint32)send_buf.ptr + send_buf.head);

   if (!desc->is_cmd_rsp_type) {
      send_buf.curr_data_block_header->length += (uint32)enc.dest - (uint32)((uint32)send_buf.ptr + send_buf.head);
   }

   send_buf.head = (uint32)enc.dest - (uint32)send_buf.ptr;

   /*---------------------------------*/
   /* Check to see if we should send the data right away*/
   if (desc->is_cmd_rsp_type) {
      diag_send_buf_cmd_index[DIAGCOMM_PORT_1]=send_buf.used;
      diagcomm_send((void *)(send_buf.ptr + send_buf.tail), send_buf.used, TRUE, DIAGCOMM_PORT_1);
   }
   else
   {
      diag_update_sendbuf_params(&send_buf, desc->is_cmd_rsp_type);
   }

   return status;
}

/*===========================================================================
FUNCTION diag_send_buf_retry

DESCRIPTION
  This function is called when SMDL has signaled diag to resend the buffer if
  it was not able to send all the data successfully in first attempt
===========================================================================*/
void diag_send_buf_retry(diagcomm_io_channel_type ch_type, diagcomm_enum_port_type port_type)
{
   uint16 send_size = 0;

   if (ch_type == DIAGCOMM_IO_DATA) {
      if (port_type == DIAGCOMM_PORT_1) {
         send_size = diag_get_send_size(DIAGCOMM_PORT_1);
         if (0 == send_size || send_size > diag_max_send_buf_len) {
            MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "invalid send_size %d", send_size);
         } else {
            diagcomm_send((void *)(diag_tx_mode[port_type].buf_ptr + diag_tx_mode[port_type].buf_tail + sizeof(diag_send_buf_header_type)), send_size, FALSE, DIAGCOMM_PORT_1);
            if(diag_drain_pool_check(DIAG_STREAM_1))
            {
                diag_set_internal_sigs(DIAG_INT_DRAIN_DSM_BUFFER_SIG);
            }
         }
      } 
#ifdef DIAG_CONSUMER_API
  else if (port_type == DIAGCOMM_PORT_2) {
         if ((diag_send_buf_data_index[port_type] >= DIAG_SEND_BUF_SIZE_DATA_DCI)) {
            ASSERT(0);
         }
         diagcomm_send((void *)diagbuf_send_data_dci, diag_send_buf_data_index[port_type], FALSE, port_type);
      }
#endif /* DIAG_CONSUMER_API */
   } else if (ch_type == DIAGCOMM_IO_CMD) {
      if ((diag_send_buf_cmd_index[port_type] >= DIAG_SEND_BUF_SIZE)) {
         ASSERT(0);
      }
      diagcomm_send((void *)diagbuf_send_cmd[port_type], diag_send_buf_cmd_index[port_type], TRUE, port_type);
   } else if (ch_type == DIAGCOMM_IO_CTRL) {
      diagcomm_ctrl_send_buf_retry(port_type);
   }
}
/*===========================================================================

FUNCTION DIAGBUF_SEND_PKT_SINGLE_PROC_DCI

DESCRIPTION
  This function sends a packet, or a fragment of a packet over DCI channel.

  If 'pkt' is NULL, the buffer in progress is flushed to the comm layer,
  making no assumptions about HDLC, packet termination, etc.

DEPENDENCIES
  This routine maintains state due to flow control requirements.
  The caller is responsible for maintaining integrity of the data stream.

===========================================================================*/
#ifdef DIAG_CONSUMER_API
diagbuf_status_enum_type diagbuf_send_pkt_single_proc_dci (diag_send_desc_type *desc)
{
   diagbuf_status_enum_type status = DIAGBUF_READY_S;
   unsigned int outbound_used = 0;

   /* Persistent data */
   static uint8 *outbound_sav_dci = NULL; /* Pointer to memory allocated by the comm layer. */
   static diag_hdlc_dest_type enc_dci = { NULL, NULL, 0 };
   static boolean first_packet_dci = TRUE;
   static uint8 *item_ptr_dci = NULL;
   uint8 *diagbuf_send_ptr; /*Pointer to the current buffer (cmd or data) that is being sent */
   uint16 *diag_send_buf_index; /*Current buffer index*/
   uint8 pad_len = 0;
   uint32 pkt_len = 0;

   /* Local copies of static variables for optimized access.
      This makes this routine persistent but not re-entrant. */
   byte *outbound = outbound_sav_dci;

   if (!(diagcomm_status(DIAGCOMM_PORT_2) && DIAG_CHECK_MASK(diag_cx_state, DIAG_CX_DCI_S))) {
      /* DIAG is not connected to the comm layer.  Listeners may still be
         active, though.  Throw away any buffer that may be lingering. */
      if (outbound != NULL) {
         outbound = NULL;
      }
      /* Now discard inbound data by adjusting the descriptor to indicate
         that all data was sent.  Return status that we are ready for more.  */
      if (desc) {
         uint8 *ptr = (uint8 *)desc->last;
         ptr++;
         desc->pkt = (void *)ptr;
         desc->state = DIAG_SEND_STATE_COMPLETE;
         status = DIAGBUF_READY_S;
      }
   } else if (desc == NULL) {
      /* Flush the buffer to the comm layer. */
      if (outbound != NULL) {
         if (diag_send_buf_cmd_index[DIAGCOMM_PORT_2] > 0) {
            outbound_used = diag_send_buf_cmd_index[DIAGCOMM_PORT_2];
            ASSERT(outbound_used < DIAG_SEND_BUF_SIZE_DATA_DCI);
            if (TRUE == diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].allow_flow) {
               diagcomm_send((void *)diagbuf_send_cmd[DIAGCOMM_PORT_2], outbound_used, TRUE, DIAGCOMM_PORT_2);
            }
         }
         if (diag_send_buf_data_index[DIAGCOMM_PORT_2] > 0) {
            outbound_used = diag_send_buf_data_index[DIAGCOMM_PORT_2];
            ASSERT(outbound_used < DIAG_SEND_BUF_SIZE_DATA_DCI);

            if (diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].allow_flow == TRUE) {
               diagcomm_send((void *)diagbuf_send_data_dci, outbound_used, FALSE, DIAGCOMM_PORT_2);
            }
         }
         outbound = NULL;
         item_ptr_dci = NULL;
      }
   } else {
      if ((!desc->is_cmd_rsp_type) && (FALSE == diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].allow_flow)) {
         status = DIAGBUF_NOT_READY_S;
         return status;
      }

      while (status == DIAGBUF_READY_S && desc->state < DIAG_SEND_STATE_COMPLETE) {
         if (desc->is_cmd_rsp_type) {
            diagbuf_send_ptr = &diagbuf_send_cmd[DIAGCOMM_PORT_2][0];
            diag_send_buf_index = &diag_send_buf_cmd_index[DIAGCOMM_PORT_2];
         } else {
            diagbuf_send_ptr = &diagbuf_send_data_dci[0];
            diag_send_buf_index = &diag_send_buf_data_index[DIAGCOMM_PORT_2];
         }

         item_ptr_dci = (uint8 *)&diagbuf_send_ptr[*diag_send_buf_index];
         outbound = (uint8 *)&diagbuf_send_ptr[*diag_send_buf_index]; /*diagbuf_send_ptr index is the index of diagbuf_send_ptr pointing to the available index*/
         enc_dci.dest = (void *)outbound;
         enc_dci.dest_last = (void *)((byte *)outbound + (diag_send_buf_threshold - *diag_send_buf_index) - 1);
         outbound_used = *diag_send_buf_index;

         pad_len = diag_get_padlen(desc);
         
         /* calculate the length of the packet */
         pkt_len = (diag_pkt_len==0) ? (((int)desc->last - (int)desc->pkt + 1) + pad_len) : (diag_pkt_len + pad_len);

         /* The condition below checks if the packet is a terminal packet whose source size won't
         fit in output buffer size - if the conditon is true send the existing data
         right away and allocate a new buffer for this packet to avoid chaining */
         if (((pkt_len + outbound_used) >= diag_send_buf_threshold)
             && (TRUE == first_packet_dci) && (!desc->is_cmd_rsp_type)) {
            /* We are assuming that this check always passes, but,
            if it doesn't, any existing data will get discarded. */
            ASSERT(item_ptr_dci != NULL);

            if (item_ptr_dci) {
               if (desc->is_cmd_rsp_type) {
                  diagcomm_send((void *)diagbuf_send_ptr, *diag_send_buf_index, TRUE, DIAGCOMM_PORT_2);
                  diagbuf_send_ptr = &diagbuf_send_cmd[DIAGCOMM_PORT_2][0];
                  diag_send_buf_index = &diag_send_buf_cmd_index[DIAGCOMM_PORT_2];

                  if (diag_send_buf_cmd_index[DIAGCOMM_PORT_2] != 0) {
                     /*Sending previously accumulated data was not successful*/
                     status = DIAGBUF_NOT_READY_S;
                     return status;
                     /*Trying to accumulate more than the buffer size -- Drop it for now since we have no choice*/
                     //  ASSERT(0);
                  }
               } else {
                  diagcomm_send((void *)diagbuf_send_ptr, *diag_send_buf_index, FALSE, DIAGCOMM_PORT_2);
                  if (diag_send_buf_data_index[DIAGCOMM_PORT_2] != 0) {
                     //Sending previously accumulated data was not successful
                     status = DIAGBUF_NOT_READY_S;
                     return status;
                  } else {
                     diagbuf_send_ptr = &diagbuf_send_data_dci[0];
                     diag_send_buf_index = &diag_send_buf_data_index[DIAGCOMM_PORT_2];
                  }
               }
               item_ptr_dci = (uint8 *)&diagbuf_send_ptr[*diag_send_buf_index];
               outbound_used = 0;
            }

            outbound = NULL;

            if (item_ptr_dci) {
               outbound = item_ptr_dci;
               enc_dci.dest = (void *)outbound;
               /*IMP: if both the buffers are of different sizes, this has to change*/
               enc_dci.dest_last = (void *)((byte *)outbound + (diag_send_buf_threshold - *diag_send_buf_index) - 1);
            } else outbound = NULL;
         }

         if (outbound) {
            diag_non_hdlc_encode(desc, &enc_dci);
            *diag_send_buf_index += (uint32)enc_dci.dest - (uint32)outbound;
         }

         /* Check for case if exactly DSM SIZE or DSM_SIZE - 1 bytes are in dsm, then packet is full, needs to be sent right away */
         if (!desc->is_cmd_rsp_type) {
           /* check if buffer is almost full and can be sent */
           if (((((uint8 *)enc_dci.dest)) >= (&diagbuf_send_data_dci[0] + diag_send_buf_threshold)) && (desc->state == DIAG_SEND_STATE_COMPLETE)) {
               outbound = NULL;
            }
         }

         /* If it is a new chain and a terminal packet - send the data OR
         if it is a cmd_rsp_type or priority packet send the data */
         if (desc->is_cmd_rsp_type) {

            diagcomm_send((void *)diagbuf_send_ptr, *diag_send_buf_index, desc->is_cmd_rsp_type, DIAGCOMM_PORT_2);
            outbound = NULL;
         }

         if (desc->terminate) {
            first_packet_dci = TRUE;
         } else {
            first_packet_dci = FALSE;
         }
      }
   }

   /* Commit local copies of persistent data back to static variables. */
   outbound_sav_dci = outbound;

   return (status);
}           /* diagbuf_send_pkt_single_proc_dci */
#endif /* #ifdef DIAG_CONSUMER_API */
/*===========================================================================

FUNCTION DIAGBUF_SEND_PKT

DESCRIPTION

===========================================================================*/
diagbuf_status_enum_type diagbuf_send_pkt (diag_send_desc_type *desc, byte stream_id)
{
    diagbuf_status_enum_type status = DIAGBUF_READY_S;
  #ifdef DIAG_CONSUMER_API
    static diag_send_desc_type desc_dci = {NULL,NULL,0,1,0,0};
    static boolean diag_dci_pending = FALSE;
    diag_send_buf_type send_buf;
    uint32 pkt_len;
  #endif /* #ifdef DIAG_CONSUMER_API */
    void *send_last = NULL;

    if(!desc) {
        if(stream_id & DIAG_STREAM_1) {
            diagbuf_send_pkt_single_proc(NULL);
        if(FALSE == diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].allow_flow)
        {
          ULogFront_RealTimePrintf(diag_debug_ulog_handle, 4, "diagbuf_send_pkt: allow_flow = %d, channel_type = %d, io_type = %d, port_num = %d",\
              diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].allow_flow, diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].channel_type, \
              diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].io_type, diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].port_num);

          status = DIAGBUF_NOT_READY_S;
        }
      }
    #ifdef DIAG_CONSUMER_API
        if(stream_id & DIAG_STREAM_2) {
        diagbuf_send_pkt_single_proc_dci(NULL);
        if(FALSE == diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].allow_flow)
        {
          status = DIAGBUF_NOT_READY_S;
        }
      }
    #endif /* #ifdef DIAG_CONSUMER_API */
      return status;
    }

  #ifdef DIAG_CONSUMER_API
    /* Lost track of old packet, resetting diag_dci_pending flag to FALSE so that packet will be sent to all applicable streams */
    if(diag_dci_pending && desc->last != send_last)
    {
      diag_dci_pending = FALSE;
    }
  #endif /* ifdef DIAG_CONSUMER_API */

  /* Trying to resend a packet which is already sent, mark state as DIAG_SEND_STATE_COMPLETE so that diagbuf_tail moves past this packet */
#ifdef DIAG_CONSUMER_API
  if(!diag_dci_pending && desc->last == send_last)
#else
  if(desc->last == send_last)
#endif /* DIAG_CONSUMER_API */
  {
    desc->state = DIAG_SEND_STATE_COMPLETE;
    return status;
  }

  #ifdef DIAG_CONSUMER_API
    if(stream_id & DIAG_STREAM_2) /* save desc for second stream before values get modified */
    {
        desc_dci.pkt = desc->pkt;
        desc_dci.last = desc->last;
        desc_dci.state = desc->state;
        desc_dci.terminate = desc->terminate;
        desc_dci.priority = desc->priority;
        desc_dci.is_cmd_rsp_type = desc->is_cmd_rsp_type;
    }
  #endif /* #ifdef DIAG_CONSUMER_API */

    if (stream_id & DIAG_STREAM_1)
    {
      #ifdef DIAG_CONSUMER_API
        if( (stream_id != (DIAG_STREAM_1 | DIAG_STREAM_2)) || ((desc->last != send_last) && !diag_dci_pending))
      #else
        if( (stream_id != (DIAG_STREAM_1)) || ((desc->last != send_last)))
      #endif /* DIAG_CONSUMER_API */
        {
         if (((FALSE == desc->is_cmd_rsp_type) && (FALSE == diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].allow_flow)) ||
            ((TRUE == desc->is_cmd_rsp_type) && (FALSE == diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].allow_flow))
            )
         {
            status = DIAGBUF_NOT_READY_S;
            ULogFront_RealTimePrintf(diag_debug_ulog_handle, 5, "diagbuf_send_pkt: allow_flow on command channel = %d, allow_flow on data channel = %d, channel_type = %d, io_type = %d, port_num = %d",\
                                    diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].allow_flow, diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].allow_flow, diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].channel_type,\
                                    diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].io_type, diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].port_num);

            return status;
         }
            status = diagbuf_send_pkt_single_proc(desc);
            send_last = (void *)desc->last;
        }

    }
    #ifdef DIAG_CONSUMER_API
    if(stream_id & DIAG_STREAM_2)
    {
           if (((FALSE == desc->is_cmd_rsp_type) && (FALSE == diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].allow_flow)) ||
           ((TRUE == desc->is_cmd_rsp_type) && (FALSE == diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].allow_flow)))
         {
            status = DIAGBUF_NOT_READY_S;
            return status;
         }
        /*Drain out diagbuf only if the DCI channel is connected*/
            status = diagbuf_send_pkt_single_proc_dci (&desc_dci);
            /* Update the desc passed in after sending desc_ctrl, in case this gets sent only to second stream */
            desc->pkt = desc_dci.pkt;
            desc->last = desc_dci.last;
            desc->state = desc_dci.state;
            desc->terminate = desc_dci.terminate;
            desc->priority = desc_dci.priority;
            desc->is_cmd_rsp_type = desc_dci.is_cmd_rsp_type;
            if(status == DIAGBUF_NOT_READY_S)
            {
              desc->state = DIAG_SEND_STATE_START;	
              if(desc->terminate == TRUE || (stream_id == DIAG_STREAM_2))
              {
                diag_dci_pending = TRUE;
              }
              else
              {   
                /* Move back tx params (head, length and used) for Stream 1*/
                diag_get_sendbuf_params(&send_buf,desc->is_cmd_rsp_type);
                pkt_len = ((int)desc->last - (int)desc->pkt + 1) + DIAG_NHDLC_PAD - DIAG_NHDLC_TERMINATE_PAD;
                send_buf.head -= pkt_len;
                send_buf.curr_data_block_header->length -= pkt_len;
                send_buf.used -= pkt_len;
                diag_update_sendbuf_params(&send_buf,desc->is_cmd_rsp_type);
                /* Reset NHDLC state for Stream 1 */
                diag_non_hdlc_encode(desc, NULL);
              }
           }
    }
    #endif /* #ifdef DIAG_CONSUMER_API */
  return status;
} /* diagbuf_send_pkt */



/*===========================================================================

FUNCTION DIAG_HDLC_ENCODE

DESCRIPTION
  This routine copies packet fragments from *src_desc to *enc.

  While copying, it performs step-wise CRC calculations and HDLC encoding.

  These descriptor structures contain memory ranges, states, etc to handle
  packetization.

  If enc->dest is NULL and src_desc is in the "start" state, CRC is
  initialized and the state is transitioned to "busy".

DEPENDENCIES
  This routine maintains state due to flow control requirements.
  The caller is responsible for maintaining integrity of the data stream.

===========================================================================*/
boolean diag_hdlc_protocol = TRUE ;

static diag_enhc_encode_state diag_enhc_state = DIAG_STATE_START;
static uint16 diag_enhc_length = 0;
static uint16 diag_enhc_total_length = 0;
static uint8 * diag_enhc_length1;
static uint8 * diag_enhc_length2;

#ifdef FEATURE_DIAG_STRESS_PROFILE
uint64 diag_hdlc_pcycles_total = 0;
#endif /* FEATURE_DIAG_SRESS_PROFILE */

void diag_hdlc_encode (diag_send_desc_type *src_desc, diag_hdlc_dest_type *enc)
{
  uint8 *dest;
  uint8 *dest_last;
  const uint8 *src;
  const uint8 *src_last;
  uint16 crc;
  byte src_byte = 0;
  diag_send_state_enum_type state;
  unsigned int used = 0;
#ifdef FEATURE_DIAG_STRESS_PROFILE
  uint64 pcycle_start, pcycle_end;

  pcycle_start = qurt_get_core_pcycles();
#endif /* FEATURE_DIAG_SERSS_PROFILE */

  if (diag_hdlc_protocol) {

  if (src_desc && enc)
    {
      /* Copy important parts to local variables. */
      src = src_desc->pkt;
      src_last = src_desc->last;
      state = src_desc->state;
      dest = enc->dest;
      dest_last = enc->dest_last;

    if (state == DIAG_SEND_STATE_START)
      {
        crc = CRC_16_L_SEED;
        state++;
      }
      else
      {
        /* Get a local copy of the CRC */
        crc = enc->crc;
      }

      /* dest or dest_last may be NULL to trigger a state transition only */
    if (dest && dest_last)
      {
        /* This condition needs to include the possiblitiy of 2 dest
           bytes for an escaped byte */
      while (src <= src_last && dest <= dest_last)
        {
          src_byte = *src++;

        if ((src_byte == DIAG_ASYNC_CONTROL_CHAR) ||
            (src_byte == DIAG_ASYNC_ESC_CHAR))
          {

            /* If the escape character is not the last byte */
          if( dest != dest_last )
            {
            crc = CRC_16_L_STEP (crc, src_byte);
              *dest++ = DIAG_ASYNC_ESC_CHAR;
              used++;
              *dest++ = src_byte ^ DIAG_ASYNC_ESC_MASK;
              used++;
          }else
            {
              /* no space available to copy the escape character, so pack
                it in the next one */
              src--;
              break;
            }
          }
          else
          {
          crc = CRC_16_L_STEP (crc, src_byte);
            *dest++ = src_byte;
            used++;
          }
        }

      if (src > src_last)
        {
        if (state == DIAG_SEND_STATE_BUSY)
          {
          if (src_desc->terminate)
            {
              crc = ~crc;
              state++;
            }
            else
            {
              /* Done with fragment, no CRC/term to be done. */
              state = DIAG_SEND_STATE_COMPLETE;
            }
          }

        while (dest <= dest_last && state >= DIAG_SEND_STATE_CRC1 &&
               state < DIAG_SEND_STATE_TERM)
          {
            /* Encode a byte of the CRC next */
            src_byte = crc & 0xFF;

            /* Copied from above for runtime efficiency */
          if ((src_byte == DIAG_ASYNC_CONTROL_CHAR) ||
              (src_byte == DIAG_ASYNC_ESC_CHAR))
            {

            if( dest != dest_last )
              {
                *dest++ = DIAG_ASYNC_ESC_CHAR;
                used++;
                *dest++ = src_byte ^ DIAG_ASYNC_ESC_MASK;
                used++;
                /* Shift to the next CRC byte.  CRC is 0 when complete. */
                crc >>= 8;
            }else
              {
                /* no space available to copy the crc, pack it in the next one */
                break;
              }
            }
            else
            {
              /* Shift to the next CRC byte.  CRC is 0 when complete. */
              crc >>= 8;
              *dest++ = src_byte;
              used++;
            }

            state++;
          }

        if (state == DIAG_SEND_STATE_TERM)
          {
          if (dest_last >= dest)
            {
              *dest++ = DIAG_ASYNC_CONTROL_CHAR;
              used++;
              state++; /* Complete */
            }
          }
        }
      }
      /* Copy local variables back into the encode structure. */
      enc->dest = dest;
      enc->dest_last = dest_last;
      enc->crc = crc;
      src_desc->pkt = src;
      src_desc->last = src_last;
      src_desc->state = state;
    }

  }

  else {

    int length_current_chunk = 0;

    src = src_desc->pkt;
    src_last = src_desc->last;

    dest = enc->dest;
    dest_last = enc->dest_last;

    /* Some callers use this state to detect a need for other initialization.
        Forcing state transition avoids multiple initialization. */

      if( DIAG_SEND_STATE_START == src_desc->state) {
      src_desc->state++;
    }

      if(dest && dest_last)
    {

          switch (diag_enhc_state) {

      case DIAG_STATE_START:

        *dest++ = DIAG_ASYNC_CONTROL_CHAR;
              if (dest > dest_last) {
          diag_enhc_state = DIAG_STATE_WRITE_FIRST_BYTE_LEN;
          break;

        }
        diag_enhc_state = DIAG_STATE_WRITE_FIRST_BYTE_LEN;

      case DIAG_STATE_WRITE_FIRST_BYTE_LEN:

              if (dest_last - dest >=1) {
          /* Memorize location of length field */
          diag_enhc_length1 = dest++;
          diag_enhc_length2 = dest++;

                if (dest > dest_last) {
            diag_enhc_state = DIAG_STATE_WRITE_PACKET;
            break;

          }
          /* Had enough space - goto writing the packet*/
          diag_enhc_state = DIAG_STATE_WRITE_PACKET;
          goto write_packet;

        }
              else if (dest_last - dest == 0) {
          diag_enhc_length1 = dest++;
          diag_enhc_state = DIAG_STATE_WRITE_SECOND_BYTE_LEN;
          break;


        }


      case DIAG_STATE_WRITE_SECOND_BYTE_LEN:

        diag_enhc_length2 = dest++;
               if (dest > dest_last) {
          diag_enhc_state = DIAG_STATE_WRITE_PACKET;
          break;

        }
        diag_enhc_state = DIAG_STATE_WRITE_PACKET;

      case DIAG_STATE_WRITE_PACKET:
        {
        write_packet:

          diag_enhc_length = src_last - src + 1;

               if (diag_enhc_length > (dest_last - dest + 1)) {
            length_current_chunk = dest_last - dest + 1;
          }
               else {
            length_current_chunk = diag_enhc_length;
          }
               memscpy(dest,(dest_last-dest + 1), src,length_current_chunk);
          dest += length_current_chunk;
          diag_enhc_length -= length_current_chunk;
          src_desc->pkt = src + length_current_chunk;
          diag_enhc_total_length += length_current_chunk;

               if (dest > dest_last) {
                   if (diag_enhc_length == 0) {

              diag_enhc_state = DIAG_STATE_END;

            }
            break;


          }
          diag_enhc_state = DIAG_STATE_END;
        }
      case DIAG_STATE_END:
        {

          src_desc->state = DIAG_SEND_STATE_COMPLETE;

                if (src_desc->terminate) {
            /*Go update the total length */
                   *diag_enhc_length1 = *((uint8 *)&diag_enhc_total_length );
            *diag_enhc_length2 = *((uint8 *)(&diag_enhc_total_length) + 1);
            *dest++ = DIAG_ASYNC_CONTROL_CHAR;
            diag_enhc_total_length = 0;
            diag_enhc_state = DIAG_STATE_START;
          }
                 else {
            diag_enhc_state = DIAG_STATE_WRITE_PACKET;
          }
        }

      }
    }

    enc->dest = dest;
    enc->dest_last = dest_last;

    src_desc->last = src_last;

  }
#ifdef FEATURE_DIAG_STRESS_PROFILE
  pcycle_end = qurt_get_core_pcycles();
  diag_hdlc_pcycles_total += pcycle_end - pcycle_start;
#endif /* FEATURE_DIAG_STRESS_PROFILE */
  return;
}

/*===========================================================================

FUNCTION DIAG_NON_HDLC_ENCODE

DESCRIPTION
  This routine copies packet fragments from *src_desc to *enc.

  These descriptor structures contain memory ranges, states, etc to handle
  packetization.

DEPENDENCIES
  This routine maintains state due to flow control requirements.
  The caller is responsible for maintaining integrity of the data stream.

===========================================================================*/
void diag_non_hdlc_encode(diag_send_desc_type *src_desc, diag_hdlc_dest_type *enc)
{
  uint8 *dest;
  uint8 *dest_last;
  const uint8 *src;
  const uint8 *src_last;

  static diag_enhc_encode_state diag_non_hdlc_state = DIAG_STATE_START;
  static uint16 diag_non_hdlc_length = 0;
  static uint16 diag_non_hdlc_total_length = 0;
  static uint8 * diag_non_hdlc_length1;
  static uint8 * diag_non_hdlc_length2;
  
  int length_current_chunk = 0;

  if(enc == NULL)
  {
    diag_non_hdlc_state = DIAG_STATE_START;
    diag_non_hdlc_total_length = 0;
    return;
  }
  src = src_desc->pkt;
  src_last = src_desc->last;

  /* Some callers use this state to detect a need for other initialization.
     Forcing state transition avoids multiple initialization. */

      if( DIAG_SEND_STATE_START == src_desc->state) {
    src_desc->state++;
  }

  dest = enc->dest;
  dest_last = enc->dest_last;

      if(dest && dest_last) {

          switch (diag_non_hdlc_state) {

    case DIAG_STATE_START:

      *dest++ = DIAG_ASYNC_CONTROL_CHAR;
      *dest++ = DIAG_CONSUMER_API_VER_1;
              if (dest > dest_last) {
                 diag_non_hdlc_state = DIAG_STATE_WRITE_FIRST_BYTE_LEN;
        break;

      }
                   diag_non_hdlc_state = DIAG_STATE_WRITE_FIRST_BYTE_LEN;

    case DIAG_STATE_WRITE_FIRST_BYTE_LEN:

              if (dest_last - dest >=1) {
        /* Memorize location of length field */
                diag_non_hdlc_length1 = dest++;
                diag_non_hdlc_length2 = dest++;

                if (dest > dest_last) {
                  diag_non_hdlc_state = DIAG_STATE_WRITE_PACKET;
          break;

        }
        /* Had enough space - goto writing the packet*/
                diag_non_hdlc_state = DIAG_STATE_WRITE_PACKET;
        goto write_packet;

      }
              else if (dest_last - dest == 0) {
                diag_non_hdlc_length1 = dest++;
                diag_non_hdlc_state = DIAG_STATE_WRITE_SECOND_BYTE_LEN;
        break;


      }


    case DIAG_STATE_WRITE_SECOND_BYTE_LEN:

               diag_non_hdlc_length2 = dest++;
               if (dest > dest_last) {
                  diag_non_hdlc_state = DIAG_STATE_WRITE_PACKET;
        break;

      }
               diag_non_hdlc_state = DIAG_STATE_WRITE_PACKET;

    case DIAG_STATE_WRITE_PACKET:
      {

      write_packet:

               diag_non_hdlc_length  = src_last - src + 1;

               if (diag_non_hdlc_length > (dest_last - dest + 1)) {
          length_current_chunk = dest_last - dest + 1;
        }
               else {
                 length_current_chunk = diag_non_hdlc_length;
        }
               memscpy(dest,(dest_last-dest + 1), src,length_current_chunk);
        dest += length_current_chunk;
               diag_non_hdlc_length -= length_current_chunk;
        src_desc->pkt = src + length_current_chunk;
               diag_non_hdlc_total_length += length_current_chunk;

               if (dest > dest_last) {
                   if (diag_non_hdlc_length == 0) {

                     diag_non_hdlc_state = DIAG_STATE_END;

          }
          break;


        }
                diag_non_hdlc_state = DIAG_STATE_END;
      }
    case DIAG_STATE_END:
      {

        src_desc->state = DIAG_SEND_STATE_COMPLETE;

                if (src_desc->terminate) {
          /*Go update the total length */
                   *diag_non_hdlc_length1 = *((uint8 *)&diag_non_hdlc_total_length );
                   *diag_non_hdlc_length2 = *((uint8 *)(&diag_non_hdlc_total_length) + 1);
          *dest++ = DIAG_ASYNC_CONTROL_CHAR;
                   diag_non_hdlc_total_length = 0;
                   diag_non_hdlc_state = DIAG_STATE_START;
        }
                 else {
                   diag_non_hdlc_state = DIAG_STATE_WRITE_PACKET;
        }
      }

    }
  }

  enc->dest = dest;
  enc->dest_last = dest_last;

  src_desc->last = src_last;

}

/*===========================================================================

FUNCTION DIAGBUF_CLEAR

DESCRIPTION
  Clear status bit of buffer for all data from start to start+offset index
   into diagbuf

===========================================================================*/
static inline void diagbuf_clear(int start, int offset) {

    int k = 0;
  diagbuf_header_type * ptr ;
    int i = DIAGBUF_ALIGNED_BYTE_4(start);
    int j = DIAGBUF_NEXT_ALIGNED_BYTE_4((start + offset));

  for (k = i; k < j ; k+=4) {
     ptr = ((diagbuf_header_type *) &diagbuf_buf[k]);
      ptr->status = DIAGBUF_UNCOMMIT_S;
    }

} /* diagbuf_clear */


/*===========================================================================

FUNCTION DIAGBUF_DRAIN

DESCRIPTION
  This function drains the diag allocation buffer to the comm layer.
  Escaping, etc is performed in this routine.

  This function may only be called by the diag task.  It is not re-entrant!

  This is considered a background process for the DIAG task.  Periodically,
  this routine checks the DIAG task sigs against 'mask'.  If any bits match,
  the routine exits promptly.

===========================================================================*/
/*=========================================================================*/
diagbuf_status_enum_type diagbuf_drain (osal_sigs_t mask)
{
  osal_sigs_t sigs;       /* Used to check if loop should be exited.      */
  int return_val = 0;
  static byte stream_id = 0;
  static int iTailIdx;       /* Byte array index to tail of diag ring buffer */
  uint32 send_buf_size = 0;

  static diagbuf_outbound_pkt_handler_type fp = NULL; /* Customer packet processor */

  diagbuf_header_type pkt_hdr;    /* Store the header in local variable     */


  static diag_send_desc_type send = {NULL, NULL, DIAG_SEND_STATE_COMPLETE, TRUE};
  byte *start_pos = NULL;
  static uint8 pad;               /* Stores the pad                         */

  diagbuf_status_enum_type status = DIAGBUF_READY_S;
  static boolean chained_extended_pkt = FALSE;/*indicate if the current pkt which is being processed
                                                 has a chained pkt*/

  static int iTail, iTail_temp;
  static int iHead;
  iHead = diagbuf_head;
  iTail = diagbuf_tail;

  /* If this fails, it typically means someone overwrote a diag buffer */
  if( (diagbuf_tail < 0) || (diagbuf_tail >= 2 * DIAGBUF_SIZE) )
    ERR_FATAL("diagbuf_drain: Buffer corruption. diagbuf_tail (%d)", diagbuf_tail, 0, 0);

  ASSERT (osal_thread_self () == diag_task_tcb);

  /* Sanity check */
  if (DIAGBUF_USED(iHead, diagbuf_tail) > DIAGBUF_SIZE)
  {
    ERR_FATAL("Ring buffer corrupt!", iHead, iTail, 0);
  }

  if (!DIAG_CHECK_MASK ((int)diag_cx_state, DIAG_CX_ANY_S))
  {
    status = DIAGBUF_NOT_READY_S;
  }
  else if( DIAGBUF_EMPTY(iHead, diagbuf_tail) )
  {
    /* Get the index of the ring buffer's tail */
    iTailIdx = DIAGBUF_CALC_IDX(diagbuf_tail);
    ASSERT (iTailIdx < DIAGBUF_SIZE);

    status = DIAGBUF_EMPTY_S;
  }
  else
  {
    /* Keep sending packets until empty or flow controlled. */
    while ( (diagcomm_status(DIAGCOMM_PORT_1)) &&
           ( status == DIAGBUF_READY_S ) &&
            (((diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].allow_flow == TRUE)
            #ifdef DIAG_CONSUMER_API
            &&
           (diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].allow_flow == TRUE)
           #endif
            ) ||
           (send.state == DIAG_SEND_STATE_COMPLETE && chained_extended_pkt == TRUE)) )
    {
      /* Get the index of the ring buffer's tail */
      iTailIdx = DIAGBUF_CALC_IDX(diagbuf_tail);

      ASSERT (iTailIdx < DIAGBUF_SIZE);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
        {
          if (DIAGBUF_EMPTY (iHead, diagbuf_tail))
        {
            status = DIAGBUF_EMPTY_S;
        }
          else
        {
            /* Prep the start of a new packet */

            pkt_hdr = *((diagbuf_header_type *) &diagbuf_buf[iTailIdx]);

            if (pkt_hdr.status == DIAGBUF_COMMIT_S)
            {
              /* Go past the header field */
              DIAGBUF_MOVE(iTailIdx, (int)sizeof(diagbuf_header_type));


              ASSERT (iTailIdx < DIAGBUF_SIZE);

              /* Initialize the send descriptor */
              send.pkt = (void *) &diagbuf_buf[iTailIdx];
              send.last = (void *) ((uint32) send.pkt + pkt_hdr.length - 1);

              if (((diagbuf_header_type *)((diagbuf_header_type *)(send.pkt) - 1))->chain == DIAGBUF_FIRST_PKT_CHAIN)
              {
                chained_extended_pkt = TRUE;/*if the current pkt is the first packet of a chained packet, we set it to TRUE*/
              }
              else
              {
                chained_extended_pkt = FALSE;
              }
              send.state = DIAG_SEND_STATE_START;
              send.terminate = TRUE;
              stream_id = diagbuf_read_stream_id(send.pkt);
              pad = pkt_hdr.pad_len;

              fp = diagbuf_get_custom_pkt_handler (diagbuf_buf[iTailIdx]);

              /* Adjust for size of overrun pattern field */
              pad += sizeof (uint16);
              iTail_temp = diagbuf_tail;


              DIAGBUF_MOVE(iTail_temp, (int)sizeof(diagbuf_header_type));

              if (DIAGBUF_USED(iHead, iTail_temp) > DIAGBUF_SIZE)
              {
                ERR_FATAL("Ring buffer corrupt!", iHead, 0, 0);
              }

              ASSERT (diagbuf_tail < 2 * DIAGBUF_SIZE);
              }
            else if (pkt_hdr.status == DIAGBUF_UNCOMMIT_S || pkt_hdr.status == DIAGBUF_UNUSED_S)
              {
              /* If panic mode, treat an uncommitted packet as a hole. */
              status = DIAGBUF_NOT_READY_S;
              }
            else if (pkt_hdr.status == DIAGBUF_WRAP_S)
              {

              iTail_temp = diagbuf_tail;
              iTailIdx = DIAGBUF_CALC_IDX(diagbuf_tail);
              diagbuf_clear(iTailIdx, 1);
              DIAGBUF_WRAP (iTail_temp);
              diagbuf_tail = iTail_temp;

              ASSERT ((diagbuf_tail == 0) || (diagbuf_tail == DIAGBUF_SIZE));
              continue;
              }
            else if (pkt_hdr.status == DIAGBUF_HOLE_S)
            {

              iTail_temp = diagbuf_tail;

              iTailIdx = DIAGBUF_CALC_IDX(iTail_temp);
              diagbuf_clear(iTailIdx, (int)(pkt_hdr.length + sizeof (diagbuf_header_type)));

              DIAGBUF_MOVE (iTail_temp, (int)(pkt_hdr.length + sizeof (diagbuf_header_type)));

              if (DIAGBUF_USED(iHead, iTail_temp) > DIAGBUF_SIZE)
              {
                ERR_FATAL("Ring buffer corrupt!", iHead, 0, 0);
              }
              diagbuf_tail = iTail_temp;

              ASSERT (diagbuf_tail < 2 * DIAGBUF_SIZE);
              continue;
              }
              }
      } /* if start of new packet */

      if (status == DIAGBUF_READY_S)
        {
        start_pos = (byte *) send.pkt;
        if (fp)
        {
          send.state = DIAG_SEND_STATE_START;
           /*If the MSB is set in stream id, then the pkt was generated 
            in UserPD or DLL and can be sent out without any processing*/
            if ( stream_id & DIAGBUF_OUTBOUND_PKT_HANDLER_BIT_MASK )
            {
              /* Calculate length of packet */
              diag_pkt_len = pkt_hdr.length;
              (void)diagbuf_send_pkt(&send, stream_id);
            }
            else
          fp(&send,stream_id);
          }
          else
          {
            /* Calculate length of packet */
            diag_pkt_len = pkt_hdr.length;
            (void)diagbuf_send_pkt(&send, stream_id);
          }

        if (DIAGBUF_USED(iHead, diagbuf_tail) > DIAGBUF_SIZE)
          {
          ERR_FATAL("Ring buffer corrupt!", iHead, 0, 0);
          }
          /* Move the tail. */

        if (send.state == DIAG_SEND_STATE_COMPLETE)
          {

          iTailIdx = DIAGBUF_CALC_IDX(diagbuf_tail);
          diagbuf_clear(iTailIdx, (int)sizeof(diagbuf_header_type));
          DIAGBUF_MOVE(iTailIdx, (int)sizeof(diagbuf_header_type));

          diagbuf_clear(iTailIdx, (int)((uint32) send.pkt - (uint32) start_pos));
          DIAGBUF_MOVE(iTailIdx, (int)((uint32) send.pkt - (uint32) start_pos));

          DIAGBUF_MOVE(iTail_temp, (int)((uint32) send.pkt - (uint32) start_pos));

          if (DIAGBUF_USED(iHead, iTail_temp) > DIAGBUF_SIZE)
          {
            ERR_FATAL("Ring buffer corrupt!", iHead, 0, 0);
          }
          diagbuf_tail = iTail_temp;

          ASSERT (diagbuf_tail < 2 * DIAGBUF_SIZE);
          ASSERT (iTailIdx < DIAGBUF_SIZE);

            fp = NULL;

            /* Packet has been exhausted.  Adjust for pad. */
          diagbuf_clear(iTailIdx, pad);
          DIAGBUF_MOVE(iTailIdx, pad);

          iTail_temp = diagbuf_tail;
          DIAGBUF_MOVE(iTail_temp, pad);
          diagbuf_tail = iTail_temp;

          ASSERT (diagbuf_tail < 2 * DIAGBUF_SIZE);
          ASSERT (iTailIdx < DIAGBUF_SIZE);
            }

          /* Sanity check */
        if (DIAGBUF_USED(iHead, diagbuf_tail) > DIAGBUF_SIZE)
          {
            /* Panic mode will reenter this function.  Must put system in a
               recoverable state. */
            // diagbuf_head = diagbuf_tail = 0;
          ERR_FATAL("Ring buffer corrupt!", iHead, 0, 0);

            /* Panic mode will swap in the DIAG task - must be able to return
               to normal task operations, so exit the loop. */
/*lint -save -e{527} */
            break;
/*lint -restore */
          }
        if (DIAGBUF_EMPTY(iHead, diagbuf_tail))
          {
            status = DIAGBUF_EMPTY_S;
          }
        }

      if(diagbuf_tail % 4 != 0)
         ERR_FATAL("diagbuf_tail 0x%x is not 4 multiple", diagbuf_tail, 0, 0);


      /* Get signals */
      return_val = osal_get_sigs(diag_task_tcb, &sigs);
      ASSERT(OSAL_SUCCESS == return_val);

      /* RPT_TIMER_SIG?  Kick the dog */
      if (sigs & DIAG_RPT_SIG)
      {
        diag_kick_watchdog();
      }

      /*if we are handling a pkt which has a chained pkt, do not check signals
         we should drain the second pkt before checking signals*/
      if (chained_extended_pkt ==FALSE)
      {
        /* mask sig or diag is blocking?  Break out.  */
        if (sigs & mask)
        {
          break; /* Exit loop.  Something more important has happened! */
        }
      }
    } /* End of while() */
    }

#ifdef DIAG_CONSUMER_API
  if((diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].allow_flow == FALSE) ||
           (diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].allow_flow == FALSE))
#else
    if(diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].allow_flow == FALSE)
#endif /* DIAG_CONSUMER_API */
  {
    /* We are unable to drain as we are flow controlled. Set signal
       to ensure forwarding task is draining the DSMs.  When DSM pool is free
       again, DIAG_TX_SIG will be set to continue draining. */
    #if defined(MP_MASTER)
    diag_fwd_notify();
    #endif
  }
  else if (status == DIAGBUF_READY_S && diagcomm_status(DIAGCOMM_PORT_1))
  {
    /* Interrupted for some reason - signal DIAG so it will come back.
       There is no reason to terminate diagbuf_send_pkt() at this time,
       since we'll be right back! */

    diag_tx_notify();
  }
  else
  {
    if (status != DIAGBUF_EMPTY_S && DIAG_CHECK_MASK (diag_cx_state, DIAG_CX_COMM_S))
    {
      diag_set_drain_timer();
    }
    /* If the device is in NRT mode force flush the outbound buffer as data might not be generated in a timely manner. 
       If the device is in RT mode, just set the drain timer for the data to be drained in a timely manner.*/
    if(diagcomm_status(DIAGCOMM_PORT_1))
    {
      /* This condition will be true when diag_drain_timer_len is 0, i.e. in NRT mode */
      if(!diag_drain_timer_enabled())
      {
         /* Force flushing in NRT mode when data traffic should be low */
         #ifdef DIAG_CONSUMER_API
           (void) diagbuf_send_pkt (NULL,DIAG_STREAM_1 | DIAG_STREAM_2);
         #else
           (void) diagbuf_send_pkt (NULL,DIAG_STREAM_1);
         #endif /* DIAG_CONSUMER_API */
      }
      else
      {
        /* In RT mode check if there is data in sendbuf */
        send_buf_size = diag_get_send_size(DIAGCOMM_PORT_1);
        if(send_buf_size) // There is some data in send_buf
        {
          diag_set_drain_timer(); // Set the drain timer to flush the data in send buf
        }
        else
        {
          diag_clr_drain_timer(); // Clear the timer if there is no data in send buf
        }
      }    
    }

  }

  iHead = diagbuf_head;


  /* Sanity check */
  if (DIAGBUF_USED(iHead, diagbuf_tail) > DIAGBUF_SIZE)
  {
    ERR_FATAL("Ring buffer corrupt!", iHead, 0, 0);
  }

  return status;

} /* diagbuf_drain */


/*===========================================================================

FUNCTION DIAGBUF_REGISTER_OUTBOUND_PKT_HANDLER

DESCRIPTION
  This function registers a function pointer to handle the sending of a
  packet.  diagbuf_drain() sends packets in raw format.  If processing/
  formatting needs to occur in DIAG task context prior to sending the
  packet, one can register a function pointer with the command code, which
  will be called when diagbuf_drain finds a packet with that command code.

  This system requires that diagbuf_send_pkt is used to send the final
  packet to the comm layer.  This routine maintains a state, which requires
  that all outbound packets be sent via that function.

===========================================================================*/
/*=========================================================================*/
boolean diagbuf_register_outbound_pkt_handler (
   uint8 cmd_code,
   diagbuf_outbound_pkt_handler_type fp
)
{
  unsigned int i;
  boolean found = FALSE;

  if (fp)
  {
    for (i = 0; !found && i < DIAGBUF_PKT_HANDLER_TBL_CNT; i++)
    {
      if (diagbuf_outbound_pkt_handler_tbl[i].cmd_code == cmd_code ||
          diagbuf_outbound_pkt_handler_tbl[i].cmd_code == 0xFF)
      {
        found = TRUE;

        diagbuf_outbound_pkt_handler_tbl[i].cmd_code = cmd_code;
        diagbuf_outbound_pkt_handler_tbl[i].fp = fp;
      }
    }
  }

  return found;
}



/*=========================================================================*/
static diagbuf_outbound_pkt_handler_type
  diagbuf_get_custom_pkt_handler (uint8 cmd_code)
{
  unsigned int i;
  diagbuf_outbound_pkt_handler_type fp = NULL;

  for (i = 0; !fp && i < DIAGBUF_PKT_HANDLER_TBL_CNT; i++)
  {
    if (diagbuf_outbound_pkt_handler_tbl[i].cmd_code == cmd_code)
    {
      fp = diagbuf_outbound_pkt_handler_tbl[i].fp;
    }
  }

  return fp;
}

/*===========================================================================

FUNCTION DIAGBUF_FLUSH

DESCRIPTION
  This function wrapper over diagbuf_flush_timeout which configures all streaming data items off.
  No more allocations will occur until these settings are changed.

  Diag will not stop collecting data immeidiately.  The data currently
  allocated will still be flushed.

===========================================================================*/
void diagbuf_flush (void)
{
  if (osal_thread_self() == diag_task_tcb)
  {
    diagbuf_flush_timeout(0);
  }
  else
  {
    /* This guarantees that flush will always happen in the diag context. */
    diag_set_internal_sigs(DIAG_INT_FLUSH_BUF_SIG);
  }
} /* diagbuf_flush_timeout */

/*===========================================================================

FUNCTION DIAGBUF_FLUSH_TIMEOUT

DESCRIPTION
  This function configures all streaming data items off.  No more allocations
  will occur until these settings are changed.It performs the same operation
  as diagbuf_flush only with optional timeout value.

  Diag will not stop collecting data immeidiately.  The data currently
  allocated will still be flushed.

  diag will block after flushing the data for specified timeout duration or
  time taken by SIO to flush the data(which ever is smaller).

  diag_flush_timeout flag will be set to TRUE if diag times out on SIO callback

@param[in] timeoutVal    Specifies the timeout duration. If specified as 0,
timeout will not occur.

===========================================================================*/
void diagbuf_flush_timeout(unsigned int timeoutVal)
{
  osal_sigs_t return_sigs;
  dword sigs;
  int return_val = 0;
  /*-----------------------------------------------
    Turn off messages, logs, and event reporting.
  -----------------------------------------------*/
  msg_set_all_rt_masks_adv (0, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);

  /* Listeners must still be serviced, but logs and events for the tool must
     at least stop being sent to the comm layer. */
  log_flush ();
  event_flush ();

  /* In case there is no connection and the buffer is non-empty. */
  DIAG_SET_MASK (diag_cx_state, DIAG_CX_FLUSH_S);

  /* Go ahead and drain the ring buffer
  */
  while (diagbuf_drain (0) != DIAGBUF_EMPTY_S)
  {
    sigs = diag_wait (DIAG_TX_SIG | DIAG_DRAIN_TIMER_SIG);
    if (sigs & DIAG_TX_SIG) {
      diag_tx_pending = FALSE;
      return_val = osal_reset_sigs(diag_task_tcb, DIAG_TX_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
    }
    if (sigs & DIAG_DRAIN_TIMER_SIG) {
      diag_drain_timer_pending = FALSE;
      return_val = osal_reset_sigs(diag_task_tcb, DIAG_DRAIN_TIMER_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
    }
  }

#if defined (DIAG_MP) && !defined (DIAG_MP_MASTER)
  if (diagcomm_cmd_status(DIAGCOMM_PORT_1 ))
     diagpkt_rsp_send();
#else
  if (diagcomm_status(DIAGCOMM_PORT_1 ))
     diagpkt_rsp_send();
#endif

  /* After flush, the comm connection state is considered disconnected. */
  DIAG_CLR_MASK (diag_cx_state, DIAG_CX_FLUSH_S | DIAG_CX_COMM_S);

  /* Flush the TX stream in the comm layer. */
    diagcomm_flush_tx (NULL,timeoutVal);
}

#if defined FEATURE_WINCE
   #error code not present
#endif /* FEATURE_WINCE */
/*===========================================================================

FUNCTION DIAGBUF_TX_SLEEP_PARAMETERS_SET

DESCRIPTION
This sets the value of diagbuf_tx_sleep_threshold and diagbuf_tx_sleep_time.
These parameters are initialized, after reading NV(AMSS)/Registry(WM).

===========================================================================*/
void diagbuf_tx_sleep_parameters_set(void)
{
#if defined (DIAG_QDSP6_APPS_PROC)
  return;
#else
#if defined (FEATURE_DIAG_NV) && !defined (FEATURE_WINCE) /* Non-WM, read from NV */
  uint64 sleep_nv_amt = 0;
  uint32 sleep_time_nv_amt = 0;
  nv_stat_enum_type sleep_nvstatus = NV_FAIL_S;

  sleep_nvstatus = diag_nv_read (NV_DIAG_DIAGBUF_TX_SLEEP_THRESHOLD_EXT_I, &sleep_nv_amt);
  if(sleep_nvstatus == NV_DONE_S)
  {
    diagbuf_tx_sleep_threshold = (uint32)sleep_nv_amt;
  }

  sleep_nvstatus = diag_nv_read (NV_DIAG_DIAGBUF_TX_SLEEP_TIME_NV_I, &sleep_time_nv_amt);
  if(sleep_nvstatus == NV_DONE_S)
  {
    diagbuf_tx_sleep_time = sleep_time_nv_amt;
  }
#elif defined (FEATURE_WINCE) /* WM, read from registry */
  #error code not present
#endif /* FEATURE_WINCE */
#endif /* DIAG_QDSP6_APPS_PROC */
} /* diagbuf_tx_sleep_parameters_set */

/*===========================================================================
FUNCTION diagbuf_ctrl_tx_sleep_parameters

DESCRIPTION
Control the value of diagbuf_tx_sleep_time and diagbuf_tx_sleep_threshold.
Currently used to change modem operation for on-device logging low-power mode
===========================================================================*/
void diagbuf_ctrl_tx_sleep_parameters (unsigned int threshold_val, unsigned int sleep_val)
{
  diagbuf_tx_sleep_time = sleep_val;
  diagbuf_tx_sleep_threshold = threshold_val;
}

/*===========================================================================
FUNCTION diagbuf_ctrl_commit_threshold
DESCRIPTION
Control diagbuf_commit_threshold
===========================================================================*/
void diagbuf_ctrl_commit_threshold(unsigned int val)
{
  diagbuf_commit_threshold = val;
}
/*===========================================================================
FUNCTION diagbuf_ctrl_commit_threshold

DESCRIPTION
Control diag_send_buf_pad length
===========================================================================*/
