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
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/classes/src/jslave/fd/cdc/ecm_fd.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
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
#include "cdc_fd.h"
#ifdef FEATURE_HSU_DCD_DEBUG_LOGS
#include "hsu_log.h"
#endif /* FEATURE_HSU_DCD_DEBUG_LOGS */
#include "hsu_conf_sel_i.h"
#include "hsu_al_ecm_int.h"

/* 
 * static structure for saving ecm specifics like string decriptors
 **/
typedef struct _ecm_ctx_t 
{
    cdc_ctx_t cdc_ctx;          /* the CDC context */
    juint8_t iMACAddress;        /* iMACAddress descriptor index */
} ecm_ctx_t;  
     
#ifdef FEATURE_HS_USB_MBIM
#ifndef USB_NTB_8_BYTE_SUPPORTED
#define USB_NTB_8_BYTE_SUPPORTED (0x20)
#endif
#define USB_INIT_ZERO (0x0)
#endif     

#ifdef FEATURE_HS_USB_ECM_INT_EP_SHARING    
static boolean hsu_ecm_fd_is_intr_pipe_configured = FALSE;
#endif /* FEATURE_HS_USB_ECM_INT_EP_SHARING */

/*
 * Host to Device - Requests
 */

/**
 * Function name:  set_multicast_filters_complete
 * Description: completion handler for the SET_MULTICAST_FILTERS request
 * Parameters: 
 *     @req: (IN) CORE request
 *     
 * Scope: local
 **/
static void set_multicast_filters_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;
    juint16_t nfilters;
    jresult_t rc;

    DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", __func__));

    if (cdc_get_iostatus(req, 1) != CDC_TRANSFER_OK)
        goto Error;
    
    /* Pass data to application */
    nfilters = le16toh(ctx->setup_packet.wValue);

    if (cbs->set_multicast_filter_h((mac_addr_t*)req->buffer.vaddr,
        nfilters, ctx->app_ctx) < 0)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, app_failed_to_handle_request");        
        goto Error;
    }

    rc = cdc_send_status(ctx);
    if (rc)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC_ACM, failed_to_send_status_packet, rc %u", rc);        
        goto Error;
    }

    return;

Error:
    core_stall_pipe(ctx->core_ctx, 0, 1);
}

/**
 * Function name:  handle_set_multicast_filters
 * Description: handler for the SET_MULTICAST_FILTERS request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t handle_set_multicast_filters(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;
    request_t *reply = NULL;
    jresult_t rc = -1;

    DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", __func__));

    if (!cbs->set_multicast_filter_h)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, no_handler_defined");
        goto Error;
    }

    /* Save setup packet */
    j_memcpy(&ctx->setup_packet, req, sizeof(ctx->setup_packet));

    /* Setup reply */
    reply = cdc_create_ep0_read(ctx, le16toh(req->wLength));
    if (!reply)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, failed_to_setup_reply");        
        goto Error;
    }

    reply->complete = set_multicast_filters_complete;
    reply->context = ctx;

    rc = core_read_data(ctx->core_ctx, 0, reply);
    
Error:
    return rc;
}

/*HSU FIX: Adding support for SET_CONTROL_LINE_STATE on ECM */
/**
* Function name:  handle_set_control_line_state
* Description:  handler for SET_CONTROL_LINE_STATE request
* Parameters: 
*     @ctx: (IN) CDC context
*     @req: (IN) USB SETUP packet
*
* Return value: 0 for success, or an error code
* Scope: local
**/
static jresult_t handle_set_control_line_state(cdc_ctx_t *ctx,
struct usb_ctrlrequest *req)
{
  ecm_callbacks_t *cbs = NULL;
  juint16_t ctrl_sig_bitmap = 0;
  jresult_t rc = -1;

  DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", __func__));

  HSU_ASSERT(ctx != NULL);
  HSU_ASSERT(req != NULL);

  cbs = &ctx->cbs.ecm;
  if (!cbs->set_control_line_state_h)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, no_handler_defined");    
    goto Error;
  }

  /* Pass data to application */
  ctrl_sig_bitmap = le16toh(req->wValue);
  if (cbs->set_control_line_state_h(ctrl_sig_bitmap, ctx->app_ctx) < 0)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, app_failed_to_handle_req");    
    goto Error;
  }

  rc = cdc_send_status(ctx);

Error:
  return rc;
}
/*End of HSU FIX*/

/**
 * Function name:  set_power_filter_complete
 * Description: completion handler for the SET_POWER_FILTER request
 * Parameters: 
 *     @req: (IN) CORE request
 *     
 * Scope: local
 **/
static void set_power_filter_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;
    juint16_t filter_id;
    juint16_t filter_size;
    ecm_pm_filter_t pm_filter;
    juint8_t *bytep;
    jresult_t rc;

    DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", __func__));

    if (cdc_get_iostatus(req, 1) != CDC_TRANSFER_OK)
        goto Error;
    
    /* Pass data to application */
    filter_id = le16toh(ctx->setup_packet.wValue);
    filter_size = le16toh(ctx->setup_packet.wLength);
    bytep = (juint8_t*)(req->buffer.vaddr);
    pm_filter.mask_size = le16toh(*(juint16_t*)req->buffer.vaddr);
    pm_filter.mask = bytep+2;
    pm_filter.pattern = bytep+2+pm_filter.mask_size;
    if (cbs->set_power_filter_h(filter_id, filter_size, &pm_filter,
        ctx->app_ctx) < 0)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, app_failed_to_handle_req");
        goto Error;
    }

    rc = cdc_send_status(ctx);
    if (rc)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC_ACM, failed_to_send_status_packet, rc %u", rc);        
        goto Error;
    }

    return;

