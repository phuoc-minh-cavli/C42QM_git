/*======================================================================
                        qapi_tsens.c

GENERAL DESCRIPTION
  Contains Kernel level QAPIs for TSENS

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2015, 2019, 2020 by Qualcomm Technologies, Inc.  All Rights Reserved.
 ======================================================================*/
/*======================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 2019-05-08   ay      CR:2440855 changes
 * 2017-11-06   KS      QAPI Custon callback changes.
 * 2017-04-07   KS      QAPI TSens Changes.
 * 2017-03-13   PC      Create.
 ======================================================================*/
// This should be on top. Do not move


#ifdef QAPI_TXM_SOURCE_CODE
#include "qapi_tsens.h"
#include "tx_api.h"
#include "qurt_txm_qapi_fwk.h"
#include "qapi_tsens.h"
#include "DALStdErr.h"
#include "DDITsens.h"
#include "DALDeviceId.h"
#include "DALTsens.h"
#include "qurt_memory.h"
#include "qurt_error.h"
#include "qurt_mutex.h"
#include "qurt.h"

/*======================================================================
                          EXTERNAL
 ======================================================================*/
 

/*======================================================================
                          MACROS
 ======================================================================*/
 
#define  TSENS_QAPI_MAGIC           0x012C
#define  TSENS_CLIENTS_PER_DEVICE      3

#define  TSENS_QAPI_ENCODE(index)   (index ^ TSENS_QAPI_MAGIC)

#define  TSENS_QAPI_DECODE(handle,index)   index = (uint32)handle ^ TSENS_QAPI_MAGIC
   
/*======================================================================
                          TYPEDEFS
 ======================================================================*/
 
 typedef struct tsens_qapi_opener
{
    qurt_mutex_t        mutex;
    void               *c_handle[TSENS_CLIENTS_PER_DEVICE];
} tsens_qapi_opener; 
 
/*======================================================================
                          GLOBALS
 ======================================================================*/
 

extern TsensDeviceDataType *pDeviceData;

static tsens_qapi_opener opener;
static boolean created_tsens = FALSE;

/*======================================================================
                          FUNCTIONS
=======================================================================*/

