#ifndef CHIPINFO_H
#define CHIPINFO_H
/*===========================================================================

                         Chipinfo Header File

DESCRIPTION
  This header file contains the public API for the chipinfo driver.

      Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rpm.bf/2.1.3/core/api/systemdrivers/Chipinfo.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/17/12   pbitra  Initial version.
01/24/13   pbitra  Added support for Chipinfo_GetVersion

===========================================================================*/

#include "comdef.h"     /* Common Definitions and Types */


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif
/**
Chip Family type.  The family type of the chip we are running on.
Each family may include multiple chip ids.
*/
typedef enum
{ 
  CHIPINFO_FAMILY_UNKNOWN    = 0,
  CHIPINFO_FAMILY_MSM8974    = 32,
  CHIPINFO_FAMILY_MDM9x25    = 35,
  CHIPINFO_FAMILY_MSM8x10    = 38,
  CHIPINFO_FAMILY_MPQ8092    = 39,
  CHIPINFO_FAMILY_MSM8x26    = 40,
  CHIPINFO_FAMILY_APQ8x94    = 43,
  CHIPINFO_FAMILY_MSM8x32    = 44,
  CHIPINFO_FAMILY_MDM9x35    = 45,
  CHIPINFO_FAMILY_MSM8916    = 52,
  CHIPINFO_FAMILY_MSM8936    = 53,
  CHIPINFO_FAMILY_MDM9x45    = 54,
  CHIPINFO_FAMILY_APQ8096    = 55,
  CHIPINFO_FAMILY_MSM8996    = 56,
  CHIPINFO_FAMILY_MSM8992    = 57,
  CHIPINFO_FAMILY_MSM8909    = 58,
  CHIPINFO_FAMILY_FSM90xx    = 59,
  CHIPINFO_FAMILY_MSM8952    = 60,
  CHIPINFO_FAMILY_QDF2432    = 61,
  CHIPINFO_FAMILY_MSM8929    = 62,
  CHIPINFO_FAMILY_MSM8956    = 63,
  CHIPINFO_FAMILY_MSM8976    = CHIPINFO_FAMILY_MSM8956,
  CHIPINFO_FAMILY_QCA961x    = 64,
  CHIPINFO_FAMILY_IPQ40xx    = CHIPINFO_FAMILY_QCA961x,
  CHIPINFO_FAMILY_MDM9x55    = 65,
  CHIPINFO_FAMILY_MDM9x07    = 66,
  CHIPINFO_FAMILY_MSM8998    = 67,
  CHIPINFO_FAMILY_MSM8953    = 68,
  CHIPINFO_FAMILY_MSM8993    = 69,
  CHIPINFO_FAMILY_MSM8937    = 70,
  CHIPINFO_FAMILY_MSM8917    = 71,
  CHIPINFO_FAMILY_MSM8996SG  = 72,
  CHIPINFO_FAMILY_MSM8997    = 73,
  CHIPINFO_FAMILY_MSM8940    = 74,
  CHIPINFO_FAMILY_MDM9x65    = 75,
  CHIPINFO_FAMILY_SDM660     = 76,
  CHIPINFO_FAMILY_SDM630     = 77,
  CHIPINFO_FAMILY_MSM8920    = 78,
  CHIPINFO_FAMILY_SDM845     = 79,
  CHIPINFO_FAMILY_IPQ807x    = 80,
  CHIPINFO_FAMILY_SDM830     = 81,
  CHIPINFO_FAMILY_SDX50M     = 82,
  CHIPINFO_FAMILY_QCA6290    = 83,
  CHIPINFO_FAMILY_SDX24      = 84,
  CHIPINFO_FAMILY_SDM670     = 85,
  CHIPINFO_FAMILY_SDM855     = 86,
  CHIPINFO_FAMILY_SDM1000    = 87,
  CHIPINFO_FAMILY_SDX20M     = CHIPINFO_FAMILY_MDM9x65,
  CHIPINFO_FAMILY_QCS605     = 88,
  CHIPINFO_FAMILY_SDM632     = 89,
  CHIPINFO_FAMILY_QCS6200    = 90,
  CHIPINFO_FAMILY_SDM439     = 91,
  CHIPINFO_FAMILY_SDM640     = 92,
  CHIPINFO_FAMILY_QCS405     = CHIPINFO_FAMILY_QCS6200,
  CHIPINFO_FAMILY_SDA865     = 93,
  CHIPINFO_FAMILY_SDX55      = 94,
  CHIPINFO_FAMILY_QCA639x    = 95,
  CHIPINFO_FAMILY_SM8150	 = CHIPINFO_FAMILY_SDM855,
  CHIPINFO_FAMILY_SM8250	 = CHIPINFO_FAMILY_SDA865,
  CHIPINFO_FAMILY_SCX8180	 = CHIPINFO_FAMILY_SDM1000,
  CHIPINFO_FAMILY_SM7150	 = 96,
  CHIPINFO_FAMILY_SM6150	 = CHIPINFO_FAMILY_SDM640,
  CHIPINFO_FAMILY_SXR1130	 = 97,
  CHIPINFO_FAMILY_QCN7605	 = 98,
  CHIPINFO_FAMILY_SC8180X	 = CHIPINFO_FAMILY_SCX8180,
  CHIPINFO_FAMILY_MDM9205	 = 99,   

  CHIPINFO_NUM_FAMILIES      = 100,
  CHIPINFO_FAMILY_32BITS     = 0x7FFFFFF
} ChipInfoFamilyType;

/**
 * Chip version type with major number in the upper 16 bits and minor
 * number in the lower 16 bits.  For example:
 *   1.0 -> 0x00010000
 *   2.3 -> 0x00020003
 * The CHIPINFO_VERSION macro should be used to generate the 
 * appropriate comparison value as the format is not guaranteed 
 * to remain unchanged. 
 */
typedef uint32 ChipInfoVersionType;

/*===========================================================================

                      MACROS

===========================================================================*/
/**
 * Macro to generate the hardware version number from a given major
 * and minor number.  For example to check if this is at least version 1.2:
 *   ChipInfo_GetVersion(h, &version);
 *   if (version >= CHIPINFO_VERSION(1,2)) ...
 */
#define CHIPINFO_VERSION(major, minor)  (((major) << 16) | (minor))

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*==========================================================================

  FUNCTION      CHIPINFO_GETFAMILY

  DESCRIPTION   This function retrieves the Chip Family
                from the SOC_HW_VERSION register.

  PARAMETERS    None

  DEPENDENCIES  None.

  RETURN VALUE  Chip Family

  SIDE EFFECTS  None.

==========================================================================*/

ChipInfoFamilyType Chipinfo_GetFamily (void);


/*==========================================================================

  FUNCTION      CHIPINFO_GETVERSION

  DESCRIPTION   This function retrieves the Chip Version
                from the SOC_HW_VERSION register.

  PARAMETERS    None

  DEPENDENCIES  None.

  RETURN VALUE  Chip Version

  SIDE EFFECTS  None.

==========================================================================*/
ChipInfoVersionType Chipinfo_GetVersion (void);

#ifdef __cplusplus
}
#endif

#endif /* CHIPINFO_H */

