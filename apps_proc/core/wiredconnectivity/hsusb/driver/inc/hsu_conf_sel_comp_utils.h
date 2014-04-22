#ifndef _HSU_CONF_SEL_COMP_UTILS_H_
#define _HSU_CONF_SEL_COMP_UTILS_H_

/*==============================================================================

     High Speed USB Composition Utilities 
        H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    This module includes various utility functions, dealing with device 
    compositions. 

  EXTERNALIZED FUNCTIONS
    hsu_conf_sel_comp_utils_comp_id_is_valid_for_hsu
    hsu_conf_sel_comp_utils_get_comp_details
    hsu_conf_sel_comp_utils_get_comp_details_by_hsu_id
    hsu_conf_sel_comp_utils_get_cur_prod_id
    hsu_conf_sel_comp_utils_get_default_comp_info
    hsu_conf_sel_comp_utils_prod_id_is_valid_for_hsu
    hsu_conf_sel_comp_utils_get_default_std_comp_info
    hsu_conf_sel_comp_utils_set_comp_info

 
  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.
    
  Copyright (c) 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_conf_sel_comp_utils.h#1 $
  $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  06/29/07 bt  Added MTP support
  11/19/06 ke  Initial version
    
==============================================================================*/
#include "customer.h" /* for FEATURE_HSU_TEST to work */
#include "nv_items.h"
#include "hsu_conf_sel_types_int.h"
#include "hsu_conf_sel_nv.h"
#ifdef FEATURE_GOBI
#include "hsu_conf_sel_gobi_comp.h"
#endif /* FEATURE_GOBI */ 

/*==============================================================================
  Forward Declarations
==============================================================================*/

/*==============================================================================
  Constants
==============================================================================*/

/* ECM */
#ifdef FEATURE_HS_USB_ECM
   #define HSU_ECM_COMP_IS_VALID 1
#else /* FEATURE_HS_USB_ECM */
   #define HSU_ECM_COMP_IS_VALID 0
#endif /* FEATURE_HS_USB_ECM */

/* STANDARD ECM */
#ifdef FEATURE_HS_USB_STANDARD_ECM
   #define HSU_STANDARD_ECM_COMP_IS_VALID  1
#else /* FEATURE_HS_USB_STANDARD_ECM */
   #define HSU_STANDARD_ECM_COMP_IS_VALID  0
#endif /* FEATURE_HS_USB_STANDARD_ECM */

/* Number of ECM instances supported*/
#ifndef HS_USB_NUM_OF_ECM_INST
  #ifdef FEATURE_HS_USB_ECM_INT_EP_SHARING
    #define HS_USB_NUM_OF_ECM_INST 1
  #else /* FEATURE_HS_USB_ECM_INT_EP_SHARING */  
    #define HS_USB_NUM_OF_ECM_INST 1
  #endif /* FEATURE_HS_USB_ECM_INT_EP_SHARING */  
#endif /*HS_USB_NUM_OF_ECM_INST*/

#if (HS_USB_NUM_OF_ECM_INST > 8)
#error The max number of ECM instances supported is 8. Please re-define HS_USB_NUM_OF_ECM_INST
#endif

/*************************** HS-USB Product IDs *******************************/
/*
 * High Speed USB devices have to use a product ID that's different than any 
 * Product ID used by FS devices. 
 * HS-USB product IDs begin with an MSB of 1, that is 0x8000 and above 
 * (regular expression: 0x[8,9,a,b,c,d,e,f]hhh where h is any hexadecimal digit).
 * The product ID should be specified in the relevant *.inf files, so that the 
 * host will be able to load the correct driver. 
 */
#define HSU_PRODUCT_ID_DIAG_NMEA_MS_MDM_NET  0x8000
#define HSU_PRODUCT_ID_DIAG_NMEA_MDM_NET     0x8001
#define HSU_PRODUCT_ID_DIAG_NMEA_MDM         0x8002