Error:
    core_stall_pipe(ctx->core_ctx, 0, 1);
}

/**
 * Function name:  handle_set_power_filter
 * Description: handler for the SET_POWER_FILTER request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t handle_set_power_filter(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;
    request_t *reply = NULL;
    jresult_t rc = -1;

    DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", __func__));

    if (!cbs->set_power_filter_h)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, no_handler_defined");
        goto Error;
    }

    /* Save setup packet */
    j_memcpy(&ctx->setup_packet, req, sizeof(ctx->setup_packet));

    /* Setup reply */
    reply = cdc_create_ep0_read(ctx, le16toh(req->wLength));
    if (!reply)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, failed_to_setup_reply");
        goto Error;
    }

    reply->complete = set_power_filter_complete;
    reply->context = ctx;

    rc = core_read_data(ctx->core_ctx, 0, reply);
    
Error:
    return rc;
}

/**
 * Function name:  get_power_filter_complete
 * Description: completion handler for the GET_POWER_FILTER request
 * Parameters: 
 *     @req: (IN) CORE request
 *     
 *Scope: local
 **/
static void get_power_filter_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;

    DBG_X(DSLAVE_CDC_ECM, ("get_power_filter_complete: entered\n"));
    
    if (cbs->get_power_filter_complete_h)
    {
        cdc_iostatus_t status = cdc_get_iostatus(req, 1);
        cbs->get_power_filter_complete_h(status, ctx->app_ctx);
    }
}

/**
 * Function name:  handle_get_power_filter
 * Description: handler for the GET_POWER_FILTER request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) CORE request
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t handle_get_power_filter(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;
    request_t *reply = NULL;
    juint16_t filter_id;
    juint16_t pattern_active;
    jbool_t is_active;
    jresult_t rc = -1;

    DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", __func__));

    if (!cbs->get_power_filter_h)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, no_handler_defined");
        goto Error;
    }

    /* Get data from application */
    filter_id = le16toh(req->wValue);
    if (cbs->get_power_filter_h(filter_id, &is_active, ctx->app_ctx) < 0)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, app_failed_to_handle_req");        
        goto Error;
    }

    /* Setup reply */
    pattern_active = htole16(is_active?1:0);
    reply = cdc_create_ep0_write(ctx, &pattern_active, sizeof(pattern_active),
        le16toh(req->wLength));
    if (!reply)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, failed_to_setup_reply");        
        goto Error;
    }

    reply->complete = get_power_filter_complete;
    reply->context = ctx;
    
    rc = core_send_data(ctx->core_ctx, 0, reply);

Error:
    return rc;
}

/**
 * Function name:  handle_set_packet_filter
 * Description: handler for the SET_PACKET_FILTER request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t handle_set_packet_filter(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;
    juint16_t pf_bitmask = 0;
    jresult_t rc = -1;

    DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", __func__));

    if (!cbs->set_packet_filter_h)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, no_handler_defined");
        goto Error;
    }

    /* Pass data to application */
    pf_bitmask = le16toh(req->wValue);
    if (cbs->set_packet_filter_h(pf_bitmask, ctx->app_ctx) < 0)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, app_failed_to_handle_req");
        goto Error;
    }

    rc = cdc_send_status(ctx);
    
Error:
    return rc;
}

/**
 * Function name:  get_statistics_complete
 * Description: completion handler for the GET_ETHERNET_STATISTICS request
 * Parameters: 
 *     @req: (IN) CORE request
 *     
 * Scope: local
 **/
static void get_statistics_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;

    DBG_X(DSLAVE_CDC_ECM, ("get_statistics_complete: entered\n"));
    
    if (cbs->get_stats_complete_h)
    {
        cdc_iostatus_t status = cdc_get_iostatus(req, 1);
        cbs->get_stats_complete_h(status, ctx->app_ctx);
    }
}

/**
 * Function name:  handle_get_statistics
 * Description: handler for the GET_ETHERNET_STATISTICS request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t handle_get_statistics(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;
    ecm_init_info_t *info = &ctx->info.ecm;
    request_t *reply = NULL;
    ecm_enum_stats_t selector = ECM_FS_NONE;
    juint32_t count;
    jresult_t rc = -1;
    
    DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", __func__));

    if (!cbs->get_stats_h)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, no_handler_defined");
        goto Error;
    }

    if (le16toh(req->wValue))
	selector = (ecm_enum_stats_t)(1 << (le16toh(req->wValue)-1));

    /* Check against device capabilities */
    if ( !(selector & info->eth_statistics) )
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC_ECM, req_for_unsupported_static %u", selector);        
        goto Error;
    }
    
    /* Get data from application */
    if (cbs->get_stats_h(selector, &count, ctx->app_ctx) < 0)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, app_failed_to_handle_req");
        goto Error;
    }

    /* Setup reply */
    count = htole32(count);
    reply = cdc_create_ep0_write(ctx, &count, sizeof(count), 
        le16toh(req->wLength));
    if (!reply)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, failed_to_setup_reply");
        goto Error;
    }

    reply->complete = get_statistics_complete;
    reply->context = ctx;
    
    rc = core_send_data(ctx->core_ctx, 0, reply);

