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
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/core/src/jslave/core/jusb_chp9.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================









************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#include <jusb_chp9_int.h>
#include "jusb_common_int.h"
#include "jusb_core_fd_int.h"
#include "jos_int.h"
#include "jslave_init.h"
#include "jusb_core.h"
#include "hsu_log.h"
#include "hsu_config_selector.h"
#include "hsu_core_config_int.h"
#include "hsu_conf_sel_stack_utils.h"
#include "jusb.h"

/*HSU Addition*/
#include "hsu_vendor_specific.h"
#include "jtypes_int.h"
#include "hsu_phy_utils_int.h"

#if defined (FEATURE_GOBI) && defined (FEATURE_USB_UBIST)
#include "hsu_ubist.h"
#endif /* FEATURE_GOBI && FEATURE_USB_UBIST */

#include "hsu_log.h"
#include "hsu_platform.h"
#include "hsu_lpm_gpio.h"
#include "hsu_common_int.h"

#include "hsu_al_ecm_int.h"
#include "cdc_fd.h"
#include "hsu_al_bam_utils.h"
#include "hsu_hwio_dependency.h"
/*End of HSU addition*/

/* String Descriptor indexes */
#define STRING_SUPPORTED_LANG  0
#define UNICODE_LANG_ID_ENG_US 0x0409

/* MS OS String descriptor */
#define MSOS_DESC_SIZE         18
#define MSOS_STRING_INDEX      0xEE
#define MSOS_VENDOR_CODE       0x01
/* HSU Addition */
/* KW: Add two 0s to the end to make the total size = 14 */
#define MSOS_SIGNATURE         "M\0S\0F\0T\0""1\0""0\0""0\0""0\0""0\0"

/* Device status flags */
#define UDS_SELF_POWERED                0x0001
#define UDS_REMOTE_WAKEUP               0x0002
/* SuperSpeed */
#define UDS_U1_ENABLE                   0x0004
#define UDS_U2_ENABLE                   0x0008
#define UDS_LTM_ENABLE                  0x0010
#define UDS_REMOTE_WAKEUP_CAPABLE       0x0001

#define REALLOC_CORE_REQUEST(core, new_size) \
{ \
    request_t *old_req = core->request; \
    \
    core->request = core_request_alloc(new_size, NULL); \
    if (!core->request) \
    { \
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE, failed_allocate_long_req, %u byte", new_size); \
        core->request = old_req; \
        return JENOMEM; \
    } \
    \
    core->request->caller_private = old_req; \
}

#define CORE_REQUEST_RESTORE(core) \
do { \
    if (core->request->caller_private) \
    { \
        request_t *old_req; \
        old_req = (request_t *)core->request->caller_private; \
        core_request_free(core->request); \
        core->request = old_req; \
        core->request->caller_private = NULL; \
    } \
} while(0)
 
 
static __INLINE__ jint_t interface_number_to_index(core_t *core, 
    juint8_t number);
static jresult_t post_command_reply(core_t *core, juint32_t size, 
    juint32_t max_send_size, juint8_t ep0_req_tag);
#define send_zero_packet(core, ep0_tag) post_command_reply(core, 0, 1, ep0_tag)

static jresult_t get_command_data(core_t *core, juint16_t size, 
    callback_t complete, juint8_t ep0_req_tag);

/* Static USB chap9 descriptors */
static struct usb_device_descriptor
device_desc = {
/*  bLength             */ USB_DT_DEVICE_SIZE,
/*  bDescriptorType     */ USB_DT_DEVICE,
/*  bcdUSB              */ htole16(0x0200),
/*  bDeviceClass        */ 0,
/*  bDeviceSubClass     */ 0,
/*  bDeviceProtocol     */ 0,
/*  bMaxPacketSize0     */ 8,
/*  idVendor            */ 0,
/*  idProduct           */ 0,
/*  bcdDevice           */ 0,
/*  iManufacturer       */ 0,
/*  iProduct            */ 0,
/*  iSerialNumber       */ 0,
/*  bNumConfigurations  */ 0
};

#ifndef CONFIG_FULL_SPEED_ONLY
static struct usb_qualifier_descriptor
dev_qualifier = {
/*  bLength             */ USB_DT_QUALIFIER_SIZE,
/*  bDescriptorType     */ USB_DT_DEVICE_QUALIFIER,
/*  bcdUSB              */ htole16(0x0200),
/*  bDeviceClass        */ 0,
/*  bDeviceSubClass     */ 0,
/*  bDeviceProtocol     */ 0,
/*  bMaxPacketSize0     */ 8,
/*  bNumConfigurations  */ 0,
/*  bRESERVED           */ 0
};
#endif

static struct usb_config_descriptor
config_descriptor = {
/*  bLength             */ USB_DT_CONFIG_SIZE,
/*  bDescriptorType     */ USB_DT_CONFIG,
/*  wTotalLength        */ 0, /* Dynamic */
/*  bNumInterfaces      */ 0, /* Dynamic */
/*  bConfigurationValue */ 0, /* Dynamic */
/*  iConfiguration      */ 0, /* No string descriptors */
/*  bmAttributes        */ USB_CONFIG_ATT_ONE,
/*  bMaxPower           */ 50  
};

static struct usb_interface_descriptor if_descriptor = {
/*  bLength             */ USB_DT_INTERFACE_SIZE,
/*  bDescriptorType     */ USB_DT_INTERFACE,
/*  bInterfaceNumber    */ 0, /* Dynamic */
/*  bAlternateSetting   */ 0, /* Dynamic */
/*  bNumEndpoints       */ 0, /* Dynamic */
/*  bInterfaceClass     */ 0, /* Dynamic */
/*  bInterfaceSubClass  */ 0, /* Dynamic */
/*  bInterfaceProtocol  */ 0, /* Dynamic */
/*  iInterface          */ 0  /* Dynamic */ 
};

static struct usb_endpoint_descriptor ep_descriptor = {
/*  bLength             */ 0, /* Dynamic */
/*  bDescriptorType     */ USB_DT_ENDPOINT,
/*  bEndpointAddress    */ 0, /* Dynamic */
/*  bmAttributes        */ 0, /* Dynamic */
/*  wMaxPacketSize      */ 0, /* Dynamic */
/*  bInterval           */ 0  /* Dynamic */
#ifdef CONFIG_FD_AUDIO
/*  bRefresh            */ ,0 /* Dynamic */
/*  bSynchAddress       */ ,0 /* Dynamic */
#endif
};

#ifdef CONFIG_JOTG
static struct usb_otg_descriptor otg_descriptor = {
/* bLength              */ USB_DT_OTG_SIZE,
/* bDescriptorType      */ USB_DT_OTG,
/* bmAttributes         */ 0
};

#define IS_DEVICE_OTG() \
    (core->dcd_ops->dcd_is_otg && core->dcd_ops->dcd_is_otg(core->dcd_dev))

#endif


/* Suppress Lint warning that some struct members are not referenced, */
/* since they are initialized here */
static struct usb_BOS_descriptor
BOS_descriptor = {
/*  bLength             */ USB_DT_BOS_SIZE,
/*  bDescriptorType     */ USB_DT_BOS,
/*  wTotalLength        */ 0, /* Dynamic */
/*  bNumDeviceCaps      */ 0  /* Dynamic */
};

/* Suppress Lint warning that some struct members are not referenced, */
/* since they are initialized here */
static struct usb_devcap_usb2_ext_descriptor
devcap_usb2_ext_descriptor = {
/*  bLength             */ USB_DT_DEVCAP_USB2_EXT_SIZE,
/*  bDescriptorType     */ USB_DT_DEVICE_CAPABILITY,
/*  bDevCapabilityType  */ USB_DEVICE_CAPABILITY_TYPE_USB2_EXT,
/*  bmAttributes        */ 0  /* Dynamic */
};

/* Suppress Lint warning that some struct members are not referenced, */
/* since they are initialized here */
static struct usb_devcap_superspeed_usb_descriptor
devcap_ss_usb_descriptor = {
/*  bLength                 */ USB_DT_DEVCAP_SUPERSPEED_USB_SIZE,
/*  bDescriptorType         */ USB_DT_DEVICE_CAPABILITY,
/*  bDevCapabilityType      */ USB_DEVICE_CAPABILITY_TYPE_SUPERSPEED_USB,
/*  bmAttributes            */ 0, /* Dynamic */
/*  wSpeedsSupported        */ 0, /* Dynamic */
/*  bFunctionalitySupported */ 0, /* Dynamic */
/*  bU1DevExitLat           */ 0, /* Dynamic */
/*  bU2DevExitLat           */ 0  /* Dynamic */
};

/* Suppress Lint warning that some struct members are not referenced, */
/* since they are initialized here */
static struct usb_superspeed_endpoint_companion_descriptor
ss_ep_companion_descriptor = {
/*  bLength             */ USB_DT_SUPERSPEED_ENDPOINT_COMPANION_SIZE,
/*  bDescriptorType     */ USB_DT_SUPERSPEED_ENDPOINT_COMPANION,
/*  bMaxBurst           */ 0, /* Dynamic */
/*  bmAttributes        */ 0, /* Dynamic */
/*  wBytesPerInterval   */ 0  /* Dynamic */
};

/* HSUSB addition */
#if defined(FEATURE_GOBI)
/* Function pointer */
typedef void (*gobi_void_cb) (void);
static gobi_void_cb gobi_fcc_auto_enable_cb = NULL;
static boolean g_fcc_auth_passed = FALSE;
#endif /* FEATURE_GOBI */
/* End of HSUSB addition */

#if defined(CONFIG_JSLAVE_TEST_MODE)
static void ep0_test_mode_complete_callback(request_t *request)
{
    core_t *core = (core_t *)request->context;

    core->dcd_ops->dcd_set_test_mode(core->dcd_dev, request->test_mode);
}
#endif

static void ep0_vendor_out_data_complete_callback(request_t *request)
{
    core_t *core = (core_t *)request->context;
    juint8_t ep0_req_tag = request->ep0_req_tag;
    jresult_t rc;

    DBG_V(DSLAVE_CORE, ("CH9: Handle completion of vendor specific out "
        "request\n"));

    if (request->status !=  REQUEST_COMPLETED)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE_CH9, failed_to_receive_data_stage_for_ep0");
        goto Error;
    }

    rc = core->device_param.vendor_cmd_out_data_stage_cb(request->buffer.vaddr,
        request->bytes_transferred);
    CORE_REQUEST_RESTORE(core);
    if (rc)
        goto Error;

    core->request->status = REQUEST_READY;
    rc = send_zero_packet(core, ep0_req_tag);
    if (rc)
        goto Error;

    return;

Error:
    core_stall_pipe(core, NULL, 1);
}

/*HSU addition*/
static void ep0_reserved_out_data_complete_callback(request_t *request)
{
    core_t *core = (core_t *)request->context;
    juint8_t ep0_req_tag = request->ep0_req_tag;
    jresult_t rc;

    DBG_V(DSLAVE_CORE, ("CH9: Handle completion of reserved specific out "
        "request\n"));

    if (request->status !=  REQUEST_COMPLETED)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE_CH9, failed_to_receive_data_stage_for_ep0");
        goto Error;
    }

    rc = core->device_param.reserved_cmd_out_data_stage_cb(request->buffer.vaddr,
        request->bytes_transferred);
    CORE_REQUEST_RESTORE(core);
    if (rc)
        goto Error;

    core->request->status = REQUEST_READY;
    rc = send_zero_packet(core, ep0_req_tag);
    if (rc)
        goto Error;

    return;

Error:
    core_stall_pipe(core, NULL, 1);
}
/*End of HSU addition*/
/**
 * Function name:  ep0_reply_complete_callback
 * Description: completion callback when response to endpoint 0 command is
 *              completed
 * Parameters: 
 *     @request: (IN) CORE request
 *
 * Return value: None
 * Scope: local
 **/
static void ep0_reply_complete_callback(request_t *request)
{
    core_t *core = (core_t *)request->context;

    if (core->request->transfer_size != core->request->bytes_transferred)
    {
        HSU_ULOG_3(ERROR_MSG, DBG_E_LOG, "CORE, cmd_reply_incorrect %d [%d:%d]", 
            request->status, request->bytes_transferred,
            request->transfer_size);        
    }

    DBG_V(DSLAVE_CORE, ("Callback: EP0 Reply finished. %d of %d sent\n", 
        core->request->bytes_transferred, core->request->transfer_size));

    if (core->request->caller_private)
    {
        DBG_I(DSLAVE_CORE, ("ep0_reply_complete_callback: Restoring old "
            "request\n"));
        CORE_REQUEST_RESTORE(core);
    }

    core->request->status = REQUEST_READY;
}

/**
 * Function name:  core_add_msos_string_descriptor
 * Description:    Add a string descriptor to be included in the response to a 
 *                 GET_MS_DESCRIPTOR request
 * Parameters:
 *     @ctx:         (IN) CORE context
 *     @vendor_code: (IN) Vendor code to send in the response
 *
 * Return value:   None
 * Scope:          Global
 **/
void core_add_msos_string_descriptor(void *ctx, juint8_t vendor_code)
{
    core_t *core = (core_t *)ctx;

    core->msos_vendor_code = vendor_code;
    core->msos_vendor_code_set = 1;
}

/* HSU addition */
void core_release_msos_string_descriptor(void *ctx)
{
  core_t *core = (core_t *)ctx;
  
  core->msos_vendor_code = 0;
  core->msos_vendor_code_set = 0;
}
/* End of HSU addition */

