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
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/core/src/jslave/core/jusb_core.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================









************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#include "jusb_common_int.h"
#include "jusb_core_fd_int.h"
#include "jos_int.h"
#include "jslave_init.h"
#include "jusb_core.h"
#include <uw_args.h>
#include "hsu_os_abstract_bus_ext.h"
#include "hsu_log.h"
#include "hsu_core_config_int.h"
#include "hsu_config_selector.h"
#include "hsu_platform.h"
#include "hsu_lpm_gpio.h"
#ifdef FEATURE_HS_USB_IPA_LOG_EFS
#include "ipa_log_efs.h"
#endif // FEATURE_HS_USB_IPA_LOG_EFS
#include "hsu_platform.h"
#include "hsu_vregmgr.h"
#include "hsu_conf_sel_i.h"
#include "hsu_al_ecm.h"

#define CORE_RECONFIG_DELAY_MS 100

/* TODO - consider move this to header file */
#ifdef CONFIG_FD_MASS
jresult_t mass_storage_init(void *ctx);
#endif

#ifdef CONFIG_FD_LOOPBACK
jresult_t loopback_init(void *ctx);
#endif

#ifdef CONFIG_FD_TEST
jresult_t fd_test_init(void *ctx);
#endif

#ifdef CONFIG_FD_SERIAL
jresult_t serial_init(void *ctx);
#endif

#ifdef CONFIG_FD_HID_KEYBOARD
jresult_t hid_kbd_init(void *ctx);
#endif

#ifdef CONFIG_FD_CDC_ACM
jresult_t cdc_init_acm(void *ctx);
#endif
    
#ifdef CONFIG_FD_CDC_ECM
jresult_t cdc_init_ecm(void *ctx);
#endif
    
#ifdef CONFIG_FD_CDC_OBEX
jresult_t cdc_init_obex(void *ctx);
#endif

#ifdef CONFIG_FD_VIDEO
jresult_t video_init(void *ctx);
#endif

#ifdef CONFIG_FD_DFU
jresult_t dfu_init(void *ctx);
#endif

#ifdef CONFIG_FD_SICD
jresult_t sicd_init(void *ctx);
#endif

#ifdef CONFIG_FD_AUDIO
jresult_t fd_audio_init(void *ctx);
#endif

static jresult_t register_finish(core_t *core);

/* HSU Addition */
jresult_t core_handle_ep0(void *core_ctx, juint8_t *buffer, device_speed_t speed);
void core_disconnect(void *core_ctx);
jresult_t core_suspend(void *core_ctx);
jresult_t core_resume(void *core_ctx);
static jresult_t core_enable(core_t *core);
static void core_disable(core_t *core);

static const core_events_t core_events = {
  core_handle_ep0,
  core_disconnect,
  NULL,
  core_suspend,
  core_resume,
  NULL,
  core_enable,
  core_disable,
};
/* HSU Addition End */

/* Cores maintenance */
static core_states_t global_core_state = CORE_NOT_INITIALIZED;
static core_t *cores;
static juint8_t ncores;


const juint16_t MAX_PACKET_SIZES[4][4] = { 
/* Control      : */ { 8,       64,     64,     512  },
/* Isoch        : */ { 0,       1023,   1024,   1024 },
/* Bulk         : */ { 0,       64,     512,    1024 },
/* Interrupt    : */ { 8,       64,     1024,   1024 }};


/**
* Function name:  core_get_max_packet_size
* Description: Retrieves max packet size from speed vs type array
*
* Parameters: 
*     @ep_desc_type: (IN) Endpoint descriptor type
*     @speed       : (IN) Speed  
*
* Return value: Max packet size for current endpoint type for given speed.
* Scope: Global
**/
juint16_t core_get_max_packet_size(pipe_type_t ep_desc_type, device_speed_t speed)
{

  juint16_t max_packet_size = 0;

  // Check index limits for EP type and speed.
  if(ep_desc_type <= PIPE_INTR && speed < SPEED_UNKNOWN)
  {
    max_packet_size =  MAX_PACKET_SIZES[ep_desc_type][speed];
  }
  else
  {
    HSU_ERR_FATAL("core_get_max_packet_size: Invalid EP type = %d or device speed = %d", ep_desc_type, 
      speed, 0);
  }

  return max_packet_size;
}


/**
 * Function name:  find_core_by_idx
 * Description: Locate the core context the match <dcd_idx>
 * Parameters: 
 *     @dcd_idx: (IN) index (zero based) of the controller 
 *
 * Return value: Core context or NULL of not found
 * Scope: local
 **/
static __INLINE__ core_t *find_core_by_idx(juint8_t dcd_idx)
{
    if (dcd_idx >= ncores)
        return NULL;

    return &cores[dcd_idx];
}


/* HSU Addition */
/**
* Function name:  find_core_by_number
* Description: Locate the core context that matches the 
*              provided core number
* Parameters: 
*     @core_number: (IN) Controller assigned number
*
* Return value: Core context or NULL of not found
* Scope: local
**/
static __INLINE__ core_t *find_core_by_number(juint8_t core_number)
{
  juint8_t dcd_idx;

  for (dcd_idx = 0; dcd_idx < ncores; dcd_idx++)
  {
    if (cores[dcd_idx].core_number == core_number)
    {
      return &cores[dcd_idx];
    }
  }

  return NULL;
  
}
void core_function_request_remote_wakeup_all_iface(juint8_t dcd_idx)
{
  core_t* core;
  if (dcd_idx >= ncores)
  {
    return;
  }
  core = &cores[dcd_idx];
  if (core->curr_speed == SPEED_SUPER)
  {
    uint32 i;
    interface_t iface;
    for (i = 0; i < core->configs[core->curr_config - 1].interface_count; i++)
    {
      iface = core->configs[core->curr_config - 1].interfaces[i];

      /*Wakeup the function only if function is Suspend, and not request yet. Due to window 7, remote wakeup if any function_remote_wakeup is enabled  */
      if ((iface.is_function_suspend) && core_is_func_remote_wake_enabled(core))
      {
        HSU_ULOG_1(NO_MSG, BUS_LOG, "calling_function_rm_wk_req, iface %u", i);
        core->dcd_ops->dcd_function_wakeup_request(core->dcd_dev, i);  
      }
    }
  }
}

/**
* Function name:  core_get_curr_speed
* Description: Locate the core context that matches the 
*              provided HAL core number
* Parameters: 
*     @core_number: (IN) Controller assigned number
*
* Return value: Core context or SPEED_UNKNOWN when core is not found
* Scope: Global
**/
device_speed_t core_get_curr_speed(juint8_t core_number)
{
  device_speed_t speed = SPEED_UNKNOWN;
  core_t* core = find_core_by_number(core_number);

  if (core != NULL)
  {
    speed = core->curr_speed;
  }

  return speed;
}

/**
* Function name:  core_get_curr_speed_by_dcd_idx
* Description: Locate the core context that matches the 
*              provided HAL core number
* Parameters: 
*     @core_number: (IN) Controller assigned number
*
* Return value: Core context or SPEED_UNKNOWN when core is not found
* Scope: Global
**/
device_speed_t core_get_curr_speed_by_dcd_idx(juint8_t dcd_idx)
{
  device_speed_t speed = SPEED_UNKNOWN;
  core_t* core = find_core_by_idx(dcd_idx);

  if (core != NULL)
  {
    speed = core->curr_speed;
  }
  
  return speed;
}

/* HSU Addition - End */


#ifdef CONFIG_REMOTE_WAKEUP_TIMEOUT
/**
 * Function name:  core_remote_wakeup_task
 * Description: Called if remote wakeup request was sent, but there was no
 *              resume from the host - cancel all queued requests.
 * Parameters: 
 *     @ctx: (IN) CORE context
 *     
 * Scope: local
 **/
static void core_remote_wakeup_task(void *ctx)
{
    jint_t i, j;
    core_t *core = (core_t *)ctx;

    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "cancel_IN_pipes");

    hsu_event_log(EVT_REMOTE_WAKEUP_FAIL, core->core_index);

    /* Abort control pipe transfer */
    core_abort_pipe(core, NULL);

    if (!core->curr_config)
        goto Exit;

    /* Abort all IN / NON-HW accelerated data pipes transfers */
    for (i = 0; i < core->configs[core->curr_config - 1].interface_count; i++)
    {
        alt_interface_desc_t *ifc =
            core->configs[core->curr_config - 1].interfaces[i].if_curr;
        
        for (j = 0; j < ifc->pipe_count; j++)
        {
            pipe_desc_t *pipe = &ifc->pipes[j];
            if ( (FALSE == STAILQ_EMPTY(&pipe->req_queue))
              && (FALSE == pipe->is_pipe_hw_accelerated)
              && (DIRECTION_IN == pipe->direction) )
            {
                HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "abort_pipe, IN pipe 0x%x", pipe->address);
                core_abort_pipe(core, pipe);
            }
        }
    }

Exit:
    core->remote_wakeup_scheduled = 0;
}
#endif

/**
 * Function name:  core_update_descriptors
 * Description: Updates descriptors
 * Parameters:
 *     @core: (IN) CORE context
 *     @ep_desc: (INOUT) Endpoint context
 *
 * Return value: None
 * Scope: Global
 **/
void core_update_descriptors(const core_t *core, pipe_desc_t* ep_desc)
{
   switch (core->curr_speed)
   {
   case SPEED_SUPER:
      // Remove reseting of burst here as it overwrites the values populated in the CDC
      // ep_desc->ss_attributes.max_burst = USB_SUPERSPEED_BULK_EP_MAXBURST;
      if (PIPE_ISOC ==  ep_desc->type || PIPE_INTR == ep_desc->type)
      {
         ep_desc->ss_attributes.max_burst = USB_SUPERSPEED_INTR_EP_MAXBURST;
		 ep_desc->ss_attributes.bytes_per_interval = ep_desc->max_pkt_size_super
            * (ep_desc->transaction_per_microframe + 1) * (ep_desc->ss_attributes.max_burst + 1);
      }
      break;
   case SPEED_HIGH:
      if (PIPE_ISOC ==  ep_desc->type || PIPE_INTR == ep_desc->type)
      {
         ep_desc->ss_attributes.bytes_per_interval = ep_desc->max_pkt_size_high
            * (ep_desc->transaction_per_microframe + 1);
      }
      break;
   case SPEED_FULL:
      if (PIPE_ISOC ==  ep_desc->type)
      {
         ep_desc->ss_attributes.bytes_per_interval = ep_desc->max_pkt_size_full;
      }
      break;
   default:
      break;
   }
}

/**
 * Function name:  core_handle_tx_completion
 * Description: Called by the function driver to handle "IN" timeout in the DCD
 * Parameters: 
 *     @ctx: (IN) CORE context
 *     @pipe: (IN) CORE pipe context
 *
 * Return value: None
 * Scope: Global
 **/
void core_handle_tx_completion(void *ctx, pipe_desc_t *pipe)
{
    core_t *core = (core_t *)ctx;

    if (core->dcd_ops->dcd_handle_tx_completion)
        core->dcd_ops->dcd_handle_tx_completion(core->dcd_dev, pipe);
}