Error:
    return rc;
}

#ifdef FEATURE_HS_USB_MBIM
typedef struct
{
    juint16_t             		wLength;

    juint16_t        			bmNtbFormatsSupported;
    
    juint32_t        			dwNtbInMaxSize;
    
    juint16_t        			wNdpInDivisor;
    
    juint16_t        			wNdpInPayloadRemainder;
    
    juint16_t        			wNdpInAlignment;
    
    juint16_t        			reservedPadding;

    juint32_t        			dwNtbOutMaxSize;

    juint16_t        			wNdpOutDivisor;

    juint16_t        			wNdpOutPayloadRemainder;

    juint16_t        			wNdpOutAlignment;

    juint16_t        			wNtbOutMaxDatagrams;
} ncm_ntb_parameters_t;

typedef struct
{
	juint32_t ntb_size;
	juint16_t max_datagrams_in_ntb;
	juint16_t ntb_reserved;
} ntb_input_size;

/**
 * Function name:  handle_get_ntb_parameters 
 * Description: handler for the GET_NTB_PARAMETERS request 
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t handle_get_ntb_parameters(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    request_t *reply = NULL;
    ncm_ntb_parameters_t ntb_param;
    jresult_t rc = -1;
    
    DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", __func__));

    ntb_param.wLength = 0x1C;
    ntb_param.bmNtbFormatsSupported = USB_NTB_FORMATS_SUPPORTED;
    ntb_param.dwNtbInMaxSize = USB_NTB_IN_MAX_SIZE;
    ntb_param.wNdpInDivisor = USB_NDP_IN_DIVISOR;
    ntb_param.wNdpInPayloadRemainder = USB_NDP_IN_PAYLOAD_REMAINDER;
    ntb_param.wNdpInAlignment = USB_NDP_IN_ALIGNMENT;
    ntb_param.dwNtbOutMaxSize = USB_NTB_OUT_MAX_SIZE;
    ntb_param.wNdpOutDivisor = USB_NDP_OUT_DIVISOR;
    ntb_param.wNdpOutPayloadRemainder = USB_NDP_OUT_PAYLOAD_REMAINDER;
    ntb_param.wNdpOutAlignment = USB_NDP_OUT_ALIGNMENT;
    ntb_param.wNtbOutMaxDatagrams = USB_NTB_OUT_MAX_DATAGRAMS; 

    /* Setup reply */
    reply = cdc_create_ep0_write(ctx, &ntb_param, sizeof(ntb_param), 
        le16toh(req->wLength));
    if (!reply)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, failed_to_setup_reply");        
        goto Error;
    }

    reply->complete = NULL;
    reply->context = ctx;
    
    rc = core_send_data(ctx->core_ctx, 0, reply);

Error:
    return rc;
}

void set_ntb_input_size_complete(cdc_handle_t handle,jresult_t wstatus)
{
  cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
  jresult_t rc =0;

  DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", __func__));
  if(wstatus < 0)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, app_failed_to_use_NTB_size");
    goto Error;
  }
  else
  {
    /*This indicates that the input size setting is successful*/
    /*Send Response to Host reporting a success*/
    rc = cdc_send_status(ctx);
    if (rc)
    {
      HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC_ECM, failed_to_send_status_packet, rc %u", rc);
    }
  }
  return;
Error:
  core_stall_pipe(ctx->core_ctx, 0, 1);
}

