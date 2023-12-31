/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostics Packet Processing

General Description
  Diagnostic packet processing core routines.

Copyright (c) 1990-2019 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================

                           Edit History

$Header: //components/rel/core.tx/6.0/services/diag/DCM/rtos/src/diagpkt.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/19   kdey    Porting of Qsockets
12/12/18   kdey    Featurizing macros under #ifndef to allow their definition outside diag
10/31/17   gn      Changes to support MMU enablement
09/07/17   kdey    Check added for tbl_entry->func_ptr is NULL or not
04/10/17   gn      DIAG QAPI header file name change to qapi_diag.h
03/24/17   gn      Changes to support DIAG delayed reponses on User space
02/24/17   gn      Changes to support DIAG QAPIs with DAM framework
09/06/16   ph      De-couple extended build ID commands with polling command.
07/20/16   nk      Changes to support ThreadX
05/27/16   ph      Updated send descriptor in diagpkt_delay_rsp_send().
04/28/16   nk      KW warnings fixes
01/13/16   is      Send feature mask before processing commands
01/16/15   sa      Updated the name for the commands injected in Diag Cmd Queue.
11/18/14   sa      Updated the condition for master to respond to the Mask commands.
10/01/14   sr      Removed the check for funct handler for cmds in slave table
09/24/14   xy      Do not register/propagate registration tables that have zero entries
05/27/14   rs      Fixed issues in composite mask update for build mask centralization.
02/28/14   sa      Support for peripherals to handle status and build version
                   commands in buffering mode if separate command channel is
                   present.
02/26/14   sa      Added support for compression in buffering mode.
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
01/20/14   rh      Added support for centralized build masks
01/07/14   xy      Added fusion DCI feature
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0
09/20/13   tbg     Added support for Diag over STM
09/12/13   xy      Fixed KW warnings
10/01/13   xy      Removed code under FEATURE_WINCE
08/23/13   sr      Added support for diag mask change notification for
                   registered users
06/03/13   sa      Fixed issues responding to V1 and V2 Diag Test Commands.
04/09/13   rs      Modified delay rsp id type to uint16.
04/03/13   sr      Replace memcpy() and memmove() with memscpy() and memsmove()
02/27/13   is      Use system heap instead of diag heap for registrations
02/22/13   is      Resolve Klockwork issue
01/25/13   sr      Fixed compiler warnings
01/21/13   is      Support DIAG_EXT_LOGMASK_F with preset masks
12/14/12   is      Use correct define for diagbuf_commit_threshold
11/16/12   ph      Klocwork warning fixes
11/15/12   is      Support for preset masks
10/30/12   ph      Polling command changes.
10/26/12   sr      Fixed the race condition issue with diagpkt_tbl_reg()
12/03/12   rh      Renamed diag_blocking_malloc to diag_rsp_malloc to better
                   reflect its purpose.
11/27/12   rh      Mainlined DIAG_CENTRAL_ROUTING; removed use_central_routing
10/30/12   ph      Polling command changes.
10/12/12   rh      Changed non-realtime parameters for Diag event draining
09/17/12   ra      Fixed slave table index going out of bounds
08/24/12   ra      Support for querying Command Registration Tables
08/31/12   ra      Use accessor macros instead of accessing private task
                   info directly; accomodated changes for User space
                   task priority changes
08/10/12   sa      Replaced DIAG_REQ_FWD with DIAG_MP_MASTER
08/03/12   vk      NULL pointer check in diagpkt_subsys_get_cmd_code and
                   diagpkt_subsys_get_id
07/24/12   ra      Changes for Polling command
06/13/12   is      Set rsp_ptr to NULL to avoid double responses
06/12/12   is      Allow CM "reset" command to response from Apps
06/11/12   is      Prevent duplicate responses for "get mask" requests
05/15/12   is      Allow "build time" mask queries from Master Diag
05/03/12   rs      Moved delayed response from diag buffer to AMSS heap
03/02/12   is      Support handling reset and powerdown CM commands on Master Diag
03/02/12   sg      Set DIAG_INT_CTRL_REG_SIG  only when the ctrl port is opened
                   on both the processors
10/07/11   is      Master Diag forwards requests before committing response.
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/15/11   is      Modifed "set mask" via CTRL msgs fot EA support
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
06/30/11   vs      Respond back with bad len response for cmd code 75,128  if
                    length is less than 4 bytes. (size of header)
04/28/11   is      Increase size of slave registration table
04/27/11   is      Resolve compiler warnings
04/18/11   is      Ensure CTRL msg sizes are constant
03/29/11   sg      Central Routing changes for Q6
03/04/11   is      Support for Diag over SMD-Lite
02/28/11   sg      Power saving changes
02/07/11   vs      Ignore reset command for fusion 9k, rpc diag command from
                    slave to master
12/20/10   mad     Changes for dual diag mask
09/28/10   mad     Added handler for diag mode change control message
10/04/10   is      Include master table in slave table define
09/14/10   is      New control channel and central routing features
08/20/10   sg      Added a check to drop the response when the response length
                   is greater than half of diag heap size
08/11/10   sg      Cleaned up call to event_Drain() in diagpkt_alloc since
                   we have separate event heap
08/09/10   is      Move definition of diagpkt_filtered_cmd_set here and extern
                   in diagcomm_v.h
07/23/10   mad     Moved diagpkt_check_validity_request_ondevice() to diag.c
07/10/10   vs      Vote for sleep after data is sent to SMD
06/07/10   is      Removed MSMulate_diag_register_ltk_init since it's not supported anymore
05/13/10   JV      Merge functionality in diagcomm_smd.c and diagcomm_smd2.c
                   in a single file
05/06/10   as      Added function to handle packet filteration.
04/29/10   is      Removed support for DIAG_DEBUG_FS_PKT_LOG, FEATURE_DIAGQ6TEST_INIT
04/29/10   JV      Events now use a separate heap space from responses. So, do not wait
                   on signals related to events while not able to allocate memory
                   for responses.
03/25/10   sg      Call osal_thread_wait() in diagpkt_alloc() to wait on rx_rsp_sig
                   and event_drain_sig when we run out of heap to drain the events
                   and responses
02/02/10   JV      Call functions to drain responses and events in case
                   diag_blocking_malloc() fails. Diagbuf_drain() does not
                   do this anymore.
01/27/10   ps      Decouple Error functions from diag
01/08/10   vs      Invoke diagbuf_send_pkt instead of multi_proc
12/28/09   ps      Fixed Klocwork errors & compiler warnings.
12/10/09   sg      Added flow control mechanism for DSM_DIAG_SIO_TX_ITEM_POOL
                   at item level granularity
12/07/09   cahn    Added Delayed Response to DIAG Health.
11/30/09   ps      Fixed compiler warnings.
11/24/09   cahn    Featurized call to diagdiag_tbl_init(). This function is
                   defined in diagdiag.c, which is not compiled for QDSP6.
11/04/09   cahn    DIAG Health
10/03/09   sg      Added call to diagdiag_tbl_init()
10/22/09   as      Decoupled nvdiag from Diag module.
10/21/09   JV      Putting in a new feature so that we can now use the
                   diagbuf_send_pkt_multi_proc() function to send data out
                   if we require the response to be prioritized.
09/28/09   vs      Moved proc_id initialization from diagpkt.h to diagpkt.c
09/10/09   JV      Removed extern diagdiag_init.
09/01/09   JV      Changes to migrate from EFS1 to EFS2 APIs.
08/28/09   JV      Fix to enable boot-up messages. Register toolsdiag and diagdiag
                   sooner. Moved from here to diag_init().
08/17/09   vs      Added call to diag_tx_notify for delayed respose
08/17/09   JV      Removed calls to hsdiag_init(), vocdiag_init(), fsdiag_init()
                   and prldiag_init().
07/31/09   JV      Merged Q6 diag code back to mainline
07/23/09   JV      Including diagdsm_v.h instead of diagdsm.h
07/10.09   as      Decoupled FEATURE_DIAG_NON_STREAMING and mainlined code
                   under FEATURE_DIAG_MANUAL_DISPATCH_TABLE
06/05/09   as      Reverted changes from EFS2 to EFS1 Api's
05/12/09   JV      Introduced the OS abstraction layer for rex.
05/07/09   vk      changes made to move from EFS1 to EFS2 apis
05/07/09   vk      changed includes from external to internal headers
03/06/09   vg      Created a function to init the diagpkt_rsp_q. Called from
                   diag_init
02/18/09   as      Changed DSM pool id used to forwarding inbound req.
02/06/09   ps      Changes to support wrapping of response Id count after
                   0xFFFF & Return wrap count.
01/08/09   as      Merged code resolve deadlock and thread protection issues
01/07/08   vg      Added FEATURE_FSDIAG_INIT this will enable the call to
                   fsdiag_init.
10/03/08   vg      Updated code to use PACK() vs. PACKED
                   Added FEATURE_TOOLSDIAG_INIT & FEATURE_NVDIAG_INIT
                   These features need to be defined only for 1 core in the
                   builds file.
09/11/08   sj      Removed calls to certain init routines for DIAG VU Decoupling
09/11/07   pc      Introduced check through diagcomm_fwd_status() before
                   transmitting over SMD.
11/21/06   as      Modified code to use diag internal features.
06/29/06   giri    Core services build support. Initializing the thinner
                   version of the core services build.
05/19/06   as      Added Multi processor modem only support
04/26/06   as      Moved vocdiag_init() under #ifndef FEATURE_WCDMA_PLT and
                   GSM PLT flags.
10/31/05   as      Fixed lint errors.
05/19/05   as      Fixed subsystem V2 delayed response length.
04/25/05   as      Fixed comparison in function diagpkt_shorten.
03/23/05   as      Added check to prevent de-referencing NULL pointer in the
                   diagpkt subsystem allocate functions.
03/09/05   as      Delayed Rsp ID is prevented from wrap-around to 1.
03/14/05   as      Moved diagpkt_init_manual_dispatch_table() call to diag.c
02/10/05   as      Corrected featurization.
02/10/05   as      Added FEATURE_DEBUG_DIAG_HEAP to debug memory corruption.
02/08/05   as      Added FEATURE_WPLT around gsmdiag_init().
12/14/04   sl      Added support for FEATURE_DIAG_MULTI_PORTS.
12/23/04   ec      Added function diagpkt_free()
11/30/04   as      Added FEATURE_AUTO_RESET. This feature resets the phone
                   automatically without user intervention if the phone
                   encounters a critical failure
04/27/04   eav     Added FEATURE_SAVE_DEBUG_TRACE to diagdebug_init()
03/15/04   as      Implemented functions to access fields in delayed response
                   header
03/05/04   as      Added diag_kick_watchdog() call, to fix diag watchdog timeout
01/21/04   as      Added prldiag_init()
12/16/03   as      Added support for version 2 subsystem packet. This
                   mechanism will allow the user to request delayed response.
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
01/08/03   ma      featurization for GSM PLT
01/07/03   lad     CRC calc for TX moved to diagbuf_send_pkt().
12/09/02   djm     do not hsdiag_init() for WPLT build
11/21/02   lad     Changed response packet queuing implementation to use
                   DIAG heap and a queue in lieu of diagbuf_alloc().
01/21/02   as      Changed diagbuf_blocking_alloc() to diagpkt_alloc().
05/21/01   lad     Moved dispatching routines to diagpkt_dispatch.c.
                   Moved header manipulation from diagbuf.c.
                   Improved opaque header support.
04/30/01   lad     Added implementation of diagpkt_subsys_alloc().
04/06/01   lad     Made diagpkt_dispatch() from diagpkt_process_request() for
                   reuse.
                   Introduced use of diagpkt_cmd_code_type.
02/23/01   lad     Created file.

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "diag_v.h"
#include "diagi_v.h"
#include "diagbuf_v.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "diagdiag_v.h"
#include "diagcomm_smd.h"
#include "diagcomm_ctrl.h"
#include "diagcomm_io.h"
#include "diagdsm_v.h"
#include "diagcomm_v.h" /* For diagpkt_filtered_cmd_set */
#include "diagtarget.h"
#include "msg.h"
#include "msgi.h"      /* For diag_ctrl_update_msg_mask() */
#include "err.h"
#include "diag_cfg.h"
#include "feature.h"   /* For diag_int_feature_mask[] */
#include <stringl/stringl.h>
#include "paramsdiag.h"
#include "diag_stm.h"
#include <stdlib.h> /*For malloc api*/
#include "diagcomm_cmd.h"
#include "qapi_diag.h"
#include "diagcomm_dci.h"

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "dog.h"
#include "assert.h"
#endif

#include "osal.h"

#include <stdio.h>
#include <string.h>     /* Prototype for memcpy() */
#include "eventi.h" /* for event_ctrl_stale_timer */
#include "diagtune.h" /* for EVENT_RPT_PKT_SIZE etc */

#ifdef DIAG_CONSUMER_API_CMD_REQ
  #include "list.h"
#endif

#ifdef FEATURE_DIAG_SWITCH
  extern int diag_on_single_proc;
#endif

#define WCDMA_STATUS_F 14
#define TDSCDMA_STATUS_F 14
#define GSM_STATUS_F 02

static q_type diagpkt_rsp_q;

typedef struct
{
  q_link_type qlink;

  unsigned int pattern;     /*!< Pattern to check validity of committed pointers. */
  unsigned int length;      /*!< Size of packet */
  byte pkt[1];
}
diagpkt_delay_q_type;

static q_type diagpkt_delay_rsp_q;

static struct
{
   boolean pending;

   diag_cmd_rsp rsp_func;
   void *rsp_func_param;
}
diagpkt_rsp_func_store = {FALSE, NULL, NULL};

/* This macro calculates the corresponding queue item pointer from a packet
   pointer. */
#define DIAGPKT_PKT2QITEM(p) \
  /*lint -save -e{545} */ ((diagpkt_q_type *) (((byte *) p) - FPOS (diagpkt_q_type, rsp.pkt))) /*lint -restore */
#define DIAGPKT_PKT2RSPTYPE(p) (&(DIAGPKT_PKT2QITEM (p)->rsp))

#define DIAGPKT_PKT2DELAYQITEM(p) \
  /*lint -save -e{545} */ ((diagpkt_delay_q_type *) (((byte *) p) - FPOS (diagpkt_delay_q_type, pkt))) /*lint -restore */


#define DIAGPKT_RSP_LEN_MAX ((DIAG_RSP_HEAP_SIZE)/2)

#if (DIAG_RSP_HEAP_SIZE < DIAGPKT_RSP_LEN_MAX)
#error "Size of response heap cannot be smaller than the maximum possible length of response."
#endif
extern uint8 diag_cur_preset_id; /* from diag.c */

extern uint8 diag_mask_notify;   /* from diag.c */
extern osal_timer_t diag_mask_change_notify_timer;   /* from diag.c */

static int diagpkt_master_tbl_init_begin = FALSE;
static int diagpkt_master_tbl_initialized = FALSE;
extern int diagpkt_user_space_tbl_initialized;
osal_mutex_arg_t diagpkt_tbl_reg_lock;

/* This is the master dispatch table. */
const diagpkt_master_table_type * diagpkt_master_table[DIAGPKT_MSTR_TBL_SIZE];
uint8 diagpkt_master_tbl_index = 0;

/* This stores the current index/count of the entries in the slave table
   for a particular port */
static uint8 diagpkt_slave_tbl_indices[NUM_SMD_PORTS];

#ifdef DIAG_MP_MASTER
  diagpkt_slave_table_type * diagpkt_slave_table[NUM_SMD_PORTS][DIAGPKT_MSTR_TBL_SIZE];
  static osal_mutex_arg_t diagpkt_slave_tbl_mutex;
  boolean is_polling_cmd_registered[NUM_SMD_PORTS];
  boolean diagpkt_is_polling_registered(void);
  boolean diagpkt_check_for_polling_cmd(const diagpkt_master_table_type *mstr_data);
#else
  typedef enum
  {
    DIAGPKT_TBL_FWD_STATUS_NOT_SENT = 0,
    DIAGPKT_TBL_FWD_STATUS_QUEUED = 1,
    DIAGPKT_TBL_FWD_STATUS_SENT = 2,
  } diagpkt_tbl_fwd_status;

  static uint8 diagpkt_table_fwd_status[DIAGPKT_MSTR_TBL_SIZE];
#endif // DIAG_MP_MASTER

#define DIAGPKT_MAX_DELAYED_RSP 0xFFFF
/* List to store delayed_response ID and the corresponding DCI tag */
#ifdef DIAG_CONSUMER_API_CMD_REQ
  static list_type diagpkt_delayed_tag_list;

  typedef struct {
    list_link_type link;
    uint32 tag;
    uint16 delayed_rsp_id;
  } diagpkt_delayed_tag_type;

  int diagpkt_delayed_tag_compare(void *list_item, void *compare_val);
  extern uint32 diag_dci_cmd_tag;