/**
 * Function name:  core_add_string_descriptor
 * Description:    Add a string descriptor to be included in the response to a 
 *                 GET_DESCRIPTOR request
 * Parameters: 
 *     @ctx:    (IN) CORE context
 *     @string: (IN) The string to add
 *     @str_desc: (IN) The string description
 *
 * Return value:   The string index
 * Scope:          Global
 **/
juint8_t core_add_string_descriptor(void *ctx, char *string, hsu_str_idx_desc_type str_desc)
{
    core_t *core = (core_t *)ctx;
    juint8_t i;
    juint8_t  idx = 0;
    
    if (!string)
        return 0; /* This is a valid value specifying "no descriptor" */ 
    
    /*HSU Addition */
    /*Check if this string has a pre-defined index for it*/
    idx = hsu_vendor_specific_get_str_idx(str_desc);
    if (idx < MAX_STRING_DESCRIPTORS && idx != 0)
    {
      /* verify that the index is free*/
      if (core->string_descriptors[idx-1])
      {
        HSU_MSG_ERROR_1("core_add_string_descriptor: the given index is already occupied!"
                "(idx = %d)", idx);
      }
      else
      {
        core->string_descriptors[idx-1] = string;
        return idx;
      }
    }
    
    for (i = 0; i < MAX_STRING_DESCRIPTORS ; i++)
    {
      /* If the index is free and not saved for another string */
      if ((!core->string_descriptors[i]) && (!hsu_vendor_specific_is_idx_occupied(i+1)))
      {
        break;
      }
    }
        
    if (i == MAX_STRING_DESCRIPTORS)
    {
      HSU_MSG_HIGH("core_add_string_descriptor: descriptors array is full");
        return 0; /* descriptors array is full */
    }
    /*End of HSU addition*/
    core->string_descriptors[i] = string;
    return i+1;
}

/**
 * Function name:  core_release_string_descriptor 
 * Description:    Remove a string descriptor
 * Parameters: 
 *     @ctx:    (IN) CORE context
 *     @index_h (IN) Index handle
 * Return value:   None 
 * Scope:          Global
 **/
void core_release_string_descriptor(void *ctx, juint8_t index)
{
    core_t *core = (core_t *)ctx;
    
    if (!index || index > MAX_STRING_DESCRIPTORS)
        return;
    core->string_descriptors[index - 1] = NULL;
}

/**
 * Function name:  core_change_string_descriptor
 * Description:    Change a string descriptor
 * Parameters: 
 *     @ctx:    (IN) CORE context
 *     @id:     (IN) Index of the string to update
 *     @string: (IN) The string to update
 *
 * Return value: None
 * Scope:        Global
 **/
void core_change_string_descriptor(void *ctx, juint8_t id, char *string)
{
    core_t *core = (core_t *)ctx;
    core->string_descriptors[id - 1] = string;
}

/**
 * Function name:  get_string_descriptor
 * Description:    Return the string at index <id>
 * Parameters: 
 *     @core: (IN) CORE context
 *     @id:   (IN) index of the requested string
 *
 * Return value:   String descriptor
 * Scope:          Local
 **/
static char *get_string_descriptor(core_t *core, juint8_t id)
{
    if (!id  || id > MAX_STRING_DESCRIPTORS)   
        return NULL;

    return core->string_descriptors[id - 1];
}

/**
 * Function name:  post_command_reply
 * Description: Send a reply to an endpoint 0 command
 * Parameters: 
 *     @core: (IN) CORE context
 *     @size: (IN) reply size (data is already ready in core->request->buffer)
 *     @max_send_size: (IN) the wLength parameter in ctrl_request
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control 
 *                        request
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t post_command_reply(core_t *core, juint32_t size, 
    juint32_t max_send_size, juint8_t ep0_req_tag)
{
    if (!core->request)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, core_req_struct_not_ready");        
        return JEINVAL;
    }

    if (core->request->status != REQUEST_READY)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, attempt_use_busy_req");             
        return JEAGAIN;
    }

    if (core->ep0_req_tag != ep0_req_tag)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, attempt_answer_expired_ep0_req"); 
        return 0;
    }

    DBG_V(DSLAVE_CORE, ("CH9: EP0 post reply size %d (max %d)\n",
        size, max_send_size));

    /* If the host is expecting more data then we plan to send
     * then we should send a ZLP to tell him that we have sent
     * all we meant to */
    if (max_send_size > size)
        core->request->zero = 1;
    else
        core->request->zero = 0;

    core->request->context      = core;
    core->request->complete     = ep0_reply_complete_callback;
    core->request->transfer_size = MIN(size, max_send_size);

    /*  When the request is from Device to Host but with no data stage 
        (wLength = 0) we need to send the status stage in OUT direction */
    if (max_send_size) 
    {
#ifdef FEATURE_HS_USB_GOLD_TREE_TEST_WORKAROUND
        /* HSU Fix - Workaround the Gold Tree test. 
        ** Each IN transaction over the control endpoint is limited by a timer.
        ** When the timer expires, CDR auto reset is disabled.
        ** CRMDB CR: #174688 - 'USB Device Not Recognized' on Gold Tree HS5'
        */
        static int ep0_timeout = 50;
        core->request->timeout = ep0_timeout;
        /* HSU Fix end. */
#endif  /* FEATURE_HS_USB_GOLD_TREE_TEST_WORKAROUND */

        return core_send_data(core, NULL, core->request);
    }
    else
        return core_read_data(core, NULL, core->request);
}

/**
 * Function name:  do_iface_ep_disable
 * Description: Disable all endpoint in a specific interface
 * Parameters: 
 *     @core: (IN) CORE context
 *     @if_desc: (IN) Interface context
 *
 * Return value: None
 * Scope: local
 **/
static void do_iface_ep_disable(core_t *core, alt_interface_desc_t *if_desc, void *cdc_ctx_ptr)
{
    jint_t i;
    for (i = 0; i < if_desc->pipe_count; i++)
    {
        DBG_V(DSLAVE_CORE, ("CORE: Disable pipe %x status %x\n",
            if_desc->pipes[i].address, if_desc->pipes[i].status));
        
        if (if_desc->pipes[i].status == PIPE_ENABLED ||
            if_desc->pipes[i].status == PIPE_STALLLED)
        {
            core->dcd_ops->dcd_disable_ep(core->dcd_dev, &if_desc->pipes[i]);
        }
    }

#ifdef FEATURE_HS_USB_GOLD_TREE_TEST_WORKAROUND
    /* Release EP 0 IN timeout task. */
    core->dcd_ops->dcd_disable_ep(core->dcd_dev, NULL);
#endif  /* FEATURE_HS_USB_GOLD_TREE_TEST_WORKAROUND */
}

/**
 * Function name:  do_iface_ep_enable
 * Description: Enable all endpoints in a specific interface
 * Parameters: 
 *     @core: (IN) CORE context
 *     @if_desc: (IN) Interface context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t do_iface_ep_enable(core_t *core, alt_interface_desc_t *if_desc, void *cdc_ctx_ptr)
{
    jint_t i;
    jresult_t rc;
    
    DBG_V(DSLAVE_CORE, ("CH9: Enabling iface %p\n", if_desc));

    for (i = 0; i < if_desc->pipe_count; i++)
    {
        DBG_X(DSLAVE_CORE, ("CH9: Enabling pipe 0x%x\n", 
            if_desc->pipes[i].address));

        /* Set the max packet size based on speed */
        if(core->curr_speed == SPEED_SUPER)
        {
            if_desc->pipes[i].max_pkt_size = if_desc->pipes[i].max_pkt_size_super;
        }
        else if(core->curr_speed == SPEED_HIGH)
        {
              if_desc->pipes[i].max_pkt_size = if_desc->pipes[i].max_pkt_size_high;
        }
        else
        {
            if_desc->pipes[i].max_pkt_size = if_desc->pipes[i].max_pkt_size_full;
        }
            
        /* Updating descriptors */ 
        core_update_descriptors(core, &if_desc->pipes[i]);

          /* Ask DCD to start things up */
          rc = core->dcd_ops->dcd_enable_ep(core->dcd_dev, &if_desc->pipes[i]);
          if (rc)
            return rc;
    }

    DBG_V(DSLAVE_CORE, ("CH9: Enable ep done\n"));
    return 0;
}

/**
 * Function name:  do_ep_disable
 * Description: Disable all device endpoints
 * Parameters: 
 *     @core: (IN) CORE context
 *     @idx:  (IN) configuration value
 *
 * Return value: None
 * Scope: local
 **/
static void do_ep_disable(core_t *core, juint8_t idx)
{
    jint_t i;
    config_t *config = &core->configs[idx - 1];

    /* Disable endpoints of each interface */
    for (i = 0; i < config->interface_count; i++)
        do_iface_ep_disable(core, config->interfaces[i].if_curr, config->interfaces[i].fd->context);
}

/**
 * Function name:  do_ep_enable
 * Description: Enable all device endpoints
 * Parameters: 
 *     @core: (IN) CORE context
 *     @idx:  (IN) configuration value
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t do_ep_enable(core_t *core, juint8_t idx)
{
    jint_t i;
    jresult_t rc;
    config_t *config = &core->configs[idx - 1];

    DBG_V(DSLAVE_CORE, ("CH9: Starting ep enable\n"));

    /* Attempt to enable endpoints of each interface */
    for (i = 0; i < config->interface_count; i++)
    {
        DBG_X(DSLAVE_CORE, ("CH9: Enabling iface %d [%p]\n",
            i, config->interfaces[i].if_curr));
            
        /* For each interface enable all endpoints */
        rc = do_iface_ep_enable(core, config->interfaces[i].if_curr, config->interfaces[i].fd->context);
        if (rc)
            return rc;
    }
    return 0;
}

/**
 * Function name:  handle_set_interface
 * Description: Handle set interface request. Disable the interface including
 *              notifying the function driver, then enable with the new 
 *              alternate setting and notify the function driver
 * Parameters: 
 *     @core: (IN) CORE context
 *     @iface: (IN) interface number
 *     @altif: (IN) alternate setting number
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t handle_set_interface(core_t *core, juint8_t iface, juint8_t altif)
{
    fd_t *fd;
    jresult_t rc = 0;
    jint_t interface_index;
    config_t *config;

    DBG_V(DSLAVE_CORE, ("SET INTERFACE %d ALT %d\n", iface, altif));

    /* Check that core is configured */
    if (!core->curr_config)
    {
        DBG_W(DSLAVE_CORE, ("Attempt to set an iface when core "
            "not configured !\n"));
        return JEINVAL;
    }

    interface_index = interface_number_to_index(core, iface);
    config = &core->configs[core->curr_config - 1];

    /* Check if we support the interface */
    if (interface_index >= config->interface_count)
    {
        DBG_W(DSLAVE_CORE, ("Attempt to set an invalid interface %d\n",
            iface));
        return JEINVAL;
    }

    /* Check if we support the alt-interface */
    if (altif >= config->interfaces[interface_index].if_arr->alt_if_num)
    {
        DBG_W(DSLAVE_CORE, ("Attempt to set an invalid alt_iface %d:%d\n",
            iface, altif));
        return JEINVAL;
    }

    DBG_V(DSLAVE_CORE, ("Setting interface %d altestting %d\n",
        iface, altif));

    config->interfaces[interface_index].function_remote_wake_tnotifification_pending = FALSE;
    fd = config->interfaces[interface_index].fd;

    /* Disable FD */
    if (fd->state == FD_ENABLED)
        fd->fd_ops->disable(fd->context);
    fd->state = FD_DISABLED; 

    /* Disable EPs */
    do_iface_ep_disable(core, config->interfaces[interface_index].if_curr, fd->context);
    
    /* Change core pointers */
    config->interfaces[interface_index].if_curr = 
        &config->interfaces[interface_index].
        if_arr->alt_ifs[altif];

    /* Set number of select alt iface */
    config->interfaces[interface_index].if_arr->alt_if_curr = altif;

    /* Enable EPs */
    rc = do_iface_ep_enable(core, config->interfaces[interface_index].if_curr, fd->context);
    if (rc)
        goto error;

    /* Update current speed in FD's desc */
    fd->fd_desc->current_speed = core->curr_speed;
        
    /* Enable FD */
    rc = fd->fd_ops->enable(fd->context);
    if (rc)
        goto error;

    fd->state = FD_ENABLED;

#ifndef FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM
    // Due to the delayed SET_INTERFACE, when the STATE MACHINE initializes the DBM and BAM pipes, 
    //   interface is not yet ENABLED.  Thus, add an extra call into hsu_al_ecm_enable_flow_with_bam( ) to ensure
    //   EPs are programmed into infinite mode.
    if ((fd->context)
        && (hsu_get_num_of_net_instances() > 0) /* not IPC router */
        && (config->interfaces[interface_index].if_curr->pipe_count > 0)
        && (TRUE == hsu_al_ecm_is_appctx_ecm(((cdc_ctx_t*)(fd->context))->app_ctx)))
    {
      hsu_al_ecm_enable_flow_with_bam(((cdc_ctx_t*)(fd->context))->app_ctx, HSU_AL_BAM_UL, HSU_AL_BAM_DL);
    }
#endif // FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM

    DBG_V(DSLAVE_CORE, ("Set interface %d altsetting %d complete (%d)\n",
        iface, altif, rc));

    return 0;

