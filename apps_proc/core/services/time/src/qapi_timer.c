/*================================================================================== 
  QAPI wrapper layer for Heap functions.                     

  Copyright (c) 2017,2019-2020,2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
==================================================================================*/

/* $Header: //components/rel/core.tx/6.0/services/time/src/qapi_timer.c#3 $ */

/*==========================================================================
when       who     what, where, why
--------   ---    ---------------------------------------------------------- 
02/29/16   ps     Created
===========================================================================*/
#include "qapi_timer.h"
#include "timer.h"
#include "malloc.h"
#include "qapi_types.h"
#include "timer_v.h"
#include "txm_module.h"
#include "qurt_txm_uid.h"
#include "qapi_timer_internal.h"

uint32   timer_root_handle = 0xABCDABCD;

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/
qapi_Status_t _qapi_Timer_Def(
	qapi_TIMER_handle_t * timer_handle,
	qapi_TIMER_define_attr_t * timer_attr,
	uint32 timer_qapi_proc_hdle,
	timer_qapi_process timer_qapi_proc_id);

qapi_Status_t _qapi_Timer_Undef(
	qapi_TIMER_handle_t timer_handle);

qapi_Status_t _qapi_time_get(
	qapi_time_unit_type  time_get_unit,
	qapi_time_get_t*     time,
	timer_qapi_process   time_qapi_proc_id);

qapi_Status_t _qapi_get_timer_inaccuracy(
	qapi_time_genoff_args_ptr    pargs,
	timer_qapi_process   time_qapi_proc_id
);

/*==================================================================================================
                                        EXTERNS
==================================================================================================*/

extern qapi_timer_table_type qapi_timer_table[QAPI_MAX_TIMER_TABLE_SIZE];
extern int _qapi_get_timer_index(uint32 timer_handle);
extern timer_ptr_type _qapi_timer_validate(uint32 temp_id);

/**
* @return
* - #QAPI_OK – Success 
* - #QAPI_ERR_INVALID_PARAM - Failure for invalid parameter
* - #QAPI_ERR_NO_MEMORY - Failure for not able to allocate memory for timer handle
* - #QAPI_ERROR - Failure for anything other than the above two
  @brief  
* Allocates internal memory in the timer module. The internal 
* memory is then formatted with parameters provided in 
* timer_def_attr variable. The timer_handle is returned to the 
* client and this handle should be used for any subsequent timer 
* operations. 
  
* @param[in] timer_handle  Handle to the timer. 
* @param[in] timer_attr    Attributes for defining timer. 
  
  @dependencies
* None. 
*  
* @sideeffects Calling this API will cause memory allocation. 
*              Therefore, whenever the timer usage is done and
*              not required, qapi_Timer_Undef shoule be called
*              to release the memory otherwise it will cause a
*              memory leak.
*/
qapi_Status_t qapi_Timer_Def(
   qapi_TIMER_handle_t*    timer_handle,
   qapi_TIMER_define_attr_t*  timer_attr  
)
{
	return (_qapi_Timer_Def(timer_handle,timer_attr,timer_root_handle,TIMERS_QAPI_FROM_ROOT));
}


/**
* @return
* - #QAPI_OK – Success 
* - #QAPI_ERR_INVALID_PARAM - Failure for invalid parameter
* - #QAPI_ERROR - Failure for any other error
  @brief  
* Starts the timer with the duration specified in timer_attr. 
* If the timer is specified as a reload timer in timer_attr, 
* then the timer will restart after its expiry 
  
* @param[in] timer_handle  Handle to the timer. 
* @param[in] timer_attr    Attributes for setting timer. 
  
  @dependencies
* qapi_Timer_Def API should be called for the timer before 
* calling qapi_Timer_Set function.
*/
qapi_Status_t qapi_Timer_Set(
   qapi_TIMER_handle_t    timer_handle,
   qapi_TIMER_set_attr_t* timer_attr     
)
{
  qapi_Status_t status = QAPI_ERROR;
  timer_ptr_type timer_ptr = NULL;
  uint32 temp_id = 0;

  
  if(NULL == timer_handle || NULL == timer_attr || timer_attr->unit >= QAPI_TIMER_UNIT_MAX) 
  {
    return QAPI_ERR_INVALID_PARAM;
  }

  temp_id = (uint32)timer_handle; // Typecast to uint32 to get the unique timer id
  timer_ptr = _qapi_timer_validate(temp_id);
  
  if(!timer_ptr)
  {
    return QAPI_ERROR;
  }	
  
  if(TE_SUCCESS == timer_set_64((timer_ptr_type)timer_ptr,
  								(time_timetick_type)timer_attr->time,
  								(time_timetick_type)timer_attr->reload,
  								(timer_unit_type)timer_attr->unit) )
  {
    status = QAPI_OK;
  }

  return status;

}

