/*==============================================================================

                 HSU_PLATFORM_9X15. C


GENERAL DESCRIPTION
  Platform Initialization based on platform and chip info.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hsu_platform_init should be called before using the API defined in this file.

==============================================================================*/
/*=============================================================================
Copyright (c) 2006-2012,2022 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/
/*=============================================================================

                            EDIT HISTORY FOR MODULE
  
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
  
$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/9205/hsu_platform.c#3 $
$DateTime: 2022/08/19 01:35:30 $
    
when     who  what, where, why
-------- ---  ---------------------------------------------------------
07/11/12 ag  Initial version
=============================================================================*/
/*==============================================================================
                        INCLUDE FILES
==============================================================================*/

#include "customer.h"
#include "hsu_platform.h"
#include "DDITlmm.h"
#include "DDIChipInfo.h"
#include "DALDeviceId.h"
#include "DDIPlatformInfo.h"
#include "hsu_common_int.h"
#include "hsu_log.h"
#include "hsu_conf_sel_comp_utils.h"
#include "hsu_hwio_dependency.h"

/*=============================================================================
  Forward Declarations
=============================================================================*/

/*=============================================================================
  Constants
=============================================================================*/

static hsu_platform_id_enum hsu_platform_index = HSU_PLATFORM_STANDALONE;

/*=============================================================================
  Macros
=============================================================================*/
// These addresses are target specific
#define HSU_PLATFORM_STAT_XO_SHUTDOWN             (0x0029DFA0)
#define HSU_PLATFORM_STAT_XOSD_WITH_VDDMIN        (0x0029DFD0)
/*=============================================================================
  Typedefs
=============================================================================*/

// One to one mapping in XML file required for this
// Definition provided from power team, not available on APPS processor
typedef struct
{
  uint32 stat_type;
  uint32 count;
  uint64 last_entered_at;
  uint64 last_exited_at;
  uint64 accumulated_duration;
  uint32 client_votes;
  uint32 reserved[3];
} hsu_platform_xosd_vddmin_stats_type;

/*==============================================================================
  Variables
==============================================================================*/

