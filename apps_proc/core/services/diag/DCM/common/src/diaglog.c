/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Logging Services

General Description
  Services to support data logging.

Initializing and Sequencing Requirements
  'log_init()' must be called once during initialization prior to use.


Copyright (c) 1990-2015, 2019-2020 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                              Edit History

$Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diaglog.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/29/15   ph      Fix KW errors.
12/12/14   sr      Fixed the issue with num_items in log set mask response.
11/07/14   xy      Fixed KW issues.
10/31/14   rs      Replaced EUIP_ID_MAX with LOG_EQUIP_ID_MAX
10/30/14   xy      Fixed issue with the initialization of log_mstr_last_item_tbl()
10/07/14   sa      Update the global diag log mask pointer inside a critical section.
09/12/14   xy      Fixed KW issue
09/03/14   sr      Fixed issues with get log mask command handler
05/27/14   rs      Fixed issues in composite mask update for build mask centralization.
04/24/14   tbg     Fixed issues with extended log pkts.
02/05/14   xy      Fixed KW issue
01/20/14   rh      Aggregate log mask boundaries from peripherals on the apps
01/07/14   xy      Added fusion DCI feature
12/03/13   ph      Update log_composite_mask algorithm to avoid possible concurrency issues.
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0
10/14/13   sr      Added support for extended event, log and F3 pkts
10/01/13   xy      Removed code under FEATURE_WINCE
09/20/13   tbg     Added support for Diag over STM
08/28/13   rh      Calls to log_update_composite_mask() use correct params now
06/14/13   rh      Support for resending mask updates if we run out of memory
06/03/13   sa      Fixed issues responding to V1 and V2 Diag Test Commands.
04/23/13   ph      Replace memcpy() and memmove() with memscpy() and memsmove()
04/01/13   sr      Resolved Klockwork warnings
03/04/13   ph      Updated loop condition in log_sleep_vote().
02/22/13   is      Resolve Klockwork issue
01/30/13   is      Klocwork warning fixes
01/25/13   sr      Fixed compiler warnings
01/21/13   is      Support DIAG_EXT_LOGMASK_F with preset masks
12/07/12   is      Warnings fixes
09/22/12   sr      Support for mask rtvl command
11/27/12   rh      Mainlined DIAG_CENTRAL_ROUTING
11/16/12   ph      Klocwork warning fixes
11/15/12   is      Support for preset masks
10/24/12   rh      Added composite (aggregated) log mask optimization
09/17/12   ra      Fixed the incorrect status in Log On Demand response
08/15/12   vk/ra   Modifications for Log On Demand algorithm change
06/11/12   is      Prevent duplicate responses for "get mask" requests
05/15/12   is      Allow "build time" mask queries from Master Diag
03/07/12   rh      Fixed Klockwork errors
10/25/11   is      Fixed passing in proper stream_id in log_config_mask()
10/07/11   is      Support sending log mask per equipment id
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/15/11   is      Modifed "set mask" via CTRL msgs fot EA support
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
03/18/11   hm      Diag sleep vote bug fix
03/09/11   hm      Dual mask bug fix
12/20/10   mad     Changes for dual mask
12/21/10   hm      Dual mask changes
08/10/10   sg      Moved log_config_mask to diaglogi.h
09/17/10   is      Migrate from deprecated clk driver APIs
08/25/10   sg      Fixed compiler warnings
02/20/10   sg      Merging back DCM WM files.Moved api
                   functionality to log_api.c on AMSS
05/06/10   is      Removed obsolete LOG_CDMA_TIME_FORMAT
02/12/10   vs      Ported diag_time_get from depot
12/28/09   ps      Fixed Klocwork errors & compiler warnings.
11/04/09   cahn    DIAG Health
09/09/09   JV      Changed signature of osal mutex calls.
08/05/09   JV      Removed the CUST_H featurization around the inclusion of
                   customer.h.
07/10/09   as      Decoupled code under FEATURE_DIAG_NON_STREAMING
05/12/09   JV      Introduced the OS abstraction layer for rex.
05/07/09   vk      changed includes from external to internal headers
04/22/09   JV      Cleaned up featurization for timestamps.
10/03/08   vg      Updated code to use PACK() vs. PACKED
                   Removed OR of Frame number into WCDMA packets
12/15/06   as      Fixed compiler warnings.
07/13/06   as      Added RPC support for mobileview
05/19/06   as      Added Multi processor modem only support
10/31/05   as      Fixed lint errors.
10/10/05   as      Changed implementation to process configuration files
                   without sending responses.
09/29/05   as      Added check for NULL pointer after diagbuf allocation.
03/09/05   as      Fixed log_on_demand_register to avoid logcode to become
                   double registered.
02/10/05   as      Added FEATURE_DEBUG_DIAG_HEAP to debug memory corruption.
03/02/04   as      Corrected the boundary in Log Set Mask Response.
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
07/30/02   as      Updated log_get_pkt to support FEATRUE_DIAG_NON_STREAMING.
07/30/02   lad     Added default values for last items for each ID in case
                   they are not defined in diagtune.h.
03/18/02   gr      Modified log_set_timestamp to call log_set_timestamp_ex.
03/15/02   gr      Added code to stuff frame number bits into the timestamp
                   field of the log header for WCDMA.
01/18/02   as      Added implementation for Log On Demand
01/17/02   lad     Fixed array size for log_mask_offset_tbl[] and
                   log_nice_offset_tbl[].
10/24/01   jal     Cleaned up some compiler warnings.
09/12/01   lad     Fixed log_process_config_pkt to correctly set log masks.
06/27/01   lad     Overhauled logging service to support multiple equipment
                   IDs and general cleanup.
                   Added FEATURE_LOG_EQUIP_ID_XLATE to allow old tools to
                   set log codes with equip ID != 1 using temp equip ID 1
                   codes.
04/17/01   lad     Added use of log_code_type instead of uint16.
04/06/01   lad     Changed use of FEATURE_DIAG_MSP_BUILD to FEATURE_ZREX_TIME.
02/23/01   lad     Updated for diag re-architecture.
12/05/00   lad     Added new logging API.
11/10/00   jq      Added support for analog logging.
11/02/00   lad     Makeover + API enhancements.
08/31/00   lad     Optimized logging API funcs.
                   Added logmask processing and logmask diag pkt functions.
07/31/00   lad     Added MSM5000_IRAM_FWD for log_submit().
05/22/00   lad     Added valid mask support for extended logmask.
05/15/00   lad     Changed log_nice default to 0.
05/10/00   lad     Added load balancing support.  Updated diagbuf API.
04/17/00   lad     FEATURE_DIAG_V7_LOG_COMPATIBILITY to use this diag with
                   old DMSS targets.
04/11/00   lad     Moved log_status to log.h as INLINE function.
02/28/00   lad     Made changes to use diagbuf allocation manager.
11/05/98   lad     Made necessary changes for ARM port.
05/03/95   rdh     Added log_status function.
08/31/92   gb      DMSS ver.
03/05/91   rdb     Created module.

===========================================================================*/

#include "comdef.h"

#include "customer.h"

#include "diagbuf_v.h"
#include "diagcmd.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "diagdiag_v.h"
#include "diagtarget.h"
#include "diagtune.h"
#include "log.h"
#include "msg.h"
#include "qw.h"
#include "osal.h"
#include "diagcomm_v.h"
#include "diag_cfg.h"
#include "diagcomm_ctrl.h" /* For DIAG_CTRL_MSG_EQUIP_LOG_MASK */
#include <stringl/stringl.h>

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h"
#endif

#include "feature.h" /* for DIAG_IS_INT_FEATURE_BIT_SET */

#include <string.h>
#include <stdlib.h>

#include "diaglogi.h"
#include "log_arrays_i.h"

osal_mutex_arg_t         log_on_demand_mutex;
osal_mutex_arg_t         log_preset_mask_cs;

#ifdef DIAG_LISTENERS_SUPPORT
  extern void log_listener_notify (uint32 seq_num, void *log, diag_searchlist_node_type *node);
#endif /* DIAG_LISTENERS_SUPPORT */

extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */

/* -------------------------------------------------------------------------
 * Definitions and Declarations
 * ------------------------------------------------------------------------- */

extern uint8 diag_cur_preset_id; /* from diag.c */

/* The log_on_demand list holds the entries for log_code and
 * function pointer. */
log_on_demand_list_item_type log_on_demand_list[LOG_ON_DEMAND_SIZE];

typedef struct{
  uint8 cmd_code; /* cmd code for extended pkts */
  uint8 version;  /* version of the extended pkt; currently we support version 1 */
  uint16 proc_id; /* processor ID where the msg is being generated */
  uint32 id;      /* ID passed in by user */
}log_ext_hdr_type;

/* This array contains the log masks for all equipment IDs. */
/*lint -save  -e572 */
byte * log_mask[DIAG_MAX_STREAM_ID];
byte diag_log_preset_mask[DIAG_MAX_PRESET_ID][LOG_MASK_SIZE];    /* Stream 1 QXDM masks. */
#ifdef DIAG_CONSUMER_API
  byte diag_log_dci_mask[LOG_MASK_SIZE];                           /* Stream 2 DCI mask. */
#endif /* #ifdef DIAG_CONSUMER_API */
byte log_composite_mask[LOG_MASK_SIZE];
#ifdef DIAG_LISTENERS_SUPPORT
  byte log_listener_mask[LOG_MASK_SIZE];
#endif /* DIAG_LISTENERS_SUPPORT */

#if defined(DIAG_MP_MASTER)
/* Defines and variables for saving to master log mask */
#define MSTR_LOG_MASK_SIZE 500
#define MSTR_LOG_MAX_LAST_ITEM (MSTR_LOG_MASK_SIZE << 3)

typedef struct
{
  uint8 status;
  byte mask[LOG_EQUIP_ID_MAX+1][MSTR_LOG_MASK_SIZE]; /* LOG_EQUIP_ID_MAX is max log equip id, but it starts at 0, so need to add 1 for size */
} diag_log_mask_type;

diag_log_mask_type * diag_log_mask_mstr[DIAG_MAX_STREAM_ID];
diag_log_mask_type diag_log_mask_mstr_preset[DIAG_MAX_PRESET_ID];    /* Stream 1 QXDM Master masks. */
#ifdef DIAG_CONSUMER_API
  diag_log_mask_type diag_log_mask_mstr_dci[DIAG_MAX_DCI_ID];          /* Stream 2 DCI Master mask. */
#endif /* #ifdef DIAG_CONSUMER_API */

uint16 diag_log_last_item_mstr_tbl[LOG_EQUIP_ID_MAX+1];
#endif

static const byte log_valid_mask[LOG_MASK_SIZE];
/*lint -restore */

/* The content of this table are computed in log_init().  'uint16' is used
 * instead of 'int' to reduce data bus cycles to access an element of this
 * array.  The last entry is the offset to the byte following the array.  It
 * is used for range checking. */
uint16 log_mask_offset_tbl[LOG_EQUIP_ID_LAST+1];


log_state_enum_type log_state = LOG_INIT_STATE;
void *log_commit_last = NULL; /* Many writers, 1 reader (DIAG) */
void *log_flush_last = NULL; /* 1 writer, 1 reader (both DIAG) */

#ifdef DIAG_LISTENERS_SUPPORT
  extern diag_searchlist_type log_listeners;
#endif /* DIAG_LISTENERS_SUPPORT */

#if defined(DIAG_MP_MASTER)
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */

uint16 log_mstr_last_item_tbl[LOG_EQUIP_ID_MAX + 1];
uint8 log_equip_id_last = LOG_EQUIP_ID_LAST_DEFAULT;
#endif

/* There are two purposes of this variable
   1. To know if there is any client registred for log on demand
   2. To know how many clients have registred for log on demand
      hence type int
*/
uint8 log_on_demand_reg_count;

/* -------------------------------------------------------------------------
 * Internal (aka private) function prototypes
 * ------------------------------------------------------------------------- */
boolean log_status_mask (log_code_type code, byte stream_id);
static boolean log_search_mask (unsigned int id, unsigned int item, byte stream_id);
void log_update_composite_mask(uint32 start, uint32 end);
boolean log_mask_enabled (const byte *xx_mask_ptr, unsigned int xx_id,
                                 unsigned int xx_item);
static log_code_type log_set_equip_id (unsigned int code,
                                       unsigned int equip_id);

static boolean log_sleep_vote (void);
static void log_service_outbound (diag_send_desc_type *desc,byte stream_id);

PACK(void *) diaglog_process_ext_config_req (PACK(void *) req_pkt_ptr, uint16 pkt_len);

/* -------------------------------------------------------------------------
 * External (aka public) Function Implementations
 * ------------------------------------------------------------------------- */
/*===========================================================================

FUNCTION LOG_ON_DEMAND_LIST_INIT

DESCRIPTION
  Initialize log on demand list.

===========================================================================*/
static void log_on_demand_list_init (void)
{
  int i;

  // Optimization variable to see if some clients have registered for log on demand
  log_on_demand_reg_count = 0;

  for (i = 0; i < LOG_ON_DEMAND_SIZE; i++)
  {
    log_on_demand_list[i].log_code = 0;
    log_on_demand_list[i].log_on_demand_ptr = NULL;
  }
}


