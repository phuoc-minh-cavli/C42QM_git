#ifndef _I2CLOG_H_
#define _I2CLOG_H_
/*
===========================================================================

FILE:   I2cLog.h

DESCRIPTION:
    This file contains the API for the I2C Log services 


===========================================================================

        Edit History

$Header: //components/rel/core.tx/6.0/buses/i2c/src/logs/inc/I2cLog.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
09/01/16 PR     Added support for MDM9x07
07/10/12 LK     Created

===========================================================================
        Copyright c 2012-2016 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "I2cTransfer.h"
#include "I2cTransferCfg.h"
#include "I2cTypes.h"
#include "I2cError.h"
#include "stdarg.h"
#include "ULogFront.h"


struct I2CLOG_DevType
{
   uint32         uPlatDevId;
   ULogHandle     gI2cDynLogHandle;
   uint32         I2cDynLogBufSize;
   uint32         uDevAddr;
   char          *pDevName;

   uint32         uI2cCallDataLogLevel;
   uint32         uI2cCallTraceLogLevel;
   uint32         uEnableQupRegWriteLogging;
   uint32         uEnableQupRegReadLogging;

   struct I2CLOG_DevType *pNext;
};

typedef struct I2CLOG_DevType I2CLOG_DevType;


extern void I2cLog_AssociateBlockAddress(I2CLOG_DevType *pLogDev, uint32 uDevAddr);
extern void I2cLog_Init(uint32 uPlatDevId, I2CLOG_DevType **ppLogDev);
extern void I2cLog_DeInit(I2CLOG_DevType *pLogDev);


#define I2C_ERROR_LOG_LEVEL0(pDevLog, args...)\
   if(   ((pDevLog) != NULL) )\
    {\
         ULogFront_RealTimePrintf((pDevLog)->gI2cDynLogHandle, ##args);\
   	}

#define I2C_CALLTRACE_LEVEL1(pDevLog, args...) \
    if(   ((pDevLog) != NULL)\
       && ((pDevLog)->uI2cCallTraceLogLevel >= 1))\
    {\
       ULogFront_RealTimePrintf((pDevLog)->gI2cDynLogHandle, ##args);\
    }

#define I2C_CALLTRACE_LEVEL2(pDevLog, args...) \
    if(   ((pDevLog) != NULL)\
       && ((pDevLog)->uI2cCallTraceLogLevel >= 2))\
    {\
       ULogFront_RealTimePrintf((pDevLog)->gI2cDynLogHandle, ##args);\
    }

#define I2C_CALLDATA_LEVEL1(pDevLog, args...) \
    if(   ((pDevLog) != NULL)\
       && ((pDevLog)->uI2cCallDataLogLevel >= 1))\
    {\
       ULogFront_RealTimePrintf((pDevLog)->gI2cDynLogHandle, ##args);\
    }

#define I2C_CALLDATA_LEVEL2(pDevLog, args...) \
    if(   ((pDevLog) != NULL)\
       && ((pDevLog)->uI2cCallDataLogLevel >= 2))\
    {\
       ULogFront_RealTimePrintf((pDevLog)->gI2cDynLogHandle, ##args);\
    }

/** @brief logs the transfers in a sequence.

    @param[in] pLogDev        Pointer to log structure.
    @param[in] pClntCfg       Pointer to Client configuration.
  
    @return                   Nothing.
  */
void
I2CLOG_LogCltCfgData
(
   I2CLOG_DevType                      *pLogDev,
   I2cClientConfig                     *pClntCfg         
);

/** @brief logs the transfers in a sequence.

    @param[in] pLogDev        Pointer to log structure.
    @param[in] pTransfer      Pointer to transfer data.
  
    @return                   Nothing.
  */
void
I2CLOG_LogTransferData
(
   I2CLOG_DevType                      *pLogDev,
   I2cTransfer                         *pTransfer         
);

/** @brief logs the transfers in a sequence.

    @param[in] pLogDev        Pointer to log structure.
    @param[in] pSeq           Pointer to sequence.
  
    @return                   Nothing.
  */
void
I2CLOG_LogSeqData
(
   I2CLOG_DevType                      *pLogDev,
   I2cSequence                         *pSeq         
);

/** @brief Searches for a log device in the list.
  
    @param[in] uPlatDevId    Platform id for the device.
    
    @return             Pointer to log dev if found, otherwise NULL.
  */
I2CLOG_DevType * 
I2cLog_SearchDevice
(
   uint32          uPlatDevId 
);

void I2cLog_LogBuffer
(
   I2CLOG_DevType                      *pLogDev,
   uint8                               *pBuff,
   uint8                                uBuffSize
);


#endif //_I2CLOG_H_

