/*==============================================================================

High Speed USB SIO functions registration 

GENERAL DESCRIPTION
This module is responsible for registering HSUSB driver into SIO.

EXTERNALIZED FUNCTIONS
hsu_sio_reg_hsusb_driver

INITALIZATION AND SEQUENCING REQUIREMENTS
None.

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/adapt_layers/hsu_al_sio_reg.c#1 $
$DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when      who     what, where, why
--------  ---     ------------------------------------------------------------
02/04/09  sm      inital varsion

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_common_int.h" 

#ifdef FEATURE_HSUSB_WRAPPER_FREE

#include "hsu_al_sio_reg.h"
#include "hsu_al_ser.h"
#include "hsu_al_ecm_int.h"
#include "hsu_host_al_eem.h"

/*==============================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/
/*==============================================================================
Typedefs
==============================================================================*/
/*==============================================================================
Variables
==============================================================================*/
/*==============================================================================
Forward Declarations
==============================================================================*/

static void hsu_sio_reg_ecm_control_close_wrapper
( 
  sio_stream_id_type stream_id, 
  sio_port_id_type   port_id,
  void               (*close_func_ptr)(void)
);

static void hsu_sio_reg_ecm_control_transmit_wrapper
( 
  sio_stream_id_type stream_id, 
  sio_port_id_type   port_id,
  dsm_item_type*     tx_ptr
);

static void al_sio_set_func_cb 
(
  sio_device_interafce* device_iface, 
  boolean is_driver_available, 
  sio_status_type (*data_open) (sio_open_type *), 
  void (*data_close) (sio_stream_id_type, sio_port_id_type, void (*)(void)), 
  void (*ioctl) (sio_stream_id_type, sio_port_id_type, sio_ioctl_cmd_type, sio_ioctl_param_type *), 
  void (*data_transmit) (sio_stream_id_type, sio_port_id_type, dsm_item_type*), 
  void (*flush_tx) (sio_stream_id_type, sio_port_id_type, void (*)(void)), 
  void (*disable_device) (sio_port_id_type), 
  sio_status_type (*control_open) (sio_open_type *), 
  void (*control_close) (sio_stream_id_type, sio_port_id_type, void (*)(void)), 
  void (*control_ioctl) (sio_stream_id_type, sio_port_id_type, sio_ioctl_cmd_type, sio_ioctl_param_type *), 
  void (*control_transmit) (sio_stream_id_type, sio_port_id_type, dsm_item_type*)
);

static uint32 al_sio_reg_device_iface(sio_device_interafce* device_iface, 
  uint32 sio_major_num, uint32 sio_minor_num, 
  sio_register_device_driver_cb_type register_driver_fn);
/*==============================================================================

FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/
#ifdef FEATURE_HS_USB_ECM
/*==============================================================================

FUNCTION          HSU_SIO_REG_ECM_CONTROL_CLOSE_WRAPPER

DESCRIPTION   
  This function is a wrapper function for hsu_al_ecm_control_close().

DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
  None.
==============================================================================*/
static void hsu_sio_reg_ecm_control_close_wrapper
( 
  sio_stream_id_type stream_id, 
  sio_port_id_type port_id,
  void             (*close_func_ptr)(void)
)
{ 
  HSU_USE_PARAM(port_id);
  hsu_al_ecm_control_close(stream_id, close_func_ptr);
}
/*==============================================================================

FUNCTION          HSU_SIO_REG_ECM_CONTROL_TRANSMIT_WRAPPER

DESCRIPTION   
  This function is a wrapper function for hsu_al_ecm_control_transmit().

DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
  None.
==============================================================================*/
static void hsu_sio_reg_ecm_control_transmit_wrapper
( 
  sio_stream_id_type stream_id, 
  sio_port_id_type port_id,
  dsm_item_type *tx_ptr
)
{ 
  HSU_USE_PARAM(port_id);
  hsu_al_ecm_control_transmit(stream_id, tx_ptr);
}
#endif /* FEATURE_HS_USB_ECM */


/*==============================================================================

FUNCTION          SIO_HSUSB_DRIVER_INIT

DESCRIPTION   
  This function responsible for mapping HSUSB driver functions to SIO.

DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
  None.
==============================================================================*/

