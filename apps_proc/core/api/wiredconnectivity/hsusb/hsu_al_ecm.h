#ifndef _HSU_AL_ECM_H_
#define _HSU_AL_ECM_H_

/*=============================================================================

High Speed USB Ethernet Networking control Model (CDC/ECM) Adaptation Layer
H E A D E R   F I L E

=============================================================================*/
/** @file hsu_al_ecm.h

Defines externalized data and functions for the ECM adaptation layer toward
Serial Input/Output (SIO) services.

Externalized functions:
- hsu_al_ecm_control_open
- hsu_al_ecm_data_open
- hsu_al_ecm_control_transmit
- hsu_al_ecm_data_transmit
- hsu_al_ecm_ioctl
- hsu_al_ecm_data_flush_tx
- hsu_al_ecm_control_close
- hsu_al_ecm_data_close

Initialization and sequencing requirements:
The stack must be initialized first (which causes ecm_init to be called), 
before all other functions in this header file can be used.
*/

/*=============================================================================
Copyright (c) 2007 - 2019 by Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
==============================================================================*/

/*=============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/wiredconnectivity/hsusb/hsu_al_ecm.h#1 $
$DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/24/10  lo  (Tech Pubs) Edited/added Doxygen comments and markup.
05/17/06  ke  Initial version

=============================================================================*/
#include <sio.h>
#include <jtypes.h>
#include "bam_dmux.h"

/** @ingroup hsusb_qc_api
    @{
*/

/*=============================================================================
Forward Declarations
=============================================================================*/

/*=============================================================================
Constants
=============================================================================*/

/*=============================================================================
Macros
=============================================================================*/
#define HSU_ENCAPS_CMD_MAX_SIZE 0x2000  /*8K*/ 
/* According to DS team the size of largest QMI msg is 4K (Sep. 2008) */
/* Updated to 8k based on IPCRouter teams demand tp 8K (Aug 2013)*/

#define ECM_MAX_NUM_RB_ITEMS_FOR_WRITE 5


/*=============================================================================
Typedefs
=============================================================================*/

typedef void * cdc_handle_t;
typedef void (*hsu_al_ecm_control_intercept_cb_type) 
(  
  sio_stream_id_type    sio_stream_id,
  sio_port_id_type      sio_port_id,
  sio_rx_func_ptr_type *rx_func_pp,
  dsm_watermark_type   *rx_queue,
  dsm_watermark_type   *tx_queue
);

#ifdef FEATURE_HS_USB_STANDARD_ECM
typedef void (*hsu_al_ecm_usb_iNiche_rx_cb_type) 
(  
  void * data,
  uint16 data_len
);

typedef void (*hsu_al_ecm_usb_iNiche_rx_done_cb_type) 
(  
  void * data
);

typedef void (*hsu_al_ecm_usb_iNiche_tx_cb_type) 
(  
  void * data,
  void * tx_priv,
  uint16 data_len
);

typedef void (*hsu_al_ecm_usb_iNiche_tx_done_cb_type) 
(  
  void * tx_priv
);
#endif /*FEATURE_HS_USB_STANDARD_ECM*/

/*Prototypes for the function call backs which has to be exposed to QBI*/
typedef void (*hsu_al_ecm_set_ntb_input_size_complete_cb_type)
(
	cdc_handle_t handle,
	jresult_t status
);

typedef void (*hsu_al_ecm_reset_mbim_complete_cb_type)
(
	cdc_handle_t handle,
	jresult_t  status
);

typedef void (*hsu_al_ecm_reset_mbim_cb_type)(cdc_handle_t handle);

typedef void (*hsu_al_ecm_set_ntb_input_size_cb_type)
(
	cdc_handle_t handle,
	juint32_t ntb_size,  /*Gives the maximum NTB Size*/
	juint16_t max_datagrams_in_ntb
);

