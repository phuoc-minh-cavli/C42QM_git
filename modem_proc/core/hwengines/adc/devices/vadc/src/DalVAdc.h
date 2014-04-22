#ifndef DALVADC_H
#define DALVADC_H
/*============================================================================
  @file DalVAdc.h
 
  Function and data structure declarations for VADC DAL
 
 
                Copyright (c) 2008-2013, 2018 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/core.mpss/3.10/hwengines/adc/devices/vadc/src/DalVAdc.h#2 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DALFramework.h"
#include "DDIAdcDevice.h"
#include "DDISpmi.h"
#include "npa.h"
#include "AdcScalingUtil.h"
#include "VAdcHal.h"
#include "pmic.h"
#include "VAdcLog.h"
#include "pm_gpio.h"
#include "timer.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define VADC_MAX_NUM_DEVICES 2

enum
{
   VADC_EVENT_DEFAULT = 0,
   VADC_EVENT_TIMEOUT,
   _VADC_NUM_EVENTS
};



/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
/*
 * Declaring VAdc data types
 */
typedef enum
{
   VADC_REQUEST_MODE_IDLE,
   VADC_REQUEST_MODE_READ
} VAdcRequestModeType;


typedef enum
{
   VADC_SCALE_TO_MILLIVOLTS = 0,
   VADC_SCALE_DIE_TEMP_TO_MILLIDEGREES,
   VADC_SCALE_CHG_TEMP_TO_MILLIDEGREES,
   VADC_SCALE_INTERPOLATE_FROM_MILLIVOLTS,
   VADC_SCALE_THERMISTOR,
   VADC_SCALE_RESISTOR_DIVIDER
} VAdcScalingMethodType;


typedef struct
{
   uint32 uNumerator;
   uint32 uDenominator;
} VAdcChannelScalingFactor;



typedef struct
{
   const char *pName;
   VAdcAmuxChannelSelectType uAdcHardwareChannel;
   VAdcSettlingDelay eSettlingDelay;
   VAdcFastAverageModeType eFastAverageMode;
   VAdcDecimationRatioType eDecimationRatio;
   VAdcCalMethodType eCalMethod;
   VAdcChannelScalingFactor scalingFactor;
   VAdcScalingMethodType eScalingMethod;
   uint32 uPullUp;
   const AdcIntTableType *pIntTable;
} VAdcChannelConfigType;


typedef struct
{
   const char *pszName;
   const char *pszLowName;
   const char *pszMedName;
   const char *pszHighName;
   int32 nMedToLow;
   int32 nLowToMed;
   int32 nHighToMed;
   int32 nMedToHigh;
} VAdcAveragedChannelConfigType;

typedef struct
{
   const uint8 *paucChannels;
   uint32 uNumChannels;
   pm_gpio_perph_index eGpio;
} VAdcGpioChannelMappingType;

typedef enum
{
   VADC_AVERAGED_CHANNEL_STATE_UNKNOWN = 0,
   VADC_AVERAGED_CHANNEL_STATE_LOW,
   VADC_AVERAGED_CHANNEL_STATE_LOW_MED,
   VADC_AVERAGED_CHANNEL_STATE_MED,
   VADC_AVERAGED_CHANNEL_STATE_MED_HIGH,
   VADC_AVERAGED_CHANNEL_STATE_HIGH
} VAdcAveragedChannelStateType;


typedef struct
{
   uint32 uLowChannel;
   uint32 uMedChannel;
   uint32 uHighChannel;
   VAdcAveragedChannelStateType eState;
} VAdcAveragedChannelType;




/*
 * VAdcBspType
 */
typedef struct
{
   const VAdcChannelConfigType *paChannels;
   const uint32 uNumChannels;
   const VAdcAveragedChannelConfigType *paAveragedChannels;
   const uint32 *puNumAveragedChannels;
   const VAdcGpioChannelMappingType *paGpioChannelMappings;
   uint32 uNumGpioChannelMappings;
   uint32 uFullScale_code;
   uint32 uFullScale_uV;
   uint32 uReadTimeout_us;
   uint32 uLDOSettlingTime_us;
   const uint32 uSlaveId;
   const uint32 uMasterID;
   const uint32 uPeripheralID;
   const SpmiBus_AccessPriorityType eAccessPriority;
   const uint32 uPmicDevice;
   boolean bUsesInterrupts;
} VAdcBspType;

/*
 * Declaring a "VAdc" Driver,Device and Client Context
 */
typedef struct VAdcDrvCtxt VAdcDrvCtxt;
typedef struct VAdcDevCtxt VAdcDevCtxt;
typedef struct VAdcClientCtxt VAdcClientCtxt;

