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
  time_fn_map_set_rtc_display_mode
  time_fn_map_get_rtc_display_mode
  time_fn_map_is_rtc_valid
  time_fn_map_poll_time_for_boundary

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  time_fn_map_initialize_fp() must be called after the PMIC initializes.


Copyright (c) 2009 - 2015, 2019 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/services/time/src/time_fn_map.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/15   abh     Changes for correctly handling ATS_PRIVATE bases.
02/14/11   ab      Featurizing the missing apis for 8660 target currently under
                   FEATURE_TIME_ALARM

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "dog.h"
#include "ats_v.h"
#include "time_fn_map.h"
#include <stringl/stringl.h>
#include "time_dal_v.h"
#include "pm_rtc.h"


#ifdef FEATURE_QMI_TIME_REMOTE
#include "qmi_time_server.h"
#endif

/** Static definition for function pointers */
static time_fn_map_fp_type time_fn_map_fps;

/** 
@ingroup Time Function Pointers
         
@brief  
*Initializes the Time Function Pointers                                 \n\n

@param[in]                                                               
 None                                                                   \n\n  
 
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
  if (time_defines.use_pmic_rtc)
    {
      time_fn_map_fps.time_fn_map_rtc_get_cmd_fp = pm_rtc_rw_get_ms_time;
      time_fn_map_fps.time_fn_map_rtc_set_cmd_fp= pm_rtc_alarm_set_ms_time;
    }
  #endif /*FEATURE_PMIC_RTC*/

  #endif /*feature FEATURE_BU_WITHOUT_PMIC*/

  time_fn_map_fps.efs_get_fp = NULL;
  time_fn_map_fps.efs_put_fp = NULL;

  #ifndef FEATURE_TIME_NO_EFS
  if (time_defines.use_pmic_rtc)
    {
	  time_fn_map_fps.efs_get_fp = efs_get;
	  time_fn_map_fps.efs_put_fp = efs_put;
	  time_fn_map_fps.efs_deltree_fp = efs_deltree;
	}
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
  time_julian_type * - pointer to julian time for getting time          \n
 
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
       (0, (pm_rtc_time_type*)(time_ptr)) 
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
  time_julian_type * - pointer to julian time to be set                 \n
 
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
       (0,(pm_rtc_time_type*)(time_ptr)) 
        == ATS_PM_ERR_FLAG_SUCCESS)
    {
      success = TRUE;
    }  
  }
  #endif

  return success;
}


