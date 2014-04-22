
/*==============================================================================

     High Speed USB configuration Selector Module - Type Definitions
        I N T E R N A L  H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    Type definitions used by the various config selector files. 

  EXTERNALIZED FUNCTIONS
    None.

  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.
    
  Copyright (c) 2007-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_conf_sel_types_int.h#2 $
  $DateTime: 2021/02/08 00:10:03 $ $Author: pwbldsvc $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  08/25/09  rg      Initial version

==============================================================================*/

#ifndef _HSU_CONF_SEL_TYPES_INT_H
#define _HSU_CONF_SEL_TYPES_INT_H
#include "hsu_conf_sel_types.h"
#include "hsu_conf_sel_nv.h"
#include "rdevmap.h"

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
/*
 * The valid HSU compositions (NV based and dynamic). 
 * Currently: 
 * 1. DIAG & NMEA & MS & MDM & Net device
 * 2. DIAG & NMEA & MDM & Net device
 * 3. DIAG & NMEA & MDM device
 * 4. MS only device
 * 5. PictBridge device
 */
typedef enum
{
  HSU_CONF_SEL_FIRST_COMP = 0,
#ifdef FEATURE_HS_USB_MS_FD
  HSU_CONF_SEL_DIAG_NMEA_MS_MDM_NET_COMP = HSU_CONF_SEL_FIRST_COMP,
  HSU_CONF_SEL_DIAG_NMEA_MDM_NET_COMP,
#else /*!FEATURE_HS_USB_MS_FD*/
  HSU_CONF_SEL_DIAG_NMEA_MDM_NET_COMP = HSU_CONF_SEL_FIRST_COMP,
#endif /*FEATURE_HS_USB_MS_FD*/

  HSU_CONF_SEL_DIAG_NMEA_MDM_COMP,

#ifdef FEATURE_HS_USB_SER3_PORT
  HSU_CONF_SEL_DIAG_NMEA_MDM_NET_SER3_COMP,
#endif /*FEATURE_HS_USB_SER3_PORT*/
  
#ifdef FEATURE_HS_USB_MS_FD
  HSU_CONF_SEL_MS_COMP,
#endif /*FEATURE_HS_USB_MS_FD*/

  HSU_CONF_SEL_PICTBRIDGE_COMP,
  HSU_CONF_SEL_DIAG_MDM_COMP,
  HSU_CONF_SEL_DIAG_MDM_NET_COMP,

#ifdef FEATURE_HSU_MTP
  HSU_CONF_SEL_BREW_EXT_COMP,
  HSU_CONF_SEL_MTP_COMP,
#endif /*FEATURE_HSU_MTP*/

#ifdef FEATURE_HS_USB_MS_FD
  HSU_CONF_SEL_DIAG_MS_COMP,
#endif /*FEATURE_HS_USB_MS_FD*/ 

#ifdef FEATURE_HS_USB_SAMPLE_LOOPBACK
  HSU_CONF_SEL_LOOPBACK_COMP,
#endif /*FEATURE_HS_USB_SAMPLE_LOOPBACK*/

#ifdef FEATURE_HS_USB_DIGITAL_AUDIO_DEVICE
  HSU_CONF_SEL_DIAG_NMEA_MS_MDM_NET_AUDIO_COMP,
  HSU_CONF_SEL_DIAG_AUDIO_COMP,
#endif /* FEATURE_HS_USB_DIGITAL_AUDIO_DEVICE */

  HSU_CONF_SEL_DIAG_STD_NET_COMP,

#ifdef FEATURE_HS_USB_MS_FD 
  HSU_CONF_SEL_DIAG_NMEA_MS_MDM_NET3_COMP,
  HSU_CONF_SEL_DIAG_NMEA_MS_STD_MDM_STD_NET_COMP,
#endif /* FEATURE_HS_USB_MS_FD */

#ifdef T_UNDP
  HSU_CONF_SEL_NET_DIAG_MDM_NMEA_COMP,
  HSU_CONF_SEL_NET_DIAG_MDM_COMP,
  HSU_CONF_SEL_NET_DIAG_NMEA_COMP,
  HSU_CONF_SEL_NET_DIAG_COMP,
#endif /* T_UNDP */

  HSU_CONF_SEL_DIAG_NMEA_MDM_NET3_COMP,

#if defined(FEATURE_HS_USB_BREWMP_COMPOSITION)
  /* Dynamic composition defined by the BrewMP layer at runtime. */
  HSU_CONF_SEL_BREWMP_COMP,
#endif  /* FEATURE_HS_USB_BREWMP_COMPOSITION */

#ifdef FEATURE_HS_USB_MS_FD 
  /* DIAG & NMEA & MS & MDM & Net device1 & Net device2 */
  HSU_CONF_SEL_DIAG_NMEA_MS_MDM_NET2_COMP,
#endif /* FEATURE_HS_USB_MS_FD */

/* DIAG & SER3 & SER4 & MDM & 5 Net devices */ 
  HSU_CONF_SEL_DIAG_SER3_SER4_MDM_NET5_COMP,
  /* DIAG & SER3 & SER4 & MDM & CSVT & 5 Net devices */ 
  HSU_CONF_SEL_DIAG_SER3_SER4_MDM_CSVT_NET4_COMP,

  /* DIAG & NMEA & MDM & Net device1 & Net device2 */
  HSU_CONF_SEL_DIAG_NMEA_MDM_NET2_COMP,
  /* DIAG & NMEA & MDM & 4 Net devices */ 
  HSU_CONF_SEL_DIAG_NMEA_MDM_NET4_COMP,
  /* DIAG & NMEA & MDM & 5 Net devices */ 
  HSU_CONF_SEL_DIAG_NMEA_MDM_NET5_COMP,
  /* DIAG & NMEA & MDM & MBIM device*/
  HSU_CONF_SEL_DIAG_NMEA_MDM_MBIM_COMP,
  /* This is to support third-party driver which only support GOBI PID. */
  HSU_CONF_SEL_DIAG_NMEA_MDM_NET5_UNDP_COMP,

  /* DIAG & SER3 & MDM & MBIM device*/
  HSU_CONF_SEL_DIAG_SER3_MDM_MBIM_COMP,
  /* DIAG & SER3 & MDM & NET device*/
  HSU_CONF_SEL_DIAG_SER3_MDM_NET_COMP,
  /* DIAG & SER3 & SER3 & MDM & MBIM device*/
  HSU_CONF_SEL_DIAG_SER3_SER4_MDM_MBIM_COMP,
  /* DIAG & SER3 & SER4 & MDM & NET device*/
  HSU_CONF_SEL_DIAG_SER3_SER4_MDM_NET_COMP,

  /* DIAG & SER4 & MDM & 5 NET device*/
  HSU_CONF_SEL_DIAG_SER4_MDM_NET5_COMP,
  
  /* MBIM device*/
  HSU_CONF_SEL_MBIM_COMP,
  /* NMEA & MBIM device*/
  HSU_CONF_SEL_NMEA_MBIM_COMP,
  /* SER3 & MBIM device*/
  HSU_CONF_SEL_SER3_MBIM_COMP,
  /* SER3 & SER4 & MBIM device*/
  HSU_CONF_SEL_SER3_SER4_MBIM_COMP,

  /* DIAG + MDM + 8 NET */
  HSU_CONF_SEL_DIAG_MDM_NET8_COMP,

  /* DIAG & MDM & NET6 */
  HSU_CONF_SEL_DIAG_MDM_NET6_COMP,

  /* DIAG & SER5 & SER4 & MDM & CSVT & NET4 */
  HSU_CONF_SEL_DIAG_SER5_SER4_MDM_CSVT_NET4_COMP,

  /* DIAG & SER4 & MDM & CSVT & NET2 */
  HSU_CONF_SEL_DIAG_SER4_MDM_CSVT_NET2_COMP,

  /* DIAG & SER3 & SER4 & MDM & 5 Net devices */ 
  HSU_CONF_SEL_DIAG_SER3_SER4_MDM_NET5_COMP_2,
  /* DIAG & SER3 & SER4 & MDM & CSVT & 5 Net devices */ 
  HSU_CONF_SEL_DIAG_SER3_SER4_MDM_CSVT_NET4_COMP_2,

  /* NEW COMPOSITION TO SUPPORT GNSS INTERFACE FOR WOS */
  HSU_CONF_SEL_DIAG_SER3_MDM_MBIM_GNSS_COMP,
  HSU_CONF_SEL_DIAG_SER3_SER4_MDM_MBIM_GNSS_COMP,
  HSU_CONF_SEL_SER3_MBIM_GNSS_COMP,
  HSU_CONF_SEL_SER3_SER4_MBIM_GNSS_COMP,

  /* DIAG & SER3 & SER4 & MDM & CSVT & 2 Net devices */
  HSU_CONF_SEL_DIAG_SER3_SER4_MDM_CSVT_NET2_COMP,

  /* DIAG & SER4 & MDM & 4 Net devices */
  HSU_CONF_SEL_DIAG_SER4_MDM_NET4_COMP,

  /* DIAG + SER4 + QDSS + MDM + 5 NET */
  HSU_CONF_SEL_DIAG_SER4_QDSS_MDM_NET5_COMP,
  
  /* DIAG & SER4 & MDM & 5 NET device 
  ** This is to support third-party driver which only support UNDP PID.
  */
  HSU_CONF_SEL_DIAG_SER4_MDM_NET5_UNDP_COMP,

  HSU_CONF_SEL_DIAG_SER3_SER4_MDM_CSVT_NET2_IPCR_COMP,
  
  HSU_CONF_SEL_DIAG_DCI_SER3_SER4_QDSS_MDM_CSVT_NET1_NET2_IPCR_COMP,
  
  HSU_CONF_SEL_DIAG_SER4_MDM_NET2_COMP,

  HSU_CONF_SEL_DIAG_SER4_MDM_NET2_LWIP_COMP,
  
  HSU_CONF_SEL_DIAG_SER3_SER4_MDM_CSVT_COMP,

  HSU_CONF_SEL_DIAG_SER3_SER4_MDM_CSVT_IPCR_COMP,

  HSU_CONF_SEL_DIAG_DCI_SER3_SER4_QDSS_MDM_CSVT_IPCR,

  // This composition uses Interrupt IN Endpoints.
  HSU_CONF_SEL_DIAG_SER3_SER4_MDM_CSVT_NET2_INTR_COMP,

  // This composition uses Interrupt IN Endpoints.
  HSU_CONF_SEL_DIAG_SER3_SER4_MDM_CSVT_NET2_IPCR_INTR_COMP,

  // This composition uses Interrupt IN Endpoints.
  HSU_CONF_SEL_DIAG_DCI_SER3_SER4_QDSS_MDM_CSVT_NET2_IPCR_INTR_COMP,

  HSU_CONF_SEL_DIAG_SER4_MDM_NET1_COMP,

  HSU_CONF_SEL_DIAG_SER4_QDSS_MDM_NET1_MUX_COMP,

  HSU_CONF_SEL_DIAG_SER4_QDSS_MDM_NET1_COMP,

  HSU_CONF_SEL_DIAG_SER4_QDSS_MDM_NET1_DPL_COMP,

  HSU_CONF_SEL_DIAG_SER4_QDSS_MDM_NET1_AUDIO_COMP,

  HSU_CONF_SEL_DIAG_QDSS_MDM_NET1_COMP,
  
  HSU_CONF_SEL_DIAG_NMEA_MDM_NET1_DPL_COMP,

  HSU_CONF_SEL_DIAG_SER4_MDM_NET1_DPL_COMP,

  HSU_CONF_SEL_DIAG_NMEA_QDSS_MDM_NET1_COMP,

  HSU_CONF_SEL_DIAG_NMEA_QDSS_MDM_NET1_MUX_COMP,

  HSU_CONF_SEL_DIAG_MDM_STD_NET_COMP,
  
  HSU_CONF_SEL_DIAG_NMEA_MDM1_MDM2_NET_COMP,

  HSU_CONF_SEL_DIAG_SER3_SER5_MDM_NET1_COMP,

  HSU_CONF_SEL_DIAG_SER3_SER5_SER6_MDM_NET1_COMP,

  HSU_CONF_SEL_DIAG_MDM1_MDM2_NET_COMP,

  HSU_CONF_SEL_DIAG_SER3_SER5_SER6_MDM_COMP,

  HSU_CONF_SEL_NUM_OF_COMPS /* must be the last one! */
} hsu_conf_sel_composition_id_enum;

