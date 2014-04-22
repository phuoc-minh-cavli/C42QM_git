/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: dcvsmgr.c
@brief: Implementation of ADSPPM DCVS Manager.

$Header: //components/rel/core.mpss/3.10/power/adsppm/src/common/core/src/dcvsmgr.c#2 $
*/

#include "adsppm.h"
#include "dcvsmgr.h"


///////////////////////////////////////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////////////////////////////////////

Adsppm_Status DcvsMgr_Init(void)
{
	
    return Adsppm_Status_Success;

}

boolean DcvsMgr_GetDcvsEnabledDisabledFlag(void)
{
  
    return Adsppm_Status_Success;
}

void DcvsMgr_GetDcvsAdsp2DdrBwVote(AdsppmBusBWDataIbAbType *pDcvsVote)
{
	
	return;
}

void DcvsMgr_GetDcvsAdspClockVote(uint32 *pDcvsVote)
{
	
	return;
}

void DcvsMgr_SysMonDcvsAdsp2DdrBwRequest(
    AdsppmBusBWDataIbAbType *pAdsp2DdrBwIbAb)
{
	return;
}

void DcvsMgr_SysMonDcvsAdspClockRequest(
    uint32 adspClockHz)
{
	return;
}

Adsppm_Status DcvsMgr_RegisterEvent(
    AdsppmDcvsManagerEventId event,
    AdsppmDcvsEventHandler handler, void* param)
{
	return Adsppm_Status_Success;
}

Adsppm_Status DcvsMgr_UnregisterEvent(
    AdsppmDcvsManagerEventId event,
    AdsppmDcvsEventHandler handler)
{

	return Adsppm_Status_Success;
}

