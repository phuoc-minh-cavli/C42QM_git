/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             S I O    W R A P P E R   F U N C T I O N S

       SIO wrapper functions for serving all devices that uses SIO

GENERAL DESCRIPTION
  This module contains the wrapper function used to access SIO server
  layer.  This layer is only use temporary.  Each port that wish to access
  the SIO layer should register their own function with SIO server, insteaded
  going though the wrapper functions.

NOTE DURING DEVELOPEMNT
    Each served device must have thses functions wrapped:
        HW_open
        HW_close
        HW_ioctl
        HW_flush_tx
        HW_disable_device
        HW_control_transmit
        HW_control_close
        HW_control_open



        open
        close
        ioctl
       flush_tx
        disable_device
        control_transmit
        control_close
        control_open

    Magic number:
        Value used to globally identify a device.  32 bit number divided into
        16 bit Major and 16 bit minor number

EXTERNALIZED FUNCTIONS

    sio_wrapper_init()
      Used to install all the function with the SIO layer.

SERVED FUNCTIONS


Copyright (c) 2007-2014,2016-2017,2019 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/buses/sio/src/amss/sio_wrapper.c#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

mm/dd/yy   who     what, where, why
--------   ---     ---------------------------------------------------------- 
5/24/19    ss      Memory optimisation on Threadx
03/08/18   rc      Added SIO_PORT_USB_SER6 port.
12/13/17   pr      Added USB_SER5 port.
03/21/17   rc      Added the USB_MDM2 PORT registration support
11/24/16   rc      Added the SIO_BITRATE_1000000
08/03/16   rc      Added the smd_init bacck due to absense of glink layer
07/10/16   rc      Ported the QURT specific API's for the OS based services
07/25/14   vv      Added DPL port.
08/19/13   vv      Added MHI driver.
03/08/12   tl      Added USB_CSVT port, QSAP device.
01/05/12   tl      Added USB_SER4 port.
06/30/11   amv     Added SPI driver.
01/10/11   amv     Added RMNET5 port.
08/27/09   amv     Added Mux driver initialization
07/31/09   amv     Added A2 driver initialization
06/32/09   amv     Added support for new smd bridge functions
06/24/09   amv     Removed static QMI_WWAN port mapping for new bridge
                   added control_ioctl for smd driver
06/23/09   amv     Mainline 8200 target specific changes
06/15/09   amv     Fixed BT featurization issue
09/12/08   amv     Added SMD ports for MODEM, APP and QDSP processor
05/19/08   rh      Add the include for aps_serial_vsp.h
05/05/08   rh      Fix a typo on disable_device for UART2
03/24/08   bfc     Added some new SMD DATA ports.
03/26/08   rh      Add entry for HSUSB ctrl_open for port NET_WWAN
02/29/08   rh      Wrapped calls to hsu_al_ecm_xx APIs with FEATURE_HS_USB_ECM
01/31/08   rh      Add sio_open and control_open to registerable function
09/26/07   rh      Adding MDD/PDD support for UART2 and UART3
07/30/07   rh      Remove entry for FSUSB on port SIO_PORT_USB_SER3
07/12/07   rh      Add new port - SIO_PORT_USB_SER3
06/13/07   rh      Add the use of MDD layer for UART1
01/11/07   rh      Created module.

===========================================================================*/

/*===========================================================================

                            INCLUDE FILES

===========================================================================*/
#include "target.h"                /* Target specific definitions          */
#include "comdef.h"                /* Definitions for byte, word, etc.     */
#include "sio.h"                   /* Serial I/O Services.                 */
#include "assert.h"
#include "sio_priv.h"
#include "sio_wrapper.h"
#include "qurt_mutex.h"

#ifdef FEATURE_A2
#include "a2_sio.h"
#endif

#ifndef SIO_NO_UART
#include "uart_init.h"
#endif

#ifdef FEATURE_SMUX
#include "smux_driver.h"
#endif

#ifdef FEATURE_USB
#include "siousb.h"                /* USB device driver layer.             */
#endif  /* FEATURE_USB */

#ifdef FEATURE_BT
#include "bt.h"
#include "btsio.h"                 /* BT Device Driver Layer.              */
#endif  /* FEATURE_BT */

#ifdef FEATURE_SMD
#include "smd_sio.h"
#endif  /* FEATURE_SMD */

#ifdef FEATURE_SMD_PORT_MGR
  #include "smd_port_mgr.h"
#endif

#if defined (FEATURE_SMD) && defined (FEATURE_SMD_BRIDGE)
#include "smd_bridge.h"

#ifndef FEATURE_SMD_BRIDGE_WRAPPER_FREE
#include "oncrpc.h"
#include "oncrpc_proxy.h"
#include "remote_apis.h"

#ifndef T_MDM8200
    #if defined(IMAGE_APPS_PROC)
      #define SMD_BRIDGE_CONNECT_FUNC     smd_bridge_connect_modem
      #define SMD_BRIDGE_DISCONNECT_FUNC  smd_bridge_disconnect_modem
    #else
      #define SMD_BRIDGE_CONNECT_FUNC     smd_bridge_connect_apps
      #define SMD_BRIDGE_DISCONNECT_FUNC  smd_bridge_disconnect_apps
    #endif /* IMAGE_APPS_PROC */
