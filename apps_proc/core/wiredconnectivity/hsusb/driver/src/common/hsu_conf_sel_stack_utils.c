/*==============================================================================

  High Speed USB Configuration Selector Stack Utilities Module
  
  GENERAL DESCRIPTION
    This module defines function that setup, stop, re-configure, etc. the USB 
    stack.
      
  EXTERNALIZED FUNCTIONS
    hsu_conf_sel_stack_utils_init_memory
    hsu_conf_sel_stack_utils_setup_device_stack
    hsu_conf_sel_stack_utils_switch_device_stack
    hsu_conf_sel_stack_utils_handle_chg_connected
    hsu_conf_sel_stack_utils_handle_chg_disconnected
    hsu_conf_sel_stack_utils_device_restart_core
    hsu_conf_sel_stack_utils_is_forced_disable
    hsu_conf_sel_stack_utils_force_disable_start
    hsu_conf_sel_stack_utils_force_disable_stop
    hsu_conf_sel_stack_utils_disable
    hsu_conf_sel_stack_utils_enable
    hsu_conf_sel_stack_utils_restart_core

 INITALIZATION AND SEQUENCING REQUIREMENTS
    hsu_conf_sel_stack_utils_init_memory should be called before any other 
    function, and before the CDC ALs are initialized. 

  Copyright (c) 2009, 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_conf_sel_stack_utils.c#1 $
  $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
  2009/01/13 04:17:02 $ $Author: pwbldsvc $
  
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  12/06/06  ke      Initial version
  04/22/07  me      Adding logic for setting the serial number
  06/29/07  bt      Added MTP (IUSBDriver) support  
  01/14/08  ab      Added HID FD support for fast charging
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "customer.h" /* for FEATURE_HSU_TEST to work */
#include "hsu_conf_sel_stack_utils_int.h"
#include "hsu_conf_sel_types_int.h"
#include "jtypes_int.h"
#include "jos_int.h"
#include "jslave_init.h"
#include "hsu_conf_sel_i.h"
#include "hsu_common_int.h"
#include "hsu_conf_sel_comp_utils.h"
#include "hsu_conf_sel_stack_utils.h"
#include "hsu_phy_utils_int.h"
#include "jusb_core_dcd.h"

#include "hsu_hwio_dependency.h"

#ifdef FEATURE_HS_USB_MS_FD
  #include "hsu_al_ms_int.h"
    #include "hsu_conf_sel_i.h" 
    #if defined(FEATURE_HS_USB_RAMDRIVE)
      #include "hsu_al_ms_ramdrive_block_device.h"
    #endif /* FEATURE_HS_USB_RAMDRIVE */
#endif
#include "jtypes_int.h"
#include "jos_int.h"
#include "jslave_init.h"
#include "jusb_common_int.h" /* for core_clear_reconfig_flag */
#include "jusb_core.h"

  #include "err.h"
  #include "hsu_mdm_apis.h"
  #include "hsu_conf_sel_nv.h"
  #include "hsu_conf_sel_types_int.h"
  #include <string.h>
  #include <stringl/stringl.h>

#include <hsu_os_abstract_bus.h>

#if defined(FEATURE_PMEM)
  #include "pmem.h"
  #endif
#include "hw.h" /* hw_partnum_version() */

#include "event.h"
#include "hsu_event_defs.h"


#ifdef T_UNDP
#include "undp_hsu_vid_pid.h"
#include "gobi_nv.h"
#endif /* T_UNDP */

#if defined(FEATURE_HS_USB_DIGITAL_AUDIO_LOOPBACK_H2D)
#include "hsu_audio_device_sample.h"
#endif /* FEATURE_HS_USB_DIGITAL_AUDIO_LOOPBACK_H2D */

#include "hsu_dev_app.h"

#include "hsu_al_task.h"

#ifdef FEATURE_HS_USB_SAMPLE_LOOPBACK
#include "hsu_fd_sample_loopback.h"
#endif /* FEATURE_HS_USB_SAMPLE_LOOPBACK */

#ifdef FEATURE_HS_USB_OTG
#include "hsu_otg_app.h"
#endif /*FEATURE_HS_USB_OTG*/

#include "hsu_vendor_specific.h"
#include "jtypes_int.h"

#include "HAL_hsusb.h"

#include "hsu_conf_sel_core_config.h"
#include "hsu_core_config_int.h"
#include "hsu_platform.h"

/* Due to a limitation in hsu_conf_sel_nv_esn_imei_read() - see more details in 
hsu_conf_sel_stack_utils_set_serial_number() */
#if defined (T_UNDP) && (defined(FEATURE_HS_USB_MS_FD) || defined(FEATURE_HS_USB_STANDARD_ECM))
#error "Gobi2k can't compile with FEATURE_HS_USB_MS_FD or FEATURE_HS_USB_STANDARD_ECM flags"
#endif /* T_UNDP && (FEATURE_HS_USB_MS_FD || FEATURE_HS_USB_STANDARD_ECM) */

#include "hsu_config_selector.h"
#include "hsu_al_ecm_int.h"

//#include "DALSys.h"
#include "hsu_boot_dload.h"
#include "hsu_log.h"
#include "hsu_clkmgr.h"
#include "hsu_al_mem.h"
/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/

/**************************************************
 * Definitions relevant for the Device Descriptor *
 **************************************************/
/* For the next defines, the reference was usbqcdev.h lines 58-60 */
#define HSU_DEVICE_CLASS_CODE              0x00
/* composite device class */
#ifdef FEATURE_HS_USB_STANDARD_ECM
#ifdef FEATURE_HS_USB_F3
  #define HSU_F3GOBI_COMM_DEVICE_CLASS_CODE   0x00
#endif /* FEATURE_HS_USB_F3 */
  #define HSU_COMM_DEVICE_CLASS_CODE         0x02
/* communication device class */
#endif /* FEATURE_HS_USB_STANDARD_ECM */
#define HSU_DEVICE_SUBCLASS_CODE           0x00
#define HSU_DEVICE_PROTOCOL_CODE           0x00
/* The reference was Jungo code, before the value was exposed */

#define HSU_DEVICE_BCD_USB                 0x0200
#define HSU_DEVICE_BCD_USB_EXT             0x0210

/* Taken from usbcdc.h lines 152-153 */
#define HSU_QC_VENDOR_ID_CODE              0x05C6

/* Used to provide a device version number, in the bcdDevice field. 
   This value is assigned by the developer. */
#ifdef T_UNDP
#define HSU_QC_DEVICE_REL_VER              0x0002
#else
#define HSU_QC_DEVICE_REL_VER              0x0000
#endif /* T_UNDP */

#ifndef HSU_MANUFACTURER_STRING
/* Taken from services\dload\dloadusb.c line 447 */
#define HSU_MANUFACTURER_STRING  "Qualcomm, Incorporated"
#endif /* HSU_MANUFACTURER_STRING */

#ifndef HSU_PRODUCT_STRING
/* Taken from services\dload\dloadusb.c line 447 */
#define HSU_PRODUCT_STRING       "Qualcomm CDMA Technologies MSM"
#endif /* HSU_PRODUCT_STRING */

#ifndef HSU_PRODUCT_UNDP_STRING
/* String for UNDP test mode to support third-party host driver */
#define HSU_PRODUCT_UNDP_STRING  "Gobi CDMA Tech"
#endif /* HSU_PRODUCT_UNDP_STRING */

#ifndef HSU_CONFIGURATION_STRING
/* String describing the configuration */
#define HSU_CONFIGURATION_STRING  "Qualcomm Configuration"
#endif /* HSU_CONFIGURATION_STRING */

/* this should be defined in hw.h but it's not there yet */
#define HW_MSM7600_REV12_VERSION_ID 0x2

/*==============================================================================
  Typedefs
==============================================================================*/

typedef struct {
  juint8_t core;
  device_params_t *new_config;
  reconfig_complete_cb_t cb;
  void *arg;
} hsu_jslave_reconfig_param_type;

/*==============================================================================
  Variables
==============================================================================*/
/*
 * Definitions related to the memory space used by the HS-USB stack 
 * (The size should be divisible by 32)
 */
#if defined(FEATURE_HS_USB_MEM_OPTIMIZATION)
  #define HSU_CFG_SELECTOR_SIZE_OF_STACK_MEM_POOL (24*1024)  /* 24 KB */
#else /* !FEATURE_HS_USB_MEM_OPTIMIZATION */

    #ifdef FEATURE_HS_USB_HOST_MS_PERF
      #define HSU_STACK_MEM_POOL_SIZE_PRIMARY (2048*1024) /* 2 MB */
    #elif defined(FEATURE_HS_USB_OBEX_PERF_TESTS)
      #define HSU_STACK_MEM_POOL_SIZE_PRIMARY (512*1024) /* 512 KB */
    #else
      /* normal Full UI build */
  #define HSU_STACK_MEM_POOL_SIZE_PRIMARY (64*1024)
  #endif

  #define HSU_STACK_MEM_POOL_ADDITION_OTG 0

  #define HSU_STACK_MEM_POOL_ADDITION_AUDIO_FD 0

  #define HSU_STACK_MEM_POOL_ADDITION_MS_FD 0
  #define HSU_STACK_MEM_POOL_ADDITION_RAMDRIVE_EBI 0

  #ifdef FEATURE_HSU_ECM_HOST_APP_STUB
  /* Allocating 15KB for 10 Ethernet frames */
  #define HSU_STACK_MEM_POOL_ADDITION_ECM_CD (15*1024) /* 15 KB */
  #else
  #define HSU_STACK_MEM_POOL_ADDITION_ECM_CD 0 
  #endif /* FEATURE_HSU_ECM_HOST_APP_STUB */

  #ifdef FEATURE_HS_USB_SAMPLE_LOOPBACK
    #define HSU_STACK_MEM_POOL_ADDITION_SAMPLE_LOOPBACK (16*1024)
  #else /* !FEATURE_HS_USB_SAMPLE_LOOPBACK */
    #define HSU_STACK_MEM_POOL_ADDITION_SAMPLE_LOOPBACK 0
  #endif /* FEATURE_HS_USB_SAMPLE_LOOPBACK */

  #ifdef FEATURE_HS_USB_DIGITAL_AUDIO_LOOPBACK_H2D
    #define HSU_STACK_MEM_POOL_ADDITION_AUDIO_LOOPBACK HSU_AUDIO_LOOPBACK_STACK_SIZE
  #else
    #define HSU_STACK_MEM_POOL_ADDITION_AUDIO_LOOPBACK 0
  #endif /* FEATURE_HS_USB_DIGITAL_AUDIO_LOOPBACK_H2D */
  
  #if defined(FEATURE_HS_USB_ECM)
    /* Additional memory required for supporting Transfer Descriptor
     * memory allocation for USB stack
     * 
     * Additional ECM Heap Memory = [malloc for 1 TD * 2] * [# of ECM instance] 
     *                              * ([tx max count] + [rx max count])
     * 
     * Note: 2 is multiplied to TD size to give room for HEAP fragmentation
     * For RX, assume max TD count per instance is 5.
     */
    #ifndef HS_USB_MEM_TD_MALLOC_SIZE
      #define HS_USB_MEM_TD_MALLOC_SIZE (120 * 2)
    #endif /* HS_USB_MEM_TD_MALLOC_SIZE */

    #define HSU_STACK_MEM_POOL_ADDITION_ECM_TD \
      ((HS_USB_MEM_TD_MALLOC_SIZE) * (HS_USB_NUM_OF_ECM_INST) \
       * ((HS_USB_ECM_TX_MAX_PENDING_BUFFERS) + 5))     
  #else  /* FEATURE_HS_USB_ECM */
    #define HSU_STACK_MEM_POOL_ADDITION_ECM_TD  (0)
  #endif /* FEATURE_HS_USB_ECM */

  #define HSU_CFG_SELECTOR_SIZE_OF_STACK_MEM_POOL \
            (HSU_STACK_MEM_POOL_SIZE_PRIMARY+     \
             HSU_STACK_MEM_POOL_ADDITION_OTG+     \
             HSU_STACK_MEM_POOL_ADDITION_MS_FD+   \
             HSU_STACK_MEM_POOL_ADDITION_SAMPLE_LOOPBACK+ \
             HSU_STACK_MEM_POOL_ADDITION_AUDIO_LOOPBACK+ \
             HSU_STACK_MEM_POOL_ADDITION_ECM_TD+ \
             HSU_STACK_MEM_POOL_ADDITION_AUDIO_FD+ \
             HSU_STACK_MEM_POOL_ADDITION_ECM_CD)
