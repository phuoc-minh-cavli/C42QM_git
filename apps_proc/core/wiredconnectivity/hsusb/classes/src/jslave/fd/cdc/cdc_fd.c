/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.

==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/classes/src/jslave/fd/cdc/cdc_fd.c#2 $
  $DateTime: 2020/04/07 02:29:23 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================










************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#include <jusb_common_int.h>
#include <jusb_core_fd_int.h>
#include <jusb_chp9_int.h>
#include "hsu_timer.h"
#include "hsu_log.h"
#include "jusb_core.h"
#include "cdc_fd.h"
#include "hsu_conf_sel_i.h"
#include "hsu_al_ecm_int.h"
#include "hsu_os_abstract_bus.h"
#include "hsu_platform.h"

static jresult_t notify_response_available(cdc_ctx_t *ctx);
static void reset_encap_state(cdc_ctx_t *ctx, jbool_t notify_app,
    cdc_iostatus_t status);

#if defined(FEATURE_HS_USB_DROP_RX_ZLP_DEBUG_EXP) || defined(FEATURE_HS_USB_DROP_TX_ZLP_DEBUG_EXP)
#define HSU_DEBUG_RMNET_INSTANCE_NUMBER         3 /* first RMNET */
#define HSU_DEBUG_LOWER_BOUND_DROP_ZLP_NUM      5 
#define HSU_DEBUG_UPPER_BOUND_DROP_ZLP_NUM      7
#endif /* FEATURE_HS_USB_DROP_RX_ZLP_DEBUG_EXP || FEATURE_HS_USB_DROP_TX_ZLP_DEBUG_EXP */

#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION
hsu_missing_zlp_debug_type hsu_missing_zlp_debug_ctx;

#ifdef FEATURE_HS_USB_ERR_FATAL_UPON_TIMER_EXPIRY_FOR_RX_TX_ZLP_COMPLETION
static uint32 HSU_DEBUG_CDC_TX_ZLP_TIMER_VAL_MS = 100; /* for easy JTAG modification */
static uint32 HSU_DEBUG_CDC_RX_ZLP_TIMER_VAL_MS = 100; /* for easy JTAG modification */
static void hsu_cdc_send_encap_cmd_tx_zlp_tmr_cb(timer_cb_data_type data)
{
  HSU_ERR_FATAL("CDC SEND ENCAP CMD, TX ZLP TIMER EXPIRY", (uint32)data, 0, 0);
}

static void hsu_cdc_get_encap_resp_rx_zlp_tmr_cb(timer_cb_data_type data)
{
  HSU_ERR_FATAL("CDC GET ENCAP RESP, RX ZLP TIMER EXPIRY", (uint32)data, 0, 0);
}
#endif /* FEATURE_HS_USB_ERR_FATAL_UPON_TIMER_EXPIRY_FOR_RX_TX_ZLP_COMPLETION */
#endif /* FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */

/**
 * Function name:  cdc_get_iostatus
 * Description:  translate CORE request status to CDC defined status 
 * Parameters: 
 *     @req: (IN) CORE request 
 *     @check_size: (IN) Should size be checked for a mismatch 
 *
 * Return value: CDC status 
 * Scope: global
 **/
cdc_iostatus_t cdc_get_iostatus(request_t *req, jbool_t check_size)
{
    switch(req->status)
    {
    case REQUEST_COMPLETED:
        if (check_size && (req->bytes_transferred != req->transfer_size)
/* HSU addition: SPS */
#ifdef FEATURE_HS_USB_BAM
          && !req->sps_mode
#endif
          )
        {
            HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "CDC, transfer_size_error, transferred/trans_size %u/%u",
              req->bytes_transferred, req->transfer_size);
            return CDC_TRANSFER_ERROR;
        }
        return CDC_TRANSFER_OK;

    case REQUEST_CANCELLED:
        DBG_I(DSLAVE_CDC, ("cdc_get_iostatus: transfer cancelled (%u/%u)\n",
            req->bytes_transferred, req->transfer_size));
        return CDC_TRANSFER_CANCELLED;

    case REQUEST_ERROR:
    default:
        HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "CDC, transfer_error, transferred/trans_size %u/%u",
          req->bytes_transferred, req->transfer_size);
        return CDC_TRANSFER_ERROR;
    }
}

/**
 * Function name:  get_max_packet_size
 * Description:  returns the maximum packet size used for a given speed
 * Parameters: 
 *     @speed: (IN) USB connection speed (LS,FS,HS)
 *
 * Return value:  maximum packet size in bytes
 * Scope: local
 **/
static juint16_t get_max_packet_size(device_speed_t speed)
{
    if(speed == SPEED_SUPER)
    {
        return 1024;
    }
    else if (speed == SPEED_HIGH)
    {
        return 512;
    }
    else
        return 64;
}

/*========================
 *     Data plane
 *=======================*/

/**
 * Function name:  read_complete
 * Description:  cdc_read() completion callback
 * Parameters: 
 *     @req: (IN) CORE request
 *
 * Scope: local
 **/
static void read_complete(request_t *req)
{
    cdc_ctx_t* ctx = (cdc_ctx_t *)req->context;
    juint32_t completed_req = req->completed_req;
    request_t *curr_req;
    jint_t i;
    cdc_iostatus_t *status;
    juint32_t *size;
    void **buffer;
    jbool_t is_short_packet = 0;
    /* HSU addition: unite 3 jmalloc/jfree into single one */
    juint32_t alloc_size;
    void* mem_ptr;
    /* HSU addition: end */

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DSLAVE_CDC, ("read_complete: entered\n"));
#endif /* CONFIG_DATA_PATH_DEBUG */

    /* HSU addition: unite 3 jmalloc/jfree into single one */
    alloc_size = completed_req * (sizeof(cdc_iostatus_t) /* for status */ +
      sizeof(juint32_t) /* for size */ + sizeof(void*) /* for buffer */);
    mem_ptr = jmalloc(alloc_size, M_ZERO);
    KASSERT(mem_ptr, ("Memory allocation failed"));

    size = (juint32_t *)mem_ptr;
    buffer = (void**)((juint8_t*)size + sizeof(juint32_t) * completed_req);
    status = (cdc_iostatus_t *)((juint8_t*)buffer + sizeof(void*) * completed_req);
    /* HSU addition: end */

    if ((ctx->cbs.basic.read_multi_complete_h && !req->single_request) || 
        (ctx->cbs.basic.read_complete_h && req->single_request))
    {
        for (i = 0, curr_req = req; i < completed_req && curr_req; 
            i++, curr_req = LIST_NEXT(curr_req, next_request_in_list))
        {
            status[i] = cdc_get_iostatus(curr_req, 0);

            /* We signal the application that the transfer is complete when a short
             * packet is recieved from the host. That is when either:
             * 1. The bytes transferred are less than requested
             * 2. The bytes_transferred == transfer_size, and the transfer size is
             *    NOT a multiple of (USB) max packet size
             * (When it is a multiple of max packet size, we expect the application
             * to call cdc_read again for which we will call it with 0 bytes
             * transferred and a TRUE transfer complete flag) */
            is_short_packet = (curr_req->bytes_transferred < 
                curr_req->transfer_size) || 
                (curr_req->transfer_size % ctx->usb_max_packet_size);
    
            if (is_short_packet)
            {
#ifdef CONFIG_DATA_PATH_DEBUG
                DBG_X(DSLAVE_CDC, ("read_complete: short packet.\n"));
#endif /* CONFIG_DATA_PATH_DEBUG */
            }

            /* Copy received data to application's buffer */
            if (!(ctx->info.basic.flags & CDC_INIT_BUFFER_DMABLE))
            {
                j_memcpy(curr_req->caller_private, curr_req->buffer.vaddr,
                    curr_req->bytes_transferred);
            }
            size[i] = curr_req->bytes_transferred;
            buffer[i] = curr_req->caller_private;
        }

        if (req->single_request)
        {
            ctx->cbs.basic.read_complete_h(*status, req->caller_private, 
                req->bytes_transferred, is_short_packet, ctx->app_ctx);
        }
        else 
        {
            ctx->cbs.basic.read_multi_complete_h(status, buffer, size,
                completed_req, ctx->app_ctx);
        }
    }
    else
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, read_complete, no_callback_provided");
    }

    core_request_free(req);

    /* HSU addition: unite 3 jmalloc/jfree into single one */
    if (mem_ptr)
        jfree(mem_ptr);
}

/* HSU addition: SPS */
#ifdef FEATURE_HS_USB_BAM
/**
* Function name:  read_sps_complete
* Description:  cdc_read() completion callback for SPS mode
* Parameters: 
*     @req: (IN) CORE request
*
* Scope: local
**/
static void read_sps_complete(request_t *req)
{
  cdc_ctx_t* ctx = (cdc_ctx_t *)req->context;
  cdc_iostatus_t status;
  juint32_t size;
  void* buffer;

#ifdef CONFIG_DATA_PATH_DEBUG
  DBG_I(DSLAVE_CDC, ("read_sps_complete: entered"));
#endif /* CONFIG_DATA_PATH_DEBUG */

  KASSERT((req->single_request && req->completed_req==1), 
    ("SPS mode supported only for single_request"));

  status = cdc_get_iostatus(req, 0);
  /* in CDC, SPS mode is always used in infinite mode so 
  bytes_transferred are irrelevant */
  size = 0;
  buffer = NULL;

  if (ctx->cbs.basic.read_complete_h)
  {
    ctx->cbs.basic.read_complete_h(status, buffer, size, FALSE, ctx->app_ctx);
  }
  else if (ctx->cbs.basic.read_multi_complete_h)
  {
    ctx->cbs.basic.read_multi_complete_h(&status, &buffer, &size, 1, ctx->app_ctx);
  }
  else
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, no_callback_provide");
  }

  core_request_free(req);
}

#endif /* FEATURE_HS_USB_BAM */
/* End of HSU addition */

/**
 * Function name:  write_complete
 * Description:  cdc_write() completion callback
 * Parameters: 
 *     @req: (IN) CORE request
 *
 * scope: local
 **/
static void write_complete(request_t *req)
{
    cdc_ctx_t* ctx = (cdc_ctx_t*)req->context;
    juint32_t completed_req = req->completed_req;
    cdc_iostatus_t *status = NULL;
    juint32_t *buf_sizes = NULL;
    void **buffers = NULL;
    jint_t i;
    request_t *curr_req;
    /* HSU addition: unite 3 jmalloc/jfree into single one */
    juint32_t alloc_size;
    void* mem_ptr;
    /* HSU addition: end */

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_X(DSLAVE_CDC, ("write_complete: entered\n"));
#endif /* CONFIG_DATA_PATH_DEBUG */

    /* HSU addition: unite 3 jmalloc/jfree into single one */
    alloc_size = completed_req * (sizeof(cdc_iostatus_t) /* for status */ +
      sizeof(juint32_t) /* for buf_sizes */ + sizeof(void*) /* for buffers */);
    mem_ptr = jmalloc(alloc_size, M_ZERO);
    KASSERT(mem_ptr, ("Memory allocation failed"));

    buf_sizes = (juint32_t *)mem_ptr;
    buffers = (void**)((juint8_t*)buf_sizes + sizeof(juint32_t) * completed_req);
    status = (cdc_iostatus_t *)((juint8_t*)buffers + sizeof(void*) * completed_req);
    /* HSU addition: end */

    if ((ctx->cbs.basic.write_complete_h && req->single_request) ||
        (ctx->cbs.basic.write_multi_complete_h && !req->single_request))
    {
        for (i = 0, curr_req = req; i < completed_req && curr_req; 
            i++, curr_req = LIST_NEXT(curr_req, next_request_in_list))
        {
            status[i] = cdc_get_iostatus(curr_req, 1);
            buf_sizes[i] = curr_req->bytes_transferred;
            buffers[i] = curr_req->caller_private;
        }

        if (req->single_request)
        {
            ctx->cbs.basic.write_complete_h(*status, req->caller_private,
                ctx->app_ctx);
        }
        else 
        {
            ctx->cbs.basic.write_multi_complete_h(status, buffers, buf_sizes,
                completed_req, ctx->app_ctx);
        }
    }

    core_request_free(req);

    /* HSU addition: unite 3 jmalloc/jfree into single one */
    if (mem_ptr)
        jfree(mem_ptr);
}

/* HSU addition: SPS */
#ifdef FEATURE_HS_USB_BAM
/**
* Function name:  write_sps_complete
* Description:  cdc_write() completion callback for SPS mode
* Parameters: 
*     @req: (IN) CORE request
*
* scope: local
**/
static void write_sps_complete(request_t *req)
{
  cdc_ctx_t* ctx = (cdc_ctx_t*)req->context;
  cdc_iostatus_t status;
  juint32_t size;
  void* buffer;

#ifdef CONFIG_DATA_PATH_DEBUG
  DBG_I(DSLAVE_CDC, ("write_sps_complete: entered"));
#endif /* CONFIG_DATA_PATH_DEBUG */

  KASSERT((req->single_request && req->completed_req==1), 
    ("SPS mode supported only for single_request"));

  status = cdc_get_iostatus(req, 0);
  /* in CDC, SPS mode is always used in infinite mode so 
  bytes_transferred are irrelevant */
  size = 0;
  buffer = NULL;

  if (ctx->cbs.basic.write_complete_h)
  {
    ctx->cbs.basic.write_complete_h(status, buffer, ctx->app_ctx);
  }
  else if (ctx->cbs.basic.write_multi_complete_h)
  {
    ctx->cbs.basic.write_multi_complete_h(&status, &buffer, &size, 1, ctx->app_ctx);
  }
  else
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, no_callback_provide");
  }

  core_request_free(req);
}

#endif /* FEATURE_HS_USB_BAM */
/* End of HSU addition */


/* HSU addition: BEGIN */
/**
 * Function name:  cdc_get_max_packet_size
 * Description:  returns current max packet size 
                 based on current USB connection speed
 * Parameters: 
 *     @handle: (IN) CDC handle
 *
 * Return value: current max packet size
 * Scope: global
 **/
juint16_t cdc_get_max_packet_size(cdc_handle_t handle)
{
    cdc_ctx_t *ctx = (cdc_ctx_t *)handle;
    if (NULL == ctx)
    {
      return 0;
    }

    return ctx->usb_max_packet_size;
}
/* HSU addition: END */


