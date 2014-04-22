#ifndef __DDITIMETICK_H__
#define __DDITIMETICK_H__
/**
* @file DDITimetick.h
* @brief Timetick Device Driver Interface Header File.
* This file contains the definitions of the constants, data
* structures, and interfaces that comprise the DAL Layer of the
* Timetick driver.
*
* Only the interfaces declared shall be used by the client for
* accessing the Timetick DAL driver.
*/

/*
===========================================================================
                             Edit History
$Header: //components/rel/core.mpss/3.10/api/systemdrivers/DDITimetick.h#2 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright (c) 2018 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include "DALDeviceId.h"
#include "DalDevice.h"
#include "DALSys.h"
#include "comdef.h"
#include "DALPropDef.h"

#define DALTIMETICK_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)


/* Enumeration of Time units for setting timers, timetick diffs etc. 

*/
/* ============================================================================
**             Public Data Structures used by the Timetick Dal
** ========================================================================== */
/*!
   @addtogroup timetick_dal_data_structures 

   @{ */
/**
 * Various units supported by the timetick module
 */


typedef enum {
  T_TICK,     /**< -- Return time in Ticks */
  T_USEC,     /**< -- Return time in Microseconds */
  T_MSEC,     /**< -- Return time in Milliseconds */
  T_SEC,      /**< -- Return time in Seconds */
  T_MIN,      /**< -- Return time in Minutes */
  T_HOUR,     /**< -- Return time in Hours */

  T_NONE=T_TICK  /**< -- Identifier to use if no paticular return type is needed */ 

}
DalTimetickUnitType;

typedef uint32                    DalTimetickTime32Type;  /**< -- Time in 32bit ticks*/ 

typedef uint64                    DalTimetickTime64Type; /**< -- Time in 64bit ticks*/ 

#ifndef DOXYGEN_SHOULD_SKIP_THIS

typedef struct DalTimetick DalTimetick;
struct DalTimetick {
  struct DalDevice DalDevice;
  DALResult (*CvtToTimetick32)(DalDeviceHandle * _h, DalTimetickTime32Type  time, DalTimetickUnitType  unit,  DalTimetickTime32Type * time_ret);
  DALResult (*CvtFromTimetick32)(DalDeviceHandle * _h, DalTimetickTime32Type  time, DalTimetickUnitType  unit,  DalTimetickTime32Type * time_ret);
  DALResult (*GetElapsed)(DalDeviceHandle * _h, DalTimetickTime32Type  start, DalTimetickUnitType  unit,  DalTimetickTime32Type * diff);
  DALResult (*Diff)(DalDeviceHandle * _h, DalTimetickTime32Type  start, DalTimetickTime32Type  end, DalTimetickUnitType  unit,  DalTimetickTime32Type * diff);
  DALResult (*GetMs)(DalDeviceHandle * _h,  DalTimetickTime32Type * ms);
  DALResult (*SetNextInterrupt)(DalDeviceHandle * _h, DalTimetickTime32Type  match_count, DalTimetickTime32Type  ticks_now,  DalTimetickTime32Type * match_val);
  DALResult (*Enable)(DalDeviceHandle * _h, boolean  enable);
  DALResult (*IsEnabled)(DalDeviceHandle * _h, boolean * pbIsEnabled);
  DALResult (*GetFreq)(DalDeviceHandle * _h, uint32 * nFreq);
  DALResult (*CvtToTimetick64)(DalDeviceHandle * _h, DalTimetickTime64Type  time, DalTimetickUnitType  unit,  DalTimetickTime64Type * time_ret);
  DALResult (*CvtFromTimetick64)(DalDeviceHandle * _h, DalTimetickTime64Type  time, DalTimetickUnitType  unit,  DalTimetickTime64Type * time_ret);
  DALResult (*UpdateBlastSigId)(DalDeviceHandle * _h, uint32 sig_id);
  DALResult (*CvtToUsec)(DalDeviceHandle * _h, DalTimetickTime32Type time, DalTimetickUnitType   unit, DalTimetickTime64Type *time_ret);
  DALResult (*CvtFromUsec)(DalDeviceHandle * _h, DalTimetickTime64Type time, DalTimetickUnitType   unit, DalTimetickTime32Type *time_ret);
  DALResult (*GetMatchValue)(DalDeviceHandle * _h, DalTimetickTime32Type * match_value);
  DALResult (*TicksToPrecUs)(DalDeviceHandle * h, DalTimetickTime32Type  nTicks, uint64 *pus);
  DALResult (*SetNextInterrupt64)(DalDeviceHandle * _h, DalTimetickTime64Type  match_count, DalTimetickTime64Type  ticks_now,  DalTimetickTime64Type * match_val);
  DALResult (*GetMatchValue64)(DalDeviceHandle * _h, DalTimetickTime64Type * match_value);
  DALResult (*GetInterruptVectorNum)(DalDeviceHandle * _h, uint32 *num);
};

