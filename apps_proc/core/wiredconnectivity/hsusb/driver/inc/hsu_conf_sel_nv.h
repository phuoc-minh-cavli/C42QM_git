#ifndef _HSU_CONF_SEL_NV_H_
#define _HSU_CONF_SEL_NV_H_

/*==============================================================================

High Speed USB configuration Selector NV
H E A D E R   F I L E

GENERAL DESCRIPTION
This file includes functions that allow reading/writing item from the NV 
memory, to be used by the High Speed USB component.

EXTERNALIZED FUNCTIONS
hsu_conf_sel_nv_esn_imei_read
hsu_conf_sel_is_usb_core_snps 
hsu_conf_sel_hsic_core_enabled

is_hsusb_core_enabled
hsu_conf_sel_is_diag_over_hsic
hsu_conf_sel_is_rmnet_over_hsusb
hsu_conf_sel_hide_ftl
hsu_conf_sel_get_rm_wakeup_delay
hsu_conf_sel_lpm_disabled
hsu_conf_sel_reconnect_on_reset_during_suspend
hsu_conf_sel_number_of_sdcc_luns
hsu_conf_sel_disable_sleep_voting
hsu_conf_sel_get_phy_config_param
hsu_conf_sel_get_efs_perf_settings
hsu_conf_sel_get_device_composition
hsu_conf_sel_get_resume_gpio
hsu_conf_sel_get_ldo_power_collapse
hsu_conf_sel_get_remote_wakeup_timeout
hsu_conf_sel_aggressive_ep0_error_capturing_enabled
hsu_conf_sel_disable_a2_pc_for_suspend
hsu_conf_sel_get_a2_pc_debounce_time_for_suspend
hsu_conf_sel_is_ahb2ahb_bypass_enabled
hsu_conf_sel_ignore_dtr_drop
hsu_conf_sel_mbim_wMTU

hsu_conf_sel_nv_efs_get
hsu_conf_sel_nv_efs_write
hsu_conf_sel_nv_efs_read
hsu_conf_sel_nv_efs_update
hsu_conf_sel_nv_efs_update_all_items
hsu_conf_sel_nv_efs_init_config_file

INITALIZATION AND SEQUENCING REQUIREMENTS
Functions should only be called from the TMC's context.     

Copyright (c) 2009, 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_conf_sel_nv.h#1 $
$DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when     who      what, where, why
-------- ------   ---------------------------------------------------------
11/19/06 ke       Initial version
04/22/07 me       Adding functions to read the ESN and IMEI
12/03/13 kameya   Clean unused nv items. New architecture to update NV items.

==============================================================================*/
#include "nv_items.h"
#include "hsu_phy_utils_int.h"
#include "hsu_clkmgr.h"

/*==============================================================================
Forward Declarations
==============================================================================*/

/*==============================================================================
Constants
==============================================================================*/

/*==============================================================================
Macros
==============================================================================*/