void sio_hsusb_driver_init(sio_register_device_driver_cb_type register_driver_fn)
{
  sio_driver_interface driver_iface;
  sio_device_interafce device_iface;
  uint32 ret;

  device_iface.driver_interface = &driver_iface;

/* SER */
#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  al_sio_set_func_cb(&device_iface ,TRUE, hsu_al_ser_open, hsu_al_ser_close, hsu_al_ser_ioctl,
    hsu_al_ser_transmit, hsu_al_ser_flush_tx, NULL, NULL, NULL, NULL, NULL);
#else 
  al_sio_set_func_cb(&device_iface ,FALSE, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL);
#endif

 /* USB_MDM */
  ret = al_sio_reg_device_iface(&device_iface, SIO_MAJOR_USB, SIO_MINOR_HSUSB_MDM,
    register_driver_fn);
  HSU_ASSERT(ret);

/* USB_SER1/DIAG */
#if defined (SIO_USES_FSHS_USB) && (defined (FEATURE_USB_DIAG) || defined(FEATURE_USB_DIAG_NMEA))
  ret = al_sio_reg_device_iface(&device_iface, SIO_MAJOR_USB, SIO_MINOR_HSUSB_SER1,
    register_driver_fn);
  HSU_ASSERT(ret);
#endif /* SIO_USES_FSHS_USB && (FEATURE_USB_DIAG || FEATURE_USB_DIAG_NMEA) */

 /* USB_SER2 */
#if defined (FEATURE_USB_DIAG_NMEA) 
  ret = al_sio_reg_device_iface(&device_iface, SIO_MAJOR_USB, SIO_MINOR_HSUSB_SER2,
    register_driver_fn);
  HSU_ASSERT(ret);
#endif /* FEATURE_USB_DIAG_NMEA */

 /* USB_SER3 */
#if defined(SIO_HAS_USB_SER3) 
  ret = al_sio_reg_device_iface(&device_iface, SIO_MAJOR_USB, SIO_MINOR_HSUSB_SER3,
    register_driver_fn);
  HSU_ASSERT(ret);
#endif

/* USB_SER4 */
#ifdef FEATURE_HS_USB_SER4_PORT
	#if defined(SIO_HAS_USB_SER4) 
	  ret = al_sio_reg_device_iface(&device_iface, SIO_MAJOR_USB, SIO_MINOR_HSUSB_SER4,
	    register_driver_fn);
	  HSU_ASSERT(ret);
	#endif /* SIO_HAS_USB_SER4 */
#endif /* FEATURE_HS_USB_SER4_PORT */
     
#ifdef FEATURE_HS_USB_QDSS_PORT
	#if defined(SIO_HAS_USB_QDSS) 
	   ret = al_sio_reg_device_iface(&device_iface, SIO_MAJOR_USB, SIO_MINOR_USB_QDSS,
		 register_driver_fn);
	   HSU_ASSERT(ret);
	#endif /* SIO_HAS_USB_SER4 */
#endif /* FEATURE_HS_USB_SER4_PORT */
     
 /* RM_NET */
#ifdef FEATURE_HS_USB_ECM
#if defined (SIO_USES_FSHS_USB) && !defined (FEATURE_HS_USB_NO_HS_OTG_CORE)  
  al_sio_set_func_cb(&device_iface, TRUE, hsu_al_ecm_data_open, hsu_al_ecm_data_close, 
   hsu_al_ecm_ioctl, hsu_al_ecm_data_transmit, hsu_al_ecm_data_flush_tx, NULL, hsu_al_ecm_control_open,
   hsu_sio_reg_ecm_control_close_wrapper, hsu_al_ecm_control_ioctl, hsu_sio_reg_ecm_control_transmit_wrapper);
#else
 al_sio_set_func_cb(&device_iface ,FALSE, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL);
#endif /* SIO_USES_FSHS_USB && !FEATURE_HS_USB_NO_HS_OTG_CORE */
    
  ret = al_sio_reg_device_iface(&device_iface, SIO_MAJOR_HSUSB_ECM, SIO_MINOR_HSUSB_ECM_NET_WWAN,
    register_driver_fn);
  HSU_ASSERT(ret);
#endif /* FEATURE_HS_USB_ECM */

