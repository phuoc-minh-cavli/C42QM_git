/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 


This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.
*/


/**@file jusb_core_dcd.h

Interface between the Device Controller Driver (DCD) and the core.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/api/wiredconnectivity/hsusb/jusb_core_dcd.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/25/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/ 

#ifndef _JUST_CORE_DCD_H_
#define _JUST_CORE_DCD_H_

#include "jusb_common.h"


/** @ingroup hsusb_jungo_api 
@{ */

#define PIPE_TYPE_MASK(pipe_type) (0x01 << pipe_type)

#define SUPPORTED_PIPE_TYPE(type_map, pipe_type) \
        (type_map & (0x01 << pipe_type))
    
#define SUPPORTED_PIPE_DIR(direction_map, pipe_dir) \
        (direction_map & (0x01 << pipe_dir))

/** DCD functions for use by the core.
*/
typedef struct {
        jresult_t (*dcd_enable)(jdevice_t dev, jbool_t is_full_speed);
          /**< Enables the DCD. */
    void (*dcd_disable)(jdevice_t dev);
          /**< Disables the DCD. */

    jresult_t (*wake_the_host)(jdevice_t dev);
          /**< Software host wakeup. */

    jresult_t (*set_self_power)(jdevice_t dev, juint8_t flag); 
          /**< Changes the device's self-power setting (1 = ON, 0 = OFF). */


    jbool_t   (*dcd_is_otg)(jdevice_t dev);
          /**< TRUE if the device is part of an OTG device. */

    jresult_t (*dcd_invoke_otg_ioctl)(jdevice_t dev, drv_ioctl_t ioctl, 
        void *args);
          /**< Invokes an IOCTL to the OTG stack. */

    jresult_t (*vbus_connect)(jdevice_t dev);
          /**< Connects the device to power. */

    jresult_t (*vbus_draw_amount)(jdevice_t dev, juint32_t power);
          /**< Sets the device power usage. */

    jresult_t (*vbus_disconnect)(jdevice_t dev);
          /**< Disconnects the device from power. */

    jresult_t (*dcd_connect)(jdevice_t dev);    
          /**< Software-controlled connect to the host. */

    jresult_t (*dcd_disconnect)(jdevice_t dev);
          /**< Software-controlled disconnect from the host. */

    jresult_t (*dcd_enable_ep)(jdevice_t dev, pipe_desc_t *pipe);
          /**< Enables an endpoint. */

    jresult_t (*dcd_disable_ep)(jdevice_t dev, pipe_desc_t *pipe);
          /**< Disables an endpoint. */

    jresult_t (*dcd_abort_ep)(jdevice_t dev, pipe_desc_t *pipe);
          /**< Removes all pending I/Os from an endpoint. */

    jresult_t (*dcd_stall_ep)(jdevice_t dev, pipe_desc_t *pipe, 
        jbool_t flag);
          /**< Sets a stall condition on an endpoint (flag: 0 = Clear, 1 = Set). */

    jresult_t (*dcd_send_io_request)(jdevice_t dev, pipe_desc_t *pipe, 
        request_t *request); 
          /**< Sends a new I/O request. */

    jresult_t (*dcd_alloc_pipe)(jdevice_t dev, pipe_desc_t *pipe, 
        juint8_t config_index, juint8_t iface_index, juint8_t alt_index);
          /**< Allocates a pipe for a function driver's use. */

    jresult_t (*dcd_free_pipe)(jdevice_t dev, pipe_desc_t *pipe);
          /**< Frees an allocated pipe. */

    jresult_t (*dcd_get_ep_status)(jdevice_t dev, juint8_t *status, 
        juint8_t ep_num);
          /**< Returns the endpoint status. */

    /* HSU FIX - Check whether the SET ADDRESS transaction is successful.
    ** If not, assume that the host last ACK is lost and force the device
    ** to have the new USB address.
    ** This fix resolves CRMDB CR #174688.
    */
    jresult_t (*dcd_set_address_state)(jdevice_t dev, juint16_t address, 
      jbool_t wait_for_status_stage);
          /**< Moves the device to an addressed state. */

    jresult_t (*dcd_get_device_address)(jdevice_t dev, juint16_t *address_ptr);
          /**< Returns the device address. */
    /* HSU FIX - end. */

    jresult_t (*dcd_handle_feature)(jdevice_t dev, juint16_t feature, 
        juint16_t index, juint8_t set);
          /**< Handles a Set/Clear Feature request. */

    jresult_t (*dcd_set_config_state)(jdevice_t dev, juint8_t config);
          /**< Moves the device to a configured state. */

    juint8_t (*dcd_get_max_packet0)(jdevice_t dev);
          /**< Returns the packet size of EP0. */

    void (*dcd_handle_tx_completion)(jdevice_t dev, pipe_desc_t *pipe);
          /**< Handles a transmit completion. */

    void (*dcd_set_test_mode)(jdevice_t dev, juint16_t test_mode);
          /**< Configures the DCD into electrical test mode. */

    void (*dcd_allow_enumeration)(jdevice_t dev);
          /**< Allows the DCD to perform enumeration. */
    
    void (*dcd_disallow_enumeration)(jdevice_t dev);
          /**< Disallows the DCD to perform enumeration. */

/* HSU addition */
    void (*dcd_register_events)(jdevice_t dev, const void* events);
          /**< Register with DCD for events. */

    jresult_t (*dcd_ioctl)(jdevice_t self, drv_ioctl_t ioctl, void *arg);

    void (*dcd_set_bam_pipe_params)(jdevice_t dev, void *connection, pipe_desc_t *out_pipe, uint32 producer_pipe_index, 
                                    uint32 producer_data_fifo_addr, uint32 producer_data_fifo_size,
                                    uint32 producer_desc_fifo_addr, uint32 producer_desc_fifo_size,
                                    pipe_desc_t *in_pipe, uint32 consumer_pipe_index,
                                    uint32 consumer_data_fifo_addr, uint32 consumer_data_fifo_size,
                                    uint32 consumer_desc_fifo_addr, uint32 consumer_desc_fifo_size);
    void (*dcd_dbm_ep_init)(jdevice_t dev, uint32 dbm_ep_num);
    void (*dcd_dbm_ep_deinit)(jdevice_t dev, uint32 dbm_ep_num);
    void (*dcd_dbm_ep_reset)(jdevice_t dev, uint32 dbm_ep_num);
    void (*dcd_dbm_init)(jdevice_t dev);
    void (*dcd_dbm_init_qdss)(jdevice_t dev, uint32 consumer_pipe_index, uint32 consumer_data_fifo_addr, uint32 consumer_data_fifo_size);
    void (*dcd_function_wakeup_request)(jdevice_t dev, uint8 iface_num);
    void (*dcd_accept_u1_u2)(boolean set_value);
    void (*dcd_get_iface_index)(pipe_desc_t *pipe, uint8* iface_index);
    

    // GSI specifics
    void      (*dcd_setup_event_buf_and_gsi_interrupters)(jdevice_t dev, uint32 idx, uint32 num_of_trbs);
              // Setup event buffers for each GSI endpoint
    void      (*dcd_reset_event_buffer)(jdevice_t dev, uint32 idx);
    void      (*dcd_usb_gsi_endpoint_config)(jdevice_t dev, pipe_desc_t *pipe, juint32_t gsi_interrupter_idx);
              // Configures DEPCFG DEPXFERCFG and DALEPENA
    jresult_t (*dcd_enable_ep_data_struct_only)(jdevice_t dev, pipe_desc_t *pipe);
              // Data structure initialization of the endpoint only, but not fully activated hardware acceleration yet
    jresult_t (*dcd_disable_ep_hw_accel)(jdevice_t dev, pipe_desc_t *pipe);
              // Disable the EP in entirety, requires full initialization of the EP for subsequent use
    jresult_t (*dcd_abort_ep_hw_accel)(jdevice_t dev, pipe_desc_t *pipe);
              // Unique abort EP for GSI endpoints that skips the need  to use streams and requests
    jresult_t (*dcd_begin_hw_accel_xfer)(jdevice_t dev, pipe_desc_t *pipe, void* trb_ring_pa_addr, juint32_t gsi_interrupter_idx, boolean is_ep_resume);
              // Equivalent to a read/write infinite
    jresult_t (*dcd_gsi_post_channel_connect_init)(jdevice_t dev, pipe_desc_t *pipe, juint32_t gsi_interrupter_idx, void* gsi_doorbell_reg, void* trb_ring_pa_addr);
    jresult_t (*dcd_init_ep_trbs_hw_accel)(jdevice_t dev, pipe_desc_t *pipe, void **trb_ring_cookie, void **trb_ring_va_addr, void **trb_ring_pa_addr, juint32_t n);
    jresult_t (*dcd_gsi_general_cfg)(void);
    jresult_t (*dcd_populate_ep_trb_hw_accel)(jdevice_t dev, pipe_desc_t *pipe, 
      juint32_t n, void **next_free_pa_running_ptr, juint32_t client_buf_size,
      void* trb_ring_va_addr, void* trb_ring_pa_addr, void **link_trb_pa_addr, juint32_t usb_gsi_interrupter_index);
    jresult_t (*dcd_set_hwo_bit__updatexfer_ul)(jdevice_t dev, pipe_desc_t *pipe, juint32_t n, void *trb_ring_va_addr);
    jresult_t (*dcd_get_ep_num)(jdevice_t dev, pipe_desc_t *pipe, juint8_t *ep_hw_addr);   
    request_t* (*dcd_get_ep_cur_req)(pipe_desc_t *pipe);
/* End of HSU addition */
} dcd_ops_t;

