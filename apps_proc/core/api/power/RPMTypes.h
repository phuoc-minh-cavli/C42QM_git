#ifndef RPMTYPES_H
#define RPMTYPES_H
/*===========================================================================

                      RPM Shared Type Definitions

DESCRIPTION
  This file contains declarations for use by both the master-side and firmware-
  side of the RPM drivers.

  $Header: //components/rel/core.tx/6.0/api/power/RPMTypes.h#1 $

   Copyright (c) 2009-2010 by Qualcomm Technologies, Inc.  Confidential and Proprietary.
===========================================================================*/

#include "DALStdDef.h" /* for uint32 */

typedef struct DAL_rpm_InternalMessageType  DAL_rpm_InternalMessageType;
typedef DAL_rpm_InternalMessageType*        DAL_rpm_MessageType;

/* type definitions */
/* list of rpm resources */
typedef enum
{
  /* housekeeping */
  DAL_RPM_RESOURCE_NOTIFICATIONS                             = 0,
  DAL_RPM_RESOURCE_REQUEST_INVALIDATE                        = 1,
  DAL_RPM_RESOURCE_TRIGGER_TIMED                             = 2,
  DAL_RPM_RESOURCE_RPM_CTL                                   = 3,
  DAL_RPM_RESOURCE_TRIGGER_CLEAR                             = 4, // not supported on any target

  /* clocks */
  DAL_RPM_RESOURCE_CXO_CLK                                   = 5,
  DAL_RPM_RESOURCE_PXO_CLK                                   = 6,
  DAL_RPM_RESOURCE_PLL_4                                     = 7,
  DAL_RPM_RESOURCE_APPS_FABRIC_CLK                           = 8,
  DAL_RPM_RESOURCE_SYSTEM_FABRIC_CLK                         = 9,
  DAL_RPM_RESOURCE_MM_FABRIC_CLK                             = 10,
  DAL_RPM_RESOURCE_DAYTONA_FABRIC_CLK                        = 11,
  DAL_RPM_RESOURCE_SFPB_CLK                                  = 12,
  DAL_RPM_RESOURCE_CFPB_CLK                                  = 13,
  DAL_RPM_RESOURCE_MMFPB_CLK                                 = 14,
  DAL_RPM_RESOURCE_SMI_CLK                                   = 15,
  DAL_RPM_RESOURCE_EBI1_CLK                                  = 16,

  /* apps l2 cache control */
  DAL_RPM_RESOURCE_APPS_L2_CACHE_CTL                         = 17,

  /* apps fabric config & arbitration */
  DAL_RPM_RESOURCE_APPS_FABRIC_CFG_HALT                      = 18,
  DAL_RPM_RESOURCE_APPS_FABRIC_CFG_CLKMOD                    = 19,
  DAL_RPM_RESOURCE_APPS_FABRIC_CFG_IOCTL                     = 20,
  DAL_RPM_RESOURCE_APPS_FABRIC_ARB                           = 21,

  /* system fabric config & arbitration */
  DAL_RPM_RESOURCE_SYS_FABRIC_CFG_HALT                       = 22,
  DAL_RPM_RESOURCE_SYS_FABRIC_CFG_CLKMOD                     = 23,
  DAL_RPM_RESOURCE_SYS_FABRIC_CFG_IOCTL                      = 24,
  DAL_RPM_RESOURCE_SYSTEM_FABRIC_ARB                         = 25,

  /* multimedia fabric config & arbitration */
  DAL_RPM_RESOURCE_MMSS_FABRIC_CFG_HALT                      = 26,
  DAL_RPM_RESOURCE_MMSS_FABRIC_CFG_CLKMOD                    = 27,
  DAL_RPM_RESOURCE_MMSS_FABRIC_CFG_IOCTL                     = 28,
  DAL_RPM_RESOURCE_MM_FABRIC_ARB                             = 29,

  /*
  *  The following sections apply only to 8660.
  *
  *  The range is either assigned to other resources
  *  on other targets, or is reserved.
  */
  /* pmic 8901 */
  DAL_RPM_RESOURCE_SMPS0B                                    = 30,
  DAL_RPM_RESOURCE_SMPS1B                                    = 31,
  DAL_RPM_RESOURCE_SMPS2B                                    = 32,
  DAL_RPM_RESOURCE_SMPS3B                                    = 33,
  DAL_RPM_RESOURCE_SMPS4B                                    = 34,
  DAL_RPM_RESOURCE_LDO0B                                     = 35,
  DAL_RPM_RESOURCE_LDO1B                                     = 36,
  DAL_RPM_RESOURCE_LDO2B                                     = 37,
  DAL_RPM_RESOURCE_LDO3B                                     = 38,
  DAL_RPM_RESOURCE_LDO4B                                     = 39,
  DAL_RPM_RESOURCE_LDO5B                                     = 40,
  DAL_RPM_RESOURCE_LDO6B                                     = 41,
  DAL_RPM_RESOURCE_LVS0B                                     = 42,
  DAL_RPM_RESOURCE_LVS1B                                     = 43,
  DAL_RPM_RESOURCE_LVS2B                                     = 44,
  DAL_RPM_RESOURCE_LVS3B                                     = 45,
  DAL_RPM_RESOURCE_MVS                                       = 46,

  /* pmic 8058 */
  DAL_RPM_RESOURCE_SMPS0                                     = 47,
  DAL_RPM_RESOURCE_SMPS1                                     = 48,
  DAL_RPM_RESOURCE_SMPS2                                     = 49,
  DAL_RPM_RESOURCE_SMPS3                                     = 50,
  DAL_RPM_RESOURCE_SMPS4                                     = 51,
  DAL_RPM_RESOURCE_LDO0                                      = 52,
  DAL_RPM_RESOURCE_LDO1                                      = 53,
  DAL_RPM_RESOURCE_LDO2                                      = 54,
  DAL_RPM_RESOURCE_LDO3                                      = 55,
  DAL_RPM_RESOURCE_LDO4                                      = 56,
  DAL_RPM_RESOURCE_LDO5                                      = 57,
  DAL_RPM_RESOURCE_LDO6                                      = 58,
  DAL_RPM_RESOURCE_LDO7                                      = 59,
  DAL_RPM_RESOURCE_LDO8                                      = 60,
  DAL_RPM_RESOURCE_LDO9                                      = 61,
  DAL_RPM_RESOURCE_LDO10                                     = 62,
  DAL_RPM_RESOURCE_LDO11                                     = 63,
  DAL_RPM_RESOURCE_LDO12                                     = 64,
  DAL_RPM_RESOURCE_LDO13                                     = 65,
  DAL_RPM_RESOURCE_LDO14                                     = 66,
  DAL_RPM_RESOURCE_LDO15                                     = 67,
  DAL_RPM_RESOURCE_LDO16                                     = 68,
  DAL_RPM_RESOURCE_LDO17                                     = 69,
  DAL_RPM_RESOURCE_LDO18                                     = 70,
  DAL_RPM_RESOURCE_LDO19                                     = 71,
  DAL_RPM_RESOURCE_LDO20                                     = 72,
  DAL_RPM_RESOURCE_LDO21                                     = 73,
  DAL_RPM_RESOURCE_LDO22                                     = 74,
  DAL_RPM_RESOURCE_LDO23                                     = 75,
  DAL_RPM_RESOURCE_LDO24                                     = 76,
  DAL_RPM_RESOURCE_LDO25                                     = 77,
  DAL_RPM_RESOURCE_LVS0                                      = 78,
  DAL_RPM_RESOURCE_LVS1                                      = 79,

  /*
  *  The following sections apply only to 8960.
  *
  *  The range is either assigned to other resources
  *  on other targets, or is reserved.
  */
  /* pmic 8921 */
  DAL_RPM_RESOURCE_PM8921_S1                                 = 30,
  DAL_RPM_RESOURCE_PM8921_S2                                 = 31,
  DAL_RPM_RESOURCE_PM8921_S3                                 = 32,
  DAL_RPM_RESOURCE_PM8921_S4                                 = 33,
  DAL_RPM_RESOURCE_PM8921_S5                                 = 34,
  DAL_RPM_RESOURCE_PM8921_S6                                 = 35,
  DAL_RPM_RESOURCE_PM8921_S7                                 = 36,
  DAL_RPM_RESOURCE_PM8921_S8                                 = 37,
  DAL_RPM_RESOURCE_PM8921_L1                                 = 38,
  DAL_RPM_RESOURCE_PM8921_L2                                 = 39,
  DAL_RPM_RESOURCE_PM8921_L3                                 = 40,
  DAL_RPM_RESOURCE_PM8921_L4                                 = 41,
  DAL_RPM_RESOURCE_PM8921_L5                                 = 42,
  DAL_RPM_RESOURCE_PM8921_L6                                 = 43,
  DAL_RPM_RESOURCE_PM8921_L7                                 = 44,
  DAL_RPM_RESOURCE_PM8921_L8                                 = 45,
  DAL_RPM_RESOURCE_PM8921_L9                                 = 46,
  DAL_RPM_RESOURCE_PM8921_L10                                = 47,
  DAL_RPM_RESOURCE_PM8921_L11                                = 48,
  DAL_RPM_RESOURCE_PM8921_L12                                = 49,
  DAL_RPM_RESOURCE_PM8921_L13                                = 50,
  DAL_RPM_RESOURCE_PM8921_L14                                = 51,
  DAL_RPM_RESOURCE_PM8921_L15                                = 52,
  DAL_RPM_RESOURCE_PM8921_L16                                = 53,
  DAL_RPM_RESOURCE_PM8921_L17                                = 54,
  DAL_RPM_RESOURCE_PM8921_L18                                = 55,
  DAL_RPM_RESOURCE_PM8921_L19                                = 56,
  DAL_RPM_RESOURCE_PM8921_L20                                = 57,
  DAL_RPM_RESOURCE_PM8921_L21                                = 58,
  DAL_RPM_RESOURCE_PM8921_L22                                = 59,
  DAL_RPM_RESOURCE_PM8921_L23                                = 60,
  DAL_RPM_RESOURCE_PM8921_L24                                = 61,
  DAL_RPM_RESOURCE_PM8921_L25                                = 62,
  DAL_RPM_RESOURCE_PM8921_L26                                = 63,
  DAL_RPM_RESOURCE_PM8921_L27                                = 64,
  DAL_RPM_RESOURCE_PM8921_L28                                = 65,
  DAL_RPM_RESOURCE_PM8921_L29                                = 66,
  DAL_RPM_RESOURCE_PM8921_CKL1                               = 67,
  DAL_RPM_RESOURCE_PM8921_CLK2                               = 68,
  DAL_RPM_RESOURCE_PM8921_LVS1                               = 69,
  DAL_RPM_RESOURCE_PM8921_LVS2                               = 70,
  DAL_RPM_RESOURCE_PM8921_LVS3                               = 71,
  DAL_RPM_RESOURCE_PM8921_LVS4                               = 72,
  DAL_RPM_RESOURCE_PM8921_LVS5                               = 73,
  DAL_RPM_RESOURCE_PM8921_LVS6                               = 74,
  DAL_RPM_RESOURCE_PM8921_LVS7                               = 75,
  /* 76-79 are unused/reserved on 8960. */

  /*
  *  The following sections apply only to 9x15.
  *
  *  The range is either assigned to other resources
  *  on other targets, or is reserved.
  */
  /* pmic 8018 */
 
  DAL_RPM_RESOURCE_PM8018_S1                                 = 30,
  DAL_RPM_RESOURCE_PM8018_S2                                 = 31,
  DAL_RPM_RESOURCE_PM8018_S3                                 = 32,
  DAL_RPM_RESOURCE_PM8018_S4                                 = 33,
  DAL_RPM_RESOURCE_PM8018_S5                                 = 34,

  DAL_RPM_RESOURCE_PM8018_L1                                 = 35,
  DAL_RPM_RESOURCE_PM8018_L2                                 = 36,
  DAL_RPM_RESOURCE_PM8018_L3                                 = 37,
  DAL_RPM_RESOURCE_PM8018_L4                                 = 38,
  DAL_RPM_RESOURCE_PM8018_L5                                 = 39,
  DAL_RPM_RESOURCE_PM8018_L6                                 = 40,
  DAL_RPM_RESOURCE_PM8018_L7                                 = 41,
  DAL_RPM_RESOURCE_PM8018_L8                                 = 42,
  DAL_RPM_RESOURCE_PM8018_L9                                 = 43,
  DAL_RPM_RESOURCE_PM8018_L10                                = 44,
  DAL_RPM_RESOURCE_PM8018_L11                                = 45,
  DAL_RPM_RESOURCE_PM8018_L12                                = 46,
  DAL_RPM_RESOURCE_PM8018_L13                                = 47,
  DAL_RPM_RESOURCE_PM8018_L14                                = 48,

  DAL_RPM_RESOURCE_PM8018_LVS1                               = 49,
  DAL_RPM_RESOURCE_DDR_SR                                    = 50,
  /* 50-79 are unused/reserved on 9x15. */
    
  /*
   * The follow section applies to all targets once again.
   */
  /* misc PMIC */
  DAL_RPM_RESOURCE_NCP                                       = 80,
  DAL_RPM_RESOURCE_CXO_BUFFERS                               = 81,
  DAL_RPM_RESOURCE_USB_OTG_SWITCH                            = 82,
  DAL_RPM_RESOURCE_HDMI_SWITCH                               = 83,

  DAL_RPM_RESOURCE_DDR_DMM                                   = 84,
  /* compilation stuff -- new entries should be placed just above these */
  DAL_RPM_RESOURCE_COUNT,
  DAL_RPM_RESOURCE_SIZE = 0x7FFFFFFF, /* force enum to 32-bits */
  DAL_RPM_RESOURCE_SIGN = -1          /* avoid stupid 'pointless comparison' warnings */
} DAL_rpm_ResourceType;

