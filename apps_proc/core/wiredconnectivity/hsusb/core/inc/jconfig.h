/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/core/inc/jconfig.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================*/


#ifndef JCONFIG_H
#define JCONFIG_H 

/* Features list header file. */
#include <customer.h>

/* Disable sync checks */
/* TODO: consider enabling checks (once the issue of giant mutex nesting
** is resolved).
** Note: Even if sync checks are enabled for the application processor build,
** they still must be disabled on RexNT, and also in ARMPRG and DLOAD.
**/
#define CONFIG_PORT_NO_GET_CONTEXT

/* Device config */
#define CONFIG_JSLAVE 1

/* For supporting OTG Device Electrical Tests */
#ifndef CONFIG_FULL_SPEED_ONLY
#define CONFIG_JSLAVE_TEST_MODE 1 
#endif /* ! CONFIG_FULL_SPEED_ONLY */

// #define CONFIG_TDI_4X 1

#if defined(FEATURE_HS_USB_SNPS_CORE) 
/* Enable the USB3.0 SNPS DCD */
#define CONFIG_SNPS_DCD 1
#endif /* FEATURE_HS_USB_SNPS_CORE */

#define CONFIG_TDI_4X_ITC 0x00

#ifdef FEATURE_HS_USB_TRANSCEIVER_DEVICE
#define CONFIG_JTRANSCEIVER 1
#endif /*FEATURE_HS_USB_TRANSCEIVER_DEVICE*/

/* Allow remote wakeup timeout in milliseconds (in case host did not issue resume) */
#define CONFIG_REMOTE_WAKEUP_TIMEOUT 2000

#ifdef FEATURE_HS_USB_OTG
/*High speed core config*/
#define CONFIG_EHCI       1

#endif  /* FEATURE_HS_USB_OTG */

#ifdef FEATURE_HS_USB_DIGITAL_AUDIO_DEVICE
#define CONFIG_FD_AUDIO
#endif /* FEATURE_HS_USB_DIGITAL_AUDIO_DEVICE */

#ifdef FEATURE_HS_USB_MS_FD
#define CONFIG_FD_MASS    1
#define CONFIG_SCSI_TRANSPARENT_COMMAND 1

#ifdef FEATURE_HS_USB_FAST_CSW
#define CONFIG_FAST_CSW 1
#endif

#endif /* FEATURE_HS_USB_MS_FD */

#define CONFIG_FD_CDC_ACM 1
#define CONFIG_IN_HW_QUEUE 1

#ifdef FEATURE_HS_USB_ECM
#define CONFIG_FD_CDC_ECM 1
#endif /* FEATURE_HS_USB_CDC_ECM */

#define CONFIG_FD_CDC 1
#define CONFIG_FD_CDC_OBEX 1

#ifdef FEATURE_PICTBRIDGE_HS

/* Sicd Function Driver */
#define CONFIG_FD_SICD 1

/* PictBridge FD */
#define CONFIG_PROTO 1
#define CONFIG_PROTO_PTP 1
#define CONFIG_PROTO_DPS 1
#define CONFIG_FD_DPS_APP 1
#define CONFIG_PKG 1
#define CONFIG_PKG_XML 1
#undef CONFIG_PROTO_PTP_VS_EXT
#undef CONFIG_PROTO_PTP_MTP
#undef CONFIG_PROTO_PTP_MTP_ENHANCED
#undef CONFIG_FD_MTP_APP
#undef CONFIG_FD_SICD_APP

#endif  /* FEATURE_PICTBRIDGE_HS */

/* OTG configuration */
#ifdef FEATURE_HS_USB_OTG
#define CONFIG_JOTG 1
#define CONFIG_JOTG_APP 1
#endif  /* FEATURE_HS_USB_OTG */

#define PTP_TRANSPORT_LITTLE_ENDIAN 1

/* Environment config */
#define CONFIG_BYTE_ORDER CPU_LITTLE_ENDIAN
#define CONFIG_ALIGN_CRITICAL 1

/* Environment config */
#define CONFIG_MEMPOOL 1
//#define CONFIG_MEMPOOL_DMABLE 1
#define CONFIG_POOL_DMA 1

#if defined(FEATURE_HS_USB_OTG)
  #define CONFIG_JUMASS
  #define CONFIG_JUMASS_ASYNC
  #define CONFIG_JHOST
  /* Embedded Host Electrical Tests Support */
  #define CONFIG_JHOST_ETEST
  #define CONFIG_JHOST_MAX_POWER 500
  #else
    #define CONFIG_TASK_SINGLE /* use a single task for the device stack */
#endif  /* FEATURE_HS_USB_OTG */

#if defined(FEATURE_HS_USB_OTG) 
  #define CONFIG_JHOST_HID
  #define CONFIG_JHOST_HID_GENERAL
#ifndef FEATURE_HS_USB_EXCLUDE_JHID 
  #define CONFIG_JHOST_HID_KEYBOARD
  #define CONFIG_JHOST_HID_MOUSE
  #define CONFIG_JHOST_HID_JOYSTICK
#endif /* !FEATURE_HS_USB_EXCLUDE_JHID */
  #define CONFIG_JHOST_HID_XBOX360_GAMEPAD
#endif  /* FEATURE_HS_USB_OTG */

/* To reduce code space usage, disable JDEBUG if memory optimization is on */
#ifdef FEATURE_HS_USB_MEM_OPTIMIZATION
#undef JDEBUG
#endif /* FEATURE_HS_USB_MEM_OPTIMIZATION */

#ifdef JDEBUG
  #ifdef FEATURE_HS_USB_ENABLE_DATA_PATH_LOGGING
  #define CONFIG_DATA_PATH_DEBUG
  #endif /* FEATURE_HS_USB_ENABLE_DATA_PATH_LOGGING */
#endif /* JDEBUG */

#endif /* JCONFIG_H */

