/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#include "adsppm.h"
#include "adsppm_utils.h"
#include "cachemgr.h"
//#include "observer.h"
#include "clientmgr.h"
//#include "asic.h"
//#include "qurt_timer.h"
//#include "mipsmgr.h"

// All state info for the Cache Partition Manager
typedef struct
{
    // DALSYSSyncHandle lock;
    // AdsppmObserverStateType observerState;
    // uint32 activeClientClasses;
    // AsicCachePartitionConfigTableType* partitionConfigTable;
    // qurt_cache_partition_size_t mainPartitionSize;
    // uint32 activeLineLockCount;
    // uint32 mppsthreshold[ADSPPM_NUM_OF_CLIENT_CLASSES];
} AdsppmCachePartitionManagerType;

//static AdsppmCachePartitionManagerType gAdsppmCachePartitionManager;

///////////////////////////////////////////////////////////////////////////////
// Static functions 
///////////////////////////////////////////////////////////////////////////////

// Notify all registered observers of the specified event
static void cpmNotify(AdsppmCachePartitionManagerEventId eventId)
{
    return;
}

// Returns true if there was a change in the active client classes.
// Assume lock already obtained.
static boolean cpmUpdateActiveClientClasses(
    AdsppmCachePartitionManagerType* self)
{
    boolean updated = FALSE;
    return updated;
}
/**
* Checks if current  clientclass MPPS vote is higher than threshold value or not.
* Returns true if it is otherwise false
*/
static boolean cpmCheckMPPSClientClassValue (AdsppmCachePartitionManagerType* self)
{
   boolean retVal = TRUE;
      
    return retVal;
}

// Returns  current cache partition size
// 
Adsppm_Status CPM_GetCachePartitionSize(uint32 *cachesize)
{
   return Adsppm_Status_Success;		
}


// Update the cache partition configuration based on the current mix of active
// client classes.
// Assume lock already obtained
static void cpmUpdatePartition(AdsppmCachePartitionManagerType* self)
{
    return;
}

// Check for change in active client classes, and if there is a change, select
// a new cache partitioning configuration based on the new mix of client
// classes
static void cpmOnClientManagerUpdate(AdsppmClientManagerEventId eventId,
    void* param)
{
    return;
}

static Adsppm_Status cpmDoLineLocking(
    const MmpmL2CacheLineLockParameterType* lockingParams)
{

    Adsppm_Status status = Adsppm_Status_Success;
    return status;

}

///////////////////////////////////////////////////////////////////////////////
// Public functions 
///////////////////////////////////////////////////////////////////////////////

uint32 CPM_GetMPPSThresholdValue (MmpmClientClassType *clientClassValue )
{
   uint32 mppsThreshold = 0;
   
   return mppsThreshold;
}
// Initialize the Cache Partition Manager.
// Requires that the Client Manager and ASIC Configuration Manager are already
// initialized.
Adsppm_Status CPM_Init(void)
{

    return Adsppm_Status_Success;

}

Adsppm_Status CPM_SetLineLocking(
    const MmpmL2CacheLineLockParameterType* oldParam,
    const MmpmL2CacheLineLockParameterType* newParam)
{

    return Adsppm_Status_Success;;
}

Adsppm_Status CPM_RegisterEvent(AdsppmCachePartitionManagerEventId event,
    AdsppmCacheEventHandler handler, void* param)
{

    return Adsppm_Status_Success;

}

Adsppm_Status CPM_UnregisterEvent(AdsppmCachePartitionManagerEventId event,
    AdsppmCacheEventHandler handler)
{

    return Adsppm_Status_Success;

}