/**
 * Function name:  cdc_handle_tx_completion
 * Description:  handle tx completion in the DCD
 * Parameters: 
 *     @handle: (IN) CDC handle
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t cdc_handle_tx_completion(cdc_handle_t handle)
{
    cdc_ctx_t *ctx = (cdc_ctx_t *)handle;

    if (!ctx->in_pipe.desc)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, in_pipe_not_enabled");
        return JEINVAL;
    }

    core_handle_tx_completion(ctx->core_ctx, ctx->in_pipe.desc);

    return 0;
}

/**
 * Function name:  cdc_write_multiple
 * Description:  send data to the USB host
 * Parameters: 
 *     @handle: (IN) CDC handle
 *     @buffers: (IN) data to send
 *     @buf_sizes: (IN) size of data to send in bytes
 *     @req_num: (IN) number of requests to send
 *     @timeout: (IN) timeout to complete the request
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t cdc_write_multiple(cdc_handle_t handle, void **buffers,
    juint32_t *buf_sizes, juint32_t req_num, juint32_t timeout)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
    cdc_init_info_t *info = &ctx->info.basic;
    request_t *req = NULL;
    request_t *curr_req = NULL;
    request_t *start_req = NULL;
    jresult_t rc = 0;
    jint_t i;

    if (!req_num)
    {
      rc = JEINVAL;
      goto Error;
    }

    /* Make sure we are within segment limits */
    for (i = 0; i < req_num; i++)
    {
        if (info->max_segment_size && (buf_sizes[i] > info->max_segment_size))
        {
            HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "CDC, illegal_size, buf_size %u, max_seg %u",
              buf_sizes[i], info->max_segment_size);
            rc = JEINVAL;
            goto Error;
        }
    }

    if (!ctx->in_pipe.desc)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, in_pipe_not_enabled_for_iface_and_setting");
        rc = JEPERM;
        goto Error;
    }

    for (i = 0; i < req_num; i++)
    {
        if (ctx->info.basic.flags & CDC_INIT_BUFFER_DMABLE)
            req = core_request_alloc(buf_sizes[i], buffers[i]);
        else 
            req = core_request_alloc(buf_sizes[i], NULL);

        if (!req)
        {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, failed_to_allocate_req");
            rc = JENOMEM;
            goto Error;
        }

        if (!start_req)
            start_req = req;
        else 
            LIST_NEXT(curr_req, next_request_in_list) = req;

        curr_req = req;

        /* Save application pointer, and copy its buffer if necessary */
        req->caller_private = buffers[i];
        if (!(ctx->info.basic.flags & CDC_INIT_BUFFER_DMABLE))
            j_memcpy(req->buffer.vaddr, buffers[i], buf_sizes[i]);

        /* Use ZLP's to delineate data:
         * 1. On segment based mediums (such as Ethernet) and the data size is
         *    less than max_segment_size.
         * 2. The application explicitly requests to send ZLP
         */
        if (ctx->info.basic.zlp_terminator || 
            (ctx->info.basic.max_segment_size &&
            (buf_sizes[i] < ctx->info.basic.max_segment_size)))
        {
            req->zero = 1;
        }
        else
        {
            req->zero = 0;
        }
    
        req->complete = write_complete;
        req->context = ctx;
        req->transfer_size = buf_sizes[i];
        req->single_request = 0;
        req->timeout = timeout;
    }

    rc = core_send_data(ctx->core_ctx, ctx->in_pipe.desc, start_req);
    if (rc)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, failed_on_send_data");
        goto Error;
    }
    
    return rc;

Error:
    if (start_req)
        core_request_free(start_req);
    
    return rc;
}

/**
 * Function name:  cdc_write
 * Description:  send data to the USB host
 * Parameters: 
 *     @handle: (IN) CDC handle
 *     @buffer: (IN) data to send
 *     @size: (IN) size of data to send in bytes
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t cdc_write(cdc_handle_t handle, void *buffer, juint32_t size)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
    cdc_init_info_t *info = &ctx->info.basic;
    request_t *req = NULL;
    jresult_t rc = 0;

    /* Make sure we are within segment limits */
    if (info->max_segment_size && (size > info->max_segment_size))
    {
        HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "CDC, illegal_size, size %u, max_seg %u",
          size, info->max_segment_size);
        rc = JEINVAL;
        goto Error;
    }

    if (!ctx->in_pipe.desc)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, in_pipe_not_enabled_for_iface_and_setting");
        rc = JEPERM;
        goto Error;
    }
        
    if (ctx->info.basic.flags & CDC_INIT_BUFFER_DMABLE)
        req = core_request_alloc(size, buffer);
    else
        req = core_request_alloc(size, NULL);

    if (!req)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, failed_to_allocate_req");
        rc = JENOMEM;
        goto Error;
    }

    /* Save application pointer, and copy its buffer if necessary */
    req->caller_private = buffer;
    if (!(ctx->info.basic.flags & CDC_INIT_BUFFER_DMABLE))
        j_memcpy(req->buffer.vaddr, buffer, size);

    /* Use ZLP's to delineate data:
     * 1. On segment based mediums (such as Ethernet) and the data size is
     *    less than max_segment_size.
     * 2. The application explicitly requests to send ZLP
     */
    if (ctx->info.basic.zlp_terminator || 
        (ctx->info.basic.max_segment_size &&
        (size < ctx->info.basic.max_segment_size)))
    {
        req->zero = 1;
    }
    else
    {
        req->zero = 0;
    }
    
    req->complete = write_complete;
    req->context = ctx;
    req->transfer_size = size;

    /* Temp  fix CR51176: ZLP in 512 bytes transfer (OBEX)*/
    req->zero = 1;
    /* End temp fix*/

    rc = core_send_data(ctx->core_ctx, ctx->in_pipe.desc, req);
    if (rc < 0)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, failed_on_send_data");
        goto Error;
    }
    
    return rc;

Error:
    if (req)
        core_request_free(req);
    
    return rc;
}

/**
 * Function name:  cdc_read
 * Description:  read data from the USB host 
 * Parameters: 
 *     @handle: (IN) CDC handle
 *     @buffer: (OUT) buffer where data will be placed
 *     @size: (IN) size of buffer
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t cdc_read_multiple(cdc_handle_t handle, void **buffers,
    juint32_t *buf_sizes, juint32_t req_num, juint32_t timeout)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
    cdc_init_info_t *info = &ctx->info.basic;
    request_t *req = NULL;
    request_t *curr_req = NULL;
    request_t *start_req = NULL;
    jint_t i;
    jresult_t rc = 0;

    hsu_perf_log( PERF(CDC_READ_MULTIPLE, Entry), 0);

    if (!req_num)
    {
      rc = JEINVAL;
      goto Error;
    }
    
    for (i = 0; i < req_num; i++)
    {
        if (info->max_segment_size && (buf_sizes[i] > info->max_segment_size))
        {
            HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "CDC, illega_size, buf_size %u, max_seg %u",
              buf_sizes[i], info->max_segment_size);
            rc = JEINVAL;
            goto Error;
        }
    }

    if (!ctx->out_pipe.desc)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, out_pipe_not_enabled_for_iface_and_setting");
        rc = JEPERM;
        goto Error;
    }

    for (i = 0; i < req_num; i++)
    {
        if (ctx->info.basic.flags & CDC_INIT_BUFFER_DMABLE)
            req = core_request_alloc(buf_sizes[i], buffers[i]);
        else 
            req = core_request_alloc(buf_sizes[i], NULL);

        if (!req)
        {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, failed_to_allocate_req");
            return JENOMEM;
        }

        if (!start_req)
            start_req = req;
        else
            LIST_NEXT(curr_req, next_request_in_list) = req;

        curr_req = req;
        req->caller_private = buffers[i];
        req->complete = read_complete;
        req->context = ctx;
        req->transfer_size = buf_sizes[i];
        req->single_request = 0;
        req->timeout = timeout;
    }

    rc = core_read_data(ctx->core_ctx, ctx->out_pipe.desc, start_req);
    if (rc)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, failed_on_read_data");
        goto Error;
    }

    hsu_perf_log( PERF(CDC_READ_MULTIPLE, Exit), 0);
    return rc;

Error:
    if (start_req)
        core_request_free(start_req);
    
    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC, rc %u", rc);
    return rc;
}

jresult_t cdc_read(cdc_handle_t handle, void *buffer, juint32_t size)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
    cdc_init_info_t *info = &ctx->info.basic;
    request_t *req = NULL;
    jresult_t rc = 0;

    /* Make sure we are within segment limits */
    if (info->max_segment_size && (size > info->max_segment_size))
    {
        HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "CDC, illegal_size, size %u, max_seg %u",
              size, info->max_segment_size);
        
        rc = JEINVAL;
        goto Error;
    }

    if (!ctx->out_pipe.desc)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, out_pipe_not_enabled_for_iface_and_setting");
        rc = JEPERM;
        goto Error;
    }
    
    if (ctx->info.basic.flags & CDC_INIT_BUFFER_DMABLE)
        req = core_request_alloc(size, buffer);
    else
        req = core_request_alloc(size, NULL);

    if (!req)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, failed_to_allocate_req");
        rc = JENOMEM;
        goto Error;
    }

    /* Save application pointer */
    req->caller_private = buffer;

    req->complete = read_complete;
    req->context = ctx;
    req->transfer_size = size;
    req->single_request = 1;

    rc = core_read_data(ctx->core_ctx, ctx->out_pipe.desc, req);
    if (rc)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, failed_on_read_data");
        goto Error;
    }
    
    return rc;

Error:
    if (req)
        core_request_free(req);
    
    return rc;
}

/* HSU addition: SPS */
#ifdef FEATURE_HS_USB_BAM
/**
* Function name:  cdc_sps_read_write_infinite
* Description:  setup read and write eTD for SPS mode
* Parameters: 
*     @handle: (IN) CDC handle
*     @read_pipe_id: (IN) SPS pipe id for OUT data
*     @write_pipe_id: (IN) SPS pipe id for IN data
*
* Return value: 0 for success, or an error code
* Scope: global
**/
jresult_t cdc_sps_read_write_infinite(cdc_handle_t handle, juint8_t read_pipe_id, jbool_t ul, juint8_t write_pipe_id, jbool_t dl)
{
#if 1    //QDSS interface is disabled for twizy and thus, no BAM operations. We will directly return from here.
	return 0;
#else
	cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
    request_t *read_req = NULL, *write_req = NULL;
    jresult_t rc = 0;

    if (!ctx->out_pipe.desc && !ctx->in_pipe.desc)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, out_or_in_Pipe_not_enabled_for_iface");
        rc = JEPERM;
        goto Error;
    }

    if (dl && ctx->in_pipe.desc)
    {
        /* write request */
        write_req = core_request_alloc(0, 0);
        if (!write_req)
        {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, failed_to_allocate_write_req");
            rc = JENOMEM;
            goto Error;
        }

        write_req->complete = write_sps_complete;
        write_req->context = ctx;
        write_req->single_request = 1;
        write_req->zero = 1;

        /* SPS infinite mode */
        write_req->sps_mode = TRUE;
        write_req->tbe = 0;
        write_req->sps_pipe_id = write_pipe_id;

        rc = core_send_data(ctx->core_ctx, ctx->in_pipe.desc, write_req);
        if (rc)
        {
            HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC, failed_on_send_data, rc %i", rc);
            goto Error;
        }
        // Clear the request handle since CORE layer owns it after
        // successful core_send_data
        write_req = NULL;
    }

    // UL BAM-to-BAM can be bypassed to use the SW path.
    if (ul
      && ctx->out_pipe.desc 
      && !(ctx->info.basic.flags & CDC_INIT_SSUSB_UL_BAM_BYPASS))
    {
        /* read request */
        read_req = core_request_alloc(0, 0);
        if (!read_req)
        {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, failed_to_allocate_read_req");
            rc = JENOMEM;
            goto Error;
        }

        read_req->complete = read_sps_complete;
        read_req->context = ctx;
        read_req->single_request = 1;

        /* SPS infinite mode */
        read_req->sps_mode = TRUE;
        read_req->tbe = 0;
        read_req->sps_pipe_id = read_pipe_id;

        rc = core_read_data(ctx->core_ctx, ctx->out_pipe.desc, read_req);
        if (rc)
        {
            HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC, failed_on_read_data, rc %i", rc);

            goto Error;
        }
        // Clear the request handle since CORE layer owns it after
        // successful core_send_data
        read_req = NULL;
    }
    return rc;

Error:
    if (read_req)
        core_request_free(read_req);

    if (write_req)
        core_request_free(write_req);

    // HSU Addition
    // ESUSPENDED is not critical error.  
    // There is legitimate race condition where SUSPEND can come before REUSME
    // is handled completely.  Instead of having caller to the API to treat 
    // this error separately as non-critical, generically handle it here to 
    // recovery gracefully.
    // For HSIC core, SUSPEND and REUSME race condition is not expected,
    // so it should be handled as critical error.
    if (ESUSPENDED == rc)
    {
        core_t* core = (core_t*)ctx->core_ctx;
        if (core)
        {
            HSU_ULOG_1(NO_MSG, BUS_LOG, "ESUSPENDED, skipped, index %u", core->core_index);
            hsu_event_log(
                EVT_ESUSPENDED_CDC_READ_WRITE_INFINITE_SKIPPED, core->core_index);

            // Set the return to SUCCESS to let upper layer handle gracefully
            rc = 0;
        }
    }
    // End of HSU Addition
    
    return rc;
#endif
}

#endif /* FEATURE_HS_USB_BAM */
/* End of HSU addition */

/**
 * Function name:  cdc_abort_write
 * Description:  cancel all write requests on the IN pipe
 * Parameters:
 *     @handle: (IN) CDC handle
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_abort_write(cdc_handle_t handle)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;

    if (!ctx->in_pipe.desc)
        return JENXIO;

    core_abort_pipe(ctx->core_ctx, ctx->in_pipe.desc);
    return 0;
}

/**
 * Function name:  cdc_abort_read
 * Description:  cancel all read requests on the OUT pipe
 * Parameters: 
 *     @handle: (IN) CDC handle 
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_abort_read(cdc_handle_t handle)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;

    if (!ctx->out_pipe.desc)
        return JENXIO;

    core_abort_pipe(ctx->core_ctx, ctx->out_pipe.desc);
    return 0;
}

/*===============================================
 *   Control class - Host to Device (Requests)
 *==============================================*/

