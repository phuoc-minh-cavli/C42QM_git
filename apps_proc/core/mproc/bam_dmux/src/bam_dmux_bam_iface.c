/*=============================================================================

                            BAM DMUX BAM IFACE

 BAM Data Multiplexer BAM Interface.

 Copyright  2016-2017 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/bam_dmux/src/bam_dmux_bam_iface.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/17/16   rv      Initial version.
===========================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"
#include "err.h"
#include "bam.h"
#include "bam_dmux_log.h"
#include "bam_dmux_os.h"
#include "bam_dmux_cfg.h"
#include "bam_dmux_power.h"
#include "bam_dmux_bam_iface.h"
#include "bam_dmux_core.h"

/*=============================================================================

                         MACRO DECLARATIONS

=============================================================================*/
#undef BAM_DMUX_LOG_TAG
#define BAM_DMUX_LOG_TAG "BAM"

#if 0
#define BAM_DMUX_LOG_FUNC_START() BAM_DMUX_LOG_DBG(1, "Func Start %s.", __FUNCTION__)
#define BAM_DMUX_LOG_FUNC_END()   BAM_DMUX_LOG_DBG(1, "Func End %s.", __FUNCTION__)
#else
#define BAM_DMUX_LOG_FUNC_START() do{}while(0)
#define BAM_DMUX_LOG_FUNC_END()   do{}while(0)
#endif

/** The descriptor size */
#define BAM_DMUX_BAM_DESC_SIZE  (8)

/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/
typedef struct {
  bam_handle             handle;
  bam_pipe_config_type   cfg;
  bam_callback_type      cb;
  bam_dmux_os_mutex_type mutex;
  bam_dir_type           dir;
  uint32                 pipe_num;
  uint16                 max_bufs;
  uint16                 buf_cnt;
  boolean                status;
} bam_dmux_bam_pipe_type;

/*=============================================================================

                       LOCAL DATA DEFINATION

=============================================================================*/
/* BAM handle */
static bam_handle        bam_dmux_bam_handle;
static bam_config_type   bam_dmux_bam_cfg;
static bam_callback_type bam_dmux_bam_cb;

/* BAM Pipe handles */
static bam_dmux_bam_pipe_type bam_dmux_bam_tx_pipe;
static bam_dmux_bam_pipe_type bam_dmux_bam_rx_pipe;

/*=============================================================================

                     PUBLIC VARIABLE DECLARATIONS

=============================================================================*/


/*=============================================================================

                     LOCAL FUNCTION DEFINATION

=============================================================================*/

/*=============================================================================
  FUNCTION  bam_dmux_bam_err_cb
=============================================================================*/
/**
 * BAM calls this callback function when there is any BAM errors.
 *
 */
/*===========================================================================*/
static void bam_dmux_bam_err_cb(bam_result_type bam_result)
{
  BAM_DMUX_LOG_FATAL(5, "BAM TX error! event=%d, status=0x%08X, address=0x%08X, "
      "data=0x%08X, bus_ctrls=0x%08X.", bam_result.event,
      bam_result.data.error.status, bam_result.data.error.address,
      bam_result.data.error.data, bam_result.data.error.bus_ctrls);
}

/*=============================================================================
  FUNCTION  bam_dmux_bam_pipe_init
=============================================================================*/
/**
 * Initializes BAM Pipe.
 *
 */