typedef enum
{
  HSU_SLOT_INDEX_FOR_DIAG,
  HSU_SLOT_INDEX_FOR_NMEA,
  HSU_SLOT_INDEX_FOR_SER3,
  HSU_SLOT_INDEX_FOR_SER4,
  HSU_SLOT_INDEX_FOR_SER5,
  HSU_SLOT_INDEX_FOR_SER6,
  HSU_SLOT_INDEX_FOR_DCI,
  HSU_SLOT_INDEX_FOR_QDSS,
  HSU_SLOT_INDEX_MAX,
}hsu_obex_slot_index_enum;

typedef enum
{
  NO_HSU_IFACE = 0,

  MS_HSU_IFACE,

  OBEX_HSU_IFACE_START,
  //---------------------------------------
  DIAG_HSU_IFACE = OBEX_HSU_IFACE_START,
  NMEA_HSU_IFACE,
  SER3_HSU_IFACE,
  SER4_HSU_IFACE,
  SER5_HSU_IFACE,
  SER6_HSU_IFACE,
  DCI_HSU_IFACE,
  QDSS_HSU_IFACE,
  //---------------------------------------
  OBEX_HSU_IFACE_END,

  ACM_HSU_IFACE_START,
  //---------------------------------------
  MDM_HSU_IFACE = ACM_HSU_IFACE_START,
  MDM2_HSU_IFACE,
  CSVT_HSU_IFACE,
  //---------------------------------------
  ACM_HSU_IFACE_END,

  ECM_HSU_IFACE_START,
  //---------------------------------------
  NET1_HSU_IFACE = ECM_HSU_IFACE_START,
  NET2_HSU_IFACE,
  NET3_HSU_IFACE,
  NET4_HSU_IFACE,
  NET5_HSU_IFACE,
  NET6_HSU_IFACE,
  NET7_HSU_IFACE,
  NET8_HSU_IFACE,

  MBIM_HSU_IFACE,
  GNSS_HSU_IFACE,
  IPCR_HSU_IFACE,
  LWIP_HSU_IFACE,
  DPL_HSU_IFACE,
  //---------------------------------------
  ECM_HSU_IFACE_END,

  AUDIO_HSU_IFACE,

  NUM_OF_HSU_IFACE,
}hsu_iface_index_enum_type;

