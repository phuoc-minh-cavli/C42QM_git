#ifndef DDICTI_H
#define DDICTI_H 

/****************************************************************************
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 *
 * FILE:            core/debugtrace/cti/dal/inc/DDICTI.h
 *
 * DESCRIPTION:     DAL Device Interface for CTI.
 *
 *                  Declares CTI function prototypes.
 *
 *
 * EDIT HISTORY FOR MODULE
 * --------------------------------------------------------------------------
 *
 * when         who         what, where, why
 * ----------   -------     -----------------
 * 2014-01-24   rpoddar     Initial revision.
 *
 *****************************************************************************/

/****************************************************************************
 * Include Files
 ****************************************************************************/

#include "DalDevice.h"

/****************************************************************************
 * Preprocessor Definitions and Constants
 ****************************************************************************/

#define DALCTI_STRING_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)

#define DAL_CTIDeviceAttach(DevId, hDalDevice) \
        DAL_StringDeviceAttachEx(NULL, DevId, DALCTI_STRING_INTERFACE_VERSION, hDalDevice)

#define TRIGIN  0
#define TRIGOUT 1

#define DAL_CTI_SUCCESS         0x0
#define DAL_CTI_ERROR           -1
#define DAL_CTI_INVALID_BLOCK   0x2
#define DAL_CTI_INVALID_TYPE    0x3
#define DAL_CTI_INVALID_TRIGGER 0x4
#define DAL_CTI_INVALID_CHANNEL 0x5


/****************************************************************************
 * Data Structure & Function Definitions
 ****************************************************************************/

typedef struct DalCTI DalCTI;
struct DalCTI
{
    struct DalDevice    DalDevice;
    DALResult           (*Map)(DalDeviceHandle *h,
                                    const char  *block,
                                    uint8       type,
                                    uint8       trigger,
                                    uint8       channel);
    DALResult           (*Unmap)(DalDeviceHandle *h,
                                    const char  *block,
                                    uint8       type,
                                    uint8       trigger,
                                    uint8       channel);
    DALResult           (*Reset)(DalDeviceHandle *h, const char *block);
    DALResult           (*Query)(DalDeviceHandle *h,
                                    const char  *block,
                                    uint8       type,
                                    uint8       max_triggers,
                                    uint8       *length,
                                    __packed uint32 *mappings);
   DALResult           (*Trigger)(DalDeviceHandle *h,
                                    const char  *block,
                                uint8       channel,
                                uint8       trigger_type);

};

typedef struct DalCTIHandle DalCTIHandle;
struct DalCTIHandle
{
    uint32              dwDalHandleId;
    const struct DalCTI *pVtbl;
    void                *pClientCtxt;
};


static __inline DALResult DALCTI_Map(DalDeviceHandle *h,
                                        const char  *block,
                                        uint8       type,
                                        uint8       trigger,
                                        uint8       channel)
{
    return ((struct DalCTIHandle *) h)->pVtbl->Map(h,
                                                    block,
                                                    type,
                                                    trigger,
                                                    channel);
}

static __inline DALResult DALCTI_Unmap(DalDeviceHandle *h,
                                        const char  *block,
                                        uint8       type,
                                        uint8       trigger,
                                        uint8       channel)
{
    return ((struct DalCTIHandle *) h)->pVtbl->Unmap(h,
                                                        block,
                                                        type,
                                                        trigger,
                                                        channel);
}

static __inline DALResult DALCTI_Reset(DalDeviceHandle *h, const char *block)
{
    return ((struct DalCTIHandle *) h)->pVtbl->Reset(h, block);
}

static __inline DALResult DALCTI_Query(DalDeviceHandle *h,
                                        const char  *block, 
                                        uint8       type,
                                        uint8       max_triggers,
                                        uint8       *length,
                                        __packed uint32 *mappings)
{
    return ((struct DalCTIHandle *) h)->pVtbl->Query(h,
                                                        block,
                                                        type,
                                                        max_triggers,
                                                        length,
                                                        mappings);
}

static __inline DALResult DALCTI_Trigger(DalDeviceHandle *h,
                                        const char  *block,
                                       uint8       channel,
                                       uint8       trigger_type)
{
    return ((struct DalCTIHandle *) h)->pVtbl->Trigger(h,
                                                    block,
                                                     channel,
                                                     trigger_type);
}


#endif // DDICTI_H
