#ifndef __DDIADCDEVICE_H__
#define __DDIADCDEVICE_H__
/*============================================================================
  @file DDIAdcDevice.h

  ADC Physical Device Driver Interface header

  This header is to be included solely by the ADC DAL and implementations of
  this DDI. It provides a common interface for the ADC DAL to interface with
  different ADC hardware.

  External clients may not access these interfaces.

                Copyright (c) 2008-2015, 2018 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/core.tx/6.0/hwengines/adc/devices/common/protected/DDIAdcDevice.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DalDevice.h"
#include "DDIAdc.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define DALADCDEVICE_INTERFACE_VERSION DALINTERFACE_VERSION(4,1)

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/


typedef enum
{
   ADC_DEVICE_CHANNEL_READY,
   ADC_DEVICE_CHANNEL_BUSY,
   ADC_DEVICE_CHANNEL_CALIBRATING,
   ADC_DEVICE_CHANNEL_ERROR,
   _ADC_MAX_DEVICE_CHANNEL_STATUS = 0x7FFFFFFF
} AdcDeviceChannelStatusType;

typedef enum
{
   ADC_DEVICE_RESULT_VALID,
   ADC_DEVICE_RESULT_INVALID,
   ADC_DEVICE_RESULT_INVALID_CHANNEL,
   ADC_DEVICE_RESULT_TIMEOUT,
   ADC_DEVICE_RESULT_FIFO_NOT_EMPTY,
   _ADC_MAX_DEVICE_RESULT_STATUS = 0x7FFFFFFF
} AdcDeviceResultStatusType;

typedef struct
{
   uint32 uNumChannels;      /* number of ADC channels */
} AdcDeviceDevicePropertiesType;


typedef struct
{
   AdcDeviceResultStatusType eStatus;
   int32 nPhysical;     /* result in physical units. Units depend on BSP */
   uint32 uPercent;     /* result as percentage of reference voltage used
                         * for conversion. 0 = 0%, 65535 = 100% */
   uint32 uMicrovolts;  /* result in microvolts */
   uint32 uCode;        /* raw ADC code */
} AdcDeviceResultType;


/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
DALResult
GetInputProperties(DalDeviceHandle *h,
   /*in*/ const char *pChannelName,
   /*out*/ uint32 *puChannelIdx);

DALResult
GetDeviceProperties(DalDeviceHandle *h,
   /*out*/ AdcDeviceDevicePropertiesType *pAdcDeviceProp);



DALResult
ReadChannel(DalDeviceHandle *h,
   /*in*/ uint32 uChannelIdx,
   /*out*/ AdcDeviceResultType *pAdcDeviceReadResult);


typedef struct DalAdcDevice DalAdcDevice;
struct DalAdcDevice
{
   DalDevice DalDevice;
   DALResult (*GetInputProperties)(DalDeviceHandle * _h, const char * pChannelName, uint32 *);
   DALResult (*GetDeviceProperties)(DalDeviceHandle * _h,  AdcDeviceDevicePropertiesType *pAdcDeviceProp);
  
   DALResult (*ReadChannel)(DalDeviceHandle * _h, uint32  uChannelIdx, AdcDeviceResultType *);
  
};

typedef struct DalAdcDeviceHandle DalAdcDeviceHandle;
struct DalAdcDeviceHandle
{
   uint32 dwDalHandleId;
   const DalAdcDevice *pVtbl;
   void *pClientCtxt;
};

/**
   @brief Attaches to the ADC physical device driver

   This function attaches the client to the physical ADC device driver.
   This function is used to obtain the device handle which is required to
   use the driver APIs.

   @param  pszDevName [in] The device ID string to attach to
   @param  hDalDevice [out] Pointer to DAL device handle pointer which will receive a pointer to the ADC device handle

   @return DAL_SUCCESS if the attach was successful. Other return values
           indicate that an error occurred.

*/
#define DAL_AdcDeviceDeviceAttach(pszDevName,hDalDevice)\
        DAL_StringDeviceAttachEx(NULL, pszDevName, DALADCDEVICE_INTERFACE_VERSION, hDalDevice)

/**
   @brief Gets the number of channels defined for this device

   @param  _h [in] Device handle obtained from DAL_AdcDeviceDeviceAttach
   @param  pAdcDeviceProp [out] Pointer to result data

   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS if successful.

*/
static __inline DALResult
DalAdcDevice_GetDeviceProperties(DalDeviceHandle * _h,  AdcDeviceDevicePropertiesType *pAdcDeviceProp)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->GetDeviceProperties( _h, pAdcDeviceProp);
}

/**
   Determine whether the channel is supported by the device.

   @param  _h [in] Device handle obtained from DAL_AdcDeviceDeviceAttach
   @param  pChannelName [in] NULL-terminated channel name
   @param  puChannelIdx [out] Channel index

   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS if successful.

*/
static __inline DALResult
DalAdcDevice_GetInputProperties(DalDeviceHandle * _h, const char *pChannelName, uint32 *puChannelIdx)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->GetInputProperties( _h, pChannelName, puChannelIdx);
}



/**
   @brief Reads an ADC channel

   This function is used to read an ADC channel.

   @param  _h [in] Device handle obtained from DAL_AdcDeviceDeviceAttach
   @param  uChannelIdx [in] The index of the channel to recalibrate
   @param  pAdcDeviceReadResult [out] Read result

   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS if successful.

*/
static __inline DALResult
DalAdcDevice_ReadChannel(DalDeviceHandle *_h, uint32 uChannelIdx, AdcDeviceResultType *pAdcDeviceReadResult)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->ReadChannel( _h, uChannelIdx, pAdcDeviceReadResult);
}







#endif

