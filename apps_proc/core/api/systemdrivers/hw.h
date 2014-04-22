#ifndef HW_H
#define HW_H
/*============================================================================

FILE:      hw.h

DESCRIPTION:
  This file contains the legacy code from the hw driver that has yet to
  be moved into a DAL.
  The functions and types in this file are generally obselete - in
  particular for all chip info functionality (version, id, chipset etc)
  please use the ChipInfo DAL, API in DDIChipInfo.h

PUBLIC CLASSES:
  hw_init
  hw_reset
  hw_power_off
  hw_msm_id          (obselete)
  hw_raw_id          (obselete)
  hw_msm_version     (obselete)
  hw_raw_version     (obselete)
  hw_chipset         (obselete)
  hw_msm_id_string   (obselete)
  hw_partnum_version (obselete)
  hw_init_smem
  hw_init_hwio

============================================================================
        Copyright (c) 2016 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
============================================================================
  $Header: //components/rel/core.tx/6.0/api/systemdrivers/hw.h#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "comdef.h"
#include "DDIChipInfo.h"
#include "DDIPlatformInfo.h"



/*=========================================================================
      Type Definitions
==========================================================================*/


/*
 * hw_msm_id_type
 *
 * MSM identification type.  Any new ids must be added to the end.
 * 
 * --- OBSELETE, see DDIChipInfo.h ---
 */
typedef enum
{
  HW_UNKNOWN_MSM   = DALCHIPINFO_ID_UNKNOWN,
  HW_MDM1000       = DALCHIPINFO_ID_MDM1000,
  HW_ESM6235       = DALCHIPINFO_ID_ESM6235 ,
  HW_QSC6240       = DALCHIPINFO_ID_QSC6240,
  HW_MSM6245       = DALCHIPINFO_ID_MSM6245,
  HW_MSM6255       = DALCHIPINFO_ID_MSM6255,
  HW_MSM6255A      = DALCHIPINFO_ID_MSM6255A,
  HW_MSM6260       = DALCHIPINFO_ID_MSM6260,
  HW_MSM6246       = DALCHIPINFO_ID_MSM6246,
  HW_QSC6270       = DALCHIPINFO_ID_QSC6270,
  HW_MSM6280       = DALCHIPINFO_ID_MSM6280,
  HW_MSM6290       = DALCHIPINFO_ID_MSM6290,
  HW_MSM7200       = DALCHIPINFO_ID_MSM7200,
  HW_MSM7201       = DALCHIPINFO_ID_MSM7201,
  HW_ESM7205       = DALCHIPINFO_ID_ESM7205,
  HW_ESM7206       = DALCHIPINFO_ID_ESM7206,
  HW_MSM7200A      = DALCHIPINFO_ID_MSM7200A,
  HW_MSM7201A      = DALCHIPINFO_ID_MSM7201A,
  HW_ESM7205A      = DALCHIPINFO_ID_ESM7205A,
  HW_ESM7206A      = DALCHIPINFO_ID_ESM7206A,
  HW_ESM7225       = DALCHIPINFO_ID_ESM7225,
  HW_MSM7225       = DALCHIPINFO_ID_MSM7225,
  HW_MSM7500       = DALCHIPINFO_ID_MSM7500,
  HW_MSM7500A      = DALCHIPINFO_ID_MSM7500A,
  HW_MSM7525       = DALCHIPINFO_ID_MSM7525,
  HW_MSM7600       = DALCHIPINFO_ID_MSM7600,
  HW_MSM7601       = DALCHIPINFO_ID_MSM7601,
  HW_MSM7625       = DALCHIPINFO_ID_MSM7625,
  HW_MSM7800       = DALCHIPINFO_ID_MSM7800,
  HW_MDM8200       = DALCHIPINFO_ID_MDM8200,
  HW_QSD8650       = DALCHIPINFO_ID_QSD8650,
  HW_MDM8900       = DALCHIPINFO_ID_MDM8900,
  HW_QST1000       = DALCHIPINFO_ID_QST1000,
  HW_QST1005       = DALCHIPINFO_ID_QST1005,
  HW_QST1100       = DALCHIPINFO_ID_QST1100,
  HW_QST1105       = DALCHIPINFO_ID_QST1105,
  HW_QST1500       = DALCHIPINFO_ID_QST1500,
  HW_QST1600       = DALCHIPINFO_ID_QST1600,
  HW_QST1700       = DALCHIPINFO_ID_QST1700,
  HW_QSD8250       = DALCHIPINFO_ID_QSD8250,
  HW_QSD8550       = DALCHIPINFO_ID_QSD8550,
  HW_QSD8850       = DALCHIPINFO_ID_QSD8850,
  HW_MDM2000       = DALCHIPINFO_ID_MDM2000,
  HW_MSM7227       = DALCHIPINFO_ID_MSM7227,
  HW_MSM7627       = DALCHIPINFO_ID_MSM7627,
  HW_ESC6270       = DALCHIPINFO_ID_ESC6270,
  HW_ESC6240       = DALCHIPINFO_ID_ESC6240,
  HW_MSM7630       = DALCHIPINFO_ID_MSM7630,
  HW_MDM8220       = DALCHIPINFO_ID_MDM8220,
  HW_MDM9200       = DALCHIPINFO_ID_MDM9200,
  HW_MDM9600       = DALCHIPINFO_ID_MDM9600,
  HW_MSM_ID_32BITS = 0xFFFFFFF
} hw_msm_id_type;