/**
 * Function name:  create_request
 * Description:  create (or reallocate) a CORE request
 * Parameters: 
 *     @p_req: (IN/OUT) CORE request, or pointer to a NULL request pointer
 *     @buffer: (IN) request data (for write requests. NULL for read requests)
 *     @size: (IN) size of data in bytes (size of buffer, in case of read)
 *     @core: (IN) core context
 * Return value: CORE request, or NULL on failure 
 * Scope: local
 **/
static request_t* create_request_ep0(request_t **p_req, void *buffer, juint32_t size, void *core_ctx)
{
    request_t *req = NULL;

    req = *p_req;

    /* Make sure we do not overlap requests */
    if (req && req->status == REQUEST_INPROGRESS)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, overlapping_replies, allocatet_req_when_in_process");
#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION
        ++hsu_missing_zlp_debug_ctx.overlapping_replies_cnt;
#endif /* FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */
        goto Error;
    }

    /* If this request is being reused, and has been flagged with an error or
     * cancel, we should flag it as ready before using it */
    if (req)
        req->status = REQUEST_READY;

    if (!req || (req->buffer.buffer_size < size))
    {
        request_t *new_req;
        
        /* Dont try to allocate 0 bytes... */
        new_req = core_request_alloc_for_ep0(size ? size : 10, core_ctx);
        if (!new_req)
        {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, error_allocating_req");
            goto Error;
        }

        if (req)
            core_request_free(req);

        req = *p_req = new_req;
    }

    if (buffer && size)
        j_memcpy(req->buffer.vaddr, buffer, size);
    
    req->transfer_size = size;
    return req;

Error:
    return NULL;
}

/**
 * Function name:  cdc_create_ep0_read
 * Description:  allocate a CORE request for a CONTROL read
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @size: (IN) size of read buffer
 *
 * Return value: CORE request, or NULL on failure 
 * Scope: global
 **/
request_t* cdc_create_ep0_read(cdc_ctx_t *ctx, juint32_t size)
{
    /* HSU ADDITION +++ */
    request_t *req = NULL;
    /* HSU ADDITION --- */

    if (!ctx)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, context_is_null");
        return NULL;
    }

    req = create_request_ep0(&ctx->ctrl_read, NULL, size, ctx->core_ctx);
    if (!req)
    {
        /* HSU ADDITION +++ */
        /* error recovery in the event that a payload is queued onto HW */
        /* this implementation is added to handle overlapping reply errors */
        core_abort_pipe(ctx->core_ctx, NULL); /* clear control pipe */
        ctx->ctrl_read->status = REQUEST_READY; /* manually ensure CTRL READ status is ready */
        /* HSU ADDITION --- */
    }
    
    return req;
}

/**
 * Function name:  cdc_create_ep0_write
 * Description: allocate a CORE request for a CONTROL write
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @buffer: (IN) data to send (optional)
 *     @size: (IN) size of data to send
 *     @max_size: (IN) wLength value
 *
 * Return value: CORE request, or NULL on failure 
 * Scope: global
 **/
request_t* cdc_create_ep0_write(cdc_ctx_t *ctx, void *buffer, juint32_t size,
    juint32_t max_size)
{
    request_t *req = NULL;

    if (!ctx)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, context_is_null");
        goto Exit;
    }

    req = create_request_ep0(&ctx->ctrl_write, buffer, MIN(size, max_size), ctx->core_ctx);
    if (!req)
    {
        /* HSU ADDITION +++ */
        /* error recovery in the event that a payload is queued onto HW, but host never picks up or send an ACK */
        /* this implementation is added to handle overlapping reply errors */
        core_abort_pipe(ctx->core_ctx, NULL); /* clear control pipe */
        ctx->ctrl_write->status = REQUEST_READY; /* manually ensure CTRL WRITE status is ready */
        /* HSU ADDITION --- */
        goto Exit;
    }


    /* If the host is expecting more data then we plan to send
     * then we should send a ZLP to tell him that we have sent
     * all we meant to */
    if (max_size > size)
        req->zero = 1;
    else
        req->zero = 0;

Exit:
    return req;
}

#ifdef JDEBUG
static void status_complete(request_t *req)
{
#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION
    cdc_iostatus_t status;
    cdc_ctx_t* ctx;
#ifdef FEATURE_HS_USB_DROP_TX_ZLP_DEBUG_EXP
    static uint32 hsu_debug_tx_zlp_complete_cnt = 0; /* use this counter to arbitrary drop TX ZLP completion */
#endif /* FEATURE_HS_USB_DROP_TX_ZLP_DEBUG_EXP */
#endif /* FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */
    DBG_X(DSLAVE_CDC, ("status_complete: entered\n"));
#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION
    /* many clients use cdc_send_status, must distinguish SEND ENCAP CMD */
    if (req && req->context)
    {
      ctx = (cdc_ctx_t *)req->context;
      if (ctx->encap_response.zlp_client == CDC_ZLP_CLIENT_SEND_ENCAP_CMD)
      {
#ifdef FEATURE_HS_USB_ERR_FATAL_UPON_TIMER_EXPIRY_FOR_RX_TX_ZLP_COMPLETION
        if (hsu_conf_sel_is_ep0_timer_on_rx_tx_zlp_err_chk_enabled())
        {
          hsu_timer_cancel(&(hsu_missing_zlp_debug_ctx.encap_cmd_tx_zlp_timer_id));
        }
#endif /* FEATURE_HS_USB_ERR_FATAL_UPON_TIMER_EXPIRY_FOR_RX_TX_ZLP_COMPLETION */

        HSU_ULOG(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "hsu_tx_zlp");

#ifdef FEATURE_HS_USB_DROP_TX_ZLP_DEBUG_EXP
        if (ctx->desc->interfaces->number == HSU_DEBUG_RMNET_INSTANCE_NUMBER) /* 1st RMNET */
        {
          if (hsu_debug_tx_zlp_complete_cnt >= HSU_DEBUG_LOWER_BOUND_DROP_ZLP_NUM
              && hsu_debug_tx_zlp_complete_cnt <= HSU_DEBUG_UPPER_BOUND_DROP_ZLP_NUM) /* simply drop the TX ZLP and create overlapping reply condition */
          {
            /* hack request such that it's still pending */
            req->status = REQUEST_INPROGRESS;
            req->transfer_size = 0;
            req->bytes_transferred = 0;
            ++hsu_debug_tx_zlp_complete_cnt;
            return;
          }
          ++hsu_debug_tx_zlp_complete_cnt;
        }
#endif /* FEATURE_HS_USB_DROP_TX_ZLP_DEBUG_EXP */

        status = cdc_get_iostatus(req, 0);
        if (status == CDC_TRANSFER_ERROR)
        {
          ++hsu_missing_zlp_debug_ctx.cdc_status_complete__transfer_error_cnt;
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, cdc_TX_ZLP_req_status_failure %d", status);
#ifdef FEATURE_HS_USB_DEBUG_MISSING_ZLP
          if (hsu_conf_sel_is_ep0_aggressive_err_chk_enabled())
          {
            HSU_ERR_FATAL("status_complete: CDC TX ZLP req status failed. status=%d req=%u\n", status, req, 0);
          }
#endif /* FEATURE_HS_USB_DEBUG_MISSING_ZLP */
        }
        /* else if (status == CDC_TRANSFER_OK || status == CDC_TRANSFER_CANCELLED) do nothing */
        ++hsu_missing_zlp_debug_ctx.status_complete_cnt;
      }
    }
#endif /* FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */
}
#endif

/**
 * Function name:  cdc_send_status
 * Description:  issue a status packet by sending a zero length packet on ep0
 * Parameters: 
 *     @ctx: (IN) CDC context
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_send_status(cdc_ctx_t *ctx)
{
    jresult_t rc = -1;
    request_t *reply;

    DBG_X(DSLAVE_CDC, ("%s: entered\n", __func__));

    reply = cdc_create_ep0_write(ctx, NULL, 0, 0);
    if (!reply)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, failed_to_setup_status_req");
#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION
        if (ctx->encap_response.zlp_client == CDC_ZLP_CLIENT_SEND_ENCAP_CMD)
        {
          hsu_missing_zlp_debug_ctx.fail_to_create_ep0_write_cnt++;
        }
#ifdef FEATURE_HS_USB_DEBUG_MISSING_ZLP
        if (hsu_conf_sel_is_ep0_aggressive_err_chk_enabled())
        {
          HSU_ERR_FATAL("cdc_send_status: failed to setup status request. ctx=%u\n", ctx, 0, 0);
        }
#endif /* FEATURE_HS_USB_DEBUG_MISSING_ZLP */
#endif /* FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */
        /* Manually recover from missing ZLP completion error;
         *  Otherwise, it would fail at all subsequent create_request of ZLPs.
         *  Also had to ensure recovery mechanism applies to legacy non-ZLP implementation
         */
        core_abort_pipe(ctx->core_ctx, NULL); /* clear control pipe, it does not propagate back upto AL layer */
        ctx->ctrl_write->status = REQUEST_READY;

        return rc;
    }

#if defined(JDEBUG) || defined(FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION)
    reply->complete = status_complete;
    reply->context = ctx;
#else
    reply->complete = NULL;
#endif

#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION
    if (ctx->encap_response.zlp_client == CDC_ZLP_CLIENT_SEND_ENCAP_CMD)
    {

#ifdef FEATURE_HS_USB_ERR_FATAL_UPON_TIMER_EXPIRY_FOR_RX_TX_ZLP_COMPLETION 
      if (hsu_conf_sel_is_ep0_timer_on_rx_tx_zlp_err_chk_enabled())
      {
        hsu_timer_set(&(hsu_missing_zlp_debug_ctx.encap_cmd_tx_zlp_timer_id), HSU_DEBUG_CDC_TX_ZLP_TIMER_VAL_MS, T_MSEC);
      }
#endif /* FEATURE_HS_USB_ERR_FATAL_UPON_TIMER_EXPIRY_FOR_RX_TX_ZLP_COMPLETION */
    }
#endif /* FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */

    rc = core_send_data(ctx->core_ctx, 0, reply);
    if (rc < 0)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, failed_to_send_reply");
#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION
        if (ctx->encap_response.zlp_client == CDC_ZLP_CLIENT_SEND_ENCAP_CMD)
        {
          hsu_missing_zlp_debug_ctx.fail_to_send_status_cnt++;
        }
#endif /* FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */
#ifdef FEATURE_HS_USB_DEBUG_MISSING_ZLP
        if (hsu_conf_sel_is_ep0_timer_on_rx_tx_zlp_err_chk_enabled())
        {
          HSU_ERR_FATAL("cdc_send_status: failed to send reply. rc=%u ctx=%u\n", rc, ctx, 0);
        }
#endif /* FEATURE_HS_USB_DEBUG_MISSING_ZLP */
    }
    
    return rc;
}

/**
 * Function name:  encapsulated_command_complete
 * Description:  completion handler for the SEND_ENCAPSULATED_COMMAND request
 * Parameters: 
 *     @req: (IN) CORE request
 *
 * Scope: local
 **/
static void encapsulated_command_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    cdc_callbacks_t *cbs = &ctx->cbs.basic;
    cdc_iostatus_t status;
    jresult_t rc;
    
    DBG_X(DSLAVE_CDC, ("%s: entered\n", __func__));

    status = cdc_get_iostatus(req, 1);
#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION
    if (status == CDC_TRANSFER_ERROR)
    {
        /* HS-USB Addition
        ** Changed "goto Error" which stalls endpoint, to "return".
        ** In case, transfer is incomplete on encap message, if the
        ** STALL is forced, then the transfer keeps going into loop
        ** of Device replying STALL to OUT token indefinately.
        */
        /* goto Error; */
        ++hsu_missing_zlp_debug_ctx.cdc_get_io_status_failure_cnt;
        HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "cdc_transfer_err, byte_transferred %u", (juint16_t)req->bytes_transferred);
#ifdef FEATURE_HS_USB_DEBUG_MISSING_ZLP
        if (hsu_conf_sel_is_ep0_timer_on_rx_tx_zlp_err_chk_enabled())
        {
          HSU_ERR_FATAL("encapsulated_command_complete: payload/data stage of CDC ENCAP CMD failed. ctx=%u, req=%u\n", ctx, req, 0);
        }
#endif /* FEATURE_HS_USB_DEBUG_MISSING_ZLP */
        return;
        /* end of HS-USB Addition */
    }
    else if (status == CDC_TRANSFER_CANCELLED)
    {
        /* it does not make sense to send ZLP if payload stage is cancelled */
        HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "status, cdc_transfer_cancelled, byte_transferred %u", (juint16_t)req->bytes_transferred);
        return;
    }
    /* else if (status == CDC_TRANSFER_OK) do nothing */
#else /* FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */
    if (status != CDC_TRANSFER_OK)
    {
        /* HS-USB Addition
        ** Changed "goto Error" which stalls endpoint, to "return".
        ** In case, transfer is incomplete on encap message, if the
        ** STALL is forced, then the transfer keeps going into loop
        ** of Device replying STALL to OUT token indefinately.
        */
        /* goto Error; */
        HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "cdc_transfer_not_ok, byte_transferred %u", (juint16_t)req->bytes_transferred);
        return;
        /* end of HS-USB Addition */
    }
#endif /* ~FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */
    HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "byte_transferred %u", (juint16_t)req->bytes_transferred);

    /* Pass data to application */
    if (cbs->encap_command_h(req->buffer.vaddr, 
        (juint16_t)req->bytes_transferred, ctx->app_ctx) < 0)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, app_failed_to_handle_req");
        goto Error;
    }

#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION
    ++hsu_missing_zlp_debug_ctx.cdc_send_status_cnt;
    ctx->encap_response.zlp_client = CDC_ZLP_CLIENT_SEND_ENCAP_CMD;
#endif /* FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */

    rc = cdc_send_status(ctx);
    if (rc)
    {
        /* keep for now, may relax HSU_ERR_FATAL later */
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC, failed_to_send_status_packet, rc %d", rc);
#ifdef FEATURE_HS_USB_DEBUG_MISSING_ZLP 
        if (hsu_conf_sel_is_ep0_timer_on_rx_tx_zlp_err_chk_enabled())
        {
          HSU_ERR_FATAL("encapsulated_command_complete: failed to send status packet rc=%u, ctx=%u, req=%u\n", rc, ctx, req);
        }
#endif /* FEATURE_HS_USB_DEBUG_MISSING_ZLP */
        goto Error;
    }

    return;
    
Error:
    core_stall_pipe(ctx->core_ctx, 0, 1);
}