/*===========================================================================

FUNCTION LOG_INIT

DESCRIPTION
  Initialize log masks.

RETURN VALUE
  None

===========================================================================*/
void log_init (void)
{
  int id, id1, return_val = 0;

#if defined(DIAG_MP_MASTER)
  uint8 port_num = 0;
#endif

  ASSERT(sizeof(log_header_type) == sizeof(log_hdr_type));
  ASSERT(LOG_EQUIP_ID_LAST <= LOG_EQUIP_ID_MAX);

  log_mask_offset_tbl[0] = 0;

  for (id = 0; id < LOG_EQUIP_ID_LAST; id++)
  {
    log_mask_offset_tbl[id+1] = 1 + log_mask_offset_tbl[id] +
      LOG_MASK_ARRAY_INDEX(log_last_item_tbl[id]);
  }
#if defined(DIAG_MP_MASTER)
  for (id = 0; id <= LOG_EQUIP_ID_LAST; id++)
  {
    diag_log_last_item_mstr_tbl[id] = 0;

    log_mstr_last_item_tbl[id] = log_last_item_tbl[id];
  }
#endif


  /* Point local masks to initial preset masks. */
  log_mask[DIAG_STREAM_1-1] = &diag_log_preset_mask[diag_cur_preset_id-1][0];
  #ifdef DIAG_CONSUMER_API
    log_mask[DIAG_STREAM_2-1] = &diag_log_dci_mask[0];
  #endif /* #ifdef DIAG_CONSUMER_API */

#if defined(DIAG_MP_MASTER)
  /* Point Master masks to initial Master preset masks. */
  diag_log_mask_mstr[DIAG_STREAM_1-1] = &diag_log_mask_mstr_preset[diag_cur_preset_id-1];
  #ifdef DIAG_CONSUMER_API
    diag_log_mask_mstr[DIAG_STREAM_2-1] = &diag_log_mask_mstr_dci[DIAG_DCI_MASK_1-1];
  #endif /* #ifdef DIAG_CONSUMER_API */

  for( port_num = 0; port_num < NUM_SMD_PORTS; port_num++ )
  {
    /* Point log mask update array to intial preset arrays. */
    diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_update[DIAG_STREAM_1-1] =
        diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_preset_update[diag_cur_preset_id-1];
  #ifdef DIAG_CONSUMER_API
    diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_update[DIAG_STREAM_2-1] =
    diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_dci_update[DIAG_DCI_MASK_1-1];
  #endif /* #ifdef DIAG_CONSUMER_API */
  for(id = DIAG_MIN_PRESET_ID; id <= DIAG_MAX_PRESET_ID; id++)
  {
    diag_log_mask_mstr_preset[id-1].status = DIAG_CTRL_MASK_INVALID;
  }
    for( id = 0; id <= LOG_EQUIP_ID_MAX; id++ )
    {
    #ifdef DIAG_CONSUMER_API
      diag_log_mask_mstr_dci[DIAG_DCI_MASK_1-1].status = DIAG_CTRL_MASK_INVALID;
    #endif /* #ifdef DIAG_CONSUMER_API */
    for( id1 = DIAG_MIN_PRESET_ID; id1 <= DIAG_MAX_PRESET_ID; id1++ )
    {
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_preset_update[id1-1][id] = FALSE;
    }
    #ifdef DIAG_CONSUMER_API
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_dci_update[DIAG_DCI_MASK_1-1][id] = FALSE;
    #endif /* #ifdef DIAG_CONSUMER_API */
    }
  }
#endif

  /* These masks are cleared with memset because the listeners do not have a
     config function like log_config_mask to do so, nor is there a listener init. */
  memset(log_composite_mask, 0, sizeof(log_composite_mask));
#ifdef DIAG_LISTENERS_SUPPORT
  memset(log_listener_mask, 0, sizeof(log_listener_mask));
#endif /* DIAG_LISTENERS_SUPPORT */

#if !defined(T_WINNT)
  (void) log_config_mask (-1, FALSE, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);
  log_config_mstr_mask (-1, FALSE, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);
  diag_sleep_vote (DIAG_SLEEP_LOG, TRUE);
#else
  /* Unit test environments default to on */
  log_config_mask (-1, TRUE, DIAG_STREAM_1, DIAG_PRESET_MASK_ALL);
  log_config_mstr_mask (-1, TRUE, DIAG_STREAM_1, DIAG_PRESET_MASK_ALL);
  diag_sleep_vote (DIAG_SLEEP_LOG, FALSE);
#endif

  log_on_demand_list_init();

  (void) diagbuf_register_outbound_pkt_handler (DIAG_LOG_F, log_service_outbound);

  log_on_demand_mutex.name = "MUTEX_LOG_ON_DEMAND";
  return_val = osal_init_mutex(&log_on_demand_mutex);
  ASSERT(OSAL_SUCCESS == return_val);


  log_preset_mask_cs.name = "MUTEX_LOG_PRESET_MASK_CS";
  return_val = osal_init_mutex(&log_preset_mask_cs);
  ASSERT(OSAL_SUCCESS == return_val);

  /* Now that init is complete, enable normal operation */
  log_state = LOG_NORMAL_STATE;

} /* log_init */


/*===========================================================================

FUNCTION LOG_CONFIG_MASK

DESCRIPTION
  This function selects which log codes are enabled for logging.

PARAMETERS
  code - Specifies which log code is being referenced. Should be one of
         the enumerated log codes defined in log_codes.h.
           -1 means entire mask.
  flag - Specifies whether to enable (TRUE) or disable (FALSE) logging
         for the specified log code.
  stream_id - Specifies which stream to enable/disable.
               Valid stream_id range: DIAG_MIN_STREAM_ID to DIAG_MAX_STREAM_ID
               Set all streams: DIAG_STREAM_ALL
  preset_id - Specifies preset_id for Stream 1 log mask.
              Valid preset_id range: DIAG_MIN_PRESET_ID to DIAG_MAX_PRESET_ID
              Set all presets: DIAG_PRESET_MASK_ALL

RETURN VALUE
  TRUE if the operations was succesful.
  FALSE if an invalid code was specified.

WARNING
  Please update log_config_mstr_mask() if log_config_mask() is updated.

===========================================================================*/
boolean log_config_mask (int code, boolean  flag, byte stream_id, uint8 preset_id)
{
  boolean return_status = FALSE;
  uint8 preset_index = 0;
  unsigned int id, item;
  unsigned int index, start, end;
  byte mask = (flag) ? 0xFF : 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This will update entire composite log mask range */
  start = 0;
  end = LOG_MASK_SIZE -1;

  if (code == -1) //sets entire mask
  {
    if( (stream_id == DIAG_STREAM_1) || (stream_id == DIAG_STREAM_ALL) )
    {
      if( preset_id == DIAG_PRESET_MASK_ALL )
      {
        for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
        {
          for( index = 0; index < LOG_MASK_SIZE; index++ )
            diag_log_preset_mask[preset_index][index] = mask;
        }
      }
      else
      {
        for( index = 0; index < LOG_MASK_SIZE; index++ )
          diag_log_preset_mask[preset_id-1][index] = mask;
      }
    } //end set stream 1 all
  #ifdef DIAG_CONSUMER_API
    if( (stream_id == DIAG_STREAM_2) || (stream_id == DIAG_STREAM_ALL) )
    {
      for( index = 0; index < LOG_MASK_SIZE; index++ )
        diag_log_dci_mask[index] = mask;
    } //end set stream 2 all
  #endif /* #ifdef DIAG_CONSUMER_API */
    return_status = diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);
  }
  else if( code > 0 )
  {
    /* Get ID and item from log code */
    id = LOG_GET_EQUIP_ID(code);
    item = LOG_GET_ITEM_NUM(code);

    /* Check if code is valid */
    if ((item <= log_last_item_tbl[id]) && (id <= LOG_EQUIP_ID_LAST))
    {
      index = log_mask_offset_tbl[id] + LOG_MASK_ARRAY_INDEX(item);
      mask = LOG_MASK_BIT_MASK(item);

      if( (stream_id == DIAG_STREAM_1) || (stream_id == DIAG_STREAM_ALL) )
      {
        if( preset_id == DIAG_PRESET_MASK_ALL )
        {
          for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
          {
            if( flag )
              diag_log_preset_mask[preset_index][index] |= mask;
            else
              diag_log_preset_mask[preset_index][index] &= ~mask;
          }
        }
        else
        {
          if( flag )
            diag_log_preset_mask[preset_id-1][index] |= mask;
          else
            diag_log_preset_mask[preset_id-1][index] &= ~mask;
        }
      } //end set stream 1 all
   #ifdef DIAG_CONSUMER_API
      if( (stream_id == DIAG_STREAM_2) || (stream_id == DIAG_STREAM_ALL) )
      {
        if( flag )
          diag_log_dci_mask[index] |= mask;
        else
          diag_log_dci_mask[index] &= ~mask;
      } //end set stream 2 all
   #endif /* #ifdef DIAG_CONSUMER_API */
      /* This will update composite log mask for specified range */
      start = index;
      end = index;
    }
    else
    {
      return_status = FALSE;
    }
  }

  log_update_composite_mask(start, end);
  return return_status;

} /* log_config_mask */


/*===========================================================================

FUNCTION LOG_CONFIG_MSTR_MASK

DESCRIPTION
  This function saves the master log mask for sending to slaves.

PARAMETERS
  code - Specifies which log code is being referenced. Should be one of
         the enumerated log codes defined in log_codes.h.
           -1 means entire mask.
  flag - Specifies whether to enable (TRUE) or disable (FALSE) logging
         for the specified log code.
  stream_id - Specifies which stream to enable/disable.
               Valid stream_id range: DIAG_MIN_STREAM_ID to DIAG_MAX_STREAM_ID
               Set all streams: DIAG_STREAM_ALL
  preset_id - Specifies preset_id for Stream 1 log mask.
              Valid preset_id range: DIAG_MIN_PRESET_ID to DIAG_MAX_PRESET_ID
              Set all presets: DIAG_PRESET_MASK_ALL

RETURN VALUE
  None

WARNING
  Please update this function if log_config_mask() is updated.

===========================================================================*/
void log_config_mstr_mask ( int code, boolean flag, byte stream_id, uint8 preset_id )
{
#if defined(DIAG_MP_MASTER)
  unsigned int id, item;
  unsigned int index;
  byte mask = (flag) ? 0xFF : 0;
  uint8 port_num, preset_index = 0;
  uint8 status;

  ASSERT( stream_id <= DIAG_MAX_STREAM_ID );
  ASSERT( preset_id <= DIAG_MAX_PRESET_ID);

  if( code == -1 ) //Sets entire mask
  {
    status = (mask) ? DIAG_CTRL_MASK_ALL_ENABLED : DIAG_CTRL_MASK_ALL_DISABLED;

    if( (stream_id == DIAG_STREAM_1) || (stream_id == DIAG_STREAM_ALL) )
    {
      if( preset_id == DIAG_PRESET_MASK_ALL )
      {
        for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
        {
          for( id = 0; id <= LOG_EQUIP_ID_MAX; id++ )
          {
            for( index = 0; index < MSTR_LOG_MASK_SIZE; index++ )
            {
              diag_log_mask_mstr_preset[preset_index].mask[id][index] = mask;
            }
            /* Master needs to keep track of updated log masks to send masks to Slaves */
            for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
            {
              diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_preset_update[preset_index][id] = TRUE;
            }
          }
          diag_log_mask_mstr_preset[preset_index].status = status;
        }
      }
      else
      {
        for( id = 0; id <= LOG_EQUIP_ID_MAX; id++ )
        {
          for( index = 0; index < MSTR_LOG_MASK_SIZE; index++ )
          {
            diag_log_mask_mstr_preset[preset_id-1].mask[id][index] = mask;
          }
          /* Master needs to keep track of updated log masks to send masks to Slaves */
          for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
          {
            diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_preset_update[preset_id-1][id] = TRUE;
          }
        }
        diag_log_mask_mstr_preset[preset_id-1].status = status;
      }
    } //end set stream 1 all

  #ifdef DIAG_CONSUMER_API
    if( (stream_id == DIAG_STREAM_2) || (stream_id == DIAG_STREAM_ALL) )
    {
      for( id = 0; id <= LOG_EQUIP_ID_MAX; id++ )
      {
        for (index = 0; index < MSTR_LOG_MASK_SIZE; index++)
        {
          diag_log_mask_mstr_dci[DIAG_DCI_MASK_1-1].mask[id][index] = mask;
        }

        /* Master needs to keep track of updated log masks to send masks to Slaves */
        for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
        {
          diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_dci_update[DIAG_DCI_MASK_1-1][id] = TRUE;
        }
      }
      diag_log_mask_mstr_dci[DIAG_DCI_MASK_1-1].status = status;
    } //end set stream 2 all
    #endif /* #ifdef DIAG_CONSUMER_API */
  }
  else if( code > 0 )
  {
    diag_log_mask_type * log_mask_mstr_ptr = NULL;
    ASSERT( stream_id !=0 );
    ASSERT( preset_id !=0 );
    if( stream_id == DIAG_STREAM_1 )
      log_mask_mstr_ptr = &diag_log_mask_mstr_preset[preset_id-1];
  #ifdef DIAG_CONSUMER_API
    else if( stream_id == DIAG_STREAM_2 )
      log_mask_mstr_ptr = &diag_log_mask_mstr_dci[DIAG_DCI_MASK_1-1];
  #endif /* #ifdef DIAG_CONSUMER_API */



    /* Get ID and item from log code */
    id = LOG_GET_EQUIP_ID(code);
    item = LOG_GET_ITEM_NUM(code);

    if( (id <= LOG_EQUIP_ID_MAX  ) && (item < MSTR_LOG_MAX_LAST_ITEM) && (log_mask_mstr_ptr != NULL))
    {
      log_mask_mstr_ptr->status = DIAG_CTRL_MASK_VALID;
      index = LOG_MASK_ARRAY_INDEX(item);
      mask = LOG_MASK_BIT_MASK(item);

      if( flag )
        log_mask_mstr_ptr->mask[id][index] |= mask;
      else
        log_mask_mstr_ptr->mask[id][index] &= ~mask;

      if( diag_log_last_item_mstr_tbl[id] < item )
        diag_log_last_item_mstr_tbl[id] = item;

      /* Master needs to keep track of updated log masks to send masks to Slaves */
      for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
      {
        if( stream_id == DIAG_STREAM_1 )
          diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_preset_update[preset_id-1][id] = TRUE;
    #ifdef DIAG_CONSUMER_API
        else if( stream_id == DIAG_STREAM_2 )
          diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_dci_update[DIAG_DCI_MASK_1-1][id] = TRUE;
    #endif /* #ifdef DIAG_CONSUMER_API */
      }
    }
  }
#endif /* DIAG_MP_MASTER */

  return;

} /* log_config_mstr_mask */


/*===========================================================================

FUNCTION LOG_FLUSH

DESCRIPTION
  Prepares the logging service for a flush.  The latest log that was
  committed is marked to be sent.  All others are discarded.

===========================================================================*/
void
log_flush (void)
{
  if (log_state == LOG_NORMAL_STATE)
  {
    /* log_commit_last has multiple writers and is written prior to signalling
       the DIAG task.  This routine, however, is called from DIAG task only.
       Therefore, DIAG is the only writer for log_flush_last which is
       captured here to mark the last log to send before declaring a flush
       complete.  Once sent, it is sent to NULL by log_service_outbound,
       also by the DIAG task. */
    log_flush_last = log_commit_last;

    if (log_flush_last)
    {
      log_state = LOG_FLUSH_STATE;
    }
  }
}

/*===========================================================================

FUNCTION LOG_PANIC

DESCRIPTION
  Prepares the logging service for panic mode.  All logs are configured off.
  All listeners are deleted.

===========================================================================*/
void
log_panic (void)
{
  if (log_state != LOG_INIT_STATE)
  {
    /* log_commit_last has multiple writers and is written prior to signalling
       the DIAG task.  This routine, however, is called from DIAG task only.
       Therefore, DIAG is the only writer for log_flush_last which is
       captured here to mark the last log to send before declaring a flush
       complete.  Once sent, it is sent to NULL by log_service_outbound,
       also by the DIAG task. */
    log_flush_last = log_commit_last;

    if (log_flush_last)
    {
      log_state = LOG_PANIC_STATE;
    }
    else
    {
      log_state = LOG_INIT_STATE; /* Disable */
    }
  }
}