/**
 * Function name:  send_io_request
 * Description: Send new request. In case the pipe request queue is empty, send
 *              the transfer to the DCD, otherwise only queue it.
 * Parameters: 
 *     @core: (IN) CORE context
 *     @pipe: (IN) CORE pipe context
 *     @request: (IN) CORE request
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static __INLINE__ jresult_t send_io_request(core_t *core, pipe_desc_t *pipe, 
    request_t *request)
{
    static boolean hsu_test_force_esuspended = FALSE;

    jbool_t queue_empty = 0;
    jresult_t rc = 0;
    request_t *curr_request;
    uint8 iface_idx =0;
    jbool_t function_suspend = FALSE;
    
    LIST_HEAD(req_list_head, request_t) req_list;

    LIST_FIRST(&req_list) = request;

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_V(DSLAVE_CORE, ("Send IO\n"));
#endif /* CONFIG_DATA_PATH_DEBUG */

    /* Make sure the device is in proper state for sending data */
    if (core->core_state != CORE_ENABLED && 
        core->core_state != CORE_SUSPENDED &&
        core->usb_state != USB_CONFIGURED)
    {
        if (request->direction == DIRECTION_IN)
        {
            HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, "
                "attempt_to_SEND_data, Core/USB_not_in_proper_state 0x%02X", core->usb_state); 
        }
        else
        {
            HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, "
              "attempt_to_READ_data, Core/USB_not_in_proper_state 0x%02X", core->usb_state);           
        }

        rc = JEPERM;
        goto Exit;
    }

    if (pipe && pipe->status == PIPE_ABORTING)
    {
        if (request->direction == DIRECTION_IN)
        {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, "
                "attempt_to_SEND_data, while_pipe_aborting"); 
        }
        else
        {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, "
                "attempt_to_READ_data, while_pipe_aborting");           
        }
        rc = JEPERM;
        goto Exit;
    }
    
    if (pipe && core->curr_speed == SPEED_SUPER)
    {
      core->dcd_ops->dcd_get_iface_index(pipe, &iface_idx);
      function_suspend = core->configs[core->curr_config - 1].interfaces[iface_idx].is_function_suspend;
    }

#ifdef CONFIG_REMOTE_WAKEUP_TIMEOUT
    if ((core->core_state == CORE_SUSPENDED) || function_suspend)
    {
        if ( hsu_test_force_esuspended 
          || (!core->remote_wakeup_enabled && (FALSE == core_is_func_remote_wake_enabled(core))) )
        {           
            if (request->direction == DIRECTION_IN)
            {
                HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, "
                    "attempt_to_SEND_data, DCD_suspend_mode, remote_wakeup_not_enabled_by_host"); 
            }
            else
            {
                HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, "
                    "attempt_to_READ_data, DCD_suspend_mode, remote_wakeup_not_enabled_by_host");           
            }

            
            rc = ESUSPENDED;
            goto Exit;
        }                     

        /* Only IN request can wake the host */
        if (request->direction == DIRECTION_OUT)
        {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, "
                 "attempt_to_READ_data, DCD_suspend_mode");    
            rc = ESUSPENDED;
            goto Exit;
        }

        if (request->sps_mode)
        {
            HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "attempt_to_send_data_in_suspend, "
              "sps_mode, req 0x%08X", (uint32)request);
            rc = ESUSPENDED;
            goto Exit;
        }
    }
#else
    if (core->core_state == CORE_SUSPENDED)
    {     
        if (request->direction == DIRECTION_IN)
        {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, "
                "attempt_to_SEND_data, DCD_suspend_mode"); 
        }
        else
        {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, "
                "attempt_to_READ_data, DCD_suspend_mode");           
        }
        rc = ESUSPENDED;
        goto Exit;
    }
#endif

    LIST_FOREACH(curr_request, &req_list, next_request_in_list)
    {
        /* check request is not in use */
        if (curr_request->status != REQUEST_READY)
        {
            HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "CORE, "
                "attempt_to_use_not_ready_req 0x%08X, size %u",
                (uint32) curr_request, curr_request->transfer_size); 
            rc = JEBUSY;
            goto Exit;
        }

        /* check buffer validity */
        if (
/* HSU addition: added sps_mode to the expression*/
#ifdef FEATURE_HS_USB_BAM
            !curr_request->sps_mode &&
#endif
            curr_request->transfer_size && !curr_request->buffer.vaddr)
        {
        if (request->direction == DIRECTION_IN)
        {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, bad_buffer_passed_to_SEND_data"); 
        }
        else
        {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, bad_buffer_passed_to_READ_data");            
        }
            rc = JEINVAL;
            goto Exit;
        }
    }

#ifdef CONFIG_REMOTE_WAKEUP_TIMEOUT
    /* Try to wakeup the host - wait for resume CONFIG_REMOTE_WAKEUP_TIMEOUT 
     * ms */
    if (core->core_state == CORE_SUSPENDED)
    {
        DBG_I(DSLAVE_CORE, ("CORE in SUSPEND mode!\n"));
        if (!core->remote_wakeup_scheduled)
        {
            HSU_ULOG(NO_MSG, BUS_LOG, "DEVICE_SUSPEND, wakeup_host");
            rc = jslave_wakeup_host(core->core_number);
            if (rc)
            {
                rc = ESUSPENDED;
                goto Exit;
            }
            #ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
            hsu_task_schedule_log(JTASK_SCH_SEND_IO_REQUEST_LOG, HSU_NON_APPLICABLE_VAL_LOG);
            #endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */
            rc = jtask_schedule(core->remote_wakeup_task,
                CONFIG_REMOTE_WAKEUP_TIMEOUT, core_remote_wakeup_task, core);
            if (rc)
            {
                HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, "
                    "jtask_schedule, remote_wakeup_task_failed, rc %u", rc);                  
                goto Exit;
            }
            core->remote_wakeup_scheduled = 1;
        }
    }

    /* If Dev is SS, need to check if the request is access on the function that is in FUNCTION_SUSPEND.
       If function_suspend and allow remote wakeup, then request remote wakeup.
    */
    if (function_suspend)
    {
      /*Wakeup the function only if function is Suspend, and not request yet. Due to window 7, remote wakeup if any function_remote_wakeup is enabled  */
      core->dcd_ops->dcd_function_wakeup_request(core->dcd_dev, iface_idx);

      //SPS(QMI) mode does not support function suspend.
      if (FALSE == request->sps_mode)
      {
        HSU_ULOG_1(NO_MSG, BUS_LOG, "function_suspend, queue_request, iface_idx %u", (uint8) iface_idx);
      }
      else
      {
        HSU_ULOG_1(NO_MSG, BUS_LOG, "function_suspend, sps_mode, dont_queue_request, iface_idx %u", (uint8) iface_idx);
      }
    }
#endif

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DSLAVE_CORE, ("Sending a new io %s pipe (0x%x) "
      "%d\n", request->single_request ?
      "single request" : "request list", pipe ? pipe->address : 0));
#endif /* CONFIG_DATA_PATH_DEBUG */

    LIST_FOREACH(curr_request, &req_list, next_request_in_list)
    {
        curr_request->status = REQUEST_INPROGRESS;
        curr_request->bytes_transferred = 0;
    }

    if (pipe && (request->direction == DIRECTION_OUT || 
        request->single_request || core->remote_wakeup_scheduled || (pipe->type == PIPE_ISOC) || 
        (function_suspend && (FALSE == request->sps_mode))))
    {
        //HSU_ULOG_6(NO_MSG, BUS_LOG, "Queue pipe, iface %u, pipe_req_queue 0x%X, req 0x%X, pipe 0x%X, first_req 0x%X, last_req 0x%X", (uint8) iface_idx, (uint32) &pipe->req_queue,
        //  (uint32) request, (uint8) pipe->address, (uint32) (&pipe->req_queue)->stqh_first, (uint32) *((&pipe->req_queue)->stqh_last));
        
        queue_empty = STAILQ_EMPTY(&pipe->req_queue);
#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_X(DSLAVE_CORE, ("send_io_request: queueing request on ep 0x%x. "
          "queue is %sempty\n", pipe->address, queue_empty ? "" : "NOT "));
#endif /* CONFIG_DATA_PATH_DEBUG */
        STAILQ_INSERT_TAIL(&pipe->req_queue, request, next_request);
    }

    if ((!core->remote_wakeup_scheduled) && (!(function_suspend && (FALSE == request->sps_mode))))
    {
      if (!pipe
          || (pipe && (pipe->type == PIPE_ISOC))
          || queue_empty
          || (!(request->direction == DIRECTION_IN) && (!request->single_request))
          || (pipe && (request->direction == DIRECTION_IN) && (pipe->type == PIPE_BULK))
         )
      {
        rc = core->dcd_ops->dcd_send_io_request(core->dcd_dev, pipe, request);
      }
    }

Exit:
    return rc;
}

/**
 * Function name:  core_send_data
 * Description: Send data to the host
 * Parameters: 
 *     @ctx: (IN) DCD context 
 *     @pipe: (IN) CORE pipe context
 *     @request: (IN) CORE request
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t core_send_data(void *ctx, pipe_desc_t *pipe, request_t *request)
{
    request_t *curr_request;
    LIST_HEAD(req_list_head, request_t) req_list;

    /*HSU Addition KW: request may be NULL*/
    HSU_ASSERT(request);

    LIST_FIRST(&req_list) = request;

    LIST_FOREACH(curr_request, &req_list, next_request_in_list)
        curr_request->direction = DIRECTION_IN;

    return send_io_request((core_t *)ctx, pipe, request);
}

/**
 * Function name:  core_read_data
 * Description: Read data from the host
 * Parameters: 
 *     @ctx: (IN) DCD context  
 *     @pipe: (IN) CORE pipe context
 *     @request: (IN) CORE request
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t core_read_data(void *ctx, pipe_desc_t *pipe, request_t *request)
{
    request_t *curr_request;
    LIST_HEAD(req_list_head, request_t) req_list;

    /*HSU Addition KW: request may be NULL*/
    HSU_ASSERT(request);

    LIST_FIRST(&req_list) = request;

    LIST_FOREACH(curr_request, &req_list, next_request_in_list)
        curr_request->direction = DIRECTION_OUT;

    return send_io_request((core_t *)ctx, pipe, request);
}

/**
 * Function name:  core_stall_pipe
 * Description: Set/Clear STALL on the pipe
 * Parameters: 
 *     @ctx: (IN) DCD context  
 *     @pipe: (IN) CORE pipe context
 *     @flag: (IN) 1 for stall the endpoint, 0 for clear stalled endpoint
 *
 * Return value: None
 * Scope: global
 **/
void core_stall_pipe(void *ctx, pipe_desc_t *pipe, juint8_t flag)
{
    core_t *core = (core_t *)ctx;
    jresult_t rc;

    rc = core->dcd_ops->dcd_stall_ep(core->dcd_dev, pipe, flag);
    if (rc)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, DCD_failed_stall_pipe, rc %u", rc);  
    }
}

/**
 * Function name:  core_abort_pipe
 * Description: Cancel all pending request on the pipe. All completion callback
 *              will be called with request status set to REQUEST_CANCELLED
 * Parameters: 
 *     @ctx: (IN) DCD context
 *     @pipe: (IN) CORE pipe context
 *
 * Return value: None
 * Scope: global
 **/
void core_abort_pipe(void *ctx, pipe_desc_t *pipe)
{
    core_t *core = (core_t *)ctx;
    jresult_t rc;

    rc = core->dcd_ops->dcd_abort_ep(core->dcd_dev, pipe);
    if (rc)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, DCD_failed_to_abort_ep, rc %u", rc);  
    }
}