/**
 * Function name:  cdc_handle_encapsulated_command
 * Description:  handler for the SEND_ENCAPSULATED_COMMAND request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) SETUP packet
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_handle_encapsulated_command(cdc_ctx_t *ctx, 
    struct usb_ctrlrequest *req)
{

    cdc_callbacks_t *cbs = &ctx->cbs.basic;
    request_t *reply = NULL;
    jresult_t rc;

    DBG_X(DSLAVE_CDC, ("%s: entered\n", __func__));

    if (!cbs->encap_command_h)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, no_handle_defined_for_req");        
        goto Error;
    }

    /* We use the host's commands here as a trigger for 'reminding' it
     * about responses that have not been retrieved yet */
    if (ctx->encap_response.state != CDC_ENCAP_READY)
    {
        hsu_trace_msg2_set(CDC_HANDLE_ENCAPSULATED_COMMAND__REDUN_RES_AVAIL__TRC);

        rc = notify_response_available(ctx);
        if (rc < 0)
        {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, failed_to_send_RESPONSE_AVAILABLE");  
        }

        hsu_trace_msg2_clear();

    }

    /* Setup data stage - get command */
    reply = cdc_create_ep0_read(ctx, le16toh(req->wLength));
    if (!reply)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, failed_to_setup_read_req");  
        goto Error;
    }

#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION
    ++hsu_missing_zlp_debug_ctx.send_encap_cmd_cnt;
#endif /* FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */
    reply->complete = encapsulated_command_complete;
    reply->context = ctx;

    hsu_event_log(EVT_HSU_CDC_HANDLE_ENCAPSULATED_COMMAND, ((core_t*)(ctx->core_ctx))->core_index);
    
    HSU_ULOG_2(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "created_ep0_read, len %u, size %u", le16toh(req->wLength), reply->transfer_size);

    return core_read_data(ctx->core_ctx, 0, reply);
    
Error:
    return -1;
}

#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION
/* A variation of dcd_rx_zlp_complete( ) for ZLDP encap response.  An issue was observed where reset_encap_state()
 * inadvertently cleared the encapsulated response structure when the RX ZLP of a ZLDP encap response completes. */
void dcd_rx_zlp_zldp_complete(request_t *req)
{
  cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
  cdc_iostatus_t status;
#ifdef FEATURE_HS_USB_ERR_FATAL_UPON_TIMER_EXPIRY_FOR_RX_TX_ZLP_COMPLETION
  if (hsu_conf_sel_is_ep0_timer_on_rx_tx_zlp_err_chk_enabled())
  {
    hsu_timer_cancel(&(hsu_missing_zlp_debug_ctx.encap_resp_rx_zlp_timer_id));
  }
#endif /* FEATURE_HS_USB_ERR_FATAL_UPON_TIMER_EXPIRY_FOR_RX_TX_ZLP_COMPLETION */

  hsu_missing_zlp_debug_ctx.rx_zlp_seq_num++;

  status = cdc_get_iostatus(req, 1);

  if (status == CDC_TRANSFER_ERROR)
  {
      /* keep DBG_E for now, may relax HSU_ERR_FATAL later */
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, RX_ZLP_didnt_complete");  
#ifdef FEATURE_HS_USB_DEBUG_MISSING_ZLP
      if (hsu_conf_sel_is_ep0_timer_on_rx_tx_zlp_err_chk_enabled())
      {
        HSU_ERR_FATAL("dcd_rx_zlp_zldp_complete: transfer failed with status %u. ctx=%u\n", status, ctx, 0);
      }
#endif /* FEATURE_HS_USB_DEBUG_MISSING_ZLP */
      goto Done;
  }
  /* else if ((status == CDC_TRANSFER_OK) || (status == CDC TRANSFER_CANCELLED)) do nothing */

  if (ctx->encap_response.state != CDC_ENCAP_RESP_SENDING)
  {
      /* keep DBG_E for now, may relax HSU_ERR_FATAL later */
      HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC, inappropriate_state_for_response,"
        "completion %u", ctx->encap_response.state);  
      status = CDC_TRANSFER_ERROR;
#ifdef FEATURE_HS_USB_DEBUG_MISSING_ZLP
      if (hsu_conf_sel_is_ep0_timer_on_rx_tx_zlp_err_chk_enabled())
      {
        HSU_ERR_FATAL("dcd_rx_zlp_zldp_complete: inappropriate state for response completion %u. ctx=%u\n", ctx->encap_response.state, ctx, 0);
      }
#endif /* FEATURE_HS_USB_DEBUG_MISSING_ZLP */
      goto Done;
  }

Done:
    /* Cleanup, Notify,  and prepare for next response */
    HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "seq_num %u", ctx->encap_response.seq_num);
    /* removed reset_encap_state() here */
}
#endif /* FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */


#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION
void dcd_rx_zlp_complete(request_t *req)
{
  cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
  cdc_iostatus_t status;
#ifdef FEATURE_HS_USB_DROP_RX_ZLP_DEBUG_EXP
  static uint32 hsu_debug_rx_zlp_complete_cnt = 0; /* use this counter to arbitrary drop RX ZLP completion */
#endif /* FEATURE_HS_USB_DROP_RX_ZLP_DEBUG_EXP */

#ifdef FEATURE_HS_USB_ERR_FATAL_UPON_TIMER_EXPIRY_FOR_RX_TX_ZLP_COMPLETION
  if (hsu_conf_sel_is_ep0_timer_on_rx_tx_zlp_err_chk_enabled())
  {
    hsu_timer_cancel(&(hsu_missing_zlp_debug_ctx.encap_resp_rx_zlp_timer_id));
  }
#endif /* FEATURE_HS_USB_ERR_FATAL_UPON_TIMER_EXPIRY_FOR_RX_TX_ZLP_COMPLETION */

  hsu_missing_zlp_debug_ctx.rx_zlp_seq_num++;

#ifdef FEATURE_HS_USB_DROP_RX_ZLP_DEBUG_EXP
  if (ctx->desc->interfaces->number == HSU_DEBUG_RMNET_INSTANCE_NUMBER) /* 1st RMNET */
  {
    if (hsu_debug_rx_zlp_complete_cnt >= HSU_DEBUG_LOWER_BOUND_DROP_ZLP_NUM
        && hsu_debug_rx_zlp_complete_cnt <= HSU_DEBUG_UPPER_BOUND_DROP_ZLP_NUM) /* simply drop RX ZLP and create overlapping reply condition */
    {
      /* hack request such that it's still pending */
      req->status = REQUEST_INPROGRESS;
      req->transfer_size = 0;
      req->bytes_transferred = 0;
    
      HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "seq_num %u", ctx->encap_response.seq_num);
      ++hsu_debug_rx_zlp_complete_cnt;
      return;
    }
    ++hsu_debug_rx_zlp_complete_cnt;
  }
#endif /* FEATURE_HS_USB_DROP_RX_ZLP_DEBUG_EXP */

  status = cdc_get_iostatus(req, 1);

  if (status == CDC_TRANSFER_ERROR)
  {
      /* keep DBG_E for now, may relax HSU_ERR_FATAL later */
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, RX_ZLP_didnt_complete");  
#ifdef FEATURE_HS_USB_DEBUG_MISSING_ZLP
      if (hsu_conf_sel_is_ep0_timer_on_rx_tx_zlp_err_chk_enabled())
      {
        HSU_ERR_FATAL("dcd_rx_zlp_complete: transfer failed with status %u. ctx=%u\n", status, ctx, 0);
      }
#endif /* FEATURE_HS_USB_DEBUG_MISSING_ZLP */
      goto Done;
  }
  /* else if ((status == CDC_TRANSFER_OK) || (status == CDC TRANSFER_CANCELLED)) do nothing */

  if (ctx->encap_response.state != CDC_ENCAP_RESP_SENDING)
  {
      /* keep DBG_E for now, may relax HSU_ERR_FATAL later */
      HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC, inappropriate_state_for_response,"
        "completion %u", ctx->encap_response.state);  
      status = CDC_TRANSFER_ERROR;
#ifdef FEATURE_HS_USB_DEBUG_MISSING_ZLP
      if (hsu_conf_sel_is_ep0_timer_on_rx_tx_zlp_err_chk_enabled())
      {
        HSU_ERR_FATAL("dcd_rx_zlp_complete: inappropriate state for response completion %u. ctx=%u\n", ctx->encap_response.state, ctx, 0);
      }
#endif /* FEATURE_HS_USB_DEBUG_MISSING_ZLP */
      goto Done;
  }

Done:
    /* Cleanup, Notify,  and prepare for next response */
    HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "seq_num %u", ctx->encap_response.seq_num);
    reset_encap_state(ctx, 1, status);
}
#endif /* FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */

/**
 * Function name:  encapsulated_response_complete
 * Description:  completion handler for GET_ENCAPSULATED_RESPONSE data stage
 * Parameters: 
 *     @req: (IN) CORE request
 *     
 * Scope: local
 **/
static void encapsulated_response_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    cdc_iostatus_t status;
#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION
    request_t *rx_zlp_req = NULL;
#endif /* FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */

    DBG_X(DSLAVE_CDC, ("%s: entered\n", __func__));

#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION
    /* Did we just send a ZLDP? */
    if (ctx->encap_response.sending_zldp)
    {
        DBG_X(DSLAVE_CDC, ("%s: ZLDP sent\n", __func__));

        /* Set our state: if there is a real response pending, set our
         * state to SENDING; otherwise get ready for next response */
        if (ctx->encap_response.size)
          ctx->encap_response.state = CDC_ENCAP_RESP_SENDING;
        else
          ctx->encap_response.state = CDC_ENCAP_READY;

        ctx->encap_response.sending_zldp = 0;

        HSU_ULOG(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "ZLDP, entry");

        if (req->status != REQUEST_CANCELLED)
        {
          /* for ZLDP, we need to create a ZLP as well */
          req->ep0_zlp_reply = 0;
  
          /* Must allocate a new request per interface (per CDC ctx).
           * Cannot use the existing one already created by GET ENCAP RESP, as we will need to propoagate its 
           * completion callback after
           */
          rx_zlp_req = cdc_create_ep0_read(ctx, 0);
          if (!rx_zlp_req)
          {
            /* HSU ADDITION +++ */
            /* error recovery in the event that a payload is queued onto HW */
            /* this implementation is added to handle overlapping reply errors */
            /* attempt to recover on the next encapsulated response */
            core_abort_pipe(ctx->core_ctx, NULL); /* clear control pipe, it does not propagate back upto AL layer */
            HSU_ULOG(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "ZLDP, create_ep0_read_failed");

            /* this would contain the last RX ZLP of the last transfer */
            /* Manually recover from missing ZLP completion error;
               otherwise, it would fail at all subsequent create_request of ZLPs */
            ctx->ctrl_read->status = REQUEST_READY;
            reset_encap_state(ctx, 1, CDC_TRANSFER_CANCELLED);
            /* HSU ADDITION --- */
            return;
          }

          rx_zlp_req->complete = dcd_rx_zlp_zldp_complete;
          rx_zlp_req->context = ctx; /* pass context to defer completion of GET ENCAP RESP to RX ZLP */
#ifdef FEATURE_HS_USB_ERR_FATAL_UPON_TIMER_EXPIRY_FOR_RX_TX_ZLP_COMPLETION
          if (hsu_conf_sel_is_ep0_timer_on_rx_tx_zlp_err_chk_enabled())
          {
            hsu_timer_set(&(hsu_missing_zlp_debug_ctx.encap_resp_rx_zlp_timer_id), HSU_DEBUG_CDC_RX_ZLP_TIMER_VAL_MS, T_MSEC);
          }
#endif /* FEATURE_HS_USB_ERR_FATAL_UPON_TIMER_EXPIRY_FOR_RX_TX_ZLP_COMPLETION */
  
          core_read_data(ctx->core_ctx, 0, rx_zlp_req);
        }
        else
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, ZLP_not_created_for_ZLDP");  
          ++hsu_missing_zlp_debug_ctx.zlp_not_created_for_zldp_cnt;
        }
        return;
    }

    ctx->encap_response.bytes_sent += req->bytes_transferred;
   
    /* check status of payload/data stage */
    status = cdc_get_iostatus(req, 1);
    if (status != CDC_TRANSFER_OK)
        goto Done; /* goto cleanup code without preparing to receive ZLP */

    if (ctx->encap_response.state != CDC_ENCAP_RESP_SENDING)
    {
        /* keep DBG_E for now, may relax HSU_ERR_FATAL later */
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC, inappropriate_state_for_response,"
        "completion %u", ctx->encap_response.state);  
        status = CDC_TRANSFER_ERROR;
#ifdef FEATURE_HS_USB_DEBUG_MISSING_ZLP
        if (hsu_conf_sel_is_ep0_aggressive_err_chk_enabled())
        {
          HSU_ERR_FATAL("encapsulated_response_complete: inappropriate state for response completion %u. ctx=%u\n", ctx->encap_response.state, ctx, 0);
        }
#endif /* FEATURE_HS_USB_DEBUG_MISSING_ZLP */
        goto Done;
    }

    DBG_X(DSLAVE_CDC, ("%s: transfered %u bytes. total: %u [out of %u] bytes.\n", __func__,
        req->bytes_transferred, ctx->encap_response.bytes_sent, ctx->encap_response.size));
        
    /* Are we done? */
    if (ctx->encap_response.bytes_sent >= ctx->encap_response.size)
    {
      if (ctx->encap_response.zlp_client == CDC_ZLP_CLIENT_GET_ENCAP_RESP)
      {
        /* defer resetting encap state machine since device must wait for RX ZLP */
        HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "seq_num %u", ctx->encap_response.seq_num);
        req->ep0_zlp_reply = 0;
  
        /* Must allocate a new request per interface (per CDC ctx).
         * Cannot use the existing one already created by GET ENCAP RESP, as we will need to propoagate its 
         * completion callback after
         */
        rx_zlp_req = cdc_create_ep0_read(ctx, 0);
        if (!rx_zlp_req)
        {
          /* HSU ADDITION +++ */
          /* error recovery in the event that a payload is queued onto HW */
          /* this implementation is added to handle overlapping reply errors */
          /* attempt to recover on the next encapsulated response */
          core_abort_pipe(ctx->core_ctx, NULL); /* clear control pipe, it does not propagate back upto AL layer */
          HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "create_ep0_read_failed, seq_num %u", ctx->encap_response.seq_num);

          /* this would contain the last RX ZLP of the last transfer */
          /* Manually recover from missing ZLP completion error;
             otherwise, it would fail at all subsequent create_request of ZLPs */
          ctx->ctrl_read->status = REQUEST_READY;
          reset_encap_state(ctx, 1, CDC_TRANSFER_CANCELLED);
          /* HSU ADDITION --- */
          return;
        }

        rx_zlp_req->complete = dcd_rx_zlp_complete;
        rx_zlp_req->context = ctx; /* pass context to defer completion of GET ENCAP RESP to RX ZLP */