#endif 

  /* Book keeping how much uncached memory is allocated total */
  static uint32 hsu_conf_sel_alloc_uncached_memory_size = 0;


  typedef struct 
  {
      void *mem_vbuf;
      void *mem_pbuf;
      juint32_t mem_buf_size;
  } hsu_stack_meminfo_type;
  
  // For the VIRTUAL_POOL in JUNGO memory
  static hsu_stack_meminfo_type hsu_stack_meminfo = 
  {
    NULL,               /**< Memory buffer. */
    NULL,               /**< Memory buffer. */
    HSU_CFG_SELECTOR_SIZE_OF_STACK_MEM_POOL,      /**< Memory buffer size. */
  };

  /* Params used for checking if there are not two consecutive calls to 
  device configured (or two for not configured) */
  static boolean configured_cb_was_called_before = FALSE;
  static jbool_t dev_is_configured;

  static char hsu_conf_sel_stack_utils_serial_number[HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN] 
  			  = {HSU_CFG_SELECTOR_DEFAULT_SERIAL_NUMBER} ;

  #ifdef T_UNDP
    #define HSU_CONF_ALT_NET_DISABLED 0x00
    #define HSU_CONF_ALT_NET_ENABLED  0x01
    static UNDP_hsu_VID_PID_type hsu_dynamic_vid_pid;
  #endif /* T_UNDP */

static hsu_jslave_reconfig_param_type  jslave_reconfig_param;
static boolean hsu_jslave_reconfig_started = FALSE;

/* describes whether USB core is in forced_disable mode */
static boolean hsu_stack_is_forced_disable = FALSE;

static hsu_conf_sel_stack_utils_restart_core_cb_type hsu_restart_core_cb_array[NUMBER_OF_CORES] = {NULL};

/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/

#ifndef T_UNDP
static void hsu_jslave_reconfig(void * args);
#endif /* !T_UNDP */

#if defined (FEATURE_GOBI)
static void hsu_conf_sel_stack_utils_set_VID_PID(dev_info_t*  device_info);
#endif /* FEATURE_GOBI */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_INIT_MEMORY

DESCRIPTION
  This function initializes variables related to the memory given to the stack. 
  
DEPENDENCIES
  Called once, when the device stack is first setup, before ALs are initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
void hsu_conf_sel_stack_utils_init_memory(void)
{
  #ifndef USB_THREADX
  DALResult dal_result;
  DALSYSMemHandle mem_handle = NULL;
  DALSYSMemInfo memInfo;
  #else
  qurt_mem_region_attr_t  mem_attr;
  int                     mem_sts;
  qurt_mem_pool_t          pool;
  qurt_mem_region_t*     mem_handle = NULL;
  qurt_mem_region_t*     mem_phys = NULL;
  #endif
  uint32 size;
  
  /* The memory should be allocated from system memory on SMI/DDR. 
  This memory area MUST be non-cached */
  unsigned int dun_log_size = 0;

  // Adding additional space to log DUN payload when EFS item is set
  if(hsu_conf_sel_nv_efs_get(HSU_EFS_DUN_LOG_ENABLED))
  {
    dun_log_size = HSU_DUN_LOG_STACK_SIZE;
  }

  //size = (HSU_CFG_SELECTOR_SIZE_OF_STACK_MEM_POOL / 2) + dun_log_size;
  size = (HSU_CFG_SELECTOR_SIZE_OF_STACK_MEM_POOL) + dun_log_size;

#ifndef USB_THREADX
  dal_result = DALSYS_MemRegionAlloc(
    DALSYS_MEM_PROPS_PHYS_CONT | DALSYS_MEM_PROPS_UNCACHED, 
    DALSYS_MEM_ADDR_NOT_SPECIFIED, 
    DALSYS_MEM_ADDR_NOT_SPECIFIED,
    size + 32, // for alignment
    &mem_handle,
    NULL);

  if ( (dal_result != DAL_SUCCESS)
    || (mem_handle == NULL) 
    || (DAL_SUCCESS != DALSYS_MemInfo(mem_handle, &memInfo))
    || (memInfo.VirtualAddr == NULL)
    || (memInfo.PhysicalAddr == NULL))
  {
    HSU_ERR_FATAL("uncached mem alloc failed size (%d)", size, 0, 0);
  }
  hsu_stack_meminfo.mem_vbuf = HSU_ALIGN_PTR_TO_32_BYTE(memInfo.VirtualAddr);
  hsu_stack_meminfo.mem_pbuf = HSU_ALIGN_PTR_TO_32_BYTE(memInfo.PhysicalAddr);
    
 #else //USB_THREADX
  if (FALSE == usb_alloc_memory(size, (void**)&mem_handle, (void**)&mem_phys))
  {
    HSU_ERR_FATAL("uncached mem alloc failed size (%d)", size, 0, 0);
  }
  hsu_stack_meminfo.mem_vbuf = mem_handle;
  hsu_stack_meminfo.mem_pbuf = mem_phys; //TODO this is virtual address
 #endif //USB_THREADX
  
  hsu_conf_sel_alloc_uncached_memory_size += (size + 32); // for alignment
  hsu_stack_meminfo.mem_buf_size = size; // bytes used for alignment cannot be used by hsu_stack 
}/* hsu_conf_sel_stack_utils_init_memory */


/*==============================================================================

FUNCTION          HSU_CONF_SEL_ALLOC_UNCACHED_MEMORY

DESCRIPTION
  This function allocates physically contiguous uncached memory specified by
  the parameter size.
  
DEPENDENCIES
  None.

RETURN VALUE
  Memory pointer to uncached memory.

SIDE EFFECTS
  None.

==============================================================================*/
void* hsu_conf_sel_alloc_uncached_memory(uint32 size)
{
#ifndef USB_THREADX

  DALResult dal_result;
  DALSYSMemHandle mem_handle = NULL;
  DALSYSMemInfo memInfo;
  
  dal_result = DALSYS_MemRegionAlloc(
    DALSYS_MEM_PROPS_PHYS_CONT | DALSYS_MEM_PROPS_UNCACHED, 
    DALSYS_MEM_ADDR_NOT_SPECIFIED, 
    DALSYS_MEM_ADDR_NOT_SPECIFIED,
    size,
    &mem_handle,
    NULL);

  if ( (dal_result != DAL_SUCCESS)
    || (mem_handle == NULL) 
    || (DAL_SUCCESS != DALSYS_MemInfo(mem_handle, &memInfo))
    || (memInfo.VirtualAddr == NULL)
    || (memInfo.PhysicalAddr == NULL))
  {
    HSU_ERR_FATAL("uncached mem alloc failed size (%d)", size, 0, 0);
  }

#else
  qurt_mem_region_t*     mem_handle = NULL;
  void                   *dummy = NULL; // for compilation
  if (FALSE == usb_alloc_memory(size, (void**)&mem_handle, (void**)&dummy))
  {
    HSU_ERR_FATAL("uncached mem alloc failed size (%d)", size, 0, 0);
  }
#endif

  hsu_conf_sel_alloc_uncached_memory_size += size;
  #ifndef USB_THREADX
  return (void*)memInfo.VirtualAddr;
  #else
  return mem_handle;
  #endif
}


