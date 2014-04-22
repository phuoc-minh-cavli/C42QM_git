#ifndef __DAL_TSENS_H__
#define __DAL_TSENS_H__
/**
  @file DALTsens.h

  Function and data structure declarations for TSENS DAL.

  This file contains data structures and functions used to configure, control,
  and query temperature sensors.
*/
/*============================================================================
               Copyright (c) 2010-2012, 2014-2015, 2019 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/core.tx/6.0/hwengines/tsens/dal/v1/DALTsens.h#1 $ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define TSENS_MAX_NUM_DEVICES 1

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DALFramework.h"
#include "DDITsens.h"
#include "DDIInterruptController.h"
#include "DALQueue.h"
#include "DDIHWIO.h"
#include "TsensBsp.h"
#include <qurt_mutex.h>

#include "dsr.h"




/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

#define _TSENS_NUM_CLIENTS 3
#define TSENS_BUSY_WAIT_TIME 25

#define TSENS_THRESHOLD_UPPER_IDX           ((uint32)TSENS_THRESHOLD_UPPER)
#define TSENS_THRESHOLD_LOWER_IDX           ((uint32)TSENS_THRESHOLD_LOWER)
#define TSENS_NUM_THRESHOLD_IDXS            ((uint32)_TSENS_NUM_THRESHOLDS)

/* Declaring a Tsens driver, device and client context */
typedef struct TsensDrvCtxt TsensDrvCtxt;
typedef struct TsensDevCtxt TsensDevCtxt;
typedef struct TsensClientCtxt TsensClientCtxt;

/* Declaring a private Tsens Vtable */
typedef struct TsensDALVtbl TsensDALVtbl;
struct TsensDALVtbl
{
   int (*Tsens_DriverInit)(TsensDrvCtxt *);
   int (*Tsens_DriverDeInit)(TsensDrvCtxt *);
};

/* Tsens device context */
struct TsensDevCtxt
{
   // Base Members
   uint32 dwRefs;
   DALDEVICEID DevId;
   uint32 dwDevCtxtRefIdx;
   TsensDrvCtxt *pTsensDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   // Tsens Dev state can be added by developers here
   DALInterruptID InterruptId;
};

/* Tsens driver context */
struct TsensDrvCtxt
{
   // Base Members
   TsensDALVtbl TsensDALVtbl;
   uint32 dwNumDev;
   uint32 dwSizeDevCtxt;
   uint32 bInit;
   uint32 dwRefs;
   TsensDevCtxt TsensDevCtxt[TSENS_MAX_NUM_DEVICES];
};

/* Tsens client context */
struct TsensClientCtxt
{
   // Base Members
   uint32 dwRefs;
   uint32 dwAccessMode;
   void *pPortCtxt;
   TsensDevCtxt *pTsensDevCtxt;
   DalTsensHandle DalTsensHandle;
   uint32 nClient;
};



/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef enum
{
   TSENS_THRESHOLD_STATE_DISABLED = 0,
   TSENS_THRESHOLD_STATE_ENABLED = 1,
   TSENS_THRESHOLD_STATE_TRIGGERED = 2
} TsensThresholdStateType;

typedef struct
{
   int32 nThresholdTemp[TSENS_MAX_NUM_SENSORS][TSENS_NUM_THRESHOLD_IDXS];
   uint32 uThresholdCode[TSENS_MAX_NUM_SENSORS][TSENS_NUM_THRESHOLD_IDXS];
   TsensThresholdStateType eThresholdState[TSENS_MAX_NUM_SENSORS][TSENS_NUM_THRESHOLD_IDXS];
   TsensThresholdCbType pfnCb[TSENS_MAX_NUM_SENSORS][_TSENS_NUM_THRESHOLDS];
   void *pCbCtxt[TSENS_MAX_NUM_SENSORS][_TSENS_NUM_THRESHOLDS];
   DALBOOL bRegistered;
   DALBOOL bEnableThresholds;
   TsensCallbackPayloadType *tsensCbPayload[TSENS_MAX_NUM_SENSORS][_TSENS_NUM_THRESHOLDS];
} TsensClientInfoType;

typedef enum
{
   TSENS_DEVICE_STATE_INIT = 0,
   TSENS_DEVICE_STATE_ERROR,
   TSENS_DEVICE_STATE_READY,
   TSENS_DEVICE_STATE_ISR
} TsensDeviceStateType;

typedef struct
{
   uint32 uNumSensors;
   TsensClientInfoType clients[_TSENS_NUM_CLIENTS];
   TsensTempRangeType tempRange[TSENS_MAX_NUM_SENSORS];
   uint32 uSensorsEnabledMask;
   TsensDeviceStateType eDeviceState;
   TsensBspType *pBsp;
   qurt_mutex_t hSync;
   //DALSYS_SYNC_OBJECT(syncObject);
   DalDeviceHandle *phHWIO;
   DALSYSEventHandle hTsensInterruptEvent;
   DALSYSWorkLoopHandle hTsensWorkLoop;
   DALBOOL bDataReady;
   dsr_obj_t *pDsr;
} TsensDeviceDataType;


/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/* Functions specific to the DAL interface */
DALResult Tsens_DalTsens_Attach(const char *, DALDEVICEID, DalDeviceHandle **);
DALResult Tsens_DriverInit(TsensDrvCtxt *);
DALResult Tsens_DriverDeInit(TsensDrvCtxt *);
DALResult Tsens_DeviceInit(TsensClientCtxt *);
DALResult Tsens_DeviceDeInit(TsensClientCtxt *);
DALResult Tsens_RegisterClient(TsensClientCtxt *);
DALResult Tsens_DeregisterClient(TsensClientCtxt *);

/* Functions specific to the DalTsens interface */
DALResult Tsens_GetTemp(TsensClientCtxt *, uint32, TsensTempType *);
DALResult Tsens_GetTempRange(TsensClientCtxt *, uint32, TsensTempRangeType *);
DALResult Tsens_SetThreshold(TsensClientCtxt *, uint32, TsensThresholdType, int32, TsensThresholdCbType pfnCallback, void *pCbCtxt);
DALResult Tsens_SetEnableThresholds(TsensClientCtxt *, DALBOOL bThresholdsEnable);
DALResult Tsens_GetNumSensors(TsensClientCtxt *, uint32 *);
DALResult Tsens_CheckCalibration(TsensClientCtxt *, uint32);

#endif