#ifdef FEATURE_HS_USB_ERR_FATAL_UPON_TIMER_EXPIRY_FOR_RX_TX_ZLP_COMPLETION
        if (hsu_conf_sel_is_ep0_timer_on_rx_tx_zlp_err_chk_enabled())
        {
          hsu_timer_set(&(hsu_missing_zlp_debug_ctx.encap_resp_rx_zlp_timer_id), HSU_DEBUG_CDC_RX_ZLP_TIMER_VAL_MS, T_MSEC);
        }
#endif /* FEATURE_HS_USB_ERR_FATAL_UPON_TIMER_EXPIRY_FOR_RX_TX_ZLP_COMPLETION */

        core_read_data(ctx->core_ctx, 0, rx_zlp_req);
      }
      else
      {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC, client_type_mismatch %u", ctx->encap_response.zlp_client);  
#ifdef FEATURE_HS_USB_DEBUG_MISSING_ZLP
        if (hsu_conf_sel_is_ep0_aggressive_err_chk_enabled())
        {
          HSU_ERR_FATAL("encapsulated_response_complete: client type mismatch. zlp_client=%u. ctx=%u\n", ctx->encap_response.zlp_client, ctx, 0);
        }
#endif /* FEATURE_HS_USB_DEBUG_MISSING_ZLP */
      }
    }
    else
    {
      /* do not mark completion of encapsulated_response_complete, return immediately */
      HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "insufficient_byte_sent, seq_num %u", ctx->encap_response.seq_num);
      ++hsu_missing_zlp_debug_ctx.encap_response_complete_insufficient_bytes_sent_cnt;
    }
    return;
    
Done:
    /* Cleanup, Notify,  and prepare for next response */
    HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "clean_up, seq_num %u", ctx->encap_response.seq_num);
    reset_encap_state(ctx, 1, status);
#else /* ~FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */
    /* Did we just send a ZLDP? */
    if (ctx->encap_response.sending_zldp)
    {
        DBG_X(DSLAVE_CDC, ("%s: ZLDP sent\n", __func__));

        /* Set our state: if there is a real response pending, set our
         * state to SENDING; otherwise get ready for next response */
        if (ctx->encap_response.size)
            ctx->encap_response.state = CDC_ENCAP_RESP_SENDING;
        else
            ctx->encap_response.state = CDC_ENCAP_READY;

        ctx->encap_response.sending_zldp = 0;

        return;
    }

    /* bytes_transferred is valid even when errors occur */
    ctx->encap_response.bytes_sent += req->bytes_transferred;

    status = cdc_get_iostatus(req, 1);
    if (status != CDC_TRANSFER_OK)
        goto Done;

    if (ctx->encap_response.state != CDC_ENCAP_RESP_SENDING)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC, inappropriate_state_for_response,"
        "completion %u", ctx->encap_response.state);
        status = CDC_TRANSFER_ERROR;
        goto Done;
    }

    DBG_X(DSLAVE_CDC, ("%s: transfered %u bytes. total: %u [out of %u] bytes.\n", __func__,
        req->bytes_transferred, ctx->encap_response.bytes_sent, ctx->encap_response.size));
        
    /* Are we done? */
    if (ctx->encap_response.bytes_sent >= ctx->encap_response.size)
        goto Done;

    return;
    
Done:
    /* Cleanup, Notify,  and prepare for next response */
    HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "seq_num %u", ctx->encap_response.seq_num);
    reset_encap_state(ctx, 1, status);
#endif /* ~FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */
}

/**
 * Function name:  cdc_handle_encapsulated_response
 * Description:  handler for GET_ENCAPSULATED_RESPONSE request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) SETUP packet
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_handle_encapsulated_response(cdc_ctx_t *ctx, 
    struct usb_ctrlrequest *req)
{
    request_t *reply = NULL;
    juint32_t bytes_to_send;
    juint8_t  *buf = (juint8_t*)ctx->encap_response.buffer;
    juint32_t max_transfer_size = le16toh(req->wLength);
    jresult_t rc;

    DBG_X(DSLAVE_CDC, ("%s: entered\n", __func__));

    /* WMCD, section 7: The host is allowed to poll for a response.
       If there is no response pending, send a zero length reply */
    if (ctx->encap_response.state == CDC_ENCAP_READY)
    {
        HSU_ULOG(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, ", no_pending_response, sending_ZLP");
        buf = NULL;
    }

    /* Send as much as we can, starting where we left off */
    bytes_to_send = MIN(max_transfer_size,
        ctx->encap_response.size - ctx->encap_response.bytes_sent);
    
    /* Send pending response to host - i.e. setup reply */
    hsu_event_log(EVT_HSU_CDC_HANDLE_ENCAPSULATED_RESPONSE, ((core_t*)(ctx->core_ctx))->core_index);
    HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "seq_num %u", ctx->encap_response.seq_num);

    reply = cdc_create_ep0_write(ctx, buf + ctx->encap_response.bytes_sent,
        bytes_to_send, max_transfer_size);
    if (!reply)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, failed_to_setup_reply");
        goto Error;
    }

    /* If this is an empty data phase (reply), we need to mark it as such */
    if (!buf)
    {
        ctx->encap_response.sending_zldp = 1;
        reply->ep0_zlp_reply = 1;
    }

    reply->complete = encapsulated_response_complete;
    reply->context = ctx;

#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION
    ctx->encap_response.zlp_client = CDC_ZLP_CLIENT_GET_ENCAP_RESP;
#endif /* FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */
    //hsu_qmi_data_log(ctx->desc->interfaces->number, reply->buffer.vaddr, reply->transfer_size);
    HSU_ULOG_DATA_5((hsu_ulog_log_type_enum)ctx->desc->interfaces->number, reply->buffer.vaddr, reply->transfer_size);
    rc = core_send_data(ctx->core_ctx, 0, reply);
    if (rc)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, failed_to_send_reply");
        goto Error;
    }   

    return 0;

Error:
    /* Cleanup, Notify, and prepare for next response */
    reset_encap_state(ctx, 1, CDC_TRANSFER_ERROR);
    ctx->encap_response.sending_zldp = 0;
    return -1;
}

/*====================================================
 *   Control class - Device to Host (Notifications)
 *===================================================*/

/**
 * Function name:  cdc_create_notification
 * Description:  allocate a CORE request for generating a notification, and fill
 *               out its header (i.e. SETUP packet format)
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @bNotification: (IN) 8-bit notification code
 *     @wValue: (IN) 16-bit wValue (as defined by CDC)
 *     @wIndex: (IN) 16-bit wIndex (as defined by CDC)
 *     @wLength: (IN) 16-bit wLength (as defined by CDC)
 *     @Data: (IN) notification data
 *
 * Return value: CORE request, or NULL on failure 
 * Scope: global
 **/
request_t* cdc_create_notification(cdc_ctx_t *ctx, juint8_t bNotification,
    juint16_t wValue, juint16_t wIndex, juint16_t wLength, juint8_t *Data)
{
    struct usb_ctrlrequest *ntf_hdr = NULL;
    request_t *req = NULL;
    juint8_t *ntf_data;
    juint32_t transfer_size = sizeof(struct usb_ctrlrequest)+wLength;

    if (!ctx)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, contex_is_null");
        return NULL;
    }

    req = core_request_alloc(transfer_size, NULL);
    if (!req)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, error_allocating_interrupt_req");
        return NULL;
    }

    req->zero = 0;
    req->transfer_size = transfer_size;

    ntf_hdr = (struct usb_ctrlrequest*)(req->buffer.vaddr);
    ntf_hdr->bRequestType = USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE;
    ntf_hdr->bRequest = bNotification;
    ntf_hdr->wValue = htole16(wValue);
    ntf_hdr->wIndex = htole16(wIndex);
    ntf_hdr->wLength = htole16(wLength);

    if (wLength && Data)
    {
        ntf_data = (juint8_t*)(ntf_hdr+1);
        j_memcpy(ntf_data, Data, wLength);
    }
    
    return req;
}

/**
 * Function name:  response_available_complete
 * Description:  completion handler for the RESPONSE_AVAILABLE notification
 * Parameters: 
 *     @req: (IN) CORE request
 *
 * Scope: local
 **/
static void response_available_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t *)req->context;

    /* HSU Addition - moved from inside JDEBUG */
    cdc_iostatus_t status = cdc_get_iostatus(req, 1);


    DBG_X(DSLAVE_CDC, ("%s: entered\n", __func__));

    ctx->encap_response.notification_pending = 0;

    /* HSU Addition */
    switch(status)
    {
      case CDC_TRANSFER_OK:
        HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "seq_num %u", ctx->encap_response.seq_num);
        break;

      case CDC_TRANSFER_ERROR:
        HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "with_ERROR, seq_num %u", ctx->encap_response.seq_num);        
#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET 
        /* Notify app to resend the payload */
        reset_encap_state(ctx, TRUE, status);
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
        break;

      case CDC_TRANSFER_CANCELLED:
        HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "with_cancelled, seq_num %u", ctx->encap_response.seq_num);        
#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET 
        /* Notify app to resend the payload */
        reset_encap_state(ctx, TRUE, status);
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
        break;

      default:        
        HSU_ERR_FATAL("response_available_complete: unknown status=%u, ctx=%u\n", status, ctx, 0);
    }    
    /* End of HSU Addition */

#ifdef JDEBUG
    {
        if (status != CDC_TRANSFER_OK && status != CDC_TRANSFER_CANCELLED)
        {
            HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC, error_sending_RESPONSE_AVAILABLE, notification %u", status);
        }
    }
#endif

    core_request_free(req);
    return;
}

/**
 * Function name:  notify_response_available
 * Description:  send the RESPONSE_AVAILABLE notification.
 *               this notification is not exported as part of the API.
 *               It is called implicitly by cdc_send_encap_response().
 * Parameters: 
 *     @ctx: (IN) CDC context
 *
 * Return value: 0 for success, or an error code 
 * Scope: local
 **/
static jresult_t notify_response_available(cdc_ctx_t *ctx)
{
    request_t *req = NULL;
    jresult_t rc = 0;
#ifdef FEATURE_HS_USB_ECM_INT_EP_SHARING    
    cdc_ctx_t *shared_intr_ctx = NULL;
#endif /* FEATURE_HS_USB_ECM_INT_EP_SHARING */

    // HSU addition
    #ifdef FEATURE_HS_USB_INTR_ONLY_ECM_PIPE
    /* If this is INTR only pipe then ctx->in_pipe.desc will be NULL, so in this case 
        we have to use intr_pipe.desc to validate the ctx. We cannot make this as default
        as intr.desc will be absent incase we are using endpoint sharing */
    
    juint16_t if_num = 0;
    
    if(ctx->info.basic.flags & CDC_INIT_INTR_ONLY_ECM_PIPE)
    {
        if (ctx->intr_pipe.desc == NULL)
        {
            rc = JENXIO;
            goto Exit;
        }
        
        if_num = ctx->intr_pipe.if_num;
    }
    else
    #endif /* FEATURE_HS_USB_INTR_ONLY_ECM_PIPE */      
    {
      if (ctx->in_pipe.desc == NULL)
      {
          rc = JENXIO;
          goto Exit;
      }
        if_num = ctx->in_pipe.if_num;
    }

    if (ctx->encap_response.notification_pending)
        goto Exit;

#ifdef FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM
    /* Send sequnce number as part of notification */
    req = cdc_create_notification(ctx, CDC_NTF_RESPONSE_AVAILABLE, 0,
        if_num, sizeof(juint32_t), 
        (juint8_t*)&(ctx->encap_response.seq_num));
#else /* !FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM */
    req = cdc_create_notification(ctx, CDC_NTF_RESPONSE_AVAILABLE, 0,
        if_num, 0, NULL);
#endif /* FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM */

    if (!req)
    {
        rc = JENOMEM;
        goto Exit;
    }
    
    req->complete = response_available_complete;
    req->context = ctx;

#ifdef FEATURE_HS_USB_ECM_INT_EP_SHARING    
    if(hsu_config_selector_is_ecm_int_ep_sharing_supported())
    {
        /* Since we have only 1 intr_pipe we need to get its handle
        * so we can send the Notify Resposnse Available to it with 
        * interface number that we need it for */
        shared_intr_ctx = (cdc_ctx_t*)hsu_al_ecm_get_shared_intr_pipe_handle();
        
        if (shared_intr_ctx->intr_pipe.desc == NULL)
        {
            rc = JENXIO;
            goto Exit;
        }

        hsu_event_log(EVT_HSU_NOTIFY_RESPONSE_AVAILABLE_WITH_INT_EP_SHARING, ((core_t*)(ctx->core_ctx))->core_index);
        HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)if_num, "int_ep_sharing, seq_num %u", ctx->encap_response.seq_num);

        rc = core_send_data(ctx->core_ctx, shared_intr_ctx->intr_pipe.desc, req);
    }
    else
#endif /* FEATURE_HS_USB_ECM_INT_EP_SHARING */
    {
        if (ctx->intr_pipe.desc == NULL)
        {
            rc = JENXIO;
            goto Exit;
        }

        hsu_event_log(EVT_HSU_NOTIFY_RESPONSE_AVAILABLE, ((core_t*)(ctx->core_ctx))->core_index);
        HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)if_num, "seq_num %u", ctx->encap_response.seq_num);

        rc = core_send_data(ctx->core_ctx, ctx->intr_pipe.desc, req);
    }

Exit:
    if (rc)
      {
        if(req)
          core_request_free(req);
      }
    else                  
        ctx->encap_response.notification_pending = 1;

    return rc;
}