/*===========================================================================

FUNCTION LOG_STATUS_MASK

DESCRIPTION
  This routine is a wrapper for log_search_mask().  It is used to look up
  the given code in the log mask for the given stream.

RETURN VALUE
  A boolean indicating if the specified log is enabled.

===========================================================================*/
boolean
log_status_mask (log_code_type code, byte stream_id)
{
  unsigned int id, item;
  boolean status = FALSE;

  id = LOG_GET_EQUIP_ID (code);
  item = LOG_GET_ITEM_NUM (code);

  if ( stream_id & DIAG_STREAM_1) {

   status = log_search_mask (id, item, DIAG_STREAM_1);
  }
  #ifdef DIAG_CONSUMER_API
  if ( stream_id & DIAG_STREAM_2) {

   status |= log_search_mask (id, item, DIAG_STREAM_2);
  }
  #endif /* #ifdef DIAG_CONSUMER_API */

  return status;
}



/*===========================================================================

FUNCTION LOG_SERVICE_OUTBOUND

DESCRIPTION
  This routine is called by diagbuf when it starts a new DIAG_LOG_F packet.
  This routine handles log listeners as well as sending to the comm layer.

  This routine also handles various 'log_state's that are used for
  preserving listener state while flushing logs to the comm layer.

===========================================================================*/
static void
log_service_outbound (diag_send_desc_type *desc,byte stream_id)
{
  const diag_log_rsp_type *log;
  log_code_type code;
  diag_send_state_enum_type state;
#ifdef DIAG_LISTENERS_SUPPORT
  unsigned int id, item;
#endif /* DIAG_LISTENERS_SUPPORT */
  log_ext_hdr_type *log_hdr = NULL;
  diagbuf_header_type *diagbuf_hdr1 = NULL;
  static diag_send_desc_type send = {NULL, NULL, DIAG_SEND_STATE_START, FALSE};
  static boolean log_ext_hdr_sent = FALSE;

  if (desc)
  {
    state = desc->state;

    if (state == DIAG_SEND_STATE_START)
    {
      if (log_state == LOG_INIT_STATE)
      {
        /* Skip immediately */
        state = DIAG_SEND_STATE_COMPLETE;
      }
      else
      {
        diagbuf_hdr1 = (diagbuf_header_type *) desc->pkt;

        /*point to the diagbuf header*/
        diagbuf_hdr1 -= 1;

        /* Processing for chained diagbuf entries (extended logs) */
        if((diagbuf_hdr1->version == DIAGBUF_VERSION_1 ) & (diagbuf_hdr1->chain == DIAGBUF_FIRST_PKT_CHAIN))
        {
          if( !( stream_id&DIAG_STREAM_1 ) )
          {
            /* Drop extended header if STREAM_1 is not active.
               Not currently supported by listeners or by DCI */
            desc->pkt = (void *)((byte *)desc->last + 1);
            desc->state = DIAG_SEND_STATE_COMPLETE;
            return;
          }

          /* Send out extended packet as needed to STREAM_1 only */
          log_hdr = (log_ext_hdr_type *) desc->pkt;
          log_hdr->cmd_code = DIAG_CMD_EXT_F;
          log_hdr->version = ((log_ext_hdr_type *)desc->pkt)->version;
          log_hdr->proc_id = ((log_ext_hdr_type *)desc->pkt)->proc_id;
          log_hdr->id = ((log_ext_hdr_type *)desc->pkt)->id;
          send.pkt = (void *) &log_hdr->cmd_code;
          send.last = (void *)( (uint32)send.pkt + sizeof(log_ext_hdr_type)-1);
          send.terminate = FALSE;
          send.state = DIAG_SEND_STATE_START;

          /* Send extended packet header, but mask out DCI STREAM until it supports extended packets */
      #ifdef DIAG_CONSUMER_API
        diagbuf_send_pkt(&send, (stream_id & ~DIAG_STREAM_2));
      #else
        diagbuf_send_pkt(&send, stream_id);
      #endif /* #ifdef DIAG_CONSUMER_API */
      
          if (send.state == DIAG_SEND_STATE_COMPLETE)
          {
            desc->pkt = (void *)((byte *)desc->last + 1);
            desc->state = DIAG_SEND_STATE_COMPLETE;
            send.pkt = NULL;
          }
          /* Set flag indicating first portion of extended log packet sent */
          log_ext_hdr_sent = TRUE;
          return;
        }
        log = (diag_log_rsp_type *) desc->pkt;
        code = log_get_code ((void *) log->log);


        /* Check listener masks before searching listener lists */
        #ifdef DIAG_LISTENERS_SUPPORT
        id = LOG_GET_EQUIP_ID (code);
        item = LOG_GET_ITEM_NUM (code);

        if (log_mask_enabled(log_listener_mask, id, item)) {
          (void) diag_searchlist_search_all (code,
                                             &log_listeners,
                                             (void *) log->log,
                                             log_listener_notify);
        }
        #endif /* DIAG_LISTENERS_SUPPORT */


        /* Check if this is the last item for a pending flush operation. */
        if (!log_status_mask (code, stream_id))
        {
          /* Mark complete - don't send to comm layer. */
          state = DIAG_SEND_STATE_COMPLETE;
        }

        /* log_commit_last has multiple writers.  log_commit_last is always
           captured before committing to the queue.  Therefore, it is always
           ahead of log->log in the queue.  Therefore, one can assume that if
           this match succeeds, log_commit_last is no longer pointing to a
           valid item in the queue, and must be cleared. */
        if ((void *) log->log == log_commit_last)
        {
          /* In the off chance DIAG is preempted by a writer of
             'log_commit_last' between the above check and writing NULL here,
         the side effect is only that the log may be ignored by a flush
             operation when it copies it to log_flush_last.  DIAG is the
             only reader of this variable, so there is no risk of capturing
             a corrupt value for log_flush_last when captured from this
             variable.  This is a tolerable side effect.  The only loss is
             that any log generated from a preemption here will eb lost if a
             flush begins before that log is sent.  The tradeoff would be to
             require INTLOCK around this 'if' block. */
          log_commit_last = NULL;
        }

        if (log_flush_last)
        {
          log_flush_last = NULL;

          /* Flush or panic mode will always disable comm bound logs.
             Panic moves to 'init' state to halt the svc when complete. */
          if (log_state == LOG_FLUSH_STATE)
          {
            (void) log_config_mask (-1, FALSE, DIAG_STREAM_ALL, diag_cur_preset_id);
            log_config_mstr_mask (-1, FALSE, DIAG_STREAM_ALL, diag_cur_preset_id);
            log_state = LOG_NORMAL_STATE;
          }
          else if (log_state == LOG_PANIC_STATE)
          {
            (void) log_config_mask (-1, FALSE, DIAG_STREAM_ALL, diag_cur_preset_id);
            log_config_mstr_mask (-1, FALSE, DIAG_STREAM_ALL, diag_cur_preset_id);
            log_state = LOG_INIT_STATE; /* disable logging */
          }
          else
          {
            MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"flush_last set in wrong state");
          }
        }
      }
    }

    if (state == DIAG_SEND_STATE_COMPLETE)
    {
      /* Skip this packet - no tool asked for it. */
      desc->pkt = (void *) (((uint8 *) desc->last) + 1);
      desc->state = state;
    }
    else
    {
      /* This routine will transition state even if there if no sending can be
         done.  Therefore, this routine can assume that state == "start" will
         only happen once. */
      if( log_ext_hdr_sent )
      {
        desc->state = DIAG_SEND_STATE_BUSY;
        log_ext_hdr_sent = FALSE;
      }
      (void) diagbuf_send_pkt (desc,stream_id);
    }
  }

  return;
}


/* -------------------------------------------------------------------------
 * Internal to diag (aka protected) function prototypes
 * ------------------------------------------------------------------------- */
