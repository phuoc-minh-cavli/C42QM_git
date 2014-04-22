/*=============================================================================

                            BAM DMUX CORE

 BAM Data Multiplexer core.

 Copyright  2016-2017, 2019 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/bam_dmux/src/bam_dmux_core.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/16   rv      Initial version.
===========================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"
#include "stdint.h"
#include "bam_dmux.h"
#include "bam_dmux_log.h"
#include "bam_dmux_cfg.h"
#include "bam_dmux_os.h"
#include "bam_dmux_list.h"
#include "bam_dmux_power.h"
#include "bam_dmux_bam_iface.h"
#include "bam_dmux_core.h"

/*=============================================================================

                         MACRO DECLARATIONS

=============================================================================*/
/* Debug log macros */
#undef BAM_DMUX_LOG_TAG
#define BAM_DMUX_LOG_TAG "CORE"

#if 0
#define BAM_DMUX_LOG_FUNC_START() BAM_DMUX_LOG_DBG(1, "Func Start %s.", __FUNCTION__)
#define BAM_DMUX_LOG_FUNC_END()   BAM_DMUX_LOG_DBG(1, "Func End %s.", __FUNCTION__)
#define BAM_DMUX_LOG_FUNC_START_ARG(str, val) \
    BAM_DMUX_LOG_DBG(3, "Func Start %s. %s:%d", __FUNCTION__, str, val)
#define BAM_DMUX_LOG_FUNC_END_ARG(str, val)   \
    BAM_DMUX_LOG_DBG(3, "Func End %s. %s:%d", __FUNCTION__, str, val)

#else
#define BAM_DMUX_LOG_FUNC_START() do{}while(0)
#define BAM_DMUX_LOG_FUNC_END()   do{}while(0)
#define BAM_DMUX_LOG_FUNC_START_ARG(str, val) do{}while(0)
#define BAM_DMUX_LOG_FUNC_END_ARG(str, val)   do{}while(0)
#endif

/* Commands */
#define BAM_DMUX_CMD_DATA                 (0)
#define BAM_DMUX_CMD_OPEN                 (1)
#define BAM_DMUX_CMD_CLOSE                (2)
#define BAM_DMUX_CMD_STATUS               (3)
#define BAM_DMUX_CMD_OPEN_PC_DISABLED     (4)

/* Magic packet indicator */
#define BAM_DMUX_HEADER_MAGIC             (0x33FC)

/* Command options, MTU negotiation support */
#define BAM_DMUX_CMD_OPT_MTU_NEGOTIATION  (0x02)

/* Command options, RX buffers count */
#define BAM_DMUX_CMD_OPT_RX_BUF_CNT_MASK  (0x30)
#define BAM_DMUX_CMD_OPT_RX_BUF_CNT_256   (0x30)
#define BAM_DMUX_CMD_OPT_RX_BUF_CNT_128   (0x20)
#define BAM_DMUX_CMD_OPT_RX_BUF_CNT_64    (0x10)
#define BAM_DMUX_CMD_OPT_RX_BUF_CNT_32    (0x00)

/* Command options, RX MUT size */
#define BAM_DMUX_CMD_OPT_RX_MTU_SIZE_MASK (0xC0)
#define BAM_DMUX_CMD_OPT_RX_MTU_SIZE_16K  (0xC0)
#define BAM_DMUX_CMD_OPT_RX_MTU_SIZE_8K   (0x80)
#define BAM_DMUX_CMD_OPT_RX_MTU_SIZE_4K   (0x40)
#define BAM_DMUX_CMD_OPT_RX_MTU_SIZE_2K   (0x00)

/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/

/* Header structure */
typedef struct __attribute__((__packed__)) {
  uint16 magic;    /** magic number */
  uint8  cmd_opts; /** command options */
  uint8  cmd;      /** command (open, close, data) */
  uint8  pad;      /** Padding added to the packet */
  uint8  channel;  /** channel number (destination) */
  uint16 len;      /** Length of the packet */
} bam_dmux_header_type;

/* Channel local states */
typedef enum {
  BAM_DMUX_CH_LOCAL_STATE_CLOSED  = 0,
  BAM_DMUX_CH_LOCAL_STATE_OPENING = 1,
  BAM_DMUX_CH_LOCAL_STATE_OPENED  = 2,
  BAM_DMUX_CH_LOCAL_STATE_CLOSING = 3
} bam_dmux_ch_local_state_type;

/* Channel remote states */
typedef enum {
  BAM_DMUX_CH_REMOTE_STATE_CLOSED = 0,
  BAM_DMUX_CH_REMOTE_STATE_OPENED = 2
} bam_dmux_ch_remote_state_type;

/* Local power vote states */
typedef enum {
  BAM_DMUX_LOCAL_POWER_UNVOTED  = 0,
  BAM_DMUX_LOCAL_POWER_UNVOTING = 1,
  BAM_DMUX_LOCAL_POWER_VOTING   = 2,
  BAM_DMUX_LOCAL_POWER_VOTED    = 3
} bam_dmux_local_power_state_type;

/* Remote power states */
typedef enum {
  BAM_DMUX_REMOTE_POWER_OFF  = 0,
  BAM_DMUX_REMOTE_POWER_ON   = 1
} bam_dmux_remote_power_state_type;

/* Forward typedef declaration */
typedef struct bam_dmux_channel bam_dmux_channel_type;
typedef struct bam_dmux_bufdesc bam_dmux_bufdesc_type;
typedef void (*bam_dmux_done_cb_type)(bam_dmux_bufdesc_type *bufdesc);

/* Structure to hold a transfer details */
struct bam_dmux_bufdesc {
  bam_dmux_channel_type *ch;
  bam_dmux_done_cb_type done_cb;
  void                  *buf;
  uint32                buf_pa;
  uint16                len;
  void                  *priv;
  bam_dmux_status       status;

  BAM_DMUX_LINK(bam_dmux_bufdesc_type, link);
};

/* Logical channel info */
struct bam_dmux_channel {
  uint8                         id;
  bam_dmux_os_mutex_type        mutex;

  /* Channel states */
  bam_dmux_ch_local_state_type  local_state;
  bam_dmux_ch_remote_state_type remote_state;

  /* Client data */
  void                          *priv;
  bam_dmux_event_notify_cb      event_cb;
  bam_dmux_tx_done_notify_cb    tx_done_cb;
  bam_dmux_rx_notify_cb         rx_cb;

  /* Channel options */
  boolean                       power_collapse;
  boolean                       mtu_negotiation;
  uint16                        remote_max_rx_buf_size;

  /* command buffer descriptor */
  /* Command sending is failed and stored it into this variable for 
   * resending when it gets a chance */
  bam_dmux_header_type          *cmd_pending;
  
  /* Command is sent successful and waiting for done */
  bam_dmux_header_type          *cmd_sent;

  /* TX and RX */
  boolean                       notify_tx_resume;
  
  /* TX bufdesc list */
  BAM_DMUX_LIST(bam_dmux_bufdesc_type, tx_bufdescs);
  
  /* Statistics */
  uint32                        stat_tx_curr_cnt;
  uint32                        stat_tx_total_cnt;
  uint32                        stat_rx_curr_cnt;
  uint32                        stat_rx_total_cnt;
};

/* Function prototype */
static void bam_dmux_tx_cmd
(
  bam_dmux_channel_type *ch,
  uint8            cmd,
  uint8            cmd_opts
);

/*=============================================================================

                       LOCAL DATA DEFINATION

=============================================================================*/
/* Mutex used to protect common data */
static bam_dmux_os_mutex_type    bam_dmux_common_mutex;

/* Logical channels */
static bam_dmux_channel_type     *bam_dmux_channels[BAM_DMUX_CFG_MAX_CHANNELS];

/* States */
static volatile boolean          bam_dmux_bam_init_status;
static volatile boolean          bam_dmux_bam_state;
static volatile boolean          bam_dmux_power_collapse_en;

static volatile bam_dmux_local_power_state_type  bam_dmux_local_power_state;
static volatile bam_dmux_remote_power_state_type bam_dmux_remote_power_state;

/* TX Inactivity timer */
static bam_dmux_os_timer_type    bam_dmux_tx_inactivity_timer;

/* Global flags */
static volatile uint32           bam_dmux_tx_cmd_pending;
static volatile uint32           bam_dmux_tx_data_pending;

/* TX and RX buffer descriptors */
static bam_dmux_bufdesc_type     *bam_dmux_tx_bufdescs;
static bam_dmux_bufdesc_type     *bam_dmux_rx_bufdescs;

/* Free TX buffer descriptors list */
static BAM_DMUX_LIST(bam_dmux_bufdesc_type, bam_dmux_free_tx_bufdescs);

/* RX submited list, only for debugging */
static BAM_DMUX_LIST(bam_dmux_bufdesc_type, bam_dmux_submited_rx_bufdescs);

/*=============================================================================

                     PUBLIC VARIABLE DECLARATIONS

=============================================================================*/


/*=============================================================================

                     LOCAL FUNCTION DEFINATION

=============================================================================*/

/*=============================================================================
  FUNCTION  bam_dmux_power_event_str
=============================================================================*/
/**
 * Returns the string for input power event.
 *
 */
/*===========================================================================*/
static const char *bam_dmux_power_event_str(bam_dmux_power_event_type event)
{
  const char *event_str;
  
  switch (event)
  {
    case BAM_DMUX_POWER_EVENT_OFF:
      event_str = "POWER_OFF";
      break;

    case BAM_DMUX_POWER_EVENT_ON:
      event_str = "POWER_ON";
      break;
      
    case BAM_DMUX_POWER_EVENT_VOTED:
      event_str = "POWER_VOTED";
      break;
      
    case BAM_DMUX_POWER_EVENT_UNVOTED:
      event_str = "POWER_UNVOTED";
      break;
      
    case BAM_DMUX_POWER_EVENT_FATAL:
      event_str = "POWER_FATAL";
      break;
      
    default:
      event_str = "EVENT_UNKNOWN";
      break;
  }
  
  return event_str;
}

/*=============================================================================
  FUNCTION  bam_dmux_local_power_state_str
=============================================================================*/
/**
 * Returns the string for input local power state.
 *
 */
/*===========================================================================*/
static const char *bam_dmux_local_power_state_str(bam_dmux_local_power_state_type state)
{
  const char *state_str;
  
  switch (state)
  {
    case BAM_DMUX_LOCAL_POWER_UNVOTED:
      state_str = "POWER_UNVOTED";
      break;

    case BAM_DMUX_LOCAL_POWER_UNVOTING:
      state_str = "POWER_UNVOTING";
      break;
      
    case BAM_DMUX_LOCAL_POWER_VOTING:
      state_str = "POWER_VOTING";
      break;
      
    case BAM_DMUX_LOCAL_POWER_VOTED:
      state_str = "POWER_VOTED";
      break;
      
    default:
      state_str = "UNKNOWN";
      break;
  }
  
  return state_str;
}

/*=============================================================================
  FUNCTION  bam_dmux_remote_power_state_str
=============================================================================*/
/**
 * Returns the string for input remote power state.
 *
 */