void handle_reset_function_complete(cdc_handle_t handle,jresult_t wstatus)
{
  cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
  jresult_t rc = 0;

    DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", __func__));
  if(wstatus < 0)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, failed_to_use_RESET_FUNCTION");    
    /*@TO DO :- Process Error if Reset_Function has failed */		
  }
  else
  {
    /*Send the status to host that the process was successful*/
    rc = cdc_send_status(ctx);
    if (rc)
    {
      HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC_ECM, failed_to_send_status_packet, rc %u", rc);      
    } 
  }
}
void ntb_size_received(request_t *req)
{
  cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
  ecm_callbacks_t *cbs = &ctx->cbs.ecm;
  juint32_t ntb_size = USB_INIT_ZERO;
  juint16_t max_datagrams_in_ntb = USB_MAX_DATAGRAMS_IN_NTB;
  juint16_t wLength = USB_INIT_ZERO; 
  ntb_input_size *ntb;
  boolean is_qbi_ready_with_callbacks;
  DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", __func__));

  /*Check whether QBI has registered with HS-USB to handle requests*/
  if(cbs->get_mbim_cb_status_h)
  {
    is_qbi_ready_with_callbacks = cbs->get_mbim_cb_status_h();
    if(!is_qbi_ready_with_callbacks)
    {
      goto Error;
    }
  }
  else
  {
    goto Error;
  }

  if (cdc_get_iostatus(req, 1) != CDC_TRANSFER_OK)
  {
    goto Error;
  }
  else
  {
    wLength = le16toh(ctx->setup_packet.wLength);
    if(!cbs->set_ntb_input_size_h)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, app_failed_to_register_callback");      
      goto Error;
    }
    /*Read the Network Capabilities flag to check if we support 4 or 8 wLength*/
    if(USB_NTB_8_BYTE_SUPPORTED & USB_NETWORK_CAPABILITIES) /*This means we support both wLengths 8 or 4*/
    {
      /*Check for the wLength and decide whether max_datagrams_in_ntb should be 0*/
      if(4 == wLength) /*Set max_datagrams_in_ntb as 0*/
      {
        max_datagrams_in_ntb = USB_MAX_DATAGRAMS_IN_NTB;
        /*As QBI's limit is 0x0A, set this value*/
        /*should be zero, meaning no limit*/
        ntb_size = le32toh(*(juint32_t*)(req->buffer.vaddr)); 
      }
      else if (8 == wLength)		/*when the length is 8*/
      {
        ntb = (ntb_input_size*)req->buffer.vaddr;
        ntb_size = le32toh(ntb->ntb_size);
        max_datagrams_in_ntb = le16toh(ntb->max_datagrams_in_ntb);
      }
      else  /*When wrong size is sent*/
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, wrong_NTB_structure_received");              
        goto Error;
      }
    }
    else
    {
      if(4 != wLength)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, ntb_size_should_be_4B");              
        goto Error;
      }
      else
      {
        max_datagrams_in_ntb = USB_MAX_DATAGRAMS_IN_NTB;
        /*As QBI's limit is 0x0A, set this value*/
        /*should be zero, meaning no limit*/
        ntb_size = le32toh(*(juint32_t*)(req->buffer.vaddr)); 
      }
    }
    if(cbs->set_ntb_input_size_h)
    {
      /*SetNtbSize is successfully done - pass info to QBI Layer*/
      cbs->set_ntb_input_size_h(ctx,ntb_size,max_datagrams_in_ntb);
    }
  }   
  return;
Error:
  core_stall_pipe(ctx->core_ctx, 0, 1);
}

static jresult_t handle_set_ntb_input_size(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    request_t *reply = NULL;
    jresult_t rc = -1;

    DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", __func__));

    /* Save setup packet */
    j_memcpy(&ctx->setup_packet, req, sizeof(ctx->setup_packet));

    /* Setup reply */
    reply = cdc_create_ep0_read(ctx, le16toh(req->wLength));
    if (!reply)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, failed_to_setup_reply");
        goto Error;
    }

    reply->complete = ntb_size_received;
    reply->context = ctx;

    rc = core_read_data(ctx->core_ctx, 0, reply);
    
Error:
    return rc;
}

static jresult_t handle_reset_function(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;
    jresult_t rc = -1;
  boolean is_qbi_ready_with_callbacks;

  DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", __func__));

  /*Check whether QBI has registered with HS-USB to handle requests*/
  if(cbs->get_mbim_cb_status_h)
  {
    is_qbi_ready_with_callbacks = cbs->get_mbim_cb_status_h();
    if(!is_qbi_ready_with_callbacks)
    {
      /*Stall the request*/
      core_stall_pipe(ctx->core_ctx, 0, 1);
      goto Error;
    }
  }
  else
  {
    goto Error;
  }
  if(cbs->handle_reset_function_h)
  {
    cbs->handle_reset_function_h(ctx);
  }
  rc = cdc_send_status(ctx);
Error:
  return rc;
}

#endif /* FEATURE_HS_USB_MBIM */

/**
 * Function name:  ecm_handle_request
 * Description:  ECM model control interface request handler
 * Parameters: 
 *     @context: (IN) CDC context
 *     @buffer: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t ecm_handle_request(void *context, void *buffer)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;
    struct usb_ctrlrequest *req = (struct usb_ctrlrequest*)buffer;
    jresult_t rc = JENOTSUP;

    DBG_X(DSLAVE_CDC_ECM, ("ecm_handle_request: called with request (%u)\n",
        req->bRequest));
    
    hsu_trace_msg2_set(ECM_HANDLE_REQUEST__TRC);
    hsu_trace_inf_set(req->bRequest);

    switch(req->bRequest)
    {
    case CDC_REQ_SEND_ENCAPSULATED_COMMAND:
#ifdef FEATURE_HSU_DCD_DEBUG_LOGS
        HSU_ULOG(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "cdc_req_send_encapsulated_cmd");

#endif /* FEATURE_HSU_DCD_DEBUG_LOGS */
        rc = cdc_handle_encapsulated_command(ctx, req);
        break;
        
    case CDC_REQ_GET_ENCAPSULATED_RESPONSE:
#ifdef FEATURE_HSU_DCD_DEBUG_LOGS
        HSU_ULOG(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "cdc_req_send_encapsulated_cmd");

#endif /* FEATURE_HSU_DCD_DEBUG_LOGS */
        rc = cdc_handle_encapsulated_response(ctx, req);
        break;
        
    case CDC_REQ_SET_ETHERNET_MULTICAST_FILTERS:
        rc = handle_set_multicast_filters(ctx, req);
        break;
        
    case CDC_REQ_SET_ETHERNET_POWER_MANAGEMENT_PATTERNFILTER:
        rc = handle_set_power_filter(ctx, req);
        break;
        
    case CDC_REQ_GET_ETHERNET_POWER_MANAGEMENT_PATTERNFILTER:
        rc = handle_get_power_filter(ctx, req);
        break;
        
    case CDC_REQ_SET_ETHERNET_PACKET_FILTER:
        rc = handle_set_packet_filter(ctx, req);
        break;
        
    case CDC_REQ_GET_ETHERNET_STATISTIC:
        rc = handle_get_statistics(ctx, req);
        break;
