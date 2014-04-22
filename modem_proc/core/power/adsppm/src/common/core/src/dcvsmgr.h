/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: dcvsmgr.h
@brief: Header for ADSPPM DCVS Manager.

$Header: //components/rel/core.mpss/3.10/power/adsppm/src/common/core/src/dcvsmgr.h#1 $
*/

#ifndef _DCVSMGR_H_
#define _DCVSMGR_H_

#include "adsppm.h"

// Enumeration for observable events generated by the DCVS Manager
typedef enum
{
    DCVSMGR_DCVS_ENABLED_DISABLED_CHANGE_EVENT = 0,
    DCVSMGR_ADSP_TO_DDR_BW_VOTE_CHANGE_EVENT,
    DCVSMGR_ADSP_CLOCK_VOTE_CHANGE_EVENT
} AdsppmDcvsManagerEventId;

typedef void (*AdsppmDcvsEventHandler)(
    AdsppmDcvsManagerEventId event,
    void* param);

// Initialize the DCVS Manager.
// Requires that the Client Manager
// is already initialized.
Adsppm_Status DcvsMgr_Init(void);

// Get DCVS enable/disable flag state
boolean DcvsMgr_GetDcvsEnabledDisabledFlag(void);

// Get current DCVS ADSP to DDR BW vote
void DcvsMgr_GetDcvsAdsp2DdrBwVote(AdsppmBusBWDataIbAbType *pDcvsVote);

// Get current DCVS ADSP core clock vote
void DcvsMgr_GetDcvsAdspClockVote(uint32 *pDcvsVote);

// ADSP to DDR BW vote update from SysMon DCVS
void DcvsMgr_SysMonDcvsAdsp2DdrBwRequest(
    AdsppmBusBWDataIbAbType *pAdsp2DdrBwIbAb);

// ADSP clock vote update from SysMon DCVS
void DcvsMgr_SysMonDcvsAdspClockRequest(
    uint32 adspClockHz);

// Register as an observer for an event
Adsppm_Status DcvsMgr_RegisterEvent(
    AdsppmDcvsManagerEventId event,
    AdsppmDcvsEventHandler handler, void* param);

// Unregister as an observer for a previously registered event
Adsppm_Status DcvsMgr_UnregisterEvent(
    AdsppmDcvsManagerEventId event,
    AdsppmDcvsEventHandler handler);


#endif /* _DCVSMGR_H_ */