typedef struct DalTimetickHandle DalTimetickHandle; 
struct DalTimetickHandle {
  uint32 dwDalHandleId;
  const DalTimetick * pVtbl;
  void * pClientCtxt;
};

#endif /* #ifndef DOXYGEN_SHOULD_SKIP_THIS */

 /*! 
   @}
 */
 /*  End of Data Structures */

/* ============================================================================
**             Mandatory APIs - Copied from DAL for documentation purposes only
** ========================================================================== */
/*!
   @addtogroup timetick_dal_mandatory_api 

   @{ */
#define DAL_TimetickDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALTIMETICK_INTERFACE_VERSION,hDalDevice)

 /*! 
   @}
 */
 /*  End of Mandatory APIs */


/* ============================================================================
**             Initialization APIs
** ========================================================================== */
/*!
   @addtogroup timetick_dal_intialization_api 

   @{ */


/* ============================================================================
**  Function : DalTimetick_Attach
** ============================================================================
*/
/*!
    @brief
    This function connects to the timetick DAL and gives the caller a handle 
    to the DAL. 

    @details
    The Timetick DAL currently supports two types of timers, 'SystemTimer' and
    'WakeupTimer'. One of these two strings needs to be passed as a parameter 
    to this API. The two strings will be mapped to the corresponding timers on
    each processor on the target.
 
    @param[in] *pSystemTimer   Timer to attach to.
    @param[out] *phDalHandle   Device handle
                     
    @par Dependencies
    None
        
    @par Side Effects
    None
    
    @retval returns 
    DAL_SUCCESS if the attach is successful or if the handle exists already.
    DAL_ERROR if the attach fails due to a wrong parameter passed to pSystemTimer.
    
    @sa None
*/


static __inline DALResult
DalTimetick_Attach 
(
  const char *pSystemTimer,
  DalDeviceHandle** phDalHandle
)
{
  DALResult             eResult = DAL_SUCCESS;
  DALSYSPropertyVar     propVar;
  DALSYS_PROPERTY_HANDLE_DECLARE(hProps);
  DALDEVICEID           devId;

  if (DAL_SUCCESS != (eResult = DAL_StringDeviceAttach((char*)pSystemTimer, phDalHandle)))
  {
    /* If it fails, it could mean that it is a uint32 device id.
    Get the property handle to timetick DAL */
    if (DAL_SUCCESS == (eResult = DALSYS_GetDALPropertyHandle(
                                              DALDEVICEID_TIMETICK,hProps)))
    {
      if (DAL_SUCCESS == (eResult = DALSYS_GetPropertyValue(hProps,
                                                  pSystemTimer,0,&propVar)))
      {
        devId =  (DALDEVICEID)propVar.Val.pdwVal;
        eResult = DAL_DeviceAttach(devId, phDalHandle);
      }
    }
  }
  return eResult;

} /* END DalTimetick_Attach */