error:
    do_iface_ep_disable(core, config->interfaces[interface_index].if_curr, fd->context);

    DBG_W(DSLAVE_CORE, ("Set interface %d alt %d encountered an error (%d)\n",
        iface, altif, rc));
    return rc;
}

/**
 * Function name:  chp9_handle_set_configuration
 * Description: Handle set configuration request. First un-configure the device
 *              (in case it is already configured) by disable all endpoints and
 *              function drivers. Then enable all the endpoints and function 
 *              drivers
 * Parameters: 
 *     @core: (IN) CORE context
 *     @config_idx: (IN) configuration value
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope: global
 **/
jresult_t chp9_handle_set_configuration(core_t *core, juint8_t config_idx)
{
    fd_t *fd;
    jint_t i;
    jresult_t rc = 0;
    config_t *config;

    DBG_V(DSLAVE_CORE, ("SET CONFIG %d (%d)\n", config_idx, core->curr_config));

    // HSU ADDITION +++
    // It is observed that some host sends SET_CONFIG with the same CONFIG NUMBER twice.
    // In such cases we should discard in order to maintain integrity of the BAM state machine
    if (core->curr_config == config_idx)
    {
      HSU_ULOG_1(NO_MSG, BUS_LOG, "req_filtered_redundant_set_configuration, config_idx %u", config_idx);
      return HSU_JSUCCESS;
    }
    // HSU ADDITION ---

    rc = core->dcd_ops->dcd_set_config_state(core->dcd_dev, config_idx);
    if (rc)
        return rc;

    /* We can assume that FD is valid, else the command would not have
     * been processed in the first place */

    /* Check if we support the configuration */
    if (config_idx > core->config_count)
    {
        DBG_W(DSLAVE_CORE, ("Attempt to set an invalid configuration\n"));
        return JEINVAL;
    }

    if (core->curr_config)
    {
        /* change usb_state to address */
        core->usb_state = USB_ADDRESS;

        core->U1_enabled = FALSE;
        core->U2_enabled = FALSE;
        core->LTM_enabled = FALSE;

        if (core->configs) /* HSU addition: fix crash when jstart_stack fails */
        {
          /* Disable all FDs */
          for (fd = core->configs[core->curr_config - 1].fd_list; fd;
              fd = fd->next)
          {
              if (fd->state == FD_ENABLED)
                  fd->fd_ops->disable(fd->context);
              fd->state = FD_DISABLED;
          }

          /* Disable all EPs */
          do_ep_disable(core, core->curr_config);
        }
    }


    /* if config_idx = 0, no configuration is set.
     * The USB devices should return to "Address" state */
    if (!config_idx)
    {
        /* Notify only in case the device was already configured */
        if (core->curr_config)
        {
            core->curr_config = 0;

            core->U1_enabled = FALSE;
            core->U2_enabled = FALSE;
            core->LTM_enabled = FALSE;

            jnotify_os(NOTIFY_DEVICE_UNCONFIGURED, 
                j_device_get_bus(core->dcd_dev));
            
            /* Temp fix for B47218 */
            
            core->app_cbs.notify_device_configured(core->app_ctx, FALSE);
            
            /* Temp fix END */
        }
        return 0;
    }


    DBG_V(DSLAVE_CORE, ("Setting configuration %d speed %d\n",
        config_idx, core->curr_speed));

    HSU_ASSERT(core->configs);
    config = &core->configs[config_idx - 1];

    /* Reset all alt_ifaces */
    for (i = 0; i < config->interface_count; i++)
    {
        /* Get pointer of active alt_iface and put it somewhere handly */
        config->interfaces[i].if_curr = &(config->interfaces[i].
            if_arr->alt_ifs[0]);

        DBG_V(DSLAVE_CORE, ("CH9: Setting iface %d [core %d] curr to %p\n",
            i, config->interfaces[i].fd_if_num, config->interfaces[i].if_curr));

        /* Set first interface to be alt-set 0 */
        config->interfaces[i].if_arr->alt_if_curr = 0;
    }
    
    // Note fd->context contains cdc_ctx_t*
    rc = do_ep_enable(core, config_idx);
    if (rc)
        goto error;
    
    core->curr_config   = config_idx;

    /* Attempt to enable function drivers */
    for (fd = config->fd_list; fd; fd = fd->next)
    {
        /* Update current speed in FD's desc */
        fd->fd_desc->current_speed = core->curr_speed;
        
        rc = fd->fd_ops->enable(fd->context);
        if (rc)
            goto error;

        fd->state = FD_ENABLED;
    }

    core->usb_state     = USB_CONFIGURED;

    jnotify_os(NOTIFY_DEVICE_CONFIGURED, j_device_get_bus(core->dcd_dev));
/* Temp fix for B47218 */

    core->app_cbs.notify_device_configured(core->app_ctx, TRUE);

/* Temp fix END */

    DBG_V(DSLAVE_CORE, ("Set configuration complete (%d)\n", rc));
    return 0;

error:
    do_ep_disable(core, config_idx);

    for (fd = config->fd_list; fd; fd = fd->next)
    {
        if (fd->state == FD_ENABLED)
            fd->fd_ops->disable(fd->context);
        fd->state = FD_DISABLED;
    }

    core->curr_config = 0;

    DBG_W(DSLAVE_CORE, ("Set configuration encountered an error (%d)\n", rc));
    return rc;
}

/**
 * Function name:  send_descriptor_device
 * Description: Send device descriptor
 * Parameters: 
 *     @core: (IN) CORE context
 *     @wLength: (IN) maximum length of the descriptor
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control 
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope: local
 **/
static jresult_t send_descriptor_device(core_t *core, juint16_t wLength, 
    juint8_t ep0_req_tag)
{
    dev_info_t *dev_info = core->device_param.dev_info; 

    DBG_V(DSLAVE_CORE, ("CH9: Send DEVICE DESCRIPTOR:\n"));

    device_desc.iProduct = core->i_product;
    device_desc.iManufacturer = core->i_manufacturer;
    device_desc.iSerialNumber = core->i_serial_number;
    device_desc.idVendor        = htole16(dev_info->vendor);
    device_desc.idProduct       = htole16(dev_info->product);
    device_desc.bcdDevice       = htole16(dev_info->release);
    device_desc.bDeviceClass    = dev_info->dev_class;
    device_desc.bDeviceSubClass = dev_info->dev_sub_class;
    device_desc.bDeviceProtocol = dev_info->dev_protocol;
    device_desc.bMaxPacketSize0 = 
        core->dcd_ops->dcd_get_max_packet0(core->dcd_dev);
    device_desc.bNumConfigurations = core->config_count;
    if (hsu_clkmgr_is_hs_only_mode())
    {
      device_desc.bcdUSB          = htole16(dev_info->bcdUSB);
    }
    else
    {
      device_desc.bcdUSB          = htole16(core->curr_speed == SPEED_SUPER ? 0x300 : dev_info->bcdUSB);
    }

    j_memcpy(core->request->buffer.vaddr, &device_desc, USB_DT_DEVICE_SIZE);
    return post_command_reply(core, USB_DT_DEVICE_SIZE, wLength, ep0_req_tag);
}

static juint16_t calc_config_desc_size(core_t *core, juint8_t config_index)
{
    interface_desc_t    *if_desc;
    alt_interface_desc_t *alt_desc;
    pipe_desc_t         *ep_desc;
    juint8_t curr_if = 0;
    juint8_t curr_alt = 0;
    jint_t curr_ep = 0;
    jint_t curr_fd_if = 0;
    fd_t *curr_fd = NULL;
    juint16_t size;
    config_t *config = &core->configs[config_index];

    size = USB_DT_CONFIG_SIZE;

#ifdef CONFIG_JOTG
    if (IS_DEVICE_OTG())
        size += USB_DT_OTG_SIZE;
#endif
    for (curr_if = 0, curr_fd_if = 0, curr_fd = config->fd_list;
        curr_if < config->interface_count; curr_if++, curr_fd_if++)
    {
        if (curr_fd_if > curr_fd->fd_desc->interface_count - 1)
        {
            curr_fd = curr_fd->next;
            curr_fd_if = 0;
        }
        if_desc = config->interfaces[curr_if].if_arr;
        
        if (!curr_fd_if && curr_fd->fd_desc->iad)
            size += USB_DT_INTERFACE_ASSOC_SIZE;

        for (curr_alt = 0; curr_alt < if_desc->alt_if_num; curr_alt++)
        {
            alt_desc = &if_desc->alt_ifs[curr_alt];

            size += USB_DT_INTERFACE_SIZE;
            
            /* Check if there is vendor/class specific descriptor data to add */
            if (!curr_alt && if_desc->extra_descriptor_size > 0)
                size += if_desc->extra_descriptor_size;

            /* Check if there is vendor/class specific descriptor data to add */
            if (alt_desc->extra_descriptor_size > 0)
                size += alt_desc->extra_descriptor_size;

            /* Add all EPs that belong to this interface */
            for (curr_ep = 0; curr_ep < alt_desc->pipe_count; curr_ep++)
            {
                ep_desc = &alt_desc->pipes[curr_ep];

#ifdef CONFIG_FD_AUDIO
                size += (if_desc->if_class == USB_CLASS_AUDIO) ?  
                    USB_DT_AUDIO_ENDPOINT_SIZE : USB_DT_ENDPOINT_SIZE;
#else
                size += USB_DT_ENDPOINT_SIZE;
#endif
                if (ep_desc->extra_descriptor_size)
                    size += ep_desc->extra_descriptor_size;
                
                if(core->curr_speed == SPEED_SUPER)
                {
                    size += USB_DT_SUPERSPEED_ENDPOINT_COMPANION_SIZE;
                }    
            }
        }
    }
/*HSU Addition*/
#if defined (FEATURE_GOBI) && defined (FEATURE_USB_UBIST)
    size += hsu_ubist_conf_descriptor(NULL, NULL);
#endif /* FEATURE_GOBI && FEATURE_USB_UBIST */                 
/*End of HSU Addition*/
    return size;
}

#define ADD_TO_BUF(from, size) \
    do { \
        if (curr_pos + size > desc_size) \
            return JE2BIG; \
        j_memcpy(buffer + curr_pos, from, size); \
        curr_pos += size; \
    } while (0)

/**
 * Function name:  send_descriptor_config
 * Description: Send configuration descriptor, including all extra descriptors
 * Parameters: 
 *     @core: (IN) CORE context
 *     @index: (IN) configuration value
 *     @speed: (IN) device speed
 *     @wLength: (IN) maximum length of the descriptor
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code  
 * Scope: local
 **/
static jresult_t send_descriptor_config(core_t *core, juint8_t index, 
    juint8_t speed, juint16_t wLength, juint8_t ep0_req_tag)
{
    jint_t curr_pos = 0;
    juint8_t curr_if = 0;
    juint8_t curr_alt = 0;
    jint_t curr_ep = 0;
    jint_t curr_fd_if = 0;
    fd_t *curr_fd = NULL;

    /* We rely on descriptor information passed from FD */
    interface_desc_t    *if_desc;
    alt_interface_desc_t *alt_desc;
    pipe_desc_t         *ep_desc;
    juint8_t *buffer;
    /* HSU temp fix - remove #ifdef CONFIG_JOTG, since dev_info is used in nandprg as well */
/*#ifdef CONFIG_JOTG*/
    dev_info_t *dev_info = core->device_param.dev_info; 
/*#endif*/
    /* end of HSU temp fix */
    juint16_t desc_size;
    config_param_t *config_param;

    DBG_V(DSLAVE_CORE, ("CH9: Send CONFIG DESCRIPTOR: %d\n", index));

    /* Verify that the requested configuration is valid */
    if (index >= core->config_count)
    {
        HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "CORE, attempt_get_inv_config, ind %u, config_cnt %u",
          index, core->config_count);        
        return JEINVAL;
    }

    desc_size = calc_config_desc_size(core, index);
    config_param = &core->device_param.config_param[index];

    if (desc_size > MAX_DESCRIPTOR_SIZE)
    {
        DBG_I(DSLAVE_CORE, ("send_descriptor_config: Config descriptor too "
            "long - allocating new request\n"));

        REALLOC_CORE_REQUEST(core, desc_size);
    }

    buffer = (juint8_t *)core->request->buffer.vaddr;

    /* Set the config descriptor*/
    config_descriptor.bNumInterfaces = core->configs[index].interface_count;
    config_descriptor.bConfigurationValue = index + 1;

    /* iConfiguration */
    config_descriptor.iConfiguration = core->configs[index].i_config;

    /* HSU Addition*/
    /* Set the configuration descriptor */
    if (config_descriptor.iConfiguration)
    {
        core_change_string_descriptor(core, config_descriptor.iConfiguration, 
         dev_info->configuration);
    }
    else
    {
        config_descriptor.iConfiguration = core_add_string_descriptor(core,
         dev_info->configuration, HSU_STR_IDX_CONFIGURATION);
    }
    /* End of HSU addition*/


    /* Self Powered ? */
    if (config_param->self_powered)
        config_descriptor.bmAttributes |= USB_CONFIG_ATT_SELFPOWER;

    /* Supports remote wake-up ? */
    if (config_param->remote_wakeup)
        config_descriptor.bmAttributes |= USB_CONFIG_ATT_WAKEUP;
    
    /* Power consumption */
    if(speed == SPEED_SUPER)
    {
        config_descriptor.bMaxPower = HSU_SS_DEVICE_MAX_POWER;
    }
    else
    {
        config_descriptor.bMaxPower = config_param->max_power;
    }

    curr_pos += USB_DT_CONFIG_SIZE;