static hsu_platform_info_type hsu_platform_details[HSU_NUM_OF_PLATFORM_IDS]=
{
    {
      /* hsu_platform_id                */  HSU_PLATFORM_STANDALONE,
      /* nVer                           */  HSU_VERSION_NOT_CONFIGURED,
      /* gpio_remote_wakeup             */  HSU_GPIO_NOT_CONFIGURED,
      /* gpio_remote_wakeup             */  HSU_GPIO_NOT_CONFIGURED,
      /* gpio_apq2mdm_vdd_min           */  HSU_GPIO_NOT_CONFIGURED,
      /* gpio_mdm2apq_vdd_min           */  HSU_GPIO_NOT_CONFIGURED,
      /* default_composition            */  #ifdef FEATURE_TX_SP_AWARE_TRACKER
                                                EFS_HSU_DIAG_SER3_SER5_MDM_NET1,  //Tracker 2.1 flavour
                                            #else
                                                EFS_HSU_DIAG_QDSS_MDM_NET1,  //EFS_HSU_DIAG_NMEA_QDSS_MDM_NET1,
                                            #endif
      /* is hsic enabled                */  FALSE, 
      /* is hsusb enabled               */  FALSE, 
      /* is diag over hsusb             */  TRUE,
      /* is_ss_phy_disabled             */  FALSE,
      /* hsu_phy_id_enum                */  HSU_NUM_PHY_IDS,
      /* is_flashless_boot              */  FALSE,
    },


    {
      /* hsu_platform_id                */  HSU_PLATFORM_M_FUSION,
      /* nVer                           */  HSU_VERSION_NOT_CONFIGURED,
      /* gpio_remote_wakeup             */  HSU_GPIO_NOT_CONFIGURED,
      /* gpio_remote_wakeup             */  HSU_GPIO_NOT_CONFIGURED,
      /* gpio_apq2mdm_vdd_min           */  HSU_GPIO_NOT_CONFIGURED,
      /* gpio_mdm2apq_vdd_min           */  HSU_GPIO_NOT_CONFIGURED,
      /* default_composition            */  EFS_HSU_DIAG_NMEA_MDM,
      /* is hsic enabled                */  FALSE, 
      /* is hsusb enabled               */  FALSE, 
      /* is diag over hsusb             */  FALSE,
      /* is_ss_phy_disabled             */  TRUE,
      /* hsu_phy_id_enum                */  HSU_NUM_PHY_IDS,
      /* is_flashless_boot              */  FALSE,
    },
      
    {
      /* hsu_platform_id                */  HSU_PLATFORM_LA_FUSION,
      /* nVer                           */  HSU_VERSION_NOT_CONFIGURED,
      /* gpio_remote_wakeup             */  HSU_LA_AP2MDM_HSIC_READY,
      /* gpio_remote_wakeup             */  HSU_LA_MDM2AP_REMOTE_WAKEUP,
      /* gpio_apq2mdm_vdd_min           */  HSU_LA_AP2MDM_VDD_MIN,
      /* gpio_mdm2apq_vdd_min           */  HSU_LA_MDM2AP_VDD_MIN,
      /* default_composition            */  EFS_HSU_DIAG_DCI_SER3_SER4_QDSS_MDM_CSVT_IPCR,
      /* is hsic enabled                */  TRUE, 
      /* is hsusb enabled               */  FALSE, 
      /* is diag over hsusb             */  FALSE,
      /* is_ss_phy_disabled             */  FALSE,
      /* hsu_phy_id_enum                */  HSU_NUM_PHY_IDS,
      /* is_flashless_boot              */  FALSE,
    },

    {
      /* hsu_platform_id                */  HSU_PLATFORM_GOBI_FUSION,
      /* nVer                           */  HSU_VERSION_NOT_CONFIGURED,
      /* gpio_remote_wakeup             */  HSU_GOBI_AP2MDM_HSIC_READY,
      /* gpio_remote_wakeup             */  HSU_GOBI_MDM2AP_REMOTE_WAKEUP,
      /* gpio_apq2mdm_vdd_min           */  HSU_GOBI_AP2MDM_VDD_MIN,
      /* gpio_mdm2apq_vdd_min           */  HSU_GOBI_MDM2AP_VDD_MIN,
      /* default_composition            */  EFS_HSU_DIAG_SER3_SER4_MDM_MBIM,
      /* is hsic enabled                */  TRUE, 
      /* is hsusb enabled               */  TRUE, 
      /* is diag over hsusb             */  TRUE,
      /* is_ss_phy_disabled             */  FALSE,
      /* hsu_phy_id_enum                */  HSU_NUM_PHY_IDS,
      /* is_flashless_boot              */  FALSE,
    },

    {
      /* hsu_platform_id                */  HSU_PLATFORM_RUMI,
      /* nVer                           */  HSU_VERSION_NOT_CONFIGURED,
      /* gpio_remote_wakeup             */  HSU_GPIO_NOT_CONFIGURED,
      /* gpio_remote_wakeup             */  HSU_GPIO_NOT_CONFIGURED,
      /* gpio_apq2mdm_vdd_min           */  HSU_GPIO_NOT_CONFIGURED,
      /* gpio_mdm2apq_vdd_min           */  HSU_GPIO_NOT_CONFIGURED,
//      /* default_composition            */  EFS_HSU_DIAG_SER4_QDSS_MDM_NET1,
      /* default_composition            */  EFS_HSU_DIAG_NMEA_MDM,
      /* is hsic enabled                */  FALSE, 
      /* is hsusb enabled               */  TRUE, 
      /* is diag over hsusb             */  TRUE,
      /* is_ss_phy_disabled             */  TRUE,
      /* hsu_phy_id_enum                */  HSU_NUM_PHY_IDS,
      /* is_flashless_boot              */  FALSE,
    },
      
    {
      /* hsu_platform_id                */  HSU_PLATFORM_VIRTIO,
      /* nVer                           */  HSU_VERSION_NOT_CONFIGURED,
      /* gpio_remote_wakeup             */  HSU_GPIO_NOT_CONFIGURED,
      /* gpio_remote_wakeup             */  HSU_GPIO_NOT_CONFIGURED,
      /* gpio_apq2mdm_vdd_min           */  HSU_GPIO_NOT_CONFIGURED,
      /* gpio_mdm2apq_vdd_min           */  HSU_GPIO_NOT_CONFIGURED,
      /* default_composition            */  EFS_HSU_DIAG_NMEA_MDM_NET,
      /* is hsic enabled                */  FALSE, 
      /* is hsusb enabled               */  TRUE, 
      /* is diag over hsusb             */  TRUE,
      /* is_ss_phy_disabled             */  FALSE,
      /* hsu_phy_id_enum                */  HSU_NUM_PHY_IDS,
      /* is_flashless_boot              */  FALSE,
    },
};