/*==============================================================================

FUNCTION          BUILD_DEVICE_INFO_STRUCT

DESCRIPTION
  This function initializes a structure to be given to jslave_enable(). 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
static void build_device_info_struct
(
  uint8         core_idx,
  dev_info_t *  device_info
#ifdef T_UNDP
  ,boolean	use_amss_product_id
#endif /* T_UNDP */
)
{
  hsu_core_config_type* core_config = NULL; 
  struct hsu_device_params_type* core_conf_sel_info = NULL;
#ifdef FEATURE_HS_USB_STANDARD_ECM
#endif /* FEATURE_HS_USB_STANDARD_ECM */

#if defined (FEATURE_HS_USB_STANDARD_ECM)
  const hsu_conf_sel_comp_info_type * comp_info = hsu_conf_sel_get_cur_comp_details();
#endif /* FEATURE_HS_USB_STANDARD_ECM */

  /* ***************************************************************************
  * Configuration Descriptor - 
  * For details about the content of the configuration descriptor, see section 
  * 9.6.3 and Table 9-10 in the USB 2.0 spec. 
  ****************************************************************************/
  /*
  * bmAttributes - 
  * Configuration characteristics, includes a Self-powered bit and a 
  * Remote Wakeup bit. 
  * Self-powered bit - A device configuration that uses power from the bus and a local source 
  * reports a non-zero value in bMaxPower to indicate the amount of bus power 
  * required and sets the Self-powered bit.
  * The actual power source at runtime may be determined using the GetStatus
  * DEVICE) request (see Section 9.4.5).
  * Remote Wakeup bit - set to 1 if a device configuration supports remote wakeup.
  */
  device_info->self_powered = 0;
  device_info->self_powered_func = NULL;

#if defined(FEATURE_HS_USB_OTG)
  /* 
   * Currently, The device doesn't support HNP & SRP 
   */
  device_info->srp_support = 0;
  device_info->hnp_support = 0;
#endif /* FEATURE_HS_USB_OTG */

  /* ***************************************************************************
  * Device Descriptor - 
  * For details about the content of the device descriptor, see section 9.6.1
  * and Table 9-8 in the USB 2.0 spec. 
  ****************************************************************************/
  /*
  * idVendor - 
  * Vendor ID (assigned by the USB-IF)
  */
  device_info->vendor = HSU_QC_VENDOR_ID_CODE;

  /*
  * idProduct - 
  * Product ID (assigned by the manufacturer)
  */ 
  core_config = hsu_core_config_get_core(core_idx);
  HSU_ASSERT((core_config != NULL) && (core_config->hsu_conf_sel_core_info != NULL));
  core_conf_sel_info = (struct hsu_device_params_type*)core_config->hsu_conf_sel_core_info;

  device_info->product = core_conf_sel_info->product_id;


  /* 
  * bcdDevice - 
  * Device version number in binary-coded decimal. Assigned by the developer. 
  */
  device_info->release = HSU_QC_DEVICE_REL_VER;
  /*
  * bDeviceClass -
  * zero means that each interface specifies its own class information, and that 
  * the various interfaces operate independently (composite device).
  * 0x02 indicates that the device is a communication device (some OSes like MAC OS
  * identify a device using this value).
  */
#if !defined (FEATURE_HS_USB_STANDARD_ECM)
  device_info->dev_class = HSU_DEVICE_CLASS_CODE; /* bDeviceClass */
#else
  if (comp_info->supported_drv_by_comp == HSU_STANDARD_CMP_ONLY)
  {
    device_info->dev_class = HSU_COMM_DEVICE_CLASS_CODE; /* bDeviceClass */
  }
#ifdef FEATURE_HS_USB_F3
  else if (comp_info->supported_drv_by_comp == HSU_GOBI_FUSION_CMP)
  {
    device_info->dev_class = HSU_F3GOBI_COMM_DEVICE_CLASS_CODE; /* bDeviceClass */
  }
#endif /* FEATURE_HS_USB_F3 */      
  else  
  {
    device_info->dev_class = HSU_DEVICE_CLASS_CODE; /* bDeviceClass */
  }    
#endif /* FEATURE_HS_USB_STANDARD_ECM */

  /*
  * bDeviceSubClass -
  * When the bDeviceClass field is zero, this field must also be set to zero.
  */
  device_info->dev_sub_class = HSU_DEVICE_SUBCLASS_CODE; 
  /* 
  * bDeviceProtocol - 
  * When this field is set to zero, the device does not use class-specific 
  * protocols on a device basis. However, it may use class specific protocols 
  * on an interface basis. 
  */
  device_info->dev_protocol = HSU_DEVICE_PROTOCOL_CODE; 

  /* String describing the product */
  device_info->product_name = HSU_PRODUCT_STRING;

  if(hsu_conf_sel_is_current_comp_undp())
  {
    /* Overwrite product string for special UNDP composition 
    ** for testing purpose
    */
    device_info->product_name = HSU_PRODUCT_UNDP_STRING;   
  }
  
  /* String describing the manufacturer */
  device_info->product_manufacturer = HSU_MANUFACTURER_STRING;
  /* String describing the configuration */
  device_info->configuration = HSU_CONFIGURATION_STRING;

#if defined(FEATURE_HS_USB_BREWMP_COMPOSITION)
  if (hsu_conf_sel_get_cur_comp_id() == HSU_CONF_SEL_BREWMP_COMP)
  {
    hsu_conf_sel_brewmp_device_data_type* 
      brewmp_info_ptr;

    dev_info_t*
      brewmp_device_info_ptr;

    brewmp_info_ptr      = hsu_conf_sel_get_bmp_device_info();
    brewmp_device_info_ptr = &(brewmp_info_ptr->device_info);

    /* Copy provided BrewMP device info. */
    if (brewmp_info_ptr->is_initialized)
    {
      *device_info = *brewmp_device_info_ptr;
    }
  }
#endif  /* FEATURE_HS_USB_BREWMP_COMPOSITION */

#ifdef T_UNDP
  /* For UNDP data card, we need to check if dynamic VID/PID data is 
  available.  If so, use VID, PID, product_name, and product_manufacturer
  data provided by UNDP.  Otherwise, keep what we have for all
  targets in general */

  /* clear the vid/pid data */
  memset(&hsu_dynamic_vid_pid, 0, sizeof(UNDP_hsu_VID_PID_type));

  /* update vid/pid data only if the API call goes through successfully */
  if ( UNDP_RET_OK == UNDP_get_hsu_VID_PID(&hsu_dynamic_vid_pid, use_amss_product_id)  )
  {
    /* Vendor ID (assigned by the USB-IF) */
    device_info->vendor = hsu_dynamic_vid_pid.VID;

    /* Product ID (assigned by the manufacturer) */
    device_info->product = hsu_dynamic_vid_pid.PID;

    /* String describing the product */
    hsu_dynamic_vid_pid.prodStr[MAX_HSU_VID_PID_STRING_DESCIPTOR_STR_LEN] = 0;
    device_info->product_name = hsu_dynamic_vid_pid.prodStr;
        
    /* String describing the manufacturer */
    hsu_dynamic_vid_pid.mfgStr[MAX_HSU_VID_PID_STRING_DESCIPTOR_STR_LEN] = 0;
    device_info->product_manufacturer = hsu_dynamic_vid_pid.mfgStr;
  }

    /* Keep the new product id/vendor ID on the modem */
    hsu_api_update_product_id(device_info->product);
    hsu_api_update_vendor_id(device_info->vendor);

#endif /* T_UNDP */

  /* Set the serial number */
  hsu_conf_sel_stack_utils_set_serial_number(device_info, 
                                             device_info->product, 
                                             core_idx);


  switch(core_idx)
  {
    case  HSU_CORE_CONFIG_USB3_IDX:
    {
#ifndef FEATURE_HS_USB_LINK_POWER_MANAGEMENT 
          device_info->bcdUSB = HSU_DEVICE_BCD_USB;
#else // FEATURE_HS_USB_LINK_POWER_MANAGEMENT
          device_info->bcdUSB = HSU_DEVICE_BCD_USB_EXT;
#endif // FEATURE_HS_USB_LINK_POWER_MANAGEMENT
    }
    break;

    default:
    {
      HSU_ERR_FATAL("build_device_info_struct: Invalid core_idx", core_idx, 0, 0);
    }
    break;
  }

  if (hsu_clkmgr_is_hs_only_mode())
  {
    // Since USB3 is not supported, program to USB 2.0 only
    device_info->bcdUSB = HSU_DEVICE_BCD_USB;
  }

} /* build_device_info_struct */


