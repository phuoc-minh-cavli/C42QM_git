/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adspclkmgr.c
@brief: Implementation of ADSPPM ADSP Clock Manager.

$Header: //components/rel/core.mpss/3.10/power/adsppm/src/common/core/src/adspclkmgr.c#1 $
*/

#include "adsppm.h"
#include "adsppm_utils.h"
#include "adspclkmgr.h"
#include "dcvsmgr.h"
#include "hal_clkrgm.h"
#include "observer.h"

#include "err.h"

// All state info for the ADSP Clock Manager
typedef struct
{
    DALSYSSyncHandle lock;
    boolean dcvsEnabledDisabledFlag;
    uint32 dcvsVote;
    uint32 clientsFloorVote;
    uint32 clientsFinalVote;
    uint32 finalVoteToNpa;
    AdsppmObserverStateType observerState;
} AdsppmAdspClkManagerType;

static AdsppmAdspClkManagerType gAdsppmAdspClkManager;

///////////////////////////////////////////////////////////////////////////////
// Static functions
///////////////////////////////////////////////////////////////////////////////

// Set the ADSP core clock and notify any observers of the event
static Adsppm_Status adspClkMgr_SetAdspClock(AdsppmAdspClkManagerType* self,
    uint32 freqHz)
{
    Adsppm_Status status;
    // Send final ADSP clock vote to HAL
    status = ClkRgm_SetClock(AdsppmClk_Adsp_Core, freqHz);
    if(status == Adsppm_Status_Success)
        observer_notify(&self->observerState, ADSPCLKMGR_CLOCK_CHANGE_EVENT);
    return status;
}

// Calculate new vote to NPA
// Assume lock is obtained
static void adspClkMgr_CalculateNewNpaVote(
    AdsppmAdspClkManagerType* self,
    uint32 newClientsFinalVote,
    uint32 newClientsFloorVote,
    uint32* pNewFinalVoteToNpa)
{

    uint32 previousClientsFinalVote = self->clientsFinalVote;
    boolean changeInClientFinalVote =
        (newClientsFinalVote != previousClientsFinalVote);

    if(self->dcvsVote)
    {
        *pNewFinalVoteToNpa = changeInClientFinalVote?
            MAX(newClientsFinalVote, self->dcvsVote):
            MAX(newClientsFloorVote, self->finalVoteToNpa);
    }

}

// Print current context values
// Assume lock is obtained
static void adspClkMgr_PrintContext(
    AdsppmAdspClkManagerType* self)
{

    if(self->dcvsEnabledDisabledFlag)
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO, "DCVS Enabled:");
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
            "   Current DCVS vote:          clk=%u",
            self->dcvsVote);
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
            "   Current Clients Floor vote: clk=%u",
            self->clientsFloorVote);
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
            "   Current Clients Final vote: clk=%u",
            self->clientsFinalVote);
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO, "DCVS Disabled");
    }

}

// Check changes in final vote to NPA
// Assume lock is obtained
static boolean adspClkMgr_checkChangeInFinalVoteToNpa(
    AdsppmAdspClkManagerType* self,
    uint32 newFinalVoteToNpa)
{

    boolean result = FALSE;

    if(newFinalVoteToNpa != self->finalVoteToNpa) 
    {
        self->finalVoteToNpa = newFinalVoteToNpa;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
            "Send new ADSP Clock to HAL");

        adspClkMgr_PrintContext(self);

        result = TRUE;
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
            "NO updates on ADSP Clock");

        result = FALSE;
    }

    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
        "   Current vote to NPA: clk=%u",
        self->finalVoteToNpa);

    return result;

}