#define RPM_IS_VALID_RESOURCE(x) (((x) >= 0) && ((x) < DAL_RPM_RESOURCE_COUNT))

/* list of masters in the system */
typedef enum
{
  DAL_RPMFW_MASTER_0 = 0,  /* all targets: APSS */
  DAL_RPMFW_MASTER_1,      /* all targets: MPSS SW */
  DAL_RPMFW_MASTER_2,      /* all targets: LPASS */
  DAL_RPMFW_MASTER_3,      /* 8960: RIVA */
  DAL_RPMFW_MASTER_4,      /* 8960: DSPS */
  DAL_RPMFW_MASTER_5,      /* 8960: MPSS FW */
  DAL_RPMFW_MASTER_COUNT,
  DAL_RPMFW_MASTER_SIZE = 0x7FFFFFFF,
  DAL_RPMFW_MASTER_SIGN = -1
} DAL_rpmfw_MasterType;

#define RPM_IS_VALID_MASTER(x) (((x) >= 0) && ((x) < DAL_RPMFW_MASTER_COUNT))

/* list of config sets in the system */
typedef enum
{
  DAL_RPM_CONFIG_SET_ACTIVE_0 = 0,
  DAL_RPM_CONFIG_SET_PRIMARY = DAL_RPM_CONFIG_SET_ACTIVE_0,
  DAL_RPM_CONFIG_SET_SLEEP,
  DAL_RPM_CONFIG_SET_COUNT,
  DAL_RPM_CONFIG_SET_INVALID = DAL_RPM_CONFIG_SET_COUNT,
  DAL_RPM_CONFIG_SET_SIZE = 0x7FFFFFFF,
  DAL_RPM_CONFIG_SET_SIGN = -1
} DAL_rpm_ConfigSetType;