/*===========================================================================*/
static void bam_dmux_bam_pipe_init
(
  bam_dmux_bam_pipe_type *pipe,
  bam_dir_type           dir,
  uint32                 pipe_num,
  uint32                 max_bufs,
  bam_callback_func_type callback
)
{
  bam_status_type status;
  
  BAM_DMUX_LOG_FUNC_START();
  
  memset(pipe, 0, sizeof(bam_dmux_bam_pipe_type));

  pipe->dir      = dir;
  pipe->pipe_num = pipe_num;
  pipe->max_bufs = max_bufs;
  pipe->buf_cnt  = 0;

  bam_dmux_os_mutex_init(&pipe->mutex);

  pipe->cfg.options      = BAM_O_DESC_DONE | BAM_O_EOT | BAM_O_OUT_OF_DESC | BAM_O_ERROR;
  pipe->cfg.dir          = dir;
  pipe->cfg.mode         = BAM_MODE_SYSTEM;
  pipe->cfg.evt_thresh   = 0x10;
  pipe->cfg.desc_size    = 0x800;//(max_bufs + 1) * BAM_DMUX_BAM_DESC_SIZE;
  pipe->cfg.desc_base_va = (uint32)bam_dmux_os_uncached_malloc(
                                        pipe->cfg.desc_size,
                                        &pipe->cfg.desc_base_pa);

  pipe->cb.func = callback;
  pipe->cb.data = NULL;

  pipe->handle = bam_pipe_init(bam_dmux_bam_handle,
                               pipe->pipe_num,
                               &pipe->cfg,
                               &pipe->cb);
  if (pipe->handle == (bam_handle)0)
  {
    BAM_DMUX_LOG_FATAL(1, "bam_pipe_init is failed! direction=%d.", dir);
  }
  status = bam_pipe_setirqmode(pipe->handle, 1, pipe->cfg.options);
  if (status != BAM_SUCCESS)
  {
    BAM_DMUX_LOG_FATAL(1, "bam_pipe_setirqmode is failed! status=%d.", status);
  }

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_bam_pipe_enable
=============================================================================*/
/**
 * Enables BAM Pipe.
 *
 */
/*===========================================================================*/
static void bam_dmux_bam_pipe_enable(bam_dmux_bam_pipe_type *pipe)
{
  bam_status_type ret;

  BAM_DMUX_LOG_INFO(2, "BAM Pipe enabling dir=%d, status=%d.", pipe->dir, pipe->status);
  
  bam_dmux_os_mutex_lock(&pipe->mutex);

  ret = bam_pipe_enable(pipe->handle);

  if (ret != BAM_SUCCESS)
  {
    BAM_DMUX_LOG_FATAL(2, "Pipe enable failed!, dir=%d, ret=%d.",  pipe->dir, ret);
  }

  pipe->buf_cnt = 0;
  
  /* Pipe is enabled, update state */
  pipe->status = TRUE;

  bam_dmux_os_mutex_unlock(&pipe->mutex);

  BAM_DMUX_LOG_INFO(1, "BAM Pipe enabling done dir=%d ", pipe->dir);
}

/*=============================================================================
  FUNCTION  bam_dmux_bam_pipe_disable
=============================================================================*/
/**
 * Disables BAM Pipe.
 *
 */
/*===========================================================================*/
static void bam_dmux_bam_pipe_disable(bam_dmux_bam_pipe_type *pipe)
{
  bam_status_type ret;

  BAM_DMUX_LOG_INFO(2, "BAM Pipe disabling dir=%d, status=%d.", pipe->dir, pipe->status);
  
  bam_dmux_os_mutex_lock(&pipe->mutex);

  ret = bam_pipe_disable(pipe->handle);

  if (ret != BAM_SUCCESS)
  {
    BAM_DMUX_LOG_FATAL(2, "Pipe disable failed!, dir=%d, ret=%d.",  pipe->dir, ret);
  }
  
  pipe->buf_cnt = 0;

  /* Pipe is disabled, update state */
  pipe->status = FALSE;
  
  bam_dmux_os_mutex_unlock(&pipe->mutex);

  BAM_DMUX_LOG_INFO(1, "BAM Pipe disabling done dir=%d.", pipe->dir);
}

/*=============================================================================
  FUNCTION  bam_dmux_bam_buf_submit
=============================================================================*/
/**
 * Submits a buffer for TX or RX to BAM.
 *
 */
/*===========================================================================*/
static boolean bam_dmux_bam_buf_submit
(
  bam_dmux_bam_pipe_type *pipe,
  void                   *buf_va,
  uint32                  buf_pa,
  uint16                  buf_len,
  void                   *priv
)
{
  bam_status_type bam_status;
  boolean         submit_status = FALSE;

  BAM_DMUX_LOG_FUNC_START();
  
  if ((buf_va == NULL) || (buf_pa == 0) || (buf_len == 0))
  {
    BAM_DMUX_LOG_FATAL(3, "Invalid valid args!, buf_va=0x%08X, buf_pa=0x%08X, "
        "buf_len=%d.", buf_va, buf_pa, buf_len);
  }

  bam_dmux_os_mutex_lock(&pipe->mutex);
  
  if (!pipe->status)
  {
    BAM_DMUX_LOG_WRN(1, "Trying to submit the buffer on disabled pipe dir=%d.", pipe->dir);
    goto UNLOCK_EXIT;
  }

  if (pipe->buf_cnt > pipe->max_bufs)
  {
    BAM_DMUX_LOG_WRN(3, "buf_cnt is not valid! buf_cnt is more max_bufs "
        "max_bufs=%d, buf_cnt=%d, dir=%d.", pipe->max_bufs, pipe->buf_cnt, pipe->dir);
    goto UNLOCK_EXIT;
  }
  else if (pipe->buf_cnt == pipe->max_bufs)
  {
    BAM_DMUX_LOG_DBG(3, "No space for new buffer descriptor in descriptor FIFO.",
        pipe->max_bufs, pipe->buf_cnt, pipe->dir);
    /* No space for new buffer descriptor in descriptor FIFO */
    goto UNLOCK_EXIT;
  }

  bam_dmux_os_cache_flush(buf_va, buf_len);

  bam_status = bam_pipe_transfer(pipe->handle,
                                 buf_pa,
                                 buf_len,
                                 BAM_IOVEC_FLAG_INT | BAM_IOVEC_FLAG_EOT,
                                 priv);
  if (bam_status != BAM_SUCCESS)
  {
    BAM_DMUX_LOG_FATAL(5, "Pipe transfer failed(%d)!, buf_va=0x%08X, buf_pa=0x%08X, "
        "buf_len=%d, dir=%d, priv=0x%08X.", bam_status, buf_va, buf_pa, buf_len, pipe->dir, priv);
  }
  else
  {
    /* Maintain the buffer submitted count */
    pipe->buf_cnt++;
    submit_status = TRUE;
  }

UNLOCK_EXIT:
  bam_dmux_os_mutex_unlock(&pipe->mutex);
  
  BAM_DMUX_LOG_FUNC_END();
  return submit_status;
}

/*=============================================================================
  FUNCTION  bam_dmux_bam_tx_cb
=============================================================================*/
/**
 * This function is called by BAM when TX(UL) transfer is done. This function
 * calls Core layer TX done function.
 *
 */
/*===========================================================================*/
static void bam_dmux_bam_tx_cb(bam_result_type bam_result)
{
  bam_dmux_bam_pipe_type *pipe = &bam_dmux_bam_tx_pipe;

  BAM_DMUX_LOG_FUNC_START();
  
  if (bam_result.event == BAM_EVENT_ERROR)
  {
    BAM_DMUX_LOG_FATAL(4, "BAM TX error! status=0x%08X, address=0x%08X, "
        "data=0x%08X, bus_ctrls=0x%08X.", bam_result.data.error.status,
        bam_result.data.error.address, bam_result.data.error.data,
        bam_result.data.error.bus_ctrls);
  }
  else if (bam_result.event == BAM_EVENT_EOT)
  {
    BAM_DMUX_LOG_DBG(3, "TX done, buf_pa=0x%08X, buf_size=%d, priv=0x%08X.",
        bam_result.data.xfer.iovec.buf_pa, bam_result.data.xfer.iovec.buf_size,
        bam_result.data.xfer.xfer_cb_data);

    bam_dmux_os_mutex_lock(&pipe->mutex);

    if ((pipe->buf_cnt == 0) || (pipe->buf_cnt > pipe->max_bufs))
    {
      BAM_DMUX_LOG_FATAL(2, "TX buf_cnt has invalid value. buf_cnt=%d, "
          "max_bufs=%d.", pipe->buf_cnt, pipe->max_bufs);
    }

    pipe->buf_cnt--;

    bam_dmux_os_mutex_unlock(&pipe->mutex);

    /* Notify core about TX done */
    bam_dmux_bam_tx_done(bam_result.data.xfer.iovec.buf_pa,
                         bam_result.data.xfer.iovec.buf_size,
                         bam_result.data.xfer.xfer_cb_data);
  }
  else
  {
    BAM_DMUX_LOG_DBG(1, "TX done callback unhandled event=%d.", bam_result.event);
  }

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_bam_rx_cb
=============================================================================*/
/**
 * This function is called by BAM when there is RX(DL) data. This function
 * calls Core layer RX done function.
 *
 */
/*===========================================================================*/
static void bam_dmux_bam_rx_cb(bam_result_type bam_result)
{
  bam_dmux_bam_pipe_type *pipe = &bam_dmux_bam_rx_pipe;

  BAM_DMUX_LOG_FUNC_START();
  
  if (bam_result.event == BAM_EVENT_ERROR)
  {
    BAM_DMUX_LOG_FATAL(4, "BAM RX error! status=0x%08X, address=0x%08X, "
        "data=0x%08X, bus_ctrls=0x%08X.", bam_result.data.error.status,
        bam_result.data.error.address, bam_result.data.error.data,
        bam_result.data.error.bus_ctrls);
  }
  else if (bam_result.event == BAM_EVENT_EOT)
  {
    BAM_DMUX_LOG_DBG(3, "RX done, buf_pa=0x%08X, buf_size=%d, priv=0x%08X.",
        bam_result.data.xfer.iovec.buf_pa, bam_result.data.xfer.iovec.buf_size,
        bam_result.data.xfer.xfer_cb_data);

    bam_dmux_os_mutex_lock(&pipe->mutex);

    if ((pipe->buf_cnt == 0) || (pipe->buf_cnt > pipe->max_bufs))
    {
      BAM_DMUX_LOG_FATAL(2, "RX buf_cnt has invalid value. buf_cnt=%d, "
          "max_bufs=%d.", pipe->buf_cnt, pipe->max_bufs);
    }

    pipe->buf_cnt--;

    bam_dmux_os_mutex_unlock(&pipe->mutex);
    
    /* Notify core about RX done */
    bam_dmux_bam_rx_done(bam_result.data.xfer.iovec.buf_pa,
                         bam_result.data.xfer.iovec.buf_size,
                         bam_result.data.xfer.xfer_cb_data);
  }
  else
  {
    BAM_DMUX_LOG_DBG(1, "RX callback unhandled event=%d.", bam_result.event);
  }

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================

                     PUBLIC FUNCTION DEFINATION

=============================================================================*/

/*=============================================================================
  FUNCTION  bam_dmux_bam_init
=============================================================================*/
/**
 * Initializes BAM. Core layer calls this function.
 *
 */
/*===========================================================================*/
void bam_dmux_bam_init(void)
{
  bam_config_type       *bam_cfg = &bam_dmux_bam_cfg;
  bam_callback_type     *bam_cb = &bam_dmux_bam_cb;

  BAM_DMUX_LOG_FUNC_START();
  
  memset(bam_cfg, 0, sizeof(bam_config_type));
  memset(bam_cb, 0, sizeof(bam_callback_type));

  bam_cfg->bam_pa       = BAM_DMUX_CFG_BAM_PHYSICAL_ADDR;
  bam_cfg->sum_thresh   = BAM_DMUX_CFG_BAM_THRESHOLD;
  bam_cfg->bam_irq      = BAM_DMUX_CFG_BAM_IRQ;
  bam_cfg->bam_irq_mask = BAM_IRQ_HRESP_ERR_EN | BAM_IRQ_ERR_EN;

  bam_cb->func = bam_dmux_bam_err_cb;
  bam_cb->data = NULL;

  bam_dmux_bam_handle = bam_init(bam_cfg, bam_cb);
  if (bam_dmux_bam_handle == (bam_handle)0)
  {
    BAM_DMUX_LOG_FATAL(0, "bam_init is failed!");
  }

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_bam_reset
=============================================================================*/
/**
 * Resets BAM. Core layer calls this function.
 *
 */
/*===========================================================================*/
void bam_dmux_bam_reset(void)
{
  bam_status_type ret;

  BAM_DMUX_LOG_INFO(0, "BAM Reseting.");
  
  bam_dmux_os_mutex_lock(&bam_dmux_bam_tx_pipe.mutex);
  bam_dmux_os_mutex_lock(&bam_dmux_bam_rx_pipe.mutex);

  ret = bam_reset(bam_dmux_bam_handle);

  bam_dmux_os_mutex_unlock(&bam_dmux_bam_rx_pipe.mutex);
  bam_dmux_os_mutex_unlock(&bam_dmux_bam_tx_pipe.mutex);

  if (ret != BAM_SUCCESS)
  {
    BAM_DMUX_LOG_FATAL(1, "bam_reset is failed!, ret=%d.", ret);
  }

  BAM_DMUX_LOG_INFO(0, "BAM Reset done.");
}

/*=============================================================================
  FUNCTION  bam_dmux_bam_pipes_init
=============================================================================*/
/**
 * Initializes BAM TX(UL) and RX(DL) Pipes. Core layer calls this function.
 *
 */
/*===========================================================================*/
void bam_dmux_bam_pipes_init(void)
{
  BAM_DMUX_LOG_FUNC_START();
  
  /* TX Pipe initialization */
  bam_dmux_bam_pipe_init(&bam_dmux_bam_tx_pipe,
                         BAM_DIR_CONSUMER,
                         BAM_DMUX_CFG_BAM_TX_PIPE_NUMBER,
                         BAM_DMUX_CFG_MAX_TX_BUFS,
                         bam_dmux_bam_tx_cb);

  /* RX Pipe initialization */
  bam_dmux_bam_pipe_init(&bam_dmux_bam_rx_pipe,
                         BAM_DIR_PRODUCER,
                         BAM_DMUX_CFG_BAM_RX_PIPE_NUMBER,
                         BAM_DMUX_CFG_MAX_RX_BUFS,
                         bam_dmux_bam_rx_cb);

  BAM_DMUX_LOG_FUNC_END();
}


/*=============================================================================
  FUNCTION  bam_dmux_bam_pipes_enable
=============================================================================*/
/**
 * Enables BAM TX(UL) and RX(DL) Pipes. Core layer calls this function.
 *
 */
/*===========================================================================*/
void bam_dmux_bam_pipes_enable(void)
{
  BAM_DMUX_LOG_FUNC_START();
  
  bam_dmux_bam_pipe_enable(&bam_dmux_bam_tx_pipe);
  bam_dmux_bam_pipe_enable(&bam_dmux_bam_rx_pipe);

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_bam_pipes_disable
=============================================================================*/
/**
 * Disables BAM TX(UL) and RX(DL) Pipes. Core layer calls this function.
 *
 */
/*===========================================================================*/
void bam_dmux_bam_pipes_disable(void)
{
  BAM_DMUX_LOG_FUNC_START();
  
  bam_dmux_bam_pipe_disable(&bam_dmux_bam_tx_pipe);
  bam_dmux_bam_pipe_disable(&bam_dmux_bam_rx_pipe);

  BAM_DMUX_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_bam_tx_buf_submit
=============================================================================*/
/**
 * Submits TX(UL) buffer to BAM. Core layer calls this function.
 *
 */
/*===========================================================================*/
boolean bam_dmux_bam_tx_buf_submit
(
  void   *buf_va,
  uint32 buf_pa,
  uint16 buf_len,
  void   *priv
)
{
  BAM_DMUX_LOG_FUNC_START();
  
  return bam_dmux_bam_buf_submit(&bam_dmux_bam_tx_pipe,
                                 buf_va,
                                 buf_pa,
                                 buf_len,
                                 priv);
}

/*=============================================================================
  FUNCTION  bam_dmux_bam_rx_buf_submit
=============================================================================*/
/**
 * Submits RX(DL) buffer to BAM. Core layer calls this function.
 *
 */
/*===========================================================================*/
boolean bam_dmux_bam_rx_buf_submit
(
  void   *buf_va,
  uint32 buf_pa,
  uint16 buf_len,
  void   *priv
)
{
  BAM_DMUX_LOG_FUNC_START();
  
  return bam_dmux_bam_buf_submit(&bam_dmux_bam_rx_pipe,
                                 buf_va,
                                 buf_pa,
                                 buf_len,
                                 priv);
}