// Check for changes in ADSP clock vote
// if there is a change, update local variable
static void adspClkMgr_onAdspClockVoteUpdate(
    AdsppmDcvsManagerEventId eventId,
    void* param)
{

    AdsppmAdspClkManagerType* self = (AdsppmAdspClkManagerType*) param;
    uint32 newFinalVoteToNpa;
    boolean changeInFinalVoteToNpa = FALSE;
    uint32 dcvsVote = 0;

    if(eventId != DCVSMGR_ADSP_CLOCK_VOTE_CHANGE_EVENT)
        return;

    DcvsMgr_GetDcvsAdspClockVote(&dcvsVote);

    adsppm_lock(self->lock);

    newFinalVoteToNpa = self->clientsFinalVote;

    if(self->dcvsEnabledDisabledFlag)
    {
        // update self->dcvsVote
        self->dcvsVote = dcvsVote;

        if(self->dcvsVote)
        {
            newFinalVoteToNpa = MAX(self->clientsFloorVote, self->dcvsVote);
        }
    }

    changeInFinalVoteToNpa = adspClkMgr_checkChangeInFinalVoteToNpa(
        self, newFinalVoteToNpa);

    adsppm_unlock(self->lock);

    // Set final ADSP clock vote
    if(changeInFinalVoteToNpa)
        adspClkMgr_SetAdspClock(self, newFinalVoteToNpa);

}

// Check for changes in DCVS' enabled/disabled flag
// if there is a change, update local variable
static void adspClkMgr_onDcvsEnabledDisabledUpdate(
    AdsppmDcvsManagerEventId eventId,
    void* param)
{

    AdsppmAdspClkManagerType* self = (AdsppmAdspClkManagerType*) param;
    boolean dcvsEnabledDisabledFlag = DcvsMgr_GetDcvsEnabledDisabledFlag();

    adsppm_lock(self->lock);

    // update self->dcvsEnabledDisabledFlag
    self->dcvsEnabledDisabledFlag = dcvsEnabledDisabledFlag;

    if(!dcvsEnabledDisabledFlag)
    {
        self->dcvsVote = 0;
    }

    adsppm_unlock(self->lock);

}

// Send updated clients final vote to SysMon DCVS
// using SysMon DCVS API
static void adspClkMgr_SendNewClientsFinalVoteToDcvs(
    uint32 newClientsFinalVote)
{

    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
        "adspClkMgr_SendNewClientsFinalVoteToDcvs - DO NOTHING(Stub)");

    ERR_FATAL("adspClkMgr_SendNewClientsFinalVoteToDcvs - DCVS Not Supported- STUB",0,0,0);
}

///////////////////////////////////////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////////////////////////////////////

Adsppm_Status AdspClkMgr_Init(void)
{

    AdsppmAdspClkManagerType* self = &gAdsppmAdspClkManager;

    memset(self, 0, sizeof(AdsppmAdspClkManagerType));

    if(DAL_SUCCESS != DALSYS_SyncCreate(
        DALSYS_SYNC_ATTR_RESOURCE,
        &self->lock,
        NULL))
    {
        return Adsppm_Status_Failed;
    }

    if(observer_init(&self->observerState) != Adsppm_Status_Success)
        return Adsppm_Status_Failed;

    if(ACM_GetFeatureStatus(AsicFeatureId_DcvsControl) ==
        AsicFeatureState_Enabled)
    {
        if(DcvsMgr_RegisterEvent(
                DCVSMGR_DCVS_ENABLED_DISABLED_CHANGE_EVENT,
                adspClkMgr_onDcvsEnabledDisabledUpdate,
                (void*) self) !=
            Adsppm_Status_Success)
        {
            return Adsppm_Status_Failed;
        }

        if(DcvsMgr_RegisterEvent(
                DCVSMGR_ADSP_CLOCK_VOTE_CHANGE_EVENT,
                adspClkMgr_onAdspClockVoteUpdate,
                (void*) self) !=
            Adsppm_Status_Success)
        {
            return Adsppm_Status_Failed;
        }
    }

    return Adsppm_Status_Success;

}