/**
 * Function name:  init_fd
 * Description: Call to the specific function driver initialization function
 * Parameters: 
 *     @core: (IN) CORE context
 *     @type: (IN) function driver type 
 *     @count: (IN) how many instances of the function driver
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t init_fd(core_t *core, fd_instance_t *fd_instance)
{
    jresult_t rc = 0;
    jint_t i;

    for (i = 0; i < fd_instance->count; i++)
    {
        switch (fd_instance->type)
        {
        #ifdef CONFIG_FD_MASS
        case DEVICE_TYPE_MSD:
            {
                rc = mass_storage_init(core);
                if (rc)
                    goto Error;
                break;
            }
        #endif

        #ifdef CONFIG_FD_LOOPBACK
        case DEVICE_TYPE_LOOPBACK:
            rc = loopback_init(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_SERIAL
        case DEVICE_TYPE_SERIAL:
            rc = serial_init(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_HID_KEYBOARD
        case DEVICE_TYPE_HID:
                rc = hid_kbd_init(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_TEST
        case DEVICE_TYPE_TEST:
            rc = fd_test_init(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_CDC_ACM
        case DEVICE_TYPE_CDC_ACM:
            rc = cdc_init_acm(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_CDC_ECM
        case DEVICE_TYPE_CDC_ECM:
            rc = cdc_init_ecm(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_CDC_OBEX
        case DEVICE_TYPE_CDC_OBEX:
            rc = cdc_init_obex(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_VIDEO
        case DEVICE_TYPE_VIDEO:
            rc = video_init(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_DFU
        case DEVICE_TYPE_DFU:
            rc = dfu_init(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_SICD
        case DEVICE_TYPE_SICD:
            rc = sicd_init(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_AUDIO
        case DEVICE_TYPE_AUDIO:
            rc = fd_audio_init(core);
            if (rc)
                goto Error;
            break;
        #endif

        case DEVICE_TYPE_VENDOR:
            if (fd_instance->init_func)
            {
                rc = fd_instance->init_func(core);
            }
            else 
            {
                HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, "
                    "got_DEVICE_TYPE_VENDOR_with_no_init_callback");                  
                rc = JEINVAL;
            }
            if (rc)
                goto Error;
            break;
        default:
            HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, "
                    "unknown_FD_type %u", fd_instance->type);   
            rc = JEINVAL;
            break;
        }
    }

Error:
    
    return rc;
}

/**
 * Function name:  init_fds
 * Description: Initialize all relevant FD's for the specific core
 * Parameters: 
 *     @core: (IN) CORE context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t init_fds(core_t *core)
{
    juint_t i, j;
    jresult_t rc = 0;

    core->configs = jmalloc(sizeof(config_t) *
        core->device_param.num_of_configs, M_ZERO);
    if (!core->configs)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, failed_allocating_config_list");           
        return JENOMEM;
    }

    core->config_count = core->device_param.num_of_configs;

    DBG_V(DSLAVE_CORE ,("init_fds: config count %d\n", core->config_count));
    for (i = 0; i < core->config_count; i++)
    {
        DBG_V(DSLAVE_CORE ,("init_fds: config #%d\n", i)); 
        core->curr_config = i + 1;
        for (j = 0; j < core->device_param.config_param[i].num_of_fd_instances;
            j++)
        {
            fd_instance_t *fd_instance =
                &core->device_param.config_param[i].fd_instances[j];

            rc = init_fd(core, fd_instance);
            if (rc)
            {
                HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, "
                    "failed_init_FD, type %u", fd_instance->type);                  
                goto Error;
            }
        }

        if (core->device_param.config_param[i].config_name)
        {
            core->configs[i].i_config = core_add_string_descriptor(core,
                core->device_param.config_param[i].config_name, HSU_STR_IDX_NOT_SET);
        }
    }

    core->curr_config = 0;
    core->core_state = CORE_FD_CONFIGURED;

    rc = register_finish(core);
    if (rc)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, register_finish_failed");
        goto Error;
    }

    return 0;

Error:
    for (i = 0; i < core->config_count; i++)
    {
        if (core->configs[i].i_config)
            core_release_string_descriptor(core, core->configs[i].i_config);

        jfree(core->configs);
        core->configs = NULL;
        core->config_count = 0;
    }

    return rc;
}

/**
 * Function name:  register_finish
 * Description: Called after all function driver have been initialized and
 *              registered with core. Allocate and build the device descriptors
 *              according to the various function drivers.
 * Parameters: 
 *     @core: (IN) CORE context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t register_finish(core_t *core)
{
    juint8_t idx;

    DBG_V(DSLAVE_CORE, ("Finishing registration process\n"));

    if (core->core_state != CORE_FD_CONFIGURED)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, called_before_CORE_FD_CONFIGURED,"
          " state %u", core->core_state);        
        return JEPERM;
    }

    for (idx = 0; idx < core->config_count; idx++)
    {
        juint8_t i, j, fd_count;
        fd_t *fd;
        config_t *config = &core->configs[idx];

        DBG_V(DSLAVE_CORE, ("registering config %d\n", idx));
        /* Initialize the interface list */
        config->interfaces = jmalloc(sizeof(interface_t) *
            config->interface_count, M_ZERO);
        if (!config->interfaces)
            goto Error;

        /* Fill up Interface data */
        for (fd_count = 0, i = 0, j = 0, fd = config->fd_list;
            fd && i < config->interface_count;
            i++, j++)
        {
            /* Each FD might handle a few interfaces */
            if (j >= fd->fd_desc->interface_count)
            {
                fd = fd->next;
                fd_count++;
                j = 0;
            }

            DBG_V(DSLAVE_CORE, ("Adding interface %d FD %d\n", i, fd_count));
            config->interfaces[i].fd_if_num = j;
            config->interfaces[i].fd      = fd;

            /* Checking if we received an interface number from the application.
             * If we did setting it as the interface number, if we didn't
             * allocation interface number */
            if (fd->fd_desc->interfaces[j].number != UWD_UNDEFINED_INTERFACE)
            {
                KASSERT(core->device_param.static_interface_number, ("Interface"
                    " number should be allocated by the core %d\n", 
                    core->device_param.static_interface_number));
                config->interfaces[i].number =
                    fd->fd_desc->interfaces[j].number;
            }
            else
            {
                KASSERT(!core->device_param.static_interface_number,
                    ("Interface number should be allocated by the "
                    "applications\n"));
                config->interfaces[i].number  = i;
                fd->fd_desc->interfaces[j].number = i;
            }

            config->interfaces[i].if_curr =
                &fd->fd_desc->interfaces[j].alt_ifs[0];
            config->interfaces[i].if_arr  = &fd->fd_desc->interfaces[j];
            config->interfaces[i].handle_usb_cmd = 
                fd->fd_desc->interfaces[j].control_msg;

            /* Set first interface to be alt-set 0 */
            fd->fd_desc->interfaces[j].alt_if_curr = 0;
        }
    }

    core->core_state = CORE_CONFIGURED;

    return 0;

Error:
    for (; idx; idx--)
        jfree(core->configs[idx - 1].interfaces);

    return JENOMEM;
}

/**
 * Function name:  core_register_fd
 * Description: Called by the function driver to register with the CORE
 * Parameters: 
 *     @ctx: (IN) CORE context 
 *     @ops: (IN) function driver callbacks
 *     @context: (IN) function driver context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t core_register_fd(void *ctx, fd_ops_t *ops, context_t context)
{
    core_t *core = (core_t *)ctx;
    fd_t *fd, **tmp;
    jint_t i, j, k;
    jresult_t rc;
    juint8_t interface_number;

    DBG_V(DSLAVE_CORE, ("Register FD\n"));

    /* Is this the right time ? */
    if (core->core_state != CORE_DCD_CONFIGURED)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, reg_FD_at_wrong_core_state 0x%02X",
          core->core_state);          
        return JEPERM;
    }

    /* Allocate new FD struct */
    fd = jmalloc(sizeof(fd_t), M_ZERO);
    if (!fd)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, cannot_alloc_memory_for_FD_structure");
        return JENOMEM;
    }

    fd->fd_ops  = ops;
    fd->context = context;
    fd->next    = NULL;
    fd->state   = FD_DISABLED;
   
    /* Retrieve FD's requested configuration */
    fd->fd_desc = fd->fd_ops->get_pipe_config(fd->context);
    if (!fd->fd_desc)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, get_no_pipe_config_from_FD");
        jfree(fd);
        return JEINVAL;
    }

    DBG_V(DSLAVE_CORE, ("Got Pipe config. Interface count = %d\n",
        fd->fd_desc->interface_count));

    /* Alloc pipes for each new interface */
    for (i = 0; i < fd->fd_desc->interface_count; i++)
    {
        if (fd->fd_desc->interfaces[i].number == 
            UWD_UNDEFINED_INTERFACE)
        {
            interface_number =
                core->configs[core->curr_config - 1].interface_count + i;
        }
        else
        {
            interface_number = fd->fd_desc->interfaces[i].number;
        }

       /* For each alternate setting */
        for (j = 0; j < fd->fd_desc->interfaces[i].alt_if_num; j++)
        {
            /* For each endpoint */
            for (k = 0; 
                k < fd->fd_desc->interfaces[i].alt_ifs[j].pipe_count; 
                k++)
            {
                pipe_desc_t *pipe = 
                    &fd->fd_desc->interfaces[i].alt_ifs[j].pipes[k];

                rc = core->dcd_ops->dcd_alloc_pipe(core->dcd_dev,
                    pipe, core->curr_config, interface_number, (juint8_t)j);
                if (rc)
                {
                    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, error_allocating_pipe_for_FD, rc %u", rc);               
                    return rc;
                }

                STAILQ_INIT(&pipe->req_queue);
            }
        }
    }

    /* Calculate total interface count */
    core->configs[core->curr_config - 1].interface_count +=
        fd->fd_desc->interface_count;

    /* Update max suported speed */
    if (core->max_speed > fd->fd_desc->speed)
        core->max_speed = fd->fd_desc->speed;

    /* If an IAD is defined, we need to know it later */
    if (fd->fd_desc->iad)
        core->iad_defined = 1;
        
    /* Add FD to the end of the list of registered FDs 
     * This affects interface number assignment */
    for (tmp = &core->configs[core->curr_config - 1].fd_list; *tmp;
        tmp = &((*tmp)->next))
        ;

    *tmp = fd;

    DBG_V(DSLAVE_CORE, ("FD registered! Core state = %02x\n", 
        core->core_state));

    return 0;
}

/**
 * Function name:  core_get_available_iface
 * Description: Return the next free interface number that can be used by the
 *              function driver
 * Parameters: 
 *     @ctx: (IN) CORE context
 *
 * Return value: The index of the next free interface
 * Scope: global
 **/
juint8_t core_get_available_iface(void *ctx)
{
    core_t *core = (core_t *)ctx;

    KASSERT(core->curr_config, ("CORE: calling core_get_available_iface "
        "outside Init sequence\n"));
    return core->configs[core->curr_config - 1].interface_count;
}

/**
 * Function name:  core_request_alloc
 * Description: Allocate new CORE request, including a data buffer (in case
 *              <address> is NULL
 *              HSU addition: For allocating CORE request which will be used 
 *              for SPS mode only, caller can set buffer_size and address to 0
 * Parameters: 
 *     @buffer_size: (IN) size of data buffer
 *     @address: (IN) data buffer address
 *
 * Return value: pointer to the new allocated request, or NULL in case of an
 *               error
 * Scope: global
 **/
request_t *core_request_alloc(juint32_t buffer_size, void *address)
{
  return core_request_alloc_ex(buffer_size, address, 0);
}