/* Begin packet processing routines */
/*===========================================================================

FUNCTION LOG_PROCESS_CONFIG_PKT

DESCRIPTION
  This procedure processes a request to set the log mask.

  Once again the log service config packet has evolved.  This time it has
  evolved to be able to handle multiple equipment IDs.  In the past, the MSM
  always assumed the equipment ID is 1.  Multi-mode products, different
  technologies can log with the same service using equipment ID to specify the
  technology.  This means we have 16 masks, one for each equipment ID.

  The extended logmask packet was designed using a very odd bit ordering scheme
  that created a lot of confusion.  To alleviate this confusion, the logmask is
  now a byte array, with each bit being ordered LSB to MSB.  Macros are defined
  to perform these operations.

PARAMETERS
  req_pkt_ptr - Pointer to request packet
  pkt_len - Length of request packet

RETURN VALUE
  Pointer to response packet.

  This command effects what type of entries are logged.
===========================================================================*/
PACK(void *) log_process_config_pkt (
  PACK(void *) req_pkt_ptr,
  uint16 pkt_len
)
{
  /* For cmd DIAG_LOG_CONFIG_F 115 - Supports Stream 1 */
  DIAG_LOG_CONFIG_F_req_type *req_ptr = (DIAG_LOG_CONFIG_F_req_type *)req_pkt_ptr;
  DIAG_LOG_CONFIG_F_rsp_type *rsp_ptr = NULL;
  /* For Diag subsys cmd DIAG_EXT_LOG_CONFIG 75 18 98 - Supports all streams */
  log_ext_config_req_type *req_dm_ptr = NULL;
  log_ext_config_rsp_type *rsp_dm_ptr = NULL;

  /* Pointers to unions for operation specific data (For non-dual-mask Packets)*/
  log_config_op_req_type *req_op_ptr = NULL;
  log_config_op_rsp_type *rsp_op_ptr = NULL;

  /* Local working variables */
  unsigned int rsp_len, operation, last_item, num_items, equip_id = 0;
  uint16 i;
  byte stream_id = 1;
  int dual_mask;
  byte *mask_rd_ptr = NULL;
  byte *mask_wr_ptr = NULL;
  boolean send_rsp = TRUE;
  log_config_status_enum_type status = LOG_CONFIG_SUCCESS_S;

  /* Sanity checks */
  if( (req_pkt_ptr == NULL) || (pkt_len == 0) )
  {
    return NULL;
  }

  dual_mask = (req_ptr->xx_header.opaque_header == DIAG_LOG_CONFIG_F) ? 0 : 1;

  if( dual_mask )
  {
    req_dm_ptr = (log_ext_config_req_type *)req_pkt_ptr;

    if( req_dm_ptr->header.cmd_version == 2 )
    {
      return diaglog_process_ext_config_req(req_pkt_ptr, pkt_len); //ToDo: separate function for now
    }
    else if( req_dm_ptr->header.cmd_version != 1 || req_dm_ptr->header.opcode == LOG_CONFIG_RETRIEVE_VALID_MASK_OP )
    {
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
    }
  }
  else
  {
    req_op_ptr = (log_config_op_req_type *)req_ptr->op_data;
  }

  rsp_len = FPOS(DIAG_LOG_CONFIG_F_rsp_type, op_data);
  operation = (dual_mask) ? req_dm_ptr->header.opcode : req_ptr->operation;

  /* First, determine the length of the response.
   * Second, allocate the response.
   * Last, fill in the response. */

  /* This switch statement computes rsp_len for the operation */
  switch( operation )
  {
    case LOG_CONFIG_DISABLE_OP:
      rsp_len = dual_mask ? sizeof(log_ext_config_disable_op_rsp) : rsp_len;
      if( dual_mask )
      {
        // Stream ID 0 is acceptable in this case (to disable all the streams).
        if( req_dm_ptr->disable_op.id.stream_id > DIAG_MAX_STREAM_ID )
        {
          status = LOG_CONFIG_INVALID_STREAM_ID;
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
        }
      }
      /* No response op data */
      break;

    case LOG_CONFIG_RETRIEVE_ID_RANGES_OP:
      rsp_len = dual_mask ? sizeof(log_ext_config_id_ranges_rsp) : (rsp_len + sizeof(log_config_ranges_rsp_type));
      break;

    case LOG_CONFIG_RETRIEVE_VALID_MASK_OP:
      equip_id = req_op_ptr->valid_mask.equip_id;
      if (equip_id > LOG_EQUIP_ID_MAX) {
        status = LOG_CONFIG_INVALID_EQUIP_ID_S;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      rsp_len += FPOS(log_config_valid_mask_rsp_type, mask) +
                 (LOG_MASK_ARRAY_INDEX(log_mstr_last_item_tbl[equip_id]) + 1) * sizeof(byte);
      break;

    case LOG_CONFIG_SET_MASK_OP:
      equip_id = dual_mask ? req_dm_ptr->set_mask.code_range.equip_id : req_op_ptr->set_mask.code_range.equip_id;
      if (equip_id > LOG_EQUIP_ID_MAX) {
        status = LOG_CONFIG_INVALID_EQUIP_ID_S;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
      }

      if( dual_mask )
      {
        if( req_dm_ptr->set_mask.id.stream_id < 1 || req_dm_ptr->set_mask.id.stream_id > DIAG_MAX_STREAM_ID )
        {
          status = LOG_CONFIG_INVALID_STREAM_ID;
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
        }
      }
      /*Save the max set log mask id sent by tool */
      diag_log_last_item_mstr_tbl[equip_id] = dual_mask ? MAX(req_dm_ptr->set_mask.code_range.last_item, diag_log_last_item_mstr_tbl[equip_id]):
                                                          MAX(req_op_ptr->set_mask.code_range.last_item, diag_log_last_item_mstr_tbl[equip_id]);
      /*The number of items in response is the minimum of max defined hard coded master log last item and
        maximum of what the tools has sent and what we support*/
      num_items = MIN(MSTR_LOG_MAX_LAST_ITEM, MAX(diag_log_last_item_mstr_tbl[equip_id], log_mstr_last_item_tbl[equip_id]));
      /*Calculate the response length*/
       rsp_len =  dual_mask ? FPOS(log_ext_config_set_mask_rsp, mask) : ((rsp_len + FPOS(log_config_valid_mask_rsp_type, mask)) +
                (LOG_MASK_INDEX(num_items) * sizeof(byte)));

      break;

    case LOG_CONFIG_GET_LOGMASK_OP:
      equip_id = dual_mask ? req_dm_ptr->get_mask.equip_id : req_op_ptr->valid_mask.equip_id;
      if( equip_id > LOG_EQUIP_ID_MAX )
      {
        status = LOG_CONFIG_INVALID_EQUIP_ID_S;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
      }
      if( dual_mask )
      {
        if( req_dm_ptr->set_mask.id.stream_id < 1 || req_dm_ptr->set_mask.id.stream_id > DIAG_MAX_STREAM_ID )
        {
          status = LOG_CONFIG_INVALID_STREAM_ID;
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
        }
      }
      rsp_len = (dual_mask ? FPOS(log_ext_config_get_mask_rsp, mask) : (rsp_len + FPOS(log_config_valid_mask_rsp_type, mask))) +
                (LOG_MASK_ARRAY_INDEX(log_mstr_last_item_tbl[equip_id]) + 1) * sizeof(byte);

      break;

    default:
      return diagpkt_err_rsp(DIAG_BAD_PARM_F,  req_pkt_ptr, pkt_len);
  } /* end switch for rsp_len computation */


  rsp_ptr = dual_mask ? rsp_ptr : (DIAG_LOG_CONFIG_F_rsp_type *)diagpkt_alloc(DIAG_LOG_CONFIG_F, rsp_len);
  rsp_dm_ptr = dual_mask ? (log_ext_config_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, DIAG_EXT_LOG_CONFIG, rsp_len) : rsp_dm_ptr;


  /* For cmd DIAG_LOG_CONFIG_F 115 - Supports Stream 1 */
  if( !dual_mask )
  {
    if( rsp_ptr == NULL )
    {
      return rsp_ptr;
    }

    rsp_ptr->pad[0] = 0;
    rsp_ptr->pad[1] = 0;
    rsp_ptr->pad[2] = 0;

    rsp_ptr->operation = operation;

    rsp_op_ptr = (log_config_op_rsp_type *)rsp_ptr->op_data;

    /* This switch statement actually performs the operation */
    switch( operation )
    {
      case LOG_CONFIG_DISABLE_OP:
        (void)log_config_mask(-1, FALSE, DIAG_STREAM_1, diag_cur_preset_id);
        log_config_mstr_mask(-1, FALSE, DIAG_STREAM_1, diag_cur_preset_id);
        break;

      case LOG_CONFIG_RETRIEVE_ID_RANGES_OP:
        for( i = 0; i <= LOG_EQUIP_ID_MAX; i++ )
        {
          rsp_op_ptr->ranges.last_item[i] = log_mstr_last_item_tbl[i];
        }
        break;

      case LOG_CONFIG_RETRIEVE_VALID_MASK_OP:
        rsp_op_ptr->valid_mask.code_range.equip_id = equip_id;
        rsp_op_ptr->valid_mask.code_range.last_item = log_mstr_last_item_tbl[equip_id];

        if( equip_id > LOG_EQUIP_ID_LAST )
        {
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
        }

        mask_rd_ptr = (byte *)&log_valid_mask[log_mstr_last_item_tbl[equip_id]];
        mask_wr_ptr = (byte *)rsp_op_ptr->valid_mask.mask;

        for( i = 0; i <= (unsigned int)
             LOG_MASK_ARRAY_INDEX(log_mstr_last_item_tbl[equip_id]); i++ )
        {

          *mask_wr_ptr = *mask_rd_ptr;
          mask_wr_ptr++;
          mask_rd_ptr++;
        }
        break;

      case LOG_CONFIG_SET_MASK_OP:
        rsp_op_ptr->set_mask.code_range.equip_id = equip_id;

        if( equip_id > LOG_EQUIP_ID_LAST )
        {
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
        }

#if defined(DIAG_MP_MASTER)
        /* Save master copy of log mask */
        mask_rd_ptr = (byte *)req_op_ptr->set_mask.mask;
        mask_wr_ptr = diag_log_mask_mstr[DIAG_STREAM_1 - 1]->mask[equip_id]; //&log_mask_mstr[DIAG_STREAM_1-1][equip_id][0];
        /*Save the max set log mask id sent by tool */
        diag_log_last_item_mstr_tbl[equip_id] = MAX(req_op_ptr->set_mask.code_range.last_item, diag_log_last_item_mstr_tbl[equip_id]);
        /*The number of items in response is the maximum of what the tools has sent and what we support*/
        num_items = MIN(MSTR_LOG_MAX_LAST_ITEM, MAX(diag_log_last_item_mstr_tbl[equip_id], log_mstr_last_item_tbl[equip_id]));
        rsp_op_ptr->set_mask.code_range.last_item = num_items;
        diag_log_mask_mstr[DIAG_STREAM_1 - 1]->status = DIAG_CTRL_MASK_VALID;

        /*Copy only upto the mask that is given in the request -- num_items can be
        different than last_item in cases where we support more log IDs than what
        the tools has send us*/
        last_item = MIN(MSTR_LOG_MAX_LAST_ITEM, req_op_ptr->set_mask.code_range.last_item);

        for( i = 0; i < LOG_MASK_INDEX(last_item); i++ )
        {
          *mask_wr_ptr = *mask_rd_ptr;
          rsp_op_ptr->set_mask.mask[i] = *mask_rd_ptr;
          mask_wr_ptr++;
          mask_rd_ptr++;
        }

        /* Zero all bits that are after the last item in the set mask */
        mask_wr_ptr--;
        i--;

        /* (The next boundary of 8) - 1 - (last_item) gives the number of bits
         * at the end of the last byte that are not specified.
         * This line 0's out the bits that remain in the mask that are not
         * included in last_item. */
/*lint -save -e504 Unusual shift operation (unusually formed right argument)*/
/*lint -save -e817 */
        *mask_wr_ptr &= 0xFF >> (((last_item + 8) & ~7) - 1 - last_item);

        /*Zero out all other masks after last item -- if the request has less number of items
        than what we support then we only copy the ones given in the request and zero out the
        rest*/
        mask_wr_ptr++;

        for(i = LOG_MASK_INDEX(last_item) + 1; i< LOG_MASK_INDEX(num_items); i++)
        {
           *mask_wr_ptr = 0;
           rsp_op_ptr->set_mask.mask[i] = *mask_wr_ptr;
           mask_wr_ptr++;
        }

        /* Master needs to keep track to send log masks to Slaves */
        for( i = 0; i < NUM_SMD_PORTS; i++ )
        {
          diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][i].log_mask_update[0][equip_id] = TRUE;
        }
#endif

        /* Set the log mask */
        mask_rd_ptr = (byte *)req_op_ptr->set_mask.mask;
        mask_wr_ptr = &log_mask[0][log_mask_offset_tbl[equip_id]];
        last_item = MIN(log_last_item_tbl[equip_id], req_op_ptr->set_mask.code_range.last_item);

        for( i = 0; i < LOG_MASK_INDEX(last_item); i++ )
        {
          *mask_wr_ptr = *mask_rd_ptr;
          mask_wr_ptr++;
          mask_rd_ptr++;
        }
        /* Zero all bits that are after the last item in the set mask */
        mask_wr_ptr--;
        i--;

        /* (The next boundary of 8) - 1 - (last_item) gives the number of bits
         * at the end of the last byte that are not specified.
         * This line 0's out the bits that remain in the mask that are not
         * included in last_item. */
/*lint -save -e504 Unusual shift operation (unusually formed right argument)*/
/*lint -save -e817 */
        *mask_wr_ptr &= 0xFF >> (((last_item + 8) & ~7) - 1 - last_item);

        /* Zero out all items that were not specified in the request. */
        for( i = last_item + 1; i <= log_last_item_tbl[equip_id]; i++ )
        {
          (void)log_config_mask(log_set_equip_id(i, equip_id), FALSE, DIAG_STREAM_1, diag_cur_preset_id);
        }

        /* Update composite mask */
        log_update_composite_mask(log_mask_offset_tbl[equip_id],
                                  log_mask_offset_tbl[equip_id] + LOG_MASK_ARRAY_INDEX(last_item));
        break;

      case LOG_CONFIG_GET_LOGMASK_OP:
        rsp_op_ptr->get_mask.code_range.equip_id = equip_id;
        rsp_op_ptr->get_mask.code_range.last_item = log_mstr_last_item_tbl[equip_id];

        if( equip_id > LOG_EQUIP_ID_LAST )
        {
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
        }

        mask_rd_ptr = (byte *)&((diag_log_mask_mstr[0])->mask[equip_id]);
        mask_wr_ptr = (byte *)rsp_op_ptr->valid_mask.mask;

        for( i = 0; i <= (unsigned int)
             LOG_MASK_ARRAY_INDEX(log_mstr_last_item_tbl[equip_id]); i++ )
        {

          *mask_wr_ptr = *mask_rd_ptr;
          mask_wr_ptr++;
          mask_rd_ptr++;

        }
        break;
      default:
        break;

    } /* end switch */


    if( (LOG_CONFIG_SUCCESS_S == status) &&
        (LOG_CONFIG_SET_MASK_OP == operation) )
    {
      (void)diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);
    }

    /* Config may have changed.  Run sleep vote routine. */
    (void)log_sleep_vote();

    rsp_ptr->status = status;

    /* if master processor on multi processor build, don't send response */
#if !defined(DIAG_RSP_SEND)
    if( !diagcomm_apps_only_config )
      send_rsp = FALSE;
#endif

#if defined(DIAG_MP_MASTER)
    if( (operation == LOG_CONFIG_SET_MASK_OP) || (operation == LOG_CONFIG_DISABLE_OP) )
    {
      /* MP Master now responds to "set mask" requests */
      send_rsp = TRUE;
    }
    else if( (operation == LOG_CONFIG_RETRIEVE_ID_RANGES_OP) ||
             (operation == LOG_CONFIG_RETRIEVE_VALID_MASK_OP) ||
             (operation == LOG_CONFIG_GET_LOGMASK_OP) )
    {
      /* If we're in buffering mode, Master needs to send out the responses.
       If Modem is not up, Master needs to send out responses as well. */
      send_rsp = TRUE;
    }
#endif

    /* if diag is not connected, don't send response */
    if( !diagcomm_status() )
    {
      send_rsp = FALSE;
    }

    /* return response */
    if( !send_rsp )
    {
      diagpkt_free(rsp_ptr);
      return NULL;
    }
    else
    {
      return ((void *)rsp_ptr);
    }
  }
  else
  {
    /* Dual mask */
    if( req_dm_ptr == NULL || rsp_dm_ptr == NULL )
    {
      return NULL;
    }
    rsp_dm_ptr->header = req_dm_ptr->header;
    switch( operation )
    {
      case LOG_CONFIG_DISABLE_OP:
        rsp_dm_ptr->disable_op.status = status;
        if( (rsp_dm_ptr->disable_op.id.stream_id = req_dm_ptr->disable_op.id.stream_id) == 0 )
        {
          // Disable all streams
          (void)log_config_mask(-1, FALSE, DIAG_STREAM_ALL, diag_cur_preset_id);
          log_config_mstr_mask(-1, FALSE, DIAG_STREAM_ALL, diag_cur_preset_id);
        }
        else
        {
          stream_id = req_dm_ptr->disable_op.id.stream_id;
          // Disable a specific stream given by stream_id
          (void)log_config_mask(-1, FALSE, stream_id, diag_cur_preset_id);
          log_config_mstr_mask(-1, FALSE, stream_id, diag_cur_preset_id);
        }
        break;

      case LOG_CONFIG_RETRIEVE_ID_RANGES_OP:
        rsp_dm_ptr->id_ranges.status = status;
        rsp_dm_ptr->id_ranges.rsvd = 0;
        for( i = 0; i <= LOG_EQUIP_ID_MAX; i++ )
        {
          rsp_dm_ptr->id_ranges.last_item[i] = log_mstr_last_item_tbl[i];
        }
        break;

      case LOG_CONFIG_SET_MASK_OP:
        rsp_dm_ptr->set_mask.id.stream_id = stream_id = req_dm_ptr->set_mask.id.stream_id;
        rsp_dm_ptr->set_mask.status = status;
        rsp_dm_ptr->set_mask.code_range.equip_id = equip_id;
        rsp_dm_ptr->set_mask.code_range.last_item = log_mstr_last_item_tbl[equip_id];

#if defined(DIAG_MP_MASTER)
        /* Save master copy of log mask */
        if(req_dm_ptr->set_mask.id.stream_id == 0 )
        {
          for(i = 0; i<= DIAG_MAX_STREAM_ID; i++)
          {
            (void)log_config_mask(-1, FALSE, stream_id, diag_cur_preset_id);
            log_config_mstr_mask(-1, FALSE, stream_id, diag_cur_preset_id);
          }
        }
        else
        {
          mask_rd_ptr = (byte *)req_dm_ptr->set_mask.mask;
          mask_wr_ptr = diag_log_mask_mstr[stream_id - 1]->mask[equip_id]; //&log_mask_mstr[DIAG_STREAM_1-1][equip_id][0];
          last_item = MIN(MSTR_LOG_MAX_LAST_ITEM, req_dm_ptr->set_mask.code_range.last_item);
          diag_log_last_item_mstr_tbl[equip_id] = last_item;
          diag_log_mask_mstr[stream_id - 1]->status = DIAG_CTRL_MASK_VALID;

          for( i = 0; i <= LOG_MASK_ARRAY_INDEX(last_item); i++ )
          {
            *mask_wr_ptr = *mask_rd_ptr;
            rsp_dm_ptr->set_mask.mask[i] = *mask_rd_ptr;
            mask_wr_ptr++;
            mask_rd_ptr++;
          }
          /* Zero all bits that are after the last item in the set mask */
          mask_wr_ptr--;
          i--;

          /* (The next boundary of 8) - 1 - (last_item) gives the number of bits
           * at the end of the last byte that are not specified.
           * This line 0's out the bits that remain in the mask that are not
           * included in last_item. */
  /*lint -save -e504 Unusual shift operation (unusually formed right argument)*/
  /*lint -save -e817 */
          *mask_wr_ptr &= 0xFF >> (((last_item + 8) & ~7) - 1 - last_item);
          rsp_dm_ptr->set_mask.mask[i] = *mask_wr_ptr;
          for( i = LOG_MASK_ARRAY_INDEX(last_item) + 1; i <=
               LOG_MASK_ARRAY_INDEX(log_mstr_last_item_tbl[equip_id]); i++ )
          {
            rsp_dm_ptr->set_mask.mask[i] = 0;
          }
          /* Master needs to keep track to send log masks to Slaves */
        }

        for( i = 0; i < NUM_SMD_PORTS; i++ )
        {
          diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][i].log_mask_update[0][equip_id] = TRUE;
        }
#endif
        /* Update the local mask */
        if( equip_id > LOG_EQUIP_ID_LAST )
        {
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
        }

        mask_rd_ptr = (byte *)req_dm_ptr->set_mask.mask;
        mask_wr_ptr = &log_mask[req_dm_ptr->set_mask.id.stream_id - 1][log_mask_offset_tbl[equip_id]];
        last_item = MIN(log_last_item_tbl[equip_id], req_dm_ptr->set_mask.code_range.last_item);

        for( i = 0; i <= LOG_MASK_ARRAY_INDEX(last_item); i++ )
        {
          *mask_wr_ptr = *mask_rd_ptr;
          mask_wr_ptr++;
          mask_rd_ptr++;
        }
        /* Zero all bits that are after the last item in the set mask */
        mask_wr_ptr--;
        i--;

        /* (The next boundary of 8) - 1 - (last_item) gives the number of bits
         * at the end of the last byte that are not specified.
         * This line 0's out the bits that remain in the mask that are not
         * included in last_item. */
        /*lint -save -e504 Unusual shift operation (unusually formed right argument)*/
        /*lint -save -e817 */
        *mask_wr_ptr &= 0xFF >> (((last_item + 8) & ~7) - 1 - last_item);
#if !defined (DIAG_MP)
        rsp_dm_ptr->set_mask.mask[i] = *mask_wr_ptr;
#endif
        /* Zero out all items that were not specified in the request. */
        for( i = last_item + 1; i <= log_last_item_tbl[equip_id]; i++ )
        {
          (void)log_config_mask(log_set_equip_id(i, equip_id), FALSE, stream_id, diag_cur_preset_id);
        }
#if !defined (DIAG_MP)
        for( i = LOG_MASK_ARRAY_INDEX(last_item) + 1; i <=
               LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]); i++ )
        {
          rsp_dm_ptr->set_mask.mask[i] = 0;
        }