#else
    #if !defined(IMAGE_QDSP6_PROC)
      #define SMD_BRIDGE_CONNECT_FUNC     smd_bridge_connect_modem
      #define SMD_BRIDGE_DISCONNECT_FUNC  smd_bridge_disconnect_modem
    #else
      #define SMD_BRIDGE_CONNECT_FUNC     smd_bridge_connect_apps
      #define SMD_BRIDGE_DISCONNECT_FUNC  smd_bridge_disconnect_apps
    #endif /* IMAGE_QDSP6_PROC */
#endif
#endif /*FEATURE_SMD_BRIDGE_WRAPPER_FREE*/

#endif /* FEATURE_SMD && FEATURE_SMD_BRIDGE */

#ifndef FEATURE_HSUSB_WRAPPER_FREE
#ifdef SIO_USES_HS_USB
#include "hsu_al_ser.h"
#include "hsu_al_ecm.h"
#include "hsu_al_audio.h"
#endif
#else
#include "hsu_al_sio_reg.h"
#endif

#ifdef FEATURE_MMGPS_CSD
#include "lsm_comm_csd.h"
#endif

#ifdef FEATURE_APS
#include "aps_serial_vsp.h"
#endif

#ifdef FEATURE_RUNTIME_DEVMAP
#include "rdevmap.h"               /* Runtime Device Mapper                */
#endif

#ifdef FEATURE_DATA_MUX
#include "ds_mux_ext.h"            /* For Mux Driver                       */
#endif

#ifdef FEATURE_ONDEVICE_DIAG
#include "ondevice_diag.h"
#endif

#ifdef FEATURE_ONDEVICE_DIAG_AUTO
#include "ondevice_diag_sio_wrapper.h"
#endif

#ifdef FEATURE_SDIOC
#include "sdioc_sio.h"
#endif

#ifdef FEATURE_SPI_AL
#include "spi_al_sio.h"
#endif

#ifdef FEATURE_QSAP_FILTER
#include "qmi_sap_filter_target_ext.h"
#endif

#ifdef FEATURE_QBI
#include "qbi_diag_sio_api.h"
#endif

#ifdef FEATURE_MHI
#include "mhi_sio.h"
#endif

/* Critical section for SIO (AMSS) */
//rex_crit_sect_type sio_crit_sect;
qurt_mutex_t sio_crit_sect;



/*===========================================================================
  Extern: variables/Functions
===========================================================================*/

/* list of driver's init_func pointers */
extern sio_driver_init_func_type   sio_driver_init_list[SIO_MAX_DRIVERS];


#ifdef FEATURE_SMEM_DS
extern sio_status_type siosmem_open_stream
(
  sio_open_type *open_ptr                      /* Configurations of stream */
);

extern void siosmem_close_stream
(
  sio_stream_id_type stream_id,               /* Stream ID                 */
  void             (*close_func_ptr)(void)    /* Function to call when
                                                 transmission is complete. */
);

void siosmem_flush_tx
(
  sio_stream_id_type stream_id,             /* Stream ID                   */
  sio_port_id_type   port_id,               /* Port ID                     */
  void             (*flush_func_ptr)(void)  /* Function to call when
                                               pending transmission is
                                               complete                    */
);

extern void siosmem_dev_transmit
(
  sio_stream_id_type stream_id,    /* Stream ID                            */
  sio_port_id_type      port_id,            /* SIO Port ID              */
  dsm_item_type *tx_ptr            /* Packet which needs to be transmitted */
);

extern void siosmem_ioctl
(
  sio_stream_id_type    stream_id, /* Stream ID                            */
  sio_port_id_type      port_id,   /* SIO Port ID                          */
  sio_ioctl_cmd_type    cmd,       /* IOCTL command                        */
  sio_ioctl_param_type *param      /* Parameter to be used for command     */
);

extern void siosmem_disable_device
(
  void
);

#endif  /* FEATURE_SMEM_DS */


#ifndef FEATURE_FSUSB_WRAPPER_FREE
/*===========================================================================
  FSUSB_HELPER: Wrapper Functions
===========================================================================*/

#if !defined (SIO_USES_HS_USB) && defined (FEATURE_USB) && !defined(FEATURE_NO_USB_HW_ON_PROCESSOR)
void sio_usb_control_close_wrapper
(
    sio_stream_id_type stream_id,
    sio_port_id_type port_id,
    void             (*close_func_ptr)(void)
)
{
   sio_usb_control_close_stream( stream_id, close_func_ptr);
}

void sio_usb_control_transmit_wrapper
(
    sio_stream_id_type stream_id,
    sio_port_id_type port_id,
    dsm_item_type *tx_ptr
)
{
   sio_usb_control_transmit( stream_id, tx_ptr);
}
#endif


/*===========================================================================
  FSUSB_HELPER: Registration Functions
===========================================================================*/