/*
 * hw_msm_version_type
 *
 * MSM version type with major number in the upper 16 bits and minor
 * number in the lower 16 bits.  For example:
 *   1.0 -> 0x00010000
 *   2.3 -> 0x00020003
 * The HW_VERSION macro should be used to generate the appropriate
 * comparison value.
 */
typedef uint32 hw_msm_version_type;


/*
 * HW_VERSION
 *
 * Macro to generate the hardware version number from a given major
 * and minor number.  Used when comparing against return value of
 * hw_version().  For example to check if this is at least version 1.2:
 *   if (hw_version() >= HW_VERSION(1,2)) ...
 */
#define HW_VERSION(major, minor)  DALCHIPINFO_VERSION(major, minor)


/*
 * hw_chipset_type
 *
 * The chipset type of the device we are running on.  Each chipset may
 * include multiple MSMs.
 *
 * --- OBSELETE, see DDIChipInfo.h ---
 */
typedef enum
{
  HW_UNKNOWN_CHIPSET = DALCHIPINFO_FAMILY_UNKNOWN,
  HW_CHIPSET_6246    = DALCHIPINFO_FAMILY_MSM6246,
  HW_CHIPSET_6260    = DALCHIPINFO_FAMILY_MSM6260,
  HW_CHIPSET_6270    = DALCHIPINFO_FAMILY_QSC6270,
  HW_CHIPSET_6280    = DALCHIPINFO_FAMILY_MSM6280,
  HW_CHIPSET_6290    = DALCHIPINFO_FAMILY_MSM6290,
  HW_CHIPSET_7200    = DALCHIPINFO_FAMILY_MSM7200,
  HW_CHIPSET_7500    = DALCHIPINFO_FAMILY_MSM7500,
  HW_CHIPSET_7600    = DALCHIPINFO_FAMILY_MSM7600,
  HW_CHIPSET_7625    = DALCHIPINFO_FAMILY_MSM7625,
  HW_CHIPSET_7630    = DALCHIPINFO_FAMILY_MSM7X30,
  HW_CHIPSET_7X30    = DALCHIPINFO_FAMILY_MSM7X30,
  HW_CHIPSET_7800    = DALCHIPINFO_FAMILY_MSM7800,
  HW_CHIPSET_8200    = DALCHIPINFO_FAMILY_MDM8200,
  HW_CHIPSET_8650    = DALCHIPINFO_FAMILY_QSD8650,
  HW_CHIPSET_7627    = DALCHIPINFO_FAMILY_MSM7627,
  HW_CHIPSET_6695    = DALCHIPINFO_FAMILY_QSC6695,
  HW_CHIPSET_9X00    = DALCHIPINFO_FAMILY_MDM9X00,
  HW_CHIPSET_8660    = DALCHIPINFO_FAMILY_MSM8X60,
  HW_NUM_CHIPSETS    = DALCHIPINFO_NUM_FAMILIES,
  HW_CHIPSET_32BITS  = 0xFFFFFFF
} hw_chipset_type;


