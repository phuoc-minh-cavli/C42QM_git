#ifndef time_fn_map_H
#define time_fn_map_H

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

  time_fn_map_rtc_get_cmd_fp
  time_fn_map_rtc_set_cmd_fp

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  time_fn_map_initialize_fp() must be called after the PMIC initializes.


Copyright (c) 2009 - 2012, 2020 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/services/time/src/time_fn_map.h#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/14/11   ab      Featurizing the missing apis for 8660 target currently under
                   FEATURE_TIME_ALARM

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_types.h"
#include "ats_rtc.h"
#include "msg.h"
#include "err.h"
#include "fs_public.h"
#include "pmapp_rtc.h"
#include "pm_rtc.h"
#ifdef FEATURE_PMIC_RTC
#include "pmic.h"
#endif /*FEATURE_PMIC_RTC*/

/**
@mainpage Time Services Mapping Functions
Function Pointers used in time code*/ 


/**
@defgroup Time Services Mapping Functions
This section contains function pointers used in time code.
*/

/** 
@ingroup Time Function Pointers
@file time_fn_map.h
This file contains unction pointers used in time code.
*/

#ifndef FEATURE_BU_WITHOUT_PMIC

#include "pm.h"

#define ATS_RTC_SYNC_PMIC_RTC
#define TIME_TOD_SYNC_PMIC_RTC

#endif /*FEATURE_BU_WITHOUT_PMIC*/

#include "time_efs.h"

/*=============================================================================
                           Enumeration types
=============================================================================*/


/** Enum abstraction for PMIC RTC display mode */
typedef enum
{
  #ifdef FEATURE_PMIC_RTC
   ATS_PM_ERR_FLAG_SUCCESS = PM_ERR_FLAG__SUCCESS,
   ATS_PM_ERR_FLAG_FAILURE = 0x1
  #else /* FEATURE_PMIC_RTC */
   ATS_PM_ERR_FLAG_SUCCESS = 0x0,
   ATS_PM_ERR_FLAG_FAILURE  = 0x1
  #endif /* FEATURE_PMIC_RTC */
} time_fn_map_err_status;

/** Function pointer structure type */
typedef struct 
{

  #ifdef FEATURE_PMIC_RTC
    /** To send read command to PMIC RTC  */
    pm_err_flag_type (*time_fn_map_rtc_get_cmd_fp)(pm_rtc_time_type *);
    /** To send write command to PMIC RTC  */
    pm_err_flag_type (*time_fn_map_rtc_set_cmd_fp)(pm_rtc_time_type *);
  #endif /*FEATURE_PMIC_RTC*/
  
  int (*efs_get_fp) (const char *, void *, uint32 );
  int (*efs_put_fp) (const char *, void *, uint32, int, int );

  void (*time_server_call_ind_fp) (uint16, uint64);

}time_fn_map_fp_type;


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
);

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
);

/** 
@ingroup Time Function Pointers
         
@brief  
*This function reads the pmic time                                      \n

@param[in]                                                               
  pm_rtc_time_type * - pointer for getting RTC time          \n
 
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
);

/** 
@ingroup Time Function Pointers
         
@brief  
*This function sets the pmic time                                       \n\n 

@param[in]                                                               
  pm_rtc_time_type * - pointer consisting of time in msecs and secs to be set                 \n
 
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
);







#endif /*time_fn_map_H*/
