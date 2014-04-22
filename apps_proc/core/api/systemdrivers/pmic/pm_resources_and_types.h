#ifndef PM_RESOURCES_AND_TYPES__H
#define PM_RESOURCES_AND_TYPES__H
/** @file pm_resources_and_types.h
*/
/* This file contains enumerations with resource names for different
 *  module types that should be used by external clients when accessing
 *   the resources that are required
 
 Copyright 2010-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/api/systemdrivers/pmic/pm_resources_and_types.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/12   hw      moving some npa type to this file so we don't need to include pmapp_npa.h
09/29/11   hs      Added CLK resource IDs.
09/14/11   dy      Move UICC resource IDs to this file
04/25/11   wra     Adding Battery Temperature Management BTM, LPG, more LVS, ADC_ARB,
                    OVP & INTerrupt channel enumeration needed for PM8921
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
07/05/10   wra     Modified Header includes to accomodate MSM8660
=============================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "DALStdDef.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/** @addtogroup pm_resources_and_types
@{ */
/**
 * PMIC power module voltage level
 */
typedef uint32 pm_volt_level_type;

typedef enum
{
   PM_OFF,
   PM_ON,
   PM_INVALID
}pm_on_off_type;

typedef enum 
{
    PM_SW_MODE_LPM     = 0,	/**< Low power mode	 */
    PM_SW_MODE_BYPASS  = 1,	/**< Bypass mode,       bit 5 */
    PM_SW_MODE_AUTO    = 2,     /**< Auto mode,         bit 6 */
    PM_SW_MODE_NPM     = 4,	/**< Normal power mode, bit 7	*/
    PM_SW_MODE_RETENTION,      /**< Retention mode. */
    PM_SW_MODE_INVALID
}pm_sw_mode_type;

/** 
  @struct peripheral_info_type
  @brief Contains Peripheral Information such as Type, Subtype, 
         Analog and Digital Major Versions. 
 */
typedef struct 
{
    uint16 base_address;
    uint8  peripheral_type;
    uint8  peripheral_subtype;
    uint8  analog_major_version;
    uint8  analog_minor_version;
    uint8  digital_major_version;
    uint8  digital_minor_version;
}peripheral_info_type;


typedef enum
{
    PM_PIN_STATUS_LOW,
    PM_PIN_STATUS_HIGH
}pm_pin_status;

/** @} */ /* end_addtogroup pm_resources_and_types */

#endif /* PM_RESOURCES_AND_TYPES__H */