void register_FSUSB_driver(sio_register_device_driver_cb_type register_driver_fn)
{
    sio_driver_interface driver_iface;
    sio_device_interafce device_iface;
    uint32 ret;

 #if !defined (SIO_USES_HS_USB) && defined (FEATURE_USB) && !defined(FEATURE_NO_USB_HW_ON_PROCESSOR)
    driver_iface.HW_open =             sio_usb_open_stream;
    driver_iface.HW_close =            sio_usb_close_stream; /* made compatible */
    driver_iface.HW_ioctl =            sio_usb_ioctl;
    driver_iface.HW_transmit =         sio_usb_dev_transmit;
    driver_iface.HW_flush_tx =         sio_usb_flush_tx;
    driver_iface.HW_disable_device =   sio_usb_disable_device;
    driver_iface.HW_control_open =     sio_usb_control_open_stream;
    driver_iface.HW_control_close =    sio_usb_control_close_wrapper; /* wrapper */
    driver_iface.HW_control_ioctl =    sio_usb_ioctl;                 /* seperated control_ioctl */
    driver_iface.HW_control_transmit = sio_usb_control_transmit_wrapper; /* wrapper */
 #else
     driver_iface.HW_open =             NULL;
     driver_iface.HW_close =            NULL;
     driver_iface.HW_ioctl =            NULL;
     driver_iface.HW_transmit =         NULL;
     driver_iface.HW_flush_tx =         NULL;
     driver_iface.HW_disable_device =   NULL;
     driver_iface.HW_control_open =     NULL;
     driver_iface.HW_control_close =    NULL;
     driver_iface.HW_control_ioctl =    NULL;
     driver_iface.HW_control_transmit = NULL;
 #endif

    device_iface.driver_interface = &driver_iface;

 /* USB_MDM */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_FSUSB_MDM);
 #if defined (FEATURE_USB_CDC_ACM) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

 /* USB_SER1/ DIAG */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_FSUSB_SER1);
 #if defined(SIO_USES_FSHS_USB) && \
   (defined( FEATURE_USB_DIAG) || defined(FEATURE_USB_DIAG_NMEA)) && \
   !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

 /* USB_MMC */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_FSUSB_MMC);
 #if defined(FEATURE_USB_PERIPHERAL_MASS_STORAGE) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

 /* USB_SER2 */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_FSUSB_SER2);
 #if defined (FEATURE_USB_DIAG_NMEA) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

 /* USB_RRDM */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_FSUSB_RRDM);
 #if defined (FEATURE_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

 /* NET_WWAN */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_HSUSB_ECM, SIO_MINOR_FSUSB_NET_WWAN);
 #if defined (SIO_USES_FSHS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) && \
    defined (FEATURE_HS_USB_ECM)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    #ifndef FEATURE_SMD_BRIDGE_WRAPPER_FREE
    device_iface.forward_port_id = SIO_PORT_SMD_WINMOB_QMI_WWAN;
    #else
    device_iface.forward_port_id = SIO_PORT_NULL;
    #endif
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

 /* RMNET2 */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_HSUSB_ECM, SIO_MINOR_FSUSB_RMNET2);
 #if defined (SIO_USES_FSHS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) && defined(FEATURE_HS_USB_ECM)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

 /* RMNET3 */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_HSUSB_ECM, SIO_MINOR_FSUSB_RMNET3);
 #if defined (SIO_USES_FSHS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) && defined(FEATURE_HS_USB_ECM)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);
}


/*===========================================================================
  FSUSB_HELPER: Init Function
===========================================================================*/

uint32 sio_fsusb_driver_init(sio_register_device_driver_cb_type register_driver_cb_fn)
{
    #if !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) && !defined(FEATURE_NO_SIO_USB_DEV_INIT)
    #if defined (FEATURE_USB_CDC_ACM) || \
        defined (FEATURE_USB_DIAG)    || \
        defined (FEATURE_USB_DIAG_NMEA) || \
        defined (FEATURE_USB_PERIPHERAL_MASS_STORAGE)
      /* Currently done in MC.*/
      sio_usb_dev_init();
    #endif /* FTE_USB_CDC_ACM or FT_USB_DIAG or FT_USB && FT_MMC */
    #endif /* !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) */

    /*----------------------------------------
    ----- OTHER INITIALIZATION IF ANY --------
    -------------- ADD HERE ------------------
    -----------------------------------------*/
    register_FSUSB_driver(register_driver_cb_fn);

    return 1;
}

#endif /* FEATURE_FSUSB_WRAPPER_FREE */


#ifndef FEATURE_HSUSB_WRAPPER_FREE
/*===========================================================================
  HSUSB_HELPER: Wrapper Functions
===========================================================================*/

#ifdef SIO_USES_HS_USB
#ifdef FEATURE_HS_USB_ECM
void hsu_al_ecm_control_close_wrapper
(
    sio_stream_id_type stream_id,
    sio_port_id_type port_id,
    void             (*close_func_ptr)(void)
)
{
   hsu_al_ecm_control_close(stream_id, close_func_ptr);
}