/* ============================================================================
**  Function : DalTimetick_Enable
** ============================================================================
*/
/*!
    @brief
    Enables/Disables the interrupt for the timer

    @details

    @param[in] *_h   Handle to the device
    @param[in] enable  Boolean to enable/disable the interrupt.
                   
    @par Dependencies
    None    
    @par Side Effects
    None
    
    @retval always returns DAL_SUCCESS 
    
    @sa None
*/
static __inline DALResult
DalTimetick_Enable
(
  DalDeviceHandle * _h,
  boolean  enable
)
{
  return((DalTimetickHandle *)_h)->pVtbl->Enable( _h, enable);
}

/* ============================================================================
**  Function : DalTimetick_IsEnabled
** ============================================================================
*/
/*!
    @brief
    Checks whether the Timetick module is enabled or not

    @details

    @param[in] *_h        Handle to the device
                   
    @par Dependencies
    None    
    @par Side Effects
    None
    
    @retval returns 
    
    TRUE  -- Module is enabled
    fALSE -- Module is disabled
    
    @sa None
*/
static __inline boolean
DalTimetick_IsEnabled
(
  DalDeviceHandle * _h
)
{
  boolean bIsEnabled = FALSE;
  (void) ((DalTimetickHandle *)_h)->pVtbl->IsEnabled( _h, &bIsEnabled);
  return bIsEnabled;
}

/* ============================================================================
**  Function : DalTimetick_InitTimetick64
** ============================================================================
*/
/*!
    @brief
    This API does nothing. It is kept for compilation compatibility. It will
    be removed during cleanup.
    
    @details 

    @param[in] *_h   Handle to the device
    
    @par Dependencies
    None
    
    @par Side Effects
    None
    
    @retval returns
 
    DAL_SUCCESS always.
    
   
    @sa None
*/

static __inline DALResult
DalTimetick_InitTimetick64
(
  DalDeviceHandle * _h
)
{
  return DAL_SUCCESS;
}

/*! 
   @}
 */
 /*  End of Initialization APIs */

/* ============================================================================
**             Operational APIs
** ========================================================================== */
/*!
   @addtogroup timetick_dal_oper_api 

   @{ */

/* ============================================================================
**  Function : DalTimetick_GetMinRearmSclk
** ============================================================================
*/
/*!
    @brief
    This API does nothing. It is still kept for compilation compatibility. It
    will be removed during cleanup. 
    
    @details

    @param[in] *_h   Handle to the device
    @param[out] *min_rearm   Unused parameter.
    
    @par Dependencies
    None
    
    @par Side Effects
    None 
    
    @retval returns DAL_SUCCESS always
    
    @sa None
*/

static __inline DALResult
DalTimetick_GetMinRearmSclk
(
  DalDeviceHandle * _h,
  DalTimetickTime32Type * min_rearm
)
{
  *min_rearm = 0;

  return DAL_SUCCESS;
}

/* ============================================================================
**  Function : DalTimetick_CvtToTimetick32
** ============================================================================
*/
/*!
    @brief
    Converts the given time value to ticks
    
    @details
    Supports various units of time, ranging from hours to microseconds.

    @param[in] *_h   Handle to the device
    @param[in] time   Time interval to convert to ticks
    @param[in] unit   Units of time interval
    @param[out] *time_ret   # of ticks which correspond to the 
                              given time value

    @par Dependencies
    Valid timetick frequency estimate
    
    @par Side Effects
    None 
    
    @retval returns DAL_SUCCESS always
    
    @sa None
*/

static __inline DALResult
DalTimetick_CvtToTimetick32
(
  DalDeviceHandle * _h, 
  DalTimetickTime32Type  time, 
  DalTimetickUnitType  unit,  
  DalTimetickTime32Type * time_ret
)
{
  return((DalTimetickHandle *)_h)->pVtbl->CvtToTimetick32( _h, time, unit,
                                                      time_ret);
}