#define HSU_IFACE_ABSENT_IN_COMP 0xFF

typedef enum
{
  CDC_FD_IF_PROTOCOL__SAHARA      = 0x10,
  CDC_FD_IF_PROTOCOL__FIREHOSE    = 0x20,
  CDC_FD_IF_PROTOCOL__DIAG_HDLC   = 0x30,
  CDC_FD_IF_PROTOCOL__DIAG_DUN    = 0x40,
  CDC_FD_IF_PROTOCOL__RMNET_QMAP  = 0x50,
  CDC_FD_IF_PROTOCOL__NMEA        = 0x60,
  CDC_FD_IF_PROTOCOL__QDSS        = 0x70,
  CDC_FD_IF_PROTOCOL__ADPL        = 0x80,
} hsu_cdc_fd_if_vendor_protocol_type;

/* 
 * The number of RDM enum values which represent USB ports 
 * Currently 5: RDM_USB_MDM_DEV, RDM_USB_MMC_DEV, RDM_USB_DIAG_DEV, 
 *              RDM_USB_NMEA_DEV, RDM_USB_NET_WWAN_DEV, RDM_USB_AUDIO_DEV,
 */
//#define HSU_NUM_OF_RDM_USB_PORTS 6

/*
 * A structure that contains all the relevant information for a composition.
 */