#endif

        /* Update composite mask */
        log_update_composite_mask(log_mask_offset_tbl[equip_id],
                                  log_mask_offset_tbl[equip_id] + LOG_MASK_ARRAY_INDEX(last_item));
        break;

      case LOG_CONFIG_GET_LOGMASK_OP:
        rsp_dm_ptr->get_mask.id.stream_id = req_dm_ptr->get_mask.id.stream_id;
        rsp_dm_ptr->get_mask.status = status;
        rsp_dm_ptr->get_mask.code_range.equip_id = equip_id;
        rsp_dm_ptr->get_mask.code_range.last_item = log_mstr_last_item_tbl[equip_id];


        if( equip_id > LOG_EQUIP_ID_LAST )
        {
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
        }

        mask_wr_ptr = (byte *)rsp_dm_ptr->get_mask.mask;

        if (req_dm_ptr->get_mask.id.stream_id >= DIAG_MIN_STREAM_ID && req_dm_ptr->get_mask.id.stream_id <= DIAG_MAX_STREAM_ID )
        {
          mask_rd_ptr = (byte *)&((diag_log_mask_mstr[req_dm_ptr->get_mask.id.stream_id-1])->mask[equip_id]);
        }
        else
        {
            return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
        }

        for( i = 0; i <= (unsigned int)
             LOG_MASK_ARRAY_INDEX(log_mstr_last_item_tbl[equip_id]); i++ )
        {

          *mask_wr_ptr = *mask_rd_ptr;
          mask_wr_ptr++;
          mask_rd_ptr++;

        }
        break;
      default:
        break;

    } /* end switch */

    if( (LOG_CONFIG_SUCCESS_S == status) &&
        (LOG_CONFIG_SET_MASK_OP == operation) )
    {
      (void)diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);
    }

    /* Config may have changed.  Run sleep vote routine. */
    (void)log_sleep_vote();

    /* if master processor on multi processor build, don't send response */
#if !defined(DIAG_RSP_SEND)
    if( !diagcomm_apps_only_config )
      send_rsp = FALSE;
#endif

#if defined(DIAG_MP_MASTER)
    if( (operation == LOG_CONFIG_SET_MASK_OP)
        || (operation == LOG_CONFIG_DISABLE_OP) ||
        (operation == LOG_CONFIG_RETRIEVE_ID_RANGES_OP) ||
        (operation == LOG_CONFIG_RETRIEVE_VALID_MASK_OP) ||
        (operation == LOG_CONFIG_GET_LOGMASK_OP) )
    {
      /* MP Master now responds to "set mask" requests */
      /* If we're in buffering mode, Master needs to send out the responses.
       If Modem is not up, Master needs to send out responses as well. */
      send_rsp = TRUE;

    }
#endif

    /* if diag is not connected, don't send response */
    if( !diagcomm_status() )
    {
      send_rsp = FALSE;
    }

    /* return response */
    if( !send_rsp )
    {
      diagpkt_free(rsp_dm_ptr);
      return NULL;
    }
    else
    {
      return ((void *)rsp_dm_ptr);
    }
  }
} /* log_process_config_pkt */

/*===========================================================================

FUNCTION LOG PKT GET LOCAL EQUIPID RANGE

DESCRIPTION
  This function will return the number of log codes in each equipment id.
  This function queries the local Diag log mask table, as opposed to log_process_config_pkt(),
  which queries the target master log mask table

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACK(void *) log_pkt_get_local_equipid_range (
  PACK(void *) req_pkt_ptr, /* pointer to request packet */
  uint16 pkt_len            /* length of request packet  */
)
{
   log_get_local_equipid_range_req_type *req_ptr = (log_get_local_equipid_range_req_type *) req_pkt_ptr;
   log_get_local_equipid_range_rsp_type *rsp = NULL;
   int i = 0;
   diagpkt_subsys_cmd_code_type cmd_code;

   if(req_ptr == NULL)
   {
     MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"log_pkt_get_local_equipid_range(): Request packet pointer is NULL \n",1);
     return NULL;
   }

   cmd_code = diagpkt_subsys_get_cmd_code (req_ptr);
   if ((pkt_len != sizeof(log_get_local_equipid_range_req_type)) ||
       ((cmd_code != DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_MODEM) && (cmd_code != DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_APP)
         && (cmd_code != DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_Q6) && (cmd_code != DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_RIVA)))
    {
     MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Value of cmd code: %d\n",cmd_code);
     return ((void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len));
    }

   if(diagcomm_status())
   {
      rsp = (log_get_local_equipid_range_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
                                              cmd_code, sizeof(log_get_local_equipid_range_rsp_type));

      if (rsp)
     {
       rsp->status=SUCCESSFUL;

       for (i = 0; i <= LOG_EQUIP_ID_MAX; i++)
       {
           rsp->equipid_ranges[i] = (uint32) log_last_item_tbl[i];
       }
     }

   }

   return ((void *) rsp);

}

/*===========================================================================

FUNCTION LOG_PROCESS_EXT_LOGMASK

DESCRIPTION
  This function sets the logmask from the extended logmask bit field specified
  in the diag_ext_logmask_req_type packet.

  This command effects what type of entries are logged.
===========================================================================*/
void log_process_ext_logmask (
  PACK(uint8 *) logmask_ptr,
  unsigned int  num_bits
)
{
  unsigned int i;
  log_code_type code;
  boolean code_enabled; /* Used to determine if a code is enabled. */

  /*----------------------------------------------------------------------
    For each bit allowed in the logging mask, configure that log condition
    active if the bit is 1 and configure that log condition inactive if
    the bit is 0.
  ----------------------------------------------------------------------*/
  /* Tell lint it is OK to have a constant used as a boolean here */
  /*lint -e506 */
  for(i = 0; i < MIN(((uint32)log_last_item_tbl[1]) + 1, num_bits); i++) {
  /*lint +e506 */
    code_enabled = (logmask_ptr[i >> 3] & (0x80 >> (i & 7)))
                   ? TRUE : FALSE;

    code = log_set_equip_id(i, 1);

    (void) log_config_mask(code, code_enabled, DIAG_STREAM_1, diag_cur_preset_id);
  }

  /*--------------------------------------------------------------------------
    If this DMSS supports more bits than are given, we should set the rest
    to 0 to make sure they don't get unsuspected logs.
  --------------------------------------------------------------------------*/
  for (; i < (uint32) (log_last_item_tbl[1] + 1); i++) {
    code = log_set_equip_id(i, 1);

    (void) log_config_mask(code, FALSE, DIAG_STREAM_1, diag_cur_preset_id);
  }

#if defined(DIAG_MP_MASTER)
  /* Save to master log mask */
  for( i = 0; i < MIN(MSTR_LOG_MAX_LAST_ITEM, num_bits); i++ )
  {
    code_enabled = (logmask_ptr[i >> 3] & (0x80 >> (i & 7))) ? TRUE : FALSE;
    code = log_set_equip_id(i, 1);
    log_config_mstr_mask(code, code_enabled, DIAG_STREAM_1, diag_cur_preset_id);
  }
#endif

  /* Vote on sleep for the logging service. */
  (void) log_sleep_vote ();

} /* log_process_ext_logmask */



/*===========================================================================

FUNCTION LOG_PROCESS_EXT_LOGMASK_PKT

DESCRIPTION
  This procedure processes a request to change the logmask.

RETURN VALUE
  Pointer to response packet.

WARNING
  This function support Obsolete cmd 93, which is replaced by cmd 115.
  However, support for 93 is left in place to support Legacy tools

  When num_bits == 0 in the request, the command queries the log valid
  mask. However, looking at the code below, log_valid_mask[] is never set.
  Therefore the "get log mask" request for cmd 93 is likely broken.

===========================================================================*/
PACK(void *) log_process_ext_logmask_pkt (
  PACK(void *) req_pkt_ptr, /* pointer to request packet */
  uint16 pkt_len            /* length of request packet  */
)
{
  DIAG_EXT_LOGMASK_F_req_type *req_ptr = (DIAG_EXT_LOGMASK_F_req_type *) req_pkt_ptr;
  DIAG_EXT_LOGMASK_F_rsp_type *rsp_ptr = NULL;
  const unsigned int rsp_len = sizeof(DIAG_EXT_LOGMASK_F_rsp_type);

  uint32 i;
  const byte *valid_mask_ptr = NULL;
  boolean enabled;
  boolean send_rsp = TRUE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Quick sanity check */
  if (pkt_len != FPOS(DIAG_EXT_LOGMASK_F_req_type, mask) +
                 (req_ptr->num_bits + 7) / 8) {
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  if (req_ptr->num_bits == 0) {
    /*-------------------------------------------------------
      Report the number of valid bits as well as the mask of
      valid bits for this build
    -------------------------------------------------------*/
    rsp_ptr = (DIAG_EXT_LOGMASK_F_rsp_type *) diagpkt_alloc(DIAG_EXT_LOGMASK_F, rsp_len);
    if(rsp_ptr){
    rsp_ptr->num_valid_bits = log_last_item_tbl[1] + 1;

    valid_mask_ptr = &log_valid_mask[log_mask_offset_tbl[1]];

    for (i = 0; i < DIAG_EXT_LOGMASK_NUM_BYTES; i++) {
      rsp_ptr->valid_mask[i] = 0;
    }

    for (i = 0; i < DIAG_EXT_LOGMASK_NUM_BITS; i++) {
      enabled = (log_mask_enabled(log_valid_mask, 1, i)) ? TRUE : FALSE;

      if (enabled) {
        rsp_ptr->valid_mask[i >> 3] |= 0x80 >> (i & 7);
      }
    }
   }
  } else {

   (void) valid_mask_ptr; /* to supress lint (Warning) 550 */

    /* Process the logmask */
    log_process_ext_logmask(&req_ptr->mask[0], req_ptr->num_bits);

    /* Only return a command code */
    rsp_ptr = (DIAG_EXT_LOGMASK_F_rsp_type *) diagpkt_alloc(DIAG_EXT_LOGMASK_F,
      FPOS(DIAG_EXT_LOGMASK_F_rsp_type, num_valid_bits));
  }


  /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
  if(!diagcomm_apps_only_config)
    send_rsp = FALSE;
  #endif

  #if defined(DIAG_CENTRAL_ROUTING) && defined(DIAG_MP_MASTER)
    /* MP Master now responds to "set mask" requests.
       If we're in buffering mode, Master needs to send out the "get mask" responses.
       If Modem is not up, Master needs to send out responses as well.
    */
    send_rsp = TRUE;
  #endif

/* if diag is not connected, don't send response */
  if (!diagcomm_status())
  {
    send_rsp = FALSE;
  }

/* return response */
  if (!send_rsp)
  {
    diagpkt_free(rsp_ptr);
    return NULL;
  }
  else
  {
    return ((void *) rsp_ptr);
  }
} /* log_process_ext_logmask_pkt */


/*===========================================================================

FUNCTION LOG PKT GET LOCAL MASKS

DESCRIPTION
  This function will return the number of log mask values
  for a particular equipment id specified in the request packet.
  This function queries the local Diag log mask table, as opposed to log_process_config_pkt(),
  which queries the target master log mask table

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACK(void *) log_pkt_get_local_masks (
  PACK(void *) req_pkt_ptr, /* pointer to request packet */
  uint16 pkt_len            /* length of request packet  */
)
{
   log_get_local_mask_req_type *req_ptr = (log_get_local_mask_req_type *) req_pkt_ptr;
   log_get_local_mask_rsp_type *rsp = NULL;
   unsigned int rsp_len = 0;
   int i = 0;
   byte *mask_rd_ptr = NULL;
   byte *mask_wr_ptr = NULL;
   diagpkt_subsys_cmd_code_type cmd_code;

   if(req_ptr == NULL)
  {
   MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "log_pkt_get_local_masks(): Request pointer is NULL \n",1);
   return NULL;
  }

   cmd_code = diagpkt_subsys_get_cmd_code (req_ptr);

   if ((pkt_len != sizeof(log_get_local_mask_req_type)) ||
       ((cmd_code != DIAGDIAG_LOG_MASK_RETRIEVAL_MODEM) && (cmd_code != DIAGDIAG_LOG_MASK_RETRIEVAL_APP)
         && (cmd_code != DIAGDIAG_LOG_MASK_RETRIEVAL_Q6) && (cmd_code != DIAGDIAG_LOG_MASK_RETRIEVAL_RIVA)) ||
       (req_ptr->stream_id > DIAG_MAX_STREAM_ID) || (req_ptr->stream_id < DIAG_MIN_STREAM_ID) ||
       (req_ptr->equip_id > LOG_EQUIP_ID_LAST) || (req_ptr->equip_id > log_last_item_tbl[req_ptr->equip_id]))
   {
     MSG_3(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "MSG_3 cmd_code %d stream id %d equipment id %d \n",cmd_code,req_ptr->stream_id,
           req_ptr->equip_id );
     return ((void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len));
   }

   if(diagcomm_status())
   {
       rsp_len = FPOS (log_get_local_mask_rsp_type, log_mask) +
                                ((LOG_MASK_ARRAY_INDEX(log_last_item_tbl[req_ptr->equip_id]) + 1) * sizeof(byte));
       rsp = (log_get_local_mask_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
                                              cmd_code, rsp_len);

   if (rsp)
   {
      rsp->equip_id = req_ptr->equip_id;
      rsp->status = SUCCESSFUL;
      rsp->stream_id = req_ptr->stream_id;
      mask_rd_ptr = (byte *) &log_mask[(req_ptr->stream_id) - 1][log_mask_offset_tbl[req_ptr->equip_id]];
      mask_wr_ptr = (byte *) rsp->log_mask;

      for (i = 0; i <= (unsigned int)
              LOG_MASK_ARRAY_INDEX(log_last_item_tbl[req_ptr->equip_id]); i++)
      {
         *mask_wr_ptr = *mask_rd_ptr;
          mask_wr_ptr++;
          mask_rd_ptr++;
      }
    }
  }
   return ((void *) rsp);

} /* log_pkt_get_local_masks */