#ifdef CONFIG_JOTG
    if (IS_DEVICE_OTG() && (dev_info->srp_support || dev_info->hnp_support))
    {
        if (dev_info->srp_support)
            otg_descriptor.bmAttributes |= USB_OTG_SRP;
        if (dev_info->hnp_support)
            otg_descriptor.bmAttributes |= USB_OTG_HNP;
        ADD_TO_BUF(&otg_descriptor, USB_DT_OTG_SIZE);
    }
#endif

    /* Add all interfaces */
    for (curr_if = 0, curr_fd_if = 0, curr_fd = core->configs[index].fd_list;
        curr_if < core->configs[index].interface_count;
        curr_if++, curr_fd_if++)
    {
        if (curr_fd_if > curr_fd->fd_desc->interface_count - 1)
        {
            curr_fd = curr_fd->next;
            curr_fd_if = 0;
        }
        if_desc = core->configs[index].interfaces[curr_if].if_arr;
        
        if (!curr_fd_if && curr_fd->fd_desc->iad)
        {
            DBG_X(DSLAVE_CORE, ("Adding IAD for if %d\n", curr_if));
            ADD_TO_BUF(curr_fd->fd_desc->iad, USB_DT_INTERFACE_ASSOC_SIZE);
        }

        for (curr_alt = 0; curr_alt < if_desc->alt_if_num; curr_alt++)
        {
            alt_desc = &if_desc->alt_ifs[curr_alt];

            /* USB interface numbers will correspond with Core's
             * internal interfaces[] */
            if_descriptor.bInterfaceNumber      = 
                curr_fd->fd_desc->interfaces[curr_fd_if].number;
            if_descriptor.bAlternateSetting     = curr_alt;
            if_descriptor.bNumEndpoints         = alt_desc->pipe_count;
            if_descriptor.bInterfaceClass       = if_desc->if_class;
            if_descriptor.bInterfaceSubClass    = if_desc->if_subclass;
            if_descriptor.bInterfaceProtocol    = if_desc->if_protocol;
            if_descriptor.iInterface            = if_desc->if_string;

            ADD_TO_BUF(&if_descriptor, USB_DT_INTERFACE_SIZE);
            
            /* Check if there is vendor/class specific descriptor data to add */
            if (!curr_alt && if_desc->extra_descriptor_size > 0)
            {
                ADD_TO_BUF(if_desc->extra_descriptor,
                    if_desc->extra_descriptor_size);
            }

            /* Check if there is vendor/class specific descriptor data to add */
            if (alt_desc->extra_descriptor_size > 0)
            {
                ADD_TO_BUF(alt_desc->extra_descriptor,
                    alt_desc->extra_descriptor_size);
            }

            /* Add all EPs that belong to this interface */
            for (curr_ep = 0; curr_ep < alt_desc->pipe_count; curr_ep++)
            {
                ep_desc = &alt_desc->pipes[curr_ep];

                if (ep_desc->direction == DIRECTION_IN)
                    ep_descriptor.bEndpointAddress = USB_DIR_IN;
                else
                    ep_descriptor.bEndpointAddress = USB_DIR_OUT;

                /* EP address is known only to core */
                ep_descriptor.bEndpointAddress |= ep_desc->address;

                ep_descriptor.bmAttributes = (juint8_t)ep_desc->type;
                ep_descriptor.bInterval = ep_desc->rate_u.nak_rate;

                if (ep_desc->type == PIPE_ISOC)
                {
                    ep_descriptor.bmAttributes |= ep_desc->sync_type  << 2;
                    ep_descriptor.bmAttributes |= ep_desc->usage_type << 4;
                }

                
                /* Additional configuration for SuperSpeed interrupt endpoints */
                if( (speed == SPEED_SUPER) && (ep_desc->type == PIPE_INTR) )
                {
                    ep_descriptor.bmAttributes |= ep_desc->usage_type << USB_BM_ATTRIBUTES_USAGE_TYPE_OFFSET;
                }

                /* Set the max packet size based on speed (don't htole16 yet) */
                if (speed == SPEED_SUPER)
                {
                    ep_descriptor.wMaxPacketSize = ep_desc->max_pkt_size_super;
                }
                else if (speed == SPEED_HIGH)
                {
                    ep_descriptor.wMaxPacketSize = ep_desc->max_pkt_size_high;    
                }
                else
                {
                    ep_descriptor.wMaxPacketSize = ep_desc->max_pkt_size_full;
                }
                
                ep_descriptor.wMaxPacketSize = htole16(
                  MIN(core_get_max_packet_size(ep_desc->type, (device_speed_t) speed),
                  ep_descriptor.wMaxPacketSize));

                /* HSU Addition */
                /* Special provision for USBCV interface descriptor test */
                /* A High speed Interrupt endpoint present in alternate interface 0x00 must have a MaxPacketSize <= 0x40 */
                if ((ep_desc->type == PIPE_INTR) && (curr_alt == 0))
                {
                    ep_descriptor.wMaxPacketSize = htole16(ep_desc->max_pkt_size_full);
                }
                /* End of HSU addition */

                if (ep_desc->type == PIPE_ISOC || ep_desc->type == PIPE_INTR)
                {
                    if ((speed == SPEED_HIGH || speed == SPEED_SUPER) && ep_desc->poll_interval_high)
                    {
                        ep_descriptor.bInterval = ep_desc->poll_interval_high;
                    }
                    else
                    {
                        if (ep_desc->type == PIPE_INTR && ep_desc->rate_u.poll_interval)
                        {
                            ep_descriptor.bInterval = ep_desc->rate_u.poll_interval;
                        }
                        else
                        {
                            ep_descriptor.bInterval = 1;
                        }
                    }
                }
                else
                {
                    ep_descriptor.bInterval = 0;
                }


#ifdef CONFIG_FD_AUDIO
                ep_descriptor.bLength = (if_desc->if_class == USB_CLASS_AUDIO) ?
                    USB_DT_AUDIO_ENDPOINT_SIZE : USB_DT_ENDPOINT_SIZE;
#else
                ep_descriptor.bLength = USB_DT_ENDPOINT_SIZE;
#endif
                ADD_TO_BUF(&ep_descriptor, ep_descriptor.bLength);

                                
                /* Add SuperSpeed endpoint companion descriptor */
                if(speed == SPEED_SUPER)
                {
                    ss_ep_companion_descriptor.bMaxBurst = ep_desc->ss_attributes.max_burst;
                    switch (ep_desc->type)
                    {
                        case PIPE_BULK:
                            ss_ep_companion_descriptor.bmAttributes = ep_desc->ss_attributes.max_streams;
                            break;
                        case PIPE_ISOC:
                            ss_ep_companion_descriptor.bmAttributes = ep_desc->transaction_per_microframe;
                            break;
                        default:
                            ss_ep_companion_descriptor.bmAttributes = 0;
                            break;
                    }
                    ss_ep_companion_descriptor.wBytesPerInterval = htole16(ep_desc->ss_attributes.bytes_per_interval);
                    ADD_TO_BUF(&ss_ep_companion_descriptor, USB_DT_SUPERSPEED_ENDPOINT_COMPANION_SIZE);
                }    

                if (ep_desc->extra_descriptor_size)
                {
                    DBG_X(DSLAVE_CORE, ("CORE: Going to add EP extra "
                        "descriptor. Size = %d Addr = 0x%p\n", 
                        ep_desc->extra_descriptor_size, 
                        ep_desc->extra_descriptor));
                        ADD_TO_BUF(ep_desc->extra_descriptor, ep_desc->extra_descriptor_size);
                }
            }
        }
    }
/*HSU Addition*/
#if defined (FEATURE_GOBI) && defined (FEATURE_USB_UBIST)
    {
       juint8_t  ubist_length;
       juint8_t *p_conf_ubist;
       ubist_length = hsu_ubist_conf_descriptor(core, &p_conf_ubist);
       ADD_TO_BUF(p_conf_ubist, ubist_length);
    }
#endif /* FEATURE_GOBI && FEATURE_USB_UBIST */ 
/*End of HSU Addition*/    
    /* Now copy the config descriptor */
    config_descriptor.wTotalLength = htole16(curr_pos);
    j_memcpy(buffer, &config_descriptor, USB_DT_CONFIG_SIZE);

    return post_command_reply(core, curr_pos, wLength, ep0_req_tag);
}

/**
 * Function name:  send_string
 * Description: Send string to the host
 * Parameters: 
 *     @core: (IN) CORE context
 *     @str: (IN) the string to send
 *     @wLength: (IN) maximum length of the string
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope: local
 **/
#define MAX_STRING_LEN 126
static jresult_t send_string(core_t *core, char *str, juint16_t wLength, 
    juint8_t ep0_req_tag)
{
    juint8_t *buffer;
    juint8_t str_len, act_len = 0;

    if (!str)
        return JEINVAL;

    buffer = (juint8_t *)core->request->buffer.vaddr;

    str_len     = MIN(j_strlen(str), MAX_STRING_LEN);

    /* HSU Addition: Check for buffer overflow */
    if (core->request->buffer.buffer_size < (2 + (str_len * 2)))
    {
      HSU_ERR_FATAL("send_string buffer overflow (buf size %u < required %u)",
        core->request->buffer.buffer_size, (2 + (str_len * 2)), 0);
    }

    buffer[0]   = (str_len + 1) * 2;
    buffer[1]   = USB_DT_STRING;
    j_memset(buffer + 2, 0, 2 * str_len);        
    
    while (act_len < str_len) 
    {
        buffer[2 + act_len * 2] = str[act_len];
        act_len++;
    }

    return post_command_reply(core, buffer[0], wLength, ep0_req_tag);
}

/**
 * Function name:  send_descriptor_string
 * Description: Send string descriptor
 * Parameters: 
 *     @core: (IN) CORE context
 *     @index: (IN) string index
 *     @lang_index: (IN) language id
 *     @wLength: (IN) maximum length of the descriptor
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope: local
 **/
static jresult_t send_descriptor_string(core_t *core, juint8_t index, 
    juint16_t lang_index, juint16_t wLength, juint8_t ep0_req_tag)
{
    juint8_t     *buffer;
    char        *string;

    DBG_V(DSLAVE_CORE, ("CH9: Send STRING DESCRIPTOR: %d\n", index));
    
    buffer = (juint8_t *)core->request->buffer.vaddr;

    /* Send the supportted language id */
    if (index == STRING_SUPPORTED_LANG)
    {
        buffer[0] = 4;
        buffer[1] = USB_DT_STRING;
        buffer[2] = UNICODE_LANG_ID_ENG_US & 0x00ff;
        buffer[3] = UNICODE_LANG_ID_ENG_US >> 8;
        return post_command_reply(core, 4, wLength, ep0_req_tag);
    }

    /* handling Microsoft OS String Descriptor */
    if (index == MSOS_STRING_INDEX && core->msos_vendor_code_set) 
    {
        DBG_I(DSLAVE_CORE, ("CH9: Send MS OS DESCRIPTOR: %d \n", index));

        buffer[0] = MSOS_DESC_SIZE;
        buffer[1] = USB_DT_STRING;
        j_memcpy(&buffer[2], MSOS_SIGNATURE, 14);
        buffer[16] = core->msos_vendor_code;
        buffer[17] = 0x00;  /* Padding */
        return post_command_reply(core, MSOS_DESC_SIZE, wLength, ep0_req_tag);
    }

    string = get_string_descriptor(core, index);
    if (!string && core->device_param.vendor_get_string_cb)
        string = core->device_param.vendor_get_string_cb(index);

    if (string)
        return send_string(core, string, wLength, ep0_req_tag);
    
    DBG_W(DSLAVE_CORE, ("Core: Uknown string descriptor %d\n", index));
    return JEINVAL;
}

#ifndef CONFIG_FULL_SPEED_ONLY
/**
 * Function name:  send_descriptor_dev_qual
 * Description: Send device qualifier descriptor
 * Parameters: 
 *     @core: (IN) CORE context
 *     @wLength: (IN) maximum length of the descriptor
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope: local
 **/
static jresult_t send_descriptor_dev_qual(core_t *core, juint16_t wLength,
    juint8_t ep0_req_tag)
{
    dev_info_t *dev_info = core->device_param.dev_info;

    DBG_V(DSLAVE_CORE, ("CH9: Send DEV QUALIFIER DESCRIPTOR:\n"));

    dev_qualifier.bDeviceClass    = dev_info->dev_class;
    dev_qualifier.bDeviceSubClass = dev_info->dev_sub_class;
    dev_qualifier.bDeviceProtocol = dev_info->dev_protocol;
    dev_qualifier.bMaxPacketSize0 = 
        core->dcd_ops->dcd_get_max_packet0(core->dcd_dev);
    dev_qualifier.bNumConfigurations = core->config_count;
    /* HSU temp fix */
    dev_qualifier.bcdUSB          = htole16(dev_info->bcdUSB);
    /* HSU temp fix */

    j_memcpy(core->request->buffer.vaddr, &dev_qualifier, 
        USB_DT_QUALIFIER_SIZE);

    return post_command_reply(core, USB_DT_QUALIFIER_SIZE, wLength, 
        ep0_req_tag);
}

/**
 * Function name:  send_descriptor_other_speed
 * Description: Send other speed descriptor
 * Parameters: 
 *     @core: (IN) CORE context
 *     @wLength: (IN) maximum length of the descriptor
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code  
 * Scope: local
 **/
