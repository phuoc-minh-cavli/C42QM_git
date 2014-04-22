#ifndef _HSU_AL_ECM_INT_H_
#define _HSU_AL_ECM_INT_H_

/*==============================================================================

High Speed USB Ethernet Networking control Model (CDC/ECM) Adaptation Layer
H E A D E R   F I L E

GENERAL DESCRIPTION
Defines externalized data and functions for the ECM adaptation layer, 
towards SIO

EXTERNALIZED FUNCTIONS
hsu_al_ecm_control_open
hsu_al_ecm_data_open
hsu_al_ecm_control_transmit
hsu_al_ecm_data_transmit
hsu_al_ecm_ioctl
hsu_al_ecm_data_flush_tx
hsu_al_ecm_control_close
hsu_al_ecm_data_close

INITALIZATION AND SEQUENCING REQUIREMENTS
The stack must be initialized first (which causes ecm_init to be called), 
before all other functions in this header file can be used.

Copyright (c) 2007 - 20017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_al_ecm_int.h#1 $
$DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
05/17/06  ke  Initial version

==============================================================================*/
#include "hsu_al_ecm.h"
#include "hsu_al_mem.h"

/*==============================================================================
Forward Declarations
==============================================================================*/

/*==============================================================================
Constants
==============================================================================*/
/* The maximum number of buffers to pass to the HW
  * This value should be at least 5 in order to achieve moderate throughput */
#ifndef HS_USB_ECM_TX_MAX_PENDING_BUFFERS
  #define HS_USB_ECM_TX_MAX_PENDING_BUFFERS 30
#elif (HS_USB_ECM_TX_MAX_PENDING_BUFFERS < 5)
  #error "HS_USB_ECM_TX_MAX_PENDING_BUFFERS must be at least 5"
#endif /* ! HS_USB_ECM_TX_MAX_PENDING_BUFFERS */

/*==============================================================================
Macros
==============================================================================*/

/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
/*===========================================================================

FUNCTION hsu_al_ecm_forward_rx_to_app

DESCRIPTION
Function calls either the Rx callback or enqueue in WMQ for the DSM item received.


DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS  
None

===========================================================================*/
void hsu_al_ecm_forward_rx_to_app(rb_t *rb);

/*===========================================================================

FUNCTION hsu_al_ecm_forward_rx_to_app

DESCRIPTION
Function calls either the control Rx callback or enqueue in WMQ for the DSM 
item received.


DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS  
None

===========================================================================*/
void hsu_al_ecm_forward_control_rx_to_app(dsm_item_type *dsm_ptr);

/*===========================================================================

FUNCTION      HSU_AL_ECM_ABORT_ALL_SPS_PIPE

DESCRIPTION   
  Deprimes the DL or UL endpoints for all SPS IN or OUT pipes.  This is used to 
  guarantee thre would be no more UL or DL data comming into SPS pipe.

  Set abort_write to TRUE to abort write pipe.
  Set abort_read to TRUE to abort read pipe.
  Setting FALSE to both will result in crash since it is misuse of the API.

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
void hsu_al_ecm_abort_all_sps_pipe(boolean abort_write, boolean abort_read);

/*===========================================================================

FUNCTION      hsu_al_ecm_set_ignore_disconnect

DESCRIPTION
Enable/disable ignoreing disable event from CDC.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_al_ecm_set_ignore_disconnect(int instance_index, boolean value);

#ifdef FEATURE_HS_USB_ECM_INT_EP_SHARING    
/*===========================================================================

FUNCTION      hsu_al_ecm_get_shared_intr_pipe_handle

DESCRIPTION   
  Returns the handle for first ECM instance which has the Interrupt Pipe enabled,
  this intrrupt ppipe will be shared for all the ECM instances for sending 
  notifications.
  
DEPENDENCIES  
  FEATURE_HS_USB_ECM_INT_EP_SHARING should be enabled else will return void.

RETURN VALUE  
  cdc_handle for teh first ECM instance.

SIDE EFFECTS  
None.

==============================================================================*/
cdc_handle_t hsu_al_ecm_get_shared_intr_pipe_handle(void);
#endif /* FEATURE_HS_USB_ECM_INT_EP_SHARING */

/*===========================================================================

FUNCTION      hsu_al_ecm_is_appctx_ecm

DESCRIPTION   
  Checks if context is ecm.

DEPENDENCIES
  Assumes context is populated before it is called.
==============================================================================*/
boolean hsu_al_ecm_is_appctx_ecm(void *appctx);


/*===========================================================================

FUNCTION      hsu_al_ecm_is_bam_flow_enabled

DESCRIPTION   
  Assumes all interfaces are all enabled/disabled solely based on the first interface.
  This is used to prevent double initializations during bootup due to 
  SET CONFIG -> SET INTERFACE, vs. ECM ENABLE -> SET CONFIG without SET INTERFACE

DEPENDENCIES
  
==============================================================================*/
boolean hsu_al_ecm_is_bam_flow_enabled(void);

/*===========================================================================

FUNCTION      hsu_al_ecm_print_f3

DESCRIPTION   
  Prints ECM stats for F3

DEPENDENCIES
  
==============================================================================*/
void hsu_al_ecm_print_f3(void);


/*===========================================================================

FUNCTION      hsu_al_ecm_enable_flow_with_bam

DESCRIPTION   
  Performs the read/write infinite

DEPENDENCIES
  
==============================================================================*/
boolean hsu_al_ecm_enable_flow_with_bam(void* ctx_param, boolean ul, boolean dl);
#endif /* _HSU_AL_ECM_INT_H_ */