request_t *core_request_alloc_ex(juint32_t buffer_size, void *address, juint16_t align)
{
    request_t *request;

    request = jmalloc(sizeof(request_t), M_ZERO);
    if (!request)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, failed_to_allocate_req");        
        return NULL;
    }

    hsu_trace_msg1_set(CORE_REQUEST_ALLOC_EX_JDMA);

    if (address)
    {
        request->buffer.vaddr = request->buffer.dma_addr = 
            request->buffer.cookie = address;
    }
    else
    {
        jresult_t rc = jdma_alloc(buffer_size, align, &request->buffer.vaddr, 
            &request->buffer.dma_addr, 0, &request->buffer.cookie);

       hsu_mem_log(MEM_JDMA__REQUEST_LOG, request->buffer.cookie, buffer_size);

        if (rc)
        {
            HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, failed_to_allocate_buff_for_req, size %u byte", buffer_size);            
            jfree(request);
            return NULL;
        }
        request->is_buffer_allocated = 1;
    }

    hsu_trace_msg1_clear();

    request->buffer.buffer_size = buffer_size;
    request->completed_req = 0;
    request->timeout = 0;
    request->single_request = 1;
    LIST_NEXT(request, next_request_in_list) = NULL;

    return request;
}

/**
 * Function name:  core_request_alloc_for_ep0
 * Description: Allocate new CORE request, including a data buffer (in case
 *              <address> is NULL
 *              Separate allocation to re-use control endpoint's data stage  
 *              buffer to avoid fragmentation by jdma_alloc()
 *
 * Parameters: 
 *     @buffer_size: (IN) size of data buffer
 *     @core_ctx:    (IN) core context which contains ep0_data_buffer
 *
 * Return value: pointer to the new allocated request, or NULL in case of an
 *               error
 * Scope: global
 **/
request_t *core_request_alloc_for_ep0(juint32_t buffer_size, void *core_ctx)
{
  request_t *request;

  core_t *core = (core_t*)core_ctx;

  KASSERT((core),"core_null");

  request = jmalloc(sizeof(request_t), M_ZERO);
  if (!request)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, failed_to_allocate_req");        
    return NULL;
  }

  if (buffer_size > core->ep0_data_buffer.buffer_size)
  {
    HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "CORE, buff_size_req_too_big, %d > %d",
      buffer_size, core->ep0_data_buffer.buffer_size);
    jfree(request);
    return NULL;
  }
    
  request->buffer.vaddr = core->ep0_data_buffer.vaddr;
  request->buffer.dma_addr = core->ep0_data_buffer.dma_addr;
  request->buffer.cookie = core->ep0_data_buffer.cookie;
  request->buffer.buffer_size = buffer_size;

  request->is_control_ep = TRUE;
  request->is_buffer_allocated = 1;
  request->completed_req = 0;
  request->timeout = 0;
  request->single_request = 1;
  LIST_NEXT(request, next_request_in_list) = NULL;

  return request;
}


/**
 * Function name:  core_request_free
 * Description: Free previously allocate request
 * Parameters: 
 *     @request: (IN) CORE request
 *
 * Return value: None
 * Scope: global
 **/
void core_request_free(request_t *request)
{
    request_t *curr_request, *prev_req;
    if (!request)
        return;

    curr_request = request;
    while (curr_request)
    {
        if (curr_request->is_buffer_allocated && curr_request->buffer.cookie && (FALSE == request->is_control_ep))
            jdma_free(curr_request->buffer.cookie);
        prev_req = curr_request;
        curr_request = LIST_NEXT(curr_request, next_request_in_list);
        jfree(prev_req);
    }
   }

/**
 * Function name:  core_handle_ep0
 * Description: Called by DCD upon endpoint 0 transfer completion
 * Parameters: 
 *     @core_ctx: (IN) CORE context
 *     @buffer: (IN) data buffer
 *     @speed: (IN) device speed
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t core_handle_ep0(void *core_ctx, juint8_t *buffer, device_speed_t speed)
{
    core_t *core = (core_t *)core_ctx;

    DBG_V(DSLAVE_CORE, ("Handle EP\n"));

    /* Increment the EP0 request tag (so we can only answer the latest EP0) */
    core->ep0_req_tag++;
    
    if (core->core_state != CORE_ENABLED)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, ep0_msg_at_wrong_core_state 0x%02X", core->core_state);        
        return JEPERM;
    }

    if (core->usb_state != USB_CONFIGURED && core->usb_state != USB_ADDRESS)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, ep0_msg_at_wrong_usb_device_state 0x%02X", core->usb_state);
        return JEPERM;
    }

    if (!buffer)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, bad_buffer(0x%08X) passed_to_handle_ep0", (uint32) buffer);
        return JEINVAL;
    }

    if (speed != core->curr_speed)
    {
      core->curr_speed = speed;
      jnotify_os(NOTIFY_DEVICE_SPEED_CHANGE, (void*)core->core_index);
    }

    /* HSU Addition */
    if (SPEED_UNKNOWN != core->curr_speed)
    {      
      if (NULL != core->ep0_handler)
      {
        HSU_ULOG(NO_MSG, BUS_LOG, "ep0_handler_not_empty");
        return core->ep0_handler(core, buffer, core->ep0_req_tag);
      }
      else
      {
        return chp9_handle_usb_command(core, buffer, core->ep0_req_tag);
      }
    }
    else
    {
      // Disconnect already occured.
      // Skip handling of remnant USB command.
      // Do not enqueue new SETUP packet or reset the control state machine.
      // Next connect will restart control state machine.
      HSU_ULOG(NO_MSG, BUS_LOG, "disc_skip_ep0_hndler");
      return HSU_JSUCCESS;
    }
    /* HSU Addition */
}

/**
 * Function name:  jslave_wakeup_host
 * Description: Called to issue a remote wakeup to the host
 * Parameters: 
 *     @core_number: (IN) Number of the core to enable
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t jslave_wakeup_host(juint8_t core_number)
{
    core_t *core = find_core_by_idx(core_get_controller_idx(core_number));
                        
    if (!core)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, trying_to_wakeup_dcd, core_is_null");        
        return JEINVAL;
    } 

    if (!core->dcd_ops->wake_the_host)
        return JENOTSUP;

    if (!core->remote_wakeup_enabled && (FALSE == core_is_func_remote_wake_enabled(core)))
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, remote_wakeup_not_enabled_by_host");
        return JEINVAL;
    }

    return core->dcd_ops->wake_the_host(core->dcd_dev);
}

/**
 * Function name:  core_suspend
 * Description: Handle suspend from the bus
 * Parameters: 
 *     @core_ctx: (IN) CORE context
 *     @bus: (IN) DCD bus handle
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t core_suspend(void *core_ctx)
{
    core_t *core = (core_t *)core_ctx;
    core->core_state = CORE_SUSPENDED;

    /* Abort current control transfer */
    //core->dcd_ops->dcd_abort_ep(core->dcd_dev, NULL);
    core->request->status = REQUEST_READY;

    /* Notify function drivers on suspend */
    if (core->curr_config)
    {
        fd_t *fd;
        uint32 fd_idx =0;

        for (fd = core->configs[core->curr_config - 1].fd_list, fd_idx=0; fd;
            fd = fd->next, fd_idx++)
        {
            HSU_ASSERT(fd_idx < core->configs->interface_count);
            if (fd->fd_ops && fd->fd_ops->suspend)
            {
              if (FALSE == core->configs[core->curr_config -1].interfaces[fd_idx].is_function_suspend)
              {
                //HSU_ULOG_2(NO_MSG, BUS_LOG, "core_suspend, fd 0x%X, fd_context 0x%x", (uint32) fd, (uint32) fd->context);
                fd->fd_ops->suspend(fd->context);
              }
              else
              //If the function is FUNCTION_SUSPEND state, the cdc_suspend callback was already invoked  
              {
                HSU_ULOG_2(NO_MSG, BUS_LOG, "skip_already_func_susp, fd 0x%X, fd_context 0x%x", (uint32) fd, (uint32) fd->context);
              }
              #ifdef FEATURE_HS_USB_IPA_LOG_EFS
              IPA_LOG_EFS_INCREASE_COUNTER(IPA_LOG_EFS_ID_USB_SUSPEND_COUNTER);
              #endif // FEATURE_HS_USB_IPA_LOG_EFS
            }
        }
    }

    /* Notify application on bus suspend */
    jnotify_os(NOTIFY_DEVICE_SUSPEND, j_device_get_bus(core->dcd_dev));

    return 0; 
}

/**
 * Function name:  core_resume
 * Description: Handle resume from the bus
 * Parameters: 
 *     @core_ctx: (IN) CORE context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t core_resume(void *core_ctx)
{
    core_t *core = (core_t *)core_ctx;
    jbus_h dcd_bus = j_device_get_bus(core->dcd_dev);
#ifdef FEATURE_HS_USB_F3
    hsu_platform_info_type const *platform_info= hsu_platform_get_info();
#endif /* FEATURE_HS_USB_F3 */

    /* Notify application on resume signal */
    jnotify_os(NOTIFY_DEVICE_RESUME, dcd_bus);

    core->core_state = CORE_ENABLED;
    
#ifdef CONFIG_REMOTE_WAKEUP_TIMEOUT
    if (core->remote_wakeup_scheduled)
    {
        jtask_stop(core->remote_wakeup_task);
        core->remote_wakeup_scheduled = 0;

        HSU_ULOG(NO_MSG, BUS_LOG, "remote_wakeup");

        if (core->curr_config)
        {
            jint_t i;
            config_t *config = &core->configs[core->curr_config - 1];

            for (i = 0; i < config->interface_count; i++)
            {
                if (config->interfaces[i].is_function_suspend)
                {
                  HSU_ULOG_1(NO_MSG, BUS_LOG, "function_suspend, pipe_not_restore, iface %u", i);
                }
                else
                {
                  alt_interface_desc_t *ifc = config->interfaces[i].if_curr;
                  jresult_t rc = core_resend_interface_pipe(ifc, core);
                  if (rc)
                  {
                    return rc;
                  }
                }
            }
        }
    }
#endif

    /* Notify function drivers on resume */
    if (core->curr_config)
    {
        fd_t *fd;
        uint32 fd_idx;
        for (fd = core->configs[core->curr_config - 1].fd_list, fd_idx=0; fd; 
            fd = fd->next, fd_idx++)
        {
            HSU_ASSERT(fd_idx < core->configs->interface_count);
            if (fd->state == FD_ENABLED && fd->fd_ops && fd->fd_ops->resume)
            {
                if ((FALSE == core->configs[core->curr_config -1].interfaces[fd_idx].is_function_suspend))
                {
                  //HSU_ULOG_2(NO_MSG, BUS_LOG, " notify_function_drive_resume, FD 0x%08X, fd_context 0x%08X", (uint32) fd, (uint32) fd->context);
                  fd->fd_ops->resume(fd->context);
                }
                //If the function is FUNCTION_SUSPEND state, then the function will remain FUNCTION_SUSPEND after bus wakeup
                else
                {
                  HSU_ULOG_2(NO_MSG, BUS_LOG, "skip_FD_resume, due_to_function_suspend, FD 0x%08X, fd_context 0x%08X", (uint32) fd, (uint32) fd->context);
                }
                #ifdef FEATURE_HS_USB_IPA_LOG_EFS
                IPA_LOG_EFS_INCREASE_COUNTER(IPA_LOG_EFS_ID_USB_RESUME_COUNTER);
                #endif // FEATURE_HS_USB_IPA_LOG_EFS
            }
        }
    }

