#ifndef __DALADC_H__
#define __DALADC_H__
/*============================================================================
  @file DalAdc.h

  Function and data structure declarations for ADC DAL


               Copyright (c) 2008-2013, 2015, 2018 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/core.tx/6.0/hwengines/adc/dal/src/DalAdc.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DALFramework.h"
#include "DDIAdc.h"
#include "DALQueue.h"
#include <qurt_mutex.h>

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define ADC_MAX_NUM_DEVICES 1

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
ADC BSP structure
------------------------------------------------------------------------------*/
typedef struct
{
   const char *pszWorkLoopName;
   uint32 uQueueSize;
} AdcQueueConfigType;

typedef struct
{
   const char *pszDevName;
   uint32 uQueueIdx;
} AdcPhysicalDeviceType;

typedef struct
{
   const AdcPhysicalDeviceType *paAdcPhysicalDevices;
   uint32 uNumPhysicalDevices;
   const AdcQueueConfigType *paAdcQueueConfigs;
   uint32 uNumQueues;
} AdcBspType;

/*------------------------------------------------------------------------------
Declaring an "Adc" Driver, Device and Client Context
------------------------------------------------------------------------------*/
typedef struct AdcDrvCtxt AdcDrvCtxt;
typedef struct AdcDevCtxt AdcDevCtxt;
typedef struct AdcClientCtxt AdcClientCtxt;
typedef struct AdcDeviceDataType AdcDeviceDataType;

/*------------------------------------------------------------------------------
Declaring a private "Adc" Vtable
------------------------------------------------------------------------------*/
typedef struct AdcDALVtbl AdcDALVtbl;
struct AdcDALVtbl
{
  int (*Adc_DriverInit)(AdcDrvCtxt *);
  int (*Adc_DriverDeInit)(AdcDrvCtxt *);
};

typedef enum
{
   ADC_REQUEST_CONVERSION
   
} AdcDeviceRequestType;

typedef struct
{
   DALFW_Q_LinkType link;
   AdcDeviceRequestType eAdcDeviceRequest;
   DALSYSEventHandle hClientEvent;
   uint32 uDeviceIdx;
   uint32 uChannelIdx;
   AdcRequestStatusType adcRequestStatus;
} AdcDeviceQueueNodeType;

typedef enum
{
  ADC_DEVICE_NOT_AVAILABLE,
  ADC_DEVICE_AVAILABLE
} AdcDeviceStatusType;

typedef struct
{
   AdcDevCtxt *pAdcDevCtxt;               /* ADC device context */
   AdcDeviceQueueNodeType *paNodes;       /* nodes */
   DALFW_Q_Type qFree;                    /* nodes that are free */
   DALFW_Q_Type qQueued;                  /* nodes that are waiting to be processed */
   DALFW_Q_Type qPending;                 /* queue for the node that is being processed */
   DALSYSWorkLoopHandle hAdcOpWorkLoop;   /* work loop used to perform ADC operations */
   DALSYSEventHandle hAdcOpEvent;         /* work loop event to perform ADC operations */
   qurt_mutex_t hSync;                 /* synchronization object handle */
   //DALSYS_SYNC_OBJECT(syncObject);        /* syn object */
   uint32 uQueueSize;                     /* queue size */
} AdcQueueType;

struct AdcDeviceDataType
{
   DalDeviceHandle *phDevice;         /* DAL device handle */
   uint32 uNumChannels;               /* number of channels supported by the ADC device */
   AdcDeviceStatusType deviceStatus;  /* device status */
   DALBOOL bDeviceIsOpen;             /* whether the device is open */
};

struct AdcDevCtxt
{
   // Base Members
   uint32 dwRefs;
   DALDEVICEID DevId;
   uint32 dwDevCtxtRefIdx;
   AdcDrvCtxt *pAdcDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   // Adc Dev state can be added by developers here
   const AdcBspType *pBsp;                /* DAL ADC BSP */
   qurt_mutex_t hSync;                /* synchronization object handle */
   //DALSYS_SYNC_OBJECT(syncObject);        /* syn object */
   AdcDeviceDataType *paDeviceData;       /* device data array */
   AdcQueueType *paQueues;                /* array of ADC queues */
};

struct AdcDrvCtxt
{
   // Base Members
   AdcDALVtbl AdcDALVtbl;
   uint32 dwNumDev;
   uint32 dwSizeDevCtxt;
   uint32 bInit;
   uint32 dwRefs;
   AdcDevCtxt AdcDevCtxt[ADC_MAX_NUM_DEVICES];
   // Adc Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "Adc" Client Context
------------------------------------------------------------------------------*/
struct AdcClientCtxt
{
   // Base Members
   uint32 dwRefs;
   uint32 dwAccessMode;
   void *pPortCtxt;
   AdcDevCtxt *pAdcDevCtxt;
   DalAdcHandle DalAdcHandle;
   // Adc Client state can be added by developers here
   uint32 uClientId;
};

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions specific to Dal interface
------------------------------------------------------------------------------*/
DALResult Adc_DalAdc_Attach(const char *, DALDEVICEID, DalDeviceHandle **);
DALResult Adc_DriverInit(AdcDrvCtxt *);
DALResult Adc_DriverDeInit(AdcDrvCtxt *);
DALResult Adc_DeviceInit(AdcClientCtxt *);
DALResult Adc_DeviceDeInit(AdcClientCtxt *);
DALResult Adc_PowerEvent(AdcClientCtxt *, DalPowerCmd, DalPowerDomain);
DALResult Adc_Open(AdcClientCtxt *, uint32);
DALResult Adc_Close(AdcClientCtxt *);
DALResult Adc_Info(AdcClientCtxt *,DalDeviceInfo *, uint32);

/*------------------------------------------------------------------------------
Functions specific to DalAdc interface
------------------------------------------------------------------------------*/
DALResult Adc_GetAdcInputProperties(AdcClientCtxt *, const char *, uint32 , AdcInputPropertiesType *);
DALResult Adc_Read(AdcClientCtxt *, const AdcInputPropertiesType *, AdcResultType *);
DALResult Adc_RegisterClient(AdcClientCtxt *);
DALResult Adc_DeregisterClient(AdcClientCtxt *);

#endif /* #ifndef __DALADC_H__ */