/* ============================================================================
**  Function : DalTimetick_CvtFromTimetick32
** ============================================================================
*/
/*!
    @brief
    Converts the ticks to the given unit
    
    @details
    Supports various units of time, ranging from hours to microseconds.

    @param[in] *_h   Handle to the device
    @param[in] time   Time interval to convert from ticks
    @param[in] unit   Units of time interval
    @param[out] *time_ret   Time in the unit requested 

 
    @par Dependencies
    Valid timetick frequency estimate
    
    @par Side Effects
    None 
    
    @retval returns DAL_SUCCESS always
    
    @sa None
*/

static __inline DALResult
DalTimetick_CvtFromTimetick32
(
  DalDeviceHandle * _h,
  DalTimetickTime32Type  time,
  DalTimetickUnitType  unit,
  DalTimetickTime32Type * time_ret
)
{
  return((DalTimetickHandle *)_h)->pVtbl->CvtFromTimetick32( _h, time, unit,
                                                       time_ret);
}

/* ============================================================================
**  Function : DalTimetick_Get
** ============================================================================
*/
/*!
    @brief
    Returns the current timetick count. It is not necessary to attach to 
    the timetick DAL before calling this API anymore. This API directly reads
    the UTIMER registers without accessing the AHB bus.
    
    @details

    @param[in] *_h   Unused parameter
    @param[out] *tick   Timetick counter value    


    @par Dependencies
    None
    
    @par Side Effects
    None 
    
    @retval returns DAL_SUCCESS always
    
    @sa None
*/

static __inline DALResult
DalTimetick_Get
(
  DalDeviceHandle * _h,
  DalTimetickTime32Type * count
)
{
  uint64 utimer = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  __asm__ volatile ("%[utimer] = UTIMER" : [utimer] "=r"(utimer));
  *count = (DalTimetickTime32Type )utimer;

  return DAL_SUCCESS;
  
}

/* ============================================================================
**  Function : DalTimetick_GetRaw
** ============================================================================
*/
/*!
    @brief
    Returns the current timetick count. It is not necessary to attach to 
    the timetick DAL before calling this API anymore. This API directly reads
    the UTIMER registers without accessing the AHB bus.
    
    @details
    Same as DalTimetick_Get.
   
    @param[in] *_h   Unused parameter
    @param[out] *tick   Timetick counter value
    
    @par Dependencies
    None
        
    @par Side Effects
    None 
    
    @retval returns DAL_SUCCESS always
    
    @sa None
*/

static __inline DALResult
DalTimetick_GetRaw
(
  DalDeviceHandle * _h,
  DalTimetickTime32Type * tick
)
{
  uint64 utimer = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  __asm__ volatile ("%[utimer] = UTIMER" : [utimer] "=r"(utimer));
  *tick = (DalTimetickTime32Type )utimer;

  return DAL_SUCCESS;
  
}

/* ============================================================================
**  Function : DalTimetick_GetElapsed
** ============================================================================
*/
/*!
    @brief
    Computes the time elapsed from a previous timetick value
    
    @details
    Supports various units of time, ranging from hours to microseconds.

    @param[in] *_h   Handle to the device
    @param[in] start   Time tick value at the start of the interval
    @param[in] unit   Units to return time interval in
    @param[out] *diff   Elapsed time, in the unit provided
                
    @par Dependencies
    Valid timetick estimate
    
    @par Side Effects
    None 
    
    @retval returns DAL_SUCCESS always
    
    @sa None
*/
static __inline DALResult
DalTimetick_GetElapsed
(
  DalDeviceHandle * _h,
  DalTimetickTime32Type  start,
  DalTimetickUnitType  unit,
  DalTimetickTime32Type * diff
)
{
  return((DalTimetickHandle *)_h)->pVtbl->GetElapsed( _h, start, unit, 
                                                      diff);
}

/* ============================================================================
**  Function : DalTimetick_Diff
** ============================================================================
*/
/*!
    @brief
    Computes the difference between two timetick counts
    
    @details
    Supports various units of time, ranging from hours to microseconds.

    @param[in] *_h   Handle to the device
    @param[in] start   Time tick value at the start of the interval
    @param[in] end   Timetick value at the end of the interval
    @param[in] unit   Units to return time interval in
    @param[out] *diff   Time difference between the two timetick 
                              counts, in the unit given
                
    @par Dependencies
    Valid timetick estimate
    
    @par Side Effects
    None 
    
    @retval returns DAL_SUCCESS always
    
    @sa None
*/