static jresult_t send_descriptor_other_speed(core_t *core, juint16_t wLength, 
    juint8_t ep0_req_tag)
{
    jresult_t rc;

    /* We support only two configs now, Full speed and High speed */
    DBG_V(DSLAVE_CORE, ("CH9: Send OTHER SPEED CONFIG DESCRIPTOR:\n"));

    /* Check if we support other speeds */
    if (core->max_speed == SPEED_FULL)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, no_other_speed_config, max_spd FULL_SPEED");           
        return JEINVAL;
    }

    /* The "other speed" configuration descriptor looks almost like a
     * regular config descriptor. */
    config_descriptor.bDescriptorType = USB_DT_OTHER_SPEED_CONFIG;

    rc = send_descriptor_config(core, 0, (core->curr_speed == SPEED_HIGH ? 
        SPEED_FULL : SPEED_HIGH), wLength, ep0_req_tag);
    
    config_descriptor.bDescriptorType = USB_DT_CONFIG;
    return rc;
}
#endif

#ifdef CONFIG_JOTG
/**
 * Function name:  send_descriptor_otg
 * Description: Send OTG descriptor
 * Parameters: 
 *     @core: (IN) CORE context
 *     @wLength: (IN) maximum length of the descriptor
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code  
 * Scope: local
 **/
static jresult_t send_descriptor_otg(core_t *core, juint16_t wLength, 
    juint8_t ep0_req_tag)
{
    DBG_V(DSLAVE_CORE, ("CH9: Send OTG DESCRIPTOR:\n"));

    j_memcpy(core->request->buffer.vaddr, &otg_descriptor, USB_DT_OTG_SIZE);
    return post_command_reply(core, USB_DT_OTG_SIZE, wLength, ep0_req_tag);
}
#endif

static jresult_t send_descriptor_bos(core_t *core, juint16_t wLength, juint8_t ep0_req_tag)
{
  uint16 curr_pos = 0;
  uint8 *buffer = (uint8 *)core->request->buffer.vaddr;
  uint16 desc_size;

  DBG_V(DSLAVE_CORE, ("CH9: Send BOS DESCRIPTOR\n"));

  desc_size = USB_DT_BOS_SIZE +
              USB_DT_DEVCAP_USB2_EXT_SIZE;

  /* Set the BOS descriptor*/
  BOS_descriptor.bNumDeviceCaps = USB_BOS_DEVICE_CAPABILITY_NUM;
  curr_pos += USB_DT_BOS_SIZE;

  /* Add Device capabilities USB2_EXT descriptor */
  ua_htole32(&devcap_usb2_ext_descriptor.bmAttributes,
     ua_le32toh(&devcap_usb2_ext_descriptor.bmAttributes)
     | USB_DEVCAP_USB2_EXIT_ATT_LPM
     | USB_DEVCAP_USB2_EXIT_ATT_BESL_AND_ALT_HIRD);
  ADD_TO_BUF(&devcap_usb2_ext_descriptor, USB_DT_DEVCAP_USB2_EXT_SIZE);

  if (FALSE == hsu_clkmgr_is_hs_only_mode())
  {
    desc_size += USB_DT_DEVCAP_SUPERSPEED_USB_SIZE;

    /* Add Device capabilities SUPERSPEED_USB descriptor */
    devcap_ss_usb_descriptor.bmAttributes = 0;
    devcap_ss_usb_descriptor.bFunctionalitySupported = 0x1; // Full speed is lowest supported speed.
    devcap_ss_usb_descriptor.wSpeedsSupported = htole16(USB_DEVCAP_SPEEDS_SUPPORTED);
    /* Set U1/U2 exit latencies */
    devcap_ss_usb_descriptor.bU1DevExitLat = 0;
    devcap_ss_usb_descriptor.bU2DevExitLat = 0;
    ADD_TO_BUF(&devcap_ss_usb_descriptor, USB_DT_DEVCAP_SUPERSPEED_USB_SIZE);
  }
  /* Now copy the BOS descriptor */
  BOS_descriptor.wTotalLength = htole16(curr_pos);
  jos_memcpy(buffer, &BOS_descriptor, USB_DT_BOS_SIZE);

  DBG_V(DSLAVE_CORE, ("CH9: Send BOS DESCRIPTOR of size BOS_descriptor.wTotalLength %d:\n", BOS_descriptor.wTotalLength));
  DBG_V(DSLAVE_CORE, ("CH9: Send BOS DESCRIPTOR of size wLength %d:\n", wLength));


  return post_command_reply(core, BOS_descriptor.wTotalLength, wLength, ep0_req_tag);
 }


/* Device status flags */
#define UDS_SELF_POWERED                0x0001
#define UDS_REMOTE_WAKEUP               0x0002

#ifdef CONFIG_REMOTE_WAKEUP_TEST
static void test_remote_wakeup(void *ctx)
{
    core_t *core = (core_t *)ctx;
    HSU_ULOG(HIGH_MSG, BUS_LOG, "CORE, wake_the_host"); 

    jslave_wakeup_host(core->core_number);
}
#endif

/**
 * Function name:  handle_get_status
 * Description: Handle get status request
 * Parameters: 
 *     @core: (IN) CORE context
 *     @bmRequestType: (IN) request type
 *     @wIndex: (IN) request index (depends on bmRequestType)
 *     @wLength: (IN) maximum length of the reply
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code  
 * Scope: local
 **/
#define FUNCTION_REMOTE_WAKE_CAPABLE_MASK (0x1)
#define FUNCTION_REMOTE_WAKE_CAPABLE_SHFT (0x0)
#define FUNCTION_REMOTE_WAKEUP_MASK       (0x2)
#define FUNCTION_REMOTE_WAKEUP_SHFT       (0x1)

static jresult_t handle_get_status(core_t *core, juint8_t bmRequestType,
    juint16_t wIndex, juint16_t wLength, juint8_t ep0_req_tag)
{
    juint8_t buffer[USB_GET_STATUS_SIZE] = {0};
    dev_info_t *dev_info = core->device_param.dev_info; 
    jresult_t rc = 0;
    
    DBG_V(DSLAVE_CORE, ("CH9: Send GET STATUS\n"));

    switch (bmRequestType & USB_RECIP_MASK)
    {
    case USB_RECIP_DEVICE:

        /* Self Powered ? */
        if (dev_info->self_powered_func)
            dev_info->self_powered = dev_info->self_powered_func() ? 1 : 0;

        if (dev_info->self_powered)
            buffer[0] |= UDS_SELF_POWERED;

        /* Is remote wakeup currently enabled ? */
        if (core->remote_wakeup_enabled)        
            buffer[0] |= UDS_REMOTE_WAKEUP;

        if (core->U1_enabled)
            buffer[0] |= UDS_U1_ENABLE;

        if (core->U2_enabled)
            buffer[0] |= UDS_U2_ENABLE;

        if (core->LTM_enabled)
            buffer[0] |= UDS_LTM_ENABLE;

        break;  
        
    case USB_RECIP_INTERFACE:
        /* We must be configured */
        if (core->usb_state != USB_CONFIGURED)
            return JEINVAL;

        if (SPEED_SUPER == core->curr_speed)
        {
          if ((core->curr_config != 0) && (wIndex < core->configs[core->curr_config - 1].interface_count))
          {
            // Device reports its function remote wake capability
            buffer[0] = FUNCTION_REMOTE_WAKE_CAPABLE_MASK
              | ((core->configs[core->curr_config - 1].interfaces[wIndex].function_remote_wake_enable) ? FUNCTION_REMOTE_WAKEUP_MASK : 0);
            core->configs[core->curr_config - 1].interfaces[wIndex].function_remote_wake_tnotifification_pending = FALSE;
          }
          else
          {
            HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, invalid_iface %u", wIndex);
            return JEINVAL;
          }
        }
        // else send two bytes of 0's
        break;
        
    case USB_RECIP_ENDPOINT:
        /* We must be configured if it is not EP0 */
        if (core->usb_state != USB_CONFIGURED && (0 != wIndex))
        {
            return JEINVAL;
        }
        rc = core->dcd_ops->dcd_get_ep_status(core->dcd_dev, buffer, 
            (juint8_t)wIndex);
        
        if (rc)
            return rc;

        break;

    default:
        return JEINVAL;  
    }

    j_memcpy(core->request->buffer.vaddr, buffer, USB_GET_STATUS_SIZE);

    return post_command_reply(core, USB_DT_DEVICE_SIZE, wLength, ep0_req_tag);
}