void hsu_al_ecm_control_transmit_wrapper
(
    sio_stream_id_type stream_id,
    sio_port_id_type port_id,
    dsm_item_type *tx_ptr
)
{
   hsu_al_ecm_control_transmit(stream_id, tx_ptr);
}
#endif /* FEATURE_HS_USB_ECM */
#endif /*SIO_USES_HS_USB*/

/*===========================================================================
  HSUSB_HELPER: Registration Functions
===========================================================================*/

void register_HSUSB_driver(sio_register_device_driver_cb_type register_driver_fn)
{
    sio_driver_interface driver_iface;
    sio_device_interafce device_iface;
    uint32 ret;

 #if defined (SIO_USES_HS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    driver_iface.HW_open =             hsu_al_ser_open;
    driver_iface.HW_close =            hsu_al_ser_close;  /* made compatible */
    driver_iface.HW_ioctl =            hsu_al_ser_ioctl;
    driver_iface.HW_transmit =         hsu_al_ser_transmit;
    driver_iface.HW_flush_tx =         hsu_al_ser_flush_tx;
    driver_iface.HW_disable_device =   NULL;
    driver_iface.HW_control_open =     NULL;
    driver_iface.HW_control_close =    NULL;
    driver_iface.HW_control_ioctl =    NULL;
    driver_iface.HW_control_transmit = NULL;
 #else
     driver_iface.HW_open =             NULL;
     driver_iface.HW_close =            NULL;
     driver_iface.HW_ioctl =            NULL;
     driver_iface.HW_transmit =         NULL;
     driver_iface.HW_flush_tx =         NULL;
     driver_iface.HW_disable_device =   NULL;
     driver_iface.HW_control_open =     NULL;
     driver_iface.HW_control_close =    NULL;
     driver_iface.HW_control_ioctl =    NULL;
     driver_iface.HW_control_transmit = NULL;
 #endif

    device_iface.driver_interface = &driver_iface;

 /* USB_MDM */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_HSUSB_MDM);
 #if defined (FEATURE_USB_CDC_ACM) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

 /* USB_SER1/ DIAG */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_HSUSB_SER1);
 #if defined(SIO_USES_FSHS_USB) && \
   (defined( FEATURE_USB_DIAG) || defined(FEATURE_USB_DIAG_NMEA)) && \
   !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_HSUSB_MMC);
 #if defined(FEATURE_USB_PERIPHERAL_MASS_STORAGE) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;  /* --HSUSB MMC not registered-- */
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

 /* USB_SER2 */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_HSUSB_SER2);
 #if defined (FEATURE_USB_DIAG_NMEA) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

 /* USB_SER3 */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_HSUSB_SER3);
 #if defined(SIO_HAS_USB_SER3) && defined(SIO_USES_HS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

 /* USB_SER4 */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_USB_SER4);
 #if defined(SIO_HAS_USB_SER4) && defined(SIO_USES_HS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

 /* USB_SER5 */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_USB_SER5);
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

 /* USB_SER6 */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_USB_SER6);
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);


 /* USB_QDSS */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_USB_QDSS);
 #ifdef FEATURE_HS_USB_QDSS_PORT
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret)


 #if defined(SIO_HAS_USB_QDSS) && defined(SIO_USES_HS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);


 /* USB_CSVT */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_USB_CSVT);
 #ifdef FEATURE_HS_USB_CSVT_PORT
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

 /* USB_SMCT */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_HSUSB_SMCT);
 #ifdef FEATURE_HS_USB_SMCT_PORT
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

/* USB_DCI */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_HSUSB_DCI);
#if defined (SIO_USES_HS_USB)
   device_iface.forward_port = FALSE;
   device_iface.forward_port_id = SIO_PORT_NULL;
   device_iface.driver_available = TRUE;
#else
   device_iface.forward_port = TRUE;
   device_iface.forward_port_id = SIO_PORT_NULL;
   device_iface.driver_available = FALSE;
#endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

 /* USB_RRDM */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_HSUSB_RRDM);
#ifdef FEATURE_NO_USB_HW_ON_PROCESSOR
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
#else
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    #endif
    device_iface.driver_available = FALSE;
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);
	
 /* USB_MDM2 */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_USB, SIO_MINOR_HSUSB_MDM2);
#if defined (FEATURE_USB_CDC_ACM) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
   device_iface.forward_port = FALSE;
   device_iface.forward_port_id = SIO_PORT_NULL;
   device_iface.driver_available = TRUE;
#else
   device_iface.forward_port = TRUE;
   device_iface.forward_port_id = SIO_PORT_NULL;
   device_iface.driver_available = FALSE;