#ifdef FEATURE_HS_USB_F3
    if(core->core_number == HSU_CORE_IDX_PRIMARY)
    {
      if (platform_info->gpio_remote_wakeup)
      {
        hsu_lpm_gpio_remote_wakeup_gpio_low();
      }
    }
#endif /* FEATURE_HS_USB_F3 */
    /* Notify application that the core is now resumed */
    jnotify_os(NOTIFY_DEVICE_RESUME_COMPLETED, dcd_bus);

    return 0; 
}

/**
 * Function name:  core_disconnect
 * Description: Handle disconnecting from the bus
 * Parameters: 
 *     @core_ctx: (IN) CORE context
 *
 * Return value: None
 * Scope: global
 **/
void core_disconnect(void *core_ctx)
{
    core_t *core = (core_t *)core_ctx;
    jresult_t rc;

    if ((core->core_state != CORE_ENABLED) &&
        (core->core_state != CORE_CONFIGURED && 
        (core->core_state != CORE_SUSPENDED)))
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, usb_disconnect_at_wrong_core_state 0x%02X", core->core_state);        
    }

    /* Disable REMOTE_WAKEUP capability */
    core->remote_wakeup_enabled = 0;

    rc = chp9_handle_set_configuration(core, 0);
    if (rc)
    {
        DBG_W(DSLAVE_CORE, ("core_unregister_dcd: Failed to set "
            "configuration 0 (%d), continue anyway\n", rc));
    }

    /* Abort control pipe transfer */
    core_abort_pipe(core, NULL);

    core->core_state = CORE_ENABLED;

    DBG_V(DSLAVE_CORE, ("Core: Disconnect %d\n", rc));
}

/**
 * Function name:  core_register_dcd
 * Description: called by DCD to register with the CORE
 * Parameters: 
 *     @dev: (IN) DCD context
 *     @dcd_ops: (IN) DCD callbacks
 *     @core_ctx: (OUT) CORE context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t core_register_dcd(jdevice_t dev, dcd_ops_t *dcd_ops, void **core_ctx)
{
    core_t *core = NULL;
    jresult_t rc;
    jint_t i;
    hsu_platform_info_type const *platform_info = hsu_platform_get_info();

    DBG_V(DSLAVE_CORE, ("CORE: Registering DCD\n"));
    
    if (global_core_state != CORE_INITIALIZED)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, attempt_reg_DCD_at_wrong_core_state 0x%02X", global_core_state);         
        return JEPERM;
    }

    /* Find the first core that has yet to assign dcd_ops */
    for (i = 0; i < ncores; i++)
    {
        if (!cores[i].dcd_ops)
        {
            core = &cores[i];
            break;
        }
    }
    if (!core)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, core_is_null");
        rc = JEINVAL;
        goto Error;
    }

    DBG_V(DSLAVE_CORE, ("core_register_dcd: adding core 0x%x\n", core));

    if (core_ctx)
        *core_ctx = core;

/* Temp fix for B47218 */

rc = dev_app_init(core->app_ctx, &(core->app_cbs));
if (rc)
{
  HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, cannot_init_dev_app, rc %u", rc);  
  goto Error;
}

/* Temp fix END */


    core->request = core_request_alloc(MAX_DESCRIPTOR_SIZE, NULL);
    if (!core->request)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, cannot_allocte_mem_for_core_req");
        rc = JENOMEM;
        goto Error;
    }
#ifdef CONFIG_REMOTE_WAKEUP_TIMEOUT
    rc = jtask_init(&core->remote_wakeup_task, TASK_CONTROLLER);
    if (rc)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, cannot_init_remote_wakeup_task, rc %u", rc);         
        goto Error;
    }
#endif
#ifdef CONFIG_REMOTE_WAKEUP_TEST
    rc = jtask_init(&core->remote_wakeup_test_task, TASK_CONTROLLER);
    if (rc)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, cannot_init_remote_wakeup_test_task, rc %u", rc);        
        goto Error;
    }
#endif
    rc = jtask_init(&core->reconfig_task, TASK_CONTROLLER);
    if (rc)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, cannot_init_reconfig_task, rc %u", rc);     
        goto Error;
    }
    rc = jtask_init(&core->func_wake_tnotif_task, TASK_CONTROLLER);
    if (rc)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, cannot_init_function_wake_tNotification_task, rc %u", rc);        
        goto Error;
    }

    core->core_state = CORE_DCD_CONFIGURED;
    core->usb_state = USB_ADDRESS;
    
    switch (platform_info->hsu_platform_id)
    {
      case HSU_PLATFORM_VIRTIO:
      case HSU_PLATFORM_RUMI:
      {
        core->max_speed = SPEED_HIGH;
      }
      break;
      default:
      {
        core->max_speed = SPEED_HIGH;
      }
      break;
    }
    
    core->dcd_ops = dcd_ops;
    core->dcd_dev = dev;
    core->iad_defined = 0;

    rc = init_fds(core);
    if (rc)
        goto Error;

    /* HSU Addition */
    if (core->dcd_ops->dcd_register_events)
    {
      core->dcd_ops->dcd_register_events(dev, &core_events);
    }        
    /* HSU Addition End */

    DBG_V(DSLAVE_CORE, ("CORE: DCD registered ! Core state = %02x\n",
        core->core_state));

    return 0;

Error:
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, failed");
    return rc;
}

static void uninit_fds(core_t *core)
{
    fd_t *fd, *temp;
    jint_t i, j, k, l;
    
    for (l = 0; l < core->config_count; l++)
    {
        for (fd = core->configs[l].fd_list; fd; fd = temp)
        {
            /* Free all allocated pipes */
            for (i = 0; i < fd->fd_desc->interface_count; i++)
            {
                /* For each alternate setting */
                for (j = 0; j < fd->fd_desc->interfaces[i].alt_if_num; j++)
                {
                    /* For each endpoint */
                    for (k = 0; 
                        k < fd->fd_desc->interfaces[i].alt_ifs[j].pipe_count; 
                        k++)
                    {
                        pipe_desc_t *pipe = 
                            &fd->fd_desc->interfaces[i].alt_ifs[j].pipes[k];

                        core->dcd_ops->dcd_free_pipe(core->dcd_dev, pipe);
                    }
                }
            }

            if (fd->fd_ops)
                fd->fd_ops->uninit(fd->context);

            temp = fd->next;
            jfree(fd);
        }

        /* Free interfaces */
        if (core->configs[l].interfaces)
            jfree(core->configs[l].interfaces);

        core->configs[l].interface_count = 0;
        core->configs[l].fd_list = NULL;

        if (core->configs[l].i_config)
            core_release_string_descriptor(core, core->configs[l].i_config);
    }

    if (core->configs)  /* HSU addition: fix crash when jstart_stack fails */
    {
      jfree(core->configs);
      core->configs = NULL;
    }
    core->curr_config = 0;
    core->core_state = CORE_DCD_CONFIGURED;
}

/**
 * Function name:  core_unregister_dcd
 * Description: called by DCD to un-register with the CORE
 * Parameters: 
 *     @core_ctx: (IN) CORE context
 *
 * Return value: None
 * Scope: global
 **/
void core_unregister_dcd(void *core_ctx)
{
    core_t *core = (core_t *)core_ctx;
    jresult_t rc;

    if (!core)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, core_is_null");        
        return;
    }

    rc = chp9_handle_set_configuration(core, 0);
    if (rc)
    {
        DBG_W(DSLAVE_CORE, ("core_unregister_dcd: Failed to set "
            "configuration 0 (%d), continue anyway\n", rc));
    }

    uninit_fds(core);

    if (core->func_wake_tnotif_task)
        jtask_uninit(core->func_wake_tnotif_task);
#ifdef CONFIG_REMOTE_WAKEUP_TIMEOUT
    if (core->remote_wakeup_task)
        jtask_uninit(core->remote_wakeup_task);
#endif
#ifdef CONFIG_REMOTE_WAKEUP_TEST
    if (core->remote_wakeup_test_task)
        jtask_uninit(core->remote_wakeup_test_task);
#endif
    if (core->reconfig_task)
        jtask_uninit(core->reconfig_task);

    if (core->request)
        core_request_free(core->request);

    core->dcd_ops = NULL;
}

static jbool_t check_idx_is_duplicated(uw_hw_core_param_t *hw_core_params, 
    juint8_t num_of_cores)
{
    jint_t i, j;

    for (i = 0; i < num_of_cores; i++)
    {
        for (j = i + 1; j < num_of_cores; j++)
        {
            if (hw_core_params[j].core_number == hw_core_params[i].core_number)
            {
                return 1;
            }
        }
    }

    return 0;
}

/**
 * Function name:  core_init
 * Description: CORE initialization function. Initialize CORE contexts list, and
 *              save argument for later use
 * Parameters: 
 *     @devices: (IN) array of device_params_t
 *     @num_of_devices: (IN) number of elements in <devices>
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t core_init(uw_hw_core_param_t *hw_core_params, juint8_t num_of_cores)
{
    jint_t i;
    core_t *core;
    device_params_t *device_param, *src_device_param; 
    jresult_t rc;

    DBG_I(DSLAVE_CORE, ("Starting core init...\n"));

    if (check_idx_is_duplicated(hw_core_params, num_of_cores))
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, received_duplicated_controller_indices");           
        return JEINVAL;
    }

    ncores = num_of_cores;
    cores = (core_t *)jmalloc(sizeof(core_t) * ncores, M_ZERO);
    if (!cores)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, failed_to_allocate_cores_list");          
        goto Error;
    }

    for (i = 0; i < ncores; i++)
    {
        jint_t j;

        /* Added by mshaked - host controller doesn't need device parameters */      
        if (hw_core_params[i].init_opt == UW_INIT_HOST)
        {
          continue;
        }
        core = &cores[i];
        /* HSU Addition */
        core->ep0_handler = NULL;
        core->core_index = hsu_hal_os_bus_core_id(hw_core_params[i].core_number);

        core->core_number = hw_core_params[i].core_number;

        src_device_param = hw_core_params[i].device_param;
        
        device_param = &core->device_param;
        device_param->dev_info = (dev_info_t *)jmalloc
            (sizeof(dev_info_t), M_ZERO);
        if (!device_param->dev_info)
            goto Error;
        j_memcpy(device_param->dev_info, src_device_param->dev_info, 
            sizeof(dev_info_t));
        
        device_param->num_of_configs = src_device_param->num_of_configs;
        device_param->config_param = jmalloc(sizeof(config_param_t) *
            device_param->num_of_configs, M_ZERO);
        if (!device_param->config_param)
            goto Error;

        for (j = 0; j < device_param->num_of_configs; j++)
        {
            config_param_t *config_param = &device_param->config_param[j];
            config_param_t *src_config_param =
                &src_device_param->config_param[j];

            DBG_I(DSLAVE_CORE, ("core_init: config #%d\n", j));
            config_param->fd_instances = jmalloc(sizeof(fd_instance_t) * 
                src_config_param->num_of_fd_instances, M_ZERO);
            if (!config_param->fd_instances)
                goto Error;
            j_memcpy(config_param->fd_instances,
                src_config_param->fd_instances, sizeof(fd_instance_t) *
                src_config_param->num_of_fd_instances);

            config_param->num_of_fd_instances = 
                src_config_param->num_of_fd_instances;

            config_param->config_name = src_config_param->config_name;
            config_param->self_powered = src_config_param->self_powered;
            config_param->remote_wakeup = src_config_param->remote_wakeup;
            config_param->max_power = src_config_param->max_power;
        }

#ifndef CONFIG_FULL_SPEED_ONLY
        device_param->full_speed_only = src_device_param->full_speed_only;