/*
 * hw_platform_type
 *
 * The target type of the device we are running on.
 */
typedef enum
{
  HW_PLATFORM_UNKNOWN = 0,
  HW_PLATFORM_SURF    = 1,
  HW_PLATFORM_FFA     = 2,
  HW_PLATFORM_FLUID   = 3,
  HW_PLATFORM_FUSION  = 4,
  HW_PLATFORM_32BITS  = 0x7FFFFFFF
} hw_platform_type;


/*
 * hw_platform_info_type
 * This structure stores:
 * The target type of the device we are running on.
 * The version of the platform in use, in HW_VERSION format.
 */
typedef struct
{
  hw_platform_type  platform;
  uint32            version;
} hw_platform_info_type;


/*
 * hw_modem_type
 *
 * Masks used to indicate which modem hardware is supported on the platform.
 */
#define HW_MODEM_1X         DALCHIPINFO_MODEM_1X
#define HW_MODEM_EVDO       DALCHIPINFO_MODEM_EVDO
#define HW_MODEM_GSM        DALCHIPINFO_MODEM_GSM
#define HW_MODEM_UMTS       DALCHIPINFO_MODEM_UMTS
#define HW_MODEM_CGPS       DALCHIPINFO_MODEM_CGPS
#define HW_MODEM_EVDO_REVA  DALCHIPINFO_MODEM_EVDO_REVA
#define HW_MODEM_EVDO_REVB  DALCHIPINFO_MODEM_EVDO_REVB

typedef uint32 hw_modem_type;


/*
 * hw_rev_type
 *
 * Legacy data type containing raw HW part number and version.
 */
#define HW_REV_TYPE_V2
typedef struct
{
  uint16 partnum;
  uint8  version;
  uint32 hw_register;
} hw_rev_type;

typedef hw_rev_type hw_rev_type_int;


/*
 * hw_smem_type
 *
 * Shared memory data structure type for chip identifiers.
 */
#define HW_SMEM_FORMAT           5
#define HW_SMEM_BUILD_ID_LENGTH 32
typedef struct
{
  uint32                      format;
  hw_msm_id_type              msm_id;
  hw_msm_version_type         msm_version;
  char                        build_id[HW_SMEM_BUILD_ID_LENGTH];
  uint32                      raw_msm_id;
  uint32                      raw_msm_version;
  DalPlatformInfoPlatformType hw_platform;
  uint32                      hw_platform_version;
  DalChipInfoIdType           fused_chip;
} hw_smem_type;



/*=========================================================================
      Function Declarations
==========================================================================*/


/*==========================================================================

  FUNCTION      hw_init

  DESCRIPTION   Generic hardware initialization.

  PARAMETERS    config - Configuration read from NV.  Not used.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void hw_init
(
  uint32 config
);


/*==========================================================================

  FUNCTION      hw_power_off

  DESCRIPTION   Turns off the power to the device.

                *** Legacy, use ChipPower DAL: DalChipPower_PowerOff

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  Does not return.

  SIDE EFFECTS  None.

==========================================================================*/

void hw_power_off (void);


/*==========================================================================

  FUNCTION      hw_reset

  DESCRIPTION   Resets the device.

                *** Legacy, use ChipPower DAL: DalChipPower_Reset

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  Does not return.

  SIDE EFFECTS  None.

==========================================================================*/

void hw_reset (void);


/*==========================================================================

  FUNCTION      hw_msm_id

  DESCRIPTION   Returns the MSM identification.

                *** Legacy, use ChipInfo DAL: DalChipInfo_GetId

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  hw_msm_id_type - The MSM identifier.

  SIDE EFFECTS  None.

==========================================================================*/

hw_msm_id_type hw_msm_id (void);


