#ifndef VADCHAL_H
#define VADCHAL_H
/*============================================================================
  @file VAdcHal.h
 
  Function and data structure declarations for VADC HAL
 
 
                Copyright (c) 2012, 2018 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/core.mpss/3.10/hwengines/adc/devices/vadc/src/VAdcHal.h#2 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "com_dtypes.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define VADC_HAL_NUM_REGISTERS_TO_DUMP 35

#define VADC_INVALID_ADC_CODE 0x8000

#define ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))



/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef struct 
{
	uint8 uDigitalMajor;
	uint8 uDigitalMinor;
	uint8 uAnalogMajor;
	uint8 uAnalogMinor;
	uint8 uType;
	uint8 uSubType;
} VAdcRevisionInfoType;


typedef enum
{
   VADC_CONVERSION_STATUS_INVALID = 0,
   VADC_CONVERSION_STATUS_COMPLETE,
   VADC_CONVERSION_STATUS_PENDING,
   VADC_CONVERSION_STATUS_WAITING
} VAdcConversionStatusType;


typedef struct 
{
   VAdcConversionStatusType eConversionStatus;
   
} VAdcStatusType;

typedef enum
{
   VADC_DISABLE = 0,
   VADC_ENABLE
} VAdcEnableType;

typedef enum
{
   VADC_DECIMATION_RATIO_256 = 0,
   VADC_DECIMATION_RATIO_512,
   VADC_DECIMATION_RATIO_1024,
   VADC_DECIMATION_RATIO_2048,
   VADC_DECIMATION_RATIO_4096
} VAdcDecimationRatioType;


typedef uint32 VAdcAmuxChannelSelectType;

typedef enum
{
   VADC_SETTLING_DELAY_0_US = 0,
   VADC_SETTLING_DELAY_100_US,
   VADC_SETTLING_DELAY_200_US,
   VADC_SETTLING_DELAY_300_US,
   VADC_SETTLING_DELAY_400_US,
   VADC_SETTLING_DELAY_500_US,
   VADC_SETTLING_DELAY_600_US,
   VADC_SETTLING_DELAY_700_US,
   VADC_SETTLING_DELAY_800_US,
   VADC_SETTLING_DELAY_900_US,
   VADC_SETTLING_DELAY_1_MS,
   VADC_SETTLING_DELAY_2_MS,
   VADC_SETTLING_DELAY_4_MS,
   VADC_SETTLING_DELAY_6_MS,
   VADC_SETTLING_DELAY_8_MS,
   VADC_SETTLING_DELAY_10_MS,
   VADC_SETTLING_DELAY_16_MS,
   VADC_SETTLING_DELAY_32_MS,
   VADC_SETTLING_DELAY_64_MS,
   VADC_SETTLING_DELAY_128_MS
} VAdcSettlingDelay;

typedef uint32 VAdcConversionCodeType;


typedef enum
{
   VADC_FAST_AVERAGE_1_SAMPLE = 0,
   VADC_FAST_AVERAGE_2_SAMPLES,
   VADC_FAST_AVERAGE_4_SAMPLES,
   VADC_FAST_AVERAGE_8_SAMPLES,
   VADC_FAST_AVERAGE_16_SAMPLES,
   VADC_FAST_AVERAGE_32_SAMPLES,
   VADC_FAST_AVERAGE_64_SAMPLES,
   VADC_FAST_AVERAGE_128_SAMPLES,
   VADC_FAST_AVERAGE_256_SAMPLES,
   VADC_FAST_AVERAGE_512_SAMPLES,
   VADC_FAST_AVERAGE_NONE = 0xFFFF
} VAdcFastAverageModeType;

typedef enum
{
   VADC_CAL_METHOD_NO_CAL = 0,
   VADC_CAL_METHOD_RATIOMETRIC,
   VADC_CAL_METHOD_ABSOLUTE
} VAdcCalMethodType;


typedef struct
{
   VAdcAmuxChannelSelectType uChannel;
   VAdcDecimationRatioType eDecimationRatio;
   VAdcSettlingDelay eSettlingDelay;
   VAdcFastAverageModeType eFastAverageMode;
   VAdcCalMethodType eCalMethod;
} VAdcConversionParametersType;


typedef enum
{
   VADC_INTERRUPT_CONFIG_LEVEL_HIGH = 0,
   VADC_INTERRUPT_CONFIG_LEVEL_LOW,
   VADC_INTERRUPT_CONFIG_RISING_EDGE,
   VADC_INTERRUPT_CONFIG_FALLING_EDGE,
} VAdcInterruptConfigType;

typedef struct
{
   VAdcInterruptConfigType eEocInterruptConfig;
   
} VAdcInterruptsConfigType;

typedef enum
{
   VADC_INTERRUPT_MASK_BIT_EOC = 0x1,
   VADC_INTERRUPT_MASK_BIT_FIFO_NOT_EMPTY = 0x2,
   VADC_INTERRUPT_MASK_BIT_SEQUENCER_TIMEOUT = 0x4,
   VADC_INTERRUPT_MASK_BIT_HIGH_THRESHOLD = 0x8,
   VADC_INTERRUPT_MASK_BIT_LOW_THRESHOLD = 0x10,
} VAdcInterruptMaskBitsType;

typedef uint32 VAdcInterruptMaskType;

typedef uint32 VAdcInterruptMid;

typedef struct
{
   uint8 u8Offset;
   uint8 u8Val;
} VAdcHalRegType;

typedef struct
{
   VAdcHalRegType aVAdcReg[VADC_HAL_NUM_REGISTERS_TO_DUMP];
} VAdcHalRegDumpType;

typedef enum
{
   VADC_HAL_SUCCESS = 0,
   VADC_HAL_ERROR = 1
} VAdcHalResultType;

typedef struct
{
   void *pCtxt;
   VAdcHalResultType(* pfnWriteBytes)(void *pCtxt, uint32 uRegisterAddress, unsigned char* pucData, uint32 uDataLen);
   VAdcHalResultType(* pfnReadBytes)(void *pCtxt, uint32 uRegisterAddress, unsigned char* pucData, uint32 uDataLen, uint32* puTotalBytesRead);
   VAdcRevisionInfoType revisionInfo;               /* VADC revision info */
} VAdcHalInterfaceType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
VAdcHalResultType VAdcHalGetRevisionInfo(VAdcHalInterfaceType *piVAdcHal, VAdcRevisionInfoType *pRevisionInfo);
VAdcHalResultType VAdcHalGetStatus(VAdcHalInterfaceType *piVAdcHal, VAdcStatusType *pStatus);
VAdcHalResultType VAdcHalConfigInterrupts(VAdcHalInterfaceType *piVAdcHal, VAdcInterruptsConfigType *pConfigs);
VAdcHalResultType VAdcHalGetPendingInterrupts(VAdcHalInterfaceType *piVAdcHal, VAdcInterruptMaskType *puIntrMask);
VAdcHalResultType VAdcHalClearInterrupts(VAdcHalInterfaceType *piVAdcHal, VAdcInterruptMaskType uIntrMask);
VAdcHalResultType VAdcHalEnableInterrupts(VAdcHalInterfaceType *piVAdcHal, VAdcInterruptMaskType uIntrMask);
VAdcHalResultType VAdcHalSetInterruptMid(VAdcHalInterfaceType *piVAdcHal, VAdcInterruptMid uIntrMid);
VAdcHalResultType VAdcHalSetEnable(VAdcHalInterfaceType *piVAdcHal, VAdcEnableType eEnable);
VAdcHalResultType VAdcHalRequestConversion(VAdcHalInterfaceType *piVAdcHal,VAdcConversionParametersType *pParams, uint32 *puConvTimeUs);
VAdcHalResultType VAdcHalGetConversionCode(VAdcHalInterfaceType *piVAdcHal, uint32 *puCode);
VAdcHalResultType VAdcHalDumpRegisters(VAdcHalInterfaceType *piVAdcHal, VAdcHalRegDumpType *pVAdcRegDump);

#endif /* #ifndef VADCHAL_H */