/*==============================================================================

FUNCTION          BUILD_FD_STRUCT

DESCRIPTION
  This function builds the fd_instance_t structure, that is given to stack bring
  up functions, based on the number of FDs array, which is part of the 
  composition details. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
static void build_fd_struct
(
  const uint8     num_of_fd_instances_in_comp[HSU_CFG_SEL_NUM_OF_FD_TYPES],
#ifdef T_UNDP
  hsu_conf_sel_composition_id_enum hsu_comp_id,
#endif /* T_UNDP */
  fd_instance_t * fds_to_init,
  uint8 *         num_of_fds
)
{
  uint8 fd_index = 0;

  HSU_ASSERT(fds_to_init != NULL);
  *num_of_fds = 0;

  /* 
  * IMPORTANT!!! 
  * OBEX must ALWAYS be the first FD to be enumerated. 
  * This is because DLOAD, before downloading a version to the phone, resets 
  * the phone and brings it up with only one OBEX FD instance (for DIAG), and 
  * the PC tries to connect to the phone on the same COM port as it was before 
  * the reset. 
  */

#ifdef T_UNDP
  /* 
   * Only in NET_DIAG_... composition (11, 12, ...), net should be the first in FD list
  */
  if (hsu_is_undp_net_first_comp_id (hsu_comp_id) == TRUE)
  {
    if(num_of_fd_instances_in_comp[HSU_CFG_SEL_ECM_FD] > 0)
    {
      fds_to_init[fd_index].type = DEVICE_TYPE_CDC_ECM;
      fds_to_init[fd_index].count = num_of_fd_instances_in_comp[HSU_CFG_SEL_ECM_FD];
      ++fd_index;
    }
  }
#endif /* T_UNDP */

  if(num_of_fd_instances_in_comp[HSU_CFG_SEL_OBEX_FD] > 0)
  {
    fds_to_init[fd_index].type = DEVICE_TYPE_CDC_OBEX;

#ifdef T_UNDP
    /* 
     * In case we separate the OBEX FDs, put the Diag as the first FD.
     * And then put remained OBEX FD (in this case NMEA) at the right position.
    */
    if ( (hsu_is_undp_nmea_comp_id (hsu_comp_id) == TRUE) &&
         (num_of_fd_instances_in_comp[HSU_CFG_SEL_OBEX_FD] >= 2)) 
    {
      fds_to_init[fd_index].count = 1;
    }
    else 
    {
      fds_to_init[fd_index].count = num_of_fd_instances_in_comp[HSU_CFG_SEL_OBEX_FD];
    }
#else /* !T_UNDP */
    fds_to_init[fd_index].count = num_of_fd_instances_in_comp[HSU_CFG_SEL_OBEX_FD];
#endif /* T_UNDP */

    ++fd_index;
  }

  if(num_of_fd_instances_in_comp[HSU_CFG_SEL_MS_FD] > 0)
  {
#ifndef FEATURE_HS_USB_MS_FD
    HSU_ERR_FATAL("build_fd_struct(): can't have MS FD instance when "
    "FEATURE_HS_USB_MS_FD is not defined!", 0, 0, 0);
#elif !defined (BUILD_HOSTDL)
    fds_to_init[fd_index].type = DEVICE_TYPE_MSD;
    fds_to_init[fd_index].count = hsu_al_ms_get_num_devices();
    ++fd_index;
#endif
  }

  if(num_of_fd_instances_in_comp[HSU_CFG_SEL_ACM_FD] > 0)
  {
    fds_to_init[fd_index].type = DEVICE_TYPE_CDC_ACM;
    fds_to_init[fd_index].count = num_of_fd_instances_in_comp[HSU_CFG_SEL_ACM_FD];
    ++fd_index;
  }

#ifdef T_UNDP
  /* 
   * Only if not NET_DIAG_... composition (11, 12, ...), work as previously
  */
  if (hsu_is_undp_net_first_comp_id (hsu_comp_id) == FALSE) 
  {
#endif /* T_UNDP */
  if(num_of_fd_instances_in_comp[HSU_CFG_SEL_ECM_FD] > 0)
  {
    fds_to_init[fd_index].type = DEVICE_TYPE_CDC_ECM;
    fds_to_init[fd_index].count = num_of_fd_instances_in_comp[HSU_CFG_SEL_ECM_FD];
    ++fd_index;
  }
#ifdef T_UNDP
  }
  if ((hsu_is_undp_nmea_comp_id (hsu_comp_id) == TRUE) &&
      (num_of_fd_instances_in_comp[HSU_CFG_SEL_OBEX_FD] >= 2)) /* In case we have more than 1 OBEX instances */
  {
    fds_to_init[fd_index].type = DEVICE_TYPE_CDC_OBEX;
    fds_to_init[fd_index].count = 1;
    ++fd_index;
  }
  /* 
  * 2nd modem port using OBEX interface 
  * This is only managed by the NV item setting as a flag to enable it or not.
  * Also it should be next to NMEA in static sequence
  */
  if (hsu_is_undp_ser3_port_enabled () == TRUE)
  {
    fds_to_init[fd_index].type = DEVICE_TYPE_CDC_OBEX;
    fds_to_init[fd_index].count = 1;
    ++fd_index;
  }
#endif /* T_UNDP */

  if(num_of_fd_instances_in_comp[HSU_CFG_SEL_SICD_FD] > 0)
  {
    fds_to_init[fd_index].type = DEVICE_TYPE_SICD;
    fds_to_init[fd_index].count = num_of_fd_instances_in_comp[HSU_CFG_SEL_SICD_FD];
    ++fd_index;
  }

#if defined(FEATURE_HS_USB_SAMPLE_LOOPBACK)
    if(num_of_fd_instances_in_comp[HSU_CFG_SEL_LOOPBACK_FD] > 0)
    {
      fds_to_init[fd_index].type = DEVICE_TYPE_VENDOR;
      fds_to_init[fd_index].count = 1;
      fds_to_init[fd_index].init_func = hsu_fd_sample_loopback_init; 
      ++fd_index;
    }
#endif /* FEATURE_HS_USB_SAMPLE_LOOPBACK */

#if defined (FEATURE_HS_USB_DIGITAL_AUDIO_DEVICE)
  if(num_of_fd_instances_in_comp[HSU_CFG_SEL_AUDIO_FD] > 0)
  {
    fds_to_init[fd_index].type = DEVICE_TYPE_AUDIO; /* required for up-init */
    fds_to_init[fd_index].count = num_of_fd_instances_in_comp[HSU_CFG_SEL_AUDIO_FD];
    ++fd_index;
  }
#endif /* FEATURE_HS_USB_DIGITAL_AUDIO_DEVICE */

  HSU_ASSERT(fd_index <= (uint8)HSU_CFG_SEL_NUM_OF_FD_TYPES);
  HSU_ASSERT(fd_index > 0);

  *num_of_fds = fd_index;
} /* build_fd_struct */

/*==============================================================================

FUNCTION          BUILD_CONFIG_PARAMS_STRUCT

DESCRIPTION
This function builds the config_param_t structure, that is given to stack bring
up functions, based on the number of the configurations number. 

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
static void build_config_params_struct
(
 uint8            core_idx,
 config_param_t * configs_to_init,
 uint8*           num_of_configs
 )
{
  uint8 num_of_fds = 0;
  uint8 config_idx;
  hsu_core_config_type* core_config = NULL;
  struct hsu_device_params_type* hsu_conf_sel_core_info;
  *num_of_configs = 0;

  HSU_ASSERT(configs_to_init != NULL);

  core_config = hsu_core_config_get_core(core_idx);
  HSU_ASSERT((core_config != NULL) && (core_config->hsu_conf_sel_core_info != NULL));

  hsu_conf_sel_core_info = (struct hsu_device_params_type*)core_config->hsu_conf_sel_core_info;

  for (config_idx = 0 ; config_idx < hsu_conf_sel_core_info->num_of_configs ; ++config_idx)
  {
    configs_to_init[config_idx].config_name = 
      hsu_conf_sel_core_info->configs_arr[config_idx].config_name;
    configs_to_init[config_idx].self_powered = 1;
#if defined (FEATURE_HS_USB_MS_FD)
    /* Canceling remote wakeup support on MS only composition, in order to pass
    USB-IF Compliance tests */
    if (hsu_conf_sel_get_cur_comp_id() == HSU_CONF_SEL_MS_COMP)
    {
      configs_to_init[config_idx].remote_wakeup = 0;
    }
    else
#endif /* !FEATURE_HS_USB_MS_FD */
    {
      configs_to_init[config_idx].remote_wakeup = 1;
    }
    /*
    * bMaxPower - 
    * Maximum power consumption of the USB device from the bus in this specific
    * configuration when the device is fully operational. Expressed in 2 mA units
    * (i.e., 50 = 100 mA).
    */
    configs_to_init[config_idx].max_power = HSU_DEVICE_MAX_POWER;

    *num_of_configs = hsu_conf_sel_core_info->num_of_configs;

    build_fd_struct(
      hsu_conf_sel_core_info->configs_arr[config_idx].num_of_fd_instances, 
#ifdef T_UNDP
      hsu_conf_sel_core_info->configs_arr[config_idx].cur_dev_comp_details->hsu_comp_id, 
#endif /* T_UNDP */
      configs_to_init[config_idx].fd_instances,
      &num_of_fds);

    configs_to_init[config_idx].num_of_fd_instances = num_of_fds;
  }
}

void dummy_std_cmd_cb(juint8_t bmRequestType, juint8_t bRequest, juint16_t wValue, juint16_t wIndex, juint16_t wLength)
{
  HSU_USE_PARAM(wIndex); /* to avoid warning */
  HSU_USE_PARAM(wLength);
  HSU_MSG_LOW_3("dummy_std_cmd_cb bmRequestType=%d bRequest=%d wValue=%d", bmRequestType, bRequest, wValue);
}

/*==============================================================================

FUNCTION          BUILD_DEVICE_PARAMS_STRUCT

DESCRIPTION
  This function builds the device_params_t structure given to stack bring-up 
  functions, based on the given fd_instance_t and product_id for the composition.

DEPENDENCIES
  None. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
static void build_device_params_struct
(
  uint8             core_idx,
  device_params_t * device_params_for_stack
#ifdef T_UNDP
  ,boolean 	use_amss_product_id
#endif /* T_UNDP */
)
{
  HSU_ASSERT(device_params_for_stack != NULL);

  build_device_info_struct(core_idx, 
                           device_params_for_stack->dev_info
#ifdef T_UNDP
	                   ,use_amss_product_id
#endif /* T_UNDP */
  );

  /* Set the config param data of this device controller */

  build_config_params_struct(
    core_idx,
    device_params_for_stack->config_param,
    &device_params_for_stack->num_of_configs);

  device_params_for_stack->vendor_get_string_cb = NULL;
  device_params_for_stack->vendor_cmd_in_cb = hsu_vendor_cmd_in_cb;
  device_params_for_stack->vendor_cmd_out_cb = NULL;
  device_params_for_stack->vendor_cmd_out_data_stage_cb = NULL;
  device_params_for_stack->standard_cmd_cb = dummy_std_cmd_cb;
  device_params_for_stack->reserved_cmd_in_cb = NULL;
  device_params_for_stack->reserved_cmd_out_cb = NULL;
  device_params_for_stack->reserved_cmd_out_data_stage_cb = NULL;


  /* stack will automatically assign interface numbers */
#ifdef T_UNDP
  device_params_for_stack->static_interface_number = 
                                         hsu_is_undp_using_static_interface ();
#else /* !T_UNDP */
  device_params_for_stack->static_interface_number = FALSE;
#endif /* T_UNDP */

} /* build_device_params_struct */

#ifndef FEATURE_HSU_TEST

/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_ENABLE_PERF_BOOST_IDX

DESCRIPTION
  This function configures various HW registers for throughput
  optimizations
  This function is called multiple times by each core that exist on the HW.
  To prevent redundant programming of the hardware, the logic must check core
    index first before any operation.

DEPENDENCIES
  The enabling of the various HW registers are dependent on HW version.

RETURN VALUE
  None

SIDE EFFECTS
  None.

==============================================================================*/
void hsu_conf_sel_stack_utils_enable_perf_boost_idx(uint8 core_index) /* logical core index */
{
#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  HSU_ULOG_ENUM(NO_MSG, BUS_LOG, "enter", HSU_CORE_OFFSET2, core_index);

  if (core_index == HS_USB_CORE_INDEX)
  {
    if (hsu_conf_sel_hs_main_core_enabled())
    {
      /* Commenting out the unnecessary writes as default HW setting takes care of it on Badger family */ 
      ///* turn off posted writes because of HW stability issues */
      //HAL_hsusb_SetPostedWrites(0,TRUE);

      ///* Enable TX_BUF_COLLISSION_MODE to enable 1-2 byte bug fix in HW */
      ///* TX_BUF_COLLISSION_MODE mode should be set to 1 on link design 2r10 and newer. */
      //HAL_hsusb_SetTxBufCollisionMode(HSU_CORE_CONFIG_USB_IDX, TRUE);

      HAL_hsusb_SetGrantStolen(HSU_CORE_CONFIG_USB_IDX, FALSE);
      HAL_hsusb_SetHprotMode(HSU_CORE_CONFIG_USB_IDX,HAL_HSUSB_HPROT_ALT);
      HAL_hsusb_SetAhbBurstMode(HSU_CORE_CONFIG_USB_IDX,HAL_HSUSB_AHB_BURST_INCR);
      HAL_hsusb_SetXtorBypass(HSU_CORE_CONFIG_USB_IDX, FALSE);
      HAL_hsusb_SetPeDpTxFifoIdleForce(HSU_CORE_CONFIG_USB_IDX, FALSE);
    }
  }
  /* else do nothing */
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE */
}
#endif /* #ifndef FEATURE_HSU_TEST*/