/** Handles all messages coming to the EP0 (core function for use by DCD).
*/
jresult_t core_handle_ep0(void *core_ctx, juint8_t *buffer, device_speed_t speed);

/** Handles a disconnect from the host (core function for use by DCD).
*/
void core_disconnect(void *core_ctx);

/** Handles suspend signals from the bus (core function for use by DCD).
*/
jresult_t core_suspend(void *core_ctx);

/** Handles resume signals from the bus (core function for use by DCD).
*/
jresult_t core_resume(void *core_ctx);

/** Core function for use by the DCD -- registers the DCD with 
    the core. */
jresult_t core_register_dcd(jdevice_t dev, dcd_ops_t *ops, void **core_ctx);

/** Core function for use by the DCD -- un-registers the DCD from 
    the core. */
void core_unregister_dcd(void *core_ctx);

/** Core function for use by the DCD -- enables a specific 
    controller. */
jresult_t core_enable_by_ctx(void *core_ctx);

/** Core function for use by the DCD -- disables a specific 
    controller. */
void core_disable_by_ctx(void *core_ctx);

/*HSU addition*/ 
/** This function handles a disconnect event on demand. */
jresult_t handle_spoof_disconnect(uint8 core_index);
/*End of HSU addition*/

/** @} */ /* end_group hsusb_jungo_api */

#endif