/* Function to invoke TSENS APIs based on command ID*/
int32_t qapi_TSENS_Handler(uint32_t id, uint32_t a, uint32_t b, uint32_t c,
                            uint32_t d, uint32_t e, uint32_t f, uint32_t g,
                            uint32_t h, uint32_t i, uint32_t j, uint32_t k,
                            uint32_t l)
{
  int32_t retval = QAPI_ERROR;
  void *call_back_mgr_1;
  uint32_t encoded_h,u_client;
  tsens_qapi_opener *opener_ptr = &opener;
  boolean found;
  int32_t num_sensors;
  qapi_TSENS_Result_t TempResult;
  uint8 handle_idx;
  uint32_t *handle_ptr;
  
  if (!created_tsens)
  {
     qurt_mutex_init (&opener_ptr->mutex);
     created_tsens = TRUE;
  }

  switch(id)
  {
    case TXM_QAPI_TSENS_OPEN:
	
      if (a == NULL) 
      {
		  return QAPI_ERR_INVALID_PARAM;
	  }  
	 
	  qurt_mutex_lock(&opener_ptr->mutex);
	  
	  found = FALSE;
	  
	  if(FALSE == qurt_check_if_module_address((qurt_addr_t)a, sizeof(int32_t)))
	  {    
         qurt_mutex_unlock(&opener_ptr->mutex);
         return QAPI_ERR_INVALID_PARAM;
	  }
      for (u_client = 0; u_client < TSENS_CLIENTS_PER_DEVICE; u_client++)
      {
          if (opener_ptr->c_handle[u_client] == NULL)
          {
              found = TRUE;
              break;
          }
      }
	  if (!found)
      {
          retval = QAPI_ERR_NOT_SUPPORTED;
          qurt_mutex_unlock (&opener_ptr->mutex);
          break;
      }
      retval = qapi_TSENS_Open((qapi_TSENS_Handle_t *)&(opener_ptr->c_handle[u_client]));
	  
	  if(retval == QAPI_OK)
	  {
		handle_ptr = (uint32_t *)a;
	    encoded_h = TSENS_QAPI_ENCODE(u_client);
       *handle_ptr = encoded_h ; 
	  }
	  qurt_mutex_unlock (&opener_ptr->mutex);
      break;

    case TXM_QAPI_TSENS_CLOSE:
	
 	  if(a == NULL)
	  {   
         return QAPI_ERR_INVALID_PARAM;
	  }
	  qurt_mutex_lock(&opener_ptr->mutex);
	  
	  TSENS_QAPI_DECODE(a,handle_idx);
	 
	  if((handle_idx >= TSENS_CLIENTS_PER_DEVICE) || (opener_ptr->c_handle[handle_idx] == NULL))
	  {
		  qurt_mutex_unlock(&opener_ptr->mutex);
		  return QAPI_ERR_INVALID_PARAM;
	  }
	  	  
      retval = qapi_TSENS_Close((qapi_TSENS_Handle_t)opener_ptr->c_handle[handle_idx]);
	  
	  if (retval == QAPI_OK)
	  {
		  opener_ptr->c_handle[handle_idx] = NULL;
	  }
	  qurt_mutex_unlock(&opener_ptr->mutex);
      break;

    case TXM_QAPI_TSENS_GET_NUM_SENSORS:
	
	  if((a == NULL) || (b == NULL))
      {
          return  QAPI_ERR_INVALID_PARAM;
      }
	  qurt_mutex_lock(&opener_ptr->mutex);
	  
	  TSENS_QAPI_DECODE(a,handle_idx);
	  
	  if((handle_idx >= TSENS_CLIENTS_PER_DEVICE) || (opener_ptr->c_handle[handle_idx] == NULL))
	  {
		  qurt_mutex_unlock(&opener_ptr->mutex);
		  return QAPI_ERR_INVALID_PARAM;
	  }
		  
      retval = qapi_TSENS_Get_Num_Sensors((qapi_TSENS_Handle_t)opener_ptr->c_handle[handle_idx], (INT*)&num_sensors);
	  if(retval == QAPI_OK)
	  {
	     if( QURT_EOK != qurt_copy_to_user((qurt_addr_t *)b, (qurt_size_t)sizeof(int32_t),
                 (qurt_addr_t *)&num_sensors, (qurt_size_t)sizeof(int32_t)))
         {
			 qurt_mutex_unlock(&opener_ptr->mutex);
             return QAPI_ERR_INVALID_PARAM;
         }
	  }  
	  qurt_mutex_unlock(&opener_ptr->mutex);	  
      break;

    case TXM_QAPI_TSENS_GET_TEMP:
	
	  if((a == NULL)||(c == NULL))
      {
		  return QAPI_ERR_INVALID_PARAM;
	  }  
	  
	  qurt_mutex_lock(&opener_ptr->mutex);
	  
	  TSENS_QAPI_DECODE(a,handle_idx);
	  
	  if((handle_idx >= TSENS_CLIENTS_PER_DEVICE) || (opener_ptr->c_handle[handle_idx] == NULL))
	  {
		  qurt_mutex_unlock(&opener_ptr->mutex);
		  return QAPI_ERR_INVALID_PARAM;
	  }   	  
      retval = qapi_TSENS_Get_Temp((qapi_TSENS_Handle_t)opener_ptr->c_handle[handle_idx], b, (qapi_TSENS_Result_t *)&TempResult);
	  if(retval == QAPI_OK)
	  {
	     if( QURT_EOK != qurt_copy_to_user((qurt_addr_t *)c, (qurt_size_t)sizeof(qapi_TSENS_Result_t),
                 (qurt_addr_t *)&TempResult, (qurt_size_t)sizeof(qapi_TSENS_Result_t)))
         {
			 qurt_mutex_unlock(&opener_ptr->mutex);
             return QAPI_ERR_INVALID_PARAM;
         }
	  }  
	  qurt_mutex_unlock(&opener_ptr->mutex);	  
      break;	  
	  
    case TXM_QAPI_TSENS_GET_CALIBRATION_STATUS:
	
      if(a == NULL)
	  {
		  return QAPI_ERR_INVALID_PARAM;
	  }  
	  qurt_mutex_lock(&opener_ptr->mutex);
	  
	  TSENS_QAPI_DECODE(a,handle_idx);
	  
	  if((handle_idx >= TSENS_CLIENTS_PER_DEVICE) || (opener_ptr->c_handle[handle_idx] == NULL))
	  {
		  qurt_mutex_unlock(&opener_ptr->mutex);
		  return QAPI_ERR_INVALID_PARAM;
	  }
	  
      retval = qapi_TSENS_Get_Calibration_Status((qapi_TSENS_Handle_t)opener_ptr->c_handle[handle_idx], b);
	  qurt_mutex_unlock(&opener_ptr->mutex);
      break;

    default:
      break;
  }
  return retval;
}