/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_SETUP_STACK

DESCRIPTION
  This function is called both by the config_selector (when FEATURE_HSU_TEST is 
  NOT defined) and by the ARMPROG/DLOAD Adaptation Layers.
  It sets-up the USB stack (by calling jstart_stack) based on the parameters 
  given. 

DEPENDENCIES
  Assumes all AL related initialization were already done. 
  Should be called only once after power up.

RETURN VALUE
  jresult_t - HSU_JSUCCESS for success, other values for failure (currently all 
  cause an ASSERT).

SIDE EFFECTS
  None.

==============================================================================*/
jresult_t hsu_conf_sel_stack_utils_setup_stack(
#ifdef T_UNDP
  boolean 	use_amss_product_id
#endif /* T_UNDP */
)
{
  device_params_t device_params_for_stack[HSU_NUM_OF_DEVICE_CONTROLLERS];
  dev_info_t      device_info[HSU_NUM_OF_DEVICE_CONTROLLERS];
  uw_hw_core_param_t      core_info[HSU_MAX_NUM_OF_CORES];
  config_param_t  config_params[HSU_NUM_OF_DEVICE_CONTROLLERS][HSU_MAX_NUM_OF_CONFIGS];
  fd_instance_t   fds_to_init[HSU_NUM_OF_DEVICE_CONTROLLERS][HSU_MAX_NUM_OF_CONFIGS][HSU_CFG_SEL_NUM_OF_FD_TYPES];
  uw_args_t       stack_init_args;
  uint32          device_controller_idx = 0;
  uint8           core_idx;
  uint8           cores_table_core_idx;
  uint32          config_idx;
  uint8           cores_table_size = 0;
  hsu_core_config_type* core_config = NULL;
  struct hsu_device_params_type* hsu_conf_sel_core_info = NULL;
  jresult_t       jresult;

  cores_table_size = hsu_core_config_get_cores_table_size();

  memset(&stack_init_args, 0, sizeof(stack_init_args));
  memset(&device_params_for_stack, 0, sizeof(device_params_for_stack));
  memset(&config_params, 0, sizeof(config_params));
  memset(&core_info, 0, sizeof(core_info));

  core_idx = 0;
  /* go over the cores table until we have all the active cores */
  for (cores_table_core_idx = 0; cores_table_core_idx < cores_table_size; ++cores_table_core_idx)
  {
    core_config = hsu_core_config_get_core(cores_table_core_idx);
    if (core_config == NULL)
    {
      continue; /* This core is irrelevant */
    }
 
    HSU_ASSERT(core_idx < HSU_MAX_NUM_OF_CORES); 
    core_info[core_idx].core_number = cores_table_core_idx;
  
    if (core_config->init_mode == HSU_CORE_CONFIG_INIT_MODE_HOST)
    {
      core_info[core_idx].init_opt = UW_INIT_HOST;
      ++core_idx;
      continue; /* Nothing else to assign for host core */
    }

    core_info[core_idx].device_param = &device_params_for_stack[device_controller_idx];
    if (core_config->init_mode == HSU_CORE_CONFIG_INIT_MODE_OTG)
    {
        core_info[core_idx].init_opt = UW_INIT_OTG;
        core_info[core_idx].device_param->auto_enable = FALSE;
#if defined(FEATURE_HS_USB_OTG)
        hsu_otg_app_set_core_index(cores_table_core_idx); /* @TODO - Multi config */
#endif /* defined(FEATURE_HS_USB_OTG) */
    }
    else
    {
      if (core_config->init_mode == HSU_CORE_CONFIG_INIT_MODE_DEVICE)
      {
        core_info[core_idx].init_opt = UW_INIT_DEVICE;
        core_info[core_idx].device_param->auto_enable = TRUE;
      }
      else
      {
        HSU_MSG_ERROR("_setup_stack(): called with undefined init option");
        return JEINVAL;
      }
    }

    hsu_conf_sel_core_info = (struct hsu_device_params_type*)core_config->hsu_conf_sel_core_info;

    /* We continue only if the core is OTG or DEVICE so it should include the device_params info */
    HSU_ASSERT(hsu_conf_sel_core_info != NULL);

    if (hsu_conf_sel_core_info->core_type == HSU_CONF_SEL_FULL_SPEED_CORE)
    {
        core_info[core_idx].device_param->full_speed_only = TRUE;
    }
    else
    {
        core_info[core_idx].device_param->full_speed_only = FALSE; 
    }

    /* Link the structure to all the structures it points to */
    device_params_for_stack[device_controller_idx].dev_info = &device_info[device_controller_idx];
    /* We want config_param to point to the first configuration of this device_controller_idx. */
    device_params_for_stack[device_controller_idx].config_param = &config_params[device_controller_idx][0];
    /* Go over all the configurations in this core */
    for (config_idx = 0; config_idx < hsu_conf_sel_core_info->num_of_configs; ++config_idx)
    {
      device_params_for_stack[device_controller_idx].config_param[config_idx].fd_instances = 
        &fds_to_init[device_controller_idx][config_idx][0];
    }

    /* Fill the structure */
    build_device_params_struct(
      cores_table_core_idx,
      &device_params_for_stack[device_controller_idx]
#ifdef T_UNDP
      , use_amss_product_id
#endif /* T_UNDP */
    ); 

    ++device_controller_idx;
    ++core_idx;
  }

  /* Keep the new serial number and product ID/vendor ID of the main core on the ARM9
     to be used later on in Software Download*/
  hsu_api_update_product_id(device_info[HSU_DEVICE_CONTROLLER_IDX_0].product);

#ifdef FEATURE_GOBI
  hsu_api_update_vendor_id(device_info[HSU_DEVICE_CONTROLLER_IDX_0].vendor);
#endif /* FEATURE_GOBI */

  if (device_info[HSU_DEVICE_CONTROLLER_IDX_0].product_serial != NULL)
  {
    hsu_api_update_serial_number(device_info[HSU_DEVICE_CONTROLLER_IDX_0].product_serial);
    hsu_api_update_is_serial_number_null(FALSE);
  }
  else
  {
    hsu_api_update_is_serial_number_null(TRUE);
  }
  
  // Since UNDP PID does not have the driver for BOOTLOADER USB,
  // do not pass UNDP PID/VID to BOOTLOADER USB and enumerate in default
  // composition.
  if (FALSE == hsu_conf_sel_is_current_comp_undp())
  {
    hsu_boot_dload_transition();
  }

  stack_init_args.mem_vbuf = hsu_stack_meminfo.mem_vbuf;
  stack_init_args.mem_pbuf = hsu_stack_meminfo.mem_pbuf;
  stack_init_args.mem_buf_size = hsu_stack_meminfo.mem_buf_size;
  stack_init_args.num_of_cores = core_idx;
  stack_init_args.hw_core_params = &core_info[0];

  /* 
   * The Giant mutex is locked in hsu_os_sync_init() which is called from 
   * hsu_os_init(), that's called from jstart_stack(). 
   * This is because only then, the giant is created. 
   * The rest of the code under jstart_stack runs with the giant locked, and 
   * we should release it when the function returns. 
   */
  jresult = jstart_stack(&stack_init_args);

  if(jresult != HSU_JSUCCESS)
  {
    ERR("*_setup_stack(): jstart_stack failed with %d",
      jresult,0,0);
    if (jresult == JOUTOFEPS)
    {
      HSU_ERR_FATAL("*_setup_stack(): jstart_stack failed - not enoughs "
        "endpoints to support the chosen composition", 0, 0, 0);
    }
    else
    {
      HSU_ERR_FATAL("*_setup_stack(): jstart_stack failed", 0, 0, 0);
    }
  }
  jsafe_leave();

  /* Notify the OS layer that the USB stack is initialized. */
  hsu_os_notify_stack_ready();


  return jresult;

} /* hsu_conf_sel_stack_utils_setup_stack */

/*==============================================================================

FUNCTION          HSU_JSLAVE_RECONFIG_CB

DESCRIPTION
  The function is a call back function which is passed as the 'cb' parameter for
  jslave_reconfig function. It will call the 'cb' passed through 
  hsu_jslave_reconfig.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

==============================================================================*/
void hsu_jslave_reconfig_cb(void *arg, jresult_t status)
{
  /* reset the reconfig state flag */
  hsu_jslave_reconfig_started = FALSE;

  /* call the callback function which has been passed in args of 
   *  hsu_jslave_reconfig. 
   */
  if(jslave_reconfig_param.cb)
  {
    jslave_reconfig_param.cb(arg,status);
  }
}

/*==============================================================================

FUNCTION          hsu_jslave_reconfig

DESCRIPTION
  It is a wrapper function for jslave_reconfig which runs in AL TASK context.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None.

==============================================================================*/
static void hsu_jslave_reconfig(void * args)
{
  hsu_jslave_reconfig_param_type *par = (hsu_jslave_reconfig_param_type *) args;

  /* set the reconfig state flag */
  hsu_jslave_reconfig_started = TRUE;

  /* replace 'cb' with internal 'cb' hsu_jslave_reconfig_cb.*/
  jslave_reconfig(par->core,
                  par->new_config,
                  hsu_jslave_reconfig_cb, 
                  par->arg);
}

/*==============================================================================

FUNCTION          HSU_CONF_SEL_IS_MS_INCLUDED_IN_CORE

DESCRIPTION
This function checks if MS is included in the current composition of 
the core configurations

DEPENDENCIES

RETURN VALUE
boolean

SIDE EFFECTS
None.

==============================================================================*/
static boolean hsu_conf_sel_is_ms_included_in_core(uint8 core_idx)
{
#ifdef FEATURE_HS_USB_MS_FD
  hsu_core_config_type* core_config = NULL;
  struct hsu_device_params_type* conf_sel_core_info = NULL;
  uint8 config_idx;

  core_config = hsu_core_config_get_core(core_idx);
  HSU_ASSERT((core_config != NULL) && (core_config->hsu_conf_sel_core_info != NULL));

  conf_sel_core_info = (struct hsu_device_params_type*)core_config->hsu_conf_sel_core_info;

  for (config_idx = 0; config_idx < conf_sel_core_info->num_of_configs; ++config_idx)
  {  
    if (conf_sel_core_info->configs_arr[config_idx].num_of_fd_instances[HSU_CFG_SEL_MS_FD] != 0)
    {
      HSU_MSG_LOW("hsu_conf_sel_is_ms_included_in_core, return TRUE");
      return TRUE;
    }
  }
  HSU_MSG_LOW("hsu_conf_sel_is_ms_included_in_core, return FALSE");
  return FALSE;
#else /*!FEATURE_HS_USB_MS_FD */
  HSU_MSG_LOW("hsu_conf_sel_is_ms_included_in_core, return FALSE");
  return FALSE;
#endif /*FEATURE_HS_USB_MS_FD */
}