#endif
        device_param->vendor_get_string_cb = 
            src_device_param->vendor_get_string_cb;
        device_param->vendor_cmd_in_cb = 
            src_device_param->vendor_cmd_in_cb;
        device_param->vendor_cmd_out_cb = 
            src_device_param->vendor_cmd_out_cb;
        device_param->vendor_cmd_out_data_stage_cb = 
            src_device_param->vendor_cmd_out_data_stage_cb;
        /*HSU addition*/
        device_param->reserved_cmd_in_cb = 
            src_device_param->reserved_cmd_in_cb;
        device_param->reserved_cmd_out_cb = 
            src_device_param->reserved_cmd_out_cb;
        device_param->reserved_cmd_out_data_stage_cb = 
            src_device_param->reserved_cmd_out_data_stage_cb;
        /*End of HSU addition*/

        /* HSU addition - standard_cmd_cb_t */
        device_param->standard_cmd_cb = 
            src_device_param->standard_cmd_cb;

        device_param->static_interface_number = 
            src_device_param->static_interface_number;

        hsu_trace_msg1_set(CORE_INIT__TRC);

        /* HSU addition - Initialize static buffer for EP0 */

        core->ep0_data_buffer.buffer_size = HSU_ENCAPS_CMD_MAX_SIZE;
        rc = jdma_alloc(core->ep0_data_buffer.buffer_size, 0, &core->ep0_data_buffer.vaddr, 
          &core->ep0_data_buffer.dma_addr, 0, &core->ep0_data_buffer.cookie);

        hsu_mem_log(MEM_JDMA__REQUEST_LOG, core->ep0_data_buffer.cookie, core->ep0_data_buffer.buffer_size);

        hsu_trace_msg1_clear();

        KASSERT((rc == HSU_JSUCCESS), "jdma_alloc_failed_for_ep_data_buffer");
    }

    /* Initialization finished */
    global_core_state = CORE_INITIALIZED;

    DBG_V(DSLAVE_CORE, ("Core initialized (%d cores initialized)\n", 
        num_of_cores));

    return 0;

Error:
    core_uninit();
    return JENOMEM;
}

/**
 * Function name:  core_uninit
 * Description: Uninitialized the CORE
 * Parameters: 
 *
 * Return value: None
 * Scope: global
 **/
void core_uninit(void)
{
    jint_t i;
    device_params_t *device_param;

    DBG_I(DSLAVE_CORE, ("Starting core un-init...\n"));
  
    /* Run as long as there are still elements in the cores list */
    for (i = 0; i < ncores; i++)
    {
        jint_t j;

        device_param = &cores[i].device_param;
        for (j = 0; j < device_param->num_of_configs; j++)
        {
            if (device_param->config_param[j].fd_instances)
                jfree(device_param->config_param[j].fd_instances);
        }
        /* HSU addition: fix crash when jstart_stack fails */
        if (device_param->config_param) 
        /* End of HSU addition */
            jfree(device_param->config_param);
        if (device_param->dev_info)
            jfree(device_param->dev_info);

        /* HSU addition: free ep0 static memory */
        cores[i].ep0_data_buffer.buffer_size = 0;
        jdma_free(cores[i].ep0_data_buffer.cookie);
    }

    if (cores)
    {
        jfree(cores);
        cores = NULL;
        ncores = 0;
    }

    global_core_state = CORE_NOT_INITIALIZED;
}

static jresult_t core_enable(core_t *core)
{
    jresult_t rc;
    jbool_t full_speed_dcd;
  
    if (!core)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, core_is_null");            
        return JEINVAL;
    }
    
    DBG_I(DSLAVE_CORE, ("core_enable: core = 0x%x (index %d, number %d), "
        "state %d\n", core, core->core_index, core->core_number, 
        core->core_state));
        
    if (core->core_state != CORE_CONFIGURED)
    {
        switch (core->core_state)
        {
        case CORE_ENABLED:
            return 0;
            
        case CORE_DCD_CONFIGURED:
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, enable_core_before_FDs_registerd");               
            break;
            
        case CORE_FD_CONFIGURED:
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, enable_core_before_register_finish_was_called");              
            break;

        default:
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, enable_core_at_unknown_state");              
            break;
        }
        
        return JEPERM;
    }

    /* If an FD is using an IAD, the device descriptor should be adjusted
     * accordingly to allow it to work */
    if (core->iad_defined)
    {
        dev_info_t *dev_info = core->device_param.dev_info;

        DBG_I(DSLAVE_CORE, ("CORE: using IAD compliant DEVICE DESCRIPTOR\n"));

        dev_info->dev_class     = USB_CLASS_MISC;
        dev_info->dev_sub_class = USB_MISC_SUBCLASS_COMMON;
        dev_info->dev_protocol  = USB_MISC_SUBCLASS_COMMON_PRTCL_IAD;
    }

    chp9_set_string_descriptors(core);

    /* Change core state */
    core->core_state = CORE_ENABLED;
    
    /* Enable the DCD */
#ifdef CONFIG_FULL_SPEED_ONLY
    full_speed_dcd = 1;
#else
    full_speed_dcd = core->device_param.full_speed_only;
#endif
    rc = core->dcd_ops->dcd_enable(core->dcd_dev, full_speed_dcd);
    if (rc)
        return rc;

    /* NOTE: FDs are enabled when SET_CONFIG(1) arrives */
    DBG_V(DSLAVE_CORE, ("Core enabled !\n"));

    return 0;
}

juint8_t core_get_curr_configuration(juint8_t core_number)
{
    core_t *core;

    core = find_core_by_idx(core_get_controller_idx(core_number));
    if (!core)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, core_is_null");         
        return 0;
    }

    return core->curr_config;
}

juint8_t core_get_controller_idx(juint8_t core_number)
{
    juint8_t i;

    for (i = 0; i < ncores; i++)
    {
        if (cores[i].core_number == core_number)
            break;
    }

    return i;
}


/**
 * Function name:  core_enable_by_idx
 * Description: Called by application to enable a specific controller
 * Parameters: 
 *     @core_index: (IN) index (zero based) of the controller to enable
 *
 * Return value: 0 on success, otherwise an error code  
 * Scope: global
 **/
jresult_t core_enable_by_idx(juint8_t core_index)
{
    return core_enable(find_core_by_idx(core_index));
}

/**
 * Function name:  core_enable_by_ctx
 * Description: Called by DCD to enable a specific controller
 * Parameters:
 *     @core_ctx: (IN) CORE context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t core_enable_by_ctx(void *core_ctx)
{
    return core_enable((core_t *)core_ctx);
}

static void core_disable(core_t *core)
{
    jresult_t rc = 0;
    
    if (!core)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, core_is_null");  
        return;
    }

    DBG_V(DSLAVE_CORE, ("CORE: Disabling core, core index = %d core number = "
        "%d\n", core->core_index, core->core_number));
    if (core->core_state != CORE_CONFIGURED && core->core_state != CORE_ENABLED 
        && core->core_state != CORE_SUSPENDED)
    {
        DBG_W(DSLAVE_CORE, ("Attempt to disable the core in wrong state." 
            " %02x\n", core->core_state));
        return;
    }

    /* Disable all FDs */
    rc = chp9_handle_set_configuration(core, 0);
    if (rc)
    {
        DBG_W(DSLAVE_CORE, ("core_disable: Failed to set configuration 0 (%d), "
            "continue anyway\n", rc));
    }
  
    /* Disable the DCD */
    core->dcd_ops->dcd_disable(core->dcd_dev);
 
    /* Change core state */
    core->core_state = CORE_CONFIGURED;
     
    DBG_V(DSLAVE_CORE, ("Core (index = %d  number = %d) disabled !\n", 
        core->core_index, core->core_number));
}

/**
 * Function name:  core_disable_by_idx
 * Description: Called by application to disable a specific controller
 * Parameters: 
 *     @core_index: (IN) index (zero based) of the controller to enable
 *
 * Return value: None
 * Scope: global
 **/
void core_disable_by_idx(juint8_t core_index)
{
    core_disable(find_core_by_idx(core_index));
}

/**
 * Function name:  core_disable_by_ctx
 * Description: Called by DCD to disable a specific controller
 * Parameters:
 *     @core_ctx: (IN) CORE context
 *
 * Return value: None
 * Scope:          Global
 **/
void core_disable_by_ctx(void *core_ctx)
{
    core_disable((core_t *)core_ctx);
}

static void core_reconfig_task(void *arg)
{
    jint_t i;
    jresult_t rc = 0;
    core_t *core = (core_t *)arg;
    device_params_t *args = &core->reconfig_args.device_param;
    device_params_t *device_param = &core->device_param;

    /* Update list of fd_instances for current Core */
    for (i = 0; i < device_param->num_of_configs; i++)
        jfree(device_param->config_param[i].fd_instances);

    jfree(device_param->config_param);
    device_param->config_param = args->config_param;
    device_param->num_of_configs = args->num_of_configs;

    /* Unconfigure the device */
    rc = chp9_handle_set_configuration(core, 0);
    if (rc)
    {
        DBG_W(DSLAVE_CORE, ("core_reconfig_task: Failed to set configuration "
            "0 (%d), continue anyway\n", rc));
    }

    /* HSU addition */
    /* Zero string descriptors array */
    chp9_clear_string_descriptors(core);
    /* End of HSU addition*/

    /* Disable and uninitialize current FD's */
    uninit_fds(core);

    /* Re-init the FD's according to new configuration */
    rc = init_fds(core);
    if (rc)
        goto Exit;

    /* Save dev_info */
    jfree(device_param->dev_info);
    device_param->dev_info = args->dev_info;

    core->core_state = core->reconfig_args.state_before_reconfig;
    if (core->core_state == CORE_ENABLED)
        chp9_set_string_descriptors(core);

    /* Reconnect to the host */
    if (core->dcd_ops->vbus_connect)
        core->dcd_ops->vbus_connect(core->dcd_dev);

Exit:                                                       

    DBG_I(DSLAVE_CORE, ("core_reconfig_task: Finish reconfigure the stack - "
        "status 0x%x\n", rc));

    core->reconfig_inprogress = 0;
    if (core->reconfig_args.cb)
        core->reconfig_args.cb(core->reconfig_args.arg, rc);
}