#endif
   ret = register_driver_fn(&device_iface);
   ASSERT(ret);

	


    /* Register HS_USB ECM under new major number */

 #if defined (SIO_USES_FSHS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) && defined(FEATURE_HS_USB_ECM)
 #ifdef SIO_USES_HS_USB
    driver_iface.HW_open =             hsu_al_ecm_data_open;
    driver_iface.HW_close =            hsu_al_ecm_data_close;  /* made compatible */
    driver_iface.HW_ioctl =            hsu_al_ecm_ioctl;
    driver_iface.HW_transmit =         hsu_al_ecm_data_transmit;
    driver_iface.HW_flush_tx =         hsu_al_ecm_data_flush_tx;
    driver_iface.HW_disable_device =   NULL;
    driver_iface.HW_control_open =     hsu_al_ecm_control_open;
    driver_iface.HW_control_close =    hsu_al_ecm_control_close_wrapper; /* wrapper */
    driver_iface.HW_control_ioctl =    hsu_al_ecm_control_ioctl;
    driver_iface.HW_control_transmit = hsu_al_ecm_control_transmit_wrapper; /* wrapper */
 #else
     driver_iface.HW_open =             NULL;
     driver_iface.HW_close =            NULL;
     driver_iface.HW_ioctl =            NULL;
     driver_iface.HW_transmit =         NULL;
     driver_iface.HW_flush_tx =         NULL;
     driver_iface.HW_disable_device =   NULL;
     driver_iface.HW_control_open =     NULL;
     driver_iface.HW_control_close =    NULL;
     driver_iface.HW_control_ioctl =    NULL;
     driver_iface.HW_control_transmit = NULL;
 #endif
 #endif
    device_iface.driver_interface = &driver_iface;
     /* NET_WWAN */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_HSUSB_ECM, SIO_MINOR_HSUSB_ECM_NET_WWAN);
 #if defined (SIO_USES_HS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    #ifndef FEATURE_SMD_BRIDGE_WRAPPER_FREE
    device_iface.forward_port_id = SIO_PORT_SMD_WINMOB_QMI_WWAN;
    #else
    device_iface.forward_port_id = SIO_PORT_NULL;
    #endif
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

     /* RMNET2 */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_HSUSB_ECM, SIO_MINOR_HSUSB_RMNET2);
 #if defined (SIO_USES_HS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) && defined(FEATURE_HS_USB_ECM)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

 /* RMNET3 */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_HSUSB_ECM, SIO_MINOR_HSUSB_RMNET3);
 #if defined (SIO_USES_HS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) && defined(FEATURE_HS_USB_ECM)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

 /* RMNET4 */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_HSUSB_ECM, SIO_MINOR_HSUSB_RMNET4);
 #if defined (SIO_USES_HS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) && defined(FEATURE_HS_USB_ECM)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

 /* RMNET5 */
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_HSUSB_ECM, SIO_MINOR_HSUSB_RMNET5);
 #if defined (SIO_USES_HS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) && defined(FEATURE_HS_USB_ECM)
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;
 #else
    device_iface.forward_port = TRUE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = FALSE;
 #endif
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

/* RMNET6 */
   device_iface.device_id = DEVICE_ID(SIO_MAJOR_HSUSB_ECM, SIO_MINOR_USB_RMNET6);
#if defined (SIO_USES_HS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) && defined(FEATURE_HS_USB_ECM)
   device_iface.forward_port = FALSE;
   device_iface.forward_port_id = SIO_PORT_NULL;
   device_iface.driver_available = TRUE;
#else
   device_iface.forward_port = TRUE;
   device_iface.forward_port_id = SIO_PORT_NULL;
   device_iface.driver_available = FALSE;
#endif
   ret = register_driver_fn(&device_iface);
   ASSERT(ret);

/* RMNET7 */
  device_iface.device_id = DEVICE_ID(SIO_MAJOR_HSUSB_ECM, SIO_MINOR_USB_RMNET7);
#if defined (SIO_USES_HS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) && defined(FEATURE_HS_USB_ECM)
  device_iface.forward_port = FALSE;
  device_iface.forward_port_id = SIO_PORT_NULL;
  device_iface.driver_available = TRUE;
#else
  device_iface.forward_port = TRUE;
  device_iface.forward_port_id = SIO_PORT_NULL;
  device_iface.driver_available = FALSE;
#endif
  ret = register_driver_fn(&device_iface);
  ASSERT(ret);

/* RMNET8 */
   device_iface.device_id = DEVICE_ID(SIO_MAJOR_HSUSB_ECM, SIO_MINOR_USB_RMNET8);
#if defined (SIO_USES_HS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) && defined(FEATURE_HS_USB_ECM)
   device_iface.forward_port = FALSE;
   device_iface.forward_port_id = SIO_PORT_NULL;
   device_iface.driver_available = TRUE;
#else
   device_iface.forward_port = TRUE;
   device_iface.forward_port_id = SIO_PORT_NULL;
   device_iface.driver_available = FALSE;
#endif
   ret = register_driver_fn(&device_iface);
   ASSERT(ret);

/* IPCR */
   device_iface.device_id = DEVICE_ID(SIO_MAJOR_HSUSB_ECM, SIO_MINOR_HSUSB_IPCR);
#if defined (SIO_USES_HS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
   device_iface.forward_port = FALSE;
   device_iface.forward_port_id = SIO_PORT_NULL;
   device_iface.driver_available = TRUE;
#else
   device_iface.forward_port = TRUE;
   device_iface.forward_port_id = SIO_PORT_NULL;
   device_iface.driver_available = FALSE;
#endif
   ret = register_driver_fn(&device_iface);
   ASSERT(ret);