/*Callbacks Structure for MBIM Interface Callback from HSUSB to QBI*/
typedef struct
{
	hsu_al_ecm_control_intercept_cb_type 			hsu_al_ecm_control_intercept_cb;	
	hsu_al_ecm_reset_mbim_cb_type 					hsu_al_ecm_hsu_al_ecm_reset_mbim_cb;
	hsu_al_ecm_set_ntb_input_size_cb_type 			hsu_al_ecm_hsu_al_ecm_set_ntb_input_size_cb;
} hsu_al_ecm_register_mbim_cb_list_usb_to_qbi_type;

/*Callbacks Structure for MBIM Interface from QBI to HSUSB*/
typedef struct
{
	hsu_al_ecm_set_ntb_input_size_complete_cb_type 	hsu_al_ecm_set_ntb_input_size_complete_cb;
	hsu_al_ecm_reset_mbim_complete_cb_type 			hsu_al_ecm_reset_mbim_complete_cb;
}hsu_al_ecm_register_mbim_cb_list_qbi_to_usb_type;

/*Finally callback Structure for interfacing with QBI and USB */
typedef struct
{
	hsu_al_ecm_register_mbim_cb_list_usb_to_qbi_type 	hsu_al_ecm_usb_to_qbi;
	hsu_al_ecm_register_mbim_cb_list_qbi_to_usb_type 	hsu_al_ecm_qbi_to_usb;	
}hsu_al_ecm_register_mbim_cb_list_type;

/*=============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/

#ifdef FEATURE_HS_USB_STANDARD_ECM
void hsu_al_ecm_usb_iNiche_rx_done_cb
(
  void * data
);

void hsu_al_ecm_usb_iNiche_tx_cb
(
  void * data,
  void * tx_priv,
  uint16 data_len
);

boolean hsu_al_ecm_is_standard_ecm_opened (void);

/*=============================================================================
FUNCTION    usb_iNiche_cb_reg
=============================================================================*/
/**   
Called by Network Stack to do CB registration.

@param 

@dependencies  


@sideeffects  
None.

@return  
SUCCESS                 -- All the callbacks are registered properly.
FAILURE                 -- something is not right or USB driver stack is not up etc.
*/
boolean hsu_al_ecm_usb_iNiche_cb_reg
(
 hsu_al_ecm_usb_iNiche_rx_cb_type      usb_iNiche_rx_cb,
 hsu_al_ecm_usb_iNiche_rx_done_cb_type *usb_iNiche_rx_done_cb,
 hsu_al_ecm_usb_iNiche_tx_cb_type      *usb_iNiche_tx_cb,
 hsu_al_ecm_usb_iNiche_tx_done_cb_type usb_iNiche_tx_done_cb
 );
#endif /*FEATURE_HS_USB_STANDARD_ECM*/

/*=============================================================================
FUNCTION    hsu_al_ecm_control_open
=============================================================================*/
/**   
Called by the application to allocate and configure the control plane for the 
network device.

@param *open_ptr [IN] -- Pointer to the ECM instance configuration.

@dependencies  
ecm_init() must have been called for this instance.

@sideeffects  
None.

@return  
SIO_DONE_S                 -- SIO stream as specified in open_ptr has been
                              successfully opened.\n
SIO_BADP_S                 -- Bad parameter.\n
SIO_UNAVAIL_S              -- All available streams have been allocated.\n
SIO_BIRATE_CHANGE_FAILED_S -- Attempt to change bit rate failed.
*/
sio_status_type hsu_al_ecm_control_open
(
 sio_open_type * open_ptr
 );

/*=============================================================================
FUNCTION    hsu_al_ecm_data_open
=============================================================================*/
/**   
Called by the application to allocate and configure the data plane for the 
network function.

@param *open_ptr [IN] -- Pointer to the ECM instance configuration.

@dependencies  
ecm_init() and hsu_al_ecm_control_open() must have been called before this 
instance.

@sideeffects  
This causes two buffers to be given to the USB stack, to be used when OUT 
transfers arrive.

@return  
SIO_DONE_S                 -- SIO stream as specified in open_ptr has been
                              successfully opened.\n
SIO_BADP_S                 -- Bad parameter.\n
SIO_UNAVAIL_S              -- All available streams have been allocated.\n
SIO_BIRATE_CHANGE_FAILED_S -- Attempt to change bit rate failed.
*/
sio_status_type hsu_al_ecm_data_open
(
 sio_open_type * open_ptr
 );