/* Function to convert TSENS internal error status to generic status*/
INT  qapi_TSENS_Convert_Status(int status)
{
   switch (status)
   {
      case DAL_SUCCESS:
         return QAPI_OK;

      case TSENS_ERROR_INVALID_PARAMETER:
         return QAPI_ERR_INVALID_PARAM;

      case DAL_ERROR_INSUFFICIENT_MEMORY:
         return QAPI_ERR_NO_MEMORY;

      case DAL_ERROR_NOT_SUPPORTED :
      case TSENS_ERROR_UNSUPPORTED :
         return QAPI_ERR_NOT_SUPPORTED;

      case DAL_ERROR_BUSY_RESOURCE:
         return QAPI_ERR_BUSY;

      case TSENS_ERROR_INVALID_CALIBRATION:
         return QAPI_ERR_INVALID_CALIB;

      case TSENS_ERROR_TIMEOUT:
         return QAPI_ERR_TIMEOUT;

      case TSENS_ERROR_DEAD_SENSOR:
         return QAPI_ERR_DEAD_SENSOR;

      default:
         return QAPI_ERROR;
   }
}

/* API for Opening TSENS */
qapi_Status_t  qapi_TSENS_Open(qapi_TSENS_Handle_t *Handle)
{
   DALResult status = DAL_ERROR;

   status = DAL_TsensDeviceAttach(DALDEVICEID_TSENS, (DalDeviceHandle **)Handle);

   return qapi_TSENS_Convert_Status(status);
}

/* API for closing TSENS */
qapi_Status_t  qapi_TSENS_Close(qapi_TSENS_Handle_t Handle)
{
   DALResult status = DAL_ERROR;

   if (Handle == NULL)
   {
      return QAPI_ERR_INVALID_PARAM;
   }
   
   status = DAL_TsensDeviceDetach((DalDeviceHandle *)Handle);

   return qapi_TSENS_Convert_Status(status);
}

/* API for reading temperature on a sensor */
qapi_Status_t qapi_TSENS_Get_Temp(qapi_TSENS_Handle_t Handle,
                                        uint32_t Sensor_Num,
                                        qapi_TSENS_Result_t *Temp_Ptr)
{
   DALResult status = DAL_ERROR;

   status = DalTsens_GetTemp((DalDeviceHandle *)Handle, Sensor_Num,
                             (TsensTempType *)Temp_Ptr);

   return qapi_TSENS_Convert_Status(status);
}

/* API to get number of sensors available on SOC */
qapi_Status_t qapi_TSENS_Get_Num_Sensors(qapi_TSENS_Handle_t Handle,
                                               int32_t *Num_Sensors)
{
   DALResult status = DAL_ERROR;

   status = DalTsens_GetNumSensors((DalDeviceHandle *)Handle, (uint32 *)Num_Sensors);

   return qapi_TSENS_Convert_Status(status);
}



/* API to get status of calibration on sensors */
qapi_Status_t qapi_TSENS_Get_Calibration_Status(qapi_TSENS_Handle_t Handle,
                                                      uint32_t Sensor_Num)
{
   DALResult status = DAL_ERROR;

   status = DalTsens_CheckCalibration((DalDeviceHandle *)Handle, Sensor_Num);

   return qapi_TSENS_Convert_Status(status);
}


#endif