static jresult_t handle_feature(core_t *core, juint8_t bmRequestType,
    juint16_t feature, juint16_t index, juint8_t set, juint8_t ep0_req_tag)
{
    jresult_t rc = 0;
    juint8_t recipient = bmRequestType & USB_RECIP_MASK;
    uint32 explicit_remote_wakeup_delay = hsu_conf_sel_get_rm_wakeup_delay();
    hsu_lpm_info_type* const core_lpm_info =hsu_os_abstract_bus_get_lpm_info(core->core_index);

#ifdef FEATURE_HS_USB_F3
    hsu_platform_info_type const *platform_info;
#endif /* FEATURE_HS_USB_F3 */
    // Additional of features directed to interface
    switch (recipient)
    {
      case USB_RECIP_INTERFACE:
      {
        juint8_t iface = index & (HSU_FUNCTION_SUSPEND__IFACE_MASK >> HSU_FUNCTION_SUSPEND__IFACE_SHFT);
        if (0 == core->curr_config)
        {
          rc = JEINVAL;
          goto Exit;
        }
        if(iface < ((core->configs[core->curr_config - 1]).interface_count))
        {
          core->configs[core->curr_config - 1].interfaces[iface].function_remote_wake_tnotifification_pending = FALSE;
        }
        
        switch (feature)
        {
          case USB_FUNCTION_SUSPEND:
          {
#if 1   // Use Legacy Function Suspend
            juint8_t iface = index & (HSU_FUNCTION_SUSPEND__IFACE_MASK >> HSU_FUNCTION_SUSPEND__IFACE_SHFT);

            if (0 == core->curr_config)
            {
              rc = JEINVAL;
              goto Exit;
            }
        
            HSU_ULOG_2(NO_MSG, BUS_LOG, "hsu_iface_function_suspend, index %u, iface %u", (uint32)(index), iface);
            if ((SPEED_SUPER == core->curr_speed) && (iface < ((core->configs[core->curr_config - 1]).interface_count)))
            {
              core->configs[core->curr_config - 1].interfaces[iface].function_remote_wake_enable =
                (index & USB_FUNCTION_SUSPEND_OPTIONS_FUNC_WKP_EN) ? 1 : 0;

              if (index & USB_FUNCTION_SUSPEND_OPTIONS_SUSP_STATE)
              {
                // Should be 1, but current properietary Host driver drives Function SUSPEND like real SUSPEND
                core->configs[core->curr_config - 1].interfaces[iface].is_function_suspend = 0; 
                HSU_ULOG_1(NO_MSG, BAM_LOG, "set_feature_func_susp_state_1, is_host_enable_func_remote_wakeup %u", (uint32) core->configs[core->curr_config - 1].interfaces[iface].function_remote_wake_enable);                
              }
              else
              {
                core->configs[core->curr_config - 1].interfaces[iface].is_function_suspend = 0;
                core->configs[core->curr_config - 1].interfaces[iface].function_remote_wake_requested = 0;
                HSU_ULOG_1(NO_MSG, BAM_LOG, "set_feature_func_susp_state_0, is_host_enable_func_remote_wake %u", (uint32) core->configs[core->curr_config - 1].interfaces[iface].function_remote_wake_enable);

                // Can stop reminding the host of subsequent function wakes if all interfaces are out of function suspend
                jtask_stop(core->func_wake_tnotif_task);
                core->func_wake_tnotif_scheduled = FALSE;
              }
            }
#else // !Use Legacy Function Suspend
            fd_t *fd;
            uint32 iface_idx = 0;
            interface_t temp_iface_object;            
            boolean all_function_wake = FALSE;
            //boolean all_function_suspend = FALSE;

            //HSU_ULOG_2(NO_MSG, BUS_LOG, "USB_FUNCTION_SUSPEND, index 0x%08X, iface %u", (uint32)(index), iface);
            if ((SPEED_SUPER == core->curr_speed) && (iface < ((core->configs[core->curr_config - 1]).interface_count)))
            {
              core->configs[core->curr_config - 1].interfaces[iface].function_remote_wake_enable =
                (index & USB_FUNCTION_SUSPEND_OPTIONS_FUNC_WKP_EN) ? 1 : 0;
              fd = core->configs[core->curr_config -1].interfaces[iface].fd;

              /**
              FUNCTION_SUSPEND: SET_FEATURE with FUNCTION_SUSPEND == 1, function enter to low power SUSPEND STATE
              **/
              if (index & USB_FUNCTION_SUSPEND_OPTIONS_SUSP_STATE)
              {
                core->configs[core->curr_config - 1].interfaces[iface].is_function_suspend = TRUE;
                core->configs[core->curr_config - 1].interfaces[iface].function_remote_wake_requested = FALSE;
                
                HSU_ULOG_3(NO_MSG, BUS_LOG, "USB_FUNCTION_SUSPEND, FUNCTION_SUSPEND, iface %u, fd_ctx 0x%X, is_host_enable_func_remote_wakeup %u",
                  iface, (uint32) fd->context, (uint32) core->configs[core->curr_config - 1].interfaces[iface].function_remote_wake_enable);        //change back to BAM log        

                fd->fd_ops->suspend(fd->context);

                /* If compliance test remote wakeup delay efs item is set, arm the remote wakeup timer */
                if (explicit_remote_wakeup_delay > 0)
                { 
                  if( core_lpm_info->is_remote_wakeup_pending == FALSE)
                  {
                    HSU_ULOG_1(HIGH_MSG, BAM_LOG, "USB_FUNCTION_SUSPEND set, explicit_remote_wakeup_delay %ums", explicit_remote_wakeup_delay);
                
                    core_lpm_info->is_remote_wakeup_pending = TRUE;
                    hsu_timer_set(
                      &(core_lpm_info->remote_wakeup_timer),
                      explicit_remote_wakeup_delay,
                      T_MSEC);
                  }
                }

                //When all function within a device are in function suspend 
                //and the PORT_U2_TIMEOUT filed is programmed to 0xFF,
                //the device shall initiate U2 after 10ms of link inactive. USB30 9.2.5.4
                /*
                if((HWIO_USB30_PORTSC_REGS_p_PORTPMSC_INMI(0, HWIO_USB30_PORTSC_REGS_p_PORTPMSC_U2_TIMEOUT_BMSK)>>HWIO_USB30_PORTSC_REGS_p_PORTPMSC_U2_TIMEOUT_SHFT)
                  ==0xFF)
                {
                  all_function_suspend = TRUE;
                  for (iface_idx=0; iface_idx < ((core->configs[core->curr_config - 1]).interface_count); iface_idx++)
                  {
                    temp_iface_object = core->configs[core->curr_config -1].interfaces[iface_idx];
                    if (FALSE == temp_iface_object.is_function_suspend)
                    {
                      all_function_suspend =FALSE;
                      break;
                    }
                  }
                  if (all_function_suspend)
                  {
                    //HSU_ULOG(NO_MSG, BAM_LOG, " USB_FUNCTION_SUSPEND, FUNCTION_SUSPEND, all_function_suspend_init_U2_suspend_timer");
                    //TODO: create a timer thread that initiate U2 after 10ms of link inactive  
                    //  core_lpm_info->link_inactive_suspend = TRUE;
                    //hsu_timer_set(
                    //  &(core_lpm_info->remote_wakeup_timer),
                    //  explicit_remote_wakeup_delay,
                    //  T_MSEC);
                  }
                }
                */
                
              }
              else
              /**  
              FUNCTION_RESUME: SET_FEATURE with FUNCTION_SUSPEND == 0, resume to Normal operation state
              **/
              {
                if (FALSE == core->configs[core->curr_config - 1].interfaces[iface].is_function_suspend)
                {
                  HSU_ULOG_1(NO_MSG, BAM_LOG, "handle_feature, USB_FUNCTION_SUSPEND clear, iface %u, already_resume", iface);
                }
                else
                {
                  alt_interface_desc_t *ifc = core->configs[core->curr_config - 1].interfaces[iface].if_curr;
                  core->configs[core->curr_config - 1].interfaces[iface].is_function_suspend = FALSE;
                  core->configs[core->curr_config - 1].interfaces[iface].function_remote_wake_requested = FALSE;

                  HSU_ULOG_3(NO_MSG, BAM_LOG, "USB_FUNCTION_SUSPEND, FUNCTION_RESUME, iface %u, fd_ctx 0x%0X, is_host_enable_func_remote_wakeup %u",
                    iface, (uint32) fd->context, (uint32) core->configs[core->curr_config - 1].interfaces[iface].function_remote_wake_enable);      

                  //HSU_ULOG_2(NO_MSG, BUS_LOG, "resend_io_req, iface_idx %u, ifc 0x%X", iface, (uint32) ifc);
                  rc = core_resend_interface_pipe(ifc, core);
                  if (rc)
                  {
                    return rc;
                  }

                  fd->fd_ops->resume(fd->context);
                  
                  // check if all functions function_remote_wake_tnotifification_pending, if yes remote the pending tnofic_task
                  all_function_wake = TRUE;
                  for (iface_idx=0; iface_idx < core->configs[core->curr_config - 1].interface_count; iface_idx++)
                  {
                    temp_iface_object = core->configs[core->curr_config -1].interfaces[iface_idx];
                    if (TRUE == temp_iface_object.function_remote_wake_tnotifification_pending) 
                    {
                      all_function_wake = FALSE;
                      break;
                    }
                  }
                  if (all_function_wake == TRUE)
                  {
                    if (jtask_pending(core->func_wake_tnotif_task))
                    {
                      jtask_stop(core->func_wake_tnotif_task);
                      core->func_wake_tnotif_scheduled = FALSE;
                      HSU_ULOG(NO_MSG, BAM_LOG, "USB_FUNCTION_SUSPEND, FUNCTION_RESUME,all_function_resume, cancel_tnotif_scheduled");                
                    }
                  }
                }
              }
            }
#endif
          }
          break;

          default:
            rc = core->dcd_ops->dcd_handle_feature(core->dcd_dev, feature, index, set);
            break;
        }
      }
      break;

      case USB_RECIP_DEVICE:
      {
        switch (feature)
        {
          case USB_DEVICE_REMOTE_WAKEUP:
          {
            if (SPEED_SUPER == core->curr_speed) 
            {
              HSU_ULOG(NO_MSG, DBG_E_LOG, "DEVICE, USB_DEVICE_REMOTE_WAKEUP, SS_NOT_ALLOW_DEV_RW");
            }
            if (recipient != USB_RECIP_DEVICE || !core->curr_config ||
                !core->device_param.config_param[core->curr_config - 1].
                remote_wakeup)
            {
              rc = JEINVAL;
              goto Exit;
            }

            HSU_ULOG_1(NO_MSG, BUS_LOG, "hsu_device_remote_wakeup, set %u", (uint32)(set));

            if (set && (SPEED_SUPER != core->curr_speed))
            {
              core->remote_wakeup_enabled = 1;
            }
            else
            {
              core->remote_wakeup_enabled = 0;
#ifdef FEATURE_HS_USB_F3
              /* HSU Addition: Cancel CLEAR_FEATURE(REMOTE_WAKEUP) timeout timer */
              if(core->core_number == HSU_CORE_IDX_PRIMARY)
              {
                platform_info = hsu_platform_get_info();

                if(platform_info->gpio_remote_wakeup)
                {
                  hsu_lpm_gpio_log_clear_feature_for_remote_wakeup();
                }
              }
#endif /* FEATURE_HS_USB_F3 */
            }

#ifdef CONFIG_REMOTE_WAKEUP_TEST
            /* Schedule a remote wakeup request for testing 
             * We schedule the task even for clear feature, to verify that the
             * core doesn't allow to remote wakeup when the host disable it */
            jtask_schedule(core->remote_wakeup_test_task, 10000, 
                test_remote_wakeup, core);
#endif
          }
          break;
#if defined(CONFIG_JSLAVE_TEST_MODE)
          case USB_DEVICE_TEST_MODE:
          {
            if (recipient != USB_RECIP_DEVICE)
            {
              rc = JEINVAL;
              goto Exit;
            }

            if (!set || !core->dcd_ops->dcd_set_test_mode)
            {
              rc = JEINVAL;
              goto Exit;
            }
            /* We need to change to test mode only after we send a status packet
             * to the host */
            core->request->zero = 0;
            core->request->context      = core;
            core->request->complete     = ep0_test_mode_complete_callback;
            core->request->transfer_size = 0;
            /* Save Test Selector to be used in the completion of the status
             * packet */
            core->request->test_mode = ((index & 0x0f00) >> 8);
            core->request->status = REQUEST_READY;

#ifdef FEATURE_HS_USB_GOLD_TREE_TEST_WORKAROUND
            /* HSU FIX - Workaround for the EL_18 test. 
             ** When USB SE0_NAK test mode is requested by the host,
             ** disable CDR auto reset.
             */
            if (core->request->test_mode == 0x3)
            {

              /* HSU Addition - CDR auto reset */
#ifdef FEATURE_HS_USB_INTEGRATED_PHY
              /* Disable CDR auto reset for SE0_NAK test mode. */
              (void)hsu_phy_utils_config_phy_cdr_auto_reset(FALSE);
#endif /* FEATURE_HS_USB_INTEGRATED_PHY */
              /* HSU Additions - end */
            }
            /* HSU FIX - End. */
#endif  /* FEATURE_HS_USB_GOLD_TREE_TEST_WORKAROUND */

            rc = core_send_data(core, NULL, core->request);

            goto Exit;
            // no break needed here as core_send_data is sent
          }
#endif
#ifdef CONFIG_JOTG
          case USB_DEVICE_B_HNP_ENABLE:
          {
            if (set && recipient != USB_RECIP_DEVICE)
            {
              rc = JEINVAL;
              goto Exit;
            }

            if (IS_DEVICE_OTG())
            {
              rc = core->dcd_ops->dcd_invoke_otg_ioctl(core->dcd_dev, 
                  DRV_IOCTL_DEV_HNP_ENBL, NULL);
            }
            else
              rc = JENOTSUP;
          }
          break;
          case USB_DEVICE_A_HNP_SUPPORT:
          {
            if (set && recipient != USB_RECIP_DEVICE)
            {
              rc = JEINVAL;
              goto Exit;
            }

            if (!IS_DEVICE_OTG())
              rc = JENOTSUP;
          }
          break;
          case USB_DEVICE_A_ALT_HNP_SUPPORT:
          {
            if (set && recipient != USB_RECIP_DEVICE)
            {
              rc = JEINVAL;
              goto Exit;
            }

            if (IS_DEVICE_OTG())
            {
              rc = core->dcd_ops->dcd_invoke_otg_ioctl(core->dcd_dev, 
                  DRV_IOCTL_DEV_HNP_ALT_SUPPORTED, NULL);
            }
            else
              rc = JENOTSUP;
          }
          break;
#endif
          case USB_FEATURE_SELECTOR_U1_ENABLE:
          {
            if (USB_CONFIGURED != core->usb_state)
            {
              rc = JEINVAL;
            }
            else
            {
              core->U1_enabled = (jbool_t)set;
            }
          }
          break;

          case USB_FEATURE_SELECTOR_U2_ENABLE:
          {
            if (USB_CONFIGURED != core->usb_state)
            {
              rc = JEINVAL;
            }
            else
            {
              core->U2_enabled = (jbool_t)set;
            }
          }
          break;

          case USB_FEATURE_SELECTOR_LTM_ENABLE:
          {
            if (USB_CONFIGURED != core->usb_state)
            {
              rc = JEINVAL;
            }
            else
            {
              core->LTM_enabled = (jbool_t)set;
            }
          }
          break;

          default:
            rc = core->dcd_ops->dcd_handle_feature(core->dcd_dev, feature, index, set);
            break;
        }
      }
      break;

      case USB_RECIP_ENDPOINT:
      {
        switch (feature)
        {
          case USB_ENDPOINT_HALT:
          {
            if (recipient != USB_RECIP_ENDPOINT || (core->usb_state != USB_CONFIGURED && index))
            {
              rc = JEINVAL;
              goto Exit;
            }

            /* HSUSB addition */
#if defined(FEATURE_GOBI)
            /* Perform Gobi FCC authentication feature */ 
            if (gobi_fcc_auto_enable_cb != NULL)
            {
              gobi_fcc_auto_enable_cb();
            }
  
            /* Save FCC auth info to cover the case that endpoint_halt  
            ** data is received before FCC auth callback is initialized
            */
            g_fcc_auth_passed = TRUE; 
  
#endif /* FEATURE_GOBI */
            /* End of HSUSB addition */
            rc = core->dcd_ops->dcd_handle_feature(core->dcd_dev, feature, index, set);
          }
          break;

        default:
          rc = core->dcd_ops->dcd_handle_feature(core->dcd_dev, feature, index, set);
          break;
        }
      }
      break;

      default:
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, unrecognized_feature, recipient %u", recipient);
        break;
    }
    if (!rc)
    {
        rc = send_zero_packet(core, ep0_req_tag);
    }
    else
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, ret_err, ZLP_not_sent");
    }

Exit:
    return rc;
}
/**
 * Function name:  handle_get_descriptor
 * Description: Handle get descriptor request
 * Parameters: 
 *     @core: (IN) CORE context
 *     @wValue: (IN) descriptor type (higher byte) and index (lower byte)
 *     @wIndex: (IN) Descriptor index
 *     @wLength: (IN) maximum length of the reply
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code  
 * Scope: local
 **/