static __inline DALResult
DalTimetick_Diff
(
  DalDeviceHandle * _h,
  DalTimetickTime32Type  start,
  DalTimetickTime32Type  end,
  DalTimetickUnitType  unit,
  DalTimetickTime32Type * diff
)
{
  return((DalTimetickHandle *)_h)->pVtbl->Diff( _h, start, end, unit, diff);
}


/* ============================================================================
**  Function : DalTimetick_GetTimetick64
** ============================================================================
*/
/*!
    @brief
    Reads the 56 bit Qtimer counter and returns it as a 64 bit type. It is not
    necessary to attach to the timetick DAL before calling this API anymore.
    This API directly reads the UTIMER registers without accessing the AHB bus.

    @details

    @param[in] *_h   Unused parameter
    @param[out] *ticks    64-bit tick count

    @par Dependencies
    None

    @par Side Effects
    None

    @retval returns DAL_SUCCESS or DAL_ERROR

    @sa None
*/

static __inline DALResult
DalTimetick_GetTimetick64
(
  DalDeviceHandle * _h,
  DalTimetickTime64Type * ticks
)
{
  uint64 utimer = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  __asm__ volatile ("%[utimer] = UTIMER" : [utimer] "=r"(utimer));
  *ticks = utimer;

  return DAL_SUCCESS;
}

/* ============================================================================
**  Function : DalTimetick_GetMs
** ============================================================================
*/
/*!
    @brief
    Returns the timetick count in milliseconds.
    
    @details
    The value returned is unrelated to CDMA, GPS, HDR, or Time of Day time-stamps.
    It will drift with respect to the above time references. This function
    sacrifices accuracy for speed.

    @param[in] *_h   Handle to the device
    @param[out] *ms     Millisecond tick count
                
    @par Dependencies
    None
    
    @par Side Effects
    None 
    
    @retval returns DAL_SUCCESS always
    
    @sa DalTimetick_CvtFromTimetick32
*/

static __inline DALResult
DalTimetick_GetMs
(
  DalDeviceHandle * _h,  
  DalTimetickTime32Type * ms
)
{
  return((DalTimetickHandle *)_h)->pVtbl->GetMs( _h, ms);
}


/* ============================================================================
**  Function : DalTimetick_UpdateFreq
** ============================================================================
*/
/*!
    @brief
    This API does nothing. It is still kept for compilation compatibility. It
    will be removed during cleanup.
    
    @details
 
    @param[in] *_h   Handle to the device
    @param[in] freq     Unused parameter.
 
               
    @par Dependencies
    None
    
    @par Side Effects
    None
    
    @retval returns DAL_SUCCESS always
    
    @sa None
*/

static __inline DALResult
DalTimetick_UpdateFreq
(
  DalDeviceHandle * _h,
  uint32  freq
)
{
  return DAL_SUCCESS;
}

/* ============================================================================
**  Function : DalTimetick_SetOffset
** ============================================================================
*/
/*!
    @brief
    This API does nothing. It is still kept for compilation compatibility. It
    will be removed during cleanup.
     
    @details

    @param[in] *_h   Handle to the device
    @param[in] delta   Unused parameter.
           
    @par Dependencies
    None
    
    @par Side Effects
    None
    
    @retval returns DAL_SUCCESS always
    
    @sa None
*/

static __inline DALResult
DalTimetick_SetOffset
(
  DalDeviceHandle * _h,
  DalTimetickTime32Type  delta
)
{
  return DAL_SUCCESS;
}


