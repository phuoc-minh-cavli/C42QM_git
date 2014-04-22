#ifndef _HSU_PLATFORM_H
#define _HSU_PLATFORM_H
/*=============================================================================

                                H E A D E R   F I L E

==============================================================================*/
/**/
/**
@file hsu_platform.h


Initialization and sequencing requirements:\n
hsu_platform_init must be called prior to performing any other operations
*/

/*=============================================================================
Copyright (c) 2006-2012 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/
/*=============================================================================

                            EDIT HISTORY FOR MODULE
  
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
  
$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_platform.h#1 $
$DateTime: 2020/01/30 22:49:35 $
    
when     who  what, where, why
-------- ---  ---------------------------------------------------------
07/11/12 ag  Initial version
=============================================================================*/
/*==============================================================================
                        INCLUDE FILES
==============================================================================*/

#include "hsu_conf_sel_nv.h"
#include "DDIChipInfo.h"

/* GPIO APIs */
#include "DDITlmm.h"
#include "DDIGPIOInt.h"
#include "DalDevice.h"
#include "DALDeviceId.h"

/*=============================================================================
  Forward Declarations
=============================================================================*/

/*=============================================================================
  Constants
=============================================================================*/

/*=============================================================================
  Macros
=============================================================================*/

    /* GPIO assignments for M Fusion  */
#define HSU_M_AP2MDM_HSIC_READY       66    /* AP2MDM_CHNL_RDY */
#define HSU_M_MDM2AP_REMOTE_WAKEUP    24    /* MDM2AP_WAKEUP */
#define HSU_M_AP2MDM_VDD_MIN          58
#define HSU_M_MDM2AP_VDD_MIN          59

  
    /* GPIO assignments for LA Fusion  */
#define HSU_LA_AP2MDM_HSIC_READY       66   /* AP2MDM_CHNL_RDY */
#define HSU_LA_MDM2AP_REMOTE_WAKEUP    24   /* MDM2AP_WAKEUP */
#define HSU_LA_AP2MDM_VDD_MIN          58
#define HSU_LA_MDM2AP_VDD_MIN          59
    
 
  /* GPIO assignments for WoS  Fusion  */
#define HSU_GOBI_AP2MDM_HSIC_READY      30
#define HSU_GOBI_MDM2AP_REMOTE_WAKEUP   7
#define HSU_GOBI_AP2MDM_VDD_MIN         5
#define HSU_GOBI_MDM2AP_VDD_MIN         6

  /* Macros with Init values */  
#define HSU_GPIO_NOT_CONFIGURED         0
#define HSU_VERSION_NOT_CONFIGURED      0
#define HSU_S1_VOTING_NOT_CONFIGURED    FALSE
#define HSU_HSUSB_NOT_ENABLED           FALSE
#define HSU_DIAG_NOT_OVER_HSUSB         FALSE

/*=============================================================================
  Typedefs
=============================================================================*/
/* Enums for platforms  */
typedef enum
{
  HSU_PLATFORM_STANDALONE = 0,
  HSU_PLATFORM_M_FUSION,
  HSU_PLATFORM_LA_FUSION,
  HSU_PLATFORM_GOBI_FUSION,
  HSU_PLATFORM_RUMI,
  HSU_PLATFORM_VIRTIO,
  
  HSU_NUM_OF_PLATFORM_IDS, //This should be last one
} hsu_platform_id_enum;


#define HSU_PHY_ID_MASK  (0x03800000)
#define HSU_PHY_ID_SHFT  (23)

typedef enum
{
  HSU_PHY_AT = 0,
  HSU_PHY_BG,
  HSU_PHY_CS,
  HSU_PHY_DI,
  HSU_PHY_EU,

  HSU_NUM_PHY_IDS, // This should be the last one
} hsu_phy_id_enum;

/* Platform specific data structure */

typedef struct 
{
  hsu_platform_id_enum hsu_platform_id;    //Holds the hsu_platform_id_enum

  DalChipInfoVersionType nVer;              //Holds the harware version 

  uint32 const gpio_hsic_ready;                   //GPIO for APQ signal for HSIC ready duriong enumeration //TODO, remove this

  uint32 const gpio_remote_wakeup;                //GPIO for MDM to APQ Remote wakeup signal

  uint32 const gpio_apq2mdm_vdd_min;              //GPIO for APQ to MDM VDD min signal

  uint32 const gpio_mdm2apq_vdd_min;              //GPIO for MDM to APQ VDD min signal

  hsu_composition_enum const default_composition; //Deafult composition for enumeration

  boolean const is_hsic_enabled;                  //Is HSIC enabled by default for this platform?

  boolean const is_hsusb_enabled;                 //Is HSUSB enabled by default for this platform ?

  boolean const is_diag_over_hsusb;               //Is Diag over HSUSB by default ?

  boolean const is_ss_phy_disabled;               //Is SS PHY disabled by default ?

  hsu_phy_id_enum phy_id;                         //USB PHY settings

  boolean is_flashless_boot;                      //Is flashless boot enabled? Used for skipping EFS API access
} hsu_platform_info_type;

typedef struct
{
  DalDeviceHandle    *h_tlmm;
  DalDeviceHandle    *h_gpio_int;
  DALGpioSignalType  pin_config;
} hsu_gpio_handle_ctx_type;

/*=============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/


/*=============================================================================
FUNCTION    hsu_platform_init
=============================================================================*/
/**
Configures the Platform specific items which includes GPIO setting, Composition etc.

@dependencies
The configuration happens based on Platform and CHIP information. These value should be configured properly.

@sideeffects
None.

@return
None
**/
void hsu_platform_init(void);

/*=============================================================================
FUNCTION    hsu_platform_get_info
=============================================================================*/
/**
Returns platform information which includes GPIO setting, Composition etc.

@dependencies
The configuration happens based on Platform and CHIP information. These value should be configured properly.

@sideeffects
None.

@return
Platform info data structure.
**/
hsu_platform_info_type const * hsu_platform_get_info(void);


/*=============================================================================
FUNCTION    hsu_platform_get_counter_xo_shutdown
=============================================================================*/
/**
Returns platform's XO Shutdown without VDD MIN count

@dependencies
None.

@sideeffects
None.

@return
XO Shutdown without VDD MIN count
**/
uint32 hsu_platform_get_counter_xo_shutdown(void);

/*=============================================================================
FUNCTION    hsu_platform_get_counter_xosd_with_vddmin
=============================================================================*/
/**
Returns platform's XO Shutdown with VDD MIN count

@dependencies
None.

@sideeffects
None.

@return
XO Shutdown with VDD MIN count
**/
uint32 hsu_platform_get_counter_xosd_with_vddmin(void);

/*=============================================================================
FUNCTION    hsu_platform_log_xosd_and_vddmin_info
=============================================================================*/
/**
Prints platform's XO Shutdown and VDD MIN counts in internal log.

@dependencies
None.

@sideeffects
None.

@return
Platform info data structure.
**/
void hsu_platform_log_xosd_and_vddmin_info(void);

#endif /* HSU_PLATFORM_H */