#endif /* DIAG_CONSUMER_API_COM_REQ */
/* delayed_rsp_id 0 represents no delay in the response. Any other number
   means that the diag packet has a delayed response. */
static uint16 delayed_rsp_id = 1;

static uint16 wrap_enabled = 0;
static uint32 wrap_count = 0;
static uint32 diagpkt_drop_cnt = 0;
static uint32 diagpkt_delay_drop_cnt = 0;
static uint32 diagpkt_delay_alloc_cnt = 0;


/* This macro gets the next delayed respose id. Once it reaches
   DIAGPKT_MAX_DELAYED_RSP, it stays at DIAGPKT_MAX_DELAYED_RSP */
uint16 diagpkt_next_delayed_rsp_id(uint16 rspid);
#ifdef FEATURE_DEBUG_DIAG_HEAP
uint32 diagpkt_alloc_cnt = 0;
uint32 diagpkt_subsys_alloc_cnt = 0;
uint32 diagpkt_subsys_alloc_v2_cnt = 0;
uint32 diagpkt_commit_free_cnt = 0;
uint32 diagpkt_delay_commit_free_cnt = 0;
uint32 diagpkt_rsp_send_free_cnt = 0;
uint32 diagpkt_delay_rsp_send_free_cnt = 0;
uint32 diagpkt_process_request_cmd_req_queued = 0;
uint32 diagpkt_process_req_fs_close_queued = 0;
uint32 diagpkt_subsys_alloc_fs_close = 0;

uint32 get_rsp_q_cnt(void)
{
  return q_cnt(&diagpkt_rsp_q);
}

extern uint32 diag_heap_used_bytes(void);
extern uint32 get_event_q_cnt(void);

#endif /* FEATURE_DEBUG_DIAG_HEAP */

extern void diag_rx_rsp_notify( void );
boolean diagpkt_rsp_send (void);
boolean diagpkt_delay_rsp_send(void);
boolean diagpkt_filtered_cmd_set = FALSE;

extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */
extern boolean bIsHdlcEncoded; /* From diagcompressed_buffer.c */

/* Extern Varialbles for Registration Table Retrieval commands */
extern diagpkt_subsys_cmd_code_type reg_tbl_cmd_code;  /*  Command Code */
extern diagpkt_subsys_delayed_rsp_id_type reg_tbl_delay_rsp_id; /* Retrieval Delayed Response ID */
extern boolean reg_tbl_is_master; /* Flag for master processor */
extern uint8 reg_tbl_port_id; /* Port ID to identify processor in Slave Table */

//For resolving linking error
extern void diagpkt_process_user_space_command (diagpkt_user_table_entry_type *tbl_entry, void *req_pkt, uint16 pkt_len, uint8 index);
extern boolean diagpkt_user_space_tbl_init (void);

/* -------------------------------------------------------------------------
 * Internal to diag functions
 * ------------------------------------------------------------------------- */
uint32 getWrapcount(uint16 wrap_flag)
{
    if(wrap_enabled == 0)
        wrap_enabled = wrap_flag;
    return wrap_count;
}

#if !defined(DIAG_MP_MASTER)
/*===========================================================================
FUNCTION DIAGPKT_FWD_CMD_REG (internal)

DESCRIPTION
  Creates ctrl reg msgs for each registered cmd table and forwards to
  master proc.

PARAMETERS
  None

RETURN VALUE
  None

============================================================================= */
void diagpkt_fwd_cmd_reg ()
{
  static int diagpkt_fwd_tbl_index = 0;
  const diagpkt_master_table_type * tbl_ptr = NULL;
  diag_ctrl_msg_type reg_pkt;
  uint32 len_written = 0;
  uint32 userpkt_len = 0;
  boolean is_sent = FALSE;
  int start_index = diagpkt_fwd_tbl_index;

  for( diagpkt_fwd_tbl_index = diagpkt_fwd_tbl_index;
       diagpkt_fwd_tbl_index < DIAGPKT_MSTR_TBL_SIZE;
       diagpkt_fwd_tbl_index++ )
  {
    tbl_ptr = diagpkt_master_table[diagpkt_fwd_tbl_index];

    /*Do not forward registration table to App if the table has zero entries.*/
    if( (tbl_ptr != NULL)  && (tbl_ptr->count > 0 ) && (diagpkt_table_fwd_status[diagpkt_fwd_tbl_index] == DIAGPKT_TBL_FWD_STATUS_NOT_SENT) )
    {
      /*-------------------------------------------------------------------------
        Obtain a new dsm item and copy the data from the table to the DSM_ITEM.
        This dsm item is sent to master Diag processor.
        -------------------------------------------------------------------------*/

      userpkt_len = sizeof(diagpkt_user_table_entry_type) * tbl_ptr->count;

      /* Compose DIAG_CTRL_MSG_REG packet */
      reg_pkt.cmd_type = DIAG_CTRL_MSG_REG;
      reg_pkt.data_len = sizeof(diag_ctrl_msg_reg_type) + userpkt_len;
      reg_pkt.data.ctrl_msg_reg.version = DIAG_CTRL_MSG_REG_VER;
      reg_pkt.data.ctrl_msg_reg.cmd_code = tbl_ptr->cmd_code;
      reg_pkt.data.ctrl_msg_reg.subsysid = tbl_ptr->subsysid;
      reg_pkt.data.ctrl_msg_reg.count = tbl_ptr->count;
      reg_pkt.data.ctrl_msg_reg.port = diagcomm_ctrl_port_id(DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1); //SIO_PORT_SMD_DIAG

      len_written = diagcomm_ctrl_send_mult_pkt( (PACK(void *))&reg_pkt, DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_reg_type),
                                                 (PACK(const void *))tbl_ptr->user_table, userpkt_len,
                                                 DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1, FALSE, &is_sent);
      if( len_written == ((DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_reg_type)) + userpkt_len) )
      {
        diagpkt_table_fwd_status[diagpkt_fwd_tbl_index] = DIAGPKT_TBL_FWD_STATUS_QUEUED;
      }
      else
      {
        /* Unable to write packet; Just quit */
        break;
      }

      if( is_sent )
      {
        for( start_index=start_index; start_index < diagpkt_fwd_tbl_index; start_index++ )
        {
          diagpkt_table_fwd_status[start_index] = DIAGPKT_TBL_FWD_STATUS_SENT;
        }
      }
    }
    else
    {
      /* No more tables to fwd; Send the current DSM item immediately and then quit */
      diagcomm_ctrl_send_mult_pkt( NULL, 0, NULL, 0, DIAGCOMM_PORT_SIO,
                                   DIAGCOMM_PORT_1, TRUE, &is_sent);
      if( is_sent )
      {
        for( start_index=start_index; start_index < diagpkt_fwd_tbl_index; start_index++ )
        {
          diagpkt_table_fwd_status[start_index] = DIAGPKT_TBL_FWD_STATUS_SENT;
        }
      }
      break;
    }
  }

} /* diagpkt_fwd_cmd_reg */
#endif /* !DIAG_MP_MASTER */


/*===========================================================================

FUNCTION DIAGPKT_CTRL_GET_MSG_TYPE

DESCRIPTION
  This function returns the ctrl msg type in the specified diag ctrl packet.

PARAMETERS
  ptr - Pointer to a Diag control packet

RETURN VALUE
  The control packet's msg type value

===========================================================================*/
uint32
diagpkt_ctrl_get_msg_type( PACK(void *) ptr )
{
  /* Diag command codes are the first byte */
  return *((uint32 *) ptr);

} /* diagpkt_ctrl_get_msg_type */


/*===========================================================================

FUNCTION DIAGPKT_PROCESS_CTRL_MSG

DESCRIPTION
  Processes a buffer of CTRL messages received, based on the type of CTRL msg.
  Various CTRL msg types can be in a single buffer (ie: 'ctrl_pkt'), but
  the buffer must contain full CTRL messages.

PARAMETERS
  ctrl_pkt - A buffer of CTRL msgs received
  ctrl_pkt_len - Length of 'ctrl_pkt'
  ctrl_channel - ctrl channel where the DSM was received from

RETURN VALUE
  None

===========================================================================*/
void
diagpkt_process_ctrl_msg( PACK(void *)ctrl_pkt, uint32 ctrl_pkt_len,
                          diagcomm_port_type port_type,
                          diagcomm_enum_port_type ctrl_channel )
{
  uint32 data_pkt_len = 0;
  uint8 * tmp_ptr = NULL;
  diag_ctrl_msg_type * msg_pkt_ptr = NULL;

#if defined(DIAG_MP_MASTER)
  uint8 slave_tbl_index = 0;
  uint32 user_pkt_len = 0, src_length = 0;
  uint8 * tmp_user_ptr = NULL;
  uint8 proc_port_id = 0;
  uint8 port_num = 0;
  int i = 0; 
  int k = 0;
  boolean found = FALSE;
#endif
   #if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
    boolean is_sent = FALSE;
   #endif

  msg_pkt_ptr = (diag_ctrl_msg_type *)ctrl_pkt;
  data_pkt_len = ctrl_pkt_len;

  while( (msg_pkt_ptr != NULL) && (data_pkt_len > 0) )
  {
    /* Ensure enough bytes were recieved to read the CTRL msg header */
    ASSERT( data_pkt_len >= DIAGCOMM_CTRL_MSG_HDR_SIZE );

    /* Ensure enough bytes were recieved to read the CTRL msg data */
    ASSERT( (data_pkt_len - DIAGCOMM_CTRL_MSG_HDR_SIZE) >= msg_pkt_ptr->data_len );

    switch( msg_pkt_ptr->cmd_type )
    {
#if defined(DIAG_MP_MASTER)
      case DIAG_CTRL_MSG_REG:
      {
        proc_port_id = ctrl_channel;
        ASSERT ( proc_port_id < NUM_SMD_PORTS );

        /* Slave Table Indices point to current available location in the slave
           table for a particualr port/processor */
        slave_tbl_index = diagpkt_slave_tbl_indices[proc_port_id];

        /* Slave Table Index shouldn't go beyond the Master Table Size */
        ASSERT ( slave_tbl_index < DIAGPKT_MSTR_TBL_SIZE);

        /*do not allocate a slave table entry if there are zero entries*/
        if (msg_pkt_ptr->data.ctrl_msg_reg.count == 0)
        {
          break;
        }

		osal_lock_mutex(&diagpkt_slave_tbl_mutex);
        if( diagpkt_slave_table[proc_port_id][slave_tbl_index] == NULL )
        {
          diagpkt_slave_table[proc_port_id][slave_tbl_index] = (diagpkt_slave_table_type *)
                                      malloc(sizeof(diagpkt_slave_table_type));
          if( diagpkt_slave_table[proc_port_id][slave_tbl_index] != NULL )
          {
            diagpkt_slave_table[proc_port_id][slave_tbl_index]->mstr_data.user_table = NULL;
            diagpkt_slave_table[proc_port_id][slave_tbl_index]->mstr_data.delay_flag = 0;
            diagpkt_slave_table[proc_port_id][slave_tbl_index]->mstr_data.cmd_code = msg_pkt_ptr->data.ctrl_msg_reg.cmd_code;
            diagpkt_slave_table[proc_port_id][slave_tbl_index]->mstr_data.subsysid = msg_pkt_ptr->data.ctrl_msg_reg.subsysid;
            diagpkt_slave_table[proc_port_id][slave_tbl_index]->mstr_data.count = msg_pkt_ptr->data.ctrl_msg_reg.count;
            diagpkt_slave_table[proc_port_id][slave_tbl_index]->mstr_data.proc_id = 0;
            diagpkt_slave_table[proc_port_id][slave_tbl_index]->mstr_data.port = msg_pkt_ptr->data.ctrl_msg_reg.port;
            diagpkt_slave_table[proc_port_id][slave_tbl_index]->mstr_data.is_fwded = 0;
            diagpkt_slave_table[proc_port_id][slave_tbl_index]->channel = ctrl_channel;

            user_pkt_len = diagpkt_slave_table[proc_port_id][slave_tbl_index]->mstr_data.count * sizeof(diagpkt_user_table_entry_type);

            if( (diagpkt_slave_table[proc_port_id][slave_tbl_index]->mstr_data.user_table == NULL) && (user_pkt_len > 0) )
            {
              diagpkt_slave_table[proc_port_id][slave_tbl_index]->mstr_data.user_table = (diagpkt_user_table_entry_type *)
                 malloc(user_pkt_len);
            }

            if( diagpkt_slave_table[proc_port_id][slave_tbl_index]->mstr_data.user_table != NULL )
            {
              tmp_ptr = ((uint8 *)msg_pkt_ptr) + DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_reg_type);
              tmp_user_ptr = (uint8 *)diagpkt_slave_table[proc_port_id][slave_tbl_index]->mstr_data.user_table;
			  src_length = diagpkt_slave_table[proc_port_id][slave_tbl_index]->mstr_data.count * sizeof(diagpkt_slave_user_table_entry_type);
			  
			  ASSERT(src_length == (msg_pkt_ptr->data_len - sizeof(diag_ctrl_msg_reg_type)));

			  /* Copy user tables sent by slave one by one to diagpkt_slave_table */
			  while(user_pkt_len > 0)
			  {
				memscpy((void *)tmp_user_ptr, sizeof(diagpkt_slave_user_table_entry_type),tmp_ptr, sizeof(diagpkt_slave_user_table_entry_type));
				tmp_user_ptr += sizeof(diagpkt_user_table_entry_type);
				tmp_ptr += sizeof(diagpkt_slave_user_table_entry_type);
				user_pkt_len -= sizeof(diagpkt_user_table_entry_type);
			  }
              if( diagpkt_check_for_polling_cmd(&(diagpkt_slave_table[proc_port_id][slave_tbl_index]->mstr_data)) == TRUE )
              {
                is_polling_cmd_registered[proc_port_id] = TRUE;
              }
            }
            else
            {
              MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Slave user table malloc failed for port=%d, index=%d", proc_port_id, slave_tbl_index);
            }
            diagpkt_slave_tbl_indices[proc_port_id]++;   /* Incrementing the slave table index on successful malloc */
          }
          else
          {
            MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Slave table malloc failed for port=%d, index=%d", proc_port_id, slave_tbl_index);
          }
        }
		osal_unlock_mutex(&diagpkt_slave_tbl_mutex);
        break;
      } /* DIAG_CTRL_MSG_REG */
      case DIAG_CTRL_MSG_DEREG:
	  {	
		proc_port_id = ctrl_channel;
		ASSERT ( proc_port_id < NUM_SMD_PORTS );
		found = FALSE;

		osal_lock_mutex(&diagpkt_slave_tbl_mutex);
		for (i=0; i < DIAGPKT_MSTR_TBL_SIZE; i++)
		{
		  if (diagpkt_slave_table[proc_port_id][i] == NULL)
		  	break;
		  
		  if((diagpkt_slave_table[proc_port_id][i]->mstr_data.cmd_code == msg_pkt_ptr->data.ctrl_msg_dereg.cmd_code)
			&& (diagpkt_slave_table[proc_port_id][i]->mstr_data.subsysid == msg_pkt_ptr->data.ctrl_msg_dereg.subsysid)
		    && (diagpkt_slave_table[proc_port_id][i]->mstr_data.user_table->cmd_code_lo == msg_pkt_ptr->data.ctrl_msg_dereg.cmd_code_lo)
			&& (diagpkt_slave_table[proc_port_id][i]->mstr_data.user_table->cmd_code_hi == msg_pkt_ptr->data.ctrl_msg_dereg.cmd_code_hi))
		  {
            /* If a polling command in slave is getting deregistered, is_polling_cmd_registered should be
            set to false, so that APPS responds to 4B 32 03 polling command. */
            if(diagpkt_check_for_polling_cmd(&(diagpkt_slave_table[proc_port_id][i]->mstr_data)) == TRUE)
            {
              is_polling_cmd_registered[proc_port_id] = FALSE;
            }
            found = TRUE;
            break;
		  }
		}
		if(found == TRUE)
		{
        // Frees this entry if this is the last entry, else copy last valid registration table after this entry to this location
		for(k = DIAGPKT_MSTR_TBL_SIZE-1; ; k--)
		{
		  if(diagpkt_slave_table[proc_port_id][k] != NULL)
		  {
		    free((void*)diagpkt_slave_table[proc_port_id][i]->mstr_data.user_table);

			if (i != k)
			  memscpy(diagpkt_slave_table[proc_port_id][i], sizeof(diagpkt_slave_table_type), diagpkt_slave_table[proc_port_id][k], 
		      sizeof(diagpkt_slave_table_type));

            diagpkt_slave_table[proc_port_id][k]->mstr_data.user_table = NULL;
			free(diagpkt_slave_table[proc_port_id][k]);
		    diagpkt_slave_table[proc_port_id][k] = NULL;
			break;
		  }
		}
		diagpkt_slave_tbl_indices[proc_port_id]--;
		}
		osal_unlock_mutex(&diagpkt_slave_tbl_mutex);
		break;
      }
      case DIAG_CTRL_BT_MSG_RANGE:
      case DIAG_CTRL_BT_MSG_MASK:
      {
        diag_ctrl_update_msg_bt_mask( msg_pkt_ptr, data_pkt_len );
        break;
      } /* DIAG_CTRL_BT_MSG_MASK | DIAG_CTRL_BT_MSG_RANGE */

      case DIAG_CTRL_BT_LOG_RANGE:
      {
        diag_ctrl_update_log_range( msg_pkt_ptr, data_pkt_len );
        break;
      }

      case DIAG_CTRL_BT_EVENT_ID:
      {
        diag_ctrl_update_event_last( msg_pkt_ptr, data_pkt_len );
        break;
      }
#endif /* DIAG_MP_MASTER */

      case DIAG_CTRL_MSG_DTR:
      {
        // Process DTR event msgs here
        break;
      }

    case DIAG_CTRL_MSG_DIAGMODE:
    {

      if( msg_pkt_ptr->data_len == sizeof(diag_ctrl_msg_diagmode_type) )
      {
        diag_control_sleep_voting(msg_pkt_ptr->data.ctrl_msg_diagmode.sleep_vote);
        /* OK to sleep, if sleep-voting is disallowed.*/
        if( msg_pkt_ptr->data.ctrl_msg_diagmode.sleep_vote == 0 )
        {
          diag_allow_sleep();
        }

        if( msg_pkt_ptr->data.ctrl_msg_diagmode.use_nrt_values == 1 )
        {
          diagbuf_ctrl_tx_sleep_parameters(msg_pkt_ptr->data.ctrl_msg_diagmode.sleep_threshold,
                                           msg_pkt_ptr->data.ctrl_msg_diagmode.sleep_time);
          diagbuf_ctrl_commit_threshold(msg_pkt_ptr->data.ctrl_msg_diagmode.commit_threshold);
          diag_ctrl_drain_timer_len(msg_pkt_ptr->data.ctrl_msg_diagmode.drain_timer_val);
          event_ctrl_stale_timer(msg_pkt_ptr->data.ctrl_msg_diagmode.event_stale_timer_val);
          event_ctrl_report_size(EVENT_RPT_PKT_SIZE_NRT, 1024, DIAGBUF_SIO_TX_ITEM_SIZ - DIAG_HDLC_PAD_LEN);
        }
        else
        {
          /* If real-time mode is disabled, look at the different tuning parameters */
          if( msg_pkt_ptr->data.ctrl_msg_diagmode.real_time == 0 )
          {
            diagbuf_ctrl_tx_sleep_parameters(DIAG_TX_SLEEP_THRESHOLD_NRT, DIAG_TX_SLEEP_TIME_NRT);
            diagbuf_ctrl_commit_threshold(DIAGBUF_COMMIT_THRESHOLD_NRT);
            diag_ctrl_drain_timer_len(DIAG_DRAIN_TIMER_LEN_NRT);
            event_ctrl_stale_timer(EVENT_STALE_TIMER_LEN_NRT);
            event_ctrl_report_size(EVENT_RPT_PKT_SIZE_NRT, 1024, DIAGBUF_SIO_TX_ITEM_SIZ - DIAG_HDLC_PAD_LEN);
          }
          else if( msg_pkt_ptr->data.ctrl_msg_diagmode.real_time == 1 )
          {
            diagbuf_ctrl_tx_sleep_parameters(DIAG_TX_SLEEP_THRESHOLD_DEFAULT, DIAG_TX_SLEEP_TIME_DEFAULT);
            diagbuf_ctrl_commit_threshold(DIAGBUF_COMMIT_THRESHOLD);
            diag_ctrl_drain_timer_len(DIAG_DRAIN_TIMER_LEN);
            event_ctrl_stale_timer(EVENT_TIMER_LEN);
            event_ctrl_report_size(EVENT_RPT_PKT_SIZE, EVENT_SEND_MAX, EVENT_RPT_PKT_LEN_SIZE);
          }
        }
     #if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
              /* Send the control message to all the peripherals */
        for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
         (void)diagcomm_ctrl_send_mult_pkt((PACK(void *))msg_pkt_ptr,
                                            DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_diagmode_type),
                                            NULL, 0, DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num, TRUE, &is_sent);
     #endif
      }
      break;
    } /* DIAG_CTRL_MSG_DIAGMODE */
#if defined FEATURE_DIAG_STM
      case DIAG_CTRL_MSG_STM_PKT:
      {

        if( msg_pkt_ptr->data_len == sizeof(diag_ctrl_msg_diag_stm_pkt_type) )
        {
          if(msg_pkt_ptr->data.ctrl_msg_stm_mode.version == DIAG_CTRL_MSG_DIAG_STM_VER)
          {
            diag_stm_control_mode(msg_pkt_ptr->data.ctrl_msg_stm_mode.ctrl_mode);
          }
          else
          {
            MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid version % for ctrl packet %d",
                  msg_pkt_ptr->data.ctrl_msg_stm_mode.version, DIAG_CTRL_MSG_STM_PKT);
          }
        }
        else
        {
          MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Data length is wrong for DIAG_CTRL_MSG_STM_PKT (received %d, expected %d)",msg_pkt_ptr->data_len,sizeof(diag_ctrl_msg_diag_stm_pkt_type));
        }
      }
      break;