static jresult_t handle_get_descriptor(core_t *core, juint16_t wValue, 
    juint16_t wIndex, juint16_t wLength, juint8_t ep0_req_tag)
{
    jresult_t    rc = 0;
    juint8_t     descriptor_type  = (juint8_t)(wValue >> 8);
    juint8_t     descriptor_index = (juint8_t)(wValue & 0x00ff);

    DBG_V(DSLAVE_CORE, ("CH9: Get descriptor %d\n", descriptor_type));

    if (descriptor_type < HSU_CHP9_GET_DESCRIPTOR_LAST2)
    {
      HSU_ULOG_ENUM_2(NO_MSG, BUS_LOG, "wValue %u, wLength %u",
        descriptor_type, HSU_CHP9_GET_DESCRIPTOR_OFFSET2, wValue, wLength);
    }
    else
    {    
      HSU_ULOG_1(NO_MSG, BUS_LOG, "get_device_descriptor_out_range %u", descriptor_type);
    }
    switch (descriptor_type)
    {
    case USB_DT_DEVICE:
        rc = send_descriptor_device(core, wLength, ep0_req_tag);
        break;
    case USB_DT_CONFIG:
        rc = send_descriptor_config(core, descriptor_index, core->curr_speed, 
            wLength, ep0_req_tag);
        break;
    case USB_DT_STRING:
        rc = send_descriptor_string(core, descriptor_index, wIndex, wLength, 
            ep0_req_tag);
        break;
    case USB_DT_DEVICE_QUALIFIER:
#ifdef CONFIG_FULL_SPEED_ONLY
            rc = JEINVAL;
#else
            // Full speed only device responds with STALL handshake to device qualifier request.
            // SS device in SS mode should not support device qualifier descriptor.
            if (core->device_param.full_speed_only || core->curr_speed == SPEED_SUPER)
                rc = JEINVAL;
            else
                rc = send_descriptor_dev_qual(core, wLength, ep0_req_tag);
#endif
        break;
    case USB_DT_OTHER_SPEED_CONFIG:
#ifdef CONFIG_FULL_SPEED_ONLY
            rc = JEINVAL;
#else
            if (core->device_param.full_speed_only)
                rc = JEINVAL;
            else
                rc = send_descriptor_other_speed(core, wLength, ep0_req_tag);
#endif
        break;
#ifdef CONFIG_JOTG
    case USB_DT_OTG:
        if (IS_DEVICE_OTG())
            rc = send_descriptor_otg(core, wLength, ep0_req_tag);
        else
        {
            DBG_W(DSLAVE_CORE, ("Got \"Get OTG Descriptor \" on a non-OTG "
                "device\n"));
            rc = JEINVAL;
        }
        break;
#endif

    case USB_DT_BOS:
        if (device_desc.bcdUSB == UD_USB_2_0) /* must report unsupported for bcdUSB 2.00 */
        {
            rc = JENOTSUP;
        }
        else /* Send BOS descriptor */
        {
            rc = send_descriptor_bos(core, wLength, ep0_req_tag);  
        }
        break;

    default:
        DBG_W(DSLAVE_CORE, ("Unknown descriptor type requested: %02x\n",
            descriptor_type));
        DBG_W(DSLAVE_CORE, ("Forwarding request to FD\n"));
        rc = JENOTSUP;
    }

    return rc;
}

/**
 * Function name:  handle_set_SEL
 * Description: Handle set SEL request
 * Parameters:
 *     @core: (IN) request Completed request
 * Scope: local
 **/
static void handle_set_SEL(struct request_t *request)
{
  core_t *core = (core_t *)(request->context);
  juint8_t ep0_req_tag = core->ep0_req_tag;
  int rc = 0;
  struct usb_set_SEL_request *data =
    (struct usb_set_SEL_request*)request->buffer.vaddr;
  request_status_t status = request->status;

  request->status = REQUEST_READY;
  if ( (status != REQUEST_COMPLETED) ||
       (request->bytes_transferred != USB_SET_SEL_REQUEST_SIZE) )
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE_CH9, failed_receive_data_stage_for_EP0");
    goto Error;
  }

  DBG_V(DSLAVE_CORE, ("CH9: Got set SEL data\n"));

  /* Let DCD handle the data */
  rc = core->dcd_ops->dcd_ioctl( core->dcd_dev, DRV_IOCTL_SET_SEL, data );
  if (rc)
  {
    goto Error;
  }

  rc = send_zero_packet(core, ep0_req_tag);
  if (rc)
  {
    goto Error;
  }

  return;

Error:
  core_stall_pipe(core, NULL, 1);
}

/**
 * Function name:  get_command_data
 * Description: Data stage for setup eg. SET SELECT setup packet.
 * Parameters: 
 *     @core: (IN) CORE context
 *     @size: (IN) size to be read passed in the wLength field during the SETUP stage.
 *     @complete: (IN) completion callback.
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control 
 *                        request
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t get_command_data(core_t *core, juint16_t size, callback_t complete, juint8_t ep0_req_tag)
{
  if (!core->request)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, core_req_struct_not_ready");
    return JEINVAL;
  }

  if (core->request->status != REQUEST_READY)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, attempt_to_use_a_busy_req");    
    return JEAGAIN;
  }

  if ( size > core->request->buffer.buffer_size )
  {
    HSU_ERR_FATAL("get_command_data - size %u is too big for current buffer %u\n", size, core->request->buffer.buffer_size, 0);
    return JEINVAL;
  }

  DBG_V(DSLAVE_CORE, ("CH9: EP0 getting data, size %lu\n", size));

  core->request->zero          = 0;
  core->request->context       = core;
  core->request->complete      = complete;
  core->request->transfer_size = size;
  core->request->ep0_req_tag = ep0_req_tag;

  return core_read_data(core, NULL, core->request);
}

/* Handle usb command directed for Core */

/**
 * Function name:  core_usb_command
 * Description: Handle standard usb requests (ch 9)
 * Parameters: 
 *     @core: (IN) CORE context
 *     @ctrl_req: (IN) control request
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t core_usb_command(core_t *core, struct usb_ctrlrequest *ctrl_req,
    juint8_t ep0_req_tag)
{
    jresult_t    rc            = 0;
    juint8_t     bmRequestType = ctrl_req->bRequestType;
    juint8_t     bRequest      = ctrl_req->bRequest;
    juint16_t    wValue        = le16toh(ctrl_req->wValue);
    juint16_t    wIndex        = le16toh(ctrl_req->wIndex);
    juint16_t    wLength       = le16toh(ctrl_req->wLength);
    jint_t       interface_index;

    juint8_t     *buffer = (juint8_t *)core->request->buffer.vaddr;

    // the offset bRequest will be out of range for USB_REQ_SET_SEL, USB_REQ_SET_ISOCH_DELAY
    // so print at the case directly
    if ((HSU_CHP9_REQ_GET_STATUS_ULOG + bRequest) < HSU_CHP9_OUT_OF_CMD_ULOG)  
    {
      HSU_ULOG_ENUM(NO_MSG, BUS_LOG, "enter", HSU_CHP9_REQ_GET_STATUS_ULOG, bRequest); 
    }
    
    switch (bRequest)
    {
    case USB_REQ_GET_STATUS:
        DBG_V(DSLAVE_CORE, ("CH9: Get status request %d:%d\n", 
            wIndex, wLength));
        rc = handle_get_status(core, bmRequestType, wIndex, wLength,
            ep0_req_tag);
        break;
    case USB_REQ_CLEAR_FEATURE:
    case USB_REQ_SET_FEATURE:
        DBG_V(DSLAVE_CORE, ("CH9: %s feature request %d:%d\n", 
            bRequest == USB_REQ_SET_FEATURE ? "Set" : "Clear", wValue, wIndex));

        rc = handle_feature(core, bmRequestType, wValue, wIndex, 
            bRequest == USB_REQ_SET_FEATURE ? 1 : 0, ep0_req_tag);
        break;
    case USB_REQ_SET_ADDRESS:
        DBG_V(DSLAVE_CORE, ("CH9: Set Address %d request\n", wValue));

#ifdef FEATURE_HS_USB_GOLD_TREE_TEST_WORKAROUND
        /* HSU FIX - Check whether the SET ADDRESS transaction is successful.
        ** If not, assume that the host last ACK is lost and force the device
        ** to have the new USB address.
        ** This fix resolves CRMDB CR #174688.
        */
        rc = core->dcd_ops->dcd_set_address_state(core->dcd_dev, wValue, TRUE);
        if (rc)
        {
            break;
        }

        rc = send_zero_packet(core, ep0_req_tag);
        if (rc)
        {
            break;
        }

        /* Let the status stage complete. */
        jdelay_ms(1);

        /* Check whether the SET ADDRESS status stage has been completed and 
        ** the device has its new address configured.
        */
        rc = core->dcd_ops->dcd_get_device_address(core->dcd_dev, NULL);
        if (rc)
        {
            /* The status SET ADDRES status stage has not been completed yet.
            ** We assume here that the host's last ACK is lost, and therefore
            ** configure the device's address without waiting.
            */
            rc = core->dcd_ops->dcd_set_address_state(core->dcd_dev, wValue,
                FALSE);
        }
#else
        rc = core->dcd_ops->dcd_set_address_state(core->dcd_dev, wValue,FALSE);
        if (!rc)
        {
            HSU_ULOG_1(NO_MSG, BUS_LOG, "hsu_chp9_req_set_address_send_zlp, wValue %u", wValue);
            rc = send_zero_packet(core, ep0_req_tag);
        }   
#endif  /* FEATURE_HS_USB_GOLD_TREE_TEST_WORKAROUND */
        break;
    case USB_REQ_GET_DESCRIPTOR:
        DBG_V(DSLAVE_CORE, ("CH9: Get descriptor request reached core\n"));
        rc = handle_get_descriptor(core, wValue, wIndex, wLength, ep0_req_tag);
        break;
    case USB_REQ_SET_DESCRIPTOR:
        /* We currently do not support descriptor changes */
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE_CH9, set_descriptor_req_reached_core");
        rc = JENOTSUP;
        break;
    case USB_REQ_GET_CONFIGURATION:
        if (core->usb_state != USB_CONFIGURED && core->curr_config != 0)
        {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE, unconfig_core_appear_configured");            
        }
        buffer[0] = core->curr_config;
        rc = post_command_reply(core, 1, wLength, ep0_req_tag);
        break;
    case USB_REQ_SET_CONFIGURATION:
        rc = chp9_handle_set_configuration(core, (juint8_t)wValue);
        DBG_I(DSLAVE_CORE, ("CH9: Set Confiuration STATUS\n"));
        if (!rc)
        {
            HSU_ULOG(NO_MSG, BUS_LOG, "hsu_chp9_req_set_configuration_send_zlp");
            rc = send_zero_packet(core, ep0_req_tag);
        }
        break;
    case USB_REQ_GET_INTERFACE:
        interface_index = interface_number_to_index(core, wIndex);
        if (core->usb_state != USB_CONFIGURED || wLength != 1 ||
            interface_index >=
            core->configs[core->curr_config - 1].interface_count)
        {
            rc = JEINVAL;
        }
        else
        {
            buffer[0] = core->configs[core->curr_config - 1].interfaces[
                interface_index].if_arr->alt_if_curr;
            rc = post_command_reply(core, 1, wLength, ep0_req_tag);
            core->configs[core->curr_config - 1].interfaces[interface_index].function_remote_wake_tnotifification_pending = FALSE;
        }
        break;
    case USB_REQ_SET_INTERFACE:
        rc = handle_set_interface(core, (juint8_t)wIndex, (juint8_t)wValue);
        if (!rc)
       {
            HSU_ULOG(NO_MSG, BUS_LOG, "hsu_chp9_req_set_iface_send_zlp");            
            rc = send_zero_packet(core, ep0_req_tag);
       }
        break;
    case USB_REQ_SYNCH_FRAME:
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE_CH9, sync_frame_req_reached_core, 0x%02X", bRequest);        
        rc = JENOTSUP;
        break;
    case USB_REQ_SET_SEL:
        HSU_ULOG(NO_MSG, BUS_LOG, "USB_REQ_SET_SEL, core_usb_cmd");
        DBG_V(DSLAVE_CORE, ("CH9: set SEL request, data length=%d\n",
          wLength));
        /* Check length */
        if( wLength != USB_SET_SEL_REQUEST_SIZE )
        {
          rc = JEINVAL;
        }
        else
        {
          /* get set SEL data stage */
          rc = get_command_data(core, wLength, handle_set_SEL, ep0_req_tag);
        }
        break;
    case USB_REQ_SET_ISOCH_DELAY:
        HSU_ULOG(NO_MSG, BUS_LOG, "USB_REQ_SET_ISOCH_DELAY, core_usb_cmd");
        core->isoc_delay = wValue;
        rc = send_zero_packet(core, ep0_req_tag);
        break;
    default:
        /* We can't handle it, the core might want to try FD handling */
        DBG_W(DSLAVE_CORE, ("CH9: Unsupported request recieved\n"));
        HSU_ULOG(NO_MSG, BUS_LOG, "hsu_chp9_req_unsupported_request_recieved");
        rc = JENOTSUP;
    }

    /* HSU addition - standard_cmd_cb_t */
    if (core->device_param.standard_cmd_cb)
    {
      core->device_param.standard_cmd_cb(bmRequestType, bRequest, wValue, wIndex, wLength);
    }

    return rc;
}

static jresult_t chp9_handle_vendor_request(core_t *core, 
    struct usb_ctrlrequest *setup, juint8_t ep0_req_tag)
{
    jresult_t rc = JEINVAL;
    juint8_t req_recip;
    jint_t i;
    /*HSU addition*/
    juint8_t     req_type;

    /* Get the request type */
    req_type = setup->bRequestType & USB_TYPE_MASK;
    /*End of HSU addition*/
    req_recip = setup->bRequestType & USB_RECIP_MASK;

    HSU_ULOG_1(NO_MSG, BUS_LOG, "recip %u", req_recip); //Did not convert offset because range too big.