/*
 * Declaring a private "VAdc" Vtable
 */
typedef struct VAdcDALVtbl VAdcDALVtbl;
struct VAdcDALVtbl
{
  int (*VAdc_DriverInit)(VAdcDrvCtxt *);
  int (*VAdc_DriverDeInit)(VAdcDrvCtxt *);
};

typedef struct
{
   VAdcDevCtxt *pDevCtxt;
} VAdcHalInterfaceCtxtType;

struct VAdcDevCtxt
{
   //Base Members
   uint32   dwRefs;                                    
   DALDEVICEID DevId;                                  
   uint32   dwDevCtxtRefIdx; 
   VAdcDrvCtxt  *pVAdcDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   //VAdc Dev state can be added by developers here
   VAdcDebugInfoType vAdcDebug;                     /* debug structure */
   const VAdcBspType   *pBsp;                       /* pointer to the BSP */
   DALBOOL bHardwareSupported;                      /* flag to indicate if the hardware is supported */
   VAdcHalInterfaceCtxtType VAdcHalInterfaceCtxt;   /* VADC HAL interface context */
   VAdcHalInterfaceType iVAdcHalInterface;          /* VADC HAL interface */
   DALSYSSyncHandle     hSync;                      /* synchronization object */
   DALSYS_SYNC_OBJECT(syncObject);
   VAdcRevisionInfoType revisionInfo;               /* VADC revision info */
   DALBOOL bCalibrated;                             /* indicates if the VADC has been calibrated */
   VAdcRequestModeType eCurrentRequestMode;         /* type of current request */
   uint32 uCurrentChannelIdx;                       /* index to current device channel being processed */
   DalDeviceHandle      *phSpmiDev;                 /* handle to the SPMI DAL device */
   DALSYSEventHandle    hClientEvent;               /* client event to be called when current conversion or calibration is complete */
   DALSYSWorkLoopHandle hAdcOpWorkLoop;             /* work loop used to perform ADC operations */
   DALSYSEventHandle    hAdcOpEvent;                /* work loop event to perform ADC operations */
   uint32 uEventParam;                              /* event parameter value to pass to client when conversion is complete */
   AdcDeviceChannelStatusType guardedDeviceStatus;  /* the current status of the VADC device channels - protect with hSync */
   DALSYSEventHandle ahEocEvent[2];                 /* end-of-conversion event - EOC signal & a timeout */
   npa_client_handle hNPACpuLatency;
   VAdcAveragedChannelType *paAveragedChannels;
   uint32 uTotalNumChannels;
   
};

struct VAdcDrvCtxt
{
   //Base Members
   VAdcDALVtbl VAdcDALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   VAdcDevCtxt VAdcDevCtxt[VADC_MAX_NUM_DEVICES];
   //VAdc Drv state can be added by developers here
};

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/*
 * Declaring a "VAdc" Client Context
 */
struct VAdcClientCtxt
{
   //Base Members
   uint32  dwRefs;                     
   uint32  dwAccessMode;  
   void *pPortCtxt;
   VAdcDevCtxt *pVAdcDevCtxt;            
   DalAdcDeviceHandle DalAdcDeviceHandle; 
   //VAdc Client state can be added by developers here
};

DALResult VAdc_DriverInit(VAdcDrvCtxt *);
DALResult VAdc_DriverDeInit(VAdcDrvCtxt *);
DALResult VAdc_DeviceInit(VAdcClientCtxt *);
DALResult VAdc_DeviceDeInit(VAdcClientCtxt *);
DALResult VAdc_Reset(VAdcClientCtxt *);
DALResult VAdc_PowerEvent(VAdcClientCtxt *, DalPowerCmd, DalPowerDomain);
DALResult VAdc_Open(VAdcClientCtxt *, uint32);
DALResult VAdc_Close(VAdcClientCtxt *);
DALResult VAdc_Info(VAdcClientCtxt *,DalDeviceInfo *, uint32);
DALResult VAdc_InheritObjects(VAdcClientCtxt *, DALInheritSrcPram *, DALInheritDestPram *);

/*
 * Functions specific to DalAdcDevice interface
 */
DALResult VAdc_GetDeviceProperties(VAdcClientCtxt *, AdcDeviceDevicePropertiesType *);
DALResult VAdc_GetChannelStatus(VAdcClientCtxt *, uint32, AdcDeviceChannelStatusType *);
DALResult VAdc_StartConversion(VAdcClientCtxt *, uint32, const DALSYSEventHandle, uint32);
DALResult VAdc_GetChannel(VAdcClientCtxt *, const char *, uint32 *);



#endif /* #ifndef DALVADC_H */