#ifndef T_UNDP /* UNDP does not support UI/Pictbridge */
/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_SWITCH_DEVICE_STACK

DESCRIPTION
  This function re-configures the stack to include a new set of FDs, as 
  described by the num_of_fd_instances_in_comp parameter.
  This only affects the composition used for the first (and if multi-core is not
  enabled, the only) core.
  It registers reconfig_cb_func_ptr as the callback function to be invoked when 
  the stack re-configuration process is completed. 

  It should be called after all application using the old stack were stopped.

DEPENDENCIES
  Assumes that no application is using the stack at the time of the call. 
  Assumes all AL related initialization were already done. 

RETURN VALUE
  jresult_t - HSU_JSUCCESS for success, other values for failure (currently all 
  cause an ASSERT).

SIDE EFFECTS
  None.

==============================================================================*/
jresult_t hsu_conf_sel_stack_utils_switch_device_stack
(
 uint8                    core_idx,
  reconfig_complete_cb_t   reconfig_cb_func_ptr,
  void *        arg_for_reconfig_cb
)
{
  static device_params_t device_params_for_stack;
  static dev_info_t      device_info;
  static fd_instance_t   fds_to_init[HSU_MAX_NUM_OF_CONFIGS][HSU_CFG_SEL_NUM_OF_FD_TYPES];
  static config_param_t  config_params[HSU_MAX_NUM_OF_CONFIGS];
  hsu_core_config_type* core_config = NULL;
  uint32 config_idx;
  struct hsu_device_params_type* conf_sel_core_info = NULL;

  memset(&config_params, 0, sizeof(config_params));
  memset(&device_params_for_stack, 0, sizeof(device_params_for_stack));

  /* link the structure to all the structure it points to */
  device_params_for_stack.dev_info = &device_info;
  device_params_for_stack.config_param = &config_params[0];
  /* Go over all the configurations in this core */
  core_config =  hsu_core_config_get_core(core_idx);

  HSU_ASSERT((core_config != NULL) && (core_config->hsu_conf_sel_core_info != NULL));

  conf_sel_core_info = (struct hsu_device_params_type*)core_config->hsu_conf_sel_core_info;
  for (config_idx = 0; config_idx < conf_sel_core_info->num_of_configs; ++config_idx)
  {
    device_params_for_stack.config_param[config_idx].fd_instances = &fds_to_init[config_idx][0];
  }

  build_device_params_struct(
    core_idx,
    &device_params_for_stack
  );

  /* schedule jlave_reconfig on AL task. */
  jslave_reconfig_param.core = core_idx;
  jslave_reconfig_param.new_config = &device_params_for_stack;
  jslave_reconfig_param.cb = reconfig_cb_func_ptr;
  jslave_reconfig_param.arg = arg_for_reconfig_cb;
      
  hsu_al_task_enqueue_cmd(hsu_jslave_reconfig, &jslave_reconfig_param);

  hsu_conf_sel_stack_utils_enable_perf_boost_idx(hsu_hal_os_bus_core_id(core_idx));

  return HSU_JSUCCESS;
} /* hsu_conf_sel_stack_utils_switch_device_stack */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_RECONFIG_DONE

DESCRIPTION
This function is called when the stack re-configuration process is completed. 
This function updates the ARM9 product_id and serial_number to be used in SW 
download.

DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
void hsu_conf_sel_stack_utils_reconfig_done(hsu_conf_sel_comp_info_type* comp_info)
{

  boolean is_ms_included = FALSE;

  /* Keep the new serial number and product ID of the main core on the ARM9
  to be used later on in Software Download*/
  hsu_api_update_product_id(comp_info->product_id);

  /* @TODO - for now dynamic composition switch is applicable in primary core only.
     For supporting other cores, we also need to pass the core_idx to this function. */
  is_ms_included = hsu_conf_sel_is_ms_included_in_core(HSU_CORE_IDX_PRIMARY);

  /* When MS is not included in the composition - use NULL serial number */
  if (is_ms_included == FALSE)
  {
    hsu_api_update_is_serial_number_null(TRUE);
  }
  else
  {
    hsu_api_update_serial_number(hsu_conf_sel_stack_utils_serial_number);
    hsu_api_update_is_serial_number_null(FALSE);
  }
}

#endif /* !T_UNDP */


/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_DEVICE_STOP_CORE

DESCRIPTION       
Stops the core and it's PHY.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
static void hsu_conf_sel_stack_utils_device_stop_core(uint32 core_index)
{
  /* Stop the LPM timer in order to avoid LPM while reseting */
  hsu_os_bus_start_stop_lpm_timer(hsu_hal_os_bus_core_id(core_index), FALSE);

#if 0 // Not for SNPS core
#ifndef CI_MERGE
  // Use inclusion style of filter in case there are additional cores in the future
  if (core_index == HSU_CORE_CONFIG_USB3_IDX)
  {
    /* clear the OTG interrupt enable bits to avoid interrupts during reset */ 
    HAL_hsusb_ClearBits32(core_index, HAL_HSUSB_OTGSC_ADDR, 
      HAL_HSUSB_OTGSC_DPIE_BMSK
      | HAL_HSUSB_OTGSC_1MSE_BMSK
      | HAL_HSUSB_OTGSC_BSEIE_BMSK
      | HAL_HSUSB_OTGSC_BSVIE_BMSK
      | HAL_HSUSB_OTGSC_ASVIE_BMSK
      | HAL_HSUSB_OTGSC_AVVIE_BMSK
      | HAL_HSUSB_OTGSC_IDIE_BMSK
    );
  }
#endif  /*CI_MERGE*/
#endif

  hsu_os_core_hw_uninit(hsu_hal_os_bus_core_id(core_index));
}

/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_DEVICE_START_CORE

DESCRIPTION       
Starts the core and it's PHY.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
static void hsu_conf_sel_stack_utils_device_start_core(uint32 core_index)
{
  jresult_t jresult;
  hsu_core_config_type* core_config = NULL;

  /* NOTE:
  ** Disable AHB2AHB Bridge Bypass Mode before re-initializing the core,
  ** otherwise, the USB/HSIC core may not reset properly due to not meeting
  ** Universal Asynchronous Reset requirement of keeping all USB/HSIC clocks
  ** OFF during GCC RESET.
  */
  if (hsu_conf_sel_hs_main_core_enabled() && (core_index == HSU_CORE_CONFIG_USB_IDX))
  {
#ifdef FEATURE_HS_USB_SNPS_CORE
    if(hsu_conf_sel_is_usb_core_snps()) 
    {
      hsu_init_snps_usb();
    }
#endif /* FEATURE_HS_USB_SNPS_CORE */
  }

  jresult = hsu_os_core_hw_init(hsu_hal_os_bus_core_id(core_index));
  if(jresult != HSU_JSUCCESS)
  {
    HSU_ERR_FATAL("hsu_os_core_hw_init failed", 0, 0, 0);
  }

  /* In non-OTG mode, the DCD controller is not re-enabled when cable is   */
  /* connected. So this is needed in order to re-enable the DCD controller */
  core_config = hsu_core_config_get_core(core_index);
  HSU_ASSERT(core_config != NULL);

  if (core_config->init_mode == HSU_CORE_CONFIG_INIT_MODE_DEVICE)
  {
    jresult  = jslave_enable(core_index);
  }

  if (jresult != HSU_JSUCCESS)
  {
    HSU_ERR_FATAL("jslave_enable failed", 0, 0, 0);
  }

  if (core_index == HSU_CORE_CONFIG_USB_IDX)
  {
    if (hsu_port_is_attached(hsu_hal_os_bus_core_id(core_index)))
    {
      HSU_MSG_HIGH("Device is already connected, skip LPM timer");
    }
    else
    {
      /* Start the LPM timer - after core reset */
      hsu_os_bus_start_stop_lpm_timer(hsu_hal_os_bus_core_id(core_index), TRUE);
    }
  }
}


/*===========================================================================

FUNCTION hsu_conf_sel_stack_utils_force_disable_start

DESCRIPTION
This function disables the usb core, puts the USB PHY in LPM.

INPUT
cb - the callback for completion (of type 'hsu_conf_sel_stack_utils_force_disable_cb_type')

DEPENDENCIES
Run in the AL TASK command queue.

RETURN VALUE
TRUE/FALSE returned by the callback function ('cb' - input).

SIDE EFFECTS
===========================================================================*/
void hsu_conf_sel_stack_utils_force_disable_start(void* cb)
{
  hsu_conf_sel_stack_utils_force_disable_cb_type completion_cb = (hsu_conf_sel_stack_utils_force_disable_cb_type)cb;
  if (hsu_stack_is_forced_disable == TRUE)
  {
    HSU_MSG_HIGH("*_force_disable_start: already in forced_disable mode");
    completion_cb(FALSE);
	  return;
  }

  hsu_stack_is_forced_disable = TRUE;

  if (hsu_conf_sel_stack_utils_disable(HSU_CORE_IDX_PRIMARY) == FALSE)
  {
    completion_cb(FALSE);
    return;
  }

  hsu_os_bus_core_enter_lpm(hsu_hal_os_bus_core_id(HSU_CORE_IDX_PRIMARY), FALSE, FALSE);
  hsu_os_bus_interrupt_uninit(hsu_hal_os_bus_core_id(HSU_CORE_IDX_PRIMARY));

  completion_cb(TRUE);
  return;
}