/*===========================================================================

FUNCTION DIAGLOG_PROCESS_EXT_CONFIG_REQ

DESCRIPTION
  This procedure processes a request to set the log mask for specific
  preset id masks for Stream 1.

PARAMETERS
  req_pkt_ptr - Pointer to request packet
  pkt_len - Length of request packet

RETURN VALUE
  Pointer to response packet.

  This command effects what type of entries are logged.
===========================================================================*/
PACK(void *) diaglog_process_ext_config_req (
  PACK(void *) req_pkt_ptr,
  uint16 pkt_len
)
{
  /* For Diag subsys cmd DIAG_EXT_LOG_CONFIG 75 18 98 - Supports all streams */
  log_ext_config_req_type *req_dm_ptr = NULL;
  log_ext_config_rsp_type *rsp_dm_ptr = NULL;

  /* Local working variables */
  unsigned int rsp_len, operation, last_item, equip_id = 0;
  uint16 i = 0;
  uint8 preset_id;
  byte *mask_rd_ptr = NULL;
  byte *mask_wr_ptr = NULL;
  log_config_status_enum_type status = LOG_CONFIG_SUCCESS_S;
  boolean send_rsp = TRUE;

  /* Sanity checks */
  if( (req_pkt_ptr == NULL) || (pkt_len == 0) )
  {
    return NULL;
  }

  req_dm_ptr = (log_ext_config_req_type *)req_pkt_ptr;
  operation = req_dm_ptr->header.opcode;

  /* if master processor on multi processor build, don't send response */
#if !defined(DIAG_RSP_SEND)
  if( !diagcomm_apps_only_config )
    send_rsp = FALSE;
#endif

#if defined(DIAG_MP_MASTER)
  if( (operation == LOG_CONFIG_SET_MASK_OP) ||
      (operation == LOG_CONFIG_DISABLE_OP) ||
      (operation == LOG_CONFIG_RETRIEVE_ID_RANGES_OP) ||
      (operation == LOG_CONFIG_RETRIEVE_VALID_MASK_OP) ||
      (operation == LOG_CONFIG_GET_LOGMASK_OP) )
  {
    /* MP Master now responds to "set mask" requests.
     If we're in buffering mode, Master needs to send out the responses
     for all common requests.
     If Modem is not up, Master needs to send out responses as well. */
    send_rsp = TRUE;
  }
#endif


  /* if diag is not connected, don't send response */
  if( !diagcomm_status() )
  {
    send_rsp = FALSE;
  }



  if( send_rsp == TRUE )
  {
    switch( operation )
    {
      case LOG_CONFIG_DISABLE_OP:
      {
        preset_id = req_dm_ptr->disable_op.id.preset_id;
        rsp_len = sizeof(log_ext_config_disable_op_rsp);

        // Preset ID 0 is acceptable in this case (to disable all the preset masks).
        if( preset_id > DIAG_MAX_PRESET_ID )
        {
          status = LOG_CONFIG_INVALID_PRESET_ID;
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
        }
        break;
      }

      case LOG_CONFIG_RETRIEVE_ID_RANGES_OP:
      {
        /* To satisfy compiler warning - Error: C3017W: preset_id may be used before being set */
        preset_id = diag_cur_preset_id;
        rsp_len = sizeof(log_ext_config_id_ranges_rsp);
        break;
      }

      case LOG_CONFIG_SET_MASK_OP:
      {
        equip_id = req_dm_ptr->set_mask.code_range.equip_id;
        preset_id = req_dm_ptr->set_mask.id.preset_id;

      if( equip_id > LOG_EQUIP_ID_MAX )
        {
          status = LOG_CONFIG_INVALID_EQUIP_ID_S;
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
        }

        if( (preset_id == 0) || (preset_id > DIAG_MAX_PRESET_ID) )
        {
          status = LOG_CONFIG_INVALID_PRESET_ID;
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
        }
        rsp_len = FPOS(log_ext_config_set_mask_rsp, mask) + (LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]) + 1) * sizeof(byte);
        break;
      }

      case LOG_CONFIG_GET_LOGMASK_OP:
      {
        equip_id = req_dm_ptr->get_mask.equip_id;
        preset_id = req_dm_ptr->set_mask.id.preset_id;

      if( equip_id > LOG_EQUIP_ID_MAX )
        {
          status = LOG_CONFIG_INVALID_EQUIP_ID_S;
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
        }

        if( req_dm_ptr->set_mask.id.preset_id == 0 || preset_id > DIAG_MAX_PRESET_ID )
        {
          status = LOG_CONFIG_INVALID_PRESET_ID;
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
        }
        rsp_len = FPOS(log_ext_config_get_mask_rsp, mask) + (LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]) + 1) * sizeof(byte);
        break;
      }

      default:
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);

    } /* end switch for rps_len computation */

    rsp_dm_ptr = (log_ext_config_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, DIAG_EXT_LOG_CONFIG, rsp_len);
    if( rsp_dm_ptr == NULL )
      return NULL;

    rsp_dm_ptr->header = req_dm_ptr->header;
    switch( operation )
    {
      case LOG_CONFIG_DISABLE_OP:
      {
        rsp_dm_ptr->disable_op.status = status;
        rsp_dm_ptr->disable_op.id.preset_id = preset_id;
        if( preset_id == 0 )
        {
          (void)log_config_mask(-1, FALSE, DIAG_STREAM_1, DIAG_PRESET_MASK_ALL);

#if defined(DIAG_MP_MASTER)
          log_config_mstr_mask(-1, FALSE, DIAG_STREAM_1, DIAG_PRESET_MASK_ALL);
          /* Propagate masks to Slaves. */
          diagcomm_mask_update(DIAG_UPDATE_MASK_LOGS, DIAG_STREAM_1, DIAG_PRESET_MASK_ALL, TRUE);
#endif
        }
        else
        {
          (void)log_config_mask(-1, FALSE, DIAG_STREAM_1, preset_id);

#if defined(DIAG_MP_MASTER)
          log_config_mstr_mask(-1, FALSE, DIAG_STREAM_1, preset_id);
          /* Propagate masks to Slaves. */
          diagcomm_mask_update(DIAG_UPDATE_MASK_LOGS, DIAG_STREAM_1, preset_id, TRUE);
#endif
        }
        break;
      }

      case LOG_CONFIG_RETRIEVE_ID_RANGES_OP:
      {
        rsp_dm_ptr->id_ranges.status = status;
        rsp_dm_ptr->id_ranges.rsvd = 0;
        for( i = 0; i <= LOG_EQUIP_ID_MAX; i++ )
        {
          rsp_dm_ptr->id_ranges.last_item[i] = log_mstr_last_item_tbl[i];
        }
        break;
      }

      case LOG_CONFIG_SET_MASK_OP:
      {
        /* XXX This is affecting the local mask, not the master one. */
        rsp_dm_ptr->set_mask.id.preset_id = preset_id;
        rsp_dm_ptr->set_mask.status = status;
        rsp_dm_ptr->set_mask.code_range.equip_id = equip_id;
        rsp_dm_ptr->set_mask.code_range.last_item = log_last_item_tbl[equip_id];

        if( equip_id > LOG_EQUIP_ID_LAST )
        {
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
        }

        mask_rd_ptr = (byte *)req_dm_ptr->set_mask.mask;
        mask_wr_ptr = &diag_log_preset_mask[preset_id - 1][log_mask_offset_tbl[equip_id]];
        last_item = MIN(log_last_item_tbl[equip_id], req_dm_ptr->set_mask.code_range.last_item);

        for( i = 0; i <= LOG_MASK_ARRAY_INDEX(last_item); i++ )
        {
          *mask_wr_ptr = *mask_rd_ptr;
          rsp_dm_ptr->set_mask.mask[i] = *mask_rd_ptr;

          mask_wr_ptr++;
          mask_rd_ptr++;
        }
        /* Zero all bits that are after the last item in the set mask */
        mask_wr_ptr--;
        i--;

        /* (The next boundary of 8) - 1 - (last_item) gives the number of bits
         * at the end of the last byte that are not specified.
         * This line 0's out the bits that remain in the mask that are not
         * included in last_item. */
        /*lint -save -e504 Unusual shift operation (unusually formed right argument)*/
        /*lint -save -e817 */
        *mask_wr_ptr &= 0xFF >> (((last_item + 8) & ~7) - 1 - last_item);
        rsp_dm_ptr->set_mask.mask[i] = *mask_wr_ptr;

        /* Zero out all items that were not spefcified in the request. */
        for( i = last_item + 1; i <= log_last_item_tbl[equip_id]; i++ )
        {
          (void)log_config_mask(log_set_equip_id(i, equip_id), FALSE, DIAG_STREAM_1, preset_id);
        }

        for( i = LOG_MASK_ARRAY_INDEX(last_item) + 1; i <=
               LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]); i++ )
        {
          rsp_dm_ptr->set_mask.mask[i] = 0;
        }

#if defined(DIAG_MP_MASTER)
        /* Save master copy of log mask */
        mask_rd_ptr = (byte *)req_dm_ptr->set_mask.mask;
        mask_wr_ptr = (byte *)diag_log_mask_mstr_preset[preset_id - 1].mask[equip_id];
        last_item = MIN(MSTR_LOG_MAX_LAST_ITEM, req_dm_ptr->set_mask.code_range.last_item);
        diag_log_last_item_mstr_tbl[equip_id] = last_item;
        diag_log_mask_mstr_preset[preset_id - 1].status = DIAG_CTRL_MASK_VALID;

        for( i = 0; i <= LOG_MASK_ARRAY_INDEX(last_item); i++ )
        {
          *mask_wr_ptr = *mask_rd_ptr;
          mask_wr_ptr++;
          mask_rd_ptr++;
        }
        /* Zero all bits that are after the last item in the set mask */
        mask_wr_ptr--;
        i--;

        /* (The next boundary of 8) - 1 - (last_item) gives the number of bits
         * at the end of the last byte that are not specified.
         * This line 0's out the bits that remain in the mask that are not
         * included in last_item. */
        /*lint -save -e504 Unusual shift operation (unusually formed right argument)*/
        /*lint -save -e817 */
        *mask_wr_ptr &= 0xFF >> (((last_item + 8) & ~7) - 1 - last_item);

        /* Master needs to keep track to send log masks to Slaves */
        for( i = 0; i < NUM_SMD_PORTS; i++ )
        {
          diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][i].log_mask_preset_update[preset_id - 1][equip_id] = TRUE;
        }

        /* Propagate masks to Slaves. */
        diagcomm_mask_update(DIAG_UPDATE_MASK_PARTIAL_LOGS, DIAG_STREAM_1, preset_id, TRUE);
#endif
        break;
      }

      case LOG_CONFIG_GET_LOGMASK_OP:
      {
        rsp_dm_ptr->get_mask.id.preset_id = preset_id;
        rsp_dm_ptr->get_mask.status = status;
        rsp_dm_ptr->get_mask.code_range.equip_id = equip_id;
        rsp_dm_ptr->get_mask.code_range.last_item = log_last_item_tbl[equip_id];

        if( equip_id > LOG_EQUIP_ID_LAST )
        {
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
        }
        mask_rd_ptr = (byte *)&diag_log_preset_mask[preset_id - 1][log_mask_offset_tbl[equip_id]];
        mask_wr_ptr = (byte *)rsp_dm_ptr->get_mask.mask;

        for( i = 0; i <= (unsigned int)LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]); i++ )
        {
          *mask_wr_ptr = *mask_rd_ptr;
          mask_wr_ptr++;
          mask_rd_ptr++;
        }
        break;
      }

    }  /* end switch */
  }

  return rsp_dm_ptr;

} /* diaglog_process_ext_config_req */


/*===========================================================================

FUNCTION LOG_PROCESS_LEGACY_LOGMASK

DESCRIPTION
  This procedure processes a request to change the logmask.
  Note: This is for the orignal 32-bit logmask that the DM used.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACK(void *) log_process_legacy_logmask (
  PACK(void *) req_pkt_ptr,
  uint16 pkt_len
)
{
  uint32 i;                /* index to loop through logmask */
  boolean code_enabled; /* Used to determine if a code is enabled. */

  DIAG_LOGMASK_F_req_type *req_ptr = (DIAG_LOGMASK_F_req_type *) req_pkt_ptr;
  DIAG_LOGMASK_F_rsp_type *rsp_ptr = NULL;
  boolean send_rsp = TRUE;
  const unsigned int rsp_len = sizeof(DIAG_LOGMASK_F_rsp_type);

  unsigned int mask = req_ptr->mask;
  const uint32 numbits = log_last_item_tbl[1] + 1;

  (void) pkt_len; /* suppress compiler warning */
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For each bit allowed in the logging mask, configure that log condition
   * active if the bit is 1 and configure that log condition inactive if
   * the bit is 0. */

  /* Tell lint it is OK to have a constant used as a boolean here */
  /*lint -e506 */
  for( i = 0; i < MIN(numbits, 32); i++ )
  {
  /*lint +e506 */

    code_enabled = (boolean) (mask & 0x01);

    /* Configure logmask */
    (void) log_config_mask(log_set_equip_id(i, 1), code_enabled, DIAG_STREAM_1, diag_cur_preset_id); //ToDo: save to master?

    mask >>= 1;
  }

  /* Set any codes above 32 to zero. */
  for(; i < numbits; i++)
  {
    (void) log_config_mask (log_set_equip_id(i, 1), FALSE, DIAG_STREAM_1, diag_cur_preset_id); //ToDo: save to master?
  }

  /* Vote on sleep for the logging service. */
  (void) log_sleep_vote ();

  /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
  if (!diagcomm_apps_only_config)
    send_rsp = FALSE;
  #endif

/* if diag is not connected, don't send response */
  if (!diagcomm_status())
  {
    send_rsp = FALSE;
  }
  if (send_rsp)
    rsp_ptr = (DIAG_LOGMASK_F_rsp_type *) diagpkt_alloc (DIAG_LOGMASK_F, rsp_len);

  (void)diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);
  return (rsp_ptr);

} /* log_process_legacy_logmask */

/* -------------------------------------------------------------------------
 * Internal (aka private) function prototypes
 * ------------------------------------------------------------------------- */
/*===========================================================================

FUNCTION LOG_SEARCH_MASK

DESCRIPTION
  This function returns a boolean indicating TRUE if the given ID and 'item'
  denotes a valid and enabled log code for the given stream.

===========================================================================*/
static boolean log_search_mask (
  unsigned int id,
  unsigned int item,
  byte stream_id
)
{
  boolean return_val = FALSE;

  if(stream_id <= 0 || stream_id > DIAG_MAX_STREAM_ID)
    return return_val;

  /* if valid code val */
  if (log_mask_enabled(log_mask[stream_id - 1], id, item)) {
    return_val = TRUE;
  }
  return return_val;

} /* log_search_mask */