#ifndef FEATURE_GOBI
/*PIDs for Gobi are defined in hsu_conf_sel_gobi_comp.h*/
/*
 * Product IDs for compositions that do not use IAD, but enumerate with a 
 * single interface per function instead. 
 */
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MS_MDM_NET    0x9000
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM_NET       0x9001
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM           0x9002
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_SER3_MDM_NET  0x9003
#define HSU_PROD_ID_SINGLE_IF_DIAG_MDM                0x9004
#define HSU_PROD_ID_SINGLE_IF_DIAG_MDM_NET            0x9005
#define HSU_PROD_ID_SINGLE_IF_DIAG_MS                 0x9006
/*
 * A product ID used for download into RAM in the U2 target.
 * This is intended to ensure that the device gets a different
 * port number when performing download into RAM than when running
 * AMSS.
 * This product ID always uses the single interface configuration.
 */
#define HSU_PRODUCT_ID_NON_AMSS           0x9008

#endif /* !FEATURE_GOBI */

/* The folowing defines the valid range for product IDs used by Linux */
#define HSU_PRODUCT_ID_LINUX_LOW_LIMIT1		  0x9010 
#define HSU_PRODUCT_ID_LINUX_HIGH_LIMIT1		  0x9020
#define HSU_PRODUCT_ID_LINUX_LOW_LIMIT2		  0xF000
#define HSU_PRODUCT_ID_LINUX_HIGH_LIMIT2		  0xF010
#define HSU_PRODUCT_ID_LINUX_LOW_LIMIT3		  0x8080
#define HSU_PRODUCT_ID_LINUX_HIGH_LIMIT3		  0x8090

/* The following defines the valid range for product IDs used by WM.
   We need 0x3200 to be legal PID, we kept a buffer of 0x20 PIDs around this PID */
#define HSU_PRODUCT_ID_WM_LOW_LIMIT 0x31F0
#define HSU_PRODUCT_ID_WM_HIGH_LIMIT 0x3210



/*
 * A product ID used for the OBEX-only composition
 * of the full-speed core (used for DIAG).
 */
#define HSU_PRODUCT_ID_FS_DIAG            0x9009

#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MS_MDM_NET_AUDIO 0x900A

/* This PID is used for DIAG & Standard ECM composition*/
#define HSU_PROD_ID_DIAG_STD_NET			 0x900B

/* This PID is used for DIAG,NMEA, DUN1,DUN2, Rmnet composition*/
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM1_MDM2_NET	   0x908B

/* This PID is  used for the Multiple ECM composition */
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MS_MDM_NET3         0x900C

/* This PID is used for the composition that includes Standard MDM 
   and Standard ECM */
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MS_STD_MDM_STD_NET  0x900D

/* This PID is used for DIAG (CDC/OBEX) + Digital Audio composition */
#define HSU_PROD_ID_SINGLE_IF_DIAG_AUDIO 0x901C

/*
* The default product ID used by DLOAG\ARMPRG when the 
* remote apis have not set the product id before going to DLOAD
*/
#define HSU_PRODUCT_ID_DIAG_ONLY             0x900E

/* This PID is used for the Multiple ECM composition without mass storage */
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM_NET3         0x900F

/* This PID is used for the Multiple ECM composition with mass storage */
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MS_MDM_NET2         0x9010

/* This PID is used for the Multiple ECM composition without mass storage */
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM_NET2         0x9011

/* This PID is used for the Multiple ECM composition without mass storage */
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM_NET4         0x9033

/* This PID is used for the Multiple ECM composition without mass storage */
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM_NET5         0x9034

/* This PID is used for DIAG + NMEA + MBIM Composition */
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM_MBIM              0x9043

/* This PID is used for the DIAG + SER3 + SER4 + MDM + NET5 */
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER3_SER4_MDM_NET5         0x9048

/* This PID is used for the DIAG + SER3 + SER4 + MDM + CSVT + NET4 */
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER3_SER4_MDM_CSVT_NET4    0x904C

/* This PID is used for the DIAG + NMEA + MDM + NET5
** This is to support MCCI driver which only support UNDP PID. 
*/
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM_NET5_UNDP         0x920B

/* This PID is used for DIAG + SER4 + MDM + NET5
** This is to support MCCI driver which only support UNDP PID 
*/
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER4_MDM_NET5_UNDP         0x920B

/* This PID is used for DIAG + SER3 + MBIM Composition */
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER3_MDM_MBIM              0x904F
/* This PID is used for DIAG + SER3 + NET Composition */
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER3_MDM_NET              0x9050
/* This PID is used for DIAG + SER3 + SER4 + MBIM Composition */
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER3_SER4_MDM_MBIM              0x9051
/* This PID is used for DIAG + SER3 + SER4 + NET Composition */
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER3_SER4_MDM_NET              0x9052
/* This PID is used for DIAG + SER4 + MDM + NET5 Composition */
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER4_MDM_NET5              0x9055

