/*=============================================================================

       T I M E   S E R V I C E   A T S   R T C   S U B S Y S T E M

GENERAL DESCRIPTION
  Initializes and maintains RTC time in the  Generic Framework.

REGIONAL FUNCTIONS
  ats_rtc_init
    Initialize the ats_rtc subsystem

  ats_rtc_remote_update
    Update Time_rtcs from the current proc to another proc

  ats_rtc_genoff_cb
    Callback called to remotely update offset

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ats_rtc_init() must be called.


Copyright (c) 2009 - 2013, 2019, 2021 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/services/time/src/ats_rtc.c#5 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/26/11   din     Fixed compiler warnings.
12/09/10   din     Fixed kockwork errors.
07/15/10   din     Featured Out Init call for base ATS_RTC.
04/27/10   din     Fixed compiler warnings.
12/8/09    din     Removed compilation warnings.
10/07/09   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "time_genoff_v.h"
#include "ats_v.h"
#include "ats_rtc.h"
#include "time_jul.h"
#include "time_remote.h"
#include "dog.h"
#include "DALSys.h"
#include "time_fn_map.h"
#include "timetick.h"
#include "time_svc.h"
#include "timetick_sclk64.h"
#include "pm_rtc.h"


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/
/* Frequency of Sclk used */
#define TIME_CONV_SCLK_FREQ            19200000
/* Number of ticks per ms */
#define TIME_CONV_SCLK_FREQ_MS         (TIME_CONV_SCLK_FREQ/1000)

/*-----------------------------------------------------------------------------
  Time RTC static information
-----------------------------------------------------------------------------*/

/* Enum-type of different bases */
static time_bases_type        time_base;

/* Pointer to ats_rtc in consideration */
static time_genoff_ptr        ptime_genoff;

static boolean                read_pmic_rtc;

static ats_rtc_status_type   rtc_status;

pm_rtc_time_type             time_at_bootup   = {0};
/*--------------------------------------------------------------------------
  Linting...
--------------------------------------------------------------------------*/

/*lint -esym(715,unused_*)  parameters prefixed with unused_ are not used */



/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/



/*=============================================================================

FUNCTION ATS_RTC_SET_FROM_PMIC_RTC

DESCRIPTION
  Initialize the time-rtc value from the PMIC's Real Time Clock,
  if available.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void ats_rtc_set_from_pmic_rtc( void )
{

  /* Milliseconds since January 6, 1980 00:00:00 */
  int64                           msecs;
  
  /* Flag indicating that the PMIC RTC value is valid */
  boolean                         valid;

  /* Time from the PMIC's Real Time Clock */
  pm_rtc_time_type                time = {0};

  
  uint64                           timetick = 0;

   #if (defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_TIME_NO_RPC)) \
                                        && defined(IMAGE_APPS_PROC)
   return;
   #else /* FEATURE_MULTIPROCESSOR && IMAGE_APPS_PROC */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If ats_rtc has already been read from the PMIC's RTC ... */
  if ( read_pmic_rtc )
  {
    /* ... don't bother re-reading it - not much point. */
    return;
  }


  /* If ats_rtc has already been set from CDMA or HDR system time ... */
  //if ( ats_rtc.initialized )
  //{
    /* ... we've got better than the PMIC RTC's 1 second resolution. */
    //return;
  //}



  /* Retrieve the real time clock's time from the PMIC */
  valid = time_fn_map_get_pmic_time(&time);
  time_at_bootup = time;
  /* If the PMIC Real Time Clock's time is valid, convert it to the
     timestamp format for the time-of-day base timestamp.  */

  if ( valid )
  {
    rtc_status = TS_RTC_VALID;
    /* Convert RTC value to milliseconds from Jan 6, 1980 00:00:00. 
       Using the msec resoltion of pmic rtc as well for more accuracy */
    msecs = ((int64)time.sec * 1000) + time.msec;
    
    timetick = timetick_sclk64_get();
    
    timetick = timetick/TIME_CONV_SCLK_FREQ_MS; 

    /* Moving rtc time to bootup of target so that for time calculations 
       timetick can be used straightly as uptime instead of subtracting 
       base_timetick always */    
    msecs -= timetick;  

    /* Set Generic Offset */
    time_genoff_set_generic_offset( ptime_genoff, msecs );
  }
  else
  {
    MSG_1(MSG_SSID_ATS,MSG_LEGACY_LOW,"PMIC API did not return success, valid = %d",valid); 
    rtc_status = TS_RTC_INVALID;
  }