/*===========================================================================

FUNCTION LOG_MASK_ENABLED

DESCRIPTION
  This function returns a boolean indicating if the specified code is enabled.

  The equipment ID and item code are passed in to avoid duplicating the
  calculation within this call.  It has to be done for most routines that call
  this anyways.

===========================================================================*/
boolean log_mask_enabled (
  const byte *mask_ptr,
  unsigned int id,
  unsigned int item
)
{
  unsigned int offset, index, mask;
  boolean enabled = FALSE;

  if (id <= LOG_EQUIP_ID_LAST && item <= log_last_item_tbl[id]) {

    offset = log_mask_offset_tbl[id];

    index = LOG_MASK_ARRAY_INDEX(item);

    mask = LOG_MASK_BIT_MASK(item);

    if (mask_ptr[offset + index] & mask) {
      enabled = TRUE;
    }
  } else {
    /* It is expected that this will be compiled out in a target build */
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Invalid code given 0x%x", (id << 12) | item);
  }
  return enabled;
} /* log_mask_enabled */



/*===========================================================================

FUNCTION LOG_SET_EQUIP_ID

DESCRIPTION
  This function returns the equipment ID of the specified 'code'

===========================================================================*/
static log_code_type log_set_equip_id (
  unsigned int code,
  unsigned int equip_id
)
{
  ASSERT(equip_id <= LOG_EQUIP_ID_MAX);

  code &= 0x00000FFF;

  equip_id &= 0x0000000F;

  equip_id <<= 12;

  return (log_code_type) code | (log_code_type)equip_id;

} /* log_set_equip_id */



/*===========================================================================

FUNCTION LOG_SLEEP_VOTE

DESCRIPTION
  This function checks the configuration of the logging service to determine
  if the system is allowed to sleep.  All log items must be disabled to
  allow sleep.

===========================================================================*/
static boolean log_sleep_vote (void)
{
  unsigned int id = 0;
  uint16 item = 0;
  boolean ok2sleep = TRUE;

  /* This is not very efficient, but it works.  It only gets called
     when a config packet is sent, not from any clients.  Therefore,
     runtime efficiency is not a huge issue. */

  for (id = 0; ok2sleep && id <= LOG_EQUIP_ID_MAX; id++)
  {
    for (item = 0; ok2sleep && item <= log_last_item_tbl[id]; item++)
    {
      if (log_mask_enabled (log_mask[DIAG_STREAM_1-1], id, item))
      {
        ok2sleep = FALSE;
      }
    }
  }

  diag_sleep_vote (DIAG_SLEEP_LOG, ok2sleep);

  return ok2sleep;

}


/*===========================================================================

FUNCTION LOG_ON_DEMAND_PKT

DESCRIPTION
  This procedure processes a log on demand request. It scans in the
  log_on_demand_list to match the log_code. If it find the log_code it
  processes the function pointer associated with it.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/

PACK(void *) log_on_demand_pkt (
  PACK(void *) req_pkt_ptr,
  uint16 pkt_len
)
{
  DIAG_LOG_ON_DEMAND_F_req_type *req_ptr =
      (DIAG_LOG_ON_DEMAND_F_req_type *) req_pkt_ptr;
  DIAG_LOG_ON_DEMAND_F_rsp_type *rsp_ptr = NULL;
  boolean send_rsp = TRUE;
  int i;

  const unsigned int rsp_len = sizeof( DIAG_LOG_ON_DEMAND_F_rsp_type );

  (void) pkt_len; /* suppress compiler warning */

  rsp_ptr = (DIAG_LOG_ON_DEMAND_F_rsp_type *) diagpkt_alloc( DIAG_LOG_ON_DEMAND_F, rsp_len );

  if( rsp_ptr == NULL ) {
    /* If we can't allocate, diagpkt_err_rsp() can't either. */
    return NULL;
  }

  rsp_ptr->log_code = req_ptr->log_code;

  rsp_ptr->status = LOG_ON_DEMAND_ACKNOWLEDGE_S;

  if (log_on_demand_reg_count != 0)
  {
    for( i=0; i<LOG_ON_DEMAND_SIZE; i++ )
    {
      if ( ( log_on_demand_list[i].log_on_demand_ptr != NULL ) && (log_on_demand_list[i].log_code == req_ptr->log_code) )
        {
            rsp_ptr->status=(*log_on_demand_list[i].log_on_demand_ptr)
              (log_on_demand_list[i].log_code);
            break;
      }
    }
  }


#if defined(DIAG_MP_MASTER)
  /* Log on demand should be responded by Modem if feature mask bit (2) i.e. log on demand bit is not set by modem.
     Since send_rsp is init to TRUE above, following condition says that since the bit is set on modem APPS should respond
  */
  if ( !DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].feature_mask,F_DIAG_LOG_ON_DEMAND_RSP_ON_MASTER))
#else
  /* This will execute on modem, or other peripherals. It will work for 'modem only' configuration as feature mask will not be set */
  if ( DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].feature_mask,F_DIAG_LOG_ON_DEMAND_RSP_ON_MASTER))
#endif
  {
      send_rsp = FALSE; // if this execute it says the bit is not set on modem, modem should respond, APPS should not respond.
  }

  /* if diag is not connected, don't send response */
  if (!diagcomm_status())
  {
    if(!diagcomm_apps_only_config)
    send_rsp = FALSE;
  }

  /* return response */
  if (!send_rsp)
  {
    diagpkt_free(rsp_ptr);
    return NULL;
  }
  else
  {
    return ((void *) rsp_ptr);
  }

} /* log_on_demand_pkt */

/*===========================================================================

FUNCTION LOG_UPDATE_COMPOSITE_MASK

DESCRIPTION
  (re)compiles the composite log mask within the specified range
  This takes the log listener mask, the log extended listener mask, and
  all the diag stream masks and ORs together their bits.
  The end result is a mask which has its bits turned on if any of the masks
  show a request for that particular log code.

PARAMETERS
  start - first mask index to be updated
  end - last mask index to be updated (inclusive)

RETURN VALUE
  None

===========================================================================*/
void log_update_composite_mask(uint32 start, uint32 end)
{
  int mask_index, stream_index;
  byte temp_mask=0;

  if(end >= LOG_MASK_SIZE) {
    end = LOG_MASK_SIZE - 1;
  }

  if(start > end) {
    return;
  }

  for(mask_index = start; mask_index <= end; mask_index++)
  {
    temp_mask = 0;
    
    #ifdef DIAG_LISTENERS_SUPPORT
    temp_mask  = log_listener_mask[mask_index];
    #endif /* DIAG_LISTENERS_SUPPORT */

  if(log_state == LOG_NORMAL_STATE) {
    for(stream_index = 0; stream_index < DIAG_MAX_STREAM_ID; stream_index += 1)
      {
            temp_mask |= log_mask[stream_index][mask_index];
      }
    }
    log_composite_mask[mask_index] =temp_mask;
  }
}

/*===========================================================================

FUNCTION LOG_STATUS_ADV

DESCRIPTION
  Checks if log code is enabled for both streams.

PARAMETERS

RETURN VALUE
  Returns appropriate stream_id.

===========================================================================*/
byte log_status_adv (log_code_type code)
{
  byte stream_id = 0;
  #ifdef DIAG_LISTENERS_SUPPORT
    boolean status = FALSE;
    unsigned int id, item;
  #endif /* DIAG_LISTENERS_SUPPORT */

  if (log_state == LOG_NORMAL_STATE || log_state == LOG_FLUSH_STATE)
  {
    #ifdef DIAG_LISTENERS_SUPPORT
    id = LOG_GET_EQUIP_ID (code);
    item = LOG_GET_ITEM_NUM (code);

    status = log_mask_enabled(log_listener_mask, id, item);

    if(status)
    {
      /* Set DIAG_STREAM_LISTENER bit if listeners active */
      stream_id |= DIAG_STREAM_LISTENER;
    }
    #endif /* #ifdef DIAG_LISTENERS_SUPPORT */

    if (log_state == LOG_NORMAL_STATE)
    {
      if(log_status_mask (code, DIAG_STREAM_1))
        stream_id |= DIAG_STREAM_1;
    #ifdef DIAG_CONSUMER_API
      if(log_status_mask (code, DIAG_STREAM_2))
        stream_id |= DIAG_STREAM_2;
    #endif /* #ifdef DIAG_CONSUMER_API */
    }
  }

  return stream_id;
} /* log_status_adv */


/*===========================================================================

FUNCTION DIAG_SWITCH_LOG_PRESET_MASK

DESCRIPTION

PARAMETERS
  None

RETURN VALUE
  None

===========================================================================*/
void diag_switch_log_preset_mask()
{
#if defined(DIAG_MP_MASTER)
  uint8 i, j = 0;
#endif

  osal_lock_mutex(&log_preset_mask_cs);

  log_mask[DIAG_STREAM_1-1] = &diag_log_preset_mask[diag_cur_preset_id-1][0];

#if defined(DIAG_MP_MASTER)
  diag_log_mask_mstr[DIAG_STREAM_1-1] = &diag_log_mask_mstr_preset[diag_cur_preset_id-1];

  for( i=0; i < NUM_SMD_PORT_TYPES; i++ )
  {
    for( j=0; j < NUM_SMD_PORTS; j++ )
      diagcomm_io_ctrl_conn[i][j].log_mask_update[DIAG_STREAM_1-1] =
      diagcomm_io_ctrl_conn[i][j].log_mask_preset_update[diag_cur_preset_id-1];
  }
#endif

  /* Update entire log mask since stream 1 has switched preset masks. */
  log_update_composite_mask( 0, LOG_MASK_SIZE-1 );


  osal_unlock_mutex(&log_preset_mask_cs);

  (void)diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);

  /* Config may have changed. Run sleep vote routine. */
  (void) log_sleep_vote ();

} /* diag_switch_log_preset_mask */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_SEND_LOG_MASK

DESCRIPTION
  Composes a CTRL msg to send a log mask update to specified Slave Diag.

PARAMETERS
  port_type - Port type (DIAGCOMM_PORT_SIO/DIAGCOMM_PORT_SMD)
  port_num - Port num (DIAGCOMM_PORT_1, DIAGCOMM_PORT_2, ...)
  stream_id - Log stream to send
  preset_id - A value between 0 (all presets) and DIAG_MAX_PRESET_ID
              which indicates what preset needs to be updated
  send_now - If True, send CTRL msg immediately.
             If False, aggregate until DSM is full before sending.

RETURN VALUE
  None