/*==============================================================================
                          FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
static void hsu_platform_chip_info(void)
{
  DALResult eResult;
  DalDeviceHandle *phChipInfo = NULL;
  static DalChipInfoVersionType hsu_nVer_info;
  static DalChipInfoFamilyType hsu_eFamily_info;
  hsu_phy_id_enum phy_id;

  eResult = DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo);
  HSU_ASSERT (eResult == DAL_SUCCESS);
   
  eResult = DalDevice_Open(phChipInfo, DAL_OPEN_SHARED);
  HSU_ASSERT (eResult == DAL_SUCCESS);

  eResult = DalChipInfo_GetChipFamily(phChipInfo, &hsu_eFamily_info);
  HSU_ASSERT (eResult == DAL_SUCCESS);

  eResult = DalChipInfo_GetChipVersion(phChipInfo, &hsu_nVer_info);
  HSU_ASSERT (eResult == DAL_SUCCESS);

  eResult = DalDevice_Close (phChipInfo);
  HSU_ASSERT (eResult == DAL_SUCCESS);

  hsu_platform_details[hsu_platform_index].nVer = hsu_nVer_info;

  if (hsu_nVer_info < DALCHIPINFO_VERSION(2,0))
  {
    // Assumed value for PHY ID detection
    phy_id = HSU_PHY_AT;
  }
  else
  {
#ifndef FEATURE_HS_USB_PRE_SILICON_BU
//    phy_id = (hsu_phy_id_enum)((HWIO_QFPROM_RAW_PTE_MSB_INM(HSU_PHY_ID_MASK) >> HSU_PHY_ID_SHFT));
    // error checking is done in the actual PHY init
    phy_id = HSU_PHY_AT;
#else
    // Assumed value for PHY ID detection
    phy_id = HSU_PHY_AT;
#endif // FEATURE_HS_USB_PRE_SILICON_BU
  }

  hsu_platform_details[hsu_platform_index].phy_id = phy_id;
}

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
void hsu_platform_init(void)
{
  DALResult eResult;
  DalDeviceHandle *phPFormInfo = NULL;
  static DalPlatformInfoPlatformInfoType hsu_platform_info;
  
#ifndef FEATURE_HS_USB_PRE_SILICON_BU
//  uint32 value = 0;
#endif // FEATURE_HS_USB_PRE_SILICON_BU

  eResult = DAL_DeviceAttach(DALDEVICEID_PLATFORMINFO, &phPFormInfo);
  HSU_ASSERT (eResult == DAL_SUCCESS);

  eResult = DalDevice_Open (phPFormInfo, DAL_OPEN_SHARED);
  HSU_ASSERT (eResult == DAL_SUCCESS);

  eResult = DalPlatformInfo_GetPlatformInfo (phPFormInfo, &hsu_platform_info );
  HSU_ASSERT (eResult == DAL_SUCCESS);

  hsu_platform_index = HSU_PLATFORM_STANDALONE;

  eResult = DalDevice_Close (phPFormInfo);
  HSU_ASSERT (eResult == DAL_SUCCESS);

  switch(hsu_platform_info.platform)
  {
    case DALPLATFORMINFO_TYPE_RUMI:     //switch(hsu_platform_info.platform)
    {
      hsu_platform_index = HSU_PLATFORM_RUMI;
      break;
    }
    case DALPLATFORMINFO_TYPE_VIRTIO:   //switch(hsu_platform_info.platform)
    {
      hsu_platform_index = HSU_PLATFORM_VIRTIO;
      break;
    }
      
    default:    //switch(hsu_platform_info.platform)
    {
      break;
    }
  }  

  hsu_platform_chip_info();

  hsu_conf_sel_comp_utils_update_default_comp_nv_value
    (hsu_platform_details[hsu_platform_index].default_composition);

}


/*=============================================================================
FUNCTION    hsu_platform_get_info
=============================================================================*/
hsu_platform_info_type const * hsu_platform_get_info(void)
{
  return &hsu_platform_details[hsu_platform_index];
}

/*=============================================================================
FUNCTION    hsu_platform_get_counter_xo_shutdown
=============================================================================*/
uint32 hsu_platform_get_counter_xo_shutdown(void)
{

  hsu_platform_xosd_vddmin_stats_type *xosd_stat_ptr = 
    (hsu_platform_xosd_vddmin_stats_type*)HSU_PLATFORM_STAT_XO_SHUTDOWN;
  HSU_ULOG_1(NO_MSG, BUS_LOG, "XO_shutdown, no_vddmin_count %u", xosd_stat_ptr->count);

  return xosd_stat_ptr->count;
}

/*=============================================================================
FUNCTION    hsu_platform_get_counter_xosd_with_vddmin
=============================================================================*/
uint32 hsu_platform_get_counter_xosd_with_vddmin(void)
{

  hsu_platform_xosd_vddmin_stats_type *vddmin_stat_ptr = 
    (hsu_platform_xosd_vddmin_stats_type*)HSU_PLATFORM_STAT_XOSD_WITH_VDDMIN;  

  return vddmin_stat_ptr->count;
}

/*=============================================================================
FUNCTION    hsu_platform_log_xosd_and_vddmin_info
=============================================================================*/
void hsu_platform_log_xosd_and_vddmin_info(void)
{

  hsu_platform_xosd_vddmin_stats_type *xosd_stat_ptr = 
    (hsu_platform_xosd_vddmin_stats_type*)HSU_PLATFORM_STAT_XO_SHUTDOWN;

  hsu_platform_xosd_vddmin_stats_type *vddmin_stat_ptr = 
    (hsu_platform_xosd_vddmin_stats_type*)HSU_PLATFORM_STAT_XOSD_WITH_VDDMIN;

  if (xosd_stat_ptr->count)
  {
    HSU_ULOG_1(NO_MSG, BUS_LOG, "XO_shutdown, no_vddmin_count %u", xosd_stat_ptr->count);
  }

  if (vddmin_stat_ptr->count)
  {
    HSU_ULOG_1(NO_MSG, BUS_LOG, "XO_shutdown, with_vddmin_count %u", vddmin_stat_ptr->count);
  }

}
