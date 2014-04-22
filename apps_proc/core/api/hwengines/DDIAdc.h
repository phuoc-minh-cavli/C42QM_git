#ifndef __DDIADC_H__
#define __DDIADC_H__

/*============================================================================
  @file DDIAdc.h

  ADC Device Driver Interface header

  Clients may include this header and use these interfaces to read analog
  inputs.

               Copyright (c) 2008-2015, 2018 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/core.tx/6.0/api/hwengines/DDIAdc.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DalDevice.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "AdcInputs.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define DALADC_INTERFACE_VERSION DALINTERFACE_VERSION(4,1)

enum
{
   ADC_ERROR_INVALID_DEVICE_IDX = 1,
   ADC_ERROR_INVALID_CHANNEL_IDX,
   ADC_ERROR_NULL_POINTER,
   ADC_ERROR_DEVICE_QUEUE_FULL,
   ADC_ERROR_INVALID_PROPERTY_LENGTH,
   ADC_ERROR_OUT_OF_MEMORY,
   ADC_ERROR_API_UNSUPPORTED_IN_THIS_CONTEXT,
   ADC_ERROR_DEVICE_NOT_READY,
   ADC_ERROR_INVALID_PARAMETER,
   ADC_ERROR_OUT_OF_TM_CLIENTS,
   ADC_ERROR_TM_NOT_SUPPORTED,
   ADC_ERROR_TM_THRESHOLD_OUT_OF_RANGE,
   ADC_ERROR_TM_BUSY,
   _ADC_MAX_ERROR = 0x7FFFFFFF
};

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum
{
   ADC_RESULT_INVALID,
   ADC_RESULT_VALID,
   ADC_RESULT_TIMEOUT,
   ADC_RESULT_FIFO_NOT_EMPTY,
   _ADC_MAX_RESULT_STATUS = 0x7FFFFFFF
} AdcResultStatusType;

typedef struct
{
   enum
   {
      ADC_REQUEST_STATUS_PENDING,   /* the request is being immediately performed */
      ADC_REQUEST_STATUS_QUEUED,    /* the request was queued */
      ADC_REQUEST_STATUS_ERROR,     /* the request was not started due to an error */
      ADC_REQUEST_STATUS_UNKNOWN,   /* the request status is unknown */
      _ADC_MAX_REQUEST_STATUS = 0x7FFFFFFF
   } eStatus;
} AdcRequestStatusType;

typedef struct
{
   uint32 nDeviceIdx;
   uint32 nChannelIdx;
} AdcInputPropertiesType;

typedef struct
{
   uint32 nDeviceIdx;
   uint32 nChannelIdx;
   DALSYSEventHandle hEvent;
} AdcRequestParametersType;

typedef struct
{
   AdcResultStatusType eStatus;  /* status of the conversion */
   uint32 nToken;       /* token which identifies this conversion */
   uint32 nDeviceIdx;   /* the device index for this conversion */
   uint32 nChannelIdx;  /* the channel index for this conversion */
   int32 nPhysical;     /* result in physical units. Units depend on BSP */
   uint32 nPercent;     /* result as percentage of reference voltage used
                         * for conversion. 0 = 0%, 65535 = 100% */
   uint32 nMicrovolts;  /* result in microvolts */
   uint32 nCode;        /* raw ADC code from hardware */
} AdcResultType;













typedef struct DalAdc DalAdc;
struct DalAdc
{
   struct DalDevice DalDevice;
   DALResult (*GetAdcInputProperties)(DalDeviceHandle *_h, const char *pInputName, uint32 nInputNameLen, AdcInputPropertiesType *pAdcInputProps);
   DALResult (*Read)(DalDeviceHandle *_h, const AdcInputPropertiesType *pAdcInputProps, AdcResultType *pAdcResult);
};

typedef struct DalAdcHandle DalAdcHandle;
struct DalAdcHandle
{
   uint32 dwDalHandleId;
   const DalAdc *pVtbl;
   void *pClientCtxt;
};

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/**
   @brief Attaches to the ADC driver

   This function attaches the client to the ADC driver. This function is used
   to obtain the device handle which is required to use the ADC driver APIs.

   @param  DevId [in] The device ID to attach to (e.g., DALDEVICEID_ADC)
   @param  hDalDevice [out] Pointer to DAL device handle pointer which will receive
                             a pointer to the ADC device handle

   @return DAL_SUCCESS if the attach was successful. Other return values
           indicate that an error occurred.

*/
#define DAL_AdcDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALADC_INTERFACE_VERSION,hDalDevice)