/*==============================================================================

FUNCTION          hsu_conf_sel_stack_utils_force_disable_stop

DESCRIPTION
This function starts the USB core, activates the core and reconnects the PHY.

INPUT
cb - the callback for completion (of type 'hsu_conf_sel_stack_utils_force_disable_cb_type')

DEPENDENCIES
Run in the AL TASK command queue.

RETURN VALUE
TRUE/FALSE returned by the callback function ('cb' - input).

SIDE EFFECTS
None.

==============================================================================*/
void hsu_conf_sel_stack_utils_force_disable_stop(void* cb)
{
  hsu_conf_sel_stack_utils_force_disable_cb_type completion_cb = (hsu_conf_sel_stack_utils_force_disable_cb_type)cb;
  if (hsu_stack_is_forced_disable == FALSE)
  {
    HSU_MSG_HIGH("*_force_disable_stop: stack not in force_disable");
    completion_cb(FALSE);
    return;
  }
  hsu_stack_is_forced_disable = FALSE;
  hsu_conf_sel_stack_utils_enable(HSU_CORE_IDX_PRIMARY);
  hsu_os_bus_interrupt_init(hsu_hal_os_bus_core_id(HSU_CORE_IDX_PRIMARY));
  completion_cb(TRUE);
  return;
}

/*==============================================================================

FUNCTION          hsu_conf_sel_stack_utils_is_forced_disable

DESCRIPTION
returns whether the USB core is in forced_disable mode..

DEPENDENCIES
None.

RETURN VALUE
FALSE - disabled.
TRUE - enabled.

SIDE EFFECTS
None.

==============================================================================*/

boolean hsu_conf_sel_stack_utils_is_forced_disable(void)
{
  return hsu_stack_is_forced_disable;
}

/*==============================================================================

FUNCTION          hsu_conf_sel_stack_utils_disable

DESCRIPTION
   This function disable the USB core, called from "hsu_conf_sel_stack_force_disable_start" 
   and "hsu_analog_audio_start".
    
DEPENDENCIES
  None.

RETURN VALUE
  FALSE - if failed.
  TRUE - on successes

SIDE EFFECTS
  None.

==============================================================================*/
boolean      hsu_conf_sel_stack_utils_disable(uint32 core_index)
{
  HSU_MSG_HIGH_1("hsu_conf_sel_stack_utils_disable, %d - entered", core_index);

  /* If we are currently in LPM we need to exit it so we can work with the link.*/
  hsu_os_bus_core_exit_lpm(hsu_hal_os_bus_core_id(core_index));

  /* Stop the USB link. This will also put D+/D- signals into high-impedance mode */
  if (!hsu_phy_utils_spoof_disconnect_core(core_index))
  {
    HSU_MSG_ERROR("*_start: Couldn't stop USB link "
      "hsu_phy_utils_spoof_disconnect_core() returned false");
    return FALSE;
  }

  hsu_conf_sel_stack_utils_device_stop_core(core_index);

  #ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  
  /* HS OTG core specific actions */
  if (core_index == HSU_CORE_CONFIG_USB_IDX)
  {
  
    #ifdef FEATURE_HS_USB_OTG
    /* in HOST mode - shutdown vbus before core_hw_uninit*/
    if (hsu_otg_app_get_otg_mode() == OTG_DEVICE_MODE_HOST)
    {
      hsu_api_vbus_shutdown();
    }
    #endif /* FEATURE_HS_USB_OTG */
  
    hsu_api_disable_pmic_otg_comparators();
  }
  
  #endif /* FEATURE_HS_USB_NO_HS_OTG_CORE */



  HSU_MSG_HIGH("hsu_conf_sel_stack_utils_disable - exit");
  return TRUE;
}

/*==============================================================================

FUNCTION          hsu_conf_sel_stack_utils_enable

DESCRIPTION
   This function enable the USB core, called from "hsu_conf_sel_stack_force_disable_stop" 
   and "hsu_analog_audio_stop".
    
DEPENDENCIES
  None.

RETURN VALUE
  FALSE - if failed.
  TRUE - on successes

SIDE EFFECTS
  None.

==============================================================================*/
boolean hsu_conf_sel_stack_utils_enable(uint32 core_index)
{
  HSU_MSG_HIGH_1("hsu_conf_sel_stack_utils_enable, %d - entered", core_index);

  hsu_os_bus_core_exit_lpm(hsu_hal_os_bus_core_id(core_index));
  hsu_conf_sel_stack_utils_device_start_core(core_index);

  return TRUE;
}



/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_SET_SERIAL_NUMBER

DESCRIPTION
This function sets the serial number according to the composition and to the
configuration (AMSS / DLOAD / ARMPROG)

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_conf_sel_stack_utils_set_serial_number(  
  dev_info_t *  device_info,
  uint16        product_id,
  uint8         core_idx
)
{
#ifdef FEATURE_HS_USB_MS_FD
  static boolean read_esn_imei = TRUE;
  int serial_number_size ;
  int i;
#endif /* FEATURE_HS_USB_MS_FD */
  char return_buff_ptr[HSU_CFG_SELECTOR_MAX_ESN_IMEI_SIZE];
  boolean is_ms_included = FALSE;
  HSU_USE_PARAM(product_id); /* to avoid unused symbol warning */

#if defined (T_UNDP)
  HSU_USE_PARAM(return_buff_ptr);
#endif /* T_UNDP */

  /* 
  * String describing the device’s serial number. 
  * There are several issues to consider when choosing what value to use here:
  * - If a different string is used for each device (ESN, for example), then 
  *   the host driver would have to be installed for each device separately. 
  * - If the same non NULL serial # string is used for all devices, then 
  *   Windows crashes, if one device is connected to a PC port, and the other 
  *   through a USB HUB. 
  * - If NULL is used, the driver has to be installed for each port a device
  *   is connected to, but changing devices on the same port doesn't require 
  *   re-installation. The host communicates only with the device's first LUN, 
  *   and thus displays only a single drive instead of two.
  *
  *   In any case, MS FD spec requires the serial number to be at least 12 
  *   digits long
  */
  /*
  *   The solution to the above is:
  *
  *   If the composition includes MS - the serial number is the ESN / IMEI.
  *   In case that the ESN / IMEI is not available we use a constant "1234567890ABCDEF".
  *
  *   If the composition doesn't include MS - the serial number is null
  */

#ifdef FEATURE_HS_USB_MS_FD
  /* Read the ESN / IMEI in any case so that if we change the composition dynamically
  to a composition that includes MS we will have it */
  if (read_esn_imei == TRUE)
  {
    memset (hsu_conf_sel_stack_utils_serial_number, 0, HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN);
    if (FALSE == hsu_conf_sel_nv_esn_imei_read(return_buff_ptr, HSU_CFG_SELECTOR_MAX_ESN_IMEI_SIZE))
    {
      /* ESN/IMEI could not be retrieved, the serial number should be set to the default value
      (HSU_CFG_SELECTOR_DEFAULT_SERIAL_NUMBER)*/
      (void)strlcpy(hsu_conf_sel_stack_utils_serial_number, 
        HSU_CFG_SELECTOR_DEFAULT_SERIAL_NUMBER, 
        HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN);
    }
    else
    {
      /* Save the ESN/IMEI as the serial number */
      (void)strlcpy(hsu_conf_sel_stack_utils_serial_number, 
        return_buff_ptr, 
        HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN);
    }

    /* Pad the serial number with zero's in case it is less than 12 digits */
    serial_number_size = strlen(hsu_conf_sel_stack_utils_serial_number);
    if (serial_number_size < HSU_CFG_SELECTOR_SERIAL_NUMBER_MIN_LEN)
    {
      for (i = serial_number_size ; i < HSU_CFG_SELECTOR_SERIAL_NUMBER_MIN_LEN ; ++i)
      {
        hsu_conf_sel_stack_utils_serial_number[i] = '0';
      }
      hsu_conf_sel_stack_utils_serial_number[HSU_CFG_SELECTOR_SERIAL_NUMBER_MIN_LEN] = '\0';
    }
    read_esn_imei = FALSE;
  }
#elif !defined (T_UNDP)
  /* In Gobi2, if the card have been erased and don't have a valid ESN, IMEI is being read. However
  since it is stored in the secure EFS and at the time that USB stack is being initialized it isn't ready
  yet, we end up in a deadlock. */
  hsu_conf_sel_nv_esn_imei_read(return_buff_ptr, HSU_CFG_SELECTOR_MAX_ESN_IMEI_SIZE);
#endif /* FEATURE_HS_USB_MS_FD */

  is_ms_included = hsu_conf_sel_is_ms_included_in_core(core_idx);

  /* When MS is not included in the composition - use NULL serial number */
  if (is_ms_included == FALSE )
  {

	uint32 serial_num = 0x00;
	
	//Reading the chip serial number
	serial_num = HWIO_SERIAL_NUM_IN;
	snprintf(hsu_conf_sel_stack_utils_serial_number, HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN, "%lx", serial_num);
  
    //Update the serail for the Non MS case, otherwise BOOT will up update default serial 
    device_info->product_serial = hsu_conf_sel_stack_utils_serial_number;
  }
  else
  {
    device_info->product_serial = hsu_conf_sel_stack_utils_serial_number;
  }
  }

/*==============================================================================

FUNCTION          DEV_NOTIFY_CONFIGURED_CB

DESCRIPTION
App's implementation of the HS-USB stack's notification in case it enters/leaves
the DEVICE CONFIGURED state.

DEPENDENCIES
TODO

RETURN VALUE
None

SIDE EFFECTS
None.

==============================================================================*/
static void dev_notify_configured_cb
(
  appctx_t ctx, 
  jbool_t configured
)
{

  HSU_USE_PARAM(ctx);

  if (!configured_cb_was_called_before)
  {
    configured_cb_was_called_before = TRUE;
  }
  else /* configured_cb_was_called_before */
  {
    /* Note: It is allowed to have the device configured == TRUE twice in a row,
    for example USBCV invokes SET_CONFIGURATION(1) twice in a row.
    Getting FALSE twice in a row is also ok. */
    if (dev_is_configured == configured)
    {
      return;
    }
  }

  dev_is_configured = configured;

}

/*==============================================================================

FUNCTION          DEV_APP_INIT

DESCRIPTION
Called by the HS-USB stack in order to utilize app's callbacks, in case
interesting events (such as moving from/to DEVICE CONFIGURED state) occur.

DEPENDENCIES
TODO

RETURN VALUE
HSU_JSUCCESS - Always.

SIDE EFFECTS
None. 
==============================================================================*/
jresult_t dev_app_init(appctx_t ctx, app_callbacks_t *app_cbs)
{
  /* TODO: When more callbacks are needed, create a static variable to be passed.*/
  HSU_USE_PARAM(ctx);

  app_cbs->notify_device_configured = dev_notify_configured_cb;

  return HSU_JSUCCESS;
}