/*===========================================================================*/
 const char *bam_dmux_remote_power_state_str(bam_dmux_remote_power_state_type state)
{
  const char *state_str;
  
  switch (state)
  {
    case BAM_DMUX_REMOTE_POWER_OFF:
      state_str = "POWER_OFF";
      break;

    case BAM_DMUX_REMOTE_POWER_ON:
      state_str = "POWER_ON";
      break;
      
    default:
      state_str = "UNKNOWN";
      break;
  }
  
  return state_str;
}

/*=============================================================================
  FUNCTION  bam_dmux_bufdescs_init
=============================================================================*/
/**
 * Allocate and initializes the TX and RX buffer descriptors.
 *
 */
/*===========================================================================*/
static void bam_dmux_bufdescs_init(void)
{
  uint16 idx;

  BAM_DMUX_LOG_FUNC_START();

  /* Maintain the buffer descriptors under common mutex protection */
  bam_dmux_os_mutex_lock(&bam_dmux_common_mutex);

  /* Allocate buffer descriptors to hold the info about TX/RX buffers */
  bam_dmux_tx_bufdescs = bam_dmux_os_calloc_assert(BAM_DMUX_CFG_MAX_TX_BUFS,
                                                   sizeof(bam_dmux_bufdesc_type));

  bam_dmux_rx_bufdescs = bam_dmux_os_calloc_assert(BAM_DMUX_CFG_MAX_RX_BUFS,
                                                   sizeof(bam_dmux_bufdesc_type));

  /* Initialize the free TX buffer descriptors list */
  BAM_DMUX_LIST_INIT(bam_dmux_free_tx_bufdescs);

  /* RX submited list */
  BAM_DMUX_LIST_INIT(bam_dmux_submited_rx_bufdescs);

  /* Add TX buffer descriptors to free list */
  for (idx = 0; idx < BAM_DMUX_CFG_MAX_TX_BUFS; idx++)
  {
    BAM_DMUX_LINK_INIT(bam_dmux_tx_bufdescs[idx].link);

    BAM_DMUX_LIST_ADD(bam_dmux_free_tx_bufdescs,
                      &bam_dmux_tx_bufdescs[idx],
                      link);
  }

  bam_dmux_os_mutex_unlock(&bam_dmux_common_mutex);
}

/*=============================================================================
  FUNCTION  bam_dmux_power_vote_helper
=============================================================================*/
/**
 * Helper function to vote/unvote for A2 Power.
 */
/*===========================================================================*/
static void bam_dmux_power_vote_helper(boolean vote)
{
  BAM_DMUX_LOG_INFO(3, "Power Vote=%d, local_state=%s, remote_state=%s.", vote,
                    bam_dmux_local_power_state_str(bam_dmux_local_power_state),
                    bam_dmux_remote_power_state_str(bam_dmux_remote_power_state));

  if ((bam_dmux_local_power_state == BAM_DMUX_LOCAL_POWER_VOTING) ||
      (bam_dmux_local_power_state == BAM_DMUX_LOCAL_POWER_UNVOTING))
  {
    BAM_DMUX_LOG_FATAL(2, "Last power vote/unvote is not yet acked. "
                       "local_vote=%d, remote_power=%d",
                    bam_dmux_local_power_state, bam_dmux_remote_power_state);
  }

  if (vote)
  {
    bam_dmux_local_power_state = BAM_DMUX_LOCAL_POWER_VOTING;
    bam_dmux_power_vote();
  }
  else
  {
    bam_dmux_local_power_state = BAM_DMUX_LOCAL_POWER_UNVOTING;
    bam_dmux_power_unvote();
  }
}

/*=============================================================================
  FUNCTION  bam_dmux_tx_inactivity_timer_cb
=============================================================================*/
/**
 * TX inactivity timeout function. This function checks if there is any 
 * TX activity within the timeout value. If there is any activity, it restart 
 * the timer otherwise calls Core layer TX inactivity timeout function.
 *
 */
/*===========================================================================*/
static void bam_dmux_tx_inactivity_timer_cb(void *ctx)
{
  BAM_DMUX_LOG_FUNC_START_ARG("TxCnt", BAM_DMUX_LIST_CNT(bam_dmux_free_tx_bufdescs));

  bam_dmux_os_mutex_lock(&bam_dmux_common_mutex);
  
  if (BAM_DMUX_LIST_CNT(bam_dmux_free_tx_bufdescs) == BAM_DMUX_CFG_MAX_TX_BUFS)
  {
    /* There is no TX activity, unvote power */
    if (bam_dmux_local_power_state == BAM_DMUX_LOCAL_POWER_VOTED)
    {
      bam_dmux_power_vote_helper(FALSE);
    }
  }

  bam_dmux_os_mutex_unlock(&bam_dmux_common_mutex);

  BAM_DMUX_LOG_FUNC_END_ARG("TxCnt", BAM_DMUX_LIST_CNT(bam_dmux_free_tx_bufdescs));
}

/*=============================================================================
  FUNCTION  bam_dmux_check_and_start_inactivity_timer
=============================================================================*/
/**
 * Check for inactivity and start the timer.
 *
 */
/*===========================================================================*/
static void bam_dmux_check_and_start_inactivity_timer(void)
{
  /* Check if all are tx bufdescs freed, to powerdown clock */
  if ((BAM_DMUX_LIST_CNT(bam_dmux_free_tx_bufdescs) == BAM_DMUX_CFG_MAX_TX_BUFS) &&
      (bam_dmux_power_collapse_en))
  {
    /* Start inactivity timer, as there is no TX activity */
    bam_dmux_os_timer_set(&bam_dmux_tx_inactivity_timer,
                          BAM_DMUX_CFG_MAX_UL_TIMEOUT);
  }
}

/*=============================================================================
  FUNCTION  bam_dmux_tx_bufdesc_put
=============================================================================*/
/**
 * Puts buffer descriptor into the free TX list.
 *
 */
/*===========================================================================*/
static void bam_dmux_tx_bufdesc_put(bam_dmux_bufdesc_type *bufdesc)
{
  boolean tx_inactive;
  
  BAM_DMUX_LOG_FUNC_START_ARG("TxCnt", BAM_DMUX_LIST_CNT(bam_dmux_free_tx_bufdescs));
  
  bam_dmux_os_mutex_lock(&bam_dmux_common_mutex);

  memset(bufdesc, 0, sizeof(bam_dmux_bufdesc_type));
  BAM_DMUX_LIST_ADD(bam_dmux_free_tx_bufdescs, bufdesc, link);
  
  bam_dmux_check_and_start_inactivity_timer();
  
  bam_dmux_os_mutex_unlock(&bam_dmux_common_mutex);

  BAM_DMUX_LOG_FUNC_END_ARG("TxCnt", BAM_DMUX_LIST_CNT(bam_dmux_free_tx_bufdescs));
}

/*=============================================================================
  FUNCTION  bam_dmux_tx_bufdesc_get
=============================================================================*/
/**
 * Gets TX buffer descriptor from the free TX list.
 * And votes for A2 power clock as TX is active.
 *
 */
/*===========================================================================*/
static bam_dmux_bufdesc_type *bam_dmux_tx_bufdesc_get(boolean *power_status)
{
  bam_dmux_bufdesc_type *bufdesc = NULL;
  boolean ret_power_status = FALSE;

  bam_dmux_os_mutex_lock(&bam_dmux_common_mutex);

  BAM_DMUX_LOG_DBG(3, "Tx_bufdesc_get enter TxCnt=%d, local_state=%s, remote_state=%s.",
                   BAM_DMUX_LIST_CNT(bam_dmux_free_tx_bufdescs),
                   bam_dmux_local_power_state_str(bam_dmux_local_power_state),
                   bam_dmux_remote_power_state_str(bam_dmux_remote_power_state));

  /* Check local & remote power state and BAM state */
  if ((bam_dmux_local_power_state  == BAM_DMUX_LOCAL_POWER_VOTED) &&
      (bam_dmux_remote_power_state == BAM_DMUX_REMOTE_POWER_ON) &&
      (bam_dmux_bam_state          == TRUE))
  {
    ret_power_status = TRUE;
  }

  /* Vote for A2 Power if it is not voted */
  if (bam_dmux_local_power_state == BAM_DMUX_LOCAL_POWER_UNVOTED)
  {
    bam_dmux_power_vote_helper(TRUE);
  }

  /* First check if free bufdescs available */  
  if (BAM_DMUX_LIST_CNT(bam_dmux_free_tx_bufdescs))
  {
    bufdesc = BAM_DMUX_LIST_HEAD(bam_dmux_free_tx_bufdescs);
    BAM_DMUX_LIST_REMOVE(bam_dmux_free_tx_bufdescs, bufdesc, link);
    BAM_DMUX_LINK_INIT(bufdesc->link);
  }

  if (power_status)
  {
    *power_status = ret_power_status;
  }

  BAM_DMUX_LOG_DBG(4, "Tx_bufdesc_get exit TxCnt=%d, local_state=%s, remote_state=%s, ret_power_status=%d.",
                   BAM_DMUX_LIST_CNT(bam_dmux_free_tx_bufdescs), 
                   bam_dmux_local_power_state_str(bam_dmux_local_power_state),
                   bam_dmux_remote_power_state_str(bam_dmux_remote_power_state), ret_power_status);
  
  bam_dmux_os_mutex_unlock(&bam_dmux_common_mutex);
  
  return bufdesc;
}

/*=============================================================================
  FUNCTION  bam_dmux_rx_bufdesc_submit
=============================================================================*/
/**
 * Submits the RX(DL) buffer to BAM.
 *
 */