/* 
** HSU ADDITION
*/
/**
 * Function name:  cdc_resend_response_available
 * Description:  Control Class API function.   This API will resend the
 *               Response Available in case host has not received previous
 *               notification.
 *
 * Parameters: 
 *     @handle: (IN) CDC handle 
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_resend_response_available(cdc_handle_t handle)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
    jresult_t rc = 0;
    
    DBG_X(DSLAVE_CDC, ("%s: entered\n", __func__));

    /* Check state for readiness */
    if (ctx->encap_response.state == CDC_ENCAP_READY)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, encap_state_is_already_READY");
      return JEINVAL;
    }

    /* If the application gets impatient, we 'urge' the host to
     * retrieve the response */
    rc = notify_response_available(ctx);
    if (rc < 0)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC, failed_to_send_RESPONSE_AVAILABLE, notification_rc %d", rc);
    }

    return rc;
}
/* 
** End 0f HSU ADDITION
*/


/**
 * Function name:  cdc_send_encap_response
 * Description:  Control Class API function.
 *               This function sends a response for an encapsulated command
 *               received from the host.
 *               Note: queueing is not supported - a second call will return
 *               JEBUSY.
 * Parameters: 
 *     @handle: (IN) CDC handle 
 *     @response: (IN) buffer containing the response
 *     @size: (IN) size of the response
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_send_encap_response(cdc_handle_t handle, void *response,
    juint32_t size)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
    jresult_t rc = 0;
    
    DBG_X(DSLAVE_CDC, ("%s: entered\n", __func__));

    if (!size)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, error_size_equal_zero");
        return JEINVAL;
    }

    /* Check state for readiness */
    if (ctx->encap_response.state != CDC_ENCAP_READY)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC, busy, response_already_in_progress, state %u", ctx->encap_response.state);

        /* If the application gets impatient, we 'urge' the host to
         * retrieve the response */
        rc = notify_response_available(ctx);
        if (rc < 0)
        {
            HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC, failed_to_send_RESPONSE_AVAILABLE, notification_rc %d", rc);
        }

        return JEBUSY;
    }

    KASSERT(!ctx->encap_response.buffer, ("Buffer already allocated!"));

    DBG_I(DSLAVE_CDC, ("%s: Allocating buffer - [%d] bytes\n",
        __func__, size));

    /* allocating response buffer to the size of the response */
    ctx->encap_response.buffer = jmalloc(size, 0);

    if (!ctx->encap_response.buffer)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC, failed_to_allocate_response_buffer");
        rc = JENOMEM;
        goto Error;
    }
    
    /* Hang response on our ctx, so that GET_ENCAPSULATED_RESPONSE can use it */
#ifdef FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM
    /* First 4-bytes are juint32_t type sequence number */
    ctx->encap_response.seq_num = *( (juint32_t*) response );

    /* Payload starts after skipping 4 bytes */
    j_memcpy(ctx->encap_response.buffer, 
      (juint8_t*)response + sizeof(juint32_t), size);
#else /* FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM */
    j_memcpy(ctx->encap_response.buffer, response, size);
#endif /* FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM */
    ctx->encap_response.size = size;
    ctx->encap_response.bytes_sent = 0;
    ctx->encap_response.state = CDC_ENCAP_RESP_SENDING;
    
    /* Send RESPONSE_AVAILABLE notification */
    rc = notify_response_available(ctx);
    if (rc < 0)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC, failed_to_send_RESPONSE_AVAILABLE, notification_rc %d", rc);
        goto Error;
    }

    return 0;

Error:
    /* In the event of an error (busy != error), we need to clean our state
     * to prepare for the next attempt */
    reset_encap_state(ctx, 0, CDC_TRANSFER_OK);
    return rc;
}

/*==========================================
 *    Initialization - internal functions
 *=========================================*/

/**
 * Function name:  cdc_free_descriptors
 * Description:  release memory allocated by cdc_alloc_descriptors()
 * Parameters: 
 *     @ctx: (IN) CDC context
 *
 *  Scope: global
 **/
void cdc_free_descriptors(cdc_ctx_t *ctx)
{
    if (ctx && ctx->desc)
    {
        juint8_t i;

        for (i = 0; i < ctx->desc->interface_count; i++)
        {
            if (ctx->desc->interfaces[i].if_string)
            {
                core_release_string_descriptor(ctx->core_ctx,
                    ctx->desc->interfaces[i].if_string);
            }
        }

        jfree(ctx->desc);
        ctx->desc = NULL;
    }
}

static jresult_t cdc_alloc_descriptors_mult(cdc_ctx_t *ctx, 
    cdc_desc_init_t *init_info);
static jresult_t cdc_alloc_descriptors_single(cdc_ctx_t *ctx, 
    cdc_desc_init_t *init_info);
/**
 * Function name:  cdc_alloc_descriptors
 * Description:  allocate memory for CDC descriptors, and fill them out.
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @init_info: (IN) information required to fill out CDC descriptors
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_alloc_descriptors(cdc_ctx_t *ctx, cdc_desc_init_t *init_info)
{
    jresult_t rc;

    if (!ctx || ctx->desc)
        return  JEINVAL;

    if (init_info->is_single_iface)
        rc = cdc_alloc_descriptors_single(ctx, init_info);
    else 
        rc = cdc_alloc_descriptors_mult(ctx, init_info);

    return rc;
}

/**
 * Function name:  cdc_alloc_descriptors_single
 * Description:  allocate memory for CDC descriptors, and fill them out.
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @init_info: (IN) information required to fill out CDC descriptors
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t cdc_alloc_descriptors_single(cdc_ctx_t *ctx, 
    cdc_desc_init_t *init_info)
{
    pipe_desc_t         *pipes = NULL;
    pipe_desc_t         *intr_pipe = NULL;
    pipe_desc_t         *data_pipes = NULL;
    alt_interface_desc_t *alts = NULL;
    interface_desc_t    *iface = NULL;
    fd_desc_t           *desc = NULL;
    void                *ex_desc = NULL;
    void                *curr = NULL;
    juint32_t            size;
    juint32_t            ex_desc_size = 0;
    juint8_t             num_alts = init_info->dc_empty_alt_0 ? 2 : 1;
    juint8_t             num_pipes = 0;
    jresult_t            rc = JENOMEM;
    juint8_t interface_number;
    hsu_platform_info_type const * platform_info = hsu_platform_get_info();

    if (init_info->interface_number == UWD_UNDEFINED_INTERFACE)
        interface_number = core_get_available_iface(ctx->core_ctx);
    else
        interface_number = init_info->interface_number;
    
    size = sizeof(fd_desc_t) +          /* desc */
        sizeof(alt_interface_desc_t)*num_alts +  /* alt */
        sizeof(interface_desc_t);       /* iface (iface) */
    
    if (init_info->get_func_headers)
    {
        ex_desc_size = init_info->get_func_headers(ctx, NULL, TRUE, 
            interface_number);
        size += ex_desc_size;
    }

    if (init_info->cc_ntf_supported)
        size += sizeof(pipe_desc_t);    /* intr pipe */
    
    if (init_info->dc_in_supported)
        size += sizeof(pipe_desc_t); /* In data pipe*/
    
    if (init_info->dc_out_supported)
        size += sizeof(pipe_desc_t); /* Out data pipes*/
    
    curr = jmalloc(size, M_ZERO);
    if (!curr)
        goto Exit;

    /* Make room for FD descriptor */
    desc = (fd_desc_t*)curr;
    curr = desc+1;

    pipes = (pipe_desc_t*)curr;
    if (init_info->cc_ntf_supported)
    {
        /* Setup interrupt pipe for notifications */
        intr_pipe = &pipes[0];
        // HSU addition
        curr = intr_pipe+1;
        intr_pipe->ss_attributes.max_burst      = USB_SUPERSPEED_INTR_EP_MAXBURST;
        intr_pipe->max_pkt_size_super           = get_max_packet_size(SPEED_SUPER);;
        intr_pipe->max_pkt_size_high            = 64;
        intr_pipe->max_pkt_size_full            = 64;
        intr_pipe->type                         = PIPE_INTR;
        intr_pipe->direction                    = DIRECTION_IN;
        intr_pipe->sync_type                    = 0;
        intr_pipe->usage_type                   = 0;
        intr_pipe->address                      = 0; /* Filled by Core */
        intr_pipe->transaction_per_microframe   = 0;
        intr_pipe->rate_u.poll_interval         = 5; /* 5ms in FS, 2ms in HS */
        num_pipes++; 
    }

    if (init_info->dc_in_supported)
    {
        data_pipes = &pipes[num_pipes];
        curr = data_pipes+1;
          data_pipes->ss_attributes.max_burst = USB_SUPERSPEED_BULK_EP_MAXBURST;
        data_pipes->max_pkt_size_super= get_max_packet_size(SPEED_SUPER);
        data_pipes->max_pkt_size_high = get_max_packet_size(SPEED_HIGH);
        data_pipes->max_pkt_size_full = get_max_packet_size(SPEED_FULL);
        data_pipes->type              = PIPE_BULK;
        data_pipes->direction         = DIRECTION_IN;
        data_pipes->sync_type         = 0;
        data_pipes->usage_type        = 0;
        data_pipes->address           = 0; /* Filled by Core */
        data_pipes->transaction_per_microframe = 0;
        data_pipes->rate_u.nak_rate   = 32; /* HS: up to 1 NAK per 4 ms */
        num_pipes++;
	}

	if (init_info->dc_out_supported)
	{
        data_pipes = &pipes[num_pipes];
        curr = data_pipes+1;
          data_pipes->ss_attributes.max_burst = USB_SUPERSPEED_BULK_EP_MAXBURST;
        data_pipes->max_pkt_size_super= get_max_packet_size(SPEED_SUPER);		
        data_pipes->max_pkt_size_high = get_max_packet_size(SPEED_HIGH);
        data_pipes->max_pkt_size_full = get_max_packet_size(SPEED_FULL);
        data_pipes->type              = PIPE_BULK;
        data_pipes->direction         = DIRECTION_OUT;
        data_pipes->sync_type         = 0;
        data_pipes->usage_type        = 0;
        data_pipes->address           = 0; /* Filled by Core */
        data_pipes->transaction_per_microframe = 0;
        data_pipes->rate_u.nak_rate   = 32; /* HS: up to 1 NAK per 4 ms */
        num_pipes++;
    }

    alts = (alt_interface_desc_t*)curr;

    curr = alts + num_alts;
    if (init_info->dc_empty_alt_0)
    {
        if (init_info->cc_ntf_supported)
        {
            alts[0].pipe_count = 1;  /* Alt setting without data */
            alts[0].pipes = pipes;
        }
        else 
        {
            alts[0].pipe_count = 0;  /* Empty alt setting */
            alts[0].pipes = NULL;
        }
        alts[1].pipe_count = num_pipes;  /* Data capabale alt setting */
        alts[1].pipes = pipes;
    }
    else
    {
        alts[0].pipe_count = num_pipes;  /* alt setting */
        alts[0].pipes = pipes;
    }

    /* Setup interface descriptors */
    iface = (interface_desc_t*)curr;
    curr = iface+1;

    iface->control_msg               = init_info->cc_handler;
    iface->alt_ifs                   = alts;
    iface->alt_if_num                = num_alts;
    iface->alt_if_curr               = 0;
    iface->extra_descriptor          = NULL;
    iface->extra_descriptor_size     = 0;
    iface->number                    = init_info->interface_number;
    /* HSU addition: HSU_STR_IDX_NOT_SET */
    iface->if_string = core_add_string_descriptor(ctx->core_ctx,
      ctx->info.basic.data_string, HSU_STR_IDX_NOT_SET);
    /* HSU addition end */
   
    if (ctx->info.basic.flags & CDC_INIT_VENDOR_SPECIFIC_CLASS)
        iface->if_class               = USB_CLASS_VENDOR_SPEC;
    else
        iface->if_class               = USB_CLASS_COMM;

    if (init_info->cci_subclass)
    {
        iface->if_subclass            = init_info->cci_subclass;
        iface->if_protocol            = init_info->cci_protocol;
    }
    else
    {
        iface->if_subclass            = USB_CLASS_VENDOR_SPEC;
        iface->if_protocol            = USB_CLASS_VENDOR_SPEC;        
    }
    
    /* Setup extra (class specific) descriptor */
    if (init_info->get_func_headers)
    {
        ex_desc = curr;
        curr = (char*)ex_desc + ex_desc_size;
        init_info->get_func_headers(ctx, ex_desc, TRUE, interface_number);
        iface->extra_descriptor = ex_desc;
        iface->extra_descriptor_size = ex_desc_size;
    }

    /* Setup FD descriptor */
    switch (platform_info->hsu_platform_id)
    {
      case HSU_PLATFORM_VIRTIO:
      case HSU_PLATFORM_RUMI:
      {
        desc->speed = SPEED_HIGH;
      }
      break;
      default:
      {
        if (hsu_clkmgr_is_hs_only_mode())
        {
          desc->speed = SPEED_HIGH;
        }
        else
        {
          desc->speed = SPEED_SUPER;
        }
      }
      break;
    }
    desc->interface_count = 1;
    desc->interfaces = iface;
    desc->iad = NULL;
    ctx->desc = desc;

    rc = 0;
Exit:
    
    return rc;
}