/* ============================================================================
**  Function : DalTimetick_GetOffset
** ============================================================================
*/
/*!
    @brief
    This API does nothing. It is still kept for compilation compatibility. It
    will be removed during cleanup.
    
    @details

    @param[in] *_h   Handle to the device
    @param[out] *delta  Unused parameter.

    @par Dependencies
    None
    
    @par Side Effects
    None
    
    @retval returns DAL_SUCCESS always
    
    @sa None
*/

static __inline DALResult
DalTimetick_GetOffset
(
  DalDeviceHandle * _h,
  DalTimetickTime32Type *  delta
)
{
  *delta = 0;

  return DAL_SUCCESS;
}

/* ============================================================================
**  Function : DalTimetick_SetNextInterrupt
** ============================================================================
*/
/*!
    @brief
    Programs the timer to generate an interrupt at the given value

    @details

    @param[in] *_h   Handle to the device
    @param[in] match_count  Timetick count at which next interrupt 
                                  will occur
    @param[in] ticks_now     Current timetick count
    @param[out] *match_val   Matchvalue set

                    
    @par Dependencies

    
    @par Side Effects
    Interrupt would be generated when timetick counter reaches given value
    
    @retval returns DAL_SUCCESS always
    
    @sa None
*/

static __inline DALResult
DalTimetick_SetNextInterrupt
(
  DalDeviceHandle * _h,
  DalTimetickTime32Type  match_count,
  DalTimetickTime32Type  ticks_now,
  DalTimetickTime32Type * match_val
)
{
  return((DalTimetickHandle *)_h)->pVtbl->SetNextInterrupt( _h, 
                                       match_count, ticks_now, match_val);
}

/* ============================================================================
**  Function : Timetick_GetFreq
** ============================================================================
*/
/*!
    @brief
    Returns the timetick frequency in Hz.

    @details
 
    @param[in] *_h   Handle to the device
    @param[out] *nFreq    timetick frequency
                    
    @par Dependencies
    None    
    @par Side Effects
    None
    
    @retval returns DAL_SUCCESS always
    
    @sa None
*/

static __inline DALResult
DalTimetick_GetFreq
(
  DalDeviceHandle * _h,
  uint32 *nFreq
)
{
  return((DalTimetickHandle *)_h)->pVtbl->GetFreq( _h, nFreq);
}

/* ============================================================================
**  Function : DalTimetick_CvtToTimetick64
** ============================================================================
*/
/*!
    @brief
    Converts the given time to 64 bit ticks

    @details

    @param[in] *_h   Handle to the device
    @param[in] time    Time interval to convert to ticks
    @param[in] unit     Units of time interval
    @param[out] *time_ret    Time in the unit requested
                    
    @par Dependencies
    None  
      
    @par Side Effects
    None
    
    @retval returns DAL_SUCCESS always
    
    @sa None
*/
static __inline DALResult
DalTimetick_CvtToTimetick64
(
  DalDeviceHandle       *_h,
  DalTimetickTime64Type time,
  DalTimetickUnitType   unit,
  DalTimetickTime64Type *time_ret
)
{
  return((DalTimetickHandle *)_h)->pVtbl->CvtToTimetick64( _h, time, unit,
                                                       time_ret);
}

/* ============================================================================
**  Function : DalTimetick_CvtFromTimetick64
** ============================================================================
*/
/*!
    @brief
    Converts 64 bit ticks to the given unit

    @details
 
    @param[in] *_h   Handle to the device
    @param[in] time    Time interval to convert from ticks
    @param[in] unit     Units of time interval
    @param[out] *time_ret    Time in the unit requested

    @par Dependencies
    None    

    @par Side Effects
    None
    
    @retval returns DAL_SUCCESS or DAL_ERROR
    
    @sa None
*/

static __inline DALResult
DalTimetick_CvtFromTimetick64
(
  DalDeviceHandle       *_h,
  DalTimetickTime64Type time,
  DalTimetickUnitType   unit,
  DalTimetickTime64Type *time_ret
)
{
  return((DalTimetickHandle *)_h)->pVtbl->CvtFromTimetick64( _h, time, unit,
                                                       time_ret);
}

