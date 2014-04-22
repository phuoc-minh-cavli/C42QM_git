#ifndef __DALVADC_H__
#define __DALVADC_H__
/*============================================================================
  @file DalVAdc.h

  Function and data structure declarations for VADC DAL


                Copyright (c) 2008-2015, 2018 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/core.tx/6.0/hwengines/adc/devices/vadc/src/DalVAdc.h#1 $ */

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



/*
 * VAdcChannelConfigType:
 *
 * pName                      -- Channel name as defined in core/api/AdcInputs
 * uAdcHardwareChannel        -- PMIC hardware channel number
 * uConfigIdx                 -- VAdc Filter Configuration used for this
 *                               channel. Refer to VAdcConfigType for more
 *                               details.
 * eSettlingDelay             -- Amount of time to wait for the signal to settle
 *                               after requesting the conversion
 * eFastAverageMode           -- If enabled, will automatically take multiple
 *                               readings and average them
 * bUseSequencer              -- Whether or not to use the conversion sequencer
 *                               for readings on this channel
 * uSequencerIdx              -- Sequencer index to use if we’re using the
 *                               sequencer
 * scalingFactor              -- Since the inputs are prescaled, we must know
 *                               the hardware’s scaling factor so that we can
 *                               multiply by the appropriate amount to get the
 *                               correct (unscaled) reading value
 * eScalingMethod             -- Scaling method for converting the reading to
 *                               the appropriate units. Ratiometric is for
                                 resistor divider-style inputs like BATT_THERM.
                                 Absolute calibration should be used for signals
                                 with an absolute value, e.g. VBATT
 * pInterpolationTable        -- Scaling function (if required by the scaling
 *                               method) to convert a reading to units other
 *                               than millivolts.
 * uInterpolationTableLength  -- the length of the interpolation table
 * eCalMethod                 -- The calibration type we'll use for a
 *                               particular channel.
 *
 * eMppConfig                 -- How to map the mpp – statically or not at all
 * eMpp                       -- The source MPP to use.
 * eChSelect                  -- The AMUX channel to route the MPP to.
 *
 * MPP routing:
 *    mpp -> amux
 *     1  ->  5
 *     2  ->  6
 *     3  ->  7
 *     4  ->  8
 *     5  ->  5
 *     6  ->  6
 *     7  ->  7
 *     8  ->  8
 */
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
 * Declaring a "VAdc" Driver, device and client context
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
   // Base Members
   uint32 dwRefs;
   DALDEVICEID DevId;
   uint32 dwDevCtxtRefIdx;
   VAdcDrvCtxt *pVAdcDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   // VAdc Dev state can be added by developers here
   VAdcDebugInfoType vAdcDebug;                     /* debug structure */
   VAdcHalInterfaceCtxtType VAdcHalInterfaceCtxt;   /* VADC HAL interface context */
   VAdcHalInterfaceType iVAdcHalInterface;          /* VADC HAL interface */
   VAdcRevisionInfoType revisionInfo;               /* VADC revision info */
   pm_device_info_type pmicDeviceInfo;              /* PMIC device info */
   const VAdcBspType *pBsp;                         /* pointer to the BSP */
   DalDeviceHandle *phSpmiDev;                      /* handle to the SPMI DAL device */
   DALSYSEventHandle ahEocEvent[_VADC_NUM_EVENTS];  /* end-of-conversion event - EOC signal & a timeout */
   npa_client_handle hNPACpuLatency;                /* npa handle for min latency vote */
   npa_client_handle hNPAPmicHkadc;                 /* npa handle for VREG */
   DALBOOL bHardwareSupported;                      /* flag to indicate if the hardware is supported */
   VAdcAveragedChannelType *paAveragedChannels;
   uint32 uTotalNumChannels;
};

struct VAdcDrvCtxt
{
   // Base Members
   VAdcDALVtbl VAdcDALVtbl;
   uint32 dwNumDev;
   uint32 dwSizeDevCtxt;
   uint32 bInit;
   uint32 dwRefs;
   VAdcDevCtxt VAdcDevCtxt[VADC_MAX_NUM_DEVICES];
   // VAdc Drv state can be added by developers here
};

/*
 * Declaring a "VAdc" Client Context
 */
struct VAdcClientCtxt
{
   // Base Members
   uint32 dwRefs;
   uint32 dwAccessMode;
   void *pPortCtxt;
   VAdcDevCtxt *pVAdcDevCtxt;
   DalAdcDeviceHandle DalAdcDeviceHandle;
   // VAdc Client state can be added by developers here
};

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/* Non-DAL functions */
uint32
VAdc_GetHardwareChannel(
   VAdcDevCtxt *pDevCtxt,
   uint32 uChannelIdx
   );


void
VAdc_ProcessConversionResult(
   VAdcDevCtxt *pDevCtxt,
   uint32 uChannel,
   const VAdcChannelConfigType *pChannel,
   uint32 uCode,
   AdcDeviceResultType *pResult
   );



/* Functions specific to DAL */
DALResult VAdc_DalAdcDevice_Attach(const char *, DALDEVICEID, DalDeviceHandle **);
DALResult VAdc_DriverInit(VAdcDrvCtxt *);
DALResult VAdc_DriverDeInit(VAdcDrvCtxt *);
DALResult VAdc_DeviceInit(VAdcClientCtxt *);
DALResult VAdc_DeviceDeInit(VAdcClientCtxt *);
DALResult VAdc_Open(VAdcClientCtxt *, uint32);
DALResult VAdc_Close(VAdcClientCtxt *);

/* Functions specific to DalAdcDevice interface */
DALResult VAdc_GetDeviceProperties(VAdcClientCtxt *, AdcDeviceDevicePropertiesType *);
DALResult VAdc_GetChannel(VAdcClientCtxt *, const char *, uint32 *);
DALResult VAdc_ReadChannel(VAdcClientCtxt *, uint32, AdcDeviceResultType *);


#endif /* #ifndef __DALVADC_H__ */

