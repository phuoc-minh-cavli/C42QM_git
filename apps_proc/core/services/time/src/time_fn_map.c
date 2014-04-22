/*=============================================================================

       T I M E   S E R V I C E   TIME FUNCTION MAP FILE

GENERAL DESCRIPTION
  Initializes and maintains function pointers used in time code. These 
  function pointers will be populated based on a specific device. For example, 
  depending on which PMIC driver is used, the function pointers will be 
  populated based on the pmic model used.

REGIONAL FUNCTIONS
  time_fn_map_initialize_fp
    Initialize the function pointers

  time_fn_map_get_pmic_time
  time_fn_map_set_pmic_time

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  time_fn_map_initialize_fp() must be called after the PMIC initializes.


Copyright (c) 2009 - 2014 , 2020 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/services/time/src/time_fn_map.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/03/14   psu     Update diag macros using optimized versions.
02/14/11   ab      Featurizing the missing apis for 8660 target currently under
                   FEATURE_TIME_ALARM

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "dog.h"
#include "time_fn_map.h"
#include <stringl/stringl.h>
#include "busywait.h"
#include "pm_rtc.h"
#include "pmapp_rtc.h"


#ifdef FEATURE_QMI_TIME_REMOTE
#include "qmi_time_server.h"
#endif

/** Static definition for function pointers */
#ifdef FEATURE_PMIC_RTC
static time_fn_map_fp_type time_fn_map_fps;
#endif /*FEATURE_PMIC_RTC*/

#ifdef FEATURE_PMIC_RTC
static pm_model_type pm_model_in_use = PMIC_IS_INVALID;
#else
#define ATS_RTC_PMIC_IS_INVALID 0xFFFFFFFF
#endif

/** 
@ingroup Time Function Pointers
         
@brief  
*Initializes the Time Function Pointers                                 \n\n

@param[in]                                                               
 time_fn_map_fp_type - Pointer to structure containing function pointer \n
                       to be initialized                                \n\n
 
@dependencies  
 None                                                                   \n\n  
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None                                                                   \n\n

*/
void time_fn_map_initialize_fp
(
  void
)
{
  #ifdef FEATURE_PMIC_RTC
  time_fn_map_fps.time_fn_map_rtc_get_cmd_fp= NULL;
  time_fn_map_fps.time_fn_map_rtc_set_cmd_fp= NULL;
  #endif


  /*Only if PMIC is available, intialize all the fps*/
  #ifndef FEATURE_BU_WITHOUT_PMIC

  #ifdef FEATURE_PMIC_RTC
  time_fn_map_fps.time_fn_map_rtc_get_cmd_fp = pmapp_rtc_get_time;
  time_fn_map_fps.time_fn_map_rtc_set_cmd_fp = pmapp_rtc_alarm_set_time;
  #endif /*FEATURE_PMIC_RTC*/

  #endif /*feature FEATURE_BU_WITHOUT_PMIC*/

  time_fn_map_fps.efs_get_fp = NULL;
  time_fn_map_fps.efs_put_fp = NULL;

  // TODO: Enable later
  #ifndef FEATURE_TIME_NO_EFS
  time_fn_map_fps.efs_get_fp = efs_get;
  time_fn_map_fps.efs_put_fp = efs_put;
  #endif /*FEATURE_TIME_NO_EFS*/

  #ifdef FEATURE_QMI_TIME_REMOTE
  time_fn_map_fps.time_server_call_ind_fp = time_server_call_ind;
  #endif /*FEATURE_QMI*/
}

/** 
@ingroup Time Function Pointers
         
@brief  
*Returns the time function pointer address               \n\n

@param[in]                                                               
 None                                                                   \n\n  
 
@dependencies  
 None                                                                   \n\n  
 
@param[out]
 time_fn_map_fp_type - Pointer to structure containing function pointer \n
                                                                          \n\n
@sideffects 
 None                                                                   \n\n

*/
time_fn_map_fp_type * time_fn_map_get_fp
(
  void
)
{
  /* Return the address of the static structure */
  return (&time_fn_map_fps);
}


/** 
@ingroup Time Function Pointers
         
@brief  
*This function reads the pmic time                                      \n

@param[in]                                                               
  pm_rtc_time_type * - pointer for getting time in msecs and secs          \n
 
@dependencies  
 None                                                                   \n\n  
 
@param[out]
 boolean - Whether the pmic time was got or not                         \n\n
 
@sideffects 
 None                                                                   \n\n

*/
boolean time_fn_map_get_pmic_time
(
  pm_rtc_time_type *time_ptr
)
{
  boolean success = FALSE;
  #ifdef FEATURE_PMIC_RTC
  if(time_fn_map_fps.time_fn_map_rtc_get_cmd_fp)
  {
    if(time_fn_map_fps.time_fn_map_rtc_get_cmd_fp
       ((pm_rtc_time_type*)(time_ptr)) 
        == ATS_PM_ERR_FLAG_SUCCESS)
    {
      success = TRUE;
    }  
  }
  #endif

  return success;
}

/** 
@ingroup Time Function Pointers
         
@brief  
*This function sets the pmic time                                       \n\n 

@param[in]                                                               
  pm_rtc_time_type * - pointer to time in msecs and secs to be set                 \n
 
@dependencies  
 None                                                                   \n\n  
 
@param[out]
 boolean - Whether the pmic time is set or not                          \n\n
 
@sideffects 
 None                                                                   \n\n

*/
boolean time_fn_map_set_pmic_time
(
  pm_rtc_time_type *time_ptr
)
{
  boolean success = FALSE;
  #ifdef FEATURE_PMIC_RTC
  if(time_fn_map_fps.time_fn_map_rtc_set_cmd_fp)
  {
    if(time_fn_map_fps.time_fn_map_rtc_set_cmd_fp
       ((pm_rtc_time_type*)(time_ptr)) 
        == ATS_PM_ERR_FLAG_SUCCESS)
    {
      success = TRUE;
    }  
  }
  #endif

  return success;
}