// This API is actually not called, but added code for placeholder purposes
// DPL
 al_sio_set_func_cb(&device_iface, TRUE, hsu_al_ecm_data_open, hsu_al_ecm_data_close, 
   hsu_al_ecm_ioctl, hsu_al_ecm_data_transmit, hsu_al_ecm_data_flush_tx, NULL, hsu_al_ecm_control_open,
   hsu_sio_reg_ecm_control_close_wrapper, hsu_al_ecm_control_ioctl, hsu_sio_reg_ecm_control_transmit_wrapper);

  ret = al_sio_reg_device_iface(&device_iface, SIO_MAJOR_HSUSB_ECM, SIO_MINOR_USB_DPL,
    register_driver_fn);
  HSU_ASSERT(ret);

/* EEM */
#if defined (FEATURE_HS_USB_UICC)
  al_sio_set_func_cb (&device_iface, TRUE, hsu_host_al_eem_data_open, hsu_host_al_eem_data_close,
  hsu_host_al_eem_ioctl, hsu_host_al_eem_data_transmit,
  hsu_host_al_eem_data_flush_tx, NULL, NULL, NULL, NULL, NULL);
#else
  al_sio_set_func_cb(&device_iface, FALSE, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL);
#endif /* FEATURE_HS_USB_UICC */
  ret = al_sio_reg_device_iface (&device_iface, SIO_MAJOR_HSUSB_EEM, SIO_MINOR_HSUSB_EEM,
    register_driver_fn);

  HSU_ASSERT(ret);
}

/*==============================================================================

FUNCTION          AL_SIO_REG_DEVICE_INFACE

DESCRIPTION   
  This function responsible for registering additional details for each 
  class driver (e.g majoy and minor numbers).

DEPENDENCIES  
  None.

RETURN VALUE  
  0 if fails.

SIDE EFFECTS  
  None.
==============================================================================*/
static uint32 al_sio_reg_device_iface(sio_device_interafce* device_iface, 
  uint32 sio_major_num, uint32 sio_minor_num, 
  sio_register_device_driver_cb_type register_driver_fn)
{
  device_iface->device_id        = DEVICE_ID(sio_major_num, sio_minor_num); 
  return register_driver_fn(device_iface);
}
/*==============================================================================

FUNCTION          AL_SIO_SET_FUNC_CB

DESCRIPTION   
  This function responsible for mapping HSUSB driver functions to SIO.

DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
  None.
==============================================================================*/
static void al_sio_set_func_cb 
(
  sio_device_interafce* device_iface, 
  boolean is_driver_available, 
  sio_status_type (*data_open) (sio_open_type *), 
  void (*data_close) (sio_stream_id_type, sio_port_id_type, void (*)(void)), 
  void (*ioctl) (sio_stream_id_type, sio_port_id_type, sio_ioctl_cmd_type, sio_ioctl_param_type *), 
  void (*data_transmit) (sio_stream_id_type, sio_port_id_type, dsm_item_type*), 
  void (*flush_tx) (sio_stream_id_type, sio_port_id_type, void (*)(void)), 
  void (*disable_device) (sio_port_id_type), 
  sio_status_type (*control_open) (sio_open_type *), 
  void (*control_close) (sio_stream_id_type, sio_port_id_type, void (*)(void)), 
  void (*control_ioctl) (sio_stream_id_type, sio_port_id_type, sio_ioctl_cmd_type, sio_ioctl_param_type *), 
  void (*control_transmit) (sio_stream_id_type, sio_port_id_type, dsm_item_type*)
)
{
  sio_driver_interface* driver_iface = device_iface->driver_interface;
  driver_iface->HW_open             = data_open;
  driver_iface->HW_close            = data_close; 
  driver_iface->HW_ioctl            = ioctl; 
  driver_iface->HW_transmit         = data_transmit;
  driver_iface->HW_flush_tx         = flush_tx;
  driver_iface->HW_disable_device   = disable_device;
  driver_iface->HW_control_open     = control_open;
  driver_iface->HW_control_close    = control_close;
  driver_iface->HW_control_ioctl    = control_ioctl;
  driver_iface->HW_control_transmit = control_transmit;
  device_iface->forward_port_id     = SIO_PORT_NULL;
  if (is_driver_available == TRUE)
  {
    device_iface->forward_port = FALSE;
    device_iface->driver_available = TRUE;
  }
  else  
  {
    device_iface->forward_port = TRUE;
    device_iface->driver_available = FALSE;
  }
}
#endif /* FEATURE_HSUSB_WRAPPER_FREE */