/* LWIP */
   device_iface.device_id = DEVICE_ID(SIO_MAJOR_HSUSB_ECM, SIO_MINOR_HSUSB_LWIP);
#if defined (SIO_USES_HS_USB)
   device_iface.forward_port = FALSE;
   device_iface.forward_port_id = SIO_PORT_NULL;
   device_iface.driver_available = TRUE;
#else
   device_iface.forward_port = TRUE;
   device_iface.forward_port_id = SIO_PORT_NULL;
   device_iface.driver_available = FALSE;
#endif
   ret = register_driver_fn(&device_iface);
   ASSERT(ret);

   /* DPL */
   device_iface.device_id = DEVICE_ID(SIO_MAJOR_HSUSB_ECM, SIO_MINOR_HSUSB_DPL);
#if defined (SIO_USES_HS_USB)
   device_iface.forward_port = FALSE;
   device_iface.forward_port_id = SIO_PORT_NULL;
   device_iface.driver_available = TRUE;
#else
   device_iface.forward_port = TRUE;
   device_iface.forward_port_id = SIO_PORT_NULL;
   device_iface.driver_available = FALSE;
#endif
   ret = register_driver_fn(&device_iface);
   ASSERT(ret);

}

/*===========================================================================
  HSUSB_HELPER: Init Function
===========================================================================*/

uint32 sio_hsusb_driver_init(sio_register_device_driver_cb_type register_driver_cb_fn)
{
    /*----------------------------------------
    ----- OTHER INITIALIZATION IF ANY --------
    -------------- ADD HERE ------------------
    -----------------------------------------*/

    register_HSUSB_driver(register_driver_cb_fn);
    return 1;
}


#endif /* FEATURE_HSUSB_WRAPPER_FREE */


#ifndef FEATURE_SMD_WRAPPER_FREE
/*===========================================================================
  SMD_HELPER: Wrapper Functions
===========================================================================*/

#ifdef FEATURE_SMD
void smd_sio_close_wrapper
(
    sio_stream_id_type stream_id,
    sio_port_id_type port_id,
    void             (*close_func_ptr)(void)
)
{
      smd_sio_close (port_id, close_func_ptr);
}

void smd_sio_control_close_wrapper
(
    sio_stream_id_type stream_id,
    sio_port_id_type port_id,
    void             (*close_func_ptr)(void)
)
{
    smd_sio_control_close (port_id, close_func_ptr);
}

void smd_sio_control_transmit_wrapper
(
    sio_stream_id_type stream_id,
    sio_port_id_type port_id,
    dsm_item_type *tx_ptr
)
{
    smd_sio_control_transmit (port_id, tx_ptr);
}

void smd_sio_dev_transmit_wrapper
(
  sio_stream_id_type stream_id,           /* Stream ID                     */
  sio_port_id_type  port_id,
  dsm_item_type     *tx_ptr               /* Packet for transmission       */
)
{
   smd_sio_dev_transmit(port_id, tx_ptr);
}

void smd_sio_flush_tx_wrapper
(
    sio_stream_id_type stream_id,           /* Stream ID                     */
    sio_port_id_type  port_id,
    void (*flush_func_ptr) (void)
)
{
    smd_sio_flush_tx(port_id, flush_func_ptr);
}

void smd_sio_ioctl_wrapper
(
    sio_stream_id_type stream_id,
    sio_port_id_type      port_id,
    sio_ioctl_cmd_type    cmd,
    sio_ioctl_param_type *param
)
{
    smd_sio_ioctl(port_id, cmd, param);
}

void smd_sio_disable_device_wrapper
(
    sio_port_id_type      port_id
)
{
    smd_sio_disable_device();
}
#else
void smd_sio_flush_tx_ext
(
  sio_port_id_type      port_id,
  sio_vpu_func_ptr_type flush_cb,
  void                  *cb_data
)
{
}

void smd_sio_close_ext
(
  sio_port_id_type      port_id,
  sio_vpu_func_ptr_type close_cb,
  void                  *cb_data
)
{
}
#endif /* FEATURE_SMD */


/*===========================================================================
  SMD_HELPER: Registration Functions
===========================================================================*/