/*HSU FIX: Adding support for SET_CONTROL_LINE_STATE on ECM*/
    case CDC_REQ_SET_CONTROL_LINE_STATE:
      rc = handle_set_control_line_state(ctx, req);
      break;  
/* End of HSU fix */

#ifdef FEATURE_HS_USB_MBIM
    case CDC_REQ_GET_NTB_PARAMETERS:
      rc = handle_get_ntb_parameters(ctx, req);
      break;  
      
    case CDC_REQ_SET_NTB_INPUT_SIZE:
      rc = handle_set_ntb_input_size(ctx, req);
      break;  

    case CDC_REQ_RESET_FUNCTION:
      rc = handle_reset_function(ctx, req);
      break;  
#endif /* FEATURE_HS_USB_MBIM */
      
    default:
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CDC_ECM, unsupported_req %u", req->bRequest);
        break;
    }


    hsu_trace_msg2_clear();
    hsu_trace_inf_clear();

    return rc;
}

/*
 * Device to Host - Notifications
 */

/**
 * Function name:  notify_conn_speed_change_complete
 * Description: completion handler for the CONNECTION_SPEED_CHANGE notification
 * Parameters: 
 *     @req: (IN) CORE request
 *     
 * Scope: local
 **/
static void notify_conn_speed_change_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    ecm_callbacks_t *cbs = &(ctx->cbs.ecm);

    DBG_X(DSLAVE_CDC_ECM, ("notify_conn_speed_change_complete: entered\n"));
    
    if (cbs->notify_speed_change_complete_h)
    {
        cdc_iostatus_t status = cdc_get_iostatus(req, 1);
        cbs->notify_speed_change_complete_h(status, ctx->app_ctx);
    }

    core_request_free(req);
}

/**
 * Function name:  ecm_notify_conn_speed_change
 * Description:  Control Interface API - sends a CONNECTION_SPEED_CHANGE
 *               notification
 * Parameters: 
 *     @handle: (IN) CDC handle
 *     @conn_speed: (IN) connection speed
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t ecm_notify_conn_speed_change(cdc_handle_t handle,
    ecm_conn_speed_t *conn_speed)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
    request_t *req = NULL;
    jresult_t rc = 0;
    ecm_conn_speed_t c_speed;
#ifdef FEATURE_HS_USB_ECM_INT_EP_SHARING    
    cdc_ctx_t *shared_intr_ctx = NULL;
#endif /* FEATURE_HS_USB_ECM_INT_EP_SHARING */

    if (ctx->in_pipe.desc == NULL)
    {
        rc = JENXIO;
        goto Error;
    }

    c_speed.upstream_bitrate = htole32(conn_speed->upstream_bitrate);
    c_speed.downstream_bitrate = htole32(conn_speed->downstream_bitrate);
    req = cdc_create_notification(ctx, CDC_NTF_CONNECTION_SPEED_CHANGE, 0,
        ctx->in_pipe.if_num, sizeof(c_speed), (juint8_t*)&c_speed);

    if (!req)
    {
        rc = JENOMEM;
        goto Error;
    }

    req->complete = notify_conn_speed_change_complete;
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
            goto Error;
        }

        rc = core_send_data(ctx->core_ctx, shared_intr_ctx->intr_pipe.desc, req);
    }
    else
#endif /* FEATURE_HS_USB_ECM_INT_EP_SHARING */
    {
        if (ctx->intr_pipe.desc == NULL)
        {
            rc = JENXIO;
            goto Error;
        }

        rc = core_send_data(ctx->core_ctx, ctx->intr_pipe.desc, req);
    }

Error:
    if (rc)
    {
        if(req)
          core_request_free(req);
    }

    return rc;
}

/**
 * Function name:  notify_network_connection_complete
 * Description:  completion handler for a NETWORK_CONNECTION notification
 * Parameters: 
 *     @req: (IN) CORE request
 *     
 * Scope: local
 **/
static void notify_network_connection_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    ecm_callbacks_t *cbs = &(ctx->cbs.ecm);

    DBG_X(DSLAVE_CDC_ECM, ("notify_network_connection_complete: entered\n"));

    if (cbs->notify_net_conn_complete_h)
    {
        cdc_iostatus_t status = cdc_get_iostatus(req, 1);
        cbs->notify_net_conn_complete_h(status, ctx->app_ctx);
    }

    core_request_free(req);
}