===========================================================================*/
/* On MP Master only */
#if defined(DIAG_MP_MASTER)
void diagcomm_ctrl_send_log_mask( diagcomm_port_type port_type,
                                  diagcomm_enum_port_type port_num,
                                  uint8 stream_id,
                                  uint8 preset_id,
                                  boolean send_now )
{
  diag_ctrl_msg_type ctrl_log_mask;
  boolean is_sent, update_all = FALSE;
  uint8 equip_id = 0;
  boolean * mask_status_ptr = NULL;
  diag_log_mask_type * log_mask_mstr_ptr = NULL;
  diagcomm_io_conn_type * conn = NULL;

  conn = &diagcomm_io_ctrl_conn[port_type][port_num];

  /* Check if stream id is valid */
  if( (stream_id > DIAG_MAX_STREAM_ID) || (stream_id < DIAG_MIN_STREAM_ID) ||
      (preset_id > DIAG_MAX_PRESET_ID) || (preset_id < DIAG_MIN_PRESET_ID) )
    return;

  /* Check if port has been opened */
  if( !conn->connected || !conn->channel_connected )
    return;

  if( diagcomm_ctrl_mask_status(port_type, port_num, DIAG_UPDATE_MASK_LOGS, stream_id, preset_id) )
  {
    update_all = TRUE;
    diagcomm_ctrl_clear_mask_status( port_type, port_num, DIAG_UPDATE_MASK_LOGS, stream_id, preset_id );
    diagcomm_ctrl_clear_mask_status( port_type, port_num, DIAG_UPDATE_MASK_PARTIAL_LOGS, stream_id, preset_id ); //ToDo: combine mask clearing
  }
  else if( diagcomm_ctrl_mask_status(port_type, port_num, DIAG_UPDATE_MASK_PARTIAL_LOGS, stream_id, preset_id) )
  {
    diagcomm_ctrl_clear_mask_status( port_type, port_num, DIAG_UPDATE_MASK_PARTIAL_LOGS, stream_id, preset_id );
  }
  else
    return;

  /* Grab the right master log mask. */
  if( stream_id == DIAG_STREAM_1 )
  {
    log_mask_mstr_ptr = &diag_log_mask_mstr_preset[preset_id-1];
    mask_status_ptr = conn->log_mask_preset_update[preset_id-1];
  }
  #ifdef DIAG_CONSUMER_API
    else if(stream_id == DIAG_STREAM_2 )
    {
      log_mask_mstr_ptr = &diag_log_mask_mstr_dci[DIAG_DCI_MASK_1-1];
      mask_status_ptr = conn->log_mask_dci_update[DIAG_DCI_MASK_1-1];
    }
  #endif /* #ifdef DIAG_CONSUMER_API */

  if( DIAG_IS_INT_FEATURE_BIT_SET(diag_int_feature_mask, F_DIAG_PRESET_MASKS) &&
      DIAG_IS_INT_FEATURE_BIT_SET(conn->feature_mask, F_DIAG_PRESET_MASKS) )
  {
    // Compose DIAG_CTRL_MSG_EQUIP_LOG_MASK_V3 packet
    ctrl_log_mask.cmd_type = DIAG_CTRL_MSG_EQUIP_LOG_MASK_V3;
    ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.stream_id = stream_id;
    ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.preset_id = preset_id;
    ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.status = log_mask_mstr_ptr->status;

    if( (log_mask_mstr_ptr->status == DIAG_CTRL_MASK_ALL_DISABLED) ||
        (log_mask_mstr_ptr->status == DIAG_CTRL_MASK_ALL_ENABLED) )
    {
      ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.equip_id = 0;
      ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.last_item = 0;
      ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.log_mask_size = 0;
      ctrl_log_mask.data_len = sizeof(diag_ctrl_msg_equip_log_mask_v3_type);

      if(diagcomm_ctrl_send_mult_pkt( (PACK(void *))&ctrl_log_mask,
                                         DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_equip_log_mask_v3_type),
                                         NULL, 0,
                                         port_type, port_num, send_now, &is_sent ) > 0)
      {
        for( equip_id = 0; equip_id <= LOG_EQUIP_ID_MAX; equip_id++ )
        {
          mask_status_ptr[equip_id] = FALSE;
        }
      }
      else
      {
        // Update Failed; bail out and try again later
        if( update_all )
        {
          diagcomm_ctrl_set_mask_status( port_type, port_num, DIAG_UPDATE_MASK_LOGS, stream_id, preset_id );
        }
        else
        {
          diagcomm_ctrl_set_mask_status( port_type, port_num, DIAG_UPDATE_MASK_PARTIAL_LOGS, stream_id, preset_id );
        }
      }
    }
    else if( log_mask_mstr_ptr->status == DIAG_CTRL_MASK_VALID )
    {
    for( equip_id = 0; equip_id <= LOG_EQUIP_ID_MAX; equip_id++ )
      {
        if( update_all || (mask_status_ptr[equip_id] == TRUE) )
        {
          if( diag_log_last_item_mstr_tbl[equip_id] > 0 )
          {
            // Send mask for only equip id's which have log codes
            ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.equip_id = equip_id;
            ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.last_item = diag_log_last_item_mstr_tbl[equip_id];
            ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.log_mask_size = LOG_MASK_ARRAY_INDEX(diag_log_last_item_mstr_tbl[equip_id])+1;
            ctrl_log_mask.data_len = sizeof(diag_ctrl_msg_equip_log_mask_v3_type) +
                                     ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.log_mask_size;

            if(diagcomm_ctrl_send_mult_pkt( (PACK(void *))&ctrl_log_mask,
                                               DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_equip_log_mask_v3_type),
                                               (PACK(void *))&log_mask_mstr_ptr->mask[equip_id][0],
                                               ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.log_mask_size,
                                               port_type, port_num, send_now, &is_sent ) > 0)
            {
              mask_status_ptr[equip_id] = FALSE;
            }
            else
            {
              // Set things for when you come back to try again...
              diagcomm_ctrl_set_mask_status( port_type, port_num, DIAG_UPDATE_MASK_PARTIAL_LOGS, stream_id, preset_id );
              mask_status_ptr[equip_id] = TRUE;
            }
          }
        }
      }

      // No more msg mask tables to fwd; Send the current DSM item immediately and then quit
      diagcomm_ctrl_send_mult_pkt( NULL, 0, NULL, 0, port_type, port_num, send_now, &is_sent );
    }
  }
  else
  {
    /* Compose DIAG_CTRL_MSG_EQUIP_LOG_MASK packet - For backwards compatibility */
    ctrl_log_mask.cmd_type = DIAG_CTRL_MSG_EQUIP_LOG_MASK;
    ctrl_log_mask.data.ctrl_msg_equip_log_mask.stream_id = stream_id;
    ctrl_log_mask.data.ctrl_msg_equip_log_mask.status = log_mask_mstr_ptr->status;

    if( (log_mask_mstr_ptr->status == DIAG_CTRL_MASK_ALL_DISABLED) ||
        (log_mask_mstr_ptr->status == DIAG_CTRL_MASK_ALL_ENABLED) )
    {
      ctrl_log_mask.data.ctrl_msg_equip_log_mask.equip_id = 0;
      ctrl_log_mask.data.ctrl_msg_equip_log_mask.last_item = 0;
      ctrl_log_mask.data.ctrl_msg_equip_log_mask.log_mask_size = 0;
      ctrl_log_mask.data_len = sizeof(diag_ctrl_msg_equip_log_mask_type);

      if(diagcomm_ctrl_send_mult_pkt( (PACK(void *))&ctrl_log_mask,
                                         DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_equip_log_mask_type),
                                         NULL, 0,
                                         port_type, port_num, send_now, &is_sent ) > 0)
      {
        for( equip_id = 0; equip_id <= LOG_EQUIP_ID_MAX; equip_id++ )
        {
          mask_status_ptr[equip_id] = FALSE;
        }
      }
      else
      {
        /* Update Failed; bail out and try again later */
        if( update_all )
        {
          diagcomm_ctrl_set_mask_status( port_type, port_num, DIAG_UPDATE_MASK_LOGS, stream_id, preset_id );
        }
        else
        {
          diagcomm_ctrl_set_mask_status( port_type, port_num, DIAG_UPDATE_MASK_PARTIAL_LOGS, stream_id, preset_id );
        }
      }
    }
    else if( log_mask_mstr_ptr->status == DIAG_CTRL_MASK_VALID )
    {
      for( equip_id = 0; equip_id <= LOG_EQUIP_ID_MAX; equip_id++ )
      {
        if( update_all || (mask_status_ptr[equip_id] == TRUE) )
        {
          if( diag_log_last_item_mstr_tbl[equip_id] > 0 )
          {
            /* Send mask for only equip id's which have log codes */
            ctrl_log_mask.data.ctrl_msg_equip_log_mask.equip_id = equip_id;
            ctrl_log_mask.data.ctrl_msg_equip_log_mask.last_item = diag_log_last_item_mstr_tbl[equip_id];
            ctrl_log_mask.data.ctrl_msg_equip_log_mask.log_mask_size = LOG_MASK_ARRAY_INDEX(diag_log_last_item_mstr_tbl[equip_id])+1;
            ctrl_log_mask.data_len = sizeof(diag_ctrl_msg_equip_log_mask_type) +
                                     ctrl_log_mask.data.ctrl_msg_equip_log_mask.log_mask_size;

            if(diagcomm_ctrl_send_mult_pkt( (PACK(void *))&ctrl_log_mask,
                                               DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_equip_log_mask_type),
                                               (PACK(void *))&log_mask_mstr_ptr->mask[equip_id][0],
                                               ctrl_log_mask.data.ctrl_msg_equip_log_mask.log_mask_size,
                                               port_type, port_num, send_now, &is_sent ) > 0)
            {
              mask_status_ptr[equip_id] = FALSE;
            }
            else
            {
              /* Set things for when you come back to try again... */
              diagcomm_ctrl_set_mask_status( port_type, port_num, DIAG_UPDATE_MASK_PARTIAL_LOGS, stream_id, preset_id );
              mask_status_ptr[equip_id] = TRUE;
            }
          }
        }
      }

      /* No more msg mask tables to fwd; Send the current DSM item immediately and then quit */
      diagcomm_ctrl_send_mult_pkt( NULL, 0, NULL, 0, port_type, port_num, send_now, &is_sent);
    }

  }

} /* diagcomm_ctrl_send_log_mask */
#endif /* DIAG_MP_MASTER */


/*===========================================================================

FUNCTION DIAG_CTRL_UPDATE_LOG_MASK

DESCRIPTION
  Given a new log mask, updates the local copy of the log mask with the
  new mask.

PARAMETERS
  ctrl_msg - Ptr to CTRL msg

RETURN VALUE
  None

===========================================================================*/
void diag_ctrl_update_log_mask( diag_ctrl_msg_type * ctrl_msg )
{
  uint8 * mask_ptr = NULL;
  uint32 mask_len, mask_end, len = 0;
  uint8 stream_id, status, equip_id = 0;
  diag_log_mask_type * log_mask_mstr_ptr = NULL;

  if( (ctrl_msg == NULL) || (ctrl_msg->cmd_type != DIAG_CTRL_MSG_EQUIP_LOG_MASK) )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_mask Sanity check failed");
    return;
  }

  mask_len = ctrl_msg->data.ctrl_msg_equip_log_mask.log_mask_size;
  stream_id = ctrl_msg->data.ctrl_msg_equip_log_mask.stream_id;
  status = ctrl_msg->data.ctrl_msg_equip_log_mask.status;
  equip_id = ctrl_msg->data.ctrl_msg_equip_log_mask.equip_id;

  if( (stream_id > DIAG_MAX_STREAM_ID) || (stream_id < DIAG_MIN_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_mask: Invalid stream_id %d", stream_id);
    return;
  }

  if(equip_id > LOG_EQUIP_ID_LAST)
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_mask: Invalid equip_id %d", equip_id);
    return;
  }

  if( mask_len > (ctrl_msg->data_len - sizeof(diag_ctrl_msg_equip_log_mask_type)) )
  {
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_mask: Invalid lengths %d, %d",
          mask_len, ctrl_msg->data_len);
    return;
  }
  #ifdef DIAG_CONSUMER_API
    if(stream_id == DIAG_STREAM_2)
    {
       diag_log_mask_mstr_dci[DIAG_DCI_MASK_1-1].status = status;
    }
  #endif /* #ifdef DIAG_CONSUMER_API */
  if( status == DIAG_CTRL_MASK_ALL_DISABLED )
  {

    (void) log_config_mask( -1, FALSE, stream_id, diag_cur_preset_id );
    (void) log_config_mstr_mask ( -1, FALSE, stream_id, diag_cur_preset_id );
  }
  else if( status == DIAG_CTRL_MASK_ALL_ENABLED )
  {

    (void) log_config_mask( -1, TRUE, stream_id, diag_cur_preset_id );
    (void) log_config_mstr_mask ( -1, TRUE, stream_id, diag_cur_preset_id );
  }
  else if( status == DIAG_CTRL_MASK_VALID )
  {
    if( ctrl_msg->data_len != (sizeof(diag_ctrl_msg_equip_log_mask_type) + mask_len) )
    {
      MSG_2( MSG_SSID_DIAG, MSG_LEGACY_ERROR,
             "diag_ctrl_update_log_mask: Invalid lengths %d != %d",
             ctrl_msg->data_len, sizeof(diag_ctrl_msg_equip_log_mask_type) + mask_len );
      return;
    }

    mask_ptr = ((uint8 *)ctrl_msg) + DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_equip_log_mask_type);
    len = MIN( mask_len, LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id])+1 );
    memscpy( (void*)&log_mask[stream_id-1][log_mask_offset_tbl[equip_id]],
             LOG_MASK_SIZE-log_mask_offset_tbl[equip_id],mask_ptr, MIN(len, LOG_MASK_SIZE) );

    /* Update composite mask */
    mask_end = log_mask_offset_tbl[equip_id] + len - 1;
    log_update_composite_mask(log_mask_offset_tbl[equip_id], mask_end);

    #ifdef DIAG_CONSUMER_API
      log_mask_mstr_ptr = &diag_log_mask_mstr_dci[DIAG_DCI_MASK_1-1];
    #endif /* #ifdef DIAG_CONSUMER_API */
    memscpy( (void*)&log_mask_mstr_ptr->mask[equip_id],
             LOG_MASK_SIZE-log_mask_offset_tbl[equip_id],mask_ptr, MIN(len, LOG_MASK_SIZE) );
  }

  (void)diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);

  /* Config may have changed. Run sleep vote routine. */
  (void) log_sleep_vote ();

} /* diag_ctrl_update_log_mask */

/*===========================================================================

FUNCTION DIAG_CTRL_UPDATE_LOG_PRESET_MASK

DESCRIPTION
  Given a new log mask, updates the local copy of the log mask with the
  new mask.

PARAMETERS
  ctrl_msg - Ptr to CTRL msg

RETURN VALUE
  None

===========================================================================*/
void diag_ctrl_update_log_preset_mask( diag_ctrl_msg_type * ctrl_msg )
{
  uint8 * ctrl_mask_ptr = NULL;
  byte * log_mask_ptr = NULL;
  uint32 mask_len, len = 0;
  uint32 length = 0;
  uint8 stream_id, preset_id, status, equip_id = 0;

  if( (ctrl_msg == NULL) || (ctrl_msg->cmd_type != DIAG_CTRL_MSG_EQUIP_LOG_MASK_V3) )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_preset_mask Sanity check failed");
    return;
  }

  mask_len = ctrl_msg->data.ctrl_msg_equip_log_mask_v3.log_mask_size;
  stream_id = ctrl_msg->data.ctrl_msg_equip_log_mask_v3.stream_id;
  preset_id = ctrl_msg->data.ctrl_msg_equip_log_mask_v3.preset_id;
  status = ctrl_msg->data.ctrl_msg_equip_log_mask_v3.status;
  equip_id = ctrl_msg->data.ctrl_msg_equip_log_mask_v3.equip_id;

  MSG_6(MSG_SSID_DIAG, MSG_LEGACY_LOW, "update_log_preset_mask procid=%d stream=%d preset=%d status=%d equip_id=%d len=%d",
        DIAG_MY_PROC_ID, stream_id, preset_id, status, equip_id, mask_len);

  if( equip_id > LOG_EQUIP_ID_LAST )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_preset_mask: Invalid equip_id %d", equip_id);
    return;
  }

  if( mask_len > (ctrl_msg->data_len - sizeof(diag_ctrl_msg_equip_log_mask_v3_type)) )
  {
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_preset_mask: Invalid lengths %d, %d",
          mask_len, ctrl_msg->data_len);
    return;
  }

  if( stream_id == DIAG_STREAM_1 )
  {
    if( (preset_id >= DIAG_MIN_PRESET_ID) && (preset_id <= DIAG_MAX_PRESET_ID) )
    {
      log_mask_ptr = &diag_log_preset_mask[preset_id-1][log_mask_offset_tbl[equip_id]];
      length = LOG_MASK_SIZE - log_mask_offset_tbl[equip_id];
    }
    else
      return;
  }
  #ifdef DIAG_CONSUMER_API
    else if( stream_id == DIAG_STREAM_2 )
    {
      log_mask_ptr = &diag_log_dci_mask[log_mask_offset_tbl[equip_id]];
      length = LOG_MASK_SIZE;
      preset_id = DIAG_DCI_MASK_1;
    }
  #endif /* #ifdef DIAG_CONSUMER_API */
  else
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_preset_mask: Invalid stream_id %d", stream_id);
    return;
  }

  if( status == DIAG_CTRL_MASK_ALL_DISABLED )
  {
    (void) log_config_mask( -1, FALSE, stream_id, preset_id );
  }
  else if( status == DIAG_CTRL_MASK_ALL_ENABLED )
  {
    (void) log_config_mask( -1, TRUE, stream_id, preset_id );
  }
  else if( status == DIAG_CTRL_MASK_VALID )
  {
    if( ctrl_msg->data_len != (sizeof(diag_ctrl_msg_equip_log_mask_v3_type) + mask_len) )
    {
      MSG_2( MSG_SSID_DIAG, MSG_LEGACY_ERROR,
             "diag_ctrl_update_log_preset_mask: Invalid lengths %d != %d",
             ctrl_msg->data_len, sizeof(diag_ctrl_msg_equip_log_mask_v3_type) + mask_len );
      return;
    }

    ctrl_mask_ptr = ((uint8 *)ctrl_msg) + DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_equip_log_mask_v3_type);
    len = MIN( mask_len, LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id])+1 );
    memscpy( (void*)log_mask_ptr, length, ctrl_mask_ptr, MIN(len, LOG_MASK_SIZE) );
  }

  (void)diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);

  /* Config may have changed. Run sleep vote routine. */
  (void) log_sleep_vote ();

} /* diag_ctrl_update_log_preset_mask */


#if defined(DIAG_MP_MASTER)

void diag_ctrl_update_log_range(diag_ctrl_msg_type *msg_pkt_ptr, uint32 data_pkt_len)
{
  diag_ctrl_log_range_type *range_msg;
  int i;

  range_msg = &(msg_pkt_ptr->data.ctrl_msg_bt_log_range);

  if ((msg_pkt_ptr->cmd_type != DIAG_CTRL_BT_LOG_RANGE)
        || range_msg->version != DIAG_CTRL_MSG_BT_MASK_VER)
        return;

  log_equip_id_last = range_msg->log_last_equipId;
  ASSERT(log_equip_id_last <= LOG_EQUIP_ID_MAX);

  for(i=0; i < range_msg->num_equipid_ranges; i++)
  {
    if( log_mstr_last_item_tbl[range_msg->ranges[i].equip_id] < range_msg->ranges[i].num_items )
    {
      log_mstr_last_item_tbl[range_msg->ranges[i].equip_id] = range_msg->ranges[i].num_items;
    }
  }
}

#endif

