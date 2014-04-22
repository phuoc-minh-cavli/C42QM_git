#ifndef SIO_WRAPPER_H
#define SIO_WRAPPER_H
/*===========================================================================

                S E R I A L    I / O    S E R V I C E S

                  W R A P P E R    F U N C T I O N S

                       H E A D E R    F I L E

DESCRIPTION
  This file contains types and declarations associated with the DMSS
  Serial I/O Services.

Copyright (c) 2007-2012,2019 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.tx/6.0/buses/sio/src/amss/sio_wrapper.h#2 $ $DateTime: 2020/07/03 00:53:16 $ $Author: pwbldsvc $

mm/dd/yy   who     what, where, why
--------   ---     ----------------------------------------------------------
5/24/19    ss      Memory optimisation on Threadx
01/05/12   tl      Added USB_SER4 port.
06/23/09   amv     Added sio critical sections to replace INTLOCKs
01/11/07   rh      Created module.

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "sio.h"
#include "sio_priv.h"

#ifdef FEATURE_SMD
#include "smd_sio.h"
#endif

/*===========================================================================

                        FEATURE FLAG MAPPING

===========================================================================*/

#if (defined(FEATURE_HS_USB) && !defined(FEATURE_NO_USB_HW_ON_PROCESSOR))
#define SIO_USES_HS_USB
#endif

#if defined(FEATURE_USB) || defined(SIO_USES_HS_USB)
#define SIO_USES_FSHS_USB
#endif

#if (defined (FEATURE_USB_DIAG_NMEA) || defined(FEATURE_USB_DIAG)) && \
   !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
#ifdef SIO_USES_HS_USB
#define SIO_HAS_USB_SER3
#define SIO_HAS_USB_SER4
#define SIO_HAS_USB_QDSS
#endif
#endif

/* For 7500, USB2 signal uses UART1 MODEM signals, remove modem GPIOs if
 * UART2 is in use                                                         */
#ifdef FEATURE_USB2_HOST_PORT
#define SIORS_UART1_USES_4WIRE_ONLY
#endif

#if !defined(FEATURE_FIRST_UART) && !defined(FEATURE_SECOND_UART) && !defined(FEATURE_THIRD_UART)
#define SIO_NO_UART
#endif

/* DSM payload type for Unescaped bytes for Async data */
#if defined(FEATURE_DATA_MM) || defined(FEATURE_GSM_PLT) || defined(FEATURE_WCDMA_PLT) || defined (FEATURE_CORE_SERVICE_ONLY) || defined(FEATURE_LTE_PLT)
#define SIO_PAYLOAD       DSM_PS_RAW_APP_PAYLOAD
#else 
#define SIO_PAYLOAD       PS_RAW_APP_PAYLOAD
#endif

#ifdef FEATURE_HTORPC_METACOMMENTS
  /*~ PARTIAL sio_ioctl_param */
  /*~ CASE SIO_IOCTL_CHANGE_BAUD_RATE sio_ioctl_param.rearm_autobaud */
  /*~ CASE SIO_IOCTL_CHANGE_BAUD_NOW sio_ioctl_param.rearm_autobaud */
  /*~ DEFAULT sio_ioctl_param.void */
#endif

#ifndef FEATURE_SMD
void smd_sio_flush_tx_ext
( 
  sio_port_id_type      port_id,
  sio_vpu_func_ptr_type flush_cb,
  void                  *cb_data
);

void smd_sio_close_ext
( 
  sio_port_id_type      port_id,
  sio_vpu_func_ptr_type close_cb,
  void                  *cb_data
);
#endif


/*===========================================================================

                        INIT FUNCTION DECLARATIONS

===========================================================================*/
#ifndef FEATURE_UART_WRAPPER_FREE
uint32 sio_uart_driver_init(sio_register_device_driver_cb_type register_driver_cb_fn);
#endif
#ifndef FEATURE_FSUSB_WRAPPER_FREE
uint32 sio_fsusb_driver_init(sio_register_device_driver_cb_type register_driver_cb_fn);
#endif
#ifndef FEATURE_HSUSB_WRAPPER_FREE
uint32 sio_hsusb_driver_init(sio_register_device_driver_cb_type register_driver_cb_fn);
#endif
#ifndef FEATURE_SMD_WRAPPER_FREE
uint32 sio_smd_driver_init(sio_register_device_driver_cb_type register_driver_cb_fn);
#endif
#ifndef FEATURE_APS_WRAPPER_FREE
uint32 sio_aps_driver_init(sio_register_device_driver_cb_type register_driver_cb_fn);
#endif


/*===========================================================================

FUNCTION sio_enter_crit_section 

@brief
  sio_enter_crit_section
      
@dependencies
  sio_init_crit_section() needs to be called first
      
@return
  None
    
@sideeffects
  None
  
===========================================================================*/
void sio_enter_crit_section(void);

/*===========================================================================

FUNCTION sio_leave_crit_section 

@brief
  sio_leave_crit_section
      
@dependencies
  sio_init_crit_section() needs to be called first
      
@return
  None
    
@sideeffects
  None
  
===========================================================================*/
void sio_leave_crit_section(void);

#endif  /* SIO_WRAPPER_H */