/* This PID is used for MBIM device*/
#define HSU_PROD_ID_SINGLE_IF_MBIM            0x905B
/* This PID is used for NMEA & MBIM device*/
#define HSU_PROD_ID_SINGLE_IF_NMEA_MBIM       0x905D
/* This PID is used for SER3 & MBIM device*/
#define HSU_PROD_ID_SINGLE_IF_SER3_MBIM       0x905E
/* This PID is used for SER3 & SER4 & MBIM device*/
#define HSU_PROD_ID_SINGLE_IF_SER3_SER4_MBIM  0x905F

/* This PID is used for DIAG + MDM + 8 NET device*/
#define HSU_PROD_ID_SINGLE_IF_DIAG_MDM_NET8   0X9062

/* This PID is used for DIAG + MDM + NET6 */
#define HSU_PROD_ID_SINGLE_IF_DIAG_MDM_NET6                        0x9068

/* This PID is used for DIAG + SER5 + SER4 + MDM + CSVT + NET4 */
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER5_SER4_MDM_CSVT_NET4         0x9069

/* This PID is used for DIAG + SER4 + MDM + CSVT + NET2 */
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER4_MDM_CSVT_NET2              0x9070

/* New PID's to support GNSS interface for WoS */
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER3_MDM_MBIM_GNSS              0x9071
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER3_SER4_MDM_MBIM_GNSS         0x9072
#define HSU_PROD_ID_SINGLE_IF_SER3_MBIM_GNSS                       0x9073
#define HSU_PROD_ID_SINGLE_IF_SER3_SER4_MBIM_GNSS                  0x9074

/* This PID is used for the DIAG + SER3 + SER4 + MDM + CSVT + NET2 */
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER3_SER4_MDM_CSVT_NET2         0x9075

#define HSU_PROD_ID_SINGLE_IF_DIAG_SER4_QDSS_MDM_NET5   0X9076

/* This PID is used for DIAG + SER4 + MDM + NET4 Composition */
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER4_MDM_NET4                   0x9077


/* This PID is used for the DIAG + SER3 + SER4 + MDM + NET5 */
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER3_SER4_MDM_NET5_2            0x9079

/* This PID is used for the DIAG + SER3 + SER4 + MDM + CSVT + NET4 */
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER3_SER4_MDM_CSVT_NET4_2       0x9080

/* This PID is used for the DIAG + SER3 + SER4 + MDM + CSVT + NET2 + IPCRouter */
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER3_SER4_MDM_CSVT_NET2_IPCR    0x908A

/* This PID is used for the 
   DIAG + DCI + SER3 + SER4 + QDSS Trace (Bulk IN only) + MDM + CSVT + NET1 + NET2 IPCRouter */
#define HSU_PROD_ID_SINGLE_IF_DIAG_DCI_SER3_SER4_QDSS_MDM_CSVT_NET1_NET2_IPCR   0x908E

/* This PID is used for DIAG + SER4 + MDM + NET2 Composition */
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER4_MDM_NET2                                0x908F

// This PID is used for DIAG + SER4 (EFS sync) + DUN + NET1_MUX + NET2_MUX + LWIP
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER4_MDM_NET2_LWIP                           0x9090

// This PID is used for  DIAG + SER3 (SYS_M) + SER4 (EFS sync) + DUN + CSVT
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER3_SER4_MDM_CSVT                           0x909C

// This PID is used for DIAG + SER3 (SYS_M) + SER4 (EFS sync) + DUN + CSVT + IPCR
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER3_SER4_MDM_CSVT_IPCR                      0x909D

// This PID is used for DIAG + DCI + SER3 (SYS_M) + SER4 (EFS sync) + QDSS Trace (Bulk IN only) + DUN +
// CSVT + IPCR (used for LA FUSION 4.5 superset without RmNET)
#define HSU_PROD_ID_SINGLE_IF_DIAG_DCI_SER3_SER4_QDSS_MDM_CSVT_IPCR             0x909E