/*==========================================================================

  FUNCTION      hw_msm_id_string

  DESCRIPTION   Returns the MSM identification as a string.

                *** Legacy, use ChipInfo DAL: DalChipInfo_GetIdString

  PARAMETERS    id - The MSM identifier (can be retrieved with hw_msm_id())

  DEPENDENCIES  None.

  RETURN VALUE  const char * - The MSM identifier string.

  SIDE EFFECTS  None.

==========================================================================*/

const char *hw_msm_id_string
(
  hw_msm_id_type id
);


/*==========================================================================

  FUNCTION      hw_msm_version

  DESCRIPTION   Returns the MSM version with major number in the upper
                16 bits and minor number in the lower 16 bits.  For
                example:
                1.0 -> 0x00010000
                2.3 -> 0x00020003
                The HW_VERSION macro should be used to generate the
                appropriate comparison value.

                *** Legacy, use ChipInfo DAL: DalChipInfo_GetVersion

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  The MSM revision in the above format.

  SIDE EFFECTS  None.

==========================================================================*/

hw_msm_version_type hw_msm_version (void);


/*==========================================================================

  FUNCTION      hw_raw_id

  DESCRIPTION   Returns the raw hardware identification.  Interpretation
                is target-specific.

                *** Legacy, use ChipInfo DAL: DalChipInfo_GetRawId

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  uint32 - The raw chip identifier.

  SIDE EFFECTS  None.

==========================================================================*/

uint32 hw_raw_id (void);


/*==========================================================================

  FUNCTION      hw_raw_version

  DESCRIPTION   Returns the raw hardware version.  Interpretation is
                target-specific.

                *** Legacy, use ChipInfo DAL: DalChipInfo_GetRawVersion

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  uint32 - The raw hardware revision.

  SIDE EFFECTS  None.

==========================================================================*/

uint32 hw_raw_version (void);


/*==========================================================================

  FUNCTION      hw_platform 

  DESCRIPTION   Returns the HW platform we are running on.

                *** Legacy, use PlatformInfo DAL: DalPlatformInfo_GetPlatform

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  hw_platform_type - The target.

  SIDE EFFECTS  None.

==========================================================================*/

hw_platform_type hw_platform (void);


/*==========================================================================

  FUNCTION      hw_platform_info

  DESCRIPTION   This function populates the client data pointer with the HW 
                platform and the platform version we are running on.
           
                *** Legacy, use PlatformInfo DAL: DalPlatformInfo_GetPlatformInfo

  PARAMETERS    hw_platform_info_type *info - Pointer to location to fill
                in with data.

  DEPENDENCIES  None.

  RETURN VALUE  None.
               
  SIDE EFFECTS  None.

==========================================================================*/

void hw_platform_info
(
  hw_platform_info_type *info
);


/*==========================================================================

  FUNCTION      hw_chipset

  DESCRIPTION   Returns the HW chipset we are running on.

                *** Legacy, use ChipInfo DAL: DalChipInfo_GetFamily

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  hw_chipset_type - The chipset.

  SIDE EFFECTS  None.

==========================================================================*/

hw_chipset_type hw_chipset (void);


/*==========================================================================

  FUNCTION      hw_modem

  DESCRIPTION   Returns the modems that the HW target we are running on
                supports.

                *** Legacy, use ChipInfo DAL: DalChipInfo_GetModemSupport

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  hw_modem_type - The mask of modems supported.

  SIDE EFFECTS  None.

==========================================================================*/

hw_modem_type hw_modem (void);


/*==========================================================================

  FUNCTION      HW_HAS_EDGE_ONLY_MODEM

  DESCRIPTION   Macro to check if the chip modem supports EDGE (GSM) but
                not UMTS.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  TRUE if EDGE is supported but not UMTS.

==========================================================================*/

#define HW_HAS_EDGE_ONLY_MODEM() \
  ((hw_modem() & HW_MODEM_GSM) && !(hw_modem() & HW_MODEM_UMTS))


/*==========================================================================

  FUNCTION      HW_HAS_NO_MODEM

  DESCRIPTION   Macro to check if the chip has any modem core at all.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  TRUE if any modem core (except for GPS) exists.

==========================================================================*/

#define HW_HAS_NO_MODEM() \
  (hw_modem() == HW_MODEM_CGPS || hw_modem() == 0)