void register_SMD_driver(sio_register_device_driver_cb_type register_driver_fn)
{
    sio_driver_interface driver_iface;
    sio_device_interafce device_iface;
    uint32 ret,i;

 #ifdef FEATURE_SMD
    driver_iface.HW_open =             smd_sio_open;
    driver_iface.HW_close =            smd_sio_close_wrapper;         /* wrapper */
    driver_iface.HW_ioctl =            smd_sio_ioctl_wrapper;         /* wrapper */
    driver_iface.HW_transmit =         smd_sio_dev_transmit_wrapper;  /* wrapper */
    driver_iface.HW_flush_tx =         smd_sio_flush_tx_wrapper;      /* wrapper */
    driver_iface.HW_disable_device =   smd_sio_disable_device_wrapper; /* wrapper */
    driver_iface.HW_control_open =     smd_sio_control_open;
    driver_iface.HW_control_close =    smd_sio_control_close_wrapper; /* wrapper */
    driver_iface.HW_control_ioctl =    smd_sio_ioctl_wrapper;         /* wrapper */
    driver_iface.HW_control_transmit = smd_sio_control_transmit_wrapper; /* wrapper */
 #else
     driver_iface.HW_open =             NULL;
     driver_iface.HW_close =            NULL;
     driver_iface.HW_ioctl =            NULL;
     driver_iface.HW_transmit =         NULL;
     driver_iface.HW_flush_tx =         NULL;
     driver_iface.HW_disable_device =   NULL;
     driver_iface.HW_control_open =     NULL;
     driver_iface.HW_control_close =    NULL;
     driver_iface.HW_control_ioctl =    NULL;
     driver_iface.HW_control_transmit = NULL;
 #endif

    device_iface.driver_interface = &driver_iface;
    device_iface.forward_port = FALSE;
    device_iface.forward_port_id = SIO_PORT_NULL;
    device_iface.driver_available = TRUE;

    for(i = SIO_MINOR_SMD_FIRST; i <= SIO_MINOR_SMD_MAX; i++){
        device_iface.device_id = DEVICE_ID(SIO_MAJOR_SMD, i);
        ret = register_driver_fn(&device_iface);
        ASSERT(ret);
    }

    /* SIO_PORT_SMD_DIAG_APPS */
    device_iface.device_id = SIO_PORT_SMD_DIAG_APPS;
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

    /* SIO_PORT_SMD_DIAG_MODEM */
    device_iface.device_id = SIO_PORT_SMD_DIAG_MODEM;
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);

}

/*===========================================================================
  SMD_HELPER: Init Function
===========================================================================*/


uint32 sio_smd_driver_init(sio_register_device_driver_cb_type register_driver_cb_fn)
{
 #ifdef FEATURE_SMD
  smd_sio_dev_init();
 #endif /* FEATURE_SMD */

    /*----------------------------------------
    ----- OTHER INITIALIZATION IF ANY --------
    -------------- ADD HERE ------------------
    -----------------------------------------*/
  register_SMD_driver(register_driver_cb_fn);
  return 1;
}
#endif /* FEATURE_SMD_WRAPPER_FREE */


#ifndef FEATURE_APS_WRAPPER_FREE

/*===========================================================================
  APS_HELPER: Registration Functions
===========================================================================*/

void register_APS_driver(sio_register_device_driver_cb_type register_driver_fn)
{
    sio_driver_interface driver_iface;
    sio_device_interafce device_iface;
    uint32 ret;

 #ifdef FEATURE_APS
    driver_iface.HW_open =             aps_serial_vsp_open_stream;
    driver_iface.HW_close =            aps_serial_vsp_close_stream;
    driver_iface.HW_ioctl =            aps_serial_vsp_ioctl;
    driver_iface.HW_transmit =         aps_serial_vsp_dev_transmit;
    driver_iface.HW_flush_tx =         aps_serial_vsp_flush_tx;
    driver_iface.HW_disable_device =   aps_serial_vsp_disable_device;
    driver_iface.HW_control_open =     NULL;
    driver_iface.HW_control_close =    NULL;
    driver_iface.HW_control_ioctl =    NULL;
    driver_iface.HW.control_transmit = NULL;
 #else
     driver_iface.HW_open =             NULL;
     driver_iface.HW_close =            NULL;
     driver_iface.HW_ioctl =            NULL;
     driver_iface.HW_transmit =         NULL;
     driver_iface.HW_flush_tx =         NULL;
     driver_iface.HW_disable_device =   NULL;
     driver_iface.HW_control_open =     NULL;
     driver_iface.HW_control_close =    NULL;
     driver_iface.HW_control_ioctl =    NULL;
     driver_iface.HW_control_transmit = NULL;
 #endif

 #ifdef FEATURE_APS
    device_iface.driver_available = TRUE;
 #else
    device_iface.driver_available = FALSE;
 #endif

    device_iface.driver_interface = &driver_iface;
    #ifdef IMAGE_APPS_PROC
        device_iface.forward_port = TRUE;
        device_iface.forward_port_id = SIO_PORT_NULL;
    #else
        device_iface.forward_port = FALSE;
        device_iface.forward_port_id = SIO_PORT_NULL;
    #endif
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_APS, SIO_MINOR_APS_DEV);
    ret = register_driver_fn(&device_iface);
    ASSERT(ret);
}

/*===========================================================================
  APS_HELPER: Init Function
===========================================================================*/

uint32 sio_aps_driver_init(sio_register_device_driver_cb_type register_driver_cb_fn)
{
    #ifdef FEATURE_APS
      aps_serial_vsp_dev_init();
    #endif

    /*----------------------------------------
    ----- OTHER INITIALIZATION IF ANY --------
    -------------- ADD HERE ------------------
    -----------------------------------------*/
    register_APS_driver(register_driver_cb_fn);
    return 1;
}


#endif /* FEATURE_APS_WRAPPER_FREE */

