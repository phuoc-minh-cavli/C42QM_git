#ifndef _CSD_OS_DEPENDENCIES_DEFS_H_
#define _CSD_OS_DEPENDENCIES_DEFS_H_
/*===========================================================================
                          CSD   W r a p p e r 
               f o r   W32/ Simulator OS dependencies

  @file CadOsDependencies.h
  This file contains OS dependencies for CAD. Each platform (OS) will 
  have its own copy of this file with their platform (OS) specific 
  dependencies/ code

Copyright (c) 2008 - 2012, 2016 QUALCOMM Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
========================================================================*/

/*===========================================================================
                             Edit History

$Header:

when       who         what, where, why
--------   ---         ------------------------------------------------------- 
10/21/13   vk          changes related to TLMM GPIO 
10/23/12   jk          Added support for dynamic module enablement  
05/25/12   rp          Added  Mem region allocation support 
03/02/09   asheeshk    Created file.
============================================================================*/

#include "DALSys.h"
#include <stdio.h>
#include <stddef.h>    // size_t, etc.  typedef unsigned int size_t;
#include "target.h"   //for #define PMEM_MSM_H "pmem_7500.h" needed in pmem.h
#include "pmem.h"
#include "mmdefs.h"
#include "DDIPlatformInfo.h"
#include "DDIChipInfo.h"
#include "DDITimetick.h"
#include "timer.h"

#define VOC_COMMON_PRIORITY         212


#define MAP_FAILED    (-1)

#define CSD_SLEEP(ms)				timer_sleep( ( uint32 ) ms, T_MSEC, TRUE )  

#define ACDB_FLAVOR_I2S             (1)
#define ACDB_FLAVOR_SLIMBUS         (2)
#define ACDB_FLAVOR_BAREBONE        (3)
#define ACDB_FLAVOR_UNKNOWN         (0xFFFFFFFF)

/*
 * Queries the platform infomration from the CDT 
 *
 * @param[out]PlatformInfo - platform infor.
 *
 * @return CSD_EOK value on success else
 *         CSD_EFAILED  value on failure case
 *         CSD_EBADPARAM  value on bad input parameters
 */
int32_t csd_get_cdt_platform_info( DalPlatformInfoPlatformInfoType *PlatformInfo);

/*
 * Queries chip family from the CDT 
 *
 * @param[out]chip_family - chip family infor.
 *
 * @return CSD_EOK value on success else
 *         CSD_EFAILED  value on failure case
 *         CSD_EBADPARAM  value on bad input parameters
 */
int32_t csd_get_cdt_chip_family( DalChipInfoFamilyType *chip_family );

/*
 * Queries the platform and nv configuration for WCD Initialization
 *
 * @param[out] flag
 *
 * @return csd status value @see csd_status.h
 */
int32_t csd_is_wcd_codec_present( bool_t* flag );

#endif /* _CSD_OS_DEPENDENCIES_DEFS_H_ */