Adsppm_Status AdspClkMgr_IssueAdspClkRequest(
    uint32 newClientsFinalVote,
    uint32 newClientsFloorVote)
{

    AdsppmAdspClkManagerType* self = &gAdsppmAdspClkManager;
    Adsppm_Status sts = Adsppm_Status_Success;
    uint32 newFinalVoteToNpa = newClientsFinalVote;
    boolean changeInFinalVoteToNpa = FALSE;
    boolean sendClientVoteToDcvs = FALSE;

    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
        "DCVS, clients floor value: clk=%u", newClientsFloorVote);

    adsppm_lock(self->lock);

    if(self->dcvsEnabledDisabledFlag)
    {
        sendClientVoteToDcvs = (newClientsFinalVote != self->clientsFinalVote);

        adspClkMgr_CalculateNewNpaVote(self, newClientsFinalVote,
            newClientsFloorVote, &newFinalVoteToNpa);
    }

    // Store new clients final and floor vote
    self->clientsFinalVote = newClientsFinalVote;
    self->clientsFloorVote = newClientsFloorVote;

    changeInFinalVoteToNpa = adspClkMgr_checkChangeInFinalVoteToNpa(
        self, newFinalVoteToNpa);

    adsppm_unlock(self->lock);

    // Set final ADSP clock vote
    if((sts == Adsppm_Status_Success) && changeInFinalVoteToNpa)
        sts = adspClkMgr_SetAdspClock(self, newFinalVoteToNpa);

	if(ACM_GetFeatureStatus(AsicFeatureId_DcvsControl) ==
		AsicFeatureState_Enabled)
	{
		// Update SysMon DCVS with new clients final vote
		if((sts == Adsppm_Status_Success) && sendClientVoteToDcvs)
			adspClkMgr_SendNewClientsFinalVoteToDcvs(newClientsFinalVote);
	}

    return sts;

}

Adsppm_Status AdspClkMgr_GetAdspClockFreqVote(uint32* adspClockFreqVote)
{
    AdsppmAdspClkManagerType* self = &gAdsppmAdspClkManager;
    Adsppm_Status status = Adsppm_Status_Failed;
    if(adspClockFreqVote != NULL)
    {
        adsppm_lock(self->lock);
        *adspClockFreqVote = self->finalVoteToNpa;
        adsppm_unlock(self->lock);
        status = Adsppm_Status_Success;
    }
    return status;
}

Adsppm_Status AdspClkMgr_RegisterEvent(AdsppmAdspClkManagerEventId event,
    AdsppmAdspClkEventHandler handler, void* param)
{
    AdsppmAdspClkManagerType* self = &gAdsppmAdspClkManager;
    // Assuming AdsppmAdspClkEventHandler can be cast to AdsppmEventHandler
    return observer_add(&self->observerState, event,
        (AdsppmEventHandler) handler, param);

}

Adsppm_Status AdspClkMgr_UnregisterEvent(AdsppmAdspClkManagerEventId event,
    AdsppmAdspClkEventHandler handler)
{
    AdsppmAdspClkManagerType* self = &gAdsppmAdspClkManager;
    // Assuming AdsppmAdspClkEventHandler can be cast to AdsppmEventHandler
    return observer_remove(&self->observerState, event,
        (AdsppmEventHandler) handler);

}

Adsppm_Status AdspClkMgr_GetInfo(
    AdsppmInfoDcvsAdspClockType* pDcvsAdspClockInfo)
{

    AdsppmAdspClkManagerType* self = &gAdsppmAdspClkManager;

    if(pDcvsAdspClockInfo == NULL)
        return Adsppm_Status_NullPointer;

    adsppm_lock(self->lock);
    pDcvsAdspClockInfo->dcvsVote         = self->dcvsVote;
    pDcvsAdspClockInfo->clientsFloorVote = self->clientsFloorVote;
    pDcvsAdspClockInfo->clientsFinalVote = self->clientsFinalVote;
    pDcvsAdspClockInfo->finalVoteToNpa   = self->finalVoteToNpa;
    adsppm_unlock(self->lock);

    return Adsppm_Status_Success;

}