/**
   @brief Gets the properties for an analog input from the ADC DAL properties

   This function looks up the properties for an analog input.

   @param  _h [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceAttach
   @param  pInputName [in] The name of the analog input to look up
   @param  nInputNameSize [in] The size of the analog input name in bytes
   @param  pAdcInputProps [out] The buffer to write the property values to

   @return DAL_SUCCESS if the input was found. DAL_SUCCESS if the input was
      found in the properties file.

      ADC_ERROR_INVALID_PROPERTY_LENGTH is returned if the
      property was incorrectly defined.

      All other values correspond to DAL error codes, and should
      generally be interpreted to mean that the analog input is
      not present on the target hardware.

*/
static __inline DALResult
DalAdc_GetAdcInputProperties(DalDeviceHandle *_h,  const char *pInputName,  uint32 nInputNameSize,  AdcInputPropertiesType *pAdcInputProps)
{
   return ((DalAdcHandle *)_h)->pVtbl->GetAdcInputProperties(_h, pInputName, nInputNameSize, pAdcInputProps);
}

/**
   @brief Reads an analog input

   This function performs a blocking ADC read for the device and channel specified by the
   client in pAdcInputProps.

   @param  _h [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceAttach
   @param  pAdcInputProps [in] Pointer to input properties for the channel
   @param  pAdcResult [out] ADC conversion result

   @see    DAL_AdcDeviceAttach

   @return DAL_SUCCESS if the request was successfully queued.

           Other return values indicate an error and include:
           ADC_ERROR_INVALID_DEVICE_IDX - the client specified an invalid device index
           ADC_ERROR_INVALID_CHANNEL_IDX - the client specified an invalid channel index
           ADC_ERROR_NULL_POINTER - pAdcInputProps or pAdcResult was NULL

*/
static __inline DALResult
DalAdc_Read(DalDeviceHandle *_h,  const AdcInputPropertiesType *pAdcInputProps,  AdcResultType *pAdcResult)
{
   return ((DalAdcHandle *)_h)->pVtbl->Read(_h, pAdcInputProps, pAdcResult);
}

/**
   @brief Reads an analog input

   This function requests the ADC driver to start an ADC conversion on
   the device and channel specified by the client in pAdcParams.

   Conversion requests are processed on a FIFO basis.

   If the hEvent parameter in pAdcParams is a DAL callback event triggered by the
   ADC driver when the conversion is complete. The payload of the callback is a pointer
   to an AdcResultType struct which contains the result of the ADC conversion.

   If hEvent in pAdcParams is NULL, this function will return ADC_ERROR_NULL_POINTER.

   @param  _h [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceAttach
   @param  pAdcParams [in] Pointer to client's request parameters
   @param  pAdcRequestStatus [out] Pointer to buffer which will indicate the status of request
                        (Client can pass NULL for pAdcRequestStatus if status is unwanted)

   @see    DAL_AdcDeviceAttach

   @return DAL_SUCCESS if the request was successfully queued.

           Other return values indicate an error and include:
           ADC_ERROR_INVALID_DEVICE_IDX - the client specified an invalid device index
           ADC_ERROR_INVALID_CHANNEL_IDX - the client specified an invalid channel index
           ADC_ERROR_NULL_POINTER - pAdcParams, pAdcParams->hEvent, or pAdcResult was NULL
           ADC_ERROR_DEVICE_QUEUE_FULL - the device request queue was full

*/
//static __inline DALResult
//DalAdc_RequestConversion(DalDeviceHandle *_h,  const AdcRequestParametersType *pAdcParams,  AdcRequestStatusType *pAdcRequestStatus)
//{
//   return ((DalAdcHandle *)_h)->pVtbl->RequestConversion(_h, pAdcParams, pAdcRequestStatus);
//}

/**
   @brief Calibrates an analog input

   This function requests the ADC driver to recalibrate the specified
   channel. If other channels share the physical device configuration
   (as defined in the ADC device BSP), then those channels may also be
   updated by the recalibration.

   The hEvent parameter in pAdcParams is a DAL callback event triggered by the
   ADC driver when the recalibration is complete. The payload of the callback is a pointer
   to an AdcRecalibrationResultType struct which contains the result of the ADC recalibration.

   @param  _h [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceAttach
   @param  pAdcParams [in] Pointer to client's request parameters

   @see    DAL_AdcDeviceAttach

   @return DAL_SUCCESS if the request was successfully queued.

           Other return values indicate an error and include:
           ADC_ERROR_INVALID_DEVICE_IDX - the client specified an invalid device index
           ADC_ERROR_INVALID_CHANNEL_IDX - the client specified an invalid channel index
           ADC_ERROR_NULL_POINTER - pAdcParams was NULL
           ADC_ERROR_DEVICE_QUEUE_FULL - the device request queue was full

*/
//static __inline DALResult
//DalAdc_RequestRecalibration(DalDeviceHandle *_h,  const AdcRequestParametersType *pAdcParams)
//{
//   return ((DalAdcHandle *)_h)->pVtbl->RequestRecalibration(_h, pAdcParams);
//}












#endif

