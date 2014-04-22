/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: mipsmgr.h
@brief: Header for ADSPPM MIPS/MPPS Manager.

$Header: //components/rel/core.mpss/3.10/power/adsppm/src/common/core/src/mipsmgr.h#1 $
*/

#ifndef MIPSMGR_H_
#define MIPSMGR_H_

#include "requestmgr.h"


Adsppm_Status MIPS_Init(void);

Adsppm_Status MIPS_ProcessRequest(coreUtils_Q_Type *pMipsReqQ);

uint32 MIPS_GetMppsAggregationInfo(uint32 clientClasses);

Adsppm_Status mipsmgr_GetMppsPerClientClass( MmpmClientClassType clientClass, uint32 *mppsvalue);

Adsppm_Status MIPS_GetCompensatedDDRBw(coreUtils_Q_Type* pRequestQueue, AdsppmBusRouteType* pRoute, uint64* pTotalRequestedBw);

#endif /* MIPSMGR_H_ */

