#ifndef __PLATFORMINFO_H__
#define __PLATFORMINFO_H__
/*
===========================================================================
*/
/**
  @file PlatformInfo.h

  Internal header file for the PlatformInfo driver.
*/
/*
  ====================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All rights reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/core.mpss/3.10/systemdrivers/platforminfo/src/PlatformInfo.h#1 $
  $DateTime: 2018/06/26 03:18:17 $
  $Author: pwbldsvc $

  ====================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "DDIPlatformInfo.h"
#include "DALSys.h"
#include "DALGlbCtxt.h"

/*=========================================================================
      Macro Definitions
==========================================================================*/

#ifdef __ARMCC_VERSION
  #define PACKED __packed
#else
  #define PACKED
#endif

/*=========================================================================
      Type Definitions
==========================================================================*/

typedef struct
{
  uint32 nKey;
  uint32 nValue;
} PlatformInfoKVPSType;

/*
 * PlatformInfoGlbCtxType
 *
 * Global context data structure for sharing the platform across
 * processors.
 */
typedef struct
{
  DALGLB_HEADER                     DALGLBHeader;
  DalPlatformInfoPlatformInfoType   PlatformInfo;
  uint32                            nNumKVPS;
  PlatformInfoKVPSType              aKVPS[];
} PlatformInfoGlbCtxType;

/*
 * PlatformInfo driver context.
 */
typedef struct PlatformInfoDrvCtxt
{
  PlatformInfoGlbCtxType          *pGlbCtxt;
  DalPlatformInfoPlatformInfoType  PlatformInfo;
  uint32                           nNumKVPS;
  PlatformInfoKVPSType            *aKVPS;
} PlatformInfoDrvCtxt;

/*=========================================================================
      Function Definitions
==========================================================================*/

DALResult PlatformInfo_DriverInit ( PlatformInfoDrvCtxt *pDrvCtxt );


/*=========================================================================
      Function Definitions From DDIPlatformInfo.h
==========================================================================*/

DALResult PlatformInfo_GetPlatform ( PlatformInfoDrvCtxt *pDrvCtxt, DalPlatformInfoPlatformType * pType );
DALResult PlatformInfo_GetPlatformInfo ( PlatformInfoDrvCtxt *pDrvCtxt, DalPlatformInfoPlatformInfoType * pInfo );
DALResult PlatformInfo_GetKeyValue ( PlatformInfoDrvCtxt *pDrvCtxt, DalPlatformInfoKeyType eKey, uint32 *nValue );
DALResult PlatformInfo_CDTConfigPreDDR ( PlatformInfoDrvCtxt *pDrvCtxt, void *pPlatformInfo );
DALResult PlatformInfo_CDTConfigPostDDR ( PlatformInfoDrvCtxt *pDrvCtxt, void *pPlatformInfo );

#endif