/**
 * Function name:  ecm_notify_network_connection
 * Description:  Control Interface API - sends a NETWORK_CONNECTION notification
 * Parameters: 
 *     @handle: (IN) CDC handle
 *     @link_up: (IN) 0 = link down, 1 = link up
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t ecm_notify_network_connection(cdc_handle_t handle, jbool_t link_up)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
    request_t *req = NULL;
    jresult_t rc = 0;
#ifdef FEATURE_HS_USB_ECM_INT_EP_SHARING    
    cdc_ctx_t *shared_intr_ctx = NULL;
#endif /* FEATURE_HS_USB_ECM_INT_EP_SHARING */

    if (ctx->in_pipe.desc == NULL)
    {
        rc = JENXIO;
        goto Error;
    }

    req = cdc_create_notification(ctx, CDC_NTF_NETWORK_CONNECTION,
        link_up?1:0, ctx->in_pipe.if_num, 0, NULL);

    if (!req)
    {
        rc = JENOMEM;
        goto Error;
    }

    req->complete = notify_network_connection_complete;
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
            goto Error;
        }

        rc = core_send_data(ctx->core_ctx, shared_intr_ctx->intr_pipe.desc, req);
    }
    else
#endif /* FEATURE_HS_USB_ECM_INT_EP_SHARING */
    {
        if (ctx->intr_pipe.desc == NULL)
        {
            rc = JENXIO;
            goto Error;
        }

        rc = core_send_data(ctx->core_ctx, ctx->intr_pipe.desc, req);
    }

Error:
    if (rc)
    {
        if(req)
          core_request_free(req);
    }

    return rc;
}

/*====================
 *  Initialization
 *===================*/

/**
 * Function name:  ecm_get_functional_headers
 * Description:  this function is called by cdc_alloc_descriptors() to create
 *               the functional descriptors for ECM. It should:
 *               1. Return the total size required for the descriptors when
 *                  buffer is NULL
 *               2. Fill the buffer with descriptors when buffer is valid
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @buffer: (OUT) buffer into which functional headers should be written
 *     @is_single_iface: (IN) indicates a single interface 
 *     @interface_number: (IN) the max interface number allocated
 * Return value: total size, or 0 for failure
 * Scope: local
 **/
static juint32_t ecm_get_functional_headers(cdc_ctx_t *ctx, void *buffer,
    jbool_t is_single_iface, juint8_t interface_number)
{
    ecm_ctx_t *ecm_ctx = (ecm_ctx_t *)ctx;
    juint32_t size = 0;
    struct cdc_header_fdesc *hdr;
    struct cdc_ecm_fdesc *ecm;
    struct cdc_union_fdesc *un;
#ifdef FEATURE_HS_USB_MBIM
    struct cdc_mbb_fdesc *mbb;
    struct cdc_mbb_extended_fdesc *mbbExt;
#endif
    juint8_t avail_iface;
    juint8_t data_iface;

    DBG_X(DSLAVE_CDC_ECM, ("ecm_get_functional_headers entered\n"));

#ifdef FEATURE_HS_USB_MBIM
  if (ctx->info.basic.flags & CDC_INIT_MBIM_ECM)
  {
      size = sizeof(struct cdc_header_fdesc) +
          sizeof(struct cdc_union_fdesc) +
          sizeof(struct cdc_mbb_fdesc) 
          + sizeof(struct cdc_mbb_extended_fdesc);
  }
	else
#endif
	{
    size = sizeof(struct cdc_header_fdesc) +
        sizeof(struct cdc_ecm_fdesc) +
        sizeof(struct cdc_union_fdesc);
	}

  if (buffer)
  {
      avail_iface = interface_number;
      
      if (is_single_iface)
          data_iface = avail_iface;
      else 
          data_iface = avail_iface + 1;
   
      hdr = (struct cdc_header_fdesc*)buffer;
      hdr->bLength    = sizeof(struct cdc_header_fdesc);
      hdr->bType      = USB_DT_CS_INTERFACE;
      hdr->bSubtype   = USB_CDC_SUBTYPE_HEADER;
      ua_htole16(&hdr->bcdCDC, 0x0110);

        
#ifdef FEATURE_HS_USB_MBIM
      if (ctx->info.basic.flags & CDC_INIT_MBIM_ECM)
      {
          un = (struct cdc_union_fdesc*)(hdr+1);
          un->bLength             = sizeof(struct cdc_union_fdesc);
          un->bType               = USB_DT_CS_INTERFACE;
          un->bSubtype            = USB_CDC_SUBTYPE_UNION;
          un->bMasterIface        = avail_iface;
          un->bSlaveIface[0]      = data_iface;
          
          mbb = (struct cdc_mbb_fdesc*)(un+1);
          mbb->bLength             = sizeof(struct cdc_mbb_fdesc);
          mbb->bType               = USB_DT_CS_INTERFACE;
          mbb->bSubtype            = USB_CDC_SUBTYPE_MBIM;
          mbb->wMbbVersion         = USB_MBB_VERSION;
          mbb->wMaxControlMessage	 = USB_MAX_CONTROL_MESSAGE;
          /* 16 filters with size 128 bytes each as specified in Win log requirement */
          mbb->bNumberPowerFilters = USB_NUMBER_POWER_FILTERS;
          mbb->bMaxFilterSize      = USB_MAX_FILTER_SIZE;
          mbb->wMaxSegmentSize     = USB_MAX_SEGMENT_SIZE;
          /* Set/Get Max Datagram size request is currently not supported. */
          mbb->bmNetworkCapabilities = USB_NETWORK_CAPABILITIES;  

          mbbExt = (struct cdc_mbb_extended_fdesc*)(mbb+1);
          mbbExt->bLength                = sizeof(struct cdc_mbb_extended_fdesc);
          mbbExt->bType                  = USB_DT_CS_INTERFACE;
          mbbExt->bSubtype               = USB_CDC_SUBTYPE_MBIM_EXT;
          mbbExt->wMbbExtVersion         = USB_MBB_VERSION;
          mbbExt->bMaxOutstandingCmdMsgs = USB_MAX_OUTSTANDING_CMDMSG_SIZE;
          mbbExt->wMTU                   = hsu_conf_sel_mbim_wMTU();
      }
	else
#endif
      {
            ecm = (struct cdc_ecm_fdesc*)(hdr+1);
            ecm->bLength = sizeof(struct cdc_ecm_fdesc);
            ecm->bType = USB_DT_CS_INTERFACE;
            ecm->bSubtype = USB_CDC_SUBTYPE_ECM;
            ecm->iMACAddress = core_add_string_descriptor(ctx->core_ctx,
                ctx->info.ecm.mac_addr, HSU_STR_IDX_NOT_SET);
            
            /* save string descriptor Index is ecm struct  */
            ecm_ctx->iMACAddress = ecm->iMACAddress;
            
            ua_htole32(&ecm->bmEtherStats, ctx->info.ecm.eth_statistics);
            ua_htole16(&ecm->wMaxSegSize, ctx->info.basic.max_segment_size);
            ua_htole16(&ecm->wNumMCFilters, 
          (*(juint16_t*)(&ctx->info.ecm.mc_filters)));
            ecm->bNumPowerFilters = ctx->info.ecm.num_power_filters;

            un = (struct cdc_union_fdesc*)(ecm+1);
            un->bLength             = sizeof(struct cdc_union_fdesc);
            un->bType               = USB_DT_CS_INTERFACE;
            un->bSubtype            = USB_CDC_SUBTYPE_UNION;
            un->bMasterIface        = avail_iface;
            un->bSlaveIface[0]      = data_iface;
      }
  }

    return size;
}