/**
 * Function name:  cdc_alloc_descriptors_mult
 * Description:  allocate memory for CDC descriptors, and fill them out.
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @init_info: (IN) information required to fill out CDC descriptors
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t cdc_alloc_descriptors_mult(cdc_ctx_t *ctx, 
    cdc_desc_init_t *init_info)
{
    pipe_desc_t         *intr_pipe = NULL;
    pipe_desc_t         *data_pipes = NULL;
    pipe_desc_t         *curr_data_pipe = NULL;

    alt_interface_desc_t *cc_alt = NULL;
    alt_interface_desc_t *dc_alts = NULL;
    interface_desc_t    *ifaces = NULL;
    struct usb_interface_assoc_descriptor *iad = NULL;
    fd_desc_t           *desc = NULL;
    void                *ex_desc = NULL;
    void                *curr = NULL;
    juint32_t            size;
    juint32_t            ex_desc_size = 0;
    juint8_t             dc_num_alts = init_info->dc_empty_alt_0 ? 2 : 1;
    juint8_t interface_number, next_interface;
    juint8_t num_of_data_pipe_present = 0;
    hsu_platform_info_type const *platform_info = hsu_platform_get_info();
  
    if(init_info->dc_in_supported)
      num_of_data_pipe_present++;
    if(init_info->dc_out_supported)
      num_of_data_pipe_present++;
  

    if (init_info->interface_number == UWD_UNDEFINED_INTERFACE)
    {
        interface_number = core_get_available_iface(ctx->core_ctx);
        next_interface = UWD_UNDEFINED_INTERFACE;
    }
    else
    {
        interface_number = init_info->interface_number;
        next_interface = init_info->interface_number + 1;
    }

    size = sizeof(fd_desc_t) +          /* desc */
        sizeof(alt_interface_desc_t) +  /* cc_alt */
        sizeof(interface_desc_t);       /* cc_iface (ifaces[0]) */

    if (init_info->get_func_headers)
    {
        ex_desc_size = init_info->get_func_headers(ctx, NULL, FALSE, 
            interface_number);
        size += ex_desc_size;
    }

    if (init_info->cc_ntf_supported)
        size += sizeof(pipe_desc_t);    /* intr_pipe */
    
    if (init_info->dc_in_supported)
        size += sizeof(pipe_desc_t);    /* data_in_pipes */

    if (init_info->dc_out_supported)
        size += sizeof(pipe_desc_t);    /* data_out_pipe */

    if (num_of_data_pipe_present)
    {
      size += sizeof(alt_interface_desc_t)*dc_num_alts +  /* dc_alts */
            sizeof(interface_desc_t) +          /* dc_iface (ifaces[1]) */
            sizeof(struct usb_interface_assoc_descriptor);
    }

    curr = jmalloc(size, M_ZERO);
    if (!curr)
        goto Error;

    /* Make room for FD descriptor */
    desc = (fd_desc_t*)curr;
    curr = desc+1;

    /* Setup IAD */
    if ((num_of_data_pipe_present) &&
      /* HSU addition */
        !(ctx->info.basic.flags & CDC_INIT_NO_IAD))
    {/* End of HSU addition*/
        iad = (struct usb_interface_assoc_descriptor*)curr;
        curr = iad+1;
        
        iad->bLength = sizeof(struct usb_interface_assoc_descriptor);
        iad->bDescriptorType = USB_DT_INTERFACE_ASSOCIATION;
        iad->bFirstInterface = interface_number;
        iad->bInterfaceCount = 2;
        iad->bFunctionClass = USB_CLASS_COMM;

#ifdef FEATURE_HS_USB_MBIM		  
	if (ctx->info.basic.flags & CDC_INIT_MBIM_ECM)
	{
          iad->bFunctionSubClass = USB_CDC_SUBCLASS_MBIM;
          iad->bFunctionProtocol = USB_CDC_PROTOCOL_MBIM;
        }
	else
#endif
	{
        iad->bFunctionSubClass = 0;
        iad->bFunctionProtocol = 0;
	}
        iad->iFunction = 0;
    }
    
    /* Setup control class alt setting and pipes */
    if (init_info->cc_ntf_supported)
    {
        /* Setup interrupt pipe for notifications */
        intr_pipe = (pipe_desc_t*)curr;
        curr = intr_pipe+1;
        intr_pipe->ss_attributes.max_burst      = USB_SUPERSPEED_INTR_EP_MAXBURST;
        intr_pipe->max_pkt_size_super           = get_max_packet_size(SPEED_SUPER);
        intr_pipe->max_pkt_size_high            = 64;
        intr_pipe->max_pkt_size_full            = 64;
        intr_pipe->type                         = PIPE_INTR;
        intr_pipe->direction                    = DIRECTION_IN;
        intr_pipe->sync_type                    = 0;
        intr_pipe->usage_type                   = 0;
        intr_pipe->address                      = 0; /* Filled by Core */
        intr_pipe->transaction_per_microframe   = 0;
        intr_pipe->rate_u.poll_interval         = 5; /* 5ms in FS, 2ms in HS */

        cc_alt = (alt_interface_desc_t*)curr;
        curr = cc_alt+1;
        cc_alt->pipe_count = 1;
        cc_alt->pipes = intr_pipe;
    }
    else
    {
        /* Control endpoint only */
        cc_alt = (alt_interface_desc_t*)curr;
        curr = cc_alt+1;
        cc_alt->pipe_count = 0;
        cc_alt->pipes = NULL;
    }

    if (num_of_data_pipe_present)
    {
      data_pipes = (pipe_desc_t*)curr;

      /* Setup data class alt setting and pipes */
      if (init_info->dc_in_supported)
      {
          curr_data_pipe = (pipe_desc_t*)curr;
          curr = curr_data_pipe+1;
            curr_data_pipe->ss_attributes.max_burst = USB_SUPERSPEED_BULK_EP_MAXBURST;
          curr_data_pipe->max_pkt_size_super= get_max_packet_size(SPEED_SUPER);
          curr_data_pipe->max_pkt_size_high = get_max_packet_size(SPEED_HIGH);
          curr_data_pipe->max_pkt_size_full = get_max_packet_size(SPEED_FULL);
          curr_data_pipe->type              = PIPE_BULK;
          curr_data_pipe->direction         = DIRECTION_IN;
          curr_data_pipe->sync_type         = 0;
          curr_data_pipe->usage_type        = 0;
          curr_data_pipe->address           = 0; /* Filled by Core */
          curr_data_pipe->transaction_per_microframe = 0;
          curr_data_pipe->rate_u.nak_rate   = 32; /* HS: up to 1 NAK per 4 ms */
      }

      if (init_info->dc_out_supported)
      {
          curr_data_pipe = (pipe_desc_t*)curr;
          curr = curr_data_pipe+1;
            curr_data_pipe->ss_attributes.max_burst = USB_SUPERSPEED_BULK_EP_MAXBURST;
          curr_data_pipe->max_pkt_size_super= get_max_packet_size(SPEED_SUPER);
          curr_data_pipe->max_pkt_size_high = get_max_packet_size(SPEED_HIGH);
          curr_data_pipe->max_pkt_size_full = get_max_packet_size(SPEED_FULL);
          curr_data_pipe->type              = PIPE_BULK;
          curr_data_pipe->direction         = DIRECTION_OUT;
          curr_data_pipe->sync_type         = 0;
          curr_data_pipe->usage_type        = 0;
          curr_data_pipe->address           = 0; /* Filled by Core */
          curr_data_pipe->transaction_per_microframe = 0;
          curr_data_pipe->rate_u.nak_rate   = 32; /* HS: up to 1 NAK per 4 ms */
      }

      dc_alts = (alt_interface_desc_t*)curr;

      curr = dc_alts + dc_num_alts;
      if (init_info->dc_empty_alt_0)
      {
          dc_alts[0].pipe_count = 0;  /* Empty alt setting */
          dc_alts[0].pipes = NULL;
          dc_alts[1].pipe_count = num_of_data_pipe_present;  /* Data capabale alt setting */
          dc_alts[1].pipes = data_pipes;
      }
      else
      {
          dc_alts[0].pipe_count = num_of_data_pipe_present;  /* Data capabale alt setting */
          dc_alts[0].pipes = data_pipes;
      }
    }

    /* Setup interface descriptors */
    ifaces = (interface_desc_t*)curr;
    curr = num_of_data_pipe_present ? ifaces+2 : ifaces+1;

    ifaces[0].control_msg               = init_info->cc_handler;
    ifaces[0].alt_ifs                   = cc_alt;
    ifaces[0].alt_if_num                = 1;
    ifaces[0].alt_if_curr               = 0;
    ifaces[0].extra_descriptor          = NULL;
    ifaces[0].extra_descriptor_size     = 0;
    ifaces[0].number                    = init_info->interface_number; 
    /* HSU addition: HSU_STR_IDX_NOT_SET */
    ifaces[0].if_string = core_add_string_descriptor(ctx->core_ctx,
      ctx->info.basic.comm_string, HSU_STR_IDX_NOT_SET);
    /* HSU addition end */

    if (ctx->info.basic.flags & CDC_INIT_VENDOR_SPECIFIC_CLASS)
        ifaces[0].if_class              = USB_CLASS_VENDOR_SPEC;
    else
        ifaces[0].if_class              = USB_CLASS_COMM;

    if (init_info->cci_subclass)
    {
        ifaces[0].if_subclass           = init_info->cci_subclass;
        ifaces[0].if_protocol           = init_info->cci_protocol;
    }
    else
    {
        ifaces[0].if_subclass           = USB_CLASS_VENDOR_SPEC;
        ifaces[0].if_protocol           = USB_CLASS_VENDOR_SPEC;        
    }

    if (num_of_data_pipe_present)
    {
        ifaces[1].control_msg           = init_info->cc_handler;
        ifaces[1].alt_ifs               = dc_alts;
        ifaces[1].alt_if_num            = dc_num_alts;
        ifaces[1].alt_if_curr           = 0;
        ifaces[1].extra_descriptor      = NULL;
        ifaces[1].extra_descriptor_size = 0;
        ifaces[1].number                = next_interface; 
        ifaces[1].if_class              = USB_CLASS_CDC_DATA;
        ifaces[1].if_subclass           = 0;

#ifdef FEATURE_HS_USB_MBIM
		if (ctx->info.basic.flags & CDC_INIT_MBIM_ECM)
		{
			ifaces[1].if_protocol           = USB_CDC_PROTOCOL_MBIM_NTB;
		}
		else
#endif
		{
        ifaces[1].if_protocol           = 0;
		}
        /* HSU addition: HSU_STR_IDX_NOT_SET */
        ifaces[1].if_string = core_add_string_descriptor(ctx->core_ctx,
          ctx->info.basic.data_string, HSU_STR_IDX_NOT_SET);
        /* HSU addition end */
    }

    /* Setup extra (class specific) descriptor */
    if (init_info->get_func_headers)
    {
        ex_desc = curr;
        curr = (char*)ex_desc + ex_desc_size;
        init_info->get_func_headers(ctx, ex_desc, FALSE, interface_number);
        ifaces[0].extra_descriptor = ex_desc;
        ifaces[0].extra_descriptor_size = ex_desc_size;
    }

    /* Setup FD descriptor */
    switch (platform_info->hsu_platform_id)
    {
      case HSU_PLATFORM_VIRTIO:
      case HSU_PLATFORM_RUMI:
      {
        desc->speed = SPEED_HIGH;
      }
      break;
      default:
      {
        if (hsu_clkmgr_is_hs_only_mode())
        {
          desc->speed = SPEED_HIGH;
        }
        else
        {
          desc->speed = SPEED_SUPER;
        }
      }
      break;
    }
    desc->interface_count = num_of_data_pipe_present ? 2 : 1;
    desc->interfaces = ifaces;
    desc->iad = num_of_data_pipe_present ? iad : NULL;
    ctx->desc = desc;

    return 0;

Error:
    if (desc)
        jfree(desc);
    
    return -1;
}

/**
 * Function name:  reset_encap_state
 * Description: Reset encapsulted command state machine
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @notify_app: (IN) calls response complete callback when true
 *     @status: (IN) io status for response complete callback
 *     
 * Scope: local
 **/
static void reset_encap_state(cdc_ctx_t *ctx, jbool_t notify_app,
    cdc_iostatus_t status)
{
    juint32_t bytes_sent;
    
    if (ctx->encap_response.state == CDC_ENCAP_READY)
        notify_app = 0;

    /* Cleanup and prepare for next response */
    ctx->encap_response.state = CDC_ENCAP_READY;

#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION
    ctx->encap_response.zlp_client = CDC_ZLP_CLIENT_UNKNOWN;
#endif /* FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */

    ctx->encap_response.size = 0;
    bytes_sent = ctx->encap_response.bytes_sent;
    ctx->encap_response.bytes_sent = 0;

    if (ctx->encap_response.buffer)
    {
        jfree(ctx->encap_response.buffer);
        ctx->encap_response.buffer = NULL;
    }

    /* Notify application of operation status */
    if (notify_app && ctx->cbs.basic.encap_response_complete_h)
    {
        ctx->cbs.basic.encap_response_complete_h(status,
            bytes_sent, ctx->app_ctx);
    }
    
    if (notify_app)
    {
      HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "status %u, notify_app 1", (status));
    }
    else
    {
      HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "status %u, notify_app 0", (status));
    }
    
}

/**
 * Function name:  cdc_abort_all_pipes
 * Description: Abort all active CDC transfers on all pipes
 * Parameters: 
 *     @ctx: (IN) CDC context
 * 
 * Scope: local
 **/
static void cdc_abort_all_pipes(cdc_ctx_t *ctx)
{
    if (ctx->intr_pipe.desc)
        core_abort_pipe(ctx->core_ctx, ctx->intr_pipe.desc);

      if (ctx->in_pipe.desc)
          core_abort_pipe(ctx->core_ctx, ctx->in_pipe.desc);
      
      if (ctx->out_pipe.desc)
          core_abort_pipe(ctx->core_ctx, ctx->out_pipe.desc);
    }


static void cdc_abort_non_dbm_pipes_only(cdc_ctx_t *ctx)
{
  // Special function for aborting interrupt pipes without aborting BAM and DBM pipes (for suspend)
  if (ctx->intr_pipe.desc)
      core_abort_pipe(ctx->core_ctx, ctx->intr_pipe.desc);

  if ( (ctx->info.basic.flags & CDC_INIT_SSUSB_UL_BAM_BYPASS) && (ctx->out_pipe.desc) )
      core_abort_pipe(ctx->core_ctx, ctx->out_pipe.desc);
}

static boolean cdc_is_appctx_ecm(cdc_ctx_t *ctx)
{
  if (!ctx)
  {
    return FALSE;
  }
  return hsu_al_ecm_is_appctx_ecm(ctx->app_ctx);
}

/**
 * Function name:  default_cdc_get_pipe_config
 * Description:  USB CORE callback providing CORE with CDC descriptors
 * Parameters: 
 *     @context: (IN) CDC context
 *
 * Return value: USB descriptors structure
 * Scope: global
 **/
static fd_desc_t* default_cdc_get_pipe_config(context_t context)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;
    DBG_X(DSLAVE_CDC, ("cdc_get_pipe_config: called (%p)\n", ctx->desc));
    
    return ctx->desc;
}

/**
 * Function name:  cdc_enable
 * Description:  performs required initializations when device is 'enabled' by
 *               the host
 * Parameters: 
 *     @ctx: (IN) CDC context
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_enable(cdc_ctx_t *ctx)
{
    jbool_t is_single_iface = FALSE;
#ifdef FEATURE_HS_USB_INTR_ONLY_ECM_PIPE
    jbool_t is_intr_only_ecm_iface = FALSE;
#endif /* FEATURE_HS_USB_INTR_ONLY_ECM_PIPE */
    juint8_t data_iface_num = 1;
    juint8_t data_pipe_base = 0;
    interface_desc_t *iface = NULL;
    DBG_X(DSLAVE_CDC, ("cdc_enable: entered\n"));
   
    if (!ctx || !ctx->desc)
        return JEINVAL;

    if (ctx->info.basic.flags & CDC_INIT_SINGLE_INTERFACE)
    {
        is_single_iface = TRUE;
        data_iface_num = 0;
        data_pipe_base = 1;
    }
    
