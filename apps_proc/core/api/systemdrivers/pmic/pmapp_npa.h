#ifndef PMAPP_NPA_H
#define PMAPP_NPA_H

/*! \file  pmapp_npa.h
 *  
 *  \brief  File Contains the PMIC NPA CMI Code
 *  \details  This file contains the needed definition and enum for PMIC NPA layer.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Locked Version: pmd9x45_06_26_2015_v1
 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2015 Qualcomm Technologies, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/core.tx/6.0/api/systemdrivers/pmic/pmapp_npa.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

/*===========================================================================

                        DEFINITION

===========================================================================*/

#define PMIC_NPA_GROUP_ID_CODEC "/pmic/client/codec"
#define PMIC_NPA_GROUP_ID_HKADC "/pmic/client/hkadc"
#define PMIC_NPA_GROUP_ID_PCIE "/pmic/client/pcie"
#define PMIC_NPA_GROUP_ID_PLL_TS "/pmic/client/pll_ts"
#define PMIC_NPA_GROUP_ID_RAIL_CX "/pmic/client/rail_cx"
#define PMIC_NPA_GROUP_ID_RAIL_MX "/pmic/client/rail_mx"
#define PMIC_NPA_GROUP_ID_SD "/pmic/client/sd"
#define PMIC_NPA_GROUP_ID_THERMAL "/pmic/client/thermal"
#define PMIC_NPA_GROUP_ID_USB_HS1 "/pmic/client/usb_hs1"
#define PMIC_NPA_GROUP_ID_USB_HSIC "/pmic/client/usb_hsic"
#define PMIC_NPA_GROUP_ID_USB_SS1 "/pmic/client/usb_ss1"
#define PMIC_NPA_GROUP_ID_WCN_48MHZ "/pmic/client/wcn1"
#define PMIC_NPA_GROUP_ID_WCN1 "/pmic/client/wcn1"

/*===========================================================================

                        ENUMERATION

===========================================================================*/

enum
{
   PMIC_NPA_MODE_ID_AUDIO_STANDBY,
   PMIC_NPA_MODE_ID_AUDIO_ACTIVE_LOW,
   PMIC_NPA_MODE_ID_AUDIO_ACTIVE_MED,
   PMIC_NPA_MODE_ID_AUDIO_ACTIVE_HIGH,
   PMIC_NPA_MODE_ID_AUDIO_MAX,
};

enum
{
   PMIC_NPA_MODE_ID_GENERIC_STANDBY,
   PMIC_NPA_MODE_ID_GENERIC_ACTIVE,
   PMIC_NPA_MODE_ID_GENERIC_MAX,
};

enum
{
   PMIC_NPA_MODE_ID_PCIE_OFF,
   PMIC_NPA_MODE_ID_PCIE_SUSPEND,
   PMIC_NPA_MODE_ID_PCIE_ACTIVE_NOMINAL,
   PMIC_NPA_MODE_ID_PCIE_ACTIVE_SVS,
   PMIC_NPA_MODE_ID_PCIE_ACTIVE_TURBO,
   PMIC_NPA_MODE_ID_PCIE_MAX,
};

enum
{
   PMIC_NPA_MODE_ID_CORE_RAIL_OFF,
   PMIC_NPA_MODE_ID_CORE_RAIL_RETENTION,
   PMIC_NPA_MODE_ID_CORE_RAIL_LOW_MINUS,
   PMIC_NPA_MODE_ID_CORE_RAIL_LOW,
   PMIC_NPA_MODE_ID_CORE_RAIL_NOMINAL,
   PMIC_NPA_MODE_ID_CORE_RAIL_NOMINAL_PLUS,
   PMIC_NPA_MODE_ID_CORE_RAIL_TURBO,
   PMIC_NPA_MODE_ID_CORE_RAIL_MAX,
};

enum
{
   PMIC_NPA_MODE_ID_HKADC_OFF = 0,
   PMIC_NPA_MODE_ID_HKADC_ACTIVE = 1,
   PMIC_NPA_MODE_ID_HKADC_MAX = 2,
};

enum
{
   PMIC_NPA_MODE_ID_SD_MODE_OFF,
   PMIC_NPA_MODE_ID_SD_MODE_1,
   PMIC_NPA_MODE_ID_SD_MODE_2,
   PMIC_NPA_MODE_ID_SD_MODE_MAX,
};

enum
{
   PMIC_NPA_MODE_ID_TEMP_MODE_NORMAL,
   PMIC_NPA_MODE_ID_TEMP_MODE_HIGH,
   PMIC_NPA_MODE_ID_TEMP_MODE_LOW,
   PMIC_NPA_MODE_ID_TEMP_MODE_MAX,
};

enum
{
   PMIC_NPA_MODE_ID_USB_HS_POWER_OFF,
   PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_0,
   PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_1,
   PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_2,
   PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_3,
   PMIC_NPA_MODE_ID_USB_HS_HOST_LPM_0,
   PMIC_NPA_MODE_ID_USB_HS_HOST_LPM_1,
   PMIC_NPA_MODE_ID_USB_HS_HOST_LPM_2,
   PMIC_NPA_MODE_ID_USB_HS_HOST_LPM_3,
   PMIC_NPA_MODE_ID_USB_HS_MAX,
};

enum
{
   PMIC_NPA_MODE_ID_USB_OFF,
   PMIC_NPA_MODE_ID_USB_PERPH_SUSPEND,
   PMIC_NPA_MODE_ID_USB_PERPH_ACTIVE,
   PMIC_NPA_MODE_ID_USB_HOST_SUSPEND,
   PMIC_NPA_MODE_ID_USB_HOST_ACTIVE,
   PMIC_NPA_MODE_ID_USB_MAX,
};

enum
{
   PMIC_NPA_MODE_ID_WCN_MODE_0,
   PMIC_NPA_MODE_ID_WCN_MODE_1,
   PMIC_NPA_MODE_ID_WCN_MODE_2,
   PMIC_NPA_MODE_ID_WCN_MODE_3,
   PMIC_NPA_MODE_ID_WCN_MODE_4,
   PMIC_NPA_MODE_ID_WCN_MODE_5,
   PMIC_NPA_MODE_ID_WCN_MODE_MAX,
};

#endif // PMAPP_NPA_H