// This PID uses Interrupt IN Endpoints. Cannot be used on 9x35
// DIAG + SER3 + SER4 (EFS sync) + MDM + CSVT + NET1_MUX + NET2_MUX
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER3_SER4_MDM_CSVT_NET2_INTR                 0x909F

// This PID uses Interrupt IN Endpoints. Cannot be used on 9x35
// DIAG + SER3 + SER4 (EFS sync) + MDM + CSVT + NET1_MUX + NET2_MUX + IPCR
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER3_SER4_MDM_CSVT_NET2_IPCR_INTR            0x90A0

// Default Superset for Fusion 4. This PID uses Interrupt IN Endpoints.
// DIAG + DCI + SER3 + SER4 (EFS sync) + QDSS + MDM + CSVT + NET1_MUX + NET2_MUX + IPCR
#define HSU_PROD_ID_SINGLE_IF_DIAG_DCI_SER3_SER4_QDSS_MDM_CSVT_NET2_IPCR_INTR   0x90A4

// DIAG + SER4 (EFS sync) + DUN(MDM) + NET1_MUX
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER4_MDM_NET1                                0x90A5

// DIAG + SER4 (EFS sync) + QDSS (Bulk IN only) + MDM + NET1_MUX
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER4_QDSS_MDM_NET1_MUX                       0x90A6

// DIAG + SER4 (EFS sync) + QDSS (Bulk IN only) + MDM + NET1
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER4_QDSS_MDM_NET1                           0x90A8

// DIAG + SER4 (EFS sync) + QDSS (Bulk IN only) + DUN + NET1 (no MUX) + DPL (Bulk IN)
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER4_QDSS_MDM_NET1_DPL                       0x90AB

// DIAG + SER4 (EFS sync) + QDSS (Bulk IN only) + MDM + NET1+AUDIO
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER4_QDSS_MDM_NET1_AUDIO                     0x90AC

// DIAG + QDSS Trace (Bulk IN) + DUN(MDM) + NET1 (no MUX)
#define HSU_PROD_ID_SINGLE_IF_DIAG_QDSS_MDM_NET1                                0x90B2

// DIAG + NMEA + DUN(MDM) + NET1 (no MUX) + DPL
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM_NET1_DPL                            0x90C2

// DIAG + SER4 (EFS sync) + DUN(MDM) + NET1 (no MUX) + DPL
#define HSU_PROD_ID_SINGLE_IF_DIAG_SER4_MDM_NET1_DPL                            0x90C3

// DIAG + NMEA+QDSS Trace (Bulk IN) + DUN(MDM) + NET1 (no MUX)
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_QDSS_MDM_NET1                           0x90C8

// DIAG + NMEA+QDSS Trace (Bulk IN) + DUN(MDM) + NET1 (MUX)
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_QDSS_MDM_NET1_MUX                       0x90C9

// DIAG + SER3 + SER5 + DUN + NET1 (no MUX)
#define HSU_PROD_ID_SINGLE_IFC_DIAG_SER3_SER5_MDM_NET1                          0x90E1

// DIAG + SER3 + SER5 + SER6 + DUN + NET1 (no MUX)
#define HSU_PROD_ID_SINGLE_IFC_DIAG_SER3_SER5_SER6_MDM_NET1                     0x90E3

/* This PID is used for DIAG,DUN1,DUN2, Rmnet composition*/
#define HSU_PROD_ID_SINGLE_IF_DIAG_MDM1_MDM2_NET                                0x90FA

// DIAG + SER3 + SER5 + SER6 + DUN 
#define HSU_PROD_ID_SINGLE_IFC_DIAG_SER3_SER5_SER6_MDM                          0x90FB

/*
 * Product IDs that do not require a Qualcomm inf file
 * (0xF*** format is used for those )
 */ 
#define HSU_PRODUCT_ID_MS                    0xF000
#define HSU_PRODUCT_ID_PICTBRIDGE            0xF001
#define HSU_PRODUCT_ID_BREW_EXT              0xF002
#define HSU_PRODUCT_ID_MTP                   0xF003

/*
 * A product ID used for standalone HID enumeration, 
 * for fast dead battery charging
 */
#define HSU_PRODUCT_ID_HID                   0xF006
#define HSU_PRODUCT_ID_LOOPBACK              0xF008 

/******************************************************************************/