// HSU addition
#ifdef FEATURE_HS_USB_INTR_ONLY_ECM_PIPE
    if (ctx->info.basic.flags & CDC_INIT_INTR_ONLY_ECM_PIPE)
    {
        is_intr_only_ecm_iface = TRUE;
    }
#endif /* FEATURE_HS_USB_INTR_ONLY_ECM_PIPE */

    if (ctx->desc->interface_count >= 1)
    {
        juint8_t curr_alt = ctx->desc->interfaces[0].alt_if_curr;
        iface = &ctx->desc->interfaces[0];
        DBG_X(DSLAVE_CDC, ("cdc_enable: iface: %d, alt_setting: %u\n", 
            iface->number, curr_alt));
        ctx->intr_pipe.if_num = iface->number;
        if (is_single_iface && iface->alt_ifs[curr_alt].pipes && 
            iface->alt_ifs[curr_alt].pipes[0].type != PIPE_INTR)
        {
            ctx->intr_pipe.desc = NULL;
            data_pipe_base = 0; /* No interrupt in single interface */
        }
        else 
        {
            ctx->intr_pipe.desc = &iface->alt_ifs[curr_alt].pipes[0];
        }
    }

    if (ctx->desc->interface_count >= 2 || is_single_iface)
    {
        juint8_t curr_alt = ctx->desc->interfaces[data_iface_num].alt_if_curr;
        iface  = &ctx->desc->interfaces[data_iface_num];
        DBG_X(DSLAVE_CDC, ("cdc_enable: iface: %d, alt_setting: %u\n", 
            iface->number, curr_alt));

        if ((iface->alt_ifs[curr_alt].pipe_count >= 1)
        #ifdef FEATURE_HS_USB_INTR_ONLY_ECM_PIPE
              && (!(is_intr_only_ecm_iface))
        /* Incase we just have INTR for ECM pipe then pipe_count will be only 1 but this
            will be INTR not BULK so we to skip this part of code */
        #endif /* FEATURE_HS_USB_INTR_ONLY_ECM_PIPE */
        )
        {
            ctx->in_pipe.if_num = iface->number;
            ctx->in_pipe.desc = &iface->alt_ifs[curr_alt].pipes[data_pipe_base];
            DBG_X(DSLAVE_CDC, ("cdc_enable: iface: %d, alt_setting: %u, "
                "in_pipe: %x (0x%p)\n", iface->number, curr_alt, 
                ctx->in_pipe.desc->address, ctx->in_pipe.desc));
        }

        if (iface->alt_ifs[curr_alt].pipe_count >= 2)
        {
            ctx->out_pipe.if_num = iface->number;
            ctx->out_pipe.desc =
                &iface->alt_ifs[curr_alt].pipes[data_pipe_base + 1];
            DBG_X(DSLAVE_CDC, ("cdc_enable: iface: %d, alt_setting: %u, "
                "out_pipe: %x (0x%p)\n", iface->number, curr_alt, 
                ctx->out_pipe.desc->address, ctx->out_pipe.desc));
        }
    }

    ctx->usb_max_packet_size = get_max_packet_size(ctx->desc->current_speed);
    return 0;
}

/**
 * Function name:  default_cdc_enable
 * Description:  default USB CORE callback for handling SetConfig & SetInterface
 *               'events' - i.e. when the device is enabled by the host
 * Parameters: 
 *     @context: (IN) CDC context
 *
 * Return value: 0 for success, or an error code 
 * Scope: local
 **/
static jresult_t default_cdc_enable(context_t context)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;
    jresult_t rc = 0;

    DBG_X(DSLAVE_CDC, ("default_cdc_enable: entered\n"));

    if (!ctx)
        return JEINVAL;

    rc = cdc_enable(ctx);
    if (rc)
        return rc;
    
    if (ctx->cbs.basic.enabled_h)
        rc = ctx->cbs.basic.enabled_h(ctx->app_ctx);

    return rc;
}

/**
 * Function name:  cdc_disable
 * Description:  performs required cleanup when device is 'disabled' by
 *               the host
 * Parameters: 
 *     @ctx: (IN) CDC context
 *
 * Return value: 0 for success, or an error code 
 * Scope: local
 **/
void cdc_disable(cdc_ctx_t *ctx)
{
    DBG_X(DSLAVE_CDC, ("cdc_disable: entered\n"));

    reset_encap_state(ctx, 1, CDC_TRANSFER_CANCELLED);

    cdc_abort_all_pipes(ctx);

    ctx->intr_pipe.desc = NULL;    
    ctx->intr_pipe.if_num = 0;
    ctx->in_pipe.desc = NULL;
    ctx->in_pipe.if_num = 0;
    ctx->out_pipe.desc = NULL;
    ctx->out_pipe.if_num = 0;
}

/**
 * Function name:  default_cdc_disable
 * Description:  default USB CORE callback for handling SetConfig & SetInterface
 *               'events' - i.e. when the device is disabled by the host
 * Parameters: 
 *     @context: (IN) CDC context
 *
 * Return value: None
 * Scope: local
 **/
static void default_cdc_disable(context_t context)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;
    
    DBG_X(DSLAVE_CDC, ("default_cdc_disable: entered\n"));

    if (!ctx)
        return;

    // ECM does not go through this path for disconnect since the basic.disabled_h was registered to ECM

    if (ctx->cbs.basic.disabled_h)
        ctx->cbs.basic.disabled_h(ctx->app_ctx);

    cdc_disable(ctx);
}

/**
 * Function name:  default_cdc_suspend
 * Description: Handle SUSPEND signal from the host. Abort all active transfers,
 *              and reset encapsulated command state.
 * Parameters: 
 *     @context: (IN) CDC context
 *
 * Return value: 0 for success, or an error code 
 * Scope: local
 **/
static jresult_t default_cdc_suspend(context_t context)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;

    HSU_USE_PARAM(ctx);

    DBG_I(DSLAVE_CDC, ("default_cdc_suspend: Received SUSPEND from host\n"));

    reset_encap_state(ctx, 1, CDC_TRANSFER_CANCELLED);
    if (cdc_is_appctx_ecm(ctx))
    {
      cdc_abort_all_pipes(ctx);
      //cdc_abort_non_dbm_pipes_only(ctx);
    }
    else if (ctx->info.basic.flags & CDC_INIT_OBEX_BULK_IN_ONLY)
    {
      HSU_ULOG_1(NO_MSG, BUS_LOG, "skipped_for_qdss, in_pipe_if_num %u", ctx->in_pipe.if_num);
    }
    else
    {
      cdc_abort_all_pipes(ctx);
    }

    #ifdef FEATURE_HS_USB_BAM
    if (ctx->info.basic.flags & CDC_INIT_OBEX_BULK_IN_ONLY)
    {
      HSU_ULOG_1(NO_MSG, BUS_LOG, "abort_qdss, in_pipe_if_num %u", ctx->in_pipe.if_num);
      cdc_abort_all_pipes(ctx);
    }
    #endif
    
    return 0;
}

/* Temp fix for B46104: added resume */

/**
* Function name:  default_cdc_resume
* Description: Handle RESUME signal from the host. Invoke app callback
* Parameters: 
*     @context: (IN) CDC context
*
* Return value: 0 for success, or an error code 
* Scope: local
**/
static jresult_t default_cdc_resume(context_t context)
{
  cdc_ctx_t *ctx = (cdc_ctx_t*)context;
  jresult_t rc = 0;

  DBG_I(DSLAVE_CDC, ("default_cdc_resume: Received RESUME from host\n"));

  if (ctx->cbs.basic.resumed_h)
    rc = ctx->cbs.basic.resumed_h(ctx->app_ctx);

  return rc;
}

/* end temp fix */


/**
 * Function name:  cdc_uninit
 * Description:  USB CORE callback for notifying CDC of stack removal. This
 *               function performs required cleanup before removal.
 * Parameters: 
 *     @context: (IN) CDC context
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_uninit(context_t context)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;
    
    DBG_X(DSLAVE_CDC, ("cdc_uninit: entered\n"));

    if (!ctx)
        return 0;

    /* Call application's uninit callback */
    if (ctx && ctx->cbs.basic.uninit_h)
        ctx->cbs.basic.uninit_h(ctx->app_ctx);

    /* Free preallocated requests */
    if (ctx->ctrl_read)
        core_request_free(ctx->ctrl_read);
    
    if (ctx->ctrl_write)
        core_request_free(ctx->ctrl_write);
    
    /* Free encapsulated reponse preallocated buffer */
    if (ctx->encap_response.buffer)
        jfree(ctx->encap_response.buffer);

    /* Free descriptors and context */
    cdc_free_descriptors(ctx);
    jfree(ctx);

    return 0;
}

/**
 * Function name:  cdc_init
 * Description:  this function is called by the CDC model's init function to
 *               register the CDC FD with the CORE.
 *               this function assigns the default handlers above, unless other
 *               handlers have been filled in the the model's code
 * Parameters: 
 *     @ctx: (IN) CDC context
 *
 * Return value: 0 for success, or an error code  
 * Scope: global
 **/
jresult_t cdc_init(cdc_ctx_t *ctx)
{
#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION
    static boolean cdc_init_run_once = FALSE;
#endif /* FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */
    jresult_t rc = -1;

    DBG_X(DSLAVE_CDC, ("cdc_init: entered\n"));

    if (!ctx->ops.get_pipe_config)
        ctx->ops.get_pipe_config = default_cdc_get_pipe_config;
    if (!ctx->ops.enable)
        ctx->ops.enable = default_cdc_enable;
    if (!ctx->ops.disable)
        ctx->ops.disable = default_cdc_disable;
    if (!ctx->ops.suspend)
        ctx->ops.suspend = default_cdc_suspend;
/* Temp fix for B46104: added resume */
    if (!ctx->ops.resume)
      ctx->ops.resume = default_cdc_resume;
/* end temp fix */
    if (!ctx->ops.uninit)
        ctx->ops.uninit = cdc_uninit;

#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION
    if (!cdc_init_run_once)
    {
      cdc_init_run_once = TRUE;
#ifdef FEATURE_HS_USB_ERR_FATAL_UPON_TIMER_EXPIRY_FOR_RX_TX_ZLP_COMPLETION 
      if (hsu_conf_sel_is_ep0_timer_on_rx_tx_zlp_err_chk_enabled())
      {
        hsu_missing_zlp_debug_ctx.encap_cmd_tx_zlp_timer_id = hsu_timer_init(HSU_TIMER_CID__ENCAP_TX_ZLP, 
                       HSU_TIMER_DISALLOW_REPRIME_WHILE_TIMER_ACTIVE | HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX,
                       hsu_cdc_send_encap_cmd_tx_zlp_tmr_cb, 
                       NULL);
        
        hsu_missing_zlp_debug_ctx.encap_resp_rx_zlp_timer_id = hsu_timer_init(HSU_TIMER_CID__ENCAP_RX_ZLP,
                       HSU_TIMER_DISALLOW_REPRIME_WHILE_TIMER_ACTIVE | HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX,
                       hsu_cdc_get_encap_resp_rx_zlp_tmr_cb,
                       NULL);
      }
#endif /* FEATURE_HS_USB_ERR_FATAL_UPON_TIMER_EXPIRY_FOR_RX_TX_ZLP_COMPLETION */
    }
#endif /* FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */

    DBG_X(DSLAVE_CDC, ("cdc_init: calling register_fd.\n"));
    rc = core_register_fd(ctx->core_ctx, &ctx->ops, ctx);
    
    return rc;
}

juint32_t cdc_get_encapsulated_response_state(cdc_ctx_t *ctx)
{
  return (juint32_t)(ctx->encap_response.state);
}

void cdc_dbm_init(cdc_ctx_t *ctx)
{
  if (ctx)
  {
    core_dbm_init(ctx->core_ctx);
  }
  return;
}

void cdc_dbm_init_qdss(cdc_ctx_t *ctx, uint32 consumer_pipe_index, uint32 consumer_data_fifo_addr, uint32 consumer_data_fifo_size)
{
  core_dbm_init_qdss(ctx->core_ctx, consumer_pipe_index, consumer_data_fifo_addr, consumer_data_fifo_size); 
  return;
}

void cdc_dbm_ep_init(cdc_ctx_t *ctx, uint32 dbm_ep_num)
{
  if (ctx)
  {
    core_dbm_ep_init(ctx->core_ctx, dbm_ep_num);
  }
  return;
}

void cdc_dbm_ep_reset(cdc_ctx_t *ctx, uint32 dbm_ep_num)
{
  if (ctx)
  {
    core_dbm_ep_reset(ctx->core_ctx, dbm_ep_num);
  }
  return;
}

void cdc_set_bam_pipe_params_for_dbm(cdc_ctx_t *ctx, void *connection, pipe_desc_t *out_pipe, uint32 producer_pipe_index, 
                                     uint32 producer_data_fifo_addr, uint32 producer_data_fifo_size,
                                     uint32 producer_desc_fifo_addr, uint32 producer_desc_fifo_size,
                                     pipe_desc_t *in_pipe, uint32 consumer_pipe_index,
                                     uint32 consumer_data_fifo_addr, uint32 consumer_data_fifo_size,
                                     uint32 consumer_desc_fifo_addr, uint32 consumer_desc_fifo_size)
{
  if (ctx)
  {
    // Do not initialize dbm_ep_num when UL_BAM_BYPASS is set (Only for SNPS core).
    if(ctx->info.basic.flags & CDC_INIT_SSUSB_UL_BAM_BYPASS)
    {
      out_pipe = NULL;
    }
    
    core_set_bam_pipe_params_for_dbm(ctx->core_ctx, connection, out_pipe, producer_pipe_index, 
                                     producer_data_fifo_addr, producer_data_fifo_size,
                                     producer_desc_fifo_addr, producer_desc_fifo_size,
                                     in_pipe, consumer_pipe_index,
                                     consumer_data_fifo_addr, consumer_data_fifo_size,
                                     consumer_desc_fifo_addr, consumer_desc_fifo_size);
  }
}