/**
* @return
* - #QAPI_OK – Success
* - #QAPI_ERR_INVALID_PARAM - Failure for invalid parameter
* - # QAPI_ERR_NOT_SUPPORTED - Failure for the case of unknown timer info type 
* - # QAPI_ERR_NO_RESOURCE - Failure if the timer handle does not does not point to an active timer resource 
* - #QAPI_ERROR - Failure for any other error
  @brief  
* Gets the specified information about the timer.
  
* @param[in] timer_handle  Handle to the timer. 
* @param[out] timer_info    Specify the type of information 
*       needed from the timer.
  
  @dependencies
* 
*/
qapi_Status_t qapi_Timer_Get_Timer_Info(
   qapi_TIMER_handle_t         timer_handle,
   qapi_TIMER_get_info_attr_t* timer_get_info_attr,
   uint64*                     data
)
{
  qapi_Status_t status = QAPI_OK;
  timer_ptr_type timer_ptr = NULL;
  uint32 temp_id = 0;

  if(NULL == timer_handle || NULL == data || NULL == timer_get_info_attr) 
  {
    return QAPI_ERR_INVALID_PARAM; 
  }

  temp_id = (uint32)timer_handle; // Typecast to uint32 to get the unique timer id
  timer_ptr = _qapi_timer_validate(temp_id);
  
  if(!timer_ptr)
  {
    return QAPI_ERROR;
  }	

  /* Check if the timer is active */
  if (FALSE == timer_is_active((timer_ptr_type)timer_ptr))
  	{
  		/* Update the data with 0 */
		*data = 0x0;

  		/* Return that the handle does not point to an active timer resource */
  		return QAPI_ERR_NO_RESOURCE;
  	}

  /* */
  switch (timer_get_info_attr->timer_info)
  	{
  		case QAPI_TIMER_TIMER_INFO_ABS_EXPIRY:
			*data = (uint64)timer_expires_at_64((timer_ptr_type)timer_ptr);
			break;
		case QAPI_TIMER_TIMER_INFO_TIMER_DURATION:
			*data = timer_get_duration((timer_ptr_type)timer_ptr,(timer_unit_type)timer_get_info_attr->unit);
			break;
		case QAPI_TIMER_TIMER_INFO_TIMER_REMAINING:
			*data = timer_get_64((timer_ptr_type)timer_ptr,(timer_unit_type)timer_get_info_attr->unit);
			break;
		case QAPI_TIMER_TIMER_INFO_MAX:
		default:
			*data = 0x0;
			status = QAPI_ERR_NOT_SUPPORTED;
  	}

  return status;
}

/**
* @return
* - #QAPI_OK – Success 
* - #QAPI_ERR_INVALID_PARAM - Failure for invalid parameter
* - #QAPI_ERROR - Failure for any other error
  @brief  
* Timed wait. Blocks a thread for specified time. 
  
* @param[in] timeout - Specify the duration to block the thread 
* @param[in] unit    - Specify the unit of duration 
* @param[in] non-deferrable  - true = processor (if in deep 
*       sleep or power collapse) will be woken up on timeout.
*       false = processor will not be woken up from deep sleep
*       or power collapse on timeout. Whenever the processor
*       wakes up due to some other reason after timeout, the
*       thread will be unblocked.
  
  @dependencies
* 
*/
qapi_Status_t qapi_Timer_Sleep(
  uint64_t             timeout,
  qapi_TIMER_unit_type unit,
  qbool_t              non_deferrable
)
{
  qapi_Status_t status = QAPI_ERROR;

  if(unit >= QAPI_TIMER_UNIT_MAX ) 
  {
    return QAPI_ERR_INVALID_PARAM;  
  }


  (void)timer_sleep((time_timetick_type)timeout,
	  			(timer_unit_type)unit,
	  			(boolean)non_deferrable);

  status = QAPI_OK;

  return status;
}