/* 
 * High Speed USB device product ID - invalid value. 
 */
#define HSU_PRODUCT_ID_INVALID               0xFFFF

/*==============================================================================
  Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*==============================================================================

FUNCTION          HSU_CONF_SEL_COMP_UTILS_COMP_ID_IS_VALID_FOR_HSU

DESCRIPTION
  Checks if the given value is a valid composition ID for HS-USB.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if valid, FALSE otherwise.

SIDE EFFECTS
  None. 

==============================================================================*/
boolean hsu_conf_sel_comp_utils_comp_id_is_valid_for_hsu
(
  hsu_composition_enum efs_comp_id
);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_COMP_UTILS_GET_COMP_DETAILS

DESCRIPTION
  This function returns a pointer to the information relevant for the given 
  composition ID.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to hsu_conf_sel_comp_info_type for success, 
  on failures returns NULL.

SIDE EFFECTS
  None.

==============================================================================*/
const hsu_conf_sel_comp_info_type * hsu_conf_sel_comp_utils_get_comp_details
(
  hsu_composition_enum nv_composition_id
);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_COMP_UTILS_GET_COMP_DETAILS_BY_HSU_ID

DESCRIPTION
  This function returns a pointer to the information relevant for the given 
  HSU composition ID.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to hsu_conf_sel_comp_info_type for success, 
  on failures returns NULL.

SIDE EFFECTS
  None.

==============================================================================*/
const hsu_conf_sel_comp_info_type * 
  hsu_conf_sel_comp_utils_get_comp_details_by_hsu_id
(
  hsu_conf_sel_composition_id_enum hsu_comp_id
);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_COMP_UTILS_GET_COMP_DETAILS_BY_PID

DESCRIPTION
  This function returns a pointer to the information relevant for the given 
  Product ID (PID).

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to hsu_conf_sel_comp_info_type for success, 
  on failures returns NULL.

SIDE EFFECTS
  None.

==============================================================================*/
const hsu_conf_sel_comp_info_type * 
  hsu_conf_sel_comp_utils_get_comp_details_by_pid
(
  uint16 product_id
);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_COMP_UTILS_GET_DEFAULT_COMP_INFO

DESCRIPTION
  Returns the composition info for the default composition. 

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the default composition's info for success,
  on failures returns NULL.

SIDE EFFECTS
  None. 

==============================================================================*/
const hsu_conf_sel_comp_info_type * 
  hsu_conf_sel_comp_utils_get_default_comp_info(void);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_COMP_UTILS_GET_DEFAULT_STD_COMP_INFO

DESCRIPTION
Returns the default composition info for an OS that supports standard driver 
composition. 

DEPENDENCIES
None.

RETURN VALUE
Pointer to the default standard composition's info for success,
on failures returns NULL.

SIDE EFFECTS
None. 

==============================================================================*/
const hsu_conf_sel_comp_info_type * 
  hsu_conf_sel_comp_utils_get_default_std_comp_info(void);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_COMP_UTILS_SET_COMP_INFO

DESCRIPTION
Set a specific composition info entry in the supported compositions table.

DEPENDENCIES
None.

RETURN VALUE
True for success, False for failure.

SIDE EFFECTS
None. 

==============================================================================*/
boolean hsu_conf_sel_comp_utils_set_comp_info(
  hsu_conf_sel_composition_id_enum  hsu_comp_id,
  /* The ID of the updated composition */

  uint16 product_id,
  /* Assigned product ID for the composition */

  uint8  num_of_instances_per_fd[HSU_CFG_SEL_NUM_OF_FD_TYPES],
  /* Function drivers included in the composition */

  uint8  num_of_rdm_dev_in_comp,                   
  /* Number of RDM devices used */

  rdm_device_enum_type rdm_devices[HSU_NUM_OF_RDM_USB_PORTS]
  /* RDM devices used */
);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_COMP_UTILS_UPDATE_DEFAULT_COMP_NV_VALUE

DESCRIPTION
Update the default_comp_nv_value based on platform detect for GOBI or LA Fusion.

DEPENDENCIES
None.

RETURN VALUE
True for success, False for failure.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_conf_sel_comp_utils_update_default_comp_nv_value(
  hsu_composition_enum nv_value);

#endif /* _HSU_CONF_SEL_COMP_UTILS_H_ */

