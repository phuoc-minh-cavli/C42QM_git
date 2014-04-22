#ifndef CUSTSIO_H
#define CUSTSIO_H

//--------------------------------------------------------------------
//  DESCRIPTION:  Configuration for SIO, RDM, and UART.
//  
//  Copyright (c) 2002 by Qualcomm Technologies Incorporated. All Rights Reserved.
//--------------------------------------------------------------------

#ifdef T_FFA6500
  #include "custffa.h"
#endif

//  Enable USB support
#ifdef FEATURE_BUILD_USB
  #define FEATURE_USB
#endif

//  Enable to open MMC port
#ifdef FEATURE_BUILD_MMC
  #define FEATURE_MMC
#endif

#define FEATURE_AHB_MSM_ABORT_ENABLED

#ifdef FEATURE_USE_UART1DM
  #define SIORS_USES_EXTERNAL_MND_CNT
  #define SIORS_HAVE_TXRDY_INTR
  #define SIORS_USES_LONG_UART_REG_NAME
  #define SIORS_USES_SEPERATE_CLK_REGIME
#endif

//  Enable UART clock to be TCXO not TCXO/4 
#ifndef FEATURE_MSM6500_UART_USE_TCXO_DIV_4
  #define FEATURE_UART_TCXO_CLK_FREQ         
#endif

//--------------------------------------------------------------------
//  Turn on power management features by default.
//--------------------------------------------------------------------

#define FEATURE_UART_POWERDOWN

//--------------------------------------------------------------------
//  Turn on UART trace buffer logging.  
//--------------------------------------------------------------------

#define FEATURE_SIO_DEBUG_TRACE

//--------------------------------------------------------------------
//  UART 1 - General purpose UART/Trace/Free
//--------------------------------------------------------------------
   
#define SIORS_FIRST_UART_PHYS_DEVICE        GSBI4_PHYS_DEVICE
#define SIORS_FIRST_UART_TYPE               HW_UARTDM_LT
#define SIORS_FIRST_UART_PD_WAKE_ON_RX      TRUE
#define SIORS_FIRST_UART_NEGATE_ARM_HALT    FALSE

//--------------------------------------------------------------------
//  UART 2 - General purpose UART
//--------------------------------------------------------------------
   
#define SIORS_SECOND_UART_PHYS_DEVICE       NO_PHYS_DEVICE
#define SIORS_SECOND_UART_TYPE              HW_NONE        
#define SIORS_SECOND_UART_PD_WAKE_ON_RX     FALSE
#define SIORS_SECOND_UART_NEGATE_ARM_HALT   FALSE

//--------------------------------------------------------------------
//  UART 3 - General purpose UART.
//--------------------------------------------------------------------

#define SIORS_THIRD_UART_PHYS_DEVICE        NO_PHYS_DEVICE
#define SIORS_THIRD_UART_TYPE               HW_NONE
#define SIORS_THIRD_UART_PD_WAKE_ON_RX      FALSE
#define SIORS_THIRD_UART_NEGATE_ARM_HALT    FALSE

//--------------------------------------------------------------------
//  Needed to pull in diagdsm.h and use single definition for DSM 
//  items.
//--------------------------------------------------------------------

#define SIORS_USES_DIAGDSM

//--------------------------------------------------------------------
//  The names of the UART IRQs change with each version of tramp.
//  The UART code now uses a UART define for the IRQ, rather than 
//  a tramp define.  Use SIORS_TRAMP_VERSION to set the UART define 
//  to the correct value for the tramp version being used.  
//  
//  Needed as long as tramp changes defines for IRQ names.  May be 
//  obsolete if HAL pic is used.
//--------------------------------------------------------------------

#define SIORS_TRAMP_VERSION                 TRAMP_VERSION_2_0

//--------------------------------------------------------------------
//  Other defines.  
//--------------------------------------------------------------------

//  Runtime Device Mapper services.  These are mutually exclusive
//  with FEATURE_SDEVMAP_UI_MENU & FEATURE_SDEVMAP_MENU_ITEM_NV.

#define FEATURE_RUNTIME_DEVMAP

//  Enables UI selection of port mapping. The port map is stored in
//  NV and retrieved - requires power cycle for changes to take effect.
//  These should be defined or undefined together.

#ifndef FEATURE_RUNTIME_DEVMAP
  #define FEATURE_SDEVMAP_UI_MENU
  #define FEATURE_SDEVMAP_MENU_ITEM_NV
#endif

#ifdef FEATURE_DS
  //  Enables UI selection of DS baud rate and the DS baud to be stored in
  //  and retrieved from NV.  These should be defined or undefined together.
  
  #define  FEATURE_DS_UI_BAUD
  #define  FEATURE_DS_DEFAULT_BITRATE_NV
#endif 

//  Enables use of DSM Items for DIAG

#define FEATURE_DSM_DIAG_ITEMS

//  Enables UI selection of DIAG baud rate and the DIAG baud to be stored in
//  and retrieved from NV.  These should be defined or undefined together.