#endif

#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
      case DIAG_CTRL_MSG_EQUIP_LOG_MASK:
      {
        diag_ctrl_update_log_mask( msg_pkt_ptr );
        break;
      } /* DIAG_CTRL_MSG_EQUIP_LOG_MASK */

      case DIAG_CTRL_MSG_EQUIP_LOG_MASK_V3:
      {
        diag_ctrl_update_log_preset_mask( msg_pkt_ptr );
        break;
      } /* DIAG_CTRL_MSG_EQUIP_LOG_MASK_V3 */

      case DIAG_CTRL_MSG_EVENT_MASK_V2:
      {
        diag_ctrl_update_event_mask( msg_pkt_ptr );
        break;
      } /* DIAG_CTRL_MSG_EVENT_MASK_V2 */

      case DIAG_CTRL_MSG_EVENT_MASK_V3:
      {
        diag_ctrl_update_event_preset_mask( msg_pkt_ptr );
        break;
      } /* DIAG_CTRL_MSG_EVENT_MASK_V3 */

      case DIAG_CTRL_MSG_F3_MASK_V2:
      {
        diag_ctrl_update_msg_mask( msg_pkt_ptr );
        break;
      } /* DIAG_CTRL_MSG_F3_MASK_V2 */

      case DIAG_CTRL_MSG_F3_MASK_V3:
      {
        diag_ctrl_update_msg_preset_mask( msg_pkt_ptr );
        break;
      } /* DIAG_CTRL_MSG_F3_MASK_V3 */

#endif /* DIAG_MP && !DIAG_MP_MASTER */

      case DIAG_CTRL_MSG_FEATURE:
      {
        uint32 len = 0;
        int hdlc_feature_mask = 0;
        int hdlc_feature_mask_apss = 0;
        len = MIN(msg_pkt_ptr->data.ctrl_msg_feature.mask_len, DIAG_INT_FEATURE_MASK_LEN);
        memscpy((void*)&diagcomm_io_ctrl_conn[port_type][ctrl_channel].feature_mask,
                DIAG_INT_FEATURE_MASK_LEN,
               (void*)&msg_pkt_ptr->data.ctrl_msg_feature.mask[0], len);
        hdlc_feature_mask = DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[port_type][ctrl_channel].feature_mask, F_DIAG_HDLC_ENCODE_IN_APPS);
        hdlc_feature_mask_apss = DIAG_INT_FEATURE_MASK_CHECK(F_DIAG_HDLC_ENCODE_IN_APPS);
        if(bIsHdlcEncoded == TRUE)
        {
          if( hdlc_feature_mask_apss && hdlc_feature_mask)
          {
             bIsHdlcEncoded = FALSE;
          }
        }
        /* Set signal to send Diag feature flag mask to remote procs */
        (void)diag_set_internal_sigs( DIAG_INT_FEAT_UPDATE_SIG );
        (void)diag_set_internal_sigs( DIAG_INT_MASK_UPDATE_SIG );

        // Opening command channel and dci command channel on receiving feature mask from modem
        diagcomm_cmd_open (DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_1);
        #ifdef DIAG_CONSUMER_API
          diagcomm_dci_cmd_open(DIAGCOMM_PORT_1);
        #endif /* DIAG_CONSUMER_API */
        break;
      }

#if defined(DIAG_MP_MASTER)
      case DIAG_CTRL_MSG_NUM_PRESETS:
      {
        /* Master is saving the max # of preset masks each Slave supports. */
        diagcomm_io_ctrl_conn[port_type][ctrl_channel].num_presets = msg_pkt_ptr->data.ctrl_msg_num_presets.num_presets;
        break;
      }
#endif /* DIAG_MP_MASTER */

#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
      case DIAG_CTRL_MSG_SET_CUR_PRESET:
      {
        /* Master is telling Slaves to switch the preset mask in use. */
        if( (msg_pkt_ptr->data.cur_preset.preset_id == 0) ||
            (msg_pkt_ptr->data.cur_preset.preset_id > DIAG_MAX_PRESET_ID) )
        {
          MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagpkt_process_ctrl_msg: Invalid preset_id %d for ctrl msg %d",
                msg_pkt_ptr->data.cur_preset.preset_id, msg_pkt_ptr->cmd_type);
          break;
        }

        diag_cur_preset_id = msg_pkt_ptr->data.cur_preset.preset_id;
        diag_switch_event_preset_mask();
        diag_switch_log_preset_mask();
        diag_switch_msg_preset_mask();
        break;
      }
#endif

      default:
      {
        MSG_1 (MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagpkt_process_ctrl_msg: Invalid msg type %d", msg_pkt_ptr->cmd_type);
        break;
      }
    } // end switch

    /* Calculate # of bytes left to process */
    data_pkt_len -= (DIAGCOMM_CTRL_MSG_HDR_SIZE + msg_pkt_ptr->data_len);

    /* Find position of next CTRL packet to process */
    tmp_ptr = (uint8 *)ctrl_pkt;
    tmp_ptr += (ctrl_pkt_len - data_pkt_len);
    msg_pkt_ptr = (diag_ctrl_msg_type *)tmp_ptr;

  } // while( (msg_pkt_ptr != NULL) && (data_pkt_len > 0) )

} /* diagpkt_process_ctrl_msg */


/*===========================================================================

FUNCTION DIAGPKT_WITH_DELAY

DESCRIPTION
  This procedure checks if the diagnostic packet has been registered with or
  without delay.
Note: Return value is 0 if Diag packet has no delayed response and 1 if Diag
      packet has delayed response

===========================================================================*/
uint32 diagpkt_with_delay (diagpkt_subsys_id_type id,
                           diagpkt_subsys_cmd_code_type code)
{
  uint16 packet_id = code;      /* Command code for std or subsystem */
  uint8 subsys_id = id;
  const diagpkt_master_table_type *mstr_tbl_entry = NULL;
  const diagpkt_user_table_entry_type *tbl_entry = NULL;
  int tbl_entry_count = 0;
  int i, j;
  boolean found = FALSE;
  uint32 delay_flag = 0;

  /* Search the dispatch table for a matching subsystem ID.  If the
     subsystem ID matches, search that table for an entry for the given
     command code. */
  for (i = 0; !found && i < DIAGPKT_MSTR_TBL_SIZE; i++)
  {
    mstr_tbl_entry = diagpkt_master_table[i];

    if (mstr_tbl_entry != NULL && mstr_tbl_entry->subsysid == subsys_id)
    {
      tbl_entry = mstr_tbl_entry->user_table;

      delay_flag = mstr_tbl_entry->delay_flag;

      tbl_entry_count = (tbl_entry) ? mstr_tbl_entry->count : 0;

      for (j = 0; (tbl_entry!=NULL) && !found && j < tbl_entry_count; j++)
      {
        if (packet_id >= tbl_entry->cmd_code_lo &&
            packet_id <= tbl_entry->cmd_code_hi)
        {
          /* If the entry has no func, ignore it. */
          found = TRUE;
        }
        tbl_entry++;
      }
    }
  }

  return delay_flag;
}               /* diagpkt_with_delay */

/*===========================================================================

FUNCTION DIAGPKT_DELAY_ALLOC

DESCRIPTION
  This function allocates the specified amount of space in AMSS heap.

===========================================================================*/
PACK(void *)
diagpkt_delay_alloc(
  diagpkt_cmd_code_type code,
  unsigned int length)
{
   PACK(void *) ptr = NULL; /* Pointer to return. */

  unsigned int size = 0;
  diagpkt_delay_q_type *item = NULL;
  PACK(uint16 *) pattern = NULL;    /* Overrun pattern. */
  unsigned char *p = NULL;

  if (length > DIAG_MAX_TX_PKT_SIZ) {
    diagpkt_delay_drop_cnt ++;
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"Attempt to alloc too much: %d", (dword) length);
    return NULL;
  }

  size =     FPOS(diagpkt_delay_q_type,pkt) +  /* Diagpkt header    */
             (uint32)length     +  /* Number of bytes requested buffer */
             sizeof(uint16) ;   /* buffer overrun pattern */

  size = DIAGBUF_NEXT_ALIGNED_BYTE(size);
  item = (diagpkt_delay_q_type *)malloc(size);

  if(item)
  {
      diagpkt_delay_alloc_cnt++;

      item->pattern = DIAGPKT_HDR_PATTERN;    /* Sanity check pattern */
      item->length = length;

      pattern = (PACK(uint16 *)) ((&item->pkt)+length) ;

      p = (unsigned char *) pattern;
      p[0] = (DIAGPKT_OVERRUN_PATTERN >> 8) & 0xff;
      p[1] = (DIAGPKT_OVERRUN_PATTERN >> 0) & 0xff;

      ptr = &item->pkt;
  }
  else
  {
    diagpkt_delay_drop_cnt ++;
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"diagpkt_delay_drop_cnt =%d", diagpkt_delay_drop_cnt);
  }
  return ptr;
} /* diagpkt_delay_alloc */

/* -------------------------------------------------------------------------
** Externalized functions
** ------------------------------------------------------------------------- */

/*===========================================================================

FUNCTION DIAGPKT_GET_CMD_CODE

DESCRIPTION
  This function returns the command code in the specified diag packet.

===========================================================================*/
diagpkt_cmd_code_type
diagpkt_get_cmd_code (PACK(void *) ptr)
{
  /* Diag command codes are the first byte */
  return *((diagpkt_cmd_code_type *) ptr);
}               /* diag_get_cmd_code */

/*===========================================================================

FUNCTION DIAGPKT_SET_CMD_CODE

DESCRIPTION
  This function sets the command code in the specified diag packet.

===========================================================================*/
void
diagpkt_set_cmd_code (PACK(void *) ptr, diagpkt_cmd_code_type cmd_code)
{
  *((diagpkt_cmd_code_type *) ptr) = cmd_code;
}               /* diagpkt_set_cmd_code */

/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_GET_ID

DESCRIPTION
  This function returns the subsystem ID in the specified diag packet.

  If the packet is not a DIAG_SUBSYS_CMD_F or DIAG_SUBSYS_CMD_VER_2_F packet,
  0xFFFF is returned.

===========================================================================*/
diagpkt_subsys_id_type
diagpkt_subsys_get_id (PACK(void *) ptr)
{
  diagpkt_subsys_id_type id = 0xFF;   // Initializing id to Legacy Subsystem value
  diagpkt_subsys_hdr_type *pkt_ptr = (void *) ptr;

  if( ptr != NULL )
  {
    if ((pkt_ptr->command_code == DIAG_SUBSYS_CMD_F) || (pkt_ptr->command_code
        == DIAG_SUBSYS_CMD_VER_2_F))
    {
      id = (diagpkt_subsys_id_type) pkt_ptr->subsys_id;
    }
  }
  return id;
}               /* diagpkt_subsys_get_id */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_GET_CMD_CODE

DESCRIPTION
  This function returns the subsystem command code in the specified
  diag packet.

  If the packet is not a DIAG_SUBSYS_CMD_F or DIAG_SUBSYS_CMD_VER_2_F packet,
  0xFFFF is returned.

===========================================================================*/
diagpkt_subsys_cmd_code_type
diagpkt_subsys_get_cmd_code (PACK(void *) ptr)
{
  diagpkt_subsys_cmd_code_type code = 0xFFFF;   // Initalizing cmd code to an out of range value greater than the max value possible
  diagpkt_subsys_hdr_type *pkt_ptr = (void *) ptr;

  if( ptr != NULL )
  {
    if ((pkt_ptr->command_code == DIAG_SUBSYS_CMD_F) || (pkt_ptr->command_code
        == DIAG_SUBSYS_CMD_VER_2_F))
    {
      code = pkt_ptr->subsys_cmd_code;
    }
  }
  return code;
}               /* diagpkt_subsys_get_cmd_code */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_GET_STATUS