/* ============================================================================
**  Function : DalTimetick_UpdateBlastSigId
** ============================================================================
*/
/*!
    @brief
    Updates the timetick driver with the blast signal Id used for RGPT.

    @details
 
    @param[in] *_h   Handle to the device
    @param[in] sig_id    signal id for blast signal corresponding 
                                  to RGPT.
    
    @par Dependencies
    None    

    @par Side Effects
    This API needs to be called atleast once for the RGPT timer, to update the 
    timetick driver with the signal id, using which the timetick driver sets 
    the RGPT's match value when DalTimetick_SetNextInterrupt is called.

    @retval returns DAL_SUCCESS always
    
    @sa None
*/

static __inline DALResult
DalTimetick_UpdateBlastSigId
(
  DalDeviceHandle * _h,
  uint32   sig_id
)
{
  return((DalTimetickHandle *)_h)->pVtbl->UpdateBlastSigId( _h, sig_id);
}

/* ============================================================================
**  Function : DalTimetick_CvtToUsec
** ============================================================================
*/
/*!
    @brief
    Converts the given value to microseconds.
 
    @details
    Supports various units of time, ranging from hours to microseconds.

    @param[in] *_h   Handle to the device
    @param[in] time   Time interval to convert from
    @param[in] unit   Units of time interval
    @param[out] *time_ret   Time in microseconds

 
    @par Dependencies
    Valid timetick frequency estimate
 
    @par Side Effects
    None
 
    @retval returns DAL_SUCCESS always
    
    @sa None
*/

static __inline DALResult
DalTimetick_CvtToUsec
( 
  DalDeviceHandle * _h, 
  DalTimetickTime32Type time, 
  DalTimetickUnitType   unit, 
  DalTimetickTime64Type *time_ret
) 
{
  return((DalTimetickHandle *)_h)->pVtbl->CvtToUsec( _h, time, unit, time_ret);
}

/* ============================================================================
**  Function : DalTimetick_CvtFromUsec
** ============================================================================
*/
/*!
    @brief
    Converts the given microsecs value to ticks.

    @details
    Supports various units of time, ranging from hours to microseconds.

    @param[in] *_h   Handle to the device
    @param[in] time   Time in microseconds
    @param[in] unit   Units of time interval to convert to
    @param[out] *time_ret   Time in the unit requested

 
    @par Dependencies
    Valid timetick frequency estimate
 
    @par Side Effects
    None
 
    @retval returns DAL_SUCCESS always
*/
static __inline DALResult
DalTimetick_CvtFromUsec
( 
  DalDeviceHandle * _h, 
  DalTimetickTime64Type time, 
  DalTimetickUnitType   unit, 
  DalTimetickTime32Type *time_ret
) 
{
  return((DalTimetickHandle *)_h)->pVtbl->CvtFromUsec( _h, time, unit, time_ret);
}

/* ============================================================================
**  Function : DalTimetick_GetMatchValue
** ============================================================================
*/
/*!
    @brief
    Returns the last programmed timetick match value.
    
    @details
    The returned timetick match value is corrected with the
    timetick offset.

    @param[in] *_h   Handle to the device
    @param[out] *match_value   Last programmed Timetick match value    


    @par Dependencies
    None
    
    @par Side Effects
    None 
    
    @retval returns DAL_SUCCESS always
    
    @sa None
*/
static __inline DALResult
DalTimetick_GetMatchValue
(
  DalDeviceHandle * _h,  
  DalTimetickTime32Type * match_value
)
{
  return((DalTimetickHandle *)_h)->pVtbl->GetMatchValue( _h, match_value);
}