/*=============================================================================
FUNCTION   hsu_al_ecm_control_transmit
=============================================================================*/
/**
Called by the application when it needs to send a message on the control
channel.\ The message is given as a chain of DSM items. 

This function gathers the DSM chain into a pre-allocated buffer and gives it to
the cdc_send_encap_response() function of the USB stack. 

The stack is responsible for generating a Response Available notification and 
for sending the message after the host sends an IN token on the control channel
following the notification. 

Only one control message can be sent per instance at a time. This means that, 
from the moment cdc_send_encap_response was called and until the 
hsu_al_ecm_encap_response_complete_cb was called, additional calls to 
cdc_send_encap_response() will fail. Therefore, additional calls to
hsu_al_ecm_control_transmit causes the DSM chain to be queued to the control
Tx WaterMark Q. 

@param stream_id [IN] -- ID of the stream.
@param *tx_ptr   [IN] -- Pointer to the Tx buffer.

@dependencies  
ecm_init() and hsu_al_ecm_control_open() must have been called before this 
instance.

@sideeffects
None.

@return
None.
*/
void hsu_al_ecm_control_transmit
( 
 sio_stream_id_type  stream_id, 
 dsm_item_type *     tx_ptr
 );

/*=============================================================================
FUNCTION    hsu_al_ecm_data_transmit 
=============================================================================*/
/**
Called by the application when it needs to send data on the bulk end point.
The data (Ethernet frame) is given as a chain of DSM items.

The function gathers the DSM chain into a pre-allocated buffer using the DMOV,
then gives it to the cdc_write() function of the USB stack.

@param stream_id [IN] -- ID of the stream.
@param port_id   [IN] -- ID of the port.
@param *tx_ptr   [IN] -- Pointer to the Tx buffer.

@dependencies
ecm_init(), hsu_al_ecm_control_open() and hsu_al_ecm_data_open() must have
been called before this instance.

@sideeffects
None.

@return
None.
*/
void hsu_al_ecm_data_transmit
( 
 sio_stream_id_type  stream_id,
 sio_port_id_type    port_id,
 dsm_item_type *     tx_ptr 
 );


/*=============================================================================
FUNCTION  hsu_al_ecm_control_ioctl
=============================================================================*/
/**   
Used to control various features of the USB function.  
The behavior of this function depends on the command issued.

@param stream_id [IN] -- ID of the stream.
@param port_id   [IN] -- ID of the port.
@param cmd       [IN] -- IOCTL command.
@param *param    [IN/OUT] -- Parameter that is associated with the IOCTL.

@dependencies  
The instance must be in the OPEN state, meaning ecm_init(), 
hsu_al_ecm_control_open(), and hsu_al_ecm_open_data() must have been called
before this instance. When the command is SIO_IOCTL_ENABLE_DTR_EVENT, the value
of *param cannot be NULL.

@sideeffects  
None.

@return  
None.
*/
void hsu_al_ecm_control_ioctl
(
 sio_stream_id_type      stream_id,
 sio_port_id_type        port_id,
 sio_ioctl_cmd_type      cmd,
 sio_ioctl_param_type *  param
);


/*=============================================================================
FUNCTION  hsu_al_ecm_ioctl
=============================================================================*/
/**   
Used to control various features of the USB function.  
The behavior of this function depends on the command issued.

@param stream_id [IN] -- ID of the stream.
@param port_id   [IN] -- ID of the port.
@param cmd       [IN] -- IOCTL command.
@param *param    [IN/OUT] -- Parameter that is associated with the IOCTL.

@dependencies  
The instance must be in the OPEN state, meaning ecm_init(), 
hsu_al_ecm_control_open(), and hsu_al_ecm_open_data() must have been called
before this instance. When the command is SIO_IOCTL_ENABLE_DTR_EVENT, the value
of *param cannot be NULL.

@sideeffects  
None.

@return  
None.
*/
void hsu_al_ecm_ioctl
(
 sio_stream_id_type      stream_id,
 sio_port_id_type        port_id,
 sio_ioctl_cmd_type      cmd,
 sio_ioctl_param_type *  param
 );