DESCRIPTION
  This function gets the status field in the DIAG_SUBSYS_CMD_VER_2_F packet

  This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
  packet.

===========================================================================*/
diagpkt_subsys_status_type
diagpkt_subsys_get_status (PACK(void *) ptr)
{
  diagpkt_subsys_hdr_type_v2 *pkt_ptr = (void *) ptr;

  ASSERT (pkt_ptr != NULL);
  ASSERT (pkt_ptr->command_code == DIAG_SUBSYS_CMD_VER_2_F);

  return pkt_ptr->status;
}               /* diagpkt_subsys_get_status */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_SET_STATUS

DESCRIPTION
  This function sets the status field in the DIAG_SUBSYS_CMD_VER_2_F packet.

  This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
  packet.

===========================================================================*/
void
diagpkt_subsys_set_status (PACK(void *) ptr, diagpkt_subsys_status_type status)
{
  diagpkt_subsys_hdr_type_v2 *pkt_ptr = (void *) ptr;

  ASSERT (pkt_ptr != NULL);
  ASSERT (pkt_ptr->command_code == DIAG_SUBSYS_CMD_VER_2_F);

  pkt_ptr->status = status;
}               /* diagpkt_subsys_set_status */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_GET_DELAYED_RSP_ID

DESCRIPTION
  This function gets the delayed response ID field in the
  DIAG_SUBSYS_CMD_VER_2_F packet.

  This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
  packet.

===========================================================================*/
diagpkt_subsys_delayed_rsp_id_type
diagpkt_subsys_get_delayed_rsp_id (PACK(void *) ptr)
{
  diagpkt_subsys_hdr_type_v2 *pkt_ptr = (void *) ptr;

  ASSERT (pkt_ptr != NULL);
  ASSERT (pkt_ptr->command_code == DIAG_SUBSYS_CMD_VER_2_F);

  return pkt_ptr->delayed_rsp_id;
}               /* diagpkt_subsys_get_delayed_rsp_id */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_RESET_DELAYED_RSP_ID

DESCRIPTION
  This function sets the delayed response ID to zero in the
  DIAG_SUBSYS_CMD_VER_2_F packet.

  This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
  packet.

===========================================================================*/
void
diagpkt_subsys_reset_delayed_rsp_id (PACK(void *) ptr)
{
  diagpkt_subsys_hdr_type_v2 *pkt_ptr = (void *) ptr;

  ASSERT (pkt_ptr != NULL);
  ASSERT (pkt_ptr->command_code == DIAG_SUBSYS_CMD_VER_2_F);

  pkt_ptr->delayed_rsp_id = 0;
}               /* diagpkt_subsys_reset_delayed_rsp_id */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_SET_RSP_CNT

DESCRIPTION
  This function sets the response count in the DIAG_SUBSYS_CMD_VER_2_F packet.

  This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
  packet.

===========================================================================*/
void
diagpkt_subsys_set_rsp_cnt (PACK(void *) ptr, diagpkt_subsys_rsp_cnt rsp_cnt)
{
  diagpkt_subsys_hdr_type_v2 *pkt_ptr = (void *) ptr;

  ASSERT (pkt_ptr != NULL);
  ASSERT (pkt_ptr->command_code == DIAG_SUBSYS_CMD_VER_2_F);

  pkt_ptr->rsp_cnt = rsp_cnt;
}               /* diagpkt_subsys_set_rsp_cnt */


/*==========================================================================*/
/* This is found in diag.c */
void *diag_rsp_malloc (uint32 num_bytes);

PACK(void *)
diagpkt_alloc (diagpkt_cmd_code_type code, unsigned int length)
{
  osal_sigs_t return_sigs;
  osal_sigs_t sigs = 0;
  diagpkt_q_type *item = NULL;
  diagpkt_hdr_type *pkt = NULL;
  PACK(uint16 *) pattern = NULL;    /* Overrun pattern. */
  unsigned char *p;
  unsigned int size = FPOS (diagpkt_q_type, rsp.pkt) + length + sizeof (uint16);
  int return_val = 0;

  if (size > ( DIAGPKT_RSP_LEN_MAX))
  {
    diagpkt_drop_cnt++ ;
    return NULL;
  }

  if (osal_thread_self () == &diag_task_tcb)
  {
    /*-----------------------------------------------
      Try to allocate a buffer.  Size of buffer must
      include space for overhead and CRC at the end.
    -----------------------------------------------*/
    while ((item = (diagpkt_q_type *) diag_rsp_malloc (size)) == NULL)
    {
      /* Alloc not successful.  Block and retry until successful. */

      /*-----------------------------------------------------------
        It is possible that we could be unable to drain due to a
        lower priority task that has not committed its buffer.

        "Not ready" can also mean that flow control was
        asserterd.  Wait for flow control to allow more TX.

        If the last attempt to drain resulted in data not ready,
        then we should wait until another task commits a buffer or
        flow control is
      -----------------------------------------------------------*/
      sigs = osal_thread_wait(&diag_task_tcb , DIAG_RX_RSP_SIG | DIAG_RPT_TIMER_SIG);

#ifdef FEATURE_DEBUG_DIAG_HEAP
      if( (get_rsp_q_cnt() == 0) &&
          (diag_heap_used_bytes() !=0) )
      {
         ERR_FATAL("Diag Heap Corruption Used = %d",diag_heap_used_bytes(),0,0);
      }
#endif /* FEATURE_DEBUG_DIAG_HEAP */

      /* Kick the dog */
      diag_kick_watchdog ();

      if (sigs & DIAG_RX_RSP_SIG)
      {
        return_val = osal_reset_sigs(&diag_task_tcb, DIAG_RX_RSP_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
        if (diagcomm_status())
          diagpkt_rsp_send();
      }
    }               /* while not getting space */
  }

  if (item)
  {
#ifdef FEATURE_DEBUG_DIAG_HEAP
    diagpkt_alloc_cnt++;
    MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "BLOCKING MALLOC diagpkt_alloc = %d heapUsed = %d CmdCode = %d",
        diagpkt_alloc_cnt, diag_heap_used_bytes(),code);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    /* This pattern is written to verify pointers elsewhere in this
       service  are valid. */
    item->rsp.pattern = DIAGPKT_HDR_PATTERN;    /* Sanity check pattern */

    /* This should always be pending.  Just in case, check it. */
    if (diagpkt_rsp_func_store.pending)
    {
      /* Clear the cmd connection mask */
      DIAG_CLR_MASK (diag_cx_state, DIAG_CX_CMD_S);

      item->rsp_func = diagpkt_rsp_func_store.rsp_func;
      item->rsp_func_param = diagpkt_rsp_func_store.rsp_func_param;
    }
    else
    {
      item->rsp_func = NULL;
      item->rsp_func_param = NULL;
    }

    /* In either case, go ahead and clear the stored rsp_func info. */
    diagpkt_rsp_func_store.pending = FALSE;
    diagpkt_rsp_func_store.rsp_func = NULL;
    diagpkt_rsp_func_store.rsp_func_param = NULL;

    /* length ==  size unless packet is resized later */
    item->rsp.size = length;
    item->rsp.length = length;

    pattern = (PACK(uint16 *)) ((&item->rsp.pkt)+length);

    /* We need this to meet alignment requirements - MATS */
    p = (unsigned char *) pattern;
    p[0] = (DIAGPKT_OVERRUN_PATTERN >> 8) & 0xff;
    p[1] = (DIAGPKT_OVERRUN_PATTERN >> 0) & 0xff;
/*lint -save -e{545,740} Suspicious use of & Unusual pointer cast*/
    pkt = (diagpkt_hdr_type *) & item->rsp.pkt;
/*lint -restore */
  }

  if (pkt)
  {
    pkt->command_code = code;
  }
  return (PACK(void *)) pkt;

}               /* diagpkt_alloc */


/*===========================================================================

FUNCTION DIAGPKT_DELAY_COMMIT

DESCRIPTION
  This function commits previously allocated space in the AMSS heap.

===========================================================================*/
void
diagpkt_delay_commit (PACK(void *) pkt)
{
    diagpkt_delay_q_type *item = NULL;
    PACK(uint16 *) pattern;
    unsigned char *p;

    if (pkt)
    {
        item = DIAGPKT_PKT2DELAYQITEM(pkt);
        /* If pattern has been inverted, this is a double-commit.  Ignore. */
        if (item->pattern != ~DIAGPKT_HDR_PATTERN)
        {
            if (item->pattern == DIAGPKT_HDR_PATTERN)
            {
                /* Invert the header pattern to mark that the packet has been
                processed.  This handles the incorrect usage of committing twice. */
                item->pattern = ~DIAGPKT_HDR_PATTERN;

                /* The 2 byte CRC field at the end of the allocated buffer is written
                * with a mask to detect buffer overrun.  Check to make sure that value
                * wasn't corrupted.
                */
                pattern = (PACK(uint16 *)) ((&item->pkt)+(item->length));

                /* This is safer - MATS */
                p = (unsigned char *) pattern;
                if(p[0] == ((DIAGPKT_OVERRUN_PATTERN >> 8) & 0xff)  &&
                   p[1] == ((DIAGPKT_OVERRUN_PATTERN >> 0) & 0xff))
                {
                    if (item->length > 0)
                    {
                        /* The pattern check was successful, go ahead and queue the packet. */
                        q_put (&diagpkt_delay_rsp_q, q_link (item, &item->qlink));

                        /* Notify the diag task so the queue can be serviced. */
                        diag_rx_delay_rsp_notify();
                    }
                    else
                    {
                        free (item);

                        #ifdef FEATURE_DEBUG_DIAG_HEAP
                        diagpkt_delay_commit_free_cnt++;
                        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_FATAL,"FREE diagpkt_delay_commit_free_cnt = %d",
                              diagpkt_delay_commit_free_cnt);
                        #endif /* FEATURE_DEBUG_DIAG_HEAP */
                    }
                }
                else
                {
                    ERR_FATAL ("Buffer overrun on packet at 0x%x", (uint32) pkt, 0, 0);
                }
            }
            else
            {
                ERR_FATAL ("Invalid pointer used for diag buffering", 0, 0, 0);
            }
        }
    }
  return;
}               /* diagpkt_delay_commit */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_ALLOC

DESCRIPTION
  This function returns the command code in the specified diag packet.

===========================================================================*/
PACK(void *)
diagpkt_subsys_alloc (diagpkt_subsys_id_type id,
              diagpkt_subsys_cmd_code_type code, unsigned int length)
{
  diagpkt_subsys_hdr_type *hdr = NULL;

  ASSERT (osal_thread_self () == &diag_task_tcb);

  hdr = (diagpkt_subsys_hdr_type *) diagpkt_alloc (DIAG_SUBSYS_CMD_F, length);

  if( hdr != NULL )
  {
    hdr->subsys_id = id;
    hdr->subsys_cmd_code = code;

#ifdef FEATURE_DEBUG_DIAG_HEAP
    diagpkt_subsys_alloc_cnt++;
    MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "BLOCKING MALLOC subsys_alloc_cnt = %d SSID = %d SS_CODE = %d",
        diagpkt_subsys_alloc_cnt, id,code);
    if( (id == 19) && (code == 3))
    {
      diagpkt_subsys_alloc_fs_close++;
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "Total fs_close req queued=%d",diagpkt_subsys_alloc_fs_close);
    }
#endif /* FEATURE_DEBUG_DIAG_HEAP */
  }

  return (PACK(void *)) hdr;

}               /* diagpkt_subsys_alloc */

uint16 diagpkt_next_delayed_rsp_id(uint16 rspid)
{
    uint16 temprspid = rspid;
    if (wrap_enabled==TRUE)
    {
        ((temprspid < DIAGPKT_MAX_DELAYED_RSP) ? temprspid++ : (temprspid=0));
        if(temprspid == 0)
        {
            wrap_count++;
            temprspid=1;
        }
    }
    else{
         ((temprspid < DIAGPKT_MAX_DELAYED_RSP) ? temprspid++ : DIAGPKT_MAX_DELAYED_RSP);
        }
    delayed_rsp_id = temprspid;
    return delayed_rsp_id;
}

uint16 diagpkt_get_delayed_rsp_id(void)
{
  return diagpkt_next_delayed_rsp_id(delayed_rsp_id);
}

/*===========================================================================
FUNCTION DIAGPKT_SUBSYS_ALLOC_V2

DESCRIPTION
  This function allocates the specified amount of space in the diag output
  buffer.  If space is unavailable, access to the allocation buffer will be
  blocked using a semaphore until space is available.

DEPENDENCIES
  This may only be called by the diag task for responses to explicit request
  packets!  This function is *not* re-entrant.  If the OS ever supports
  semaphores other than INTLOCK, this function can be made re-entrant.

  diagpkt_commit() must be called to commit the response packet to be sent.
  Not calling diagpkt_commit() will result in a memory leak.
===========================================================================*/
PACK(void *)
diagpkt_subsys_alloc_v2 (diagpkt_subsys_id_type id,
              diagpkt_subsys_cmd_code_type code, unsigned int length)
{
  diagpkt_subsys_hdr_type_v2 *hdr = NULL;

  ASSERT (osal_thread_self () == &diag_task_tcb);

  hdr = (diagpkt_subsys_hdr_type_v2 *) diagpkt_alloc (DIAG_SUBSYS_CMD_VER_2_F, length);

  if (hdr != NULL)
  {
    hdr->subsys_id = id;
    hdr->subsys_cmd_code = code;
    hdr->status = 0;

#ifdef FEATURE_DEBUG_DIAG_HEAP
    diagpkt_subsys_alloc_v2_cnt++;
    MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "BLOCKING MALLOC subsys_alloc_v2 = %d subsys_id = %d subsys_code = %d",
      diagpkt_subsys_alloc_v2_cnt, id,code);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    if( diagpkt_with_delay (id,code) )
    {
      hdr->delayed_rsp_id = diagpkt_next_delayed_rsp_id(delayed_rsp_id);
    }else
    {
      hdr->delayed_rsp_id = 0;
    }
    hdr->rsp_cnt = 0;
  }

  return (PACK(void *)) hdr;

}               /* diagpkt_subsys_alloc_v2 */

/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_ALLOC_V2_DELAY

DESCRIPTION
  This function allocates the specified amount of space in AMSS heap.
  This function is used to send a delayed response.

DEPENDENCIES
  diagpkt_delay_commit() must be called to commit the response packet to be
  sent. Not calling diagpkt_delay_commit() will result in a memory leak and
  response packet will not be sent.

  Note:User is required to provide delayed response id as an argument.
       This helps tools to match the delayed response with the original
       request response pair.

===========================================================================*/
PACK(void *)
diagpkt_subsys_alloc_v2_delay (diagpkt_subsys_id_type id,
              diagpkt_subsys_cmd_code_type code,
              diagpkt_subsys_delayed_rsp_id_type delayed_rsp_id_arg,
              unsigned int length)
{
  diagpkt_subsys_hdr_type_v2 *hdr = NULL;

  hdr = (diagpkt_subsys_hdr_type_v2 *) diagpkt_delay_alloc(
                 DIAG_SUBSYS_CMD_VER_2_F,
                 length);

  if(hdr != NULL)
  {
    hdr->command_code = DIAG_SUBSYS_CMD_VER_2_F;
    hdr->subsys_id = id;
    hdr->subsys_cmd_code = code;
    hdr->status = 0;
    hdr->delayed_rsp_id = delayed_rsp_id_arg;
    hdr->rsp_cnt = 1;
  }

  return (PACK(void *)) hdr;

}               /* diagpkt_subsys_alloc_v2_delay */

/*==========================================================================*/
void
diagpkt_shorten (PACK(void *) pkt, unsigned int new_length)
{

  diagpkt_q_type *item = NULL;
  PACK(uint16 *) pattern = NULL;

  if (pkt)
  {
    /* Do pointer arithmetic in bytes, then case to q_type; */
    item = DIAGPKT_PKT2QITEM (pkt);

    if (new_length < item->rsp.size)
    {
      unsigned char *p;
      item->rsp.length = new_length;

      /* Write the new buffer overrun detection pattern */
      pattern = (PACK(uint16 *)) & item->rsp.pkt[new_length];

      /* We need this to meet alignment requirements - MATS */
      p = (unsigned char *) pattern;
      p[0] = (DIAGPKT_OVERRUN_PATTERN >> 8) & 0xff;
      p[1] = (DIAGPKT_OVERRUN_PATTERN >> 0) & 0xff;
    }
    else
    {
      /* handle failure case here */
    }
  }

  return;
}               /* diagpkt_shorten */


/*=========================================================================

FUNCTION DIAGPKT_FREE

DESCRIPTION
  This function free the packet allocated by diagpkt_alloc(), which doesn't
  need to 'commit' for sending as a response if it is merely a temporary
  processing packet.

===========================================================================*/
void
diagpkt_free(PACK(void *) pkt)
{
  if (pkt)
  {
    diagpkt_q_type *item = DIAGPKT_PKT2QITEM(pkt);
    diag_rsp_free ((void *)item);
  }

  return;
}