/**
 * Function name:  ecm_disable
 * Description:  USB CORE 'disable' callback (overrides default in cdc_fd.c)
 * Parameters: 
 *     @context: (IN) CDC context
 *
 * Return value: None
 * Scope: local
 **/
static void ecm_disable(context_t context)
{
#ifdef FEATURE_HS_USB_ECM_INT_EP_SHARING    
    cdc_ctx_t *shared_intr_ctx = NULL;
#endif /* FEATURE_HS_USB_ECM_INT_EP_SHARING */

    cdc_ctx_t *ctx = (cdc_ctx_t*)context;
    
    DBG_X(DSLAVE_CDC_ECM, ("ecm_disable: entered\n"));

    if (!ctx)
        return;

    if (ctx->cbs.basic.disabled_h && ctx->app_enabled)
    {
        ctx->app_enabled = 0;
        ctx->cbs.basic.disabled_h(ctx->app_ctx);
    }

    cdc_disable(ctx);

#ifdef FEATURE_HS_USB_ECM_INT_EP_SHARING    
    if(hsu_config_selector_is_ecm_int_ep_sharing_supported())
    {
      shared_intr_ctx = (cdc_ctx_t*)hsu_al_ecm_get_shared_intr_pipe_handle();

      if (ctx == shared_intr_ctx)
      {
        hsu_ecm_fd_is_intr_pipe_configured = FALSE;
      }
    }
#endif /* FEATURE_HS_USB_ECM_INT_EP_SHARING */
}

/**
 * Function name:  ecm_enable
 * Description:  USB CORE 'enable' callback (overrides default in cdc_fd.c)
 * Parameters: 
 *     @context: (IN) CDC context
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t ecm_enable(context_t context)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;
    jresult_t rc = 0;
    jint_t data_if_num = 1;

    DBG_X(DSLAVE_CDC_ECM, ("ecm_enable: entered\n"));

    if (!ctx)
        return JEINVAL;

    rc = cdc_enable(ctx);
    if (rc)
        return rc;

    if (ctx->info.basic.flags & CDC_INIT_SINGLE_INTERFACE)
        data_if_num = 0;

    /* first factor in the bellow expression is HSU addition:
       API to disable alt. settings */
    if (((ctx->info.basic.flags & CDC_INIT_NO_ALT_SETTINGS) ||
        (ctx->desc->interfaces[data_if_num].alt_if_curr != 0)) &&
        ctx->cbs.basic.enabled_h)
    {
        rc = ctx->cbs.basic.enabled_h(ctx->app_ctx);
        if (!rc)
            ctx->app_enabled = 1;
    }

    return rc;
}

/**
 * Function name:  cdc_uninit_ecm
 * Description: ECM FD uninitialization - release string descriptor
 * Parameters:
 *     @context: (IN) ecm context  
 * Return value: 0 for success, or an error code
 * Scope: local
 * **/
static jresult_t cdc_uninit_ecm(context_t context)
{
    ecm_ctx_t *ecm_ctx = (ecm_ctx_t *)context;
    cdc_ctx_t *cdc_ctx = (cdc_ctx_t *)ecm_ctx;
    void *core_ctx = cdc_ctx->core_ctx; 

    HSU_ULOG_1(NO_MSG, BAM_LOG, "ecm_ctx %u", (uint32)ecm_ctx);
    
    core_release_string_descriptor(core_ctx,ecm_ctx->iMACAddress); 
    ecm_ctx->iMACAddress = 0;

    return cdc_uninit(context);
}