#define FEATURE_DIAG_UI_BAUD
#define FEATURE_DIAG_DEFAULT_BITRATE_NV

//--------------------------------------------------------------------
//  FEATURE_USB
//--------------------------------------------------------------------

#ifdef FEATURE_USB //{

  //  Enables USB CDC ACM Port.  Must define
  //  FEATURE_NEW_SIO for this to be effective.
  
  #define FEATURE_USB_CDC_ACM

  //  Enable diag over USB
  
  #define FEATURE_USB_DIAG
  
  #ifdef FEATURE_USB_OTG
    #define FEATURE_USB_OTG_XCVR
  #else
    #define FEATURE_USB_FIVE_WIRE_XCVR
  #endif 

  #ifdef FEATURE_RUNTIME_DEVMAP
    //  Map DS to USB as the default configuration
    
    #define FEATURE_RDEVMAP_DS_DEFAULT_TO_USB

    //  When defined, allows DS to be mappable to UART2.  However, since
    //  UART2 does not have a full set of RS232 signals, not all DS services
    //  are possible on UART2.
    
    #undef FEATURE_DS_MAPPABLE_TO_UART2

  #endif

  #define FEATURE_USB_USE_DMOV
  
 #ifdef T_RUMI
    #define USB_PHILIPS_XCVR
 #elif defined(T_SURF)
    #define FEATURE_USB_READ_XCVR_SELECT_SWITCH
 #endif 

#endif //} 

//--------------------------------------------------------------------
//  FEATURE_BUILD_USB
//--------------------------------------------------------------------

#ifdef FEATURE_BUILD_USB //{
    #ifdef T_SURF
      #define FEATURE_USB2_HOST_PORT_IS_UNUSED   // unused host transceiver on usb2 port 
      #define FEATURE_USB3_HOST_PORT_IS_UNUSED   // unused host transceiver on usb3 port
    #elif defined (T_FFA)
      #define FEATURE_USB2_HOST_PORT_IS_UNUSED   // unused host transceiver on usb2 port 
    #endif   

    #if 0
      #define FEATURE_USB3_HOST_PORT  // host transceiver on usb3 port 
    #endif
    
    #if defined(FEATURE_USB2_HOST_PORT) || defined (FEATURE_USB3_HOST_PORT)
      #define USB_PHILIPS_XCVR_ON_HOST_PORTS
      #define FEATURE_USB_HOST_MULTIPORT
    #endif 

   #define FEATURE_USB
   #define FEATURE_USB_HOST
   #define FEATURE_USB_HOST_ENHANCED
   #define FEATURE_USB_HOST_HID
   #define FEATURE_PMIC_USB_OTG
   #define FEATURE_USB_HOST_ACQUIRE_VBUS
   #if !defined(FEATURE_WCDMA_PLT) && \
       !defined(FEATURE_GSM_PLT) && \
       !defined(FEATURE_THIN_UI) && \
       !defined(IMAGE_MODEM_PROC)
     #define FEATURE_USB_HOST_MASS_STORAGE
     #define FEATURE_USB_HOST_TEST_ENHANCED
     #define FEATURE_USB_HOST_HID_TEST_ENHANCED
   #endif 
#endif //}

//--------------------------------------------------------------------
//  RDEVMAP features.
//--------------------------------------------------------------------

// Don't define default service for diag.  Let it 
// be set to either UART or USB by .builds file 
// featurization.
//
// #define FEATURE_RDEVMAP_DIAG_DEFAULT_TO_USB
// #define RDM_USB_SER1_INIT_SRVC    RDM_DIAG_SRVC
// #define RDM_UART1_INIT_SRVC       RDM_DIAG_SRVC

#define RDM_USB_MDM_INIT_SRVC     RDM_DATA_SRVC

#define RDM_SMD_DS_INIT_SRVC      RDM_DATA_SRVC
#define RDM_SMD_DATA_INIT_SRVC    RDM_DATA_SRVC

#define FEATURE_RDM_3_0
#define FEATURE_RDM_NO_MISC_API

#define RDM_NO_WWAN_SRVC_MAPPING

//  To enable RDM server on all proc: On 8960 target only
#define RDM_SERVER_ON_ALL_PROC

//--------------------------------------------------------------------
//  Obsolete.  Should be removed but may still be used.
//--------------------------------------------------------------------

#define FEATURE_USB_CDC_ACM
#define FEATURE_NEW_SIO
//#define FEATURE_SERIAL_DEVICE_MAPPER
#define FEATURE_UART_WRAPPER_FREE
#define FEATURE_FSUSB_WRAPPER_FREE
#define SIORS_USES_MDDPDD_DRIVER

//  The next two are related to obsolete features using 7 wire UART.

//  Inverts the polarity of DTR line.
//  Fixes uart disconnect problem on cougar.
#define FEATURE_SIO_DTR_ACTIVE_LOW

//  Enables a nested GPIO interrupt to be used for DTR.  This is required for
//  MSM5105 since all GPIO interrupts are nested
#define FEATURE_SIO_DTR_HIGH_GPIO

#endif 