/*==============================================================================
Typedefs
==============================================================================*/
typedef enum {
  EFS_HSU_DIAG_NMEA_MDM,
  EFS_HSU_DIAG_NMEA_MDM_NET,                  
  EFS_HSU_DIAG_NMEA_MS_MDM_NET,
  EFS_HSU_MS,                                 
  EFS_HSU_MTP,                                 
  EFS_HSU_DIAG_MDM,                                         // NV Value is  5
  EFS_HSU_DIAG_MDM_NET,                                     // NV Value is  6
  EFS_HSU_DIAG_MS,                                          // NV Value is  7
  EFS_HSU_LOOPBACK,                                         // NV Value is  8
  EFS_HSU_DIAG_NMEA_MS_MDM_NET_AUDIO,                       // NV Value is  9
  EFS_HSU_DIAG_NMEA_SER3_MDM_NET,                           // NV Value is 10
  EFS_HSU_NET_DIAG_MDM_NMEA,                                // NV Value is 11
  EFS_HSU_NET_DIAG_MDM,                                     // NV Value is 12
  EFS_HSU_NET_DIAG_NMEA,                                    // NV Value is 13
  EFS_HSU_NET_DIAG,                                         // NV Value is 14
  EFS_HSU_STD_NET,                                          // NV Value is 15
  EFS_HSU_DIAG_MTP,                                         // NV Value is 16
  EFS_HSU_DIAG_NMEA_MS_MDM_NET3,                            // NV Value is 17
  EFS_HSU_DIAG_NMEA_MS_STD_MDM_STD_NET,                     // NV Value is 18
  EFS_HSU_DIAG_AUDIO,                                       // NV Value is 19
  EFS_HSU_DIAG_NMEA_MDM_NET3,                               // NV Value is 20
  EFS_HSU_DIAG_NMEA_MS_MDM_NET2,                            // NV Value is 21
  EFS_HSU_DIAG_NMEA_MDM_NET2,                               // NV Value is 22
  EFS_HSU_BREWMP_COMPOSITION,                               // NV Value is 23
  EFS_HSU_DIAG_NMEA_MDM_NET4,                               // NV Value is 24
  EFS_HSU_DIAG_NMEA_MDM_NET5,                               // NV Value is 25
  EFS_HSU_RNDIS_DIAG,                                       // NV Value is 26
  EFS_HSU_DIAG_NMEA_MDM_MBIM,                               // NV Value is 27
  EFS_HSU_DIAG_SER3_SER4_MDM_NET5,                          // NV Value is 28
  EFS_HSU_DIAG_SER3_SER4_MDM_CSVT_NET4,                     // NV Value is 29
  EFS_HSU_DIAG_NMEA_MDM_NET5_UNDP,                          // NV Value is 30
  EFS_HSU_DIAG_SER3_MDM_MBIM,                               // NV Value is 31
  EFS_HSU_DIAG_SER3_MDM_NET,                                // NV Value is 32
  EFS_HSU_DIAG_SER3_SER4_MDM_MBIM,                          // NV Value is 33
  EFS_HSU_DIAG_SER3_SER4_MDM_NET,                           // NV Value is 34
  EFS_HSU_DIAG_SER4_MDM_NET5,                               // NV Value is 35
  EFS_HSU_MBIM,                                             // NV Value is 36
  EFS_HSU_NMEA_MBIM,                                        // NV Value is 37
  EFS_HSU_SER3_MBIM,                                        // NV Value is 38
  EFS_HSU_SER3_SER4_MBIM,                                   // NV Value is 39
  EFS_HSU_DIAG_MDM_NET8,                                    // NV Value is 40
  EFS_HSU_DIAG_MDM_NET6,                                    // NV Value is 41            
  EFS_HSU_DIAG_SER5_SER4_MDM_CSVT_NET4,                     // NV Value is 42
  EFS_HSU_DIAG_SER4_MDM_CSVT_NET2,                          // NV Value is 43
  EFS_HSU_DIAG_SER3_MDM_MBIM_GNSS,                          // NV Value is 44
  EFS_HSU_DIAG_SER3_SER4_MDM_MBIM_GNSS,                     // NV Value is 45
  EFS_HSU_SER3_MBIM_GNSS,                                   // NV Value is 46
  EFS_HSU_SER3_SER4_MBIM_GNSS,                              // NV Value is 47
  EFS_HSU_DIAG_SER3_SER4_MDM_CSVT_NET2,                     // NV Value is 48
  EFS_HSU_DIAG_SER4_QDSS_MDM_NET5,                          // NV Value is 49
  EFS_HSU_DIAG_SER4_MDM_NET4,                               // NV Value is 50
  EFS_HSU_DIAG_SER3_SER4_MDM_NET5_2,                        // NV Value is 51
  EFS_HSU_DIAG_SER3_SER4_MDM_CSVT_NET4_2,                   // NV Value is 52
  EFS_HSU_DIAG_SER4_MDM_NET5_UNDP,                          // NV Value is 53
  EFS_HSU_DIAG_SER3_SER4_MDM_CSVT_NET2_IPCR,                // NV Value is 54
  EFS_HSU_DIAG_DCI_SER3_SER4_QDSS_MDM_CSVT_NET1_NET2_IPCR,  // NV Value is 55
  EFS_HSU_DIAG_SER4_MDM_NET2,                               // NV Value is 56
  EFS_HSU_DIAG_SER4_MDM_NET2_LWIP,                          // NV Value is 57
  EFS_HSU_DIAG_SER3_SER4_MDM_CSVT,                          // NV Value is 58
  EFS_HSU_DIAG_SER3_SER4_MDM_CSVT_IPCR,                     // NV Value is 59
  EFS_HSU_DIAG_DCI_SER3_SER4_QDSS_MDM_CSVT_IPCR,            // NV Value is 60
  EFS_HSU_DIAG_SER3_SER4_MDM_CSVT_NET2_INTR,                // NV Value is 61
  EFS_HSU_DIAG_SER3_SER4_MDM_CSVT_NET2_IPCR_INTR,           // NV Value is 62
  EFS_HSU_DIAG_DCI_SER3_SER4_QDSS_MDM_CSVT_NET2_IPCR_INTR,  // NV Value is 63
  EFS_HSU_DIAG_SER4_MDM_NET1,                               // NV Value is 64
  EFS_HSU_DIAG_SER4_QDSS_MDM_NET1_MUX,                      // NV Value is 65
  EFS_HSU_DIAG_SER4_QDSS_MDM_NET1,                          // NV Value is 66
  EFS_HSU_DIAG_SER4_QDSS_MDM_NET1_DPL,                      // NV Value is 67
  EFS_HSU_DIAG_SER4_QDSS_MDM_NET1_AUDIO,                    // NV Value is 68
  EFS_HSU_DIAG_QDSS_MDM_NET1,                               // NV Value is 69
  EFS_HSU_DIAG_DCI_SER3_SER4_QDSS_MDM_CSVT_NET2_IPCR_INTR__FAST_DIAG, // NV Value is 70
  EFS_HSU_DIAG_NMEA_MDM_NET_DPL,                            // NV Value is 71
  EFS_HSU_DIAG_SER4_MDM_NET_DPL,                            // NV Value is 72
  EFS_HSU_DIAG_DCI_SER3_SER4_MDM_NET1_IPCR,                 // NV Value is 73
  EFS_HSU_DIAG_DCI_SER3_SER4_MDM_USBNET1_IPCR,              // NV Value is 74
  EFS_HSU_DIAG_DCI_SER3_SER4_MDM_USBNET1_NET2_IPCR,         // NV Value is 75
  EFS_HSU_DIAG_NMEA_QDSS_MDM_NET1,                          // NV Value is 76                  
  EFS_HSU_DIAG_NMEA_QDSS_MDM_NET1_MUX,                      // NV Value is 77
  EFS_HSU_DIAG_MDM_STD_NET,                                 // NV Value is 78
  EFS_HSU_DIAG_NMEA_MDM1_MDM2_NET,                          // NV Value is 79
  EFS_HSU_DIAG,                                             // NV Value is 80
  EFS_HSU_DIAG_QDSS_MDM,                                    // NV Value is 81
  EFS_HSU_DIAG_SER3_SER5_MDM_NET1,                          // NV Value is 82
  EFS_HSU_MBIM_GNSS,                                        // NV Value is 83
  EFS_HSU_DIAG_SER3_SER5_SER6_MDM_NET1,                     // NV Value is 84
  EFS_HSU_DIAG_MDM1_MDM2_NET,                               // NV Value is 85
  EFS_HSU_DIAG_SER3_SER5_SER6_MDM,                          // NV Value is 86
  /* 
  * A define for a composition that cannot be set using EFS, but only
  * through the dynamic composition menu (UI)
  */
  EFS_HSU_NO_DEVICE  = 0xFF
} hsu_composition_enum; 