/*=============================================================================
FUNCTION    hsu_al_ecm_data_flush_tx 
=============================================================================*/
/**
Flushes the Tx data pipe.

@param stream_id       [IN] -- ID of the stream.
@param port_id         [IN] -- ID of the port.
@param *flush_func_ptr [IN] -- Pointer to a callback to be called on completion.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_ecm_data_flush_tx
( 
 sio_stream_id_type  stream_id,   
 sio_port_id_type    port_id,
 void                (*flush_func_ptr)(void)  
);

/*=============================================================================
FUNCTION    hsu_al_ecm_control_close
=============================================================================*/
/**
Closes the ECM control pipe.

@param stream_id       [IN] -- ID of the stream.
@param *close_func_ptr [IN] -- Pointer to a callback to be called on completion.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_ecm_control_close
( 
 sio_stream_id_type  stream_id,
 void                (*close_func_ptr)(void)  
);

/*=============================================================================
FUNCTION    hsu_al_ecm_data_close
=============================================================================*/
/**
Closes the ECM data pipe

@param stream_id       [IN] -- ID of the stream.
@param port_id         [IN] -- ID of the port.
@param *close_func_ptr [IN] -- Pointer to a callback to be called on completion.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_ecm_data_close
( 
 sio_stream_id_type stream_id,
 sio_port_id_type   port_id,
 void              (*close_func_ptr)(void)    
);

/*=============================================================================
FUNCTION    hsu_al_ecm_set_standard_ecm
=============================================================================*/
/**
Used for unit testing only.

@param is_standard [IN] -- Whether standard ECM is used. 

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_ecm_set_standard_ecm(boolean is_standard);

/*=============================================================================
FUNCTION    hsu_al_ecm_pool_many_cb
=============================================================================*/
/**
Invokes the appropriate actions when the ECM memory pool arrives 
to DSM_MEM_LEVEL_USB_MANY on DSM_MEM_OP_FREE.

@param pool_id [IN] -- ID of the pool.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_ecm_pool_many_cb(dsm_mempool_id_enum_type pool_id);

/*=============================================================================
FUNCTION    hsu_al_ecm_register_ctl_intrcpt_callbacks
=============================================================================*/
/**

Register intercept functions of control messages between SIO and HSU_AL_ECM

@param control_intercept_cb [IN] -- callback for setting up intercept of control 
msgs

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_ecm_register_control_intercept_cb
( 
  hsu_al_ecm_control_intercept_cb_type control_intercept_cb
);
/*=============================================================================
FUNCTION    hsu_al_ecm_register_mbim_callbacks
=============================================================================*/
/**

Registers the MBIM function Callbacks between QBI and HSUSB

@param cb_list [IN] -- object containing the list of Callback functions 


@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_ecm_register_mbim_cb
(
    hsu_al_ecm_register_mbim_cb_list_type *cb_list
);



/*=============================================================================
FUNCTION    hsu_al_ecm_enable_flow_all_bam
=============================================================================*/
/**
Prepares the eTD and issues the read/write infinite.  Essentially opens the gate
 for traffic to flow between Host Controller and Device Controller
Function connects to IPA BAM if it has never been connected prior.
Function can be safely invoked multiple times.

@param
None.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_ecm_enable_flow_all_bam(boolean ul, boolean dl);

/*=============================================================================
FUNCTION    hsu_al_ecm_enable_read_write_infinite_all_bam_ep
=============================================================================*/
/**
  This API should only be used for the bootup case where BAM pipe INIT implicitly
  performs a BAM pipe ENABLE
*/
void hsu_al_ecm_enable_read_write_infinite_all_bam_ep(void);

void hsu_al_ecm_tx_data (bam_dmux_handle  handle,void *priv,void *rx_priv,void *data,uint16 data_len);

void hsu_al_ecm_rx_complete(bam_dmux_handle handle, void  *priv, void *tx_priv, void *data, uint16 data_len, bam_dmux_status status);



/** @} */ /* endgroup hsusb_qc_api */
#endif /* _HSU_AL_ECM_H_ */