jresult_t core_reconfig(juint8_t core_index, device_params_t *new_config,
    reconfig_complete_cb_t cb, void *arg)
{
    jint_t i;
    core_t *core = find_core_by_idx(core_index);
    reconfig_arg_t *reconfig_args = NULL;
    jresult_t rc = 0;

    if (!core)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, core_is_null, core_index %u", core_index); 
        rc = JEINVAL;
        goto Exit;
    }

    if (core->reconfig_inprogress)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, reconfig_already_in_progress");         
        rc = JEINVAL;
        goto Exit;
    }

    reconfig_args = &core->reconfig_args;
    reconfig_args->device_param.num_of_configs = new_config->num_of_configs;
    reconfig_args->device_param.config_param = (config_param_t *)jmalloc
        (sizeof(config_param_t) * new_config->num_of_configs, M_ZERO);
    if (!reconfig_args->device_param.config_param)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, failed_allocate_new config_param array");          
        rc = JENOMEM;
        goto Exit;
    }

    for (i = 0; i < new_config->num_of_configs; i++)
    {
        reconfig_args->device_param.config_param[i].fd_instances =
            (fd_instance_t *)jmalloc(sizeof(fd_instance_t) *
            new_config->config_param[i].num_of_fd_instances, M_ZERO);
        if (!reconfig_args->device_param.config_param[i].fd_instances)
        {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, failed_allocate_new fd_instance array");             
            rc = JENOMEM;
            goto Exit;
        }
    }

    reconfig_args->device_param.dev_info = (dev_info_t *)jmalloc
        (sizeof(dev_info_t), M_ZERO);
    if (!reconfig_args->device_param.dev_info)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, failed_allocate_new device_info");         
        rc = JENOMEM;
        goto Exit;
    }
    reconfig_args->state_before_reconfig = core->core_state;

    core->reconfig_inprogress = 1;

    core->ep0_handler = NULL;

    /* Disconnect from the host */
    /* HSU Addition */
    if (core->dcd_ops)
    if (core->dcd_ops->vbus_disconnect)
        core->dcd_ops->vbus_disconnect(core->dcd_dev);

    /* Run the reconfigure process in a generic task to avoid infinite recursive
     * FD's disable */
    /* Copy the new configuration parameters to the core's context */
    for (i = 0; i < new_config->num_of_configs; i++)
    {
        j_memcpy(reconfig_args->device_param.config_param[i].fd_instances,
            new_config->config_param[i].fd_instances, sizeof(fd_instance_t) *
            new_config->config_param[i].num_of_fd_instances);
        reconfig_args->device_param.config_param[i].num_of_fd_instances = 
            new_config->config_param[i].num_of_fd_instances;
        reconfig_args->device_param.config_param[i].config_name =
            new_config->config_param[i].config_name;
        reconfig_args->device_param.config_param[i].self_powered =
            new_config->config_param[i].self_powered;
        reconfig_args->device_param.config_param[i].remote_wakeup =
            new_config->config_param[i].remote_wakeup;
        reconfig_args->device_param.config_param[i].max_power =
            new_config->config_param[i].max_power;
    }

    j_memcpy(reconfig_args->device_param.dev_info, new_config->dev_info, 
        sizeof(dev_info_t));

    core->reconfig_args.cb = cb;
    core->reconfig_args.arg = arg;

    /* HSU addition : avoid interference by calling core_reconfig_task without
       releasing mutex by jtask_schedule */
    if (rc && reconfig_args && reconfig_args->device_param.config_param)
    {
        for (i = 0; i < new_config->num_of_configs &&
            reconfig_args->device_param.config_param[i].fd_instances; i++)
        {
            jfree(reconfig_args->device_param.config_param[i].fd_instances);
        }

        jfree(reconfig_args->device_param.config_param);
    }

    hsu_os_locked_delay_ms(CORE_RECONFIG_DELAY_MS);
    
    core_reconfig_task(core);

    return rc;
    /* End of HSU addition  */
    
    /* HSU addition : block scheduling core_reconfig_task
    jtask_schedule(core->reconfig_task, CORE_RECONFIG_DELAY_MS, core_reconfig_task, core);
       End of HSU addition */

Exit:
    if (rc && reconfig_args && reconfig_args->device_param.config_param)
    {
        for (i = 0; i < new_config->num_of_configs &&
            reconfig_args->device_param.config_param[i].fd_instances; i++)
        {
            jfree(reconfig_args->device_param.config_param[i].fd_instances);
        }

        jfree(reconfig_args->device_param.config_param);
    }

    return rc;
}

/**
 * Function name:  get_controller_index
 * Description:   Return the Controller index of the specific core instance
 * Parameters:
 *     @core_ctx: (IN) CORE context
 *
 * Return value:  The specific core's controller index 
 * Scope:          Global
 **/
juint8_t core_get_controller_index(void *core_ctx)
{
    return ((core_t *)core_ctx)->core_index;
}


/* HSU Addition */
/**
* Function name:  get_controller_number
* Description:   Return the Controller number of the specific core instance
* Parameters:
*     @core_ctx: (IN) CORE context
*
* Return value:  The specific core's controller index 
* Scope:          Global
**/
juint8_t core_get_controller_number(void *core_ctx)
{
  return ((core_t *)core_ctx)->core_number;
}
/* HSU Addition - End */


/**
* Function name:  core_set_ep0_handler
* Description:   Registers a handler for an EP0 handler.
*                It allows external entity (i.e., BREW layer) to register 
*                a handler for EP0.
* Parameters:
*     @core_index: (IN) CORE index
*     @ep0_handler: (IN) handler for EP0.
* Return value:   0 on success, otherwise an error code  
* Scope:          Global
**/
jresult_t core_set_ep0_handler(juint8_t core_index, 
                               jresult_t (*ep0_handler)(core_t *core, 
                               void *buffer,juint8_t ep0_req_tag))
{
  core_t *core = find_core_by_idx(core_index);
  HSU_ASSERT(core);
  core->ep0_handler = ep0_handler;
  return 0;
}

/**
* Function name:  core_get_core_context
* Description:   
* Parameters:
* Return value:   
* Scope:          Global
**/

void *core_get_core_context(uint8 core_idx)
{
  void * ret;

  ret = find_core_by_idx(core_idx);

  return ret;
}

#ifdef FEATURE_HS_USB_POLLING
/**
* Function name:  core_set_fd_ops
* Description:   
* Parameters:
* Return value:   
* Scope:          Global
**/
void core_set_fd_ops(void *core_ctx, fd_ops_t *fd_ops)
{
  core_t *core = (core_t *)core_ctx;
  juint8_t i;

  for (i=0; i < core->config_count; i++)
  {
    fd_t *fd;

    for (fd = core->configs[i].fd_list; fd; fd = fd->next)
    {
      fd->fd_ops = fd_ops;
    }
  }
}

/**
* Function name:  core_get_pipe_desc
* Description:   
* Parameters:
* Return value:   
* Scope:          Global
**/
pipe_desc_t *core_get_pipe_desc(void *core_ctx, juint8_t direction, juint8_t ep_num)
{

  core_t *core = (core_t *)core_ctx;
  interface_desc_t * infs;
  juint8_t inf_cnt;
  juint8_t i;
  alt_interface_desc_t *alt_infs;
  juint8_t alt_inf_cnt;
  juint8_t j;
  pipe_desc_t *pipes;
  juint8_t pipe_cnt;
  juint8_t k;

  if (core->curr_config)
  {
    fd_t *fd;
    for (fd = core->configs[core->curr_config - 1].fd_list; fd; fd = fd->next)
    {
      infs = fd->fd_desc->interfaces;
      inf_cnt = fd->fd_desc->interface_count;
      for(i=0; i < inf_cnt; i++)
      {
        alt_infs = infs[i].alt_ifs;
        alt_inf_cnt = infs[i].alt_if_num;
        for(j=0; j < alt_inf_cnt; j++)
        {
          pipes = alt_infs[j].pipes;
          pipe_cnt = alt_infs[j].pipe_count;
          for(k=0; k < pipe_cnt; k++)
          {
            if(( pipes[k].direction == (unsigned)direction) &&
               ((pipes[k].address & 0x0F) == ep_num))
            {
              return &pipes[k];
            }
          }
        }
      }
    }
  }


  return NULL;
}

/**
* Function name:  core_is_configured
* Description:   
* Parameters:
* Return value:   
* Scope:          Global
**/
boolean core_is_configured(void *core_ctx)
{
  core_t *core = (core_t *)core_ctx;
  boolean ret = FALSE;

  if((core->core_state == CORE_ENABLED) && 
     (core->usb_state == USB_CONFIGURED))
  {
    ret = TRUE;
  }
  
  return ret;
}
#endif /* FEATURE_HS_USB_POLLING */

void core_clear_reconfig_flag(juint8_t core_index)
{
    core_t *core = find_core_by_idx(core_index);

    HSU_ASSERT(core);
    core->reconfig_inprogress = 0;
}

/* HSU Addition */
/**
* Function name:  core_allow_enumeration
* Description:    Allow the core to perform USB 
*                 enumerations
* Parameters:
* Return value:   
* Scope:          Global
**/
void core_allow_enumeration(juint8_t core_number)
{
  core_t *core = find_core_by_number(core_number);

  if (hsu_vregmgr_is_usb_cable_attached() == FALSE)  
  {
    HSU_ULOG (NO_MSG, DATA_LOG, "vbus_not_high");
	return;
  }

#ifdef FEATURE_HS_USB_STANDARD_ECM
  if (hsu_config_selector_is_standard_ecm_supported() == TRUE)
  {
    if (hsu_al_ecm_is_standard_ecm_opened() == FALSE)
    {
      HSU_ULOG (NO_MSG, DATA_LOG, "stdECM_not_opened");
	  return;
    }
  }
#endif

  if (core != NULL)
  {
    if(core->dcd_ops->dcd_allow_enumeration)
    {
      HSU_ULOG_1(NO_MSG, BAM_LOG, "core_number %u", core_number);
      core->dcd_ops->dcd_allow_enumeration(core->dcd_dev);
    }
  }
}


/**
* Function name:  core_disallow_enumeration
* Description:    Prevent the core from performing USB
*                 enumerations until the core_allow_enumeration()
*                 API is called
* Parameters:
* Return value:   
* Scope:          Global
**/
void core_disallow_enumeration(juint8_t core_number)
{
  core_t *core = find_core_by_number(core_number);

  if (core != NULL)
  {
    if(core->dcd_ops->dcd_disallow_enumeration)
    {
      HSU_ULOG_1(NO_MSG, BAM_LOG, "core_number %u", core_number);
      core->dcd_ops->dcd_disallow_enumeration(core->dcd_dev);
    }
  }
}


/**
* Function name: core_dbm_init
**/
void core_dbm_init(void *core_ctx)
{
  core_t *core = (core_t*)core_ctx;

  if (core != NULL)
  {
    HSU_ULOG_1(NO_MSG, BAM_LOG, "core 0x%08X", (uint32)core);
    if (core->dcd_ops && core->dcd_ops->dcd_dbm_init)
    {
      core->dcd_ops->dcd_dbm_init(core->dcd_dev);
    }
  }
}

/**
* Function name: core_dbm_init_qdss
**/
void core_dbm_init_qdss(void *core_ctx, uint32 consumer_pipe_index, uint32 consumer_data_fifo_addr, uint32 consumer_data_fifo_size)
{
  core_t *core = (core_t*)core_ctx;

  if (core != NULL)
  {
    if (core->dcd_ops && core->dcd_ops->dcd_dbm_init_qdss)
    {
      core->dcd_ops->dcd_dbm_init_qdss(core->dcd_dev, consumer_pipe_index, consumer_data_fifo_addr, consumer_data_fifo_size);
    }
  }
}


/**
* Function name: core_dbm_ep_init 
**/
void core_dbm_ep_init(void *core_ctx, uint32 dbm_ep_num)
{
  core_t *core = (core_t*)core_ctx;

  if (core != NULL)
  {
    HSU_ULOG_1(NO_MSG, BAM_LOG, "dbm_ep_num %u", dbm_ep_num);
    if (core->dcd_ops && core->dcd_ops->dcd_dbm_ep_init)
    {
      core->dcd_ops->dcd_dbm_ep_init(core->dcd_dev, dbm_ep_num);
    }
  }
}

/**
* Function name: core_dbm_ep_reset
**/
void core_dbm_ep_reset(void *core_ctx, uint32 dbm_ep_num)
{
  core_t *core = (core_t*)core_ctx;

  if (core != NULL)
  {
    HSU_ULOG_1(NO_MSG, BAM_LOG, "dbm_ep_num %u", dbm_ep_num);
    if (core->dcd_ops && core->dcd_ops->dcd_dbm_ep_reset)
    {
      core->dcd_ops->dcd_dbm_ep_reset(core->dcd_dev, dbm_ep_num);
    }
  }
}