typedef enum {
#ifndef USB_THREADX
  HSU_EFS_HSUSB_ENABLED = 0,
#endif
  // depends on HSU_EFS_HSIC_ENABLED and HSU_EFS_HSUSB_ENABLED
  HSU_EFS_USB_CORE_SELECT, 

  HSU_EFS_HIDE_FTL,
  HSU_EFS_REMOTE_WAKEUP_DELAY,
  HSU_EFS_DISABLE_LPM,
  HSU_EFS_RECONNECT_ON_RESET_DURING_SUSPEND,

#ifdef FEATURE_HS_USB_MS_FD
  HSU_EFS_NO_OF_SDCC_LUNS,
#endif /* FEATURE_HS_USB_MS_FD */

  HSU_EFS_DISABLE_SLEEP_VOTING,
  HSU_EFS_PHY_CONFIG_PARAM, 
  HSU_EFS_DEVICE_COMPOSITION,
  HSU_EFS_LDO_POWER_COLLAPSE,
  HSU_EFS_GPIO_REMOTE_WAKEUP_TIMEOUT,
  HSU_EFS_AGGRESSIVE_EP0_ERROR_CAPTURING,
  HSU_EFS_DISABLE_A2_PC_FOR_SUSPEND,
  HSU_EFS_A2_PC_DEBOUNCE_TIME_FOR_SUSPEND,
  HSU_EFS_AHB2AHB_BYPASS,
  HSU_EFS_IGNORE_DTR_DROP,

#ifdef FEATURE_HS_USB_MBIM
  HSU_EFS_MBIM_WMTU,
#endif // FEATURE_HS_USB_MBIM

  HSU_EFS_PHY_PERF_SETTINGS,
  HSU_EFS_UL_BAM_BYPASS,
  HSU_EFS_ECM_RX_DSM_ITEM_SIZE,
  HSU_EFS_ECM_RX_DSM_ITEM_CNT,
  HSU_EFS_ECM_RX_TIMEOUT_MS,
  HSU_EFS_DUN_LOG_ENABLED,
  HSU_EFS_AUDIO_SELF_LOOPBACK_ENABLED,
  HSU_EFS_COMPLIANCE_TEST_MODE_ENABLED,
  HSU_EFS_SS_PHY_DISABLED,

  // HSU_EFS_NEW_ITEM
  // Add new NV item enum here 

  HSU_EFS_NO_OF_EFS_ITEMS
} hsu_efs_item_type;