/**
 * Function name:  cdc_init_ecm
 * Description: ECM FD initialization
 * Parameters: 
 *     @core_ctx: (IN) USB CORE context
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t cdc_init_ecm(void *core_ctx)
{
    ecm_ctx_t *ecm_ctx = NULL;
    cdc_ctx_t *ctx = NULL;
    cdc_desc_init_t desc_info;
    jresult_t rc = 0;
    cdc_callbacks_t cbs;

    /* Create our context */
    ecm_ctx = (ecm_ctx_t *)jmalloc(sizeof(ecm_ctx_t), M_ZERO);
    ctx = (cdc_ctx_t *)ecm_ctx;
    if (!ctx)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, failed_to_allocate");        
        rc = JENOMEM;
        goto Error;
    }
    
    ctx->core_ctx = core_ctx;
    
    /* Call application init function */
    ctx->info.basic.interface_number = UWD_UNDEFINED_INTERFACE;
    rc = ecm_init(ctx, &ctx->cbs.ecm, &ctx->info.ecm,
        &ctx->app_ctx);
    if (rc)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, app_failed_to_init");
        goto Error;
    }

    cbs = ctx->cbs.basic;
    KASSERT((cbs.write_multi_complete_h && !cbs.write_complete_h) || 
        (!cbs.write_multi_complete_h && cbs.write_complete_h),
        ("using both single and multiple requests is not supported\n"));

    j_memset(&desc_info, 0, sizeof(cdc_desc_init_t));
    desc_info.cc_handler = ecm_handle_request;

#ifdef FEATURE_HS_USB_ECM_INT_EP_SHARING    
    /*  We create an Interrupt pipe only for the first ECM instance, 
    * we are going to use the same  intr pipe for other ECM instances
    * for NOTIFY_RESPONSE_AVAILABLE and other notifications with the 
    * right interface number    */
    if(hsu_config_selector_is_ecm_int_ep_sharing_supported())
    {
        if(!hsu_ecm_fd_is_intr_pipe_configured)
        {
            hsu_ecm_fd_is_intr_pipe_configured = TRUE;
            desc_info.cc_ntf_supported = TRUE;
        }
    }
    else
#endif /* FEATURE_HS_USB_ECM_INT_EP_SHARING */
    {
        desc_info.cc_ntf_supported = TRUE;
    }

#ifdef FEATURE_HS_USB_INTR_ONLY_ECM_PIPE
    /* Incase this is a INTR only instance of ECM then we do not need the BUILK eps*/
    if (!(ctx->info.basic.flags & CDC_INIT_INTR_ONLY_ECM_PIPE))
#endif/* FEATURE_HS_USB_INTR_ONLY_ECM_PIPE */
    {
        desc_info.dc_in_supported = TRUE;
        desc_info.dc_out_supported = TRUE;
    }

    if (ctx->info.basic.flags & CDC_INIT_DPL)
    {
      desc_info.cc_ntf_supported = FALSE;
      desc_info.dc_in_supported = TRUE;
      desc_info.dc_out_supported = FALSE;
    }
      
    /* HSU addition: API to disable alt. settings */
    desc_info.dc_empty_alt_0 = 
      ((ctx->info.ecm.basic.flags & CDC_INIT_NO_ALT_SETTINGS) ? FALSE : TRUE);

    if (ctx->info.ecm.basic.flags & CDC_INIT_VENDOR_SPECIFIC_CLASS)
    {
        desc_info.cci_subclass = ctx->info.basic.cci_subclass;
        desc_info.cci_protocol = ctx->info.basic.cci_protocol;
    }
    else
    {
#ifdef FEATURE_HS_USB_MBIM
      if (ctx->info.basic.flags & CDC_INIT_MBIM_ECM)
      {
          desc_info.cci_subclass = USB_CDC_SUBCLASS_MBIM;
          desc_info.cci_protocol = USB_CDC_PROTOCOL_MBIM;
      }
      else
#endif
      {
        desc_info.cci_subclass = USB_CDC_SUBCLASS_ECM;
        desc_info.cci_protocol = USB_CDC_CTRL_PROTO_NONE;
      }
    }

    if (!(ctx->info.ecm.basic.flags & CDC_INIT_NO_CLASS_DESC))
        desc_info.get_func_headers = ecm_get_functional_headers;

    if (ctx->info.ecm.basic.flags & CDC_INIT_SINGLE_INTERFACE)
        desc_info.is_single_iface = TRUE;
    else 
        desc_info.is_single_iface = FALSE;

    desc_info.interface_number = ctx->info.basic.interface_number;

    /* Create descriptors */
    rc = cdc_alloc_descriptors(ctx, &desc_info);
    if (rc)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, failed_to_allocate_descriptors");        
        goto Error;
    }

    /* Override enable/disable/uninit default handlers */
    ctx->ops.enable = ecm_enable;
    ctx->ops.disable = ecm_disable;
    ctx->ops.uninit = cdc_uninit_ecm;

    /* Initialize CDC functionality */
    rc = cdc_init(ctx);
    if (rc)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CDC_ECM, failed_on_cdc_init");        
        goto Error;
    }

    return rc;
    
Error:
    if (ctx)
    {
        cdc_free_descriptors(ctx);
        jfree(ctx);
    }
 
    return rc;
}