/*===========================================================================*/
static void bam_dmux_rx_bufdesc_submit
(
  bam_dmux_bufdesc_type *bufdesc
)
{
  BAM_DMUX_LOG_FUNC_START();

  bam_dmux_os_mutex_lock(&bam_dmux_common_mutex);

  bufdesc->ch = NULL;

  if (bam_dmux_bam_state == FALSE)
  {
    BAM_DMUX_LOG_ERR(3, "RX buffer descriptor submit failed due to BAM not ready!"
        "bam_state=%d, local_vote=%d, remote_power=%d", 
        bam_dmux_bam_state, bam_dmux_local_power_state, bam_dmux_remote_power_state);
    bam_dmux_os_mutex_unlock(&bam_dmux_common_mutex);
    return;
  }

  memset(bufdesc->buf, 'D', BAM_DMUX_HEADER_SIZE);

  /* Add to the buf submited list */
  BAM_DMUX_LINK_INIT(bufdesc->link);
  BAM_DMUX_LIST_ADD(bam_dmux_submited_rx_bufdescs, bufdesc, link);

  /* Buffer descriptor should be already filled, as we are reusing it */
  if (!bam_dmux_bam_rx_buf_submit(bufdesc->buf,
                                  bufdesc->buf_pa,
                                  bufdesc->len,
                                  bufdesc))
  {
    BAM_DMUX_LOG_FATAL(0, "RX buffer descriptor submit failed!");
    BAM_DMUX_LIST_REMOVE(bam_dmux_submited_rx_bufdescs, bufdesc, link);
  }

  bam_dmux_os_mutex_unlock(&bam_dmux_common_mutex);

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_ch_alloc
=============================================================================*/
/**
 * Allocates and initializes new channel structure.
 *
 */
/*===========================================================================*/
static bam_dmux_channel_type *bam_dmux_ch_alloc(uint8 ch_id)
{
  bam_dmux_channel_type *ch;

  BAM_DMUX_LOG_FUNC_START();

  ch = bam_dmux_os_calloc_assert(1, sizeof(bam_dmux_channel_type));

  ch->id = ch_id;

  ch->local_state  = BAM_DMUX_CH_LOCAL_STATE_CLOSED;
  ch->remote_state = BAM_DMUX_CH_REMOTE_STATE_CLOSED;

  bam_dmux_os_mutex_init(&ch->mutex);

  BAM_DMUX_LOG_FUNC_END();

  return ch;
}

/*=============================================================================
  FUNCTION  bam_dmux_ch_free
=============================================================================*/
/**
 * Frees channel structure.
 *
 */
/*===========================================================================*/
static void bam_dmux_ch_free(bam_dmux_channel_type *ch)
{
  BAM_DMUX_LOG_FUNC_START();

  bam_dmux_os_mutex_deinit(&ch->mutex);

  bam_dmux_os_free(ch);

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_ch_get
=============================================================================*/
/**
 * Returns the channel structure of given channel id from global channel array.
 *
 */
/*===========================================================================*/
static bam_dmux_channel_type *bam_dmux_ch_get(uint8 ch_id)
{
  bam_dmux_channel_type *ch;

  if (ch_id >= BAM_DMUX_CFG_MAX_CHANNELS)
  {
    BAM_DMUX_LOG_ERR(1, "Trying to get channel from invalid ch_id=%d.", ch_id);
    return NULL;
  }

  /* Access bam_dmux_channels[] under common mutex protection */
  bam_dmux_os_mutex_lock(&bam_dmux_common_mutex);

  ch = bam_dmux_channels[ch_id];

  bam_dmux_os_mutex_unlock(&bam_dmux_common_mutex);

  return ch;
}

/*=============================================================================
  FUNCTION  bam_dmux_ch_set
=============================================================================*/
/**
 * Updates the channel structure in the global channel array.
 *
 */
/*===========================================================================*/
static void bam_dmux_ch_set(uint8 ch_id, bam_dmux_channel_type *ch)
{
  BAM_DMUX_LOG_FUNC_START();

  if (ch_id >= BAM_DMUX_CFG_MAX_CHANNELS)
  {
    BAM_DMUX_LOG_ERR(2, "Trying to set channel from invalid ch_id=%d, ch=0x%08X.", ch_id, ch);
  }

  /* Access bam_dmux_channels[] under common mutex protection */
  bam_dmux_os_mutex_lock(&bam_dmux_common_mutex);

  bam_dmux_channels[ch_id] = ch;

  bam_dmux_os_mutex_unlock(&bam_dmux_common_mutex);

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_header_fill
=============================================================================*/
/**
 * Fills the the header.
 *
 */
/*===========================================================================*/
static void bam_dmux_header_fill
(
  bam_dmux_header_type *hdr,
  uint8                ch_id,
  uint8                cmd,
  uint8                cmd_opts,
  uint16               len,
  uint8                pad_len
)
{
  hdr->magic    = BAM_DMUX_HEADER_MAGIC;
  hdr->cmd_opts = cmd_opts;
  hdr->cmd      = cmd;
  hdr->pad      = pad_len;
  hdr->channel  = ch_id;
  hdr->len      = len;
}

/*=============================================================================
  FUNCTION  bam_dmux_bam_tx_buf_flush
=============================================================================*/
/**
 * Submits the TX(UL) buffers to BAM.
 * Should be called under channel lock
 *
 */
/*===========================================================================*/
static void bam_dmux_bam_tx_buf_flush(bam_dmux_channel_type *ch)
{
  bam_dmux_bufdesc_type *bufdesc;
  boolean status;
  
  while (NULL != (bufdesc = BAM_DMUX_LIST_HEAD(ch->tx_bufdescs)))
  {
    status = bam_dmux_bam_tx_buf_submit(bufdesc->buf,
                                        bufdesc->buf_pa,
                                        bufdesc->len,
                                        bufdesc);
    
    /* As we got the free buffer descriptor already, submission should not fail */
    if (status == TRUE)
    {
      BAM_DMUX_LIST_REMOVE(ch->tx_bufdescs, bufdesc, link);
      BAM_DMUX_LINK_INIT(bufdesc->link);
    }
    else
    {
      BAM_DMUX_LOG_ERR(2, "TX FIFO full! channel_id=%d, ch=0x%08X.", ch->id, ch);
      break;
    }
  }
}

/*=============================================================================
  FUNCTION  bam_dmux_tx_submit
=============================================================================*/
/**
 * Submits the TX(UL) buffer to BAM.
 *
 */
/*===========================================================================*/
static boolean bam_dmux_tx_submit
(
  bam_dmux_channel_type *ch,
  bam_dmux_done_cb_type done_cb,
  void                  *buf,
  uint16                len,
  void                  *priv
)
{
  bam_dmux_bufdesc_type *bufdesc;
  bam_dmux_header_type  *hdr = (bam_dmux_header_type *)buf;
  boolean                power_status;

  BAM_DMUX_LOG_FUNC_START();

  bufdesc = bam_dmux_tx_bufdesc_get(&power_status);
  if (bufdesc == NULL)
  {
    BAM_DMUX_LOG_WRN(1, "TX bufdesc get failed, bufdescs are full channel_id=%d.", ch->id);
    return FALSE;
  }

  /* Fill buffer descriptor */
  bufdesc->ch      = ch;
  bufdesc->done_cb = done_cb;
  bufdesc->buf     = buf;
  bufdesc->buf_pa  = bam_dmux_os_va_to_pa(buf);
  bufdesc->len     = len;
  bufdesc->priv    = priv;
  bufdesc->status  = BAM_DMUX_STATUS_SUCESS;

  BAM_DMUX_LOG_DBG(5, "TX: cmd=%d, cmd_opts=0x%02X, ch_id=%d, pad=%d, len=%d.",
      hdr->cmd, hdr->cmd_opts, hdr->channel, hdr->pad, hdr->len);

  /* Add bufdesc to TX list */
  BAM_DMUX_LINK_INIT(bufdesc->link);
  BAM_DMUX_LIST_ADD(ch->tx_bufdescs, bufdesc, link);
  
  if (power_status == TRUE)
  {
    bam_dmux_bam_tx_buf_flush(ch);
  }

  BAM_DMUX_LOG_FUNC_END();

  return TRUE;
}

/*=============================================================================
  FUNCTION  bam_dmux_tx_cmd_done_cb
=============================================================================*/
/**
 * This function is called when a command (open/close) is transferred to A2.
 *
 */
/*===========================================================================*/
static void bam_dmux_tx_cmd_done_cb(bam_dmux_bufdesc_type *bufdesc)
{
  bam_dmux_channel_type    *ch;
  bam_dmux_header_type     *hdr;
  bam_dmux_event_notify_cb event_cb = NULL;
  bam_dmux_event           event = BAM_DMUX_EVENT_NONE;
  bam_dmux_event_data      event_data;
  boolean                  free_ch = FALSE;
  void                    *ch_priv = NULL;

  BAM_DMUX_LOG_FUNC_START();

  if (bufdesc == NULL)
  {
    BAM_DMUX_LOG_FATAL(1, "bufdesc corrupted! bufdesc=0x%08X", bufdesc);
  }

  ch = bufdesc->ch;
  hdr = (bam_dmux_header_type *)bufdesc->buf;

  /* Validate the bufdesc and cmd header */
  if ((ch           == NULL) ||
      (hdr          == NULL) ||
      (hdr->magic   != BAM_DMUX_HEADER_MAGIC) ||
      (hdr->channel != ch->id) ||
      ((hdr->cmd    != BAM_DMUX_CMD_OPEN) &&
       (hdr->cmd    != BAM_DMUX_CMD_OPEN_PC_DISABLED) &&
       (hdr->cmd    != BAM_DMUX_CMD_CLOSE)))
  {
    BAM_DMUX_LOG_FATAL(3, "bufdesc or cmd header corrupted! bufdesc=0x%08X"
        "ch=0x%08X, hdr=0x%08X,", bufdesc, ch, hdr);
  }

  BAM_DMUX_LOG_DBG(6, "TX cmd done: cmd=%d, cmd_opts=0x%02X, ch_id=%d, pad=%d, len=%d, status=%d",
      hdr->cmd, hdr->cmd_opts, hdr->channel, hdr->pad, hdr->len, bufdesc->status);

  if (bufdesc->status != BAM_DMUX_STATUS_SUCESS)
  {
    BAM_DMUX_LOG_ERR(7, "Bufdesc CMD TX failed! bufdesc=0x%08X, channel_id=%d, "
        "cmd=%d, buf_va=0x%08X, buf_pa=0x%08X, buf_len=%d, status=%d.", bufdesc,
        ch->id, hdr->cmd, bufdesc->buf, bufdesc->buf_pa, bufdesc->len, bufdesc->status);
  }

  event_cb = NULL;
  memset(&event_data, 0, sizeof(event_data));

  bam_dmux_os_mutex_lock(&ch->mutex);

  if (bufdesc->status != BAM_DMUX_STATUS_SUCESS)
  {
    bam_dmux_os_atomic_set_mask(&bam_dmux_tx_cmd_pending, 1 << ch->id);
    bam_dmux_tx_bufdesc_put(bufdesc);
    bam_dmux_os_mutex_unlock(&ch->mutex);
    return;
  }

  if ((ch->cmd_sent == NULL) || (ch->cmd_sent != hdr))
  {
    BAM_DMUX_LOG_ERR(3, "Cmd(0x%08X) sent is not same as current done cmd(0x%08X)!"
        " channel_id=%d.", ch->cmd_sent, hdr, ch->id);
    bam_dmux_tx_bufdesc_put(bufdesc);
    bam_dmux_os_mutex_unlock(&ch->mutex);
    return;
  }

  //when BAM_DMUX_STATUS_SUCESS
  ch->cmd_sent    = NULL;
  ch->cmd_pending = NULL;
  bam_dmux_os_atomic_clr_mask(&bam_dmux_tx_cmd_pending, 1 << ch->id);

  if ((hdr->cmd == BAM_DMUX_CMD_OPEN) ||
      (hdr->cmd == BAM_DMUX_CMD_OPEN_PC_DISABLED))
  {
    switch(ch->local_state)
    {
      case BAM_DMUX_CH_LOCAL_STATE_OPENING:
        ch->local_state = BAM_DMUX_CH_LOCAL_STATE_OPENED;
        if (ch->remote_state == BAM_DMUX_CH_REMOTE_STATE_OPENED)
        {
          event_cb = ch->event_cb;
          ch_priv  = ch->priv;
          event    = BAM_DMUX_EVENT_OPEN;
          event_data.open.remote_max_rx_buf_size = ch->remote_max_rx_buf_size;
        }
        break;

      case BAM_DMUX_CH_LOCAL_STATE_CLOSING:
        bam_dmux_tx_cmd(ch, BAM_DMUX_CMD_CLOSE, 0);
        break;

      default:
        BAM_DMUX_LOG_FATAL(3, "channel local state is invalid! ch=0x%08X, "
          "state=%d, cmd=%d.", ch, ch->local_state, hdr->cmd);
        break;
    }
  }
  else if (hdr->cmd == BAM_DMUX_CMD_CLOSE)
  {
    switch(ch->local_state)
    {
      case BAM_DMUX_CH_LOCAL_STATE_CLOSING:
        ch->local_state = BAM_DMUX_CH_LOCAL_STATE_CLOSED;

        event_cb = ch->event_cb;
        ch_priv  = ch->priv;
        event    = BAM_DMUX_EVENT_CLOSE_COMPLETE;

        ch->priv       = NULL;
        ch->event_cb   = NULL;
        ch->tx_done_cb = NULL;
        ch->rx_cb      = NULL;

        /* Remove the channel from channel table, free the channel after
         * notifying to client */
        bam_dmux_ch_set(ch->id, NULL);

        free_ch = TRUE;
        break;

      default:
        BAM_DMUX_LOG_FATAL(3, "channel local state is invalid! ch=0x%08X, "
          "state=%d, cmd=%d.", ch, ch->local_state, hdr->cmd);
        break;
    }
  }

  bam_dmux_os_mutex_unlock(&ch->mutex);

  /* Free the header and bufdesc */
  bam_dmux_os_free(hdr);
  bam_dmux_tx_bufdesc_put(bufdesc);

  /* Notify the client about the open/close event */
  if (event_cb)
  {
    event_cb(ch, ch_priv, event, &event_data);
  }
  if (free_ch == TRUE)
  {
    bam_dmux_ch_free(ch);
  }

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_tx_cmd
=============================================================================*/
/**
 * Sends a command(open/close) to A2
 *
 */
/*===========================================================================*/
static void bam_dmux_tx_cmd
(
  bam_dmux_channel_type *ch,
  uint8            cmd,
  uint8            cmd_opts
)
{
  bam_dmux_header_type *hdr;
  uint16               len = BAM_DMUX_HEADER_SIZE;

  BAM_DMUX_LOG_FUNC_START();

  hdr = bam_dmux_os_calloc_assert(1, len);
  bam_dmux_header_fill(hdr, ch->id, cmd, cmd_opts, 0, 0);

  ch->cmd_sent = hdr;
  if (!bam_dmux_tx_submit(ch, bam_dmux_tx_cmd_done_cb, hdr, len, NULL))
  {
    ch->cmd_pending = hdr;
    ch->cmd_sent    = NULL;
    bam_dmux_os_atomic_set_mask(&bam_dmux_tx_cmd_pending, 1 << ch->id);
  }
  else
  {
    ch->cmd_pending = NULL;
  }

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_tx_data_done_cb
=============================================================================*/
/**
 * This function is called when a TX(UL) data is transferred to A2.
 * Calls client TX done callback function to return the buffer to client.
 *
 */
/*===========================================================================*/
static void bam_dmux_tx_data_done_cb(bam_dmux_bufdesc_type *bufdesc)
{
  bam_dmux_channel_type      *ch;
  bam_dmux_tx_done_notify_cb tx_done_cb;
  void                       *ch_priv;
  void                       *tx_priv;
  void                       *data;
  uint16                     data_len;
  bam_dmux_status            status;

  BAM_DMUX_LOG_FUNC_START();

  if ((bufdesc == NULL) || (bufdesc->ch == NULL))
  {
    BAM_DMUX_LOG_FATAL(1, "bufdesc corrupted! bufdesc=0x%08X", bufdesc);
  }
  
  ch       = bufdesc->ch;
  data     = bufdesc->buf;
  data_len = bufdesc->len;
  tx_priv  = bufdesc->priv;
  status   = bufdesc->status;

  if (status != BAM_DMUX_STATUS_SUCESS)
  {
    BAM_DMUX_LOG_ERR(7, "TX failed! bufdesc=0x%08X, channel_id=%d, "
        "buf_va=0x%08X, buf_pa=0x%08X, buf_len=%d, status=%d.", bufdesc,
        ch->id, bufdesc->buf, bufdesc->buf_pa, data_len, status);
  }

  bam_dmux_os_mutex_lock(&ch->mutex);

  ch_priv    = ch->priv;
  tx_done_cb = ch->tx_done_cb;
  
  ch->stat_tx_curr_cnt--;

  if (status != BAM_DMUX_STATUS_SUCESS)
  {
    ch->notify_tx_resume = TRUE;
    bam_dmux_os_atomic_set_mask(&bam_dmux_tx_data_pending, 1 << ch->id);
  }

  bam_dmux_os_mutex_unlock(&ch->mutex);

  /* Free the buffer descriptor  */
  bam_dmux_tx_bufdesc_put(bufdesc);
  
  /* Notify the client about tx done */
  if (tx_done_cb)
  {
    tx_done_cb(ch, ch_priv, tx_priv, data, data_len, status);
  }

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_tx_data
=============================================================================*/
/**
 * Sends a TX(UL) data to A2
 *
 */
/*===========================================================================*/
static boolean bam_dmux_tx_data
(
  bam_dmux_channel_type *ch,
  void                  *data,
  uint16                data_len,
  void                  *priv
)
{
  bam_dmux_header_type *hdr = (bam_dmux_header_type*)data;
  uint16               len = BAM_DMUX_HEADER_SIZE + data_len;

  BAM_DMUX_LOG_FUNC_START();

  bam_dmux_header_fill(hdr, ch->id, BAM_DMUX_CMD_DATA, 0, data_len, 0);

  if (!bam_dmux_tx_submit(ch, bam_dmux_tx_data_done_cb, hdr, len, priv))
  {
    ch->notify_tx_resume = TRUE;
    bam_dmux_os_atomic_set_mask(&bam_dmux_tx_data_pending, 1 << ch->id);
    return FALSE;
  }
  else
  {
    ch->notify_tx_resume = FALSE;
    bam_dmux_os_atomic_clr_mask(&bam_dmux_tx_data_pending, 1 << ch->id);
    
    ch->stat_tx_curr_cnt++;
    ch->stat_tx_total_cnt++;
  }

  BAM_DMUX_LOG_FUNC_END();
  return TRUE;
}

/*=============================================================================
  FUNCTION  bam_dmux_open_cmd_opts
=============================================================================*/
/**
 * Prepares the open command options.
 *
 */
/*===========================================================================*/
static uint8 bam_dmux_open_cmd_opts(bam_dmux_channel_type *ch)
{
  uint8 cmd_opts = 0;
  uint8 cmd_opts_rx_buf_cnt;
  uint8 cmd_opts_rx_buf_size;


  BAM_DMUX_LOG_FUNC_START();

  /* Check for MTU negotiation support */
  if (ch->mtu_negotiation == FALSE)
  {
    return cmd_opts;
  }

  /* Max RX buffers */
  if (BAM_DMUX_CFG_MAX_RX_BUFS >= 256)
  {
    cmd_opts_rx_buf_cnt = BAM_DMUX_CMD_OPT_RX_BUF_CNT_256;
  }
  else if (BAM_DMUX_CFG_MAX_RX_BUFS >= 128)
  {
    cmd_opts_rx_buf_cnt = BAM_DMUX_CMD_OPT_RX_BUF_CNT_128;
  }
  else if (BAM_DMUX_CFG_MAX_RX_BUFS >= 64)
  {
    cmd_opts_rx_buf_cnt = BAM_DMUX_CMD_OPT_RX_BUF_CNT_64;
  }
  else
  {
    cmd_opts_rx_buf_cnt = BAM_DMUX_CMD_OPT_RX_BUF_CNT_32;
  }

  /* RX buffer size */
  if (BAM_DMUX_CFG_RX_BUF_SIZE >= 0x4000)
  {
    cmd_opts_rx_buf_size = BAM_DMUX_CMD_OPT_RX_MTU_SIZE_16K;
  }
  else if (BAM_DMUX_CFG_RX_BUF_SIZE >= 0x2000)
  {
    cmd_opts_rx_buf_size = BAM_DMUX_CMD_OPT_RX_MTU_SIZE_8K;
  }
  else if (BAM_DMUX_CFG_RX_BUF_SIZE >= 0x1000)
  {
    cmd_opts_rx_buf_size = BAM_DMUX_CMD_OPT_RX_MTU_SIZE_4K;
  }
  else
  {
    cmd_opts_rx_buf_size = BAM_DMUX_CMD_OPT_RX_MTU_SIZE_2K;
  }

  cmd_opts = cmd_opts_rx_buf_cnt  |
             cmd_opts_rx_buf_size |
             BAM_DMUX_CMD_OPT_MTU_NEGOTIATION;

  BAM_DMUX_LOG_FUNC_END();

  return cmd_opts;
}

/*=============================================================================
  FUNCTION  bam_dmux_tx_pending_cmds
=============================================================================*/
/**
 * Sends any pending commands to A2.
 *
 */
/*===========================================================================*/
static void bam_dmux_tx_pending_cmds(void)
{
  bam_dmux_channel_type *ch;
  uint8                 ch_id;


  BAM_DMUX_LOG_FUNC_START();

  for (ch_id = 0; ch_id < BAM_DMUX_CFG_MAX_CHANNELS; ch_id++)
  {
    ch = bam_dmux_ch_get(ch_id);
    if (ch == NULL)
    {
      continue;
    }

    bam_dmux_os_mutex_lock(&ch->mutex);
    if (ch->cmd_pending != NULL)
    {
      ch->cmd_sent = ch->cmd_pending;
      if (!bam_dmux_tx_submit(ch,
                              bam_dmux_tx_cmd_done_cb,
                              ch->cmd_pending,
                              BAM_DMUX_HEADER_SIZE,
                              NULL))
      {
        ch->cmd_sent = NULL;
        bam_dmux_os_atomic_set_mask(&bam_dmux_tx_cmd_pending, 1 << ch->id);
        bam_dmux_os_mutex_unlock(&ch->mutex);
        break;
      }
      else
      {
        ch->cmd_pending = NULL;
      }
    }
    bam_dmux_os_mutex_unlock(&ch->mutex);
  }

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_notify_clients_tx_resume
=============================================================================*/
/**
 * Notifies all clients about TX resume event.
 *
 */
/*===========================================================================*/
static void bam_dmux_notify_clients_tx_resume(void)
{
  bam_dmux_channel_type    *ch;
  bam_dmux_event_notify_cb event_cb = NULL;
  void                     *ch_priv = NULL;
  uint8                    ch_id;

  BAM_DMUX_LOG_FUNC_START();

  /* Notify TX resume to the clients who are waiting for it */
  for (ch_id = 0; ch_id < BAM_DMUX_CFG_MAX_CHANNELS; ch_id++)
  {
    ch = bam_dmux_ch_get(ch_id);
    if (ch == NULL)
    {
      continue;
    }

    bam_dmux_os_mutex_lock(&ch->mutex);
    if (ch->notify_tx_resume == TRUE)
    {
      ch->notify_tx_resume = FALSE;

      if ((ch->local_state  == BAM_DMUX_CH_LOCAL_STATE_OPENED) &&
          (ch->remote_state == BAM_DMUX_CH_REMOTE_STATE_OPENED))
      {
        event_cb = ch->event_cb;
        ch_priv  = ch->priv;
      }
      else
      {
        event_cb = NULL;
      }
    }

    bam_dmux_os_mutex_unlock(&ch->mutex);

    if (event_cb)
    {
      event_cb(ch, ch_priv, BAM_DMUX_EVENT_TX_RESUME, NULL);
    }
  }

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_handle_open_cmd
=============================================================================*/
/**
 * Handles the open command from A2 and notifies the open event to client.
 *
 */
/*===========================================================================*/
static void bam_dmux_handle_open_cmd(bam_dmux_header_type *hdr)
{
  bam_dmux_channel_type *ch;
  uint8                 ch_id = hdr->channel;

  BAM_DMUX_LOG_FUNC_START();

  /* Access bam_dmux_channels[] under common mutex protection */
  bam_dmux_os_mutex_lock(&bam_dmux_common_mutex);

  /* Remote side opened first */
  if (bam_dmux_channels[ch_id] == NULL)
  {
    ch = bam_dmux_ch_alloc(ch_id);

    bam_dmux_channels[ch_id] = ch;
  }
  else
  {
    ch = bam_dmux_channels[ch_id];
  }

  bam_dmux_os_mutex_unlock(&bam_dmux_common_mutex);


  bam_dmux_os_mutex_lock(&ch->mutex);

  if (ch->remote_state == BAM_DMUX_CH_REMOTE_STATE_OPENED)
  {
    BAM_DMUX_LOG_FATAL(2, "Channel is already opened from remote!  ch_id=%d, "
        "ch=0x%08X.", ch_id, ch);
  }

  ch->remote_state = BAM_DMUX_CH_REMOTE_STATE_OPENED;

  if (hdr->cmd == BAM_DMUX_CMD_OPEN)
  {
    ch->power_collapse = TRUE;
  }
  else
  {
    ch->power_collapse = FALSE;
    bam_dmux_os_mutex_lock(&bam_dmux_common_mutex);
    bam_dmux_power_collapse_en = FALSE;
    bam_dmux_os_mutex_unlock(&bam_dmux_common_mutex);
  }

  if (hdr->cmd_opts & BAM_DMUX_CMD_OPT_MTU_NEGOTIATION)
  {
    ch->mtu_negotiation = TRUE;

    switch (hdr->cmd_opts & BAM_DMUX_CMD_OPT_RX_MTU_SIZE_MASK)
    {
      case BAM_DMUX_CMD_OPT_RX_MTU_SIZE_16K:
        ch->remote_max_rx_buf_size = 0x4000;
        break;

      case BAM_DMUX_CMD_OPT_RX_MTU_SIZE_8K:
        ch->remote_max_rx_buf_size = 0x2000;
        break;

      case BAM_DMUX_CMD_OPT_RX_MTU_SIZE_4K:
        ch->remote_max_rx_buf_size = 0x1000;
        break;

      case BAM_DMUX_CMD_OPT_RX_MTU_SIZE_2K:
      default:
        ch->remote_max_rx_buf_size = 0x800;
        break;
    }
  }

  /* Send open command to remote only when local side is also opening */
  if (ch->local_state == BAM_DMUX_CH_LOCAL_STATE_OPENING)
  {
     bam_dmux_tx_cmd(ch, hdr->cmd, bam_dmux_open_cmd_opts(ch));
  }

  bam_dmux_os_mutex_unlock(&ch->mutex);

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_handle_close_cmd
=============================================================================*/
/**
 * Handles the close command from A2 and notifies the remote close or close
 * complete event to client.
 *
 */
/*===========================================================================*/
static void bam_dmux_handle_close_cmd(bam_dmux_header_type *hdr)
{
  bam_dmux_channel_type    *ch;
  bam_dmux_event_notify_cb event_cb = NULL;
  bam_dmux_event           event = BAM_DMUX_EVENT_NONE;
  uint8                    ch_id = hdr->channel;
  void                     *ch_priv = NULL;

  BAM_DMUX_LOG_FUNC_START();

  ch = bam_dmux_ch_get(ch_id);
  if (ch == NULL)
  {
    BAM_DMUX_LOG_FATAL(2, "Channel is already closed from remote!  ch_id=%d, "
          "bam_dmux_channels[ch_id]=0x%08X.", ch_id, bam_dmux_channels[ch_id]);
    return;
  }

  bam_dmux_os_mutex_lock(&ch->mutex);
  if (ch->remote_state == BAM_DMUX_CH_REMOTE_STATE_CLOSED)
  {
    BAM_DMUX_LOG_FATAL(2, "Channel is already closed from remote!  ch_id=%d, "
        "ch=0x%08X.", ch_id, ch);
  }

  ch->remote_state = BAM_DMUX_CH_REMOTE_STATE_CLOSED;

  switch(ch->local_state)
  {
    case BAM_DMUX_CH_LOCAL_STATE_CLOSED:
        event_cb = ch->event_cb;
        ch_priv  = ch->priv;
        event    = BAM_DMUX_EVENT_CLOSE_COMPLETE;

        ch->priv       = NULL;
        ch->event_cb   = NULL;
        ch->tx_done_cb = NULL;
        ch->rx_cb      = NULL;

        /* Remove the channel from channel table, free the channel after
         * notifying to client */
        bam_dmux_ch_set(ch_id, NULL);
        break;

    case BAM_DMUX_CH_LOCAL_STATE_OPENING:
      if (ch->cmd_pending)
      {
        /* Don't send open command if we already not sent */
        bam_dmux_os_free(ch->cmd_pending);
      }
      /* Don't break */
    default:
      event_cb = ch->event_cb;
      ch_priv  = ch->priv;
      event    = BAM_DMUX_EVENT_REMOTE_CLOSE;
      break;

  }

  bam_dmux_os_mutex_unlock(&ch->mutex);

  /* Notify the client about the open/close event */
  if (event_cb)
  {
    event_cb(ch, ch_priv, event, NULL);
  }
  if (event == BAM_DMUX_EVENT_CLOSE_COMPLETE)
  {
    bam_dmux_ch_free(ch);
  }

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_handle_data_cmd
=============================================================================*/
/**
 * Handles the RX(DL) data from A2 and calls client's RX callback .
 *
 */
/*===========================================================================*/
static void bam_dmux_handle_data_cmd
(
  bam_dmux_bufdesc_type *bufdesc,
  uint16                rx_len
)
{
  bam_dmux_header_type  *hdr  = (bam_dmux_header_type *)bufdesc->buf;
  uint8                 ch_id = hdr->channel;
  bam_dmux_channel_type *ch;
  bam_dmux_rx_notify_cb rx_cb = NULL;
  void                  *ch_priv = NULL;

  BAM_DMUX_LOG_FUNC_START();

  if (rx_len < BAM_DMUX_HEADER_SIZE)
  {
    BAM_DMUX_LOG_ERR(4, "RX data len is not invalid! ch_id=%d bufdesc=0x%08X, "
        "cmd_len=%d, rx_len=%d ", ch_id, bufdesc, hdr->len, rx_len);

    /* Resubmit buffer descriptor */
    bam_dmux_rx_bufdesc_submit(bufdesc);
    return;
  }

  if (hdr->len == 0xFFFF)
  {
    /* This is special handling for QMAP over DMUX: if len in DMUX header set
       to 0xFFFF by A2, then this needs to be filled by no_of_bytes param,
       which is set by BAM driver for complete byte transfer.
       We need to reduce DMUX header size from no_of_bytes and set the DMUX
       header len as packet length.*/
    hdr->len = rx_len - BAM_DMUX_HEADER_SIZE;
  }

  if ((hdr->len == 0) ||
      ((hdr->len + BAM_DMUX_HEADER_SIZE) > rx_len))
  {
    BAM_DMUX_LOG_ERR(4, "Data cmd len is not invalid! ch_id=%d bufdesc=0x%08X, "
        "cmd_len=%d, rx_len=%d ", ch_id, bufdesc, hdr->len, rx_len);

    /* Resubmit buffer descriptor */
    bam_dmux_rx_bufdesc_submit(bufdesc);
    return;
  }

  ch = bam_dmux_ch_get(ch_id);
  if (ch)
  {
    bam_dmux_os_mutex_lock(&ch->mutex);

    if ((ch->local_state  != BAM_DMUX_CH_LOCAL_STATE_OPENED) ||
        (ch->remote_state != BAM_DMUX_CH_REMOTE_STATE_OPENED))
    {
      BAM_DMUX_LOG_ERR(4, "Channel is not opened completely! ch_id=%d, ch=0x%08X, "
        "local_state = %d, remote_state=%d.", ch_id, ch, ch->local_state, ch->remote_state);
    }
    else
    {
      bufdesc->ch = ch;
      rx_cb   = ch->rx_cb;
      ch_priv = ch->priv;
      
      ch->stat_rx_curr_cnt++;
      ch->stat_rx_total_cnt++;
    }

    bam_dmux_os_mutex_unlock(&ch->mutex);
  }
  else
  {
    BAM_DMUX_LOG_ERR(2, "Channel is not opened!  ch_id=%d, "
            "bam_dmux_channels[ch_id]=0x%08X.", ch_id, bam_dmux_channels[ch_id]);
  }

  if (rx_cb)
  {
    rx_cb(ch, ch_priv, bufdesc, hdr + 1, hdr->len);
  }
  else
  {
    /* Droping DL data */
    bam_dmux_rx_bufdesc_submit(bufdesc);
  }

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_ch_tx_flush
=============================================================================*/
/**
 * Flushes any pending TX pending data or commands
 *
 */
/*===========================================================================*/
static void bam_dmux_ch_tx_flush(void)
{
  bam_dmux_channel_type *ch;
  uint8                 ch_id;

  /* Notify power state, flush any pending cmd/data */
  for (ch_id = 0; ch_id < BAM_DMUX_CFG_MAX_CHANNELS; ch_id++)
  {
    ch = bam_dmux_ch_get(ch_id);
    if (ch == NULL)
    {
      continue;
    }
    
    bam_dmux_os_mutex_lock(&ch->mutex);
    
    /* TX Flush */
    bam_dmux_bam_tx_buf_flush(ch);
    
    /* Pending cmd flush */
    if ((bam_dmux_os_atomic_get_mask(&bam_dmux_tx_cmd_pending) & (1 << ch->id)) &&
        (ch->cmd_pending))
    {
      ch->cmd_sent = ch->cmd_pending;
      if (!bam_dmux_tx_submit(ch,
                              bam_dmux_tx_cmd_done_cb,
                              ch->cmd_pending,
                              BAM_DMUX_HEADER_SIZE,
                              NULL))
      {
        ch->cmd_sent = NULL;
        bam_dmux_os_atomic_set_mask(&bam_dmux_tx_cmd_pending, 1 << ch->id);
      }
      else
      {
        ch->cmd_pending = NULL;
      }
    }
    
    bam_dmux_os_mutex_unlock(&ch->mutex);
  }
}

/*=============================================================================
  FUNCTION  bam_dmux_notify_power_state
=============================================================================*/
/**
 * Notifies the power event to a client.
 *
 */
/*===========================================================================*/
static void bam_dmux_notify_power_state(bam_dmux_channel_type *ch, boolean state)
{
  bam_dmux_event_notify_cb event_cb = NULL;
  bam_dmux_event_data      event_data;
  void                     *ch_priv;

  BAM_DMUX_LOG_FUNC_START();

  memset(&event_data, 0, sizeof(event_data));

  bam_dmux_os_mutex_lock(&ch->mutex);

  if ((ch->local_state == BAM_DMUX_CH_LOCAL_STATE_OPENING) ||
      (ch->local_state == BAM_DMUX_CH_LOCAL_STATE_OPENED))
  {
    event_cb = ch->event_cb;
    ch_priv  = ch->priv;
    
    event_data.power.state = state;
  }

  bam_dmux_os_mutex_unlock(&ch->mutex);

  if (event_cb)
  {
    event_cb(ch, ch_priv, BAM_DMUX_EVENT_POWER, &event_data);
  }

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_ch_notify_power_state
=============================================================================*/
/**
 * Notifies the power event to all the clients.
 *
 */
/*===========================================================================*/
static void bam_dmux_ch_notify_power_state(boolean state)
{
  bam_dmux_channel_type *ch;
  uint8                  ch_id;

  BAM_DMUX_LOG_FUNC_START();

  /* Notify TX resume to the clients who are waiting for it */
  for (ch_id = 0; ch_id < BAM_DMUX_CFG_MAX_CHANNELS; ch_id++)
  {
    ch = bam_dmux_ch_get(ch_id);
    if (ch == NULL)
    {
      continue;
    }

    bam_dmux_notify_power_state(ch, state);
  }

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_enable_bam
=============================================================================*/
/**
 * Initialize the BAM and Pipes if it is first
 * time power on otherwise reset the BAM and enables the Pipes.
 * Submits the RX buffers to BAM to receive commands and DL data from A2.
 */
/*===========================================================================*/
static void bam_dmux_enable_bam(void)
{
  bam_dmux_bufdesc_type *bufdesc;
  bam_dmux_channel_type *ch;
  uint8                 ch_id;
  boolean               status;
  uint16                idx;

  BAM_DMUX_LOG_FUNC_START();

  bam_dmux_bam_state = TRUE;

  if (bam_dmux_bam_init_status == FALSE)
  {
    bam_dmux_bam_init_status = TRUE;

    /* Initialize BAM on first time power on */
    bam_dmux_bam_init();
    bam_dmux_bam_pipes_init();
    bam_dmux_bam_pipes_enable();

    /* Allocate te RX buffers for fisrt timeinit */
    for (idx = 0; idx < BAM_DMUX_CFG_MAX_RX_BUFS; idx++)
    {
      uint32 aligned = (uint32)(uintptr_t)bam_dmux_os_calloc_assert(1, 
                            BAM_DMUX_CFG_RX_BUF_SIZE + 
                            BAM_DMUX_CFG_CACHE_LINE_SIZE * 2);

      aligned = (aligned + BAM_DMUX_CFG_CACHE_LINE_SIZE - 1);
      aligned &= ~(BAM_DMUX_CFG_CACHE_LINE_SIZE - 1);
     
      bufdesc = &bam_dmux_rx_bufdescs[idx]; 
      /* Fill the RX buffer descriptor,
       * these values fix, they are not going to update later */
      bufdesc->buf = (void*)(uintptr_t)aligned;
      bufdesc->len = BAM_DMUX_CFG_RX_BUF_SIZE;
      bufdesc->buf_pa = bam_dmux_os_va_to_pa(bufdesc->buf);
    }
  }
  else
  {
    bam_dmux_bam_reset();
    bam_dmux_bam_pipes_enable();
  }

  /* Submit RX buffers */
  BAM_DMUX_LIST_INIT(bam_dmux_submited_rx_bufdescs);

  for (idx = 0; idx < BAM_DMUX_CFG_MAX_RX_BUFS; idx++)
  {
    bufdesc = &bam_dmux_rx_bufdescs[idx];
    if (bufdesc->ch)
    {
      BAM_DMUX_LOG_WRN(3, "Client not yet did rx_done! bufdesc=0x%08X, ch=0x%08X"
            "ch_id=%d.", bufdesc, bufdesc->ch, bufdesc->ch->id);
      continue;
    }

    /* Fill the RX buffer descriptor,
     * these values fix, they are not going to update later */
    bufdesc->priv   = bufdesc;
    bufdesc->status = BAM_DMUX_STATUS_SUCESS;

    memset(bufdesc->buf, 'O', BAM_DMUX_HEADER_SIZE);

    /* Add the buf desc to submited list first */
    BAM_DMUX_LINK_INIT(bufdesc->link);
    BAM_DMUX_LIST_ADD(bam_dmux_submited_rx_bufdescs, bufdesc, link);

    status = bam_dmux_bam_rx_buf_submit(bufdesc->buf,
                                        bufdesc->buf_pa,
                                        bufdesc->len,
                                        bufdesc);
    /* Submission should not fail */
    if (status == FALSE)
    {
      BAM_DMUX_LOG_FATAL(4, "RX Bufdesc submit failed! bufdesc=0x%08X,"
          "buf_va=0x%08X, buf_pa=0x%08X, buf_len=%d.", bufdesc,
          bufdesc->buf, bufdesc->buf_pa, bufdesc->len);
    }
  }
  
  bam_dmux_check_and_start_inactivity_timer();

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_disable_bam
=============================================================================*/
/**
 * Disables the BAM Pipes and reset the BAM.
 *
 */
/*===========================================================================*/
static void bam_dmux_disable_bam(void)
{
  BAM_DMUX_LOG_FUNC_START();

  bam_dmux_bam_state = FALSE;

  if (bam_dmux_bam_init_status == TRUE)
  {
    bam_dmux_bam_pipes_disable();
    bam_dmux_bam_reset();
  }

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================

                     PUBLIC FUNCTION DEFINATION

=============================================================================*/

/*=============================================================================
  FUNCTION  bam_dmux_init
=============================================================================*/
/**
 * This is the BAM DMUX initialization function.
 *
 */
/*===========================================================================*/
void bam_dmux_init(void)
{
  /* Initialize the log first */
  bam_dmux_log_init();

  BAM_DMUX_LOG_FUNC_START();

  bam_dmux_os_init();

  /* initialize states */
  bam_dmux_bam_init_status = FALSE;
  bam_dmux_bam_state       = FALSE;
  
  bam_dmux_local_power_state  = BAM_DMUX_LOCAL_POWER_UNVOTED;
  bam_dmux_remote_power_state = BAM_DMUX_REMOTE_POWER_OFF;
  
  /* Common mutex for protecting common resources */
  bam_dmux_os_mutex_init(&bam_dmux_common_mutex);

  /* Initialize Tx inactivity timer */
  bam_dmux_os_timer_init(&bam_dmux_tx_inactivity_timer,
                         bam_dmux_tx_inactivity_timer_cb,
                         NULL);
  
  /* Initialize buffer descriptors */
  bam_dmux_bufdescs_init();

  /* Initialize channels array */
  memset(bam_dmux_channels, 0, sizeof(bam_dmux_channels));

  /* Enable power collapse by default, later update based on remote interest */
  bam_dmux_power_collapse_en = TRUE;

  /* Initialize the power, initially remote(A2) starts power request */
  bam_dmux_power_init();

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_open
=============================================================================*/
/**
 * Opens a logical channel.
 *
 * @param[in]  channel_id    The logical channel id
 * @param[in]  priv          Private data for client to maintain context
 *                           This data is passed back to client in the
 *                           notification callbacks
 * @param[in]  options       Channel open options (see BAM_DMUX_OPEN_OPTIONS*)
 * @param[in]  event_cb      Client notification callback function, which will
                             be called to notify about the channel events
                             (see bam_dmux_event enumeration)
 * @param[in]  tx_done_cb    Client notification callback function, which will
                             be called when TX transfer is completed.
 * @param[in]  rx_cb         Client notification callback function, which will
                             be called when there is an incoming (RX) data
                             from remote
 * @param[out] handle        The handle to be used to interact with this
 *                           channel on a successful open
 *
 * @return
 * Sets the handle and returns BAM_DMUX_STATUS_SUCESS if function is successful,
 * Otherwise returns error code (see bam_dmux_status enumeration).
 *
 * @note
 */
/*===========================================================================*/
bam_dmux_status bam_dmux_open
(
  uint8                      channel_id,
  void                       *priv,
  bam_dmux_open_options      *options,
  bam_dmux_event_notify_cb   event_cb,
  bam_dmux_tx_done_notify_cb tx_done_cb,
  bam_dmux_rx_notify_cb      rx_cb,
  bam_dmux_handle            *handle
)
{
  bam_dmux_channel_type *ch;
  boolean                power_state;

  BAM_DMUX_LOG_FUNC_START();

  /* Validate the arguments */
  if ((channel_id >= BAM_DMUX_CFG_MAX_CHANNELS) ||
      (event_cb   == NULL) ||
      (tx_done_cb == NULL) ||
      (rx_cb      == NULL) ||
      (handle     == NULL))
  {
    BAM_DMUX_LOG_ERR(5, "Invalid arguments! channel_id=%d, event_cb=0x%08X, "
        "tx_done_cb=0x%08X, rx_cb=0x%08X, handle=0x%08X.", channel_id,
        event_cb, tx_done_cb, rx_cb, handle);

    return BAM_DMUX_STATUS_INVALID_ARGS;
  }

  BAM_DMUX_LOG_INFO(6, "Channel is opening! channel_id=%d, priv=0x%08X, event_cb=0x%08X, "
        "tx_done_cb=0x%08X, rx_cb=0x%08X, handle=0x%08X.", channel_id, priv,
        event_cb, tx_done_cb, rx_cb, handle);

  /* Access bam_dmux_channels[] under common mutex protection */
  bam_dmux_os_mutex_lock(&bam_dmux_common_mutex);

  /* Local side opened first */
  if (bam_dmux_channels[channel_id] == NULL)
  {
    ch = bam_dmux_ch_alloc(channel_id);

    bam_dmux_channels[channel_id] = ch;
  }
  else
  {
    ch = bam_dmux_channels[channel_id];
  }

  bam_dmux_os_mutex_unlock(&bam_dmux_common_mutex);

  bam_dmux_os_mutex_lock(&ch->mutex);

  if (ch->local_state != BAM_DMUX_CH_LOCAL_STATE_CLOSED)
  {
    BAM_DMUX_LOG_ERR(2, "Channel is already opened! channel_id=%d, "
        "ch=0x%08X.", channel_id, ch);

    bam_dmux_os_mutex_unlock(&ch->mutex);
    return BAM_DMUX_STATUS_INVALID_CH_STATE;
  }

  *handle = ch;

  ch->priv       = priv;
  ch->event_cb   = event_cb;
  ch->tx_done_cb = tx_done_cb;
  ch->rx_cb      = rx_cb;

  ch->local_state = BAM_DMUX_CH_LOCAL_STATE_OPENING;

  /* Send open command to remote only when remote side is opened */
  if (ch->remote_state == BAM_DMUX_CH_REMOTE_STATE_OPENED)
  {
    bam_dmux_tx_cmd(ch,
                    ch->power_collapse ? BAM_DMUX_CMD_OPEN :
                        BAM_DMUX_CMD_OPEN_PC_DISABLED,
                    bam_dmux_open_cmd_opts(ch));
  }

  bam_dmux_os_mutex_unlock(&ch->mutex);

  /* Notify the power on state, if power is already on */
  bam_dmux_os_mutex_lock(&bam_dmux_common_mutex);
  
  if ((bam_dmux_local_power_state  == BAM_DMUX_LOCAL_POWER_VOTED) &&
      (bam_dmux_remote_power_state == BAM_DMUX_REMOTE_POWER_ON))
  {
    power_state = TRUE;
  }
  else
  {
    power_state = FALSE;
  }
  bam_dmux_os_mutex_unlock(&bam_dmux_common_mutex);
  
  /* Notify the power on state, if power is already on */
  if (power_state)
  {
    bam_dmux_notify_power_state(ch, power_state);
  }

  BAM_DMUX_LOG_FUNC_END();

  return BAM_DMUX_STATUS_SUCESS;
}

/*=============================================================================
  FUNCTION  bam_dmux_tx
=============================================================================*/
/**
 * Sends (UL) data to remote. Client should allocate and leave space for
 * BAM DMUX header (of size BAM_DMUX_HEADER_SIZE) at the start of the data
 * buffer. Which means the total size of data buffer should be equal to actual
 * payload size plus BAM DMUX header size (BAM_DMUX_HEADER_SIZE).
 *
 * Clients should set BAM_DMUX_HEADER_CLIENT_SIGNATURE at the start of the
 * data buffer (which is start of BAM DMUX header). This is only for integrity
 * check to ensure client has reserved the space for BAM DMUX header.
 * BAM DMUX will check this signature before start sending the data.
 *
 * Client TX data buffer size (including BAM DMUX header) should not exceed
 * the remote max RX buffer size. BAM DMUX shares/notify the max remote RX
 * buffer size with BAM_DMUX_EVENT_OPEN event data (see bam_dmux_event_data union)
 * to clients.
 *
 * |-----------------|---------------------------------------------|
 * | BAM DMUX header | Client TX data                              |
 * |-----------------|---------------------------------------------|
 *
 *
 * @param[in]  handle    Channel handle returned in the bam_dmux_open()
 * @param[in]  tx_priv   Client private data for this TX transfer
 * @param[in]  data      TX data pointer (starting with BAM DMUX header)
 * @param[in]  data_len  TX data length (excluding BAM DMUX header length)
 *
 * @return
 * BAM_DMUX_STATUS_SUCESS if function is successful,
 * Otherwise returns error code (see bam_dmux_status enumeration).
 *
 * @note
 * BAM DMUX calls bam_dmux_tx_done_notify_cb() notification callback once
 * data is transferred to remote.
 *
 * When this function is fails with BAM_DMUX_STATUS_TX_ERROR error code,
 * BAM DMUX calls bam_dmux_event_notify_cb() callback function with
 * BAM_DMUX_EVENT_TX_RESUME event when further bam_dmux_tx() are allowed.
 */
/*===========================================================================*/
bam_dmux_status bam_dmux_tx
(
  bam_dmux_handle handle,
  void            *tx_priv,
  void            *data,
  uint16          data_len
)
{
  bam_dmux_channel_type *ch = handle;
  bam_dmux_status       ret;
  uint32                resv_sig;

  BAM_DMUX_LOG_FUNC_START();

  /* Validate the arguments */
  if ((ch           == NULL) ||
      (data         == NULL) ||
      (data_len     == 0))
  {
    BAM_DMUX_LOG_ERR(3, "Invalid arguments! ch=0x%08X, data=0x%08X, "
        "data_len=%d.", ch, data, data_len);

    return BAM_DMUX_STATUS_INVALID_ARGS;
  }
  resv_sig = *((uint32*)data);
  if (resv_sig != BAM_DMUX_HEADER_CLIENT_SIGNATURE)
  {
    BAM_DMUX_LOG_ERR(2, "No headroom is reserverd! resv_sig=0x%08X, expected_sig=0x%08X.",
        resv_sig, BAM_DMUX_HEADER_CLIENT_SIGNATURE);

    return BAM_DMUX_STATUS_TX_NO_HEADROOM;
  }

  bam_dmux_os_mutex_lock(&ch->mutex);

  if ((ch->local_state  != BAM_DMUX_CH_LOCAL_STATE_OPENED) ||
      (ch->remote_state != BAM_DMUX_CH_REMOTE_STATE_OPENED))
  {
    BAM_DMUX_LOG_ERR(2, "Invalid channel state! local=%d, remote=%d.",
        ch->local_state, ch->remote_state);

    ret = BAM_DMUX_STATUS_INVALID_CH_STATE;
    goto UNLOCK_EXIT;
  }

  if (data_len > ch->remote_max_rx_buf_size)
  {
    BAM_DMUX_LOG_ERR(2, "Invalid data length! remote_max_rx_buf_size=%d, data_len=%d.",
        ch->remote_max_rx_buf_size, data_len);

    ret = BAM_DMUX_STATUS_INVALID_ARGS;
    goto UNLOCK_EXIT;
  }

  if (!bam_dmux_tx_data(ch, data, data_len, tx_priv))
  {
    ret = BAM_DMUX_STATUS_TX_ERROR;
  }
  else
  {
    ret = BAM_DMUX_STATUS_SUCESS;
  }

UNLOCK_EXIT:
  bam_dmux_os_mutex_unlock(&ch->mutex);

  BAM_DMUX_LOG_FUNC_END();

  return ret;
}

/*=============================================================================
  FUNCTION  bam_dmux_rx_done
=============================================================================*/
/**
 * Client uses this to indicate the BAM DMUX that it is done with the
 * received RX (DL) data (which was given to client in the bam_dmux_rx_notify_cb()
 * notification callback).
 *
 * @param[in]  handle    Channel handle returned in the bam_dmux_open()
 * @param[in]  rx_priv   BAM DMUX private data for the RX transfer
 *
 * @return
 * BAM_DMUX_STATUS_SUCESS if function is successful,
 * Otherwise returns error code (see bam_dmux_status enumeration).
 *
 * @note
 */
/*===========================================================================*/
bam_dmux_status bam_dmux_rx_done
(
  bam_dmux_handle handle,
  void            *rx_priv
)
{
  bam_dmux_channel_type *ch      = handle;
  bam_dmux_bufdesc_type *bufdesc = rx_priv;

  BAM_DMUX_LOG_FUNC_START();

  /* Validate the arguments */
  if ((ch           == NULL) ||
      (bufdesc      == NULL) ||
      (bufdesc->buf == NULL) ||
      (bufdesc->len == 0) ||
      (bufdesc->ch  != ch))
  {
    BAM_DMUX_LOG_ERR(2, "Invalid arguments! ch=0x%08X, bufdesc=0x%08X.", ch, bufdesc);
    return BAM_DMUX_STATUS_INVALID_ARGS;
  }
  
  ch->stat_rx_curr_cnt--;

  /* Resubmit buffer descriptor */
  bam_dmux_rx_bufdesc_submit(bufdesc);

  BAM_DMUX_LOG_FUNC_END();

  return BAM_DMUX_STATUS_SUCESS;
}

/*=============================================================================
  FUNCTION  bam_dmux_close
=============================================================================*/
/**
 * Closes an opened channel.
 *
 * After calling this function, clients must wait for
 * BAM_DMUX_EVENT_CLOSE_COMPLETE event before reopening the port.
 *
 * @param[in]  handle    Channel handle returned in the bam_dmux_open()
 *
 * @return
 * BAM_DMUX_STATUS_SUCESS if function is successful,
 * Otherwise returns error code (see bam_dmux_status enumeration).
 *
 * @note
 */
/*===========================================================================*/
bam_dmux_status bam_dmux_close
(
  bam_dmux_handle handle
)
{
  bam_dmux_channel_type    *ch = handle;
  bam_dmux_event_notify_cb event_cb = NULL;
  bam_dmux_event           event = BAM_DMUX_EVENT_NONE;
  void                     *ch_priv;

  BAM_DMUX_LOG_FUNC_START();

  /* Validate the arguments */
  if (ch == NULL)
  {
    BAM_DMUX_LOG_ERR(1, "Invalid arguments! ch=0x%08X.", ch);
    return BAM_DMUX_STATUS_INVALID_ARGS;
  }

  BAM_DMUX_LOG_INFO(3, "Channel is closing! ch_id=%d, local=%d, remote=%d.",
        ch->id, ch->local_state, ch->remote_state);

  bam_dmux_os_mutex_lock(&ch->mutex);
  if ((ch->local_state != BAM_DMUX_CH_LOCAL_STATE_OPENING) &&
      (ch->local_state != BAM_DMUX_CH_LOCAL_STATE_OPENED))
  {
    BAM_DMUX_LOG_ERR(3, "Invalid channel state! ch_id=%d, local=%d, remote=%d.",
        ch->id, ch->local_state, ch->remote_state);

    bam_dmux_os_mutex_unlock(&ch->mutex);
    return BAM_DMUX_STATUS_INVALID_CH_STATE;
  }

  if ((ch->local_state  == BAM_DMUX_CH_LOCAL_STATE_OPENING) &&
      (ch->remote_state == BAM_DMUX_CH_REMOTE_STATE_CLOSED) &&
      (ch->cmd_pending))
  {
    event_cb = ch->event_cb;
    ch_priv  = ch->priv;
    event    = BAM_DMUX_EVENT_CLOSE_COMPLETE;

    ch->priv       = NULL;
    ch->event_cb   = NULL;
    ch->tx_done_cb = NULL;
    ch->rx_cb      = NULL;

    /* Remove the channel from channel table, free the channel after
     * notifying to client */
    bam_dmux_ch_set(ch->id, NULL);
  }
  else
  {
    ch->local_state = BAM_DMUX_CH_LOCAL_STATE_CLOSING;

    bam_dmux_tx_cmd(ch, BAM_DMUX_CMD_CLOSE, 0);
  }

  bam_dmux_os_mutex_unlock(&ch->mutex);

  /* Notify the client about the open/close event */
  if (event_cb)
  {
    event_cb(ch, ch_priv, event, NULL);
  }
  if (event == BAM_DMUX_EVENT_CLOSE_COMPLETE)
  {
    bam_dmux_ch_free(ch);
  }

  BAM_DMUX_LOG_FUNC_END();

  return BAM_DMUX_STATUS_SUCESS;
}

/*=============================================================================
  FUNCTION  bam_dmux_power_notify
=============================================================================*/
/**
 * This function is called by the Power layer to Core layer to notify the
 * A2 Power state event.
 *
 * @param[in]  event      Power event.
 *
 */
/*===========================================================================*/
void bam_dmux_power_notify(bam_dmux_power_event_type event)
{
  boolean power_state = FALSE;
  boolean ch_notify   = FALSE;
  boolean tx_flush    = FALSE;
  
  bam_dmux_os_mutex_lock(&bam_dmux_common_mutex);

  BAM_DMUX_LOG_INFO(3, "Power enter event=%s, local_state=%s, remote_state=%s.",
                    bam_dmux_power_event_str(event), 
                    bam_dmux_local_power_state_str(bam_dmux_local_power_state),
                    bam_dmux_remote_power_state_str(bam_dmux_remote_power_state));

  if (event == BAM_DMUX_POWER_EVENT_FATAL)
  {
    BAM_DMUX_LOG_FATAL(1, "Power fatal event! event=%d.", event);
    bam_dmux_os_mutex_unlock(&bam_dmux_common_mutex);
    return;
  }
  
  /* Validate the power event */
  if (((event == BAM_DMUX_POWER_EVENT_OFF) &&
       (bam_dmux_remote_power_state == BAM_DMUX_REMOTE_POWER_OFF)) ||    /* Remote OFF */
      ((event == BAM_DMUX_POWER_EVENT_ON) &&
       (bam_dmux_remote_power_state == BAM_DMUX_REMOTE_POWER_ON)) ||     /* Remote ON */
      ((event == BAM_DMUX_POWER_EVENT_UNVOTED) &&
       (bam_dmux_local_power_state != BAM_DMUX_LOCAL_POWER_UNVOTING)) || /* LOcal UNVOTED */
      ((event == BAM_DMUX_POWER_EVENT_VOTED) &&
       (bam_dmux_local_power_state != BAM_DMUX_LOCAL_POWER_VOTING)))     /* LOcal VOTED */
  {
    BAM_DMUX_LOG_ERR(3, "Unexpected power event=%d, local_vote=%d, remote_power=%d.",
                      event, bam_dmux_local_power_state, bam_dmux_remote_power_state);
    bam_dmux_os_mutex_unlock(&bam_dmux_common_mutex);
    return;
  }

  /* Update the local and remote power/vote states */
  switch (event)
  {
    case BAM_DMUX_POWER_EVENT_OFF:
      bam_dmux_remote_power_state = BAM_DMUX_REMOTE_POWER_OFF;
      break;

    case BAM_DMUX_POWER_EVENT_ON:
      bam_dmux_remote_power_state = BAM_DMUX_REMOTE_POWER_ON;
      break;

    case BAM_DMUX_POWER_EVENT_VOTED:
      bam_dmux_local_power_state = BAM_DMUX_LOCAL_POWER_VOTED;
      break;

    case BAM_DMUX_POWER_EVENT_UNVOTED:
      bam_dmux_local_power_state = BAM_DMUX_LOCAL_POWER_UNVOTED;
      break;

    default:
      BAM_DMUX_LOG_ERR(1, "Unknown Power event! event=%d.", event);
      bam_dmux_os_mutex_unlock(&bam_dmux_common_mutex);
      return;
  }

  if (bam_dmux_remote_power_state == BAM_DMUX_REMOTE_POWER_ON)
  {
    if (bam_dmux_local_power_state == BAM_DMUX_LOCAL_POWER_VOTED)
    {
      /* Local and remote power on, enable the A2 BAM if it is not */
      if (bam_dmux_bam_state == FALSE)
      {
        bam_dmux_enable_bam();
        power_state = TRUE;
        ch_notify   = TRUE;
      }
      tx_flush = TRUE;
    }
    /* A2 has DL data, so vote for power. Later, unvote if there is no activity */
    else if ((event == BAM_DMUX_POWER_EVENT_ON) &&
             (bam_dmux_local_power_state == BAM_DMUX_LOCAL_POWER_UNVOTED))
    {
      /* A2 has DL data, so vote for power. Later, unvote if there is activity */
      bam_dmux_power_vote_helper(TRUE);
    }
  }
  else if (event == BAM_DMUX_POWER_EVENT_OFF)
  {
    /* Local and remote power off, disable the A2 BAM if it is not */
    if (bam_dmux_bam_state == TRUE)
    {
      bam_dmux_disable_bam();
      power_state = FALSE;
      ch_notify   = TRUE;
    }
    
    /* Vote for power if there is TX activity */
    if ((BAM_DMUX_LIST_CNT(bam_dmux_free_tx_bufdescs) != BAM_DMUX_CFG_MAX_TX_BUFS) &&
        (bam_dmux_local_power_state == BAM_DMUX_LOCAL_POWER_UNVOTED))
    {
      bam_dmux_power_vote_helper(TRUE);
    }
  }

  BAM_DMUX_LOG_INFO(4, "Power exit event=%s, local_state=%s, remote_state=%s tx_flush=%d.",
                    bam_dmux_power_event_str(event), 
                    bam_dmux_local_power_state_str(bam_dmux_local_power_state),
                    bam_dmux_remote_power_state_str(bam_dmux_remote_power_state), tx_flush);
  
  bam_dmux_os_mutex_unlock(&bam_dmux_common_mutex);
  
  if (ch_notify)
  {
    bam_dmux_ch_notify_power_state(power_state);
  }
  if (tx_flush)
  {
    bam_dmux_ch_tx_flush();
  }
}

/*=============================================================================
  FUNCTION  bam_dmux_bam_tx_done
=============================================================================*/
/**
 * This function is called by the BAM interface layer to Core layer when
 * TX(UL) transfer is completed.
 *
 * @param[in]  buf_pa      Buffer physical address.
 * @param[in]  buf_len     Buffer transfered length.
 * @param[in]  priv        Transfers private context.
 *
 */
/*===========================================================================*/
void bam_dmux_bam_tx_done
(
  uint32 buf_pa,
  uint16 buf_len,
  void   *priv
)
{
  bam_dmux_bufdesc_type *bufdesc = priv;

  BAM_DMUX_LOG_FUNC_START();

  /* Validate the arguments */
  if ((bufdesc          == NULL) ||
      (bufdesc->done_cb == NULL) ||
      (bufdesc->buf     == NULL) ||
      (bufdesc->buf_pa  != buf_pa) ||
      (bufdesc->len     != buf_len) ||
      (bufdesc->ch      == NULL))
  {
    BAM_DMUX_LOG_FATAL(1, "Invalid bufdesc! bufdesc=0x%08X.", bufdesc);
    return;
  }

  bufdesc->status = BAM_DMUX_STATUS_SUCESS;
  bufdesc->done_cb(bufdesc);

  if (bam_dmux_os_atomic_get_mask(&bam_dmux_tx_cmd_pending))
  {
    bam_dmux_tx_pending_cmds();
  }
  else if (bam_dmux_os_atomic_get_mask(&bam_dmux_tx_data_pending))
  {
    bam_dmux_notify_clients_tx_resume();
  }

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_bam_rx_done
=============================================================================*/
/**
 * This function is called by the BAM interface layer to Core layer when
 * RX(DL) transfer is completed.
 * This function handles the received command or data from A2.
 *
 * @param[in]  buf_pa      Buffer physical address.
 * @param[in]  rx_len      received buffer length.
 * @param[in]  priv        Transfers private context.
 *
 */
/*===========================================================================*/
void bam_dmux_bam_rx_done
(
  uint32 buf_pa,
  uint16 rx_len,
  void   *priv
)
{
  bam_dmux_bufdesc_type *bufdesc = priv;
  bam_dmux_header_type  *hdr;

  BAM_DMUX_LOG_FUNC_START();

  /* Validate the arguments */
  if ((bufdesc         == NULL) ||
      (bufdesc->buf    == NULL) ||
      (bufdesc->buf_pa != buf_pa) ||
      (rx_len          < BAM_DMUX_HEADER_SIZE))
  {
    BAM_DMUX_LOG_FATAL(1, "Invalid bufdesc! bufdesc=0x%08X.", bufdesc);
    return;
  }

  /* Just for validation */
  bam_dmux_os_mutex_lock(&bam_dmux_common_mutex);
  {
    bam_dmux_bufdesc_type *head_bufdesc;

    head_bufdesc = BAM_DMUX_LIST_HEAD(bam_dmux_submited_rx_bufdescs);

    /* current bufdesc should the head of the submited list */
    if (head_bufdesc != bufdesc)
    {
      BAM_DMUX_LOG_FATAL(4, "Head bufdesc is not same as current one! bufdesc=0x%08X, "
          "head_bufdesc=0x%08X.", bufdesc, head_bufdesc);
    }
    BAM_DMUX_LIST_REMOVE(bam_dmux_submited_rx_bufdescs, bufdesc, link);
    BAM_DMUX_LINK_INIT(bufdesc->link);
  }
  bam_dmux_os_mutex_unlock(&bam_dmux_common_mutex);

  bam_dmux_os_cache_invalidate(bufdesc->buf, rx_len);

  hdr = (bam_dmux_header_type *)bufdesc->buf;

  if ((hdr->magic   != BAM_DMUX_HEADER_MAGIC) ||
      (hdr->channel >= BAM_DMUX_CFG_MAX_CHANNELS))
  {
    BAM_DMUX_LOG_ERR(4, "Invalid header! bufdesc=0x%08X, hdr=0x%08X, "
        "magic=0x%04X, channel_id=%d.", bufdesc, hdr, hdr->magic, hdr->channel);
    bam_dmux_rx_bufdesc_submit(bufdesc);
    return;
  }

  BAM_DMUX_LOG_DBG(5, "RX: cmd=%d, cmd_opts=0x%02X, ch_id=%d, pad=%d, len=%d",
      hdr->cmd, hdr->cmd_opts, hdr->channel, hdr->pad, hdr->len);

  switch (hdr->cmd)
  {
    case BAM_DMUX_CMD_OPEN:
    case BAM_DMUX_CMD_OPEN_PC_DISABLED:
      bam_dmux_handle_open_cmd(hdr);
      bam_dmux_rx_bufdesc_submit(bufdesc);
      break;

    case BAM_DMUX_CMD_CLOSE:
      bam_dmux_handle_close_cmd(hdr);
      bam_dmux_rx_bufdesc_submit(bufdesc);
      break;

    case BAM_DMUX_CMD_DATA:
      bam_dmux_handle_data_cmd(bufdesc, rx_len);
      break;

    default:
      BAM_DMUX_LOG_ERR(3, "Invalid cmd! bufdesc=0x%08X, hdr=0x%08X, "
          "cmd=0x%02X.", bufdesc, hdr, hdr->cmd);

      /* Resubmit buffer descriptor */
      bam_dmux_rx_bufdesc_submit(bufdesc);
      break;
  }

  BAM_DMUX_LOG_FUNC_END();
}