/* ============================================================================
**  Function : DalTimetick_TicksToPrecUs
** ============================================================================
*/
/*!
    @brief
    Converts the given ticks to precision microseconds.
        
    @details
    This function also supports the full range of timeticks.

    @param[in] *_h    Handle to the device
    @param[in] nTicks Duration in ticks to be converted into 
                      microseconds   
    @param[out] pus   Number of microseconds which corresponds
                      to the given time value

    @par Dependencies
    None
    
    @par Side Effects
    None 
    
    @retval returns DAL_SUCCESS always
    
    @sa None
*/
static __inline DALResult
DalTimetick_TicksToPrecUs
(
  DalDeviceHandle * _h, 
  DalTimetickTime32Type  nTicks, 
  uint64 *pus
)
{
  return((DalTimetickHandle *)_h)->pVtbl->TicksToPrecUs( _h, nTicks, pus);
}

/* ============================================================================
**  Function : DalTimetick_SetNextInterrupt64
** ============================================================================
*/
/*!
    @brief
    Converts the given ticks to precision microseconds.
        
    @details
    This function also supports the full range of timeticks.

    @param[in] *_h    Handle to the device
    @param[in] nTicks Duration in ticks to be converted into 
                      microseconds   
    @param[out] pus   Number of microseconds which corresponds
                      to the given time value

    @par Dependencies
    None
    
    @par Side Effects
    None 
    
    @retval returns DAL_SUCCESS always
    
    @sa None
*/

static __inline DALResult
DalTimetick_SetNextInterrupt64
(
  DalDeviceHandle * _h, 
  DalTimetickTime64Type  match_count, 
  DalTimetickTime64Type  ticks_now,  
  DalTimetickTime64Type * match_val
)
{
  return((DalTimetickHandle *)_h)->pVtbl->SetNextInterrupt64( _h, 
                                                              match_count, 
                                                              ticks_now, 
                                                              match_val);
}


/* ============================================================================
**  Function : DalTimetick_GetMatchValue64
** ============================================================================
*/
/*!
    @brief
    Converts the given ticks to precision microseconds.
        
    @details
    This function also supports the full range of timeticks.

    @param[in] *_h    Handle to the device
    @param[in] nTicks Duration in ticks to be converted into 
                      microseconds   
    @param[out] pus   Number of microseconds which corresponds
                      to the given time value

    @par Dependencies
    None
    
    @par Side Effects
    None 
    
    @retval returns DAL_SUCCESS always
    
    @sa None
*/

static __inline DALResult
DalTimetick_GetMatchValue64
(
   DalDeviceHandle * _h, 
  DalTimetickTime64Type * match_val
) 
{
  return((DalTimetickHandle *)_h)->pVtbl->GetMatchValue64( _h,
                                                            match_val);
}



/* ============================================================================
**  Function : DalTimetick_RegFreqChangeNotification
** ============================================================================
*/
/*!
    @brief
    This API does nothing. It is still kept for compilation compatibility. It
    will be removed during cleanup.

    @details


    @param[in] *_h    Handle to the device
    @param[in] nTicks Unused parameter.
    @param[out] pus   Unused paramter.

    @par Dependencies
    None

    @par Side Effects
    None

    @retval returns DAL_SUCCESS always

    @sa None
*/

static __inline DALResult
DalTimetick_RegFreqChangeNotification
(
   DalDeviceHandle * _h,
   DALSYSCallbackFunc cbFunc,
  DALSYSCallbackFuncCtx cbFuncCtx
)
{
  return DAL_SUCCESS;
}




/* ============================================================================
**  Function : DalTimetick_GetInterruptVector
** ============================================================================
*/
/*!
    @brief
    Returns the interrupt vector number associated with the qtimer.
        
    @details
    None

    @param[in] *_h    Handle to the device
    @param[out] num   Interrupt vector number of the qtimer.

    @par Dependencies
    None
    
    @par Side Effects
    None 
    
    @retval returns DAL_SUCCESS always
    
    @sa None
*/

static __inline DALResult
DalTimetick_GetInterruptVectorNum
(
   DalDeviceHandle * _h, 
   uint32 * num 
) 
{
  return((DalTimetickHandle *)_h)->pVtbl->GetInterruptVectorNum( _h,
                                                            num);
}


 /*! 
   @}
 */
 /*  End of Operational APIs */

#endif