/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_DEVICE_RESTART_CORE

DESCRIPTION       
 This function is the SW workaround for a HW bug regarding the
 stuck PHY issue. 
 On each disconnection this function is called and restarts the 
 core and the PHY.

DEPENDENCIES
 None.

RETURN VALUE
 None.

SIDE EFFECTS
 None.

==============================================================================*/
void hsu_conf_sel_stack_utils_device_restart_core
(
  void* core_index_param
)
{
  /* logical core_index */
  uint32     core_index = (uint32)core_index_param;
  core_t    *core;

  HSU_MSG_HIGH("hsu_conf_sel_stack_utils_device_restart_core - entered");

  // Disallow enumeration before restart core occurs.
  core_disallow_enumeration(core_index);
  
  /* The device_restart_core stops jslave_reconfig in progress. If 
   * jlsave_reconfig was in progress, it should be rescheduled to complete
   * its task.
   */
  if( hsu_jslave_reconfig_started )
  {
    core_clear_reconfig_flag(jslave_reconfig_param.core);
    hsu_al_task_enqueue_cmd( hsu_jslave_reconfig, &jslave_reconfig_param);
  }

  jsafe_enter();

  HSU_ULOG_1(NO_MSG, BUS_LOG, "core_index %u", core_index);
  core = (core_t *)core_get_core_context(core_index);

  HSU_ASSERT(core != NULL);

  if (core->remote_wakeup_scheduled)
  {
    jtask_stop(core->remote_wakeup_task);
    core->remote_wakeup_scheduled = 0;
  }
  if (core->func_wake_tnotif_scheduled)
  {
    jtask_stop(core->func_wake_tnotif_task);
    core->func_wake_tnotif_scheduled = 0;
  }

  hsu_conf_sel_stack_utils_device_stop_core(hsu_os_bus_hal_core_id(core_index));
  hsu_conf_sel_stack_utils_device_start_core(hsu_os_bus_hal_core_id(core_index));

  jsafe_leave();

  hsu_conf_sel_stack_utils_restart_core_confirm_complete(core_index);

  HSU_MSG_HIGH("hsu_conf_sel_stack_utils_device_restart_core - exit");

  // Allow enumeration after restart core is completed.
  core_allow_enumeration(core_index);
}


#ifdef T_UNDP
/*==============================================================================

FUNCTION          hsu_get_undp_hsu_comp_id

DESCRIPTION
This function gets HSU composition ID info from UNDP.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS

==============================================================================*/

boolean hsu_get_undp_hsu_comp_id(hsu_composition_enum 
                                 * nv_composition_id)
{
  boolean ret = FALSE;
  uint16 composition;

  HSU_ASSERT(NULL != nv_composition_id);

  if (UNDP_RET_OK == UNDP_get_hsu_comp_id(&composition))
  {
    *nv_composition_id = (hsu_composition_enum) composition;
    ret = TRUE;
  }

  return (ret);
}

/*==============================================================================

FUNCTION          hsu_is_undp_net_first_comp_id

DESCRIPTION
This function checks if the given comp ID is Net first or not in UNDP

DEPENDENCIES
None.

RETURN VALUE
TRUE/FALSE

SIDE EFFECTS

==============================================================================*/

boolean hsu_is_undp_net_first_comp_id
(
  hsu_conf_sel_composition_id_enum composition_id
)
{
  boolean ret = FALSE;

  switch(composition_id)
  {
    case HSU_CONF_SEL_NET_DIAG_MDM_NMEA_COMP:
    case HSU_CONF_SEL_NET_DIAG_MDM_COMP:
    case HSU_CONF_SEL_NET_DIAG_NMEA_COMP:
    case HSU_CONF_SEL_NET_DIAG_COMP:
      ret = TRUE;
      break;

    default: /* No other values are legal for UNDP */
      break;
  }

  return (ret);
}
/*==============================================================================

FUNCTION          hsu_is_undp_nmea_comp_id

DESCRIPTION
This function checks if the given comp ID contains NMEA port or not in UNDP

DEPENDENCIES
None.

RETURN VALUE
TRUE/FALSE

SIDE EFFECTS

==============================================================================*/

boolean hsu_is_undp_nmea_comp_id
(
  hsu_conf_sel_composition_id_enum composition_id
)
{
  boolean ret = FALSE;

  switch(composition_id)
  {
    case HSU_CONF_SEL_NET_DIAG_MDM_NMEA_COMP:
    case HSU_CONF_SEL_NET_DIAG_NMEA_COMP:
      ret = TRUE;
      break;

    default: /* No other values are legal for UNDP */
      break;
  }

  return (ret);
}

/*==============================================================================

FUNCTION          hsu_is_undp_using_static_interface

DESCRIPTION
This function checks if the current USB port uses ststic interface or not in UNDP

DEPENDENCIES
None.

RETURN VALUE
TRUE/FALSE

SIDE EFFECTS

==============================================================================*/

boolean hsu_is_undp_using_static_interface ()
{
  boolean ret = FALSE;

  uint16 composition = 0;

  if ( UNDP_RET_OK == UNDP_get_hsu_comp_id(&composition))
  {
    /* UNDP_get_hsu_comp_id() returns one of the valid values of 
       hsu_composition_enum for UNDP */
    switch(composition)
    {
      case EFS_HSU_NET_DIAG_MDM_NMEA:
      case EFS_HSU_NET_DIAG_MDM:
      case EFS_HSU_NET_DIAG_NMEA:
      case EFS_HSU_NET_DIAG:
        ret = TRUE;
        break;

    default: /* No other values uses static interfaces for UNDP */
        break;
    }
  }

  return (ret);
} /* hsu_is_undp_using_static_interface */


/*==============================================================================

FUNCTION          hsu_is_undp_ser3_port_enabled

DESCRIPTION
This function checks if the current config support serial-3 port or not 
with both compositin number validity and NV ser port enabled flag setting.
Also, it should return FALSE if this function is called in DLOAD/QDL mode

DEPENDENCIES
None.

RETURN VALUE
TRUE/FALSE

SIDE EFFECTS
None.
==============================================================================*/

boolean hsu_is_undp_ser3_port_enabled ()
{
  boolean ret = FALSE;

  uint32 feat_flag;

  if (hsu_is_undp_using_static_interface () == TRUE) 
  {
    feat_flag = UNDP_get_hsu_feat_flag();
    if (feat_flag & UNDP_HSU_FEAT_FLAGS_SER3_ENABLED) 
    { 
      ret = TRUE;
    }
  }

  return (ret);
} /* hsu_is_undp_ser3_port_enabled */

/*==============================================================================

FUNCTION          hsu_is_undp_curr_comp_id_for_nmea

DESCRIPTION
This function checks if the current comp ID contains NMEA port or not in UNDP
Also, this function is not for DLOAD/QDL mode

DEPENDENCIES
None.

RETURN VALUE
TRUE/FALSE

SIDE EFFECTS

==============================================================================*/
boolean hsu_is_undp_curr_comp_id_for_nmea ()
{
  boolean ret = FALSE;

  uint16 composition = 0;

  if ( UNDP_RET_OK == UNDP_get_hsu_comp_id(&composition))
  {
    /* UNDP_get_hsu_comp_id() returns one of the valid values of 
       hsu_composition_enum for UNDP */
    switch(composition)
    {
      case EFS_HSU_NET_DIAG_MDM_NMEA:
      case EFS_HSU_NET_DIAG_NMEA:
        ret = TRUE;
        break;

      default: /* No other values are legal for UNDP */
        break;
    }
  }

  return (ret);
} /* hsu_is_undp_curr_comp_id_for_nmea */

/*==============================================================================

FUNCTION           HSU_IS_ALTERNATE_NET_ENABLED

DESCRIPTION
This function checks if the current config enabled alternate NET or not. 
By default, this feature should be set as DISABLED.
Also, it should return FALSE if this function is called in DLOAD/QDL mode

DEPENDENCIES
None.

RETURN VALUE
TRUE/FALSE

SIDE EFFECTS
None.
==============================================================================*/
boolean hsu_is_alternate_net_enabled ()
{
  boolean ret = FALSE;

  /* Currently Gobi specific NV item is used for temporarily.
  ** However, it should be changed to use a generic USB NV item 
  ** for all target teams as needed. 
  */


  uint8 alt_net_config = HSU_CONF_ALT_NET_DISABLED;

  if(gobi_nv_read (GOBI_NV_ITEM_HSU_ALT_NET_CONFIG, 0, &alt_net_config, sizeof(uint8)) == NV_DONE_S) 
  {
    if (alt_net_config & HSU_CONF_ALT_NET_ENABLED)
    {
      ret = TRUE;
    }
  }


  return (ret);
} /* hsu_is_alternate_net_enabled */
#endif /* T_UNDP */

/*==============================================================================

FUNCTION          hsu_is_dev_configured

DESCRIPTION
This function returns TRUE if the device is configured and FALSE otherwise

DEPENDENCIES
None.

RETURN VALUE
jbool_t - dev_is_configured

SIDE EFFECTS
None.
==============================================================================*/
jbool_t hsu_is_dev_configured(void)
{
  return dev_is_configured;
}

/*==============================================================================

FUNCTION          hsu_conf_sel_stack_utils_restart_core

DESCRIPTION
This function 

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
==============================================================================*/
void hsu_conf_sel_stack_utils_restart_core(uint32 logical_core_index,
                             hsu_conf_sel_stack_utils_restart_core_cb_type cb_func)
{
  HSU_ASSERT(logical_core_index < NUMBER_OF_CORES);
  if (cb_func != NULL)
  {
    /* store callback for use upon completion of restart core */
    hsu_restart_core_cb_array[logical_core_index] = cb_func;
  }

  hsu_os_bus_core_exit_lpm(logical_core_index);

  hsu_al_task_enqueue_cmd(hsu_conf_sel_stack_utils_device_restart_core, (void *)logical_core_index);

}

/*==============================================================================

FUNCTION          hsu_conf_sel_stack_utils_restart_core_confirm_complete

DESCRIPTION
This function invokes registered callbacks upon completion of restart core

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
==============================================================================*/
void hsu_conf_sel_stack_utils_restart_core_confirm_complete(uint32 logical_core_index)
{
  if (hsu_restart_core_cb_array[logical_core_index] != NULL)
  {
    hsu_restart_core_cb_array[logical_core_index](logical_core_index);
  }
}