typedef struct 
{
  /* The HSU composition id representing this composition */
  hsu_conf_sel_composition_id_enum  hsu_comp_id;

  /* The EFS value representing this composition (EFS_HSU_NO_DEVICE if
     the composition cannot be chosen using EFS). */
  hsu_composition_enum  efs_comp_id;

  /* The product ID for this composition */
  uint16  product_id;

  /* The OS that supports this composition */
  hsu_conf_sel_supported_drv_by_comp_enum supported_drv_by_comp;

  /* For each type of FD, the number of instances used in the composition. */
  uint8   num_of_fd_instances_in_comp[HSU_CFG_SEL_NUM_OF_FD_TYPES];

  /* 
   * Array that holds the SLOT index for diff FD type interfaces. 
   * This slot index assignment should match to Host driver interface 
   * infomataion as it expects the instances in certain order.
   */
  hsu_iface_index_enum_type iface_type[NUM_OF_HSU_IFACE];

  /* 
  * Array of the RDM devices (USB ports) that participate in the composition.
  * Used for dynamic composition changes - 
  * we go over them one by one and call rdm_get_service. If there is in fact a 
  * service that uses this USB port, we call rdm_assign_port for that service,
  * with NULL as the destination port.
  */
  uint8   num_of_rdm_dev_in_comp;

  rdm_device_enum_type rdm_devices[HSU_NUM_OF_RDM_USB_PORTS];

  boolean comp_is_supported;

} hsu_conf_sel_comp_info_type;

#endif