void core_set_bam_pipe_params_for_dbm(void *core_ctx, void *connection, pipe_desc_t *out_pipe, uint32 producer_pipe_index, 
                                      uint32 producer_data_fifo_addr, uint32 producer_data_fifo_size,
                                      uint32 producer_desc_fifo_addr, uint32 producer_desc_fifo_size,
                                      pipe_desc_t *in_pipe, uint32 consumer_pipe_index,
                                      uint32 consumer_data_fifo_addr, uint32 consumer_data_fifo_size,
                                      uint32 consumer_desc_fifo_addr, uint32 consumer_desc_fifo_size)
{
  core_t *core = (core_t*)core_ctx;
  
  if (core != NULL)
  {
    HSU_ULOG_1(NO_MSG, BAM_LOG, "connection 0x%08X", (uint32)connection);
    if (core->dcd_ops && core->dcd_ops->dcd_set_bam_pipe_params)
    {
      core->dcd_ops->dcd_set_bam_pipe_params(core->dcd_dev, connection, out_pipe, producer_pipe_index, 
                                    producer_data_fifo_addr, producer_data_fifo_size,
                                    producer_desc_fifo_addr, producer_desc_fifo_size,
                                    in_pipe, consumer_pipe_index,
                                    consumer_data_fifo_addr, consumer_data_fifo_size,
                                    consumer_desc_fifo_addr, consumer_desc_fifo_size);
    }
  }
}

/**
* Function name: core_is_func_remote_wake_enabled
* Return: TRUE when any of the functions have remote wakeup enabled and requested
**/
boolean core_is_func_remote_wake_enabled(void *core_ctx)
{
  core_t *core = (core_t*)core_ctx;
  uint32 i = 0;

  // REMOTE WAKEUP needs to be triggered if any of the interface has 
  // FUNCTION REMOTE WAKEUP enabled.  This is due to Host Driver only sending 
  // REMOTE WAKEUP enabled flag to first interface only.

  if (core && core->configs)
  {
    for (i = 0; i < core->configs->interface_count; i++)
    {
      if (core->configs->interfaces[i].function_remote_wake_enable)
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}


jresult_t core_resend_interface_pipe(alt_interface_desc_t *ifc, core_t *core)
{  
  uint32 pipe_idx;
  HSU_ASSERT((NULL != ifc) && (NULL != core));

  for (pipe_idx = 0; pipe_idx < ifc->pipe_count; pipe_idx++)
  {
      pipe_desc_t *pipe = &ifc->pipes[pipe_idx];
      request_t *req;
      jresult_t rc;
      jbool_t in_multiple_req = 0;

      if (STAILQ_EMPTY(&pipe->req_queue))
          continue;

      if (core->dcd_ops->dcd_get_ep_cur_req)
      {                
          if (core->dcd_ops->dcd_get_ep_cur_req(pipe))
          {
              // Only print log for IN since OUT EP will be this case always
              if (pipe->direction == DIRECTION_IN)
              {
                  HSU_ULOG_1(NO_MSG, DATA_LOG, "IN, ep_cur_req_exist__skip_send_io_req, pipe 0x%X", (uint32)pipe->address);
              }
              continue;
          }
      }

      req = STAILQ_FIRST(&pipe->req_queue);
      if (req->direction == DIRECTION_IN && !req->single_request)
      {
        in_multiple_req = 1;
      }
      DBG_V(DSLAVE_DCD, ("resume: resuming transfers on pipe "
          "0x%x\n", pipe->address));
      
      HSU_ULOG_2(NO_MSG, BUS_LOG, "not_empty, pipe 0x%X, req 0x%X", (uint32)pipe->address, (uint32)req);

      do
      {
          // == HSU ADDITION ==
          // Let BAM state machine deal with Start/EndXfer on 
          // SPS (aka. DBM) request
          if (req->sps_mode)
          {
              HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "send_to_skipped_dbm_req,"
                " aborting_pipe, req 0x%08X, pipe 0x%08X", 
                (uint32)req, (uint32)pipe);
              hsu_event_log(EVT_CORE_RESUME_SEND_SKIP_DBM_REQ, core->core_index);
              core_abort_pipe(core, pipe);
              break;
          }
          else if (req->direction == DIRECTION_OUT)
          {
              //HSU_ERR_FATAL("out_ep_in_incorrect_state, pipe 0x%X, req 0x%X", pipe->address, req, 0);
              HSU_ULOG_2(NO_MSG, BUS_LOG,"out_ep_in_incorrect_state, pipe 0x%X, req 0x%X", (uint32)pipe->address, (uint32)req);
          }
          else
          {
              rc = core->dcd_ops->dcd_send_io_request(core->dcd_dev,
                  pipe, req);
              if (rc)
              {
                  HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, failed_sending_request, rc %u", rc);
                  STAILQ_REMOVE_HEAD(&pipe->req_queue, next_request);
                  if (req->complete)
                      req->complete(req);
                  return rc;
              }
              
              if (in_multiple_req)
              {
                STAILQ_REMOVE_HEAD(&pipe->req_queue, next_request);
              }
              req = STAILQ_FIRST(&pipe->req_queue);
          }
      }while (in_multiple_req && !STAILQ_EMPTY(&pipe->req_queue));
  }
  return 0;
}

// GSI related APIs

/**
* Function name: core_setup_event_buf_and_gsi_interrupters
* Return: 
**/
void core_setup_event_buf_and_gsi_interrupters(void *core_ctx, uint32 idx, uint32 num_of_trbs)
{
  core_t *core = (core_t*)core_ctx;

  if (core != NULL)
  {
    if (core->dcd_ops && core->dcd_ops->dcd_setup_event_buf_and_gsi_interrupters)
    {
      core->dcd_ops->dcd_setup_event_buf_and_gsi_interrupters(core->dcd_dev, idx, num_of_trbs);
    }
  }
  return;
}

/**
* Function name: core_reset_event_buffer
* Return: 
**/
void core_reset_event_buffer(void *core_ctx, uint32 idx)
{
  core_t *core = (core_t*)core_ctx;

  if (core != NULL)
  {
    if (core->dcd_ops && core->dcd_ops->dcd_reset_event_buffer)
    {
      core->dcd_ops->dcd_reset_event_buffer(core->dcd_dev, idx);
    }
  }
  return;
}


/**
* Function name: core_dcd_usb_gsi_endpoint_config
* Return: 
**/
void core_dcd_usb_gsi_endpoint_config(void *core_ctx, pipe_desc_t *pipe, juint32_t gsi_interrupter_idx)
{
  core_t *core = (core_t*)core_ctx;

  if (core)
  {
    if (core->dcd_ops && core->dcd_ops->dcd_usb_gsi_endpoint_config)
    {
      // This is the customization of the generic
      //   core->dcd_ops->dcd_enable_ep(core->dcd_dev, &if_desc->pipes[i])
      core->dcd_ops->dcd_usb_gsi_endpoint_config(core->dcd_dev, pipe, gsi_interrupter_idx);
    }
  }
  return;
}


/**
* Function name: core_gsi_post_channel_connect_init
* Return: 
**/
void core_gsi_post_channel_connect_init(void *core_ctx, pipe_desc_t *pipe, juint32_t gsi_interrupter_idx, void* gsi_doorbell_reg, void* trb_ring_pa_addr)
{
  core_t *core = (core_t*)core_ctx;

  if (core)
  {
    if (core->dcd_ops && core->dcd_ops->dcd_gsi_post_channel_connect_init)
    {      
      core->dcd_ops->dcd_gsi_post_channel_connect_init(core->dcd_dev, pipe, gsi_interrupter_idx, gsi_doorbell_reg, trb_ring_pa_addr);
    }
  }  
  return;
}

/**
* Function name: core_gsi_general_cfg
* Return: 
**/
void core_gsi_general_cfg(void *core_ctx)
{
  core_t *core = (core_t*)core_ctx;

  if (core)
  {
    if (core->dcd_ops && core->dcd_ops->dcd_gsi_general_cfg)
    {      
      core->dcd_ops->dcd_gsi_general_cfg();
    }
  }
  return;
}


/**
* Function name: core_begin_hw_accel_xfer
* Return: 
**/
void core_begin_hw_accel_xfer(void *core_ctx, pipe_desc_t *pipe, void* trb_ring_pa_addr, juint32_t gsi_interrupter_idx, boolean is_ep_resume)
{
  core_t *core = (core_t*)core_ctx;

  if (core)
  {
    if (core->dcd_ops && core->dcd_ops->dcd_begin_hw_accel_xfer)
    {      
      core->dcd_ops->dcd_begin_hw_accel_xfer(core->dcd_dev, pipe, trb_ring_pa_addr, gsi_interrupter_idx, is_ep_resume);
    }
  }
  return;
}


/**
* Function name: core_init_ep_trbs_hw_accel
* Return: 
**/
void core_init_ep_trbs_hw_accel(void *core_ctx, pipe_desc_t *pipe, void **trb_ring_cookie, void **trb_ring_va_addr, void **trb_ring_pa_addr, juint32_t n)
{
  core_t *core = (core_t*)core_ctx;

  if (core)
  {
    if (core->dcd_ops && core->dcd_ops->dcd_init_ep_trbs_hw_accel)
    {      
      core->dcd_ops->dcd_init_ep_trbs_hw_accel(core->dcd_dev, pipe, trb_ring_cookie, trb_ring_va_addr, trb_ring_pa_addr, n);
    }
  }
  return;
}


/**
* Function name: core_populate_ep_trb_hw_accel
* Return: 
**/
void core_populate_ep_trb_hw_accel(void *core_ctx, pipe_desc_t *pipe, 
  juint32_t n, void **next_free_pa_running_ptr, juint32_t client_buf_size,
  void* trb_ring_va_addr, void* trb_ring_pa_addr, void **link_trb_pa_addr, juint32_t usb_gsi_interrupter_index)
{
  core_t *core = (core_t*)core_ctx;

  if (core)
  {
    if (core->dcd_ops && core->dcd_ops->dcd_populate_ep_trb_hw_accel)
    {
      core->dcd_ops->dcd_populate_ep_trb_hw_accel(core->dcd_dev, pipe, n, next_free_pa_running_ptr, client_buf_size, trb_ring_va_addr, trb_ring_pa_addr, link_trb_pa_addr, usb_gsi_interrupter_index);
    }
  }  
  return;
}


/**
* Function name: core_set_hwio_bit__updatexfer_ul
* Return: 
**/
void core_set_hwio_bit__updatexfer_ul(void *core_ctx, pipe_desc_t *pipe, juint32_t n, void* trb_ring_va_addr)
{
  core_t *core = (core_t*)core_ctx;

  if (core)
  {
    if (core->dcd_ops && core->dcd_ops->dcd_set_hwo_bit__updatexfer_ul)
    {
      core->dcd_ops->dcd_set_hwo_bit__updatexfer_ul(core->dcd_dev, pipe, n, trb_ring_va_addr);
    }
  }  
  return;
}


/**
* Function name: core_get_ep_num
* Return: 
**/
void core_get_ep_num(void *core_ctx, pipe_desc_t *pipe, juint8_t *ep_hw_addr)
{
  core_t *core = (core_t*)core_ctx;

  if (core)
  {
    if (core->dcd_ops && core->dcd_ops->dcd_get_ep_num)
    {
      core->dcd_ops->dcd_get_ep_num(core->dcd_dev, pipe, ep_hw_addr);
    }
  }  
  return;
}