/*===========================================================================

FUNCTION sio_set_driver_init_list();                         INTERNAL FUNCTION

@brief
  set driver init functions (target specific featurized function)

@dependencies
  Should be called just once.

@return
  None

@sideeffects
  None

===========================================================================*/
void sio_set_driver_init_list(void)
{
  int index = 0;
  /*UART*/
  #if !defined(SIO_NO_UART)
    sio_driver_init_list[index++] = sio_uart_driver_init;
  #endif
  #ifdef FEATURE_SMUX
    sio_driver_init_list[index++] = smux_driver_sio_register;
  #endif
  /*FSUSB*/
  #if !defined(FEATURE_NO_FSUSB_INIT) && defined (FEATURE_FS_USB_ONLY)
    sio_driver_init_list[index++] = sio_fsusb_driver_init;
  #endif
  /*HSUSB*/
  #if !defined(FEATURE_NO_HSUSB_INIT) && !defined (FEATURE_FS_USB_ONLY)
    sio_driver_init_list[index++] = sio_hsusb_driver_init;
  #endif
  
  
  /*APS*/
  #if !defined(FEATURE_NO_APS_INIT)
    sio_driver_init_list[index++] = sio_aps_driver_init;
  #endif
  

  /*A2*/
  #ifdef FEATURE_A2
    sio_driver_init_list[index++] = a2_sio_driver_init;
  #endif

  /*MUX*/
  #ifdef FEATURE_DATA_MUX
    sio_driver_init_list[index++] = ds_mux_ext_sio_init;
  #endif

  
  /* SDIOC */
  #ifdef FEATURE_SDIOC
    sio_driver_init_list[index++] = register_sdioc_sio_driver;
  #endif

  

  /* QSAP FILTER */
  #ifdef FEATURE_QSAP_FILTER
    sio_driver_init_list[index++] = qmi_sap_sio_driver_init;
  #endif

  #if defined(FEATURE_QBI) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    sio_driver_init_list[index++] = qbi_diag_driver_init;
  #endif

  
  ASSERT(index <= SIO_MAX_DRIVERS);
  /* -- UPDATE THE SIO_MAX_DRIVERS AFTER ADDING DRIVER INIT HERE IN FUTURE -- */
}

/*===========================================================================

FUNCTION sio_init_rdm_sdm();                         INTERNAL FUNCTION

@brief
  Initialize rdm and sdm (not implemented for WM)

@dependencies
  Should be called just once.

@return
  None

@sideeffects
  None

===========================================================================*/

void sio_init_rdm_sdm()
{
  qurt_mutex_init(&sio_crit_sect);
  	
#ifndef T_MDM8200
#if defined (FEATURE_RUNTIME_DEVMAP) && (!defined (IMAGE_APPS_PROC) || defined (FEATURE_STANDALONE))
  rdm_init();
#endif
#else
#if defined (FEATURE_RUNTIME_DEVMAP) && ((defined (IMAGE_MODEM_PROC) && defined (IMAGE_QDSP6_PROC)) || defined (FEATURE_STANDALONE) || defined (FEATURE_STANDALONE_RDM) )
  rdm_init();
#endif
#endif
}


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
void sio_enter_crit_section()
{
    qurt_mutex_lock(&sio_crit_sect);
}

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
void sio_leave_crit_section()
{
    qurt_mutex_unlock(&sio_crit_sect);
}

/*===========================================================================

FUNCTION sio_is_uart_baud_valid();                         INTERNAL FUNCTION

@brief
  Check if baud is valid

@dependencies
  Should be called just once.

@return
  True: if valid
  False: otherwise

@sideeffects
  None

===========================================================================*/

boolean sio_is_uart_baud_valid(sio_bitrate_type check_baud)
{
   boolean return_val = FALSE;

 /* !!!! Must keep this in sync with sio.h:sio_bitrate_type def'n. !!!! */

  switch ( check_baud )
  {
#ifndef FEATURE_JCDMA_1X
    case SIO_BITRATE_300:
    case SIO_BITRATE_600:
    case SIO_BITRATE_1200:
    case SIO_BITRATE_2400:
    case SIO_BITRATE_4800:
    case SIO_BITRATE_9600:
    case SIO_BITRATE_19200:
    case SIO_BITRATE_38400:
    case SIO_BITRATE_57600:
#endif
    case SIO_BITRATE_115200:
    case SIO_BITRATE_230400:
    case SIO_BITRATE_460800:
#if defined(FEATURE_JCDMA_DS) || defined(FEATURE_JCDMA_SIO)
    case SIO_BITRATE_14400:
    case SIO_BITRATE_76800:
#endif
    case SIO_BITRATE_921600:
    case SIO_BITRATE_1000000:
    case SIO_BITRATE_2000000:
    case SIO_BITRATE_2900000:
    case SIO_BITRATE_3000000:
    case SIO_BITRATE_3200000:
    case SIO_BITRATE_3686400:
    case SIO_BITRATE_4000000:
#ifdef MSMU_MVR_HS_CUSTOM
    case SIO_BITRATE_HS_CUSTOM:
#endif
      return_val = TRUE;  /* baud rate is valid */
      break;

    default:
      /* The baud rate is not valid under the current
       * FEATURE set. We want to return FALSE.
       */
      break;
  }

  return ( return_val );
}