/*==========================================================================*/
void
diagpkt_commit (PACK(void *) pkt)
{
  diagpkt_q_type *item = NULL;
  PACK(uint16 *) pattern;
  unsigned char *p;

#ifdef DIAG_CONSUMER_API_CMD_REQ
  diagpkt_subsys_hdr_type_v2 *hdr = NULL;
  diagpkt_delayed_tag_type *list_item = NULL;
#endif

  if (pkt)
  {
    item = DIAGPKT_PKT2QITEM (pkt);

    /* If pattern has been inverted, this is a double-commit.  Ignore. */
    if (item->rsp.pattern != ~DIAGPKT_HDR_PATTERN)
    {
      if (item->rsp.pattern == DIAGPKT_HDR_PATTERN)
      {
        /* Invert the header pattern to mark that the packet has been
        processed.  This handles the incorrect usage of committing twice. */
        item->rsp.pattern = ~DIAGPKT_HDR_PATTERN;

        /* The 2 byte CRC field at the end of the allocated buffer is written
         * with a mask to detect buffer overrun.  Check to make sure that value
         * wasn't corrupted.
         */
        pattern = (PACK(uint16 *)) & item->rsp.pkt[item->rsp.length];

         /* This is safer - MATS */
        p = (unsigned char *) pattern;
        if(p[0] == ((DIAGPKT_OVERRUN_PATTERN >> 8) & 0xff)  &&
           p[1] == ((DIAGPKT_OVERRUN_PATTERN >> 0) & 0xff))
        {
          if (item->rsp.length > 0)
          {
#ifdef DIAG_CONSUMER_API_CMD_REQ
            hdr = (diagpkt_subsys_hdr_type_v2 *)&(item->rsp.pkt);
            if( hdr->command_code == DIAG_SUBSYS_CMD_VER_2_F && hdr->delayed_rsp_id )
            {
              /* If sent on DCI channel, add the tag to the list */
              if( item->rsp.cmd_tag)
              {
                list_item = (diagpkt_delayed_tag_type *)diag_malloc(sizeof(diagpkt_delayed_tag_type));

                if( list_item )
                {
                  list_item->tag            = item->rsp.cmd_tag;
                  list_item->delayed_rsp_id = hdr->delayed_rsp_id;
                  list_push_back (&diagpkt_delayed_tag_list, &list_item->link);
                  list_item = NULL;
                }
                else
                {
                  ERR_FATAL("diag_malloc returned NULL",0,0,0);
                }
              }
            }
#endif

            /* The pattern check was successful, go ahead and queue the packet. */
            q_put (&diagpkt_rsp_q, q_link (item, &item->qlink));

             #ifdef FEATURE_DEBUG_DIAG_HEAP
             MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                "Q_PUT Diagpkt_Commit diagpkt_rsp_q_cnt = %d  heap used = %d",
             q_cnt (&diagpkt_rsp_q),  diag_heap_used_bytes());
             #endif /* FEATURE_DEBUG_DIAG_HEAP */

            /* Notify the diag task so the queue can be serviced. */
            diag_rx_rsp_notify ();
          }
          else
          {
            diag_rsp_free (item);

             #ifdef FEATURE_DEBUG_DIAG_HEAP
             diagpkt_commit_free_cnt++;
             MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                    "FREE diagpkt_commit_free_cnt = %d, diag heap used = %d ",
             diagpkt_commit_free_cnt,diag_heap_used_bytes());
             #endif /* FEATURE_DEBUG_DIAG_HEAP */
           }
        }
        else
        {
          ERR_FATAL ("Buffer overrun on packet at 0x%x", (uint32) pkt, 0, 0);
        }
      }
      else
      {
        ERR_FATAL ("Invalid pointer used for diag buffering", 0, 0, 0);
      }
    }
  }

  return;
}               /* diagpkt_commit */

/*==========================================================================*/
boolean
diagpkt_rsp_send (void)
{
  static diagpkt_q_type *item_sav = NULL;
  static diag_send_desc_type send =
    { NULL, NULL, DIAG_SEND_STATE_COMPLETE, TRUE, TRUE, TRUE };

  /* Local copies of static variables for optimized access.
     This makes this routine persistent but not re-entrant. */
  diagpkt_q_type *item = item_sav;

  /* Local variables */
  diagbuf_status_enum_type status = DIAGBUF_READY_S;
  boolean finished = FALSE;

  while (!finished && status == DIAGBUF_READY_S)
  {

    if (item)
    {
      finished = FALSE;


      /* Drain the outbound buffer before sending this response packet. Since
         response packets have high priority we want to make sure the packet is
         transmitted alone (and placed at the front of the queue). */
      #ifdef DIAG_CONSUMER_API
        (void)diagbuf_send_pkt (NULL, DIAG_STREAM_1 | DIAG_STREAM_2);
      #else
        (void)diagbuf_send_pkt (NULL, DIAG_STREAM_1);
      #endif /* #ifdef DIAG_CONSUMER_API */

      status = diagbuf_send_pkt (&send,DIAG_STREAM_1);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        diag_rsp_free (item);

        #ifdef FEATURE_DEBUG_DIAG_HEAP
        diagpkt_rsp_send_free_cnt++;
        MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "FREE diagpkt_rsp_send_free_cnt = %d, diag heap used = %d ",
        diagpkt_rsp_send_free_cnt,diag_heap_used_bytes());
        #endif /* FEATURE_DEBUG_DIAG_HEAP */
        item = NULL;
      }
    }

    if (!item)
    {
      item = q_get (&diagpkt_rsp_q);

      if (item)
      {
        #ifdef FEATURE_DEBUG_DIAG_HEAP
        MSG_5(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                "Q_GET diagpkt_rsp_send rsp_q_cnt = %d, diag heap used = %d \
                Q_GET RSP cmd = %d, %d, %d ",
                         q_cnt (&diagpkt_rsp_q),diag_heap_used_bytes(), (*(item)).rsp.pkt[0],
             (*(item)).rsp.pkt[1], (*(item)).rsp.pkt[2] );
        #endif /* FEATURE_DEBUG_DIAG_HEAP */
        /* If the caller specified a function to be called with the
           response packet, call it now. Otherwise, send to the comm layer.*/
        if (item->rsp_func)
        {
          /* Clear the command connection mask. */
          DIAG_CLR_MASK (diag_cx_state, DIAG_CX_CMD_S);

          item->rsp_func (item->rsp.pkt, item->rsp.length,
                          item->rsp_func_param);

          /* This item is now complete */
          diag_rsp_free (item);

          #ifdef FEATURE_DEBUG_DIAG_HEAP
          diagpkt_rsp_send_free_cnt++;
          MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                "FREE diagpkt_rsp_send_free_cnt = %d, diag heap used = %d",
              diagpkt_rsp_send_free_cnt,diag_heap_used_bytes());
          #endif /* FEATURE_DEBUG_DIAG_HEAP */
          item = NULL;
        }
        else
        {
          send.pkt = item->rsp.pkt;
          send.last = (void *) ((uint32) send.pkt + item->rsp.length - 1);
          send.state = DIAG_SEND_STATE_START;
          send.terminate = TRUE;
        }
      }
      else
      {
        finished = TRUE;
      }
    }
  }

  /* Save local copies of persistent data back to static variables. */
  item_sav = item;

  return finished;
}

/*==========================================================================*/
boolean
diagpkt_delay_rsp_send (void)
{
  static diagpkt_delay_q_type *item_sav = NULL;
  static diag_send_desc_type send =
    { NULL, NULL, DIAG_SEND_STATE_COMPLETE, TRUE, TRUE, TRUE };

  diagbuf_status_enum_type status = DIAGBUF_READY_S;

  /* Local copies of static variables for optimized access.
     This makes this routine persistent but not re-entrant. */
  diagpkt_delay_q_type *item = item_sav;
  boolean finished = FALSE;

  while (!finished && status == DIAGBUF_READY_S)
  {
    if (item)
    {
      finished = FALSE;

      status = diagbuf_send_pkt (&send,DIAG_STREAM_1);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        free (item);

        #ifdef FEATURE_DEBUG_DIAG_HEAP
        diagpkt_delay_rsp_send_free_cnt++;
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_FATAL,"FREE diagpkt_rsp_send_free_cnt = %d",
                      diagpkt_delay_rsp_send_free_cnt);
        #endif /* FEATURE_DEBUG_DIAG_HEAP */
        item = NULL;
      }
    }

    if (!item)
    {
      item = q_get (&diagpkt_delay_rsp_q);

      if (item)
      {
        #ifdef FEATURE_DEBUG_DIAG_HEAP
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_FATAL,"Q_GET diagpkt_rsp_send rsp_q_cnt = %d",
                         q_cnt (&diagpkt_delay_rsp_q));
        #endif /* FEATURE_DEBUG_DIAG_HEAP */

        send.pkt = item->pkt;
        send.last = (void *) ((uint32) send.pkt + item->length - 1);
        send.state = DIAG_SEND_STATE_START;
        send.terminate = TRUE;
      }
      else
      {
        finished = TRUE;
      }
    }
  }

  /* Save local copies of persistent data back to static variables. */
  item_sav = item;

  return finished;
}


/* ==========================================================================
FUNCTION DIAGPKT_MASTER_TBL_INIT

DESCRIPTION Registers the table given to the diagpkt master table
============================================================================= */
static boolean
diagpkt_master_tbl_init (void)
{
  int i, j;

  int return_val;

  if(!osal_atomic_compare_and_set((osal_atomic_word_t*)&diagpkt_master_tbl_init_begin, FALSE, TRUE))
    return FALSE;

    for (i = 0; i < DIAGPKT_MSTR_TBL_SIZE; i++)
    {
      diagpkt_master_table[i] = NULL;
#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
      diagpkt_table_fwd_status[i] = DIAGPKT_TBL_FWD_STATUS_NOT_SENT; /* mark 'not fwded' */
#endif /* DIAG_MP && !DIAG_MP_MASTER */
    }

#if defined(DIAG_MP_MASTER)
    for(i = 0; i< NUM_SMD_PORTS; i++)
    {
        for (j = 0; j < DIAGPKT_MSTR_TBL_SIZE; j++)
        {
            diagpkt_slave_table[i][j] = NULL;
        }
    }
#endif /* DIAG_MP_MASTER */
   /*initialize the mutex used during registration of user table*/
  diagpkt_tbl_reg_lock.name = "MUTEX_DIAG_MASTER_TBL_REG";
    return_val = osal_init_mutex(&diagpkt_tbl_reg_lock);
    ASSERT(OSAL_SUCCESS == return_val);

#if defined(DIAG_MP_MASTER)

   /*initialize the mutex used during registration of user table*/
  diagpkt_slave_tbl_mutex.name = "MUTEX_DIAG_SLAVE_TBL_REG";
	return_val = osal_init_mutex(&diagpkt_slave_tbl_mutex);
    ASSERT(OSAL_SUCCESS == return_val);  
#endif /* DIAG_MP_MASTER */
  diagpkt_master_tbl_initialized = TRUE;
    return TRUE;
}

/* ==========================================================================
FUNCTION DIAGPKT_TBL_REG

DESCRIPTION
  Registers the user table given to the diagpkt master table
============================================================================= */
void
diagpkt_tbl_reg (const diagpkt_master_table_type * tbl_ptr)
{
  /*do not register the table if it has zero entries.*/
  if (tbl_ptr == NULL || tbl_ptr->count <= 0)
  {
    return;
  }

  /* Set in diagpkt_master_tbl_init() */
  if(diagpkt_master_tbl_initialized == FALSE )
  {
    /* Make sure this is initialized */
    diagpkt_master_tbl_init ();

    while( diagpkt_master_tbl_initialized == FALSE )
    {
      diagpkt_master_tbl_init ();
    }
	
	do
	{
		diagpkt_user_space_tbl_init();	
	}while(diagpkt_user_space_tbl_initialized == FALSE);
  }

  osal_lock_mutex(&diagpkt_tbl_reg_lock);

  for( diagpkt_master_tbl_index = diagpkt_master_tbl_index;
       diagpkt_master_tbl_index < DIAGPKT_MSTR_TBL_SIZE;
       diagpkt_master_tbl_index++ )
  {
    if( diagpkt_master_table[diagpkt_master_tbl_index] == NULL )
    {
      /* Always register the user table locally */
      diagpkt_master_table[diagpkt_master_tbl_index] = tbl_ptr;
      /* If multi-proc and not Master, forward CMD registration to Master */
      #if defined (DIAG_MP) && !defined (DIAG_MP_MASTER)
      /* Check if connection to Master proc is established first */
      if( diagcomm_connect_state() && diagcomm_ctrl_status(DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1) )
      {  
        /* Set signal to notify Diag that there's reg tables to be fwded */
        (void)diag_set_internal_sigs( DIAG_INT_CTRL_REG_SIG );
      }
      #endif /* DIAG_MP && !DIAG_MP_MASTER */
      break;
    }
  }

    osal_unlock_mutex(&diagpkt_tbl_reg_lock);

} /* diagpkt_tbl_reg */


/* ==========================================================================
FUNCTION DIAGPKT_TBL_REG_APPEND_PROC

DESCRIPTION
  Registers the user table given to the diagpkt master table and updates the
  proc_id field with the current proc id.
============================================================================= */
void
diagpkt_tbl_reg_append_proc (diagpkt_master_table_type * tbl_ptr)
{

#if defined (DIAG_IMAGE_APPS_PROC)
   if ((tbl_ptr) && (tbl_ptr->proc_id == DIAG_NO_PROC)) tbl_ptr->proc_id = DIAG_APP_PROC;
#else
   if ((tbl_ptr) && (tbl_ptr->proc_id == DIAG_NO_PROC)) tbl_ptr->proc_id = DIAG_MODEM_PROC;
#endif

   diagpkt_tbl_reg(tbl_ptr);
}

/* Tell lint that we are OK with the fact that pkt_len and req_ptr are not
** used in some of the packets, as this is dictated by the awesome design */
/*lint -esym(715,pkt_len,req_ptr) */

/* We are forced to use this coupling since the ARM C++ compiler does not
   handler 'extern "C" {}' properly. */

extern void diagcomm_init (void);

extern void paramsdiag_init (void);

void diagpkt_init_manual_dispatch_table (void)
{
  static boolean initialized = FALSE;

  if (!initialized)
  {
    initialized = TRUE;


#if  !defined (DIAG_QDSP6_APPS_PROC)
    paramsdiag_init();
#endif

#if defined (DIAG_DIAGTBL_INIT)
    diagdiag_tbl_init();
#endif
  }
}


/*===========================================================================

FUNCTION DIAGPKT_RSP_Q_INIT

DESCRIPTION
  This function initializes the response queue.

===========================================================================*/

void
diagpkt_rsp_q_init (void)
{
    (void) q_init (&diagpkt_rsp_q);
}

/*===========================================================================

FUNCTION DIAGPKT_DELAY_RSP_Q_INIT

DESCRIPTION
  This function initializes the response queue.

===========================================================================*/
void
diagpkt_delay_rsp_q_init (void)
{
    (void) q_init (&diagpkt_delay_rsp_q);
}