/**
* @return
* - #QAPI_OK – Success 
* - #QAPI_ERR_INVALID_STATE – Success. This is returned when the timer was already in suspended state and then it is undefined 
* - #QAPI_ERR_INVALID_PARAM - Failure for invalid parameter
* - #QAPI_ERROR - Failure for any other error
  @brief  
* Undefines the timer. This API needs to be called whenever 
* timer usage is done. Calling this API will release the 
* internal timer  memory that was allocated when the timer was 
* defined, make the corresponding entry in the qapi timer table
* as NULL and add a node in the list of free indices
  
* @param[in] handle - timer handle to undefine the timer
  
  @dependencies
* 
*/
qapi_Status_t qapi_Timer_Undef(
    qapi_TIMER_handle_t    timer_handle
)
{
  return _qapi_Timer_Undef(timer_handle);
}

/**
* @return
* - #QAPI_OK – Success
* - #QAPI_ERR_INVALID_PARAM - Failure for invalid parameter
* - #QAPI_ERROR - Failure for any other error
  @brief  
* Stops the timer. Note: This function does NOT deallocate the 
* memory that was allocated when timer was defined.  
  
* @param[in] handle - timer handle to stop the timer
  
  @dependencies
* 
*/
qapi_Status_t qapi_Timer_Stop(
    qapi_TIMER_handle_t    timer_handle
)
{
  qapi_Status_t status = QAPI_ERROR;
  timer_ptr_type timer_ptr = NULL;
  uint32 temp_id = 0;

  if(NULL ==  timer_handle) 
  {
    return QAPI_ERR_INVALID_PARAM;  
  }

  temp_id = (uint32)timer_handle; // Type casting to get the unique timer id

  timer_ptr = _qapi_timer_validate(temp_id);
  
  if(!timer_ptr)
  {
    return QAPI_ERROR;
  }	

  if(TE_SUCCESS == timer_stop((timer_ptr_type)timer_ptr,T_NONE,NULL))
  {
    status = QAPI_OK;
  }

  return status;
}

/**
* @return
* - #QAPI_OK – Success
* - #QAPI_ERR_INVALID_PARAM - Failure for invalid parameter
* - #QAPI_ERROR - Failure for any other error
  @brief  
* Sets the timer with expiry specified in absolute ticks. 
  
* @param[in] handle - timer handle 
* @param[in] abs_time - timetick when the timer expires 
  
  @dependencies
* 
*/
qapi_Status_t qapi_Timer_set_absolute(
  qapi_TIMER_handle_t      timer_handle,
  uint64_t                 abs_time
)
{
  qapi_Status_t status = QAPI_ERROR;
  timer_ptr_type timer_ptr = NULL;
  uint32 temp_id = NULL;

  if(NULL ==  timer_handle || 0 == abs_time) 
  {
    return QAPI_ERR_INVALID_PARAM;  
  }

  temp_id = (uint32)timer_handle;
  timer_ptr = _qapi_timer_validate(temp_id);
  
  if(!timer_ptr)
  {
    return QAPI_ERROR;
  }	

  
  if(TE_SUCCESS == timer_set_absolute((timer_ptr_type)timer_ptr,(time_timetick_type)abs_time))
  {
    status = QAPI_OK;
  }

  return status;
}

/**
* @return
* - #QAPI_OK – Success
* - #QAPI_ERR_INVALID_PARAM - Failure for invalid parameter 
* - #QAPI_ERROR - Failure for anything other than the above two
* @brief  
* Gets the time in the specified format. 
*
* @param[in] time_get_unit  Unit in which to get the time. 
* @param[in] time           Pointer to qapi_time_get_t variable. 

*
* @dependencies
* none
*/
qapi_Status_t qapi_time_get(
   qapi_time_unit_type    time_get_unit,
   qapi_time_get_t* time
)
{
	return _qapi_time_get(time_get_unit,time,TIMERS_QAPI_FROM_ROOT);
}

/**
 * *?@return
* - #QAPI_OK ? Success
* - #QAPI_ERR_INVALID_PARAM - Failure for invalid parameter?
* - #QAPI_ERROR - Failure for anything other than the above two
* @brief 
*   
* Gets the timer inaccuracy in milliseconds. 
*
* @param[in] pargs  Pointer to the arguments.            

  @return
  QAPI_OK on success, an error code on failure.
*/
qapi_Status_t qapi_get_timer_inaccuracy(
	qapi_time_genoff_args_ptr    pargs
)
{
  return _qapi_get_timer_inaccuracy(pargs, TIMERS_QAPI_FROM_ROOT);
}