typedef enum
{
  //HS_USB_DIAG_OVER_HSIC,  
  HS_USB_DIAG_OVER_HS_USB  
}hsu_diag_over_hs_core_type;

typedef enum
{
  //HS_USB_RMNET_OVER_HSIC,  
  HS_USB_RMNET_OVER_HS_USB  
}hsu_rmnet_over_hs_core_type;

typedef enum
{  
  USB3_SNPS_CORE,
  USB_CORE_MAX
} hsu_usb_core_type;

typedef struct {  
  hsu_efs_item_type type;  
  const char*       filename;  
  uint32            default_value;  
  uint32            current_value;  
  uint8             size;
} hsu_efs_item_info_type;


/*==============================================================================
Public function declaration for module
==============================================================================*/
boolean hsu_conf_sel_nv_esn_imei_read(char *, uint32);
boolean hsu_conf_sel_nv_get_esn(char * return_buff_ptr, uint32 return_buff_size);
boolean hsu_conf_sel_nv_get_imei(char * return_buff_ptr, uint16 return_buff_size);

/*==============================================================================
APIs to get local cache value of EFS items. Please maintain order.
==============================================================================*/
boolean hsu_conf_sel_is_usb_core_snps (void);

boolean hsu_conf_sel_hide_ftl (void);
uint32 hsu_conf_sel_get_rm_wakeup_delay(void);
boolean hsu_conf_sel_lpm_disabled(void);

boolean hsu_conf_sel_reconnect_on_reset_during_suspend(void);

#ifdef FEATURE_HS_USB_MS_FD  
uint8 hsu_conf_sel_number_of_sdcc_luns (void);
#endif /* FEATURE_HS_USB_MS_FD */

boolean hsu_conf_sel_disable_sleep_voting(void);
hsu_phy_utils_config_par_type hsu_conf_sel_get_phy_config_param(void);
hsu_clkmgr_speed_level_type hsu_conf_sel_get_efs_perf_settings(void);
hsu_composition_enum hsu_conf_sel_get_device_composition(void);
boolean hsu_conf_sel_get_resume_gpio(void);
boolean hsu_conf_sel_get_ldo_power_collapse(void);
uint32 hsu_conf_sel_get_remote_wakeup_timeout(void);
uint8 hsu_conf_sel_aggressive_ep0_error_capturing_enabled(void);
boolean hsu_conf_sel_disable_a2_pc_for_suspend(void);
hsu_phy_utils_config_pll hsu_conf_sel_get_a2_pc_debounce_time_for_suspend(void);
boolean hsu_conf_sel_is_ahb2ahb_bypass_enabled(void);
boolean hsu_conf_sel_ignore_dtr_drop(void);
#ifdef FEATURE_HS_USB_MBIM
  uint16 hsu_conf_sel_mbim_wMTU(void);
#endif // FEATURE_HS_USB_MBIM
boolean hsu_conf_sel_ul_bam_bypassed(void);
uint32 hsu_conf_sel_get_ecm_rx_dsm_item_size(void);
uint32 hsu_conf_sel_get_ecm_rx_dsm_item_cnt(void);
uint32 hsu_conf_sel_get_ecm_rx_timeout(void);


// Add a declaration for API to read new EFS item here

/*==============================================================================
Basic APIs
==============================================================================*/
uint32 hsu_conf_sel_nv_efs_get(hsu_efs_item_type);
boolean hsu_conf_sel_nv_efs_write( hsu_efs_item_type, uint32);
boolean hsu_conf_sel_nv_efs_read( hsu_efs_item_type, uint32*);
void hsu_conf_sel_nv_efs_update(hsu_efs_item_type);
void hsu_conf_sel_nv_efs_update_all_items(void);
void hsu_conf_sel_nv_efs_init_config_file(void);

#endif /* _HSU_CONF_SEL_NV_H_ */