#endif /* FEATURE_MULTIPROCESSOR && IMAGE_MODEM_PROC */

} /* ats_rtc_set_from_pmic_rtc */


/*=============================================================================

FUNCTION ATS_PRINT_RTC_STATUS

DESCRIPTION
Prints the RTC bootup time & RTC time now

DEPENDENCIES

RETURN VALUE
Returns the value in rtc_statuc global var

SIDE EFFECTS

=============================================================================*/

ats_rtc_status_type ats_print_rtc_status( void )
{
  /* Time from the PMIC's Real Time Clock */
  pm_rtc_time_type                  time = {0};
  boolean                         valid = FALSE;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ATS_MSG_2("TIME_DEBUG: Bootup RTC Time = %d:%d ", 
             time_at_bootup.sec, 
			 time_at_bootup.msec);


  /* Retrieve the real time clock's time/date from the PMIC*/
  valid = time_fn_map_get_pmic_time(&time);
  ATS_MSG_HIGH("TIME_DEBUG: rtc_status=%d, timetick_now=%d, current_valid=%d ",
            rtc_status,
            timetick_get(),
            valid);
  
  ATS_MSG_2("TIME_DEBUG: RTC Time Now = = %d:%d ", 
             time_at_bootup.sec, 
			 time_at_bootup.msec);
  
  return rtc_status;

} /* ats_print_rtc_status */


/*=============================================================================

FUNCTION ATS_RTC_REMOTE_UPDATE                                        REGIONAL

DESCRIPTION
  Update TOD from the current proc to another proc

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates remote generic offset if set to True

=============================================================================*/

void ats_rtc_remote_update(void)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update ats_rtc from the current proc to another proc */
  time_genoff_set_remote_genoff( ptime_genoff );

} /* ats_rtc_remote_update */



/*=============================================================================

FUNCTION ATS_RTC_INIT                                                 REGIONAL

DESCRIPTION
  Initializes the ats_rtc subsystem

DEPENDENCIES
  Must be called when the phone first powers on, from time_init( ).

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void ats_rtc_init( void )
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Add initialization function */
  rtc_status = TS_RTC_INVALID;
  read_pmic_rtc = FALSE;

  /* Get the pointer to ats_rtc instance */
  ptime_genoff = time_genoff_get_pointer( time_base );

  /* Initialize ats_rtc */
  time_genoff_pre_init( ptime_genoff, ATS_RTC );

  /* Add base subsys to ats_rtc */
  time_genoff_add_init_call( ptime_genoff, &ats_rtc_set_from_pmic_rtc );

  /* Initialize ats_rtc */
  time_genoff_post_init( ptime_genoff );

} /* ats_rtc_init */


#if defined(ATS_RTC_SYNC_PMIC_RTC) && \
    !defined(FEATURE_ATS_RTC_AS_RTC_OFFSET) && \
    (!defined(FEATURE_MULTIPROCESSOR) || defined(IMAGE_MODEM_PROC))


/*=============================================================================

FUNCTION ATS_RTC_SET_TO_PMIC_RTC

DESCRIPTION
  Synchronize the PMIC's Real Time Clock with the current time-of-day

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void ats_rtc_set_to_pmic_rtc( void )
{

  /* Seconds since January 6, 1980 00:00:00 */
  uint32                          secs;
  /* Milliseconds since January 6, 1980 00:00:00 */
  uint32                          msecs;

  /* Time from the PMIC's Real Time Clock */
  pm_rtc_time_type                  time;

  boolean                         valid = FALSE;

  /* instace to send the display mode parameter */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the local time in seconds */
  secs = time_get_secs();
  msecs = secs * 1000;

  time.sec = secs;
  time.msec = msecs;


  /*set the julian time*/
  valid = time_fn_map_set_pmic_time(&time);

  if(FALSE == valid)
  {
    ATS_ERR_1("Could not set PMIC time. secs: 0x%x", secs);
  } 

} /* ats_rtc_set_to_pmic_rtc */

#endif /* ATS_RTC_SYNC_PMIC_RTC && !FEATURE_TIME_TOD_AS_RTC_OFFSET &&
       (!FEATURE_MULTIPROCESSOR || IMAGE_MODEM_PROC) */