/*===========================================================================

FUNCTION DIAGPKT_PROCESS_REQUEST

DESCRIPTION
  This procedure formats a response packet in response to a request
  packet received from the Diagnostic Monitor.  Calls packet processing
  functions through a table, counts input and output types

Dual Processor:
  Diag tries to handle the packet locally (on modem) first. If no handler is
  found, it tries remotely (on app). If found generate an response packet. If
  diag function handler packet is not found on app processor, error response
  would be generated by the app processor and send it out
===========================================================================*/
void
diagpkt_process_request (void *req_pkt, uint16 pkt_len,
                         diag_cmd_rsp rsp_func, void *rsp_func_param,boolean bFromDiagCmdQ)
{
  uint16 packet_id;     /* Command code for std or subsystem */
  uint8 subsys_id = DIAGPKT_NO_SUBSYS_ID;
  const diagpkt_master_table_type *mstr_tbl_entry = NULL;
#if defined(DIAG_MP_MASTER)
  const diagpkt_slave_table_type *slave_tbl_entry = NULL;
  uint32 port_num = 0;
#endif
  const diagpkt_user_table_entry_type *tbl_entry = NULL;
  int tbl_entry_count = 0;
  int h, i, j;
  void *rsp_pkt = NULL;
  boolean found = FALSE;
  uint16 cmd_code = 0xFF;
  boolean skip_master_table = FALSE;
  boolean skip_slave_table = FALSE;
 uint32 proc_id = 0;
  boolean diagpkt_common_proc_req = FALSE;  /* True:dispatch registered for all processors
                                               False:dispatch registered for specific processor */
  boolean diagpkt_fwd_comm_req = TRUE;      /* True: Fwd the common req to slave
                                               False: Do not fwd the common req to slave */
  boolean diagpkt_mask_req = FALSE;         /* True: This is a 'mask' command.
                                               False: This is not a 'mask' command. */


/* See cmdiag_mode_enum_type in modem\mmode\cm\src\cmdiag.h */
#define DIAG_CM_RESET     2
#define DIAG_CM_POWER_OFF 6

#if defined (DIAG_IGNORE_RESET)
  if( !bFromDiagCmdQ)
  {
    if ((((byte *)req_pkt)[0] == DIAG_CONTROL_F) && (((byte *)req_pkt)[1] == DIAG_CM_RESET)
         && (((byte *)req_pkt)[2] == 0))
    {

    rsp_pkt = (void *) diagpkt_err_rsp (DIAG_BAD_CMD_F, req_pkt, pkt_len);
      if (rsp_pkt)
      {
        /* Return the pointer to diagpk_rsp_type, not the ptr returned
           to the avg user. */
        diagpkt_commit (rsp_pkt);
      }
    return;
   }
  }
#endif /* DIAG_IGNORE_RESET */

  if (!bFromDiagCmdQ)
  {
    if( ((byte *)req_pkt)[0] == DIAG_CONTROL_F )
    {
      /* By default, CM commands handled on Modem */
      skip_master_table = TRUE;
      skip_slave_table = FALSE;
    }

#if defined(DIAG_HANDLE_CM_RESET) && defined(DIAG_MP_MASTER)
    if((((byte *)req_pkt)[0] == DIAG_CONTROL_F) && (((byte *)req_pkt)[1] == DIAG_CM_RESET)
           && (((byte *)req_pkt)[2] == 0))
    {
      /* Handle Reset command on Master (Apps) */
      skip_master_table = FALSE;
      skip_slave_table = TRUE;
    }
#endif

#if defined(DIAG_HANDLE_CM_POWER_OFF) && defined(DIAG_MP_MASTER)
    if((((byte *)req_pkt)[0] == DIAG_CONTROL_F) && (((byte *)req_pkt)[1] == DIAG_CM_POWER_OFF)
           && (((byte *)req_pkt)[2] == 0))
    {
      /* Handle Power Down command on Master (Apps) */
      skip_master_table = FALSE;
      skip_slave_table = TRUE;
    }
#endif
  } /* !bFromDiagCmdQ */

  packet_id = diagpkt_get_cmd_code (req_pkt);

  if ( packet_id == DIAG_SUBSYS_CMD_VER_2_F )
  {
    cmd_code = packet_id;
  }
  if ((packet_id == DIAG_SUBSYS_CMD_F) || ( packet_id == DIAG_SUBSYS_CMD_VER_2_F ))
  {
    if (pkt_len < sizeof(diagpkt_subsys_hdr_type)) {

        /* pkt_len should atleast be 4 bytes Send err rsp */
        rsp_pkt = (void *) diagpkt_err_rsp( DIAG_BAD_LEN_F, req_pkt, pkt_len );

        if( rsp_pkt )
        {
          diagpkt_commit (rsp_pkt);
        }
        return;
    }

    subsys_id = diagpkt_subsys_get_id (req_pkt);
    packet_id = diagpkt_subsys_get_cmd_code (req_pkt);
  }

  if (!bFromDiagCmdQ)
  {
  if(diagpkt_filtered_cmd_set == TRUE)
  {
    if( !diagpkt_check_validity_request_ondevice(req_pkt)) {
     return;
    }
  }
  }

  switch (((byte *)req_pkt)[0])
  {
    case DIAG_SUBSYS_CMD_F:
    {
       /* Special case for handling corebsp polling command i.e. 0x4B 0x32 0x03 0x00, APPS handles the command only if
       none of the peripherals have registered for any polling command*/
      if( DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].feature_mask, F_DIAG_REQ_RSP_CHANNEL) )
      {
        if ((((byte *)req_pkt)[1] == DIAG_SUBSYS_PARAMS) && (((uint16 *)req_pkt)[1])== PARAMSDIAG_DIAG_STATUS_F)
        {
          if (diagpkt_is_polling_registered() == TRUE)  // let a peripheral handle polling command, if it has registered for it
          {
            skip_master_table = TRUE;
          }
        }
      }
      else
      {
       if (diag_buffered_mode() == FALSE)
       {
          if ((((byte *)req_pkt)[1] == DIAG_SUBSYS_PARAMS) && (((uint16 *)req_pkt)[1])== PARAMSDIAG_DIAG_STATUS_F)
          {
            if (diagpkt_is_polling_registered() == TRUE)  // let a peripheral handle polling command, if it has registered for it
            {
                skip_master_table = TRUE;
            }
          }
       }
       else
       {
          if (((((byte *)req_pkt)[1] == DIAG_SUBSYS_WCDMA) && (((uint16 *)req_pkt)[1])== WCDMA_STATUS_F) ||
             ((((byte *)req_pkt)[1] == DIAG_SUBSYS_GSM) && (((uint16 *)req_pkt)[1])== GSM_STATUS_F))
          {
             skip_slave_table = TRUE;
          }
       }
      }
       break;
    }
    case DIAG_STATUS_F:
    {
      if( !DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].feature_mask, F_DIAG_REQ_RSP_CHANNEL) )
         {
             skip_slave_table = TRUE;
         }
         break;
    }
    case DIAG_VERNO_F:
    case DIAG_EXT_BUILD_ID_F:
    {
      if( (DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].feature_mask, F_DIAG_REQ_RSP_CHANNEL))
            && (diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].feature_mask_sent == TRUE))
      {
          skip_master_table = TRUE;
      }
      else
      {
        if (diag_buffered_mode() == FALSE)
        {
            // let a peripheral handle Version number and Extended Build id, if it has registered for it
            skip_master_table = TRUE;
        }
      }
      break;
    }
  }

  /* Search the dispatch table for a matching subsystem ID.  If the
     subsystem ID matches, search that table for an entry for the given
     command code. */

  for (i = 0; !found && !skip_master_table && i < DIAGPKT_MSTR_TBL_SIZE; i++)
  {
#if defined (DIAG_MP_MASTER)
    diagpkt_common_proc_req = FALSE;
#endif

    mstr_tbl_entry = diagpkt_master_table[i];

    if (mstr_tbl_entry != NULL && mstr_tbl_entry->subsysid == subsys_id
        && mstr_tbl_entry->cmd_code == cmd_code)
    {
      tbl_entry = mstr_tbl_entry->user_table;

#if defined (DIAG_MP_MASTER)
      proc_id = mstr_tbl_entry->proc_id;

      if(proc_id == DIAG_COMMON_PROC)
      {
        diagpkt_common_proc_req = TRUE;
      }
      else if(proc_id != DIAG_MY_PROC_ID)
      {
        continue;
      }
#endif
      tbl_entry_count = (tbl_entry) ? mstr_tbl_entry->count : 0;

      for (j = 0; (tbl_entry!=NULL) && !found && j < tbl_entry_count; j++)
      {
        if (packet_id >= tbl_entry->cmd_code_lo &&
            packet_id <= tbl_entry->cmd_code_hi)
        {
          /* If the entry has no func, ignore it. */
          if (tbl_entry->func_ptr || tbl_entry->user_func_ptr)
          {
            found = TRUE;

            if (rsp_func)
            {
              /* Set the cmd connection mask to ensure the TX chain will
                 service the queue. */
              DIAG_SET_MASK (diag_cx_state, DIAG_CX_CMD_S);
            }

            /* Store rsp_func and rsp_func_param in a global to be captured
               when the dispatched function calls diagpkt_alloc().  If
               for some odd reason it never does, call rsp_func as a no-op. */
            diagpkt_rsp_func_store.pending = TRUE;
            diagpkt_rsp_func_store.rsp_func = rsp_func;
            diagpkt_rsp_func_store.rsp_func_param = rsp_func_param;
			
			/* If the port field of master table entry is non negative, its an user space client's entry */
			if(mstr_tbl_entry->port >= 0)
			{
			    /* diagpkt_process_user_space_command transfer the packet to diag_user_space_dispatcher we can just return from this function*/
				diagpkt_process_user_space_command((diagpkt_user_table_entry_type *)tbl_entry, req_pkt, pkt_len, mstr_tbl_entry->port);
				return;
			}
			
			else
			{
				if(tbl_entry->func_ptr)
					rsp_pkt = (void *) (*tbl_entry->func_ptr) (req_pkt, pkt_len);
			}

            if (diagpkt_rsp_func_store.pending == TRUE)
            {
              /* The dispatched function did not call diagpkt_alloc().
                 Go ahead and send an empty response. */
              diagpkt_rsp_func_store.pending = FALSE;

              if (diagpkt_rsp_func_store.rsp_func)
              {
                /* Clear the cmd connection mask */
                DIAG_CLR_MASK (diag_cx_state, DIAG_CX_CMD_S);

                diagpkt_rsp_func_store.rsp_func (NULL, 0,
                  diagpkt_rsp_func_store.rsp_func_param);

                diagpkt_rsp_func_store.rsp_func = NULL;
                diagpkt_rsp_func_store.rsp_func_param = NULL;
              }
            }

            if (rsp_pkt)
            {
#ifdef FEATURE_DEBUG_DIAG_HEAP
              diagpkt_process_request_cmd_req_queued++;
              if( (subsys_id == 19) && (packet_id == 3))
              {
                diagpkt_process_req_fs_close_queued++;
              }
              MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                "Total req_queued =%d fs_close req queued=%d",
                  diagpkt_process_request_cmd_req_queued,
                  diagpkt_process_req_fs_close_queued);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

#if !defined(DIAG_MP_MASTER)
              /* Slaves can commit rsp here, but Master needs to commit the response after forwarding "common" and "set mask"
                 requests to slaves to ensure we have enough DSM items for the
                 next request from QXDM. */
              diagpkt_commit (rsp_pkt);
#endif
            }
          }
          else
          {
            /* if rsp_func is not register in the table send err rsp */
             rsp_pkt = (void *) diagpkt_err_rsp( DIAG_BAD_CMD_F, req_pkt, pkt_len );

             if ( rsp_pkt )
             {
                diagpkt_commit (rsp_pkt);
             }
             return;
          }
        }

        tbl_entry++;
      }
    }
  }

  /* Assume that rsp and rsp_pkt are NULL if !found */