/*==========================================================================

  FUNCTION      HW_PARTNUM_VERSION

  DESCRIPTION   Fill in the hardware partnumber and version ID from the MSM.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None.

==========================================================================*/

void hw_partnum_version
(
  hw_rev_type *rev_ptr
);


/*==========================================================================

  FUNCTION      hw_init_smem

  DESCRIPTION   Initializes the HW shared memory data.  Should be called
                from the bootloader once SMEM is ready.

  PARAMETERS    None.

  DEPENDENCIES  Shared memory must be ready.

  RETURN VALUE  None.

==========================================================================*/

void hw_init_smem (void);


/*==========================================================================

  FUNCTION      hw_init_hwio

  DESCRIPTION   Initializes HWIO access.  Maps any required virtual
                memory for AMSS.

  PARAMETERS    None.

  DEPENDENCIES  For multi-core targets using mutexes for HWIO
                read-mask-write mutual exclusion the DAL must be ready.

  RETURN VALUE  None.

==========================================================================*/

void hw_init_hwio (void);


/*==========================================================================

  FUNCTION      hw_restore

  DESCRIPTION   This function restores any state managed by the HW driver
                after power collapse.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None.

==========================================================================*/

void hw_restore (void);


/*==========================================================================

  FUNCTION      hw_valid_addr

  DESCRIPTION   This procedure verifies that an address is within legal
                range for diag peek/poke commands. "Legal range" means
                physical RAM not containing code, or MSM register memory.
                Given the complexity of supporting this in multi-OS and
                multi-processor environments, this function has been
                depreciated and will always return TRUE.

  PARAMETERS    ptr  -  Start of range to check
                len  -  Length of range to check in bytes

  DEPENDENCIES  None.

  RETURN VALUE  Always returns TRUE.

==========================================================================*/

boolean hw_valid_addr
(
  void   *ptr,
  uint16  len
);



/*=========================================================================
      Legacy Mappings
==========================================================================*/

/*
 * Renamed functions
 */
#define hw_version_int(ptr)         hw_partnum_version(ptr)
#define hw_sec_partnum_version(ptr) hw_partnum_version(ptr)
#define hw_map_amss_hwio            hw_init_hwio

/*
 * Obselete functions mapped to fixed return values.
 */
#define msm_device_init()
#define hw_config_pmic()
#define hw_enable_boot_ram()
#define hw_exit_low_power_mode()
#define hw_enter_low_power_mode()
#define hw_version()        (0xFF00)
#define hw_force_offline()  (FALSE)

/*
 * Obselete target types.
 */
#define MSM_6245   DALCHIPINFO_ID_MSM6245
#define MSM_6255A  DALCHIPINFO_ID_MSM6255A
#define MSM_6260   DALCHIPINFO_ID_MSM6260
#define MSM_OTHER  DALCHIPINFO_ID_UNKNOWN
#define MSM_50_A0  0x6000        
#define HW_MSM7600_REV12_VERSION_ID 0x2

/*
 * Legacy macros for checking if HW has certain features,
 * fixes or bugs.
 */
#define HW_HAS_EUL_CIPHERING_FIX() 1
#define HW_HAS_FAST_SLEEP_FIX() 1
#define HW_CAN_ACCESS_SLP_CTLR_AT_HIGH_SPEED() 1
#define HW_SUPPORTS_ADSP_PREFETCH() 1
#define HW_HAS_GRFC_0_UHF_VCO_EN() 1
#define HW_HAS_BROKEN_MPM() 0
#define HW_MSM6800_HAS_R1_V0 0
#define HW_MSM6500_HAS_R1_V1 0
#define HW_MSM6500_HAS_R1_V3 0


/*
 * Legacy hw_targets
 */
#define HW_FFA HW_PLATFORM_FFA
#define HW_SURF HW_PLATFORM_SURF
#define hw_target hw_platform
#define HW_PLATFORM_VELCRO HW_PLATFORM_FUSION
#define hw_platform_version(info) hw_platform_info(info)
#define hw_platform_version_type hw_platform_info_type


#endif  /* HW_H */


