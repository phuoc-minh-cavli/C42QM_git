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
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/core/inc/jusb_chp9_int.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================









************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _JUSB_CHP9_INT_H
#define _JUSB_CHP9_INT_H
#include "jusb_chp9.h"
#include <jusb_common_int.h>
struct usb_endpoint_descriptor {
    juint8_t  bLength;
    juint8_t  bDescriptorType;

    juint8_t  bEndpointAddress;
    juint8_t  bmAttributes;
    juint16_t wMaxPacketSize;
    juint8_t  bInterval;
#ifdef CONFIG_FD_AUDIO
    juint8_t  bRefresh;
    juint8_t  bSynchAddress;
#endif
};

/* SuperSpeed feature selectors, USB 3.0 spec table 9-6 */
#define USB_FUNCTION_SUSPEND            0
#define USB_U1_ENABLE                   0x30
#define USB_U2_ENABLE                   0x31
#define USB_LTM_ENABLE                  0x32

/* Encoding of the FUNCTION_SUSPEND options field */
#define HSU_FUNCTION_SUSPEND__IFACE_MASK         0xFF
#define HSU_FUNCTION_SUSPEND__IFACE_SHFT         0x00
#define USB_FUNCTION_SUSPEND_OPTIONS_SUSP_STATE  0x100
#define USB_FUNCTION_SUSPEND_OPTIONS_FUNC_WKP_EN 0x200

/* SuperSpeed descriptors, USB 3.0 spec table 9-5 */
#define USB_DT_BOS                      0x0f  /* BOS descriptor. */
#define USB_DT_DEVICE_CAPABILITY        0x10  /* DEVICE CAPABILITY descriptor. */
#define USB_DT_SUPERSPEED_ENDPOINT_COMPANION 0x30  /* SUPERSPEED ENDPOINT COMPANION descriptor. */
#define USB_DT_BOS_SIZE         5

/** USB_DT_BOS: BOS descriptor (see USB3.0 spec)
*/
struct usb_BOS_descriptor
{
  uint8  bLength;               /* Length of the following descriptor. */
  uint8  bDescriptorType;       /* BOS descriptor. */
  uint16 wTotalLength;          /* Total size of this descriptor and the 
                                   following descriptors. */
  uint8  bNumDeviceCaps;        /* Number of Device Capabilities descriptors 
                                   following. */
};

/* Two device capability descriptors: usb2_ext + ss_usb */
#define USB_BOS_DEVICE_CAPABILITY_NUM             2
/* Device capability type codes... USB 3.0 spec table 9-11 */
#define USB_DEVICE_CAPABILITY_TYPE_WIRELESS_USB   0x01
#define USB_DEVICE_CAPABILITY_TYPE_USB2_EXT       0x02
#define USB_DEVICE_CAPABILITY_TYPE_SUPERSPEED_USB 0x03
#define USB_DEVICE_CAPABILITY_TYPE_CONTAINER_ID   0x04

/* From Device capabilities usb2 extension bmAttributes */
#define USB_DEVCAP_USB2_EXIT_ATT_LPM              (1 << 1) /* must be set */

/* To pass USBCV 3.0's "TD 9.21: LPM L1 Suspend Resume Test", this bit needs
** to be set to 1 */
#define USB_DEVCAP_USB2_EXIT_ATT_BESL_AND_ALT_HIRD  (1 << 2)

#define USB_DT_DEVCAP_USB2_EXT_SIZE                7

/** USB_DEVICE_CAPABILITY_TYPE_USB2_EXT (see USB3.0 spec)
*/
PACKED struct usb_devcap_usb2_ext_descriptor
{
  uint8  bLength;               /* Length of the following descriptor. */
  uint8  bDescriptorType;       /* Device capabilities, USB2 extension 
                                   descriptor. */

  uint8  bDevCapabilityType;    /* Type of Device Capability descriptor */
  uint32 bmAttributes;          /* Attributes of this descriptor */
};

/* Device capabilities SuperSpeed capabilities */
#define USB_DEVCAP_SPEEDS_SUPPORTED               0xE // Full, High and Super Speeds
#define USB_DT_DEVCAP_SUPERSPEED_USB_SIZE  10
PACKED struct usb_devcap_superspeed_usb_descriptor
{
  uint8  bLength;
  uint8  bDescriptorType;

  uint8  bDevCapabilityType;
  uint8  bmAttributes;
  uint16 wSpeedsSupported;
  uint8  bFunctionalitySupported;
  uint8  bU1DevExitLat;
  uint16 bU2DevExitLat;
};

#define USB_SUPERSPEED_BULK_EP_MAXBURST  0x0
#define USB_SUPERSPEED_INTR_EP_MAXBURST  0x0
/* USB_DT_SUPERSPEED_ENDPOINT_COMPANION: SuperSpeed endpoint companion descriptor */
#define USB_DT_SUPERSPEED_ENDPOINT_COMPANION_SIZE  6
PACKED struct usb_superspeed_endpoint_companion_descriptor
{
  uint8  bLength;
  uint8  bDescriptorType;

  uint8  bMaxBurst;
  uint8  bmAttributes;
  uint16 wBytesPerInterval;
};

#endif