#if defined(DIAG_MP_MASTER)
  /* Slave Diag recieves mask updates via CTRL msgs instead of common requests
   now. Set signals to send mask updates to Slaves. Master Diag will continue
   to forward all common requests to Slave except for "set mask" requests. */
  if( diagpkt_common_proc_req == TRUE )
  {
    /* For any command that should be responded by Master, set this to FALSE so it
    doesn't get forwarded to Slaves. */
    diagpkt_fwd_comm_req = TRUE;
    diagpkt_mask_req = FALSE;

    //ToDo: merge ctrl msg sending logic to respective callbacks?
    if( subsys_id == 0xFF )
    {
    switch( packet_id )
    {
      case DIAG_MSG_F: //31
      {
        diagpkt_mask_req = TRUE;
        diagpkt_fwd_comm_req = FALSE;
        diagcomm_mask_update(DIAG_UPDATE_MASK_F3, DIAG_STREAM_1, diag_cur_preset_id, FALSE);

        for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
        {
          /* DIAG_MSG_F only updates stream 1 masks */
          diagcomm_ctrl_send_msg_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                       (diagcomm_enum_port_type)DIAG_STREAM_1, diag_cur_preset_id, TRUE );
        }
        break;
      }

      case DIAG_EXT_MSG_CONFIG_F: //125
      {
        msg_get_ssid_ranges_req_type *common_req = (msg_get_ssid_ranges_req_type *)req_pkt;
        if( (common_req->sub_cmd == MSG_EXT_SUBCMD_GET_RANGES) || //1
            (common_req->sub_cmd == MSG_EXT_SUBCMD_GET_BUILD_MASK) || //2
            (common_req->sub_cmd == MSG_EXT_SUBCMD_GET_RT_MASK) ) //3
        {
          diagpkt_mask_req = TRUE;
          if( (DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].feature_mask, F_DIAG_BT_MASK_CENTRALIZATION_SUPPORT))
              || (diag_buffered_mode() && (!(DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].feature_mask, F_DIAG_REQ_RSP_CHANNEL))))
              || (!diagcomm_smd_status(DIAGCOMM_PORT_1)))
          {
            /* Handle mask queries on Master if build mask centralization feature is supported on modem OR in Buffered mode but command channel separation is not supported
                 OR Modem is not up */
            diagpkt_fwd_comm_req = FALSE;
          }
        }
        else if( common_req->sub_cmd == MSG_EXT_SUBCMD_SET_RT_MASK ) //4
        {
          diagpkt_mask_req = TRUE;
          diagpkt_fwd_comm_req = FALSE;
          /* DIAG_EXT_MSG_CONFIG_F only updates stream 1 masks */
          diagcomm_mask_update(DIAG_UPDATE_MASK_PARTIAL_F3, DIAG_STREAM_1, diag_cur_preset_id, FALSE);
        }
        else if( common_req->sub_cmd == MSG_EXT_SUBCMD_SET_ALL_RT_MASKS ) //5
        {
          diagpkt_mask_req = TRUE;
          diagpkt_fwd_comm_req = FALSE;
          diagcomm_mask_update(DIAG_UPDATE_MASK_F3, DIAG_STREAM_1, diag_cur_preset_id, FALSE);
        }

        for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
        {
          /* DIAG_EXT_MSG_CONFIG_F only updates stream 1 masks */
          diagcomm_ctrl_send_msg_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                       (diagcomm_enum_port_type)DIAG_STREAM_1, diag_cur_preset_id, TRUE );
        }
        break;
      }

      case DIAG_LOG_CONFIG_F: //115
      {
        DIAG_LOG_CONFIG_F_req_type *common_req = (DIAG_LOG_CONFIG_F_req_type *)req_pkt;

        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "DIAG_LOG_CONFIG_F %d", DIAG_LOG_CONFIG_F);

        if( ((unsigned int)common_req->operation == LOG_CONFIG_DISABLE_OP) || //0
            ((unsigned int)common_req->operation == LOG_CONFIG_SET_MASK_OP) ) //3
        {
          diagpkt_mask_req = TRUE;
          diagpkt_fwd_comm_req = FALSE;
          diagcomm_mask_update(DIAG_UPDATE_MASK_PARTIAL_LOGS, DIAG_STREAM_1, diag_cur_preset_id, FALSE);

          for( port_num = 0; port_num < NUM_SMD_PORTS; port_num++ )
          {
            diagcomm_ctrl_send_log_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                         (diagcomm_enum_port_type)DIAG_STREAM_1, diag_cur_preset_id, TRUE );
          }
        }
        else if( ((unsigned int)common_req->operation == LOG_CONFIG_RETRIEVE_ID_RANGES_OP) || //1
                 ((unsigned int)common_req->operation == LOG_CONFIG_RETRIEVE_VALID_MASK_OP) || //2
                 ((unsigned int)common_req->operation == LOG_CONFIG_GET_LOGMASK_OP) ) //4
        {
          diagpkt_mask_req = TRUE;
          if( (DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].feature_mask, F_DIAG_BT_MASK_CENTRALIZATION_SUPPORT))
              || (diag_buffered_mode() && (!(DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].feature_mask, F_DIAG_REQ_RSP_CHANNEL))))
              || (!diagcomm_smd_status(DIAGCOMM_PORT_1)))
          {
            /* Handle mask queries on Master if build mask centralization feature is supported on modem OR in Buffered mode but command channel separation is not supported
                 OR Modem is not up */
            diagpkt_fwd_comm_req = FALSE;
          }
        }
        break;
      }

      case DIAG_EXT_LOGMASK_F: //93
      {
        DIAG_EXT_LOGMASK_F_req_type *req_ptr = (DIAG_EXT_LOGMASK_F_req_type *)req_pkt;

        diagpkt_mask_req = TRUE;

        if( req_ptr->num_bits == 0 )
        {
          if( (DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].feature_mask, F_DIAG_BT_MASK_CENTRALIZATION_SUPPORT))
              || (diag_buffered_mode() && (!(DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].feature_mask, F_DIAG_REQ_RSP_CHANNEL))))
              || (!diagcomm_smd_status(DIAGCOMM_PORT_1)))
          {
            /* Handle mask queries on Master if build mask centralization feature is supported on modem OR in Buffered mode but command channel separation is not supported
                 OR Modem is not up */
            diagpkt_fwd_comm_req = FALSE;
          }
        }
        else
        {
          diagpkt_fwd_comm_req = FALSE;
          diagcomm_mask_update(DIAG_UPDATE_MASK_LOGS, DIAG_STREAM_1, diag_cur_preset_id, FALSE);

          for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
          {
            /* DIAG_EXT_LOGMASK_F only updates stream 1 masks */
            diagcomm_ctrl_send_log_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num,
                                         (diagcomm_enum_port_type)DIAG_STREAM_1, diag_cur_preset_id, TRUE );
          }
        }
        break;
      }

      case DIAG_EVENT_REPORT_F: //96
      {
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "DIAG_EVENT_REPORT_F %d", DIAG_EVENT_REPORT_F);

        diagpkt_mask_req = TRUE;
        diagpkt_fwd_comm_req = FALSE;

        /* Propagate masks to Slaves */
        diagcomm_mask_update(DIAG_UPDATE_MASK_EVENTS, DIAG_STREAM_1, diag_cur_preset_id, TRUE);
        break;
      }

      case DIAG_EVENT_MASK_GET_F: //129
      {
        diagpkt_mask_req = TRUE;
          if( (DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].feature_mask, F_DIAG_BT_MASK_CENTRALIZATION_SUPPORT))
              || (diag_buffered_mode() && (!(DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].feature_mask, F_DIAG_REQ_RSP_CHANNEL))))
              || (!diagcomm_smd_status(DIAGCOMM_PORT_1)) )
          {
            /* Handle mask queries on Master if build mask centralization feature is supported on modem OR in Buffered mode but command channel separation is not supported
                 OR Modem is not up */
          diagpkt_fwd_comm_req = FALSE;
        }
        break;
      }

      case DIAG_EVENT_MASK_SET_F: //130
      {
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "DIAG_EVENT_MASK_SET_F %d", DIAG_EVENT_MASK_SET_F);

        diagpkt_mask_req = TRUE;
        diagpkt_fwd_comm_req = FALSE;

        /* Propagate masks to Slaves */
        diagcomm_mask_update(DIAG_UPDATE_MASK_EVENTS, DIAG_STREAM_1, diag_cur_preset_id, TRUE);
        break;
      }

      default:
        break;
    } // END switch
    }// END if subsys_id == 0xFF

    if( subsys_id == DIAG_SUBSYS_DIAG_SERV ) //18
    {
      if( packet_id == DIAG_EXT_EVENT_REPORT_CTRL ) //96
      {
        event_ext_rpt_ctrl_type *common_req = (event_ext_rpt_ctrl_type *)req_pkt;

        if( (unsigned int)common_req->subcmd == DIAG_EXT_EVENT_REPORT_GET_MASK )
        {
            diagpkt_mask_req = TRUE;

          if( (DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].feature_mask, F_DIAG_BT_MASK_CENTRALIZATION_SUPPORT))
              || (diag_buffered_mode() && (!(DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].feature_mask, F_DIAG_REQ_RSP_CHANNEL))))
              || (!diagcomm_smd_status(DIAGCOMM_PORT_1)) )
            {
            /* Handle mask queries on Master if build mask centralization feature is supported on modem OR in Buffered mode but command channel separation is not supported
                 OR Modem is not up */
                diagpkt_fwd_comm_req = FALSE;
            }
        }
        else if (((unsigned int)common_req->subcmd == DIAG_EXT_EVENT_REPORT_SET_MASK) ||
                    ((unsigned int)common_req->subcmd == DIAG_EXT_EVENT_REPORT_SET_ALL))
        {
            diagpkt_mask_req = TRUE;
            diagpkt_fwd_comm_req = FALSE;
        }
      }

      else if( packet_id == DIAG_EXT_MSG_CONFIG_ADV ) //97
      {
        ext_config_header *common_req = (ext_config_header *)req_pkt;

        if( ((unsigned int)common_req->opcode == MSG_EXT_SUBCMD_SET_RT_MASK) ||
                 ((unsigned int)common_req->opcode == MSG_EXT_SUBCMD_SET_ALL_RT_MASKS) )
        {
          diagpkt_mask_req = TRUE;
          diagpkt_fwd_comm_req = FALSE;
        }
        else if( ((unsigned int)common_req->opcode == MSG_EXT_SUBCMD_GET_RANGES) ||
                 ((unsigned int)common_req->opcode == MSG_EXT_SUBCMD_GET_BUILD_MASK) ||
                 ((unsigned int)common_req->opcode == MSG_EXT_SUBCMD_GET_RT_MASK) )
        {
          diagpkt_mask_req = TRUE;
          if( (DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].feature_mask, F_DIAG_BT_MASK_CENTRALIZATION_SUPPORT))
              || (diag_buffered_mode() && (!(DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].feature_mask, F_DIAG_REQ_RSP_CHANNEL))))
              || (!diagcomm_smd_status(DIAGCOMM_PORT_1)) )
          {
            /* Handle mask queries on Master if build mask centralization feature is supported on modem OR in Buffered mode but command channel separation is not supported
                 OR Modem is not up */
            diagpkt_fwd_comm_req = FALSE;
          }
        }
        else
        {
          diagpkt_mask_req = TRUE;
          diagpkt_fwd_comm_req = FALSE;
        }
      }

      else if( packet_id == DIAG_EXT_LOG_CONFIG ) //98
      {
        ext_config_header *common_req = (ext_config_header *)req_pkt;

        if( ((unsigned int)common_req->opcode == LOG_CONFIG_DISABLE_OP) ||
            ((unsigned int)common_req->opcode == LOG_CONFIG_SET_MASK_OP) )
        {
          diagpkt_mask_req = TRUE;
          diagpkt_fwd_comm_req = FALSE;
        }
        else if( ((unsigned int)common_req->opcode == LOG_CONFIG_RETRIEVE_ID_RANGES_OP) )
        {
          /* Handle mask queries on Master if Modem not up or in Buffered mode */
          diagpkt_mask_req = TRUE;
          if( (DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].feature_mask, F_DIAG_BT_MASK_CENTRALIZATION_SUPPORT))
              || (diag_buffered_mode() && (!(DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].feature_mask, F_DIAG_REQ_RSP_CHANNEL))))
              || (!diagcomm_smd_status(DIAGCOMM_PORT_1)) )
          {
            /* Handle mask queries on Master if build mask centralization feature is supported on modem OR in Buffered mode but command channel separation is not supported
                 OR Modem is not up */
            diagpkt_fwd_comm_req = FALSE;
          }
        }
        else if( ((unsigned int)common_req->opcode == LOG_CONFIG_RETRIEVE_VALID_MASK_OP) ||
                 ((unsigned int)common_req->opcode == LOG_CONFIG_GET_LOGMASK_OP) )
        {
          diagpkt_mask_req = TRUE;

          if( (DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].feature_mask, F_DIAG_BT_MASK_CENTRALIZATION_SUPPORT))
              || (diag_buffered_mode() && (!(DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].feature_mask, F_DIAG_REQ_RSP_CHANNEL))))
              || (!diagcomm_smd_status(DIAGCOMM_PORT_1)) )
          {
            /* Handle mask queries on Master if build mask centralization feature is supported on modem OR in Buffered mode but command channel separation is not supported
                 OR Modem is not up */
            diagpkt_fwd_comm_req = FALSE;
          }

        }

      }

    } //END DIAG_SUBSYS_DIAG_SERV
  } /* END diagpkt_common_proc_req */

  if( diagpkt_common_proc_req )
  {
    if( diagpkt_fwd_comm_req )
    {
      // Forward non "set mask" common requests to all slave procs
      for( i = 0; i < NUM_SMD_PORTS; i++ )
      {
        diagcomm_send_cmd(req_pkt, pkt_len, (diagcomm_enum_port_type)i, FALSE);
      }

      if( diagpkt_mask_req )
      {
        /* If this was a mask command and we're forwarding to Slaves, then
           we need to free the response generated by Master to allow Modem
           to respond.
        */
        diagpkt_free(rsp_pkt);
        rsp_pkt = NULL;
      }
    }
    if( rsp_pkt )
    {
      /* If cmd is not forwarded to Slaves, Master needs to send response. */
      diagpkt_commit(rsp_pkt);
    }
  }
  else if( found )
  {
    if( rsp_pkt )
    {
      /* Committing the rsp now */
      diagpkt_commit(rsp_pkt);
    }
  }
    else if( !found )
    {
      boolean found_slave = FALSE;
       for ( h = 0; !found_slave && h < NUM_SMD_PORTS; h++)
      {
      // Search slave table for cmd registration

      for( i = 0; !found_slave && !skip_slave_table && i < DIAGPKT_MSTR_TBL_SIZE; i++ )
      {
        slave_tbl_entry = diagpkt_slave_table[h][i];

        if( (slave_tbl_entry != NULL) && (slave_tbl_entry->mstr_data.subsysid == subsys_id)
            && (slave_tbl_entry->mstr_data.cmd_code == cmd_code) )
        {
          tbl_entry = slave_tbl_entry->mstr_data.user_table;
          tbl_entry_count = (tbl_entry) ? slave_tbl_entry->mstr_data.count : 0;

          for( j = 0; (tbl_entry != NULL) && !found && (j < tbl_entry_count); j++ )
          {
            if( (packet_id >= tbl_entry->cmd_code_lo) &&
                (packet_id <= tbl_entry->cmd_code_hi) )
            {
                found_slave = TRUE;
            }
            tbl_entry++;
          } /* end 'j' for-loop */
        }
      } /* end 'i' for-loop */
      }/* end of 'h' for-loop */

      if( found_slave == TRUE )
      {
        if( diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][slave_tbl_entry->channel].feature_mask_sent == TRUE )
        {
          // Pkt ID was found; Send to slave proc which registered the CMD
          diagcomm_send_cmd( req_pkt, pkt_len, slave_tbl_entry->channel, FALSE );
        }
        else
        {
          /* Feature mask hasn't been sent yet, so can't forward command. Send error rsp instead. */
          rsp_pkt = (void *) diagpkt_err_rsp( DIAG_BAD_CMD_F, req_pkt, pkt_len );
          if( rsp_pkt )
          {
            diagpkt_commit (rsp_pkt);
          }
        }
      }
      else
      {
        /* Pkt ID not found even in slave table; Send err rsp */
        rsp_pkt = (void *) diagpkt_err_rsp( DIAG_BAD_CMD_F, req_pkt, pkt_len );
        if( rsp_pkt )
        {
          diagpkt_commit (rsp_pkt);
        }
      }
    }

#else /* DIAG_MP_MASTER */

  /* Assume that rsp and rsp_pkt are NULL if !found
     Make sure that error responses get sent. */
  if (!found)
  {
    /* Packet id not found, send an error packet. */
    rsp_pkt = (void *) diagpkt_err_rsp (DIAG_BAD_CMD_F, req_pkt, pkt_len);

    if (rsp_pkt)
    {
      /* Return the pointer to diagpk_rsp_type, not the ptr returned
         to the avg user. */
      diagpkt_commit (rsp_pkt);
    }
  }
#endif /* DIAG_MP_MASTER */
  return;
}               /* diagpkt_process_request */

/*=========================================================================*/
PACK(void *)
diagpkt_err_rsp(diagpkt_cmd_code_type code, PACK(void *) req_pkt,
                  uint16 req_len)
{
  DIAG_BAD_CMD_F_rsp_type *rsp;
  const unsigned int rsp_len = MIN (sizeof (DIAG_BAD_CMD_F_rsp_type),
               req_len + FPOS (DIAG_BAD_CMD_F_rsp_type, pkt));
  rsp = (DIAG_BAD_CMD_F_rsp_type *) diagpkt_alloc (code, rsp_len);

  if(req_pkt)
  {
    memscpy ((void *) rsp->pkt, DIAGPKT_MAX_ERR,
            (void *) req_pkt,
            rsp_len - FPOS (DIAG_BAD_CMD_F_rsp_type, pkt));
  }
  else if (req_len != 0)
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "Non-0 request length (%d) and NULL request pointer!",req_len);
  }

  return ((void *) rsp);
}               /* diagkt_err_rsp */

/*==========================================================================*/
void
diagpkt_async_block (void)
{
  osal_sigs_t return_sigs;
  int return_val = 0;
  /* Wait until async sig is set by release() call. */
  (void) diag_wait (DIAG_ASYNC_BLOCK_SIG);
  return_val = osal_reset_sigs(&diag_task_tcb, DIAG_ASYNC_BLOCK_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);
}

/*==========================================================================*/
void
diagpkt_async_release (void)
{
  /* Release by setting the ASYNC sig */
  osal_sigs_t return_sigs;
  int return_val = 0;
  return_val = osal_set_sigs(&diag_task_tcb, DIAG_ASYNC_BLOCK_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);
}

#if defined(DIAG_MP_MASTER) && defined (DIAG_SMD_SUPPORT)
/*===========================================================================

FUNCTION diagpkt_is_polling_registered

DESCRIPTION
  This function checks if anyone peripheral have registered for any polling command.
  i.e.
  0x0C,
  4B 04 0E 00,
  4B 08 02 00,
  4B 32 03 00

RETURN VALUE
  TRUE  - If peripheral has a polling command registered
  FALSE - If peripheral does not have a polling command registered

===========================================================================*/
boolean diagpkt_is_polling_registered(void)
{
   boolean status = FALSE; // Start with the assumption that no polling
                           // command is registered
   int i = 0;

   for ( i = 0; i < NUM_SMD_PORTS; i++ )
   {
      if (is_polling_cmd_registered[i] == TRUE)
      {
         status = TRUE;
         break;
      }
   }

   return status;
}

/*===========================================================================

FUNCTION diagpkt_check_for_polling_cmd

DESCRIPTION
  This function update is_polling_cmd_registered table to indicate if any peripheral
  has any polling command registered i.e.
  0x0C,
  4B 04 0E 00,
  4B 08 02 00,
  4B 32 03 00

RETURN VALUE
  TRUE  - If the command being registered is a polling command
  FALSE - If the command being registered is NOT a polling command

===========================================================================*/
boolean diagpkt_check_for_polling_cmd(const diagpkt_master_table_type *mstr_data)
{
    boolean status = FALSE; // Start with the assumption that the command being checked
                            // is not a polling command

    const diagpkt_user_table_entry_type *user_table =  mstr_data->user_table;

    if(user_table != NULL)
    {
        uint16 cmd_code      =  mstr_data->cmd_code;
        word   subsys_id     =  mstr_data->subsysid;
        word   count         =  mstr_data->count;
        word   array_index   =  0; // Since in a table there can be more than one registrations
        word   cmd_code_lo;
        word   cmd_code_hi;

        for (array_index = 0; array_index < count; array_index++)
        {
            cmd_code_lo = user_table[array_index].cmd_code_lo;
            cmd_code_hi = user_table[array_index].cmd_code_hi;

            if((cmd_code == DIAG_SUBSYS_LEGACY && subsys_id == DIAG_SUBSYS_LEGACY &&
                cmd_code_lo <= DIAG_STATUS_F && cmd_code_hi >= DIAG_STATUS_F) ||
                // check for 0x0C polling command
                (cmd_code == DIAG_SUBSYS_LEGACY && subsys_id == DIAG_SUBSYS_WCDMA &&
                cmd_code_lo <= WCDMA_STATUS_F && cmd_code_hi >= WCDMA_STATUS_F) ||
                // Check for 0x4B 0x04 0x0E 0x00 polling command
                (cmd_code == DIAG_SUBSYS_LEGACY && subsys_id == DIAG_SUBSYS_GSM &&
                cmd_code_lo <= GSM_STATUS_F && cmd_code_hi >= GSM_STATUS_F) ||
                // Check for 0x4B 0x08 0x02 0x00 polling command
                (cmd_code == DIAG_SUBSYS_LEGACY && subsys_id == DIAG_SUBSYS_PARAMS &&
                cmd_code_lo <= PARAMSDIAG_DIAG_STATUS_F && cmd_code_hi >= PARAMSDIAG_DIAG_STATUS_F) ||
                // Check for 0x4B 0x32 0x03 0x00 polling command
                (cmd_code == DIAG_SUBSYS_LEGACY && subsys_id == DIAG_SUBSYS_TDSCDMA &&
                cmd_code_lo <= TDSCDMA_STATUS_F && cmd_code_hi >= TDSCDMA_STATUS_F))
				// Check for 0x4B 0x57 0x0E 0x00 polling command
            {
                status = TRUE; // the command is a polling command
                break; // Since we only care about any command (not all)
            }
        }
    }// end of user_table != NULL

    return status;
}