    switch (req_recip)
    {
    case USB_RECIP_DEVICE:
        {
            jbool_t is_in = setup->bRequestType & USB_DIR_IN;
            juint16_t act_size = 0;
            juint16_t wLength = le16toh(setup->wLength);

            if ((req_type == USB_TYPE_VENDOR) &&
                !((is_in && core->device_param.vendor_cmd_in_cb) ||
                (!is_in && core->device_param.vendor_cmd_out_cb && 
                core->device_param.vendor_cmd_out_data_stage_cb)))
            {
                HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE_CH9, vender_specific_cmd_but_no_handler_reg");                  
                goto Exit;
            }

            /*HSU addition*/
            if ((req_type == USB_TYPE_RESERVED) &&
                !((is_in && core->device_param.reserved_cmd_in_cb) ||
                (!is_in && core->device_param.reserved_cmd_out_cb && 
                core->device_param.reserved_cmd_out_data_stage_cb)))
            {
                HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE_CH9, received_reserve_cmd_but_no_handler_reg");                  
                goto Exit;
            }
            /*End of HSU addition*/

            DBG_V(DSLAVE_CORE, ("CHP9: Handling device recipient vendor "
                "specific request\n"));

            if (wLength > MAX_DESCRIPTOR_SIZE)
                REALLOC_CORE_REQUEST(core, wLength);

            if (is_in)
            {
                /*HSU addition*/
                if (req_type == USB_TYPE_RESERVED)
                {
                    rc = core->device_param.reserved_cmd_in_cb(setup->bRequest, 
                        setup->wValue, setup->wIndex, core->request->buffer.vaddr, 
                        wLength, &act_size);
                } /*End of HSU addition*/
                else
                {
                rc = core->device_param.vendor_cmd_in_cb(setup->bRequest, 
                    setup->wValue, setup->wIndex, core->request->buffer.vaddr, 
                    wLength, &act_size);
                }
                if (rc)
                {
                    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE_CH9,"
                        " failed_to_handle_device_recipitent_IN_vendor_specific_req");                    
                    CORE_REQUEST_RESTORE(core);
                    goto Exit;
                }
                if (wLength && !act_size)
                    core->request->ep0_zlp_reply = 1;

                rc = post_command_reply(core, act_size, wLength, ep0_req_tag);
                if (rc)
                {
                    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE_CH9,"
                        " failed_to_reply_device_recipient_vendor_specific_req, rc %u", rc);                      
                }
                goto Exit;
            }
            else
            {
                /*HSU addition*/
                if (req_type == USB_TYPE_RESERVED)
                {
                    rc = core->device_param.reserved_cmd_out_cb(setup->bRequest,
                        setup->wValue, setup->wIndex, setup->wLength);
                }/*End of HSU addition*/
                else
                {
                rc = core->device_param.vendor_cmd_out_cb(setup->bRequest,
                    setup->wValue, setup->wIndex, setup->wLength);
                }
                if (rc)
                {
                    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CORE_CH9,"
                        " failed_to_handle_device_recipient_OUT_vendor_specific_req");                       
                    goto Exit;
                }

                if (wLength)
                {
                    core->request->zero = 0;
                    core->request->ep0_req_tag = ep0_req_tag;
                    core->request->context = core;
                    /*HSU addition*/
                    if (req_type == USB_TYPE_RESERVED)
                    {
                        core->request->complete = 
                            ep0_reserved_out_data_complete_callback;
                    }/*End of HSU addition*/
                    else
                    {
                    core->request->complete = 
                        ep0_vendor_out_data_complete_callback;
                    }
                    core->request->transfer_size = wLength;
                    core->request->status = REQUEST_READY;

                    rc = core_read_data(core, NULL, core->request);
                    if (rc)
                    {
                        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE_CH9,"
                        " failed_to_read_data_stage, rc %u", rc);                        
                    }
                    goto Exit;
                }
                else
                {
                    rc = send_zero_packet(core, ep0_req_tag);
                    goto Exit;
                }
            }
        }
    case USB_RECIP_INTERFACE:
        /* For vendor command with no specific interface we should try all
         * interfaces */
        if (core->usb_state != USB_CONFIGURED)
            goto Exit;

        for (i = 0; i < core->configs[core->curr_config - 1].interface_count;
            i++)
        {
            rc = core->configs[core->curr_config - 1].interfaces[i].
                handle_usb_cmd(core->configs[core->curr_config - 1].
                interfaces[i].fd->context, (void *)setup);
            if (rc != JENOTSUP)
            {
                DBG_I(DSLAVE_CORE, ("CHP9: Vendor specific request was handled "
                    "by interface %d\n", i));
                goto Exit;
            }
        }
        break;
    }
Exit:
    /* HSU addition start */
    if(rc)
    {
      if (core->usb_state == USB_CONFIGURED)
      {
        for (i = 0; 
             i < core->configs[core->curr_config - 1].interface_count;
             i++)
        {
            rc = core->configs[core->curr_config - 1].interfaces[i].
                handle_usb_cmd(core->configs[core->curr_config - 1].
                interfaces[i].fd->context, (void *)setup);
            if(!rc) break;
        }
      }
    }
   /* HSU addition end */
    return rc;
}

static __INLINE__ jint_t interface_number_to_index(core_t *core, 
    juint8_t number)
{
    jint_t i;
    jint_t idx = core->curr_config ? core->curr_config - 1 : 0;

    for (i = 0; i < core->configs[idx].interface_count; i++)
    {
        if (core->configs[idx].interfaces[i].number == number)
            break;
    }

    return i;
}

/**
 * Function name:  chp9_handle_usb_command
 * Description: Handle an usb chap9 command (decide who's responsibility it is)
 * Parameters: 
 *     @core: (IN) CORE context
 *     @buffer: (IN) the control request
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t chp9_handle_usb_command(core_t *core, void *buffer, 
    juint8_t ep0_req_tag)
{
    jresult_t    rc = 0;
    jint_t       i, j;
    jbool_t      found = 0;
    juint8_t     req_type;
    juint8_t     req_recip;
    jint_t       config_index = core->curr_config ? core->curr_config - 1 : 0;
    jint_t       interface_index = core->configs[config_index].interface_count;
    juint8_t     interface_num = 0;
    juint8_t     endpoint_num;
    struct usb_ctrlrequest *ctrl_req = (struct usb_ctrlrequest*)buffer;
    config_t *config = &core->configs[config_index];

    /* Get the request type */
    req_type = ctrl_req->bRequestType & USB_TYPE_MASK;
    
    /* Get the request pecipient */
    req_recip = ctrl_req->bRequestType & USB_RECIP_MASK;

    DBG_V(DSLAVE_CORE, ("CH9: bRequestType = 0x%x, bRequest = 0x%x,"
        "wValue = 0x%x, wIndex = 0x%x, wLength = 0x%x\n", 
        ctrl_req->bRequestType, ctrl_req->bRequest, ctrl_req->wValue,
        ctrl_req->wIndex, ctrl_req->wLength));

    /* Try to pass the request to Core and see if it can handle it */
    if (req_type == USB_TYPE_STANDARD)
    {
        rc = core_usb_command(core, ctrl_req, ep0_req_tag);
        if (rc != JENOTSUP)
            goto Exit;
    }
    else if (req_type == USB_TYPE_VENDOR && req_recip == USB_RECIP_DEVICE &&
        ((ctrl_req->bRequest == core->msos_vendor_code) && 
        core->msos_vendor_code_set))
    {
        DBG_I(DSLAVE_CORE, ("CH9: request recipient :device, type: vendor \n"));
        interface_num = (juint8_t)(le16toh(USB_REQ_INTERFACE_NUMBER(
            ctrl_req->wValue)));
        /* If we're not configured yet, we'll check the first configuration
         * for the requested interface */
        interface_index = interface_number_to_index(core, interface_num);
    }

/*HSU Addition*/
#if defined (FEATURE_GOBI) && defined (FEATURE_USB_UBIST)
    else if (req_type == USB_TYPE_VENDOR &&
             ctrl_req->bRequest == UBIST_CMDID) 
    {
       juint8_t *p_ubist_status = (juint8_t *)core->request->buffer.vaddr;
       juint8_t ret_status_len = hsu_ubist_handle_command(ctrl_req,
                                                          p_ubist_status);
       if (ret_status_len > 0) {
          rc = post_command_reply(core, ret_status_len, ret_status_len, 
                                  ep0_req_tag);
       }
       else {
          rc = send_zero_packet(core, ep0_req_tag);
       }
       return rc;
    }
#endif /* FEATURE_GOBI && FEATURE_USB_UBIST */ 
/*End of HSU Addition*/

    endpoint_num = (juint8_t)USB_REQ_ENDPOINT_NUMBER(ctrl_req->wIndex);

    /* If not interface was specified, try the first FD
     * else find the exact recipient by iface or endpoint number */
    if (req_recip == USB_RECIP_INTERFACE)
    {
        if (core->usb_state != USB_CONFIGURED)
        {
            rc = JEINVAL;
            goto Exit;
        }

        interface_num = (juint8_t)USB_REQ_INTERFACE_NUMBER(ctrl_req->wIndex);
        interface_index = interface_number_to_index(core, interface_num);
    }
    else if (req_recip == USB_RECIP_ENDPOINT && endpoint_num)
    {
        if (core->usb_state != USB_CONFIGURED)
        {
            rc = JEINVAL;
            goto Exit;
        }

        endpoint_num |= (juint8_t)USB_REQ_ENDPOINT_DIRECTION(ctrl_req->wIndex);

        /* Check out all interfaces */
        for (i = 0; i < config->interface_count && !found; i++)
        {
            /* Check out all active endpoints in that interface */
            for (j = 0; config->interfaces[i].if_curr && 
                j < config->interfaces[i].if_curr->pipe_count && !found; j++)
            {
                /* Did we find it ? */
                if (config->interfaces[i].if_curr->pipes[j].address == 
                    endpoint_num)
                {
                    interface_num = (juint8_t)config->interfaces[i].number;
                    interface_index = i;
                    found = 1;
                }
            }
        }
    }

    if (interface_index >= config->interface_count)
    {
        /*HSU addition*/
        if ((req_type != USB_TYPE_VENDOR) && (req_type != USB_TYPE_RESERVED))
        {
            HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "CORE_CH9," 
              " req_referencing_non_existant_iface %u", interface_num); 
            rc = JEINVAL;
            goto Exit;
        }
        HSU_ULOG(NO_MSG, BUS_LOG, "cmd_to_chp9_handle_vendor_request");
        rc = chp9_handle_vendor_request(core, ctrl_req, ep0_req_tag);
        goto Exit;
    }

    DBG_V(DSLAVE_CORE, ("CH9: Class specific at interface %d\n", 
        interface_num));

    /* Call the FD's EP0 command handler */
    rc = config->interfaces[interface_index].handle_usb_cmd(
        config->interfaces[interface_index].fd->context, buffer);

Exit:
    return rc;
}

/* HSU addition */
/**
 * Function name: chp9_clear_string_descriptors 
 * Description: Clear string descriptors array & indexes
 * Parameters: 
 *     @core: (IN) CORE context
 *
 * Return value: None
 * Scope: global
 **/
void chp9_clear_string_descriptors(core_t *core)
{
  juint8_t i = 0;

  /* Zero string descriptors array */
  for (i = 0; i < MAX_STRING_DESCRIPTORS; i++)
  {
    core->string_descriptors[i] = NULL;
  }

  core->i_product = 0;
  core->i_manufacturer = 0;
  core->i_serial_number = 0;
  config_descriptor.iConfiguration = 0;
}
/* End of HSU addition */

/**
 * Function name: chp9_set_string_descriptors 
 * Description: Set device level string descriptors
 * Parameters: 
 *     @core: (IN) CORE context
 *
 * Return value: None
 * Scope: global
 **/
void chp9_set_string_descriptors(core_t *core)
{
    dev_info_t *dev_info = core->device_param.dev_info;

    if (core->i_product)
    {
        core_change_string_descriptor(core, core->i_product, 
         dev_info->product_name);
    }
    else
    {
        core->i_product = core_add_string_descriptor(core,
         dev_info->product_name, HSU_STR_IDX_PRODUCT);
    }
       
    if (core->i_manufacturer)
    {
        core_change_string_descriptor(core, core->i_manufacturer, 
         dev_info->product_manufacturer);
    }
    else
    {
        core->i_manufacturer = core_add_string_descriptor(core,
         dev_info->product_manufacturer,HSU_STR_IDX_MANUFACTURER);
    }

    if (dev_info->product_serial)
    {
        if (core->i_serial_number)
        {
            core_change_string_descriptor(core, core->i_serial_number, 
             dev_info->product_serial);
        }
        else
        {
            core->i_serial_number = core_add_string_descriptor(core, 
             dev_info->product_serial, HSU_STR_IDX_SERIAL_NUMBER);
        }
    }
    else        /* there's no serial now */
    {
        if (core->i_serial_number)
        {
            /* special case : previously there was a serial descriptor and now
             * there isn't any (probably after device reconfiguration) so we
             * should release the space in the descriptors array 
             * */
            core_release_string_descriptor(core, core->i_serial_number);
            core->i_serial_number = 0;
        }
    }
}

/* HSUSB addition */
#if defined(FEATURE_GOBI)
/**
 * Function name: jusb_gobi_fcc_auth_set_cb
 * Description: Set Gobi FCC Authenication callback.
 * Parameters: 
 *     @cb_func: (IN) Callback function
 *     @fcc_early_passed: (IN) boolean to return if FCC auth already passed or not
 * Return value: None
 * Scope: global
 **/
void jusb_gobi_fcc_auth_set_cb(void * cb_func, boolean *fcc_early_passed)
{

    gobi_fcc_auto_enable_cb = (gobi_void_cb) cb_func;

    /* Set the saved fcc auth passed flag */
    *fcc_early_passed = g_fcc_auth_passed;
}
#endif /* FEATURE_GOBI */
/* End of HSUSB addition */