#define RPM_IS_VALID_CONFIG_SET(x) (((x) >= 0) && ((x) < DAL_RPM_CONFIG_SET_COUNT))

/* structure passed to drivers in request callbacks */
typedef struct
{
  DAL_rpm_ResourceType  Resource;
  DAL_rpmfw_MasterType  Master;
  DAL_rpm_ConfigSetType ConfigSet;
  uint32                DataLen;
  void                 *Data;
  DALBOOL               Rejected;
} DAL_rpmfw_RequestType;

typedef enum
{
  DAL_RPMFW_SPM_AWAKE = 0,
  DAL_RPMFW_SPM_GOING_TO_SLEEP,
  DAL_RPMFW_SPM_SLEEPING,
  DAL_RPMFW_SPM_WAKING_UP
} DAL_rpmfw_SPMStatusType;

typedef enum
{
  DAL_RPM_INTERRUPT_INVALID = 0,
  DAL_RPM_INTERRUPT_SPM_SHUTDOWN_HANDSHAKE,
  DAL_RPM_INTERRUPT_SPM_WAKEUP_HANDSHAKE,
  DAL_RPM_INTERRUPT_COUNT,
  DAL_RPM_INTERRUPT_SIZE = 0x7FFFFFFF,
  DAL_RPM_INTERRUPT_SIGN = -1
} DAL_rpm_InterruptType;

#define RPM_IS_VALID_INTERRUPT(x) (((x) >= 0) && ((x) < DAL_RPM_INTERRUPT_COUNT))

typedef enum
{
  DAL_RPM_FEATURE_DEEP_SLEEP_ONLY        = 0,
  DAL_RPM_FEATURE_MULTI_TIER_ARBITRATION = 1,
  DAL_RPM_FEATURE_COUNT,
  DAL_RPM_FEATURE_SIZE                   = 0x7FFFFFFF
} DAL_rpm_FeatureType;
#define DAL_RPM_FEATURE_DISABLED 0
#define DAL_RPM_FEATURE_ENABLED  1
#endif /* RPMTYPES_H */