/*===========================================================================
FUNCTION DIAGPKT_slave_table_cleanup

DESCRIPTION
  This function cleanup slave table based on port passed to it. It also mark
  is_polling_cmd_registered table FALSE for the port this function is call.

RETURN VALUE
  None.

===========================================================================*/
void diagpkt_slave_table_cleanup (diagcomm_enum_port_type port_num)
{
    if (port_num < NUM_SMD_PORTS)
    {
        uint8 slave_tbl_index = 0;
        diagpkt_slave_table_type *ptr_to_slave_table = NULL;
        is_polling_cmd_registered[port_num] = FALSE;

		osal_lock_mutex(&diagpkt_slave_tbl_mutex);
        for( slave_tbl_index = 0; slave_tbl_index < DIAGPKT_MSTR_TBL_SIZE; slave_tbl_index++ )
        {
            ptr_to_slave_table = diagpkt_slave_table[port_num][slave_tbl_index];
            if (ptr_to_slave_table != NULL)
            {
                if( ptr_to_slave_table->mstr_data.user_table)
                {
                    diag_free((void*)ptr_to_slave_table->mstr_data.user_table);
                    ptr_to_slave_table->mstr_data.user_table = NULL;
                }
                diag_free(ptr_to_slave_table);
                ptr_to_slave_table = NULL;
                diagpkt_slave_table[port_num][slave_tbl_index] = NULL;
            }// end of if (ptr_to_slave_table != NULL)
        } // end of for loop

    /* Resetting the Slave Table Index for this port since we
       are clearing the registration entries */
    diagpkt_slave_tbl_indices[port_num] = 0;
	osal_unlock_mutex(&diagpkt_slave_tbl_mutex);

    }// end of port_num sanity check
    return;
}
#endif /* defined(DIAG_MP_MASTER) && defined (DIAG_SMD_SUPPORT) */

/*===========================================================================
FUNCTION DIAGPKT_GET_REG_CMD_COUNT

DESCRIPTION
   This function is used to get the number of command codes /command handlers
   registered for a given processor

RETURNS
   The number of entries in the master or slave table
============================================================================*/
uint8
diagpkt_get_reg_cmd_count()
{
  uint8 table_count = 0;
  if(reg_tbl_is_master)
  {
    /* Number of entries in the the Master Table */
    table_count = diagpkt_master_tbl_index;
  }
#ifdef DIAG_MP_MASTER
  else
  {
    /* Port ID should not be greater than the maximum ports available.
       NUM_SMD_PORTS is the correct macro to be used here. */
    if(reg_tbl_port_id < NUM_SMD_PORTS)
    {
      /* Number of entries in the slave table for the given port */
      table_count = diagpkt_slave_tbl_indices[reg_tbl_port_id];
    }
  }
#endif
  return table_count;
}

/*===========================================================================
FUNCTION DIAGPKT_GET_TBL_PKT_LENGTH

DESCRIPTION
   This procedure returns the total number of bytes needed to store the
   registration table information -- used to send the registration table
   as a response to Registration Table Retrieval Commands

RETURNS
   The total size (in bytes) that should be allocated in diagbuf to
   accomodate the response for registration command retrieval request
============================================================================*/
uint32
diagpkt_get_reg_tbl_pkt_length()
{
  uint32 pkt_len = 0;
  uint8 table_index = 0;
  const diagpkt_master_table_type *mstr_tbl_entry = NULL;

  /* Adding the size of the header */
  pkt_len += sizeof(diagpkt_subsys_header_v2_type);

  if( reg_tbl_is_master == TRUE )  /* query the size of the master table */
  {
    for(table_index = 0; table_index < DIAGPKT_MSTR_TBL_SIZE; table_index++)
    {
      mstr_tbl_entry = diagpkt_master_table[table_index];
      if( mstr_tbl_entry != NULL )
      {
        /* adding the size of the diagdiag_ctrl_msg_reg_type for the cmd_code registered (with out the user commands) */
        pkt_len += sizeof(diagdiag_ctrl_msg_reg_type) - (sizeof(diagdiag_ctrl_msg_user_tbl_entry_type));

        /* adding the size of diagdiag_ctrl_msg_user_tbl_entry_type for every user command registered */
        pkt_len += ((mstr_tbl_entry->count) * sizeof(diagdiag_ctrl_msg_user_tbl_entry_type));
      }
    }
  }
#ifdef DIAG_MP_MASTER
  else  /* query the size of the slave table */
  {
    /* Port ID should not be greater than the maximum ports available */
    if( reg_tbl_port_id >= NUM_SMD_PORTS )
    {
      return pkt_len;
    }

    for(table_index = 0; table_index < DIAGPKT_MSTR_TBL_SIZE; table_index++)
    {
      if( diagpkt_slave_table[reg_tbl_port_id][table_index] != NULL )
      {
        mstr_tbl_entry = &(diagpkt_slave_table[reg_tbl_port_id][table_index]->mstr_data);
        if( mstr_tbl_entry != NULL )
        {
          /* adding the size of the diagdiag_ctrl_msg_reg_type for the cmd_code registered (with out the user commands) */
          pkt_len += sizeof(diagdiag_ctrl_msg_reg_type) - (sizeof(diagdiag_ctrl_msg_user_tbl_entry_type));
          /* adding the size of diagdiag_ctrl_msg_user_tbl_entry_type for every user command registered */
          pkt_len += ((mstr_tbl_entry->count) * sizeof(diagdiag_ctrl_msg_user_tbl_entry_type));
        }
      }
    }
  }
#endif
  return pkt_len;
} /* diagpkt_get_reg_tbl_pkt_length */


/*===========================================================================
FUNCTION DIAGPKT_GET_REGISTRATION_TABLE

DESCRIPTION
   This function is called by DIAGDIAG_GET_REG_TABLE to query the processors's
   registration table. It sends one or more delayed responses containing the
   information about the commands registered in the processor's
   master or slave registration table

RETURNS
   None

============================================================================*/
void
diagpkt_get_registration_table ()
{
  const diagpkt_master_table_type *mstr_tbl_entry = NULL;
  const diagpkt_user_table_entry_type *usr_tbl_entry = NULL;
  diagdiag_get_reg_tbl_delayed_rsp_type *rsp = NULL;
  diagdiag_get_reg_tbl_delayed_rsp_type *rsp_entry = NULL;

  uint16 pkt_size = 0;  /* Keeps track of the current packet size to ensure that we don't cross the MAX pkt length */
  uint16 table_index = 0;  /* Index for the master or slave table */
  uint16 usr_cmd_index = 0;  /* Index for the user command table */

  uint32 total_pkt_len = 0;  /* Total length of the registration information that is to be sent */
  uint8 offset = 0;  /* Used for offsetting every entry in the registration table */
  uint8 rsp_count = 1;  /* Keeps track of the reponse number in case of multiple responses (packet exceeding the Max pkt Length) */
  boolean rsp_incomplete = FALSE;  /* Flag to determine if all the information is sent or not */
  diagdiag_ctrl_msg_user_tbl_entry_type *usr_cmd_entry; /*user table entry  */

  /* Check if the value of the global command code is valid */
  if( !diagdiag_is_valid_reg_tbl_cmd() )
  {
    return;
  }

  /* Port ID should not be greater than the maximum ports available 
     NUM_SMD_PORTS is the correct macro to be used here. */
  if(reg_tbl_port_id >= NUM_SMD_PORTS)
  {
    return;
  }

  /* Get the total length of the reponse packet */
  total_pkt_len = diagpkt_get_reg_tbl_pkt_length();

  if(reg_tbl_is_master == TRUE)
  {
    while (TRUE)  /* repeat till registration information is completely sent */
    {
      pkt_size = 0;  /* reset the packet size at each iteration */

      /* Allocate space in the diag buffer for delayed response */
      rsp = (diagdiag_get_reg_tbl_delayed_rsp_type *) diagpkt_subsys_alloc_v2_delay(
                                DIAG_SUBSYS_DIAG_SERV,
                                reg_tbl_cmd_code,
                                reg_tbl_delay_rsp_id,
                                total_pkt_len );
      if( rsp != NULL )
      {
        ((diagpkt_subsys_hdr_type_v2 *)rsp)->rsp_cnt = rsp_count;  /* Set the response count for the delayed response */
        rsp_entry = rsp;  /* the response entry should point to rsp - the first location of the allocated response */

        for( table_index = table_index; table_index < DIAGPKT_MSTR_TBL_SIZE; table_index++ )
        {
          /* Querying the master table */
          mstr_tbl_entry = diagpkt_master_table[table_index];
          if( mstr_tbl_entry != NULL )
          {
            /* Check if adding the current master table entry to the response packet will exceed the maximum packet
               size that can be transmitted */
            pkt_size += ((mstr_tbl_entry->count - 1) * sizeof(diagdiag_ctrl_msg_user_tbl_entry_type)) + sizeof(diagdiag_ctrl_msg_reg_type);
            if(pkt_size > DIAG_MAX_TX_PKT_SIZ)  /* DIAG_MAX_TX_PKT_SIZ is the maximum length of a packet that the tools can receive */
            {
              rsp_incomplete = TRUE;  /* more packets to come */
              break;
            }

            /* Fill the reponse entry with the information from the master table */
            rsp_entry->reg_cmds[0].cmd_code = mstr_tbl_entry->cmd_code;
            rsp_entry->reg_cmds[0].subsysid = mstr_tbl_entry->subsysid;
            rsp_entry->reg_cmds[0].cmd_count = mstr_tbl_entry->count;
            rsp_entry->reg_cmds[0].proc_id = mstr_tbl_entry->proc_id;
            usr_tbl_entry = mstr_tbl_entry->user_table;

            if( usr_tbl_entry != NULL )  /* Fill the user commands with information from the user table */
            {
              for( usr_cmd_index = 0; usr_cmd_index < mstr_tbl_entry->count; usr_cmd_index++ )
              {
                usr_cmd_entry = &(rsp_entry->reg_cmds[0].usr_cmds[0]);
                usr_cmd_entry+= usr_cmd_index;
                usr_cmd_entry->cmd_code_lo = usr_tbl_entry[usr_cmd_index].cmd_code_lo;
                usr_cmd_entry->cmd_code_hi = usr_tbl_entry[usr_cmd_index].cmd_code_hi;
              }
            }
            /* Offset is determined by the number of user commands registered (more than 1) -- used to offset
               the location of the next registered command entry in the response packet */
            offset = (mstr_tbl_entry->count - 1) * sizeof(diagdiag_ctrl_msg_user_tbl_entry_type);

            /* Add the offset to the current response entry to get the location of the next response entry */
            rsp_entry = (diagdiag_get_reg_tbl_delayed_rsp_type *)((byte *)rsp_entry + sizeof(diagdiag_ctrl_msg_reg_type) + offset);

            /* if we reach here, the packet can be considered complete, unless the next entry is bigger than the maximim packet length */
            rsp_incomplete = FALSE;
          }
        }
        diagpkt_delay_commit((PACK(void *))rsp);  /* Committing the reponse to the diag buffer - this gets executed for both completed and incomplete response */
        if( rsp_incomplete )
        {
          rsp_count = rsp_count + 1; /* Incrementing the response count for the next delayed response */
          continue;
        }
      }
      break;
    }
  }
#ifdef DIAG_MP_MASTER
  else
  {
    while (TRUE) /* repeat till registration information is completely sent */
    {
      pkt_size = 0;  /* reset the packet size at each iteration */

      /* Allocate space in the diag buffer for delayed response */
      rsp = (diagdiag_get_reg_tbl_delayed_rsp_type *) diagpkt_subsys_alloc_v2_delay(
                                DIAG_SUBSYS_DIAG_SERV,
                                reg_tbl_cmd_code,
                                reg_tbl_delay_rsp_id,
                                total_pkt_len );
      if( rsp != NULL )
      {
        ((diagpkt_subsys_hdr_type_v2 *)rsp)->rsp_cnt = rsp_count; /* Set the response count for the delayed response */
        rsp_entry = rsp; /* the response entry should point to rsp - the first location of the allocated response */

        for( table_index = table_index; table_index < DIAGPKT_MSTR_TBL_SIZE; table_index++ )
        {
          /* Querying the slave table */
          if( diagpkt_slave_table[reg_tbl_port_id][table_index] != NULL)
          {
            mstr_tbl_entry = &(diagpkt_slave_table[reg_tbl_port_id][table_index]->mstr_data);
            if( mstr_tbl_entry!= NULL )
            {
              /* Check if adding the current master table entry to the response packet will exceed the maximum packet
               size that can be transmitted */
              pkt_size += (mstr_tbl_entry->count - 1) * sizeof(diagdiag_ctrl_msg_user_tbl_entry_type) + sizeof(diagdiag_ctrl_msg_reg_type);
              if(pkt_size > DIAG_MAX_TX_PKT_SIZ)  /* DIAG_MAX_TX_PKT_SIZ is the maximum length of a packet that the tools can receive */
              {
                rsp_incomplete = TRUE; /* more packets to come */
                break;
              }

              /* Fill the reponse entry with the information from the master table */
              rsp_entry->reg_cmds[0].cmd_code = mstr_tbl_entry->cmd_code;
              rsp_entry->reg_cmds[0].subsysid = mstr_tbl_entry->subsysid;
              rsp_entry->reg_cmds[0].cmd_count = mstr_tbl_entry->count;
              rsp_entry->reg_cmds[0].proc_id = mstr_tbl_entry->proc_id;
              usr_tbl_entry = mstr_tbl_entry->user_table;
              if( usr_tbl_entry != NULL )  /* Fill the user commands with information from the user table */
              {
                for( usr_cmd_index = 0; usr_cmd_index < mstr_tbl_entry->count; usr_cmd_index++ )
                {
                  usr_cmd_entry = &(rsp_entry->reg_cmds[0].usr_cmds[0]);
                  usr_cmd_entry+= usr_cmd_index;
                  usr_cmd_entry->cmd_code_lo = usr_tbl_entry[usr_cmd_index].cmd_code_lo;
                  usr_cmd_entry->cmd_code_hi = usr_tbl_entry[usr_cmd_index].cmd_code_hi;
                }
              }
              /* Offset is determined by the number of user commands registered (more than 1) -- used to offset
                 the location of the next registered command entry in the response packet */
              offset = (mstr_tbl_entry->count - 1) * sizeof(diagdiag_ctrl_msg_user_tbl_entry_type);

              /* Add the offset to the current response entry to get the location of the next response entry */
              rsp_entry = (diagdiag_get_reg_tbl_delayed_rsp_type *)((byte *)rsp_entry + sizeof(diagdiag_ctrl_msg_reg_type) + offset);

              /* if we reach here, the packet can be considered complete, unless the next entry is bigger than the maximim packet length */
              rsp_incomplete = FALSE;
            }
          }
        }
        diagpkt_delay_commit((PACK(void *))rsp); /* Committing the reponse to the diag buffer - this gets executed for both completed and incomplete response */
        if( rsp_incomplete )
        {
          rsp_count = rsp_count + 1;
          continue;
        }
      }
      break;
    }
  }
#endif
}

/*===========================================================================
FUNCTION DIAG_CLIENT_MASKCHANGE

DESCRIPTION
   This function is called when there is a event/log/F3 mask change. In modem,
   it is used to notify registered users about the mask change.

RETURNS
   TRUE: If the notification was successful
   FALSE: If the notification failed

============================================================================*/
boolean diag_client_maskchange(int mask_type)
{
  int return_val;
  switch(mask_type)
  {
     case DIAG_EVENTSVC_MASK_CHANGE:
              DIAG_SET_MASK(diag_mask_notify,DIAG_EVENT_MASK_NOTIFY_BIT);
              break;
     case DIAG_LOGSVC_MASK_CHANGE:
              DIAG_SET_MASK(diag_mask_notify,DIAG_LOG_MASK_NOTIFY_BIT);
              break;
     case DIAG_MSGSVC_MASK_CHANGE:
              DIAG_SET_MASK(diag_mask_notify,DIAG_MSG_MASK_NOTIFY_BIT);
              break;
     default: MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Bad parameters passed into diag_client_maskchange %d for proc id %d",mask_type, DIAG_MY_PROC_ID);
              return FALSE;
  }
   if (osal_get_remaining_time (&diag_mask_change_notify_timer) == 0)
   {
     return_val =  osal_set_timer(&diag_mask_change_notify_timer, DIAG_MASK_NOTIFY_TIMER_LEN);
     ASSERT(OSAL_SUCCESS == return_val);
   }
   return TRUE;
}

/*===========================================================================
FUNCTION DIAGPKT_RESET_DROP_COUNT_DELAY

DESCRIPTION
    This function resets the internal variables diagpkt_delay_drop_cnt and
    diagpkt_delay_alloc_cnt.
===========================================================================*/
void diagpkt_reset_drop_count_delay( void ) {
    diagpkt_delay_drop_cnt = 0;
    diagpkt_delay_alloc_cnt = 0;
}
/*===========================================================================
FUNCTION DIAGPKT_GET_DROP_COUNT_DELAY

DESCRIPTION
    This function returns the internal variable diagpkt_delay_drop_cnt.
===========================================================================*/
uint32 diagpkt_get_drop_count_delay( void ) {
    return diagpkt_delay_drop_cnt;
}
/*===========================================================================
FUNCTION DIAGPKT_GET_ALLOC_COUNT_DELAY

DESCRIPTION
    This function returns the internal variable diagpkt_delay_alloc_cnt.
===========================================================================*/
uint32 diagpkt_get_alloc_count_delay( void ) {
    return diagpkt_delay_alloc_cnt;
}
/* Restore error reporting for these symbols */
/*lint +esym(715,pkt_len,req_ptr) */
