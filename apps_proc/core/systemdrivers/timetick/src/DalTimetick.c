/*==============================================================================

FILE:      DalTimetick.c

DESCRIPTION: This file implements the Timetick Device Driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2016 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
============================================================================
                              EDIT HISTORY
  $Header: //components/rel/core.tx/6.0/systemdrivers/timetick/src/DalTimetick.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 04/23/11   pbi      Added support for PMU timer
=========================================================================*/

/*========================================================================
                           INCLUDES
========================================================================*/
#include "DalTimetick.h"
#include "DDITimer.h"
#include "TimerHWIO.h"
#include "DDIHWIO.h"
#include "HALhwio.h"
#include "err.h"
#include <stdlib.h>
/*========================================================================

                           DATA DEFINITIONS

========================================================================*/

/*-----------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------*/

DALResult 
Timetick_DriverInit(TimetickDrvCtxt *pCtxt)
{
  return DAL_SUCCESS;
}

DALResult 
Timetick_DriverDeInit(TimetickDrvCtxt *pCtxt)
{
  return DAL_SUCCESS;
}

/*-------------------------------------------------------------------------
Following functions are declared in DalDevice Interface. 
-------------------------------------------------------------------------*/
DALResult 
Timetick_DeviceInit(TimetickClientCtxt *pCtxt)
{
  DALResult        result = DAL_SUCCESS;


  static DALSYSPropertyVar system_property_var; 
  /*declare property handle variable */
  static DALSYS_PROPERTY_HANDLE_DECLARE(timetick_property); 

  /* Get the propery handle to timetick DAL */
  if(pCtxt->pTimetickDevCtxt->DevId == 0)
  {
    if(DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr((char*)(pCtxt->pTimetickDevCtxt->strDeviceName),timetick_property))
    {
        return DAL_ERROR;
    }

  }else
  {
     if(DAL_SUCCESS != DALSYS_GetDALPropertyHandle(pCtxt->pTimetickDevCtxt->DevId,timetick_property))
     {
         return DAL_ERROR; 
     }
  }
  
  //ULog Initialization
  if(DAL_SUCCESS == ULogFront_RealTimeInit (&pCtxt->hTimeTickLog, "TimeTick Log",
                            TIMETICK_LOG_SIZE,
                            ULOG_MEMORY_LOCAL, ULOG_LOCK_OS))
  {
    ULogCore_HeaderSet (pCtxt->hTimeTickLog,
                      "Content-Type: text/tagged-log-1.0;");
  }
  result = DALSYS_GetPropertyValue(timetick_property,
                                 "QTIMER_INTVECTNUM",
                         0,&system_property_var); 
  if(result != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }
  pCtxt->pTimetickDevCtxt->QTimerIntVectNum = system_property_var.Val.dwVal;  

  result = DALSYS_GetPropertyValue(timetick_property,
                                "QTIMER_BASE",
                           0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    pCtxt->pTimetickDevCtxt->QTimerBase = system_property_var.Val.dwVal;
  }
  else
  {
   return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Attach to the HWIO DAL.                                               */
  /*-----------------------------------------------------------------------*/
  result = DAL_DeviceAttach(DALDEVICEID_HWIO, &pCtxt->pTimetickDevCtxt->phDalHWIO);
  if (result == DAL_SUCCESS)
  {
    result = DalHWIO_MapRegionByAddress(pCtxt->pTimetickDevCtxt->phDalHWIO, 
                              (uint8*)(system_property_var.Val.dwVal),
                              (uint8 **)&(pCtxt->pTimetickDevCtxt->QTimerBase));
    if (result != DAL_SUCCESS)
    {
      return DAL_ERROR;
    }
  }

	result = DALSYS_GetPropertyValue(timetick_property,
                                "QTIMER_AC_OFFSET",
                        0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
   pCtxt->pTimetickDevCtxt->QTimerACBase = pCtxt->pTimetickDevCtxt->QTimerBase +
                      system_property_var.Val.dwVal;
  
  }
  else
  {
   return DAL_ERROR;
  }
  result = DALSYS_GetPropertyValue(timetick_property,
                                "QTIMER_OFFSET",
                        0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    pCtxt->pTimetickDevCtxt->QTimerBase = pCtxt->pTimetickDevCtxt->QTimerBase + 
                      system_property_var.Val.dwVal;
  }
  else
  {
    return DAL_ERROR;
  }

  result = DALSYS_GetPropertyValue(timetick_property,
                              "QTIMER_FRAME",
                         0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    pCtxt->pTimetickDevCtxt->QTimerFrame = system_property_var.Val.dwVal;
  }
  else
  {
    return DAL_ERROR;
  }
  result = DALSYS_GetPropertyValue(timetick_property,
                              "QTIMER_CNT_VIRTUAL_OFFSET",
                         0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    pCtxt->pTimetickDevCtxt->QTimerFrameCNTVirtualOffset = system_property_var.Val.dwVal;
  }
  else
  {
    return DAL_ERROR;
  }
    result = DALSYS_GetPropertyValue(timetick_property,
                              "QTIMER_CVAL_VIRTUAL_OFFSET",
                         0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    pCtxt->pTimetickDevCtxt->QTimerFrameCVALVirtualOffset = system_property_var.Val.dwVal;
  }
  else
  {
    return DAL_ERROR;
  }
  HWIO_QTMR_AC_CNTACR_n_OUTI(pCtxt->pTimetickDevCtxt->QTimerACBase,pCtxt->pTimetickDevCtxt->QTimerFrame,0x3F);
  
  pCtxt->pTimetickDevCtxt->freq = HWIO_QTMR_CNTFRQ_IN(pCtxt->pTimetickDevCtxt->QTimerBase);

  /* Create a sync object to guard critical sections */
  pCtxt->pTimetickDevCtxt->mSyncObj = (qurt_mutex_t *)malloc(sizeof(qurt_mutex_t));
  if(pCtxt->pTimetickDevCtxt->mSyncObj == NULL)
  {
    ERR_FATAL
        ("Failed to allocate memory to the synchronization object", 
         0, 0, 0);
  }
  qurt_mutex_init(pCtxt->pTimetickDevCtxt->mSyncObj);
  
   return result;
}

DALResult 
Timetick_DeviceDeInit(TimetickClientCtxt *pCtxt)
{
    DALResult        result = DAL_SUCCESS;

    /* Delete the no-preemption sync object*/
    qurt_mutex_destroy(pCtxt->pTimetickDevCtxt->mSyncObj);

    return result;
}

DALResult 
Timetick_PowerEvent(TimetickClientCtxt *pCtxt, DalPowerCmd PowerCmd, 
                    DalPowerDomain PowerDomain)
{
    return DAL_SUCCESS;
}

DALResult 
Timetick_Open(TimetickClientCtxt *pCtxt, uint32 dwaccessMode )
{
    return DAL_SUCCESS;
}

DALResult 
Timetick_Close(TimetickClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult 
Timetick_Info(TimetickClientCtxt *pCtxt,DalDeviceInfo *pdeviceInfo, uint32 dwSize)
{
    return DAL_SUCCESS;
}
/*=============================================================================

                   INTERNAL FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

FUNCTION CVT_TICKS_TO_MS

DESCRIPTION
  Converts the given ticks to milliseconds

DEPENDENCIES
  Valid ticks 

RETURN VALUE
  # of milliseconds which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

static uint32 cvt_ticks_to_ms
(
  uint32                ticks,
  uint32                nFreq
)
{
  uint32                milliseconds = 0;
    /* Seconds portion of duration */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Convert ticks to milliseconds, with rounding. */
  milliseconds = ((uint64)ticks * 1000 + nFreq/2) / nFreq;
 
  return milliseconds;

} /* cvt_ticks_to_ms */


/*=============================================================================

FUNCTION CVT_TIMETICK64_TO_MS

DESCRIPTION
  Converts the given ticks value to milliseconds

DEPENDENCIES
  Valid ticks

RETURN VALUE
  # of milliseconds which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

static uint64 cvt_timetick64_to_ms
(
  uint64                ticks,
    /* Duration to be converted into milliseconds */
  uint32                nFreq
)
{
  /* Seconds portion of duration */
  uint64                          milliseconds = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ticks >= nFreq )
  {
    /* Compute seconds portion of time period first, to avoid overflows */
    milliseconds  = (ticks / nFreq) * 1000;
    ticks        %= nFreq;
  }

  milliseconds += 
     (ticks * 1000 + nFreq/2) / nFreq;
    /* Convert remaining ticks to milliseconds, with rounding. */

  return milliseconds;

} /* cvt_timetick64_to_ms */



/*------------------------------------------------------------------------------
Following functions are extended in DalTimetick Interface. 
------------------------------------------------------------------------------*/

/*==========================================================================

  FUNCTION      Timetick_GetMinRearmSclk

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult Timetick_GetMinRearmSclk
(
  TimetickClientCtxt      *pCtxt,
  DalTimetickTime32Type   *pMinRearm
)
{
  *pMinRearm = 0;

  return DAL_SUCCESS;

} /* END Timetick_GetMinRearmSclk */
/*==========================================================================

  FUNCTION      Timetick_CvtToTimetick32

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult 
Timetick_CvtToTimetick32
(
  TimetickClientCtxt      *pCtxt,
  DalTimetickTime32Type   time,
  DalTimetickUnitType     unit,
  DalTimetickTime32Type   *pTimeRet
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( unit )
  {
    case T_HOUR:
      /* Convert hours to minutes */
      time *= 60;
      /*lint -fallthrough */

    case T_MIN:
      /* Convert minutes to seconds */
      time *= 60;
      /*lint -fallthrough */

    case T_SEC:
      /* Seconds to ticks */
      time *= pCtxt->pTimetickDevCtxt->freq;
      break;

    case T_MSEC:
      /* Milliseconds to ticks */
      time = ((uint64)time * pCtxt->pTimetickDevCtxt->freq) / 1000;
      break;

    case T_USEC:
      /* Microseconds to ticks */
      time = ((uint64)time * pCtxt->pTimetickDevCtxt->freq) / 1000000;
      break;

    case T_TICK:
      /* time = time; */
      break;

    default:
      ULOG_RT_PRINTF_1 (pCtxt->hTimeTickLog, 
                      "Timetick_CvtToTimetick32: Invalid timetick conversion %d", 
                      unit);
      break;
  }

  *pTimeRet = time;

   return DAL_SUCCESS;
} /* END Timetick_CvtToTimetick32 */


/*==========================================================================

  FUNCTION      Timetick_CvtFromTimetick32

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult Timetick_CvtFromTimetick32
( 
  TimetickClientCtxt      *pCtxt, 
  DalTimetickTime32Type   time, 
  DalTimetickUnitType     unit, 
  DalTimetickTime32Type   *pTimeRet
) 
{
   /* Factor for converting to hours or minutes */
  uint32                          scale = 1;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( unit )
  {
    case T_HOUR:
      /*  Scale minutes to hours */
      scale *= 60;
      /*lint -fallthrough */

    case T_MIN:
      /* Scale seconds to minutes */
      scale *= 60;
      /*lint -fallthrough */

    case T_SEC:
      /* Convert ticks to seconds (or minutes, or hours) */
      time /= (uint64)pCtxt->pTimetickDevCtxt->freq * scale;
      break;

    case T_MSEC:
      /* Convert ticks to milliseconds */
      time = cvt_ticks_to_ms( time, pCtxt->pTimetickDevCtxt->freq );
      break;

    case T_USEC:
      /* Convert ticks to microseconds */
      time = cvt_ticks_to_ms( time * 10, pCtxt->pTimetickDevCtxt->freq ) * 100;
      break;

    case T_TICK:
      /* time = time; */
      break;

    default:

     ULOG_RT_PRINTF_1 (pCtxt->hTimeTickLog,
                "Timetick_CvtFromTimetick32: Invalid timetick conversion %d",
                 unit);
      break;
  }
  *pTimeRet = time;

  return DAL_SUCCESS;

}  /* END Timetick_CvtFromTimetick32 */

/*=============================================================================

  FUNCTION      Timetick_CvtToTimetick64

  DESCRIPTION   See DDITimetick.h
=============================================================================*/
DALResult Timetick_CvtToTimetick64
(
  TimetickClientCtxt    *pCtxt,
  DalTimetickTime64Type time,
  DalTimetickUnitType   unit,
  DalTimetickTime64Type *pTimeRet
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( unit )
  {
    case T_HOUR:
      /* Convert hours to minutes */
      time *= 60;
      /* fall-through */

    case T_MIN:
      /* Convert minutes to seconds */
      time *= 60;
      /* fall-through */

    case T_SEC:
      /* Seconds to ticks */
      time *= pCtxt->pTimetickDevCtxt->freq;
      break;

    case T_MSEC:
      /* Milliseconds to ticks */
      time = (time * pCtxt->pTimetickDevCtxt->freq)/ 1000;
      break;

    case T_USEC:
      /* Microseconds to ticks */
      time = ((time * pCtxt->pTimetickDevCtxt->freq) / 1000000);
      break;

    case T_TICK:
      /* time = time; */
      break;

    default:
      ULOG_RT_PRINTF_1 (pCtxt->hTimeTickLog, 
                "Timetick_CvtToTimetick64: Invalid timetick conversion %d",
                 unit);
      break;

  }

  *pTimeRet = time;

  return DAL_SUCCESS;

} /* Timetick_CvtToTimetick64 */

/*=============================================================================

  FUNCTION      Timetick_CvtFromTimetick64

  DESCRIPTION   See DDITimetick.h
=============================================================================*/
DALResult Timetick_CvtFromTimetick64
(
  TimetickClientCtxt    *pCtxt,
  DalTimetickTime64Type time,
  DalTimetickUnitType   unit,
  DalTimetickTime64Type *pTimeRet
)
{
  /* Factor for converting to hours or minutes */
  uint64                          scale = 1;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( unit )
  {
    case T_HOUR:
      /*  Scale minutes to hours */
      scale *= 60;
      /* fall-through */

    case T_MIN:
      /* Scale seconds to minutes */
      scale *= 60;
      /* fall-through */

    case T_SEC:
      /* Convert slow clocks to seconds (or minutes, or hours) */
      time /= pCtxt->pTimetickDevCtxt->freq * scale;
      break;

    case T_MSEC:
      /* Convert ticks to milliseconds */
      time = cvt_timetick64_to_ms( time, pCtxt->pTimetickDevCtxt->freq );
      break;

    case T_USEC:
      /* Convert ticks to microseconds */
      time = ((uint64)time * 1000000) / (uint64)pCtxt->pTimetickDevCtxt->freq;
      break;

    case T_TICK:
      /* time = time; */
      break;

    default:
      ULOG_RT_PRINTF_1 (pCtxt->hTimeTickLog, 
                "Timetick_CvtFromTimetick64: Invalid timetick conversion %d"
                , unit);
      break;
  }

  *pTimeRet = time;

  return DAL_SUCCESS;

} /* Timetick_CvtFromTimetick64 */


/*==========================================================================

  FUNCTION      Timetick_GetRaw

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult Timetick_GetRaw
(
  TimetickClientCtxt          *pCtxt,
  DalTimetickTime32Type         *pTick
)
{
  if (pCtxt->pTimetickDevCtxt->QTimerBase != NULL )
  {
    *pTick = HWIO_QTMR_CNTCT_LO_IN((pCtxt->pTimetickDevCtxt->QTimerBase + pCtxt->pTimetickDevCtxt->QTimerFrameCNTVirtualOffset));
  }
  
  return DAL_SUCCESS;
}/* END Timetick_GetRaw */


/*=============================================================================

FUNCTION Timetick_Enable
 
DESCRIPTION See DDITimetick.h

=============================================================================*/


DALResult Timetick_Enable
(
    TimetickClientCtxt *pCtxt,
    boolean             enable
)
{
  HWIO_QTMR_CNT_CTL_OUTM((pCtxt->pTimetickDevCtxt->QTimerBase + pCtxt->pTimetickDevCtxt->QTimerFrameCVALVirtualOffset ),
                          HWIO_QTMR_CNT_CTL_EN_BMSK,enable);
  return DAL_SUCCESS;
}/* END Timetick_Enable */


/*==========================================================================

  FUNCTION      Timetick_SetNextInterrupt

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult Timetick_SetNextInterrupt
(
  TimetickClientCtxt          *pCtxt,
  DalTimetickTime32Type         matchCount,
  DalTimetickTime32Type         ticksNow,
  DalTimetickTime32Type         *matchVal
)
{

  HWIO_QTMR_CNT_CVAL_LO_OUT((pCtxt->pTimetickDevCtxt->QTimerBase + pCtxt->pTimetickDevCtxt->QTimerFrameCVALVirtualOffset), matchCount);
  *matchVal = HWIO_QTMR_CNT_CVAL_LO_IN((pCtxt->pTimetickDevCtxt->QTimerBase + pCtxt->pTimetickDevCtxt->QTimerFrameCVALVirtualOffset));

  pCtxt->pTimetickDevCtxt->last_match_value = *matchVal;
  return DAL_SUCCESS;

} /* END Timetick_SetNextInterrupt */


 /*==========================================================================

  FUNCTION      Timetick_SetOffset

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult Timetick_SetOffset
(
  TimetickClientCtxt          *pCtxt,
  DalTimetickTime32Type          timetickDelta
)
{

//  do nothing

  return DAL_SUCCESS;
} /* END Timetick_SetOffset */

 /*==========================================================================

  FUNCTION      Timetick_GetOffset

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult Timetick_GetOffset
(
  TimetickClientCtxt           *pCtxt,
  DalTimetickTime32Type          *timetickDelta
)
{
  *timetickDelta = 0;
  return DAL_SUCCESS;
} /* END Timetick_GetOffset */

/*==========================================================================

  FUNCTION      Timetick_Get

  DESCRIPTION   See DDITimetick.h

==========================================================================*/
DALResult Timetick_Get
(
  TimetickClientCtxt          *pCtxt,
  DalTimetickTime32Type       *pTick
)
{

  if (pCtxt->pTimetickDevCtxt->QTimerBase != NULL )
  {
    *pTick = HWIO_QTMR_CNTCT_LO_IN((pCtxt->pTimetickDevCtxt->QTimerBase + pCtxt->pTimetickDevCtxt->QTimerFrameCNTVirtualOffset));
  }
  return DAL_SUCCESS;

} /* END Timetick_Get */

/*==========================================================================

  FUNCTION      Timetick_InitTimetick64

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult Timetick_InitTimetick64
(
  TimetickClientCtxt * pCtxt
)
{
  /* do nothing*/
  return DAL_SUCCESS;

} /* END Timetick_InitTimetick64 */

/*==========================================================================

  FUNCTION      Timetick_GetTimetick64

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult Timetick_GetTimetick64
(
  TimetickClientCtxt    *pCtxt,
  uint64                *pTicks
)
{
  DalTimetickTime32Type   nCountLow, nCountHi1,nCountHi2;

  if (pCtxt->pTimetickDevCtxt->QTimerBase != NULL )
  {
    do
    {
      nCountHi1 = HWIO_QTMR_CNTCT_HI_IN((pCtxt->pTimetickDevCtxt->QTimerBase + pCtxt->pTimetickDevCtxt->QTimerFrameCNTVirtualOffset));
      nCountLow = HWIO_QTMR_CNTCT_LO_IN((pCtxt->pTimetickDevCtxt->QTimerBase + pCtxt->pTimetickDevCtxt->QTimerFrameCNTVirtualOffset));
   
      /* repeat the above until the same value is read from the   
        high count register successively */
      nCountHi2 = HWIO_QTMR_CNTCT_HI_IN((pCtxt->pTimetickDevCtxt->QTimerBase + pCtxt->pTimetickDevCtxt->QTimerFrameCNTVirtualOffset));
    }while (nCountHi1 != nCountHi2);
  
    *pTicks = (((uint64)nCountHi1 <<32)| nCountLow);
  }
 
  return DAL_SUCCESS;

} /* END Timetick_GetTimetick64 */

/*==========================================================================

  FUNCTION      Timetick_GetElapsed

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult Timetick_GetElapsed
(
  TimetickClientCtxt      *pCtxt,
  DalTimetickTime32Type   start,
  DalTimetickUnitType     unit,
  uint32                  *pDiff
)
{
  /* Time Tick count */
  DalTimetickTime32Type                   end = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the current timetick count value for the end of the interval */
  Timetick_Get(pCtxt, &end);

  /* Compute and return the difference between the timestamps */
  Timetick_Diff(pCtxt, start, end, unit, pDiff);

  return DAL_SUCCESS;
}/* END Timetick_GetElapsed */

/*==========================================================================

  FUNCTION      Timetick_Diff

  DESCRIPTION   See DDITimetick.h

==========================================================================*/
DALResult Timetick_Diff
(
  TimetickClientCtxt          *pCtxt,
  DalTimetickTime32Type       start,
  DalTimetickTime32Type       end,
  DalTimetickUnitType         unit,
  DalTimetickTime32Type       *pDiff
)
{
     /* Time Tick count */
  DalTimetickTime32Type                   delta;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Compute the difference between the timetick timestamps */
  delta = end - start;

  /* Compute and return the difference between the timestamps */
  Timetick_CvtFromTimetick32( pCtxt, delta, unit, pDiff);

  return DAL_SUCCESS;
}/* END Timetick_Diff */

/*=============================================================================

FUNCTION Timetick_GetMs

DESCRIPTION   See DDITimetick.h

=============================================================================*/

DALResult Timetick_GetMs
(
    TimetickClientCtxt    *pCtxt,
    DalTimetickTime32Type *pMsecs
)
{
  /* Current time (or delta) in ticks */
  DalTimetickTime32Type                   ticks = 0;

  /* Seconds since origin time */
  DalTimetickTime32Type                   secs = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Start critical section */
  qurt_mutex_lock(pCtxt->pTimetickDevCtxt->mSyncObj);

  /* Get the elapsed time since the arbitrary origin */
  Timetick_Get(pCtxt, &ticks);
  
  ticks = ticks - pCtxt->pTimetickDevCtxt->ms_info.origin;

  /* Compute number of seconds that have elapsed. */
  secs = ticks / pCtxt->pTimetickDevCtxt->freq;

  /* If over a second has elapsed, advance origin. */
  if ( secs > 0 )
  {
    ticks                  -= secs * pCtxt->pTimetickDevCtxt->freq;
    pCtxt->pTimetickDevCtxt->ms_info.origin += secs * pCtxt->pTimetickDevCtxt->freq;
    pCtxt->pTimetickDevCtxt->ms_info.msec += secs * 1000;
  }

  /* Compute millisecond timestamp */
  *pMsecs = ticks * 1000 / pCtxt->pTimetickDevCtxt->freq + pCtxt->pTimetickDevCtxt->ms_info.msec;

  /* End critical section */
  qurt_mutex_unlock(pCtxt->pTimetickDevCtxt->mSyncObj);
  
  return DAL_SUCCESS;

} /* END Timetick_GetMs */

/*=============================================================================

FUNCTION Timetick_GetFreq

DESCRIPTION   See DDITimetick.h

=============================================================================*/
DALResult 
Timetick_GetFreq( TimetickClientCtxt * pCtxt, uint32 *nFreq) 
{
  if (pCtxt->pTimetickDevCtxt->QTimerBase != NULL )
  {
    *nFreq = HWIO_QTMR_CNTFRQ_IN(pCtxt->pTimetickDevCtxt->QTimerBase);
  }
  pCtxt->pTimetickDevCtxt->freq = *nFreq;

    return DAL_SUCCESS;
}/* END Timetick_GetFreq */

/*=============================================================================

FUNCTION Timetick_UpdateBlastSigId

DESCRIPTION   See DDITimetick.h

=============================================================================*/
DALResult 
Timetick_UpdateBlastSigId( TimetickClientCtxt * pCtxt, uint32 nSigId) 
{
    pCtxt->pTimetickDevCtxt->blast_sig_id = nSigId;
    return DAL_SUCCESS;
}/* END Timetick_UpdateBlastSigId */

/*=============================================================================

FUNCTION Timetick_CvtToUsec

DESCRIPTION See DDITimetick.h

=============================================================================*/
DALResult 
Timetick_CvtToUsec
( 
  TimetickClientCtxt    *pCtxt, 
  DalTimetickTime32Type time, 
  DalTimetickUnitType   unit, 
  DalTimetickTime64Type *pTimeRet
) 
{
  DalTimetickTime64Type   usecs;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  usecs = (DalTimetickTime64Type)time;

  switch ( unit )
  {
    case T_HOUR:
      /* Convert hours to minutes */
      usecs *= 60;
      /* fall-through */
    case T_MIN:
      /* Convert minutes to seconds */
      usecs *= 60;
      /* fall-through */
    case T_SEC:
      /* Seconds to slow clocks */
      usecs *= 1000;
    case T_MSEC:
      /* Milliseconds to slow clocks */
      usecs = usecs * 1000;
    case T_USEC:
      break;
    case T_TICK:
      usecs = ((uint64)time * 1000000) / (uint64)pCtxt->pTimetickDevCtxt->freq;
      break;

    default:
      ULOG_RT_PRINTF_1 (pCtxt->hTimeTickLog, 
                "Timetick_CvtToUsec: Invalid timetick conversion %d", unit);
      break;
  }

  *pTimeRet = usecs;

  return DAL_SUCCESS;
}/* Timetick_CvtToUsec*/

/*=============================================================================

FUNCTION Timetick_CvtFromUsec

DESCRIPTION See DDITimetick.h 
 
=============================================================================*/
DALResult 
Timetick_CvtFromUsec
( 
  TimetickClientCtxt    *pCtxt, 
  DalTimetickTime64Type time, 
  DalTimetickUnitType   unit, 
  DalTimetickTime32Type *pTimeRet
) 
{
  /* Factor for converting to hours or minutes */
  uint32                          scale = 1;

  /* Converted value in 32-bit integer */
  DalTimetickTime32Type           converted_time = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( unit )
  {
    case T_HOUR:
      /*  Scale minutes to hours */
      scale *= 60;
      /* fall-through */

    case T_MIN:
      /* Scale seconds to minutes */
      scale *= 60;
      /* fall-through */

    case T_SEC:
      /* Convert slow clocks to seconds (or minutes, or hours) */
      scale *= 1000;

    case T_MSEC:
      scale *= 1000;

    case T_USEC:
      converted_time = (DalTimetickTime32Type)(time/scale);
      break;

    case T_TICK:
      converted_time = (time * pCtxt->pTimetickDevCtxt->freq) / 1000000;
      break;

    default:
      ULOG_RT_PRINTF_1 (pCtxt->hTimeTickLog, 
                "Timetick_CvtFromUsec: Invalid timetick conversion %d", unit);
      break;

  }

  *pTimeRet = converted_time;

  return DAL_SUCCESS;

} /* Timetick_CvtFromUsec */



/*==========================================================================

  FUNCTION      Timetick_GetMatchValue

  DESCRIPTION   See DDITimetick.h

==========================================================================*/
DALResult Timetick_GetMatchValue
(
  TimetickClientCtxt          *pCtxt,
  DalTimetickTime32Type       *pTick
)
{

  *pTick = pCtxt->pTimetickDevCtxt->last_match_value;
  return DAL_SUCCESS;

} /* END Timetick_GetMatchValue */

/*==========================================================================

  FUNCTION      Timetick_TicksToPrecUs

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult 
Timetick_TicksToPrecUs
(
  TimetickClientCtxt    *pCtxt,
  DalTimetickTime32Type nTicks,
  uint64                *pus
)
{
  *pus = ((uint64)nTicks * 1000000) / (uint64)pCtxt->pTimetickDevCtxt->freq;

  return DAL_SUCCESS;
} /* END Timetick_TicksToPrecUs */

/*==========================================================================

  FUNCTION      Timetick_SetNextInterrupt64

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult 
Timetick_SetNextInterrupt64
(
  TimetickClientCtxt    *pCtxt,
  DalTimetickTime64Type  matchCount,
  DalTimetickTime64Type  ticksNow,
  DalTimetickTime64Type *matchVal
)
{
  uint32 matchValLow,matchValHi;


  matchValLow = matchCount & 0xFFFFFFFF;
  matchValHi = matchCount >> 32;

  /* Start critical section */
  qurt_mutex_lock(pCtxt->pTimetickDevCtxt->mSyncObj);

  HWIO_QTMR_CNT_CVAL_HI_OUT(
    (pCtxt->pTimetickDevCtxt->QTimerBase + pCtxt->pTimetickDevCtxt->QTimerFrameCVALVirtualOffset), matchValHi);

  HWIO_QTMR_CNT_CVAL_LO_OUT((pCtxt->pTimetickDevCtxt->QTimerBase + pCtxt->pTimetickDevCtxt->QTimerFrameCVALVirtualOffset),
                              matchValLow);

  /* End critical section */
  qurt_mutex_unlock(pCtxt->pTimetickDevCtxt->mSyncObj);
  
  *matchVal = matchCount;

  return DAL_SUCCESS;
} /* END Timetick_SetNextInterrupt64 */


/*==========================================================================

  FUNCTION      Timetick_GetMatchValue64

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult 
Timetick_GetMatchValue64
(
  TimetickClientCtxt    *pCtxt,
  DalTimetickTime64Type *pMatchVal
)
{

  DalTimetickTime32Type nCountCvalLow,nCountCvalHi;

  if (pCtxt->pTimetickDevCtxt->QTimerBase != NULL )
  {

    nCountCvalHi = HWIO_QTMR_CNT_CVAL_HI_IN(
                                  (pCtxt->pTimetickDevCtxt->QTimerBase + pCtxt->pTimetickDevCtxt->QTimerFrameCVALVirtualOffset));
    nCountCvalLow = HWIO_QTMR_CNT_CVAL_LO_IN(
                                  (pCtxt->pTimetickDevCtxt->QTimerBase + pCtxt->pTimetickDevCtxt->QTimerFrameCVALVirtualOffset));

    *pMatchVal = (((uint64)nCountCvalHi <<32)| nCountCvalLow);
  }
  return DAL_SUCCESS;
} /* END Timetick_GetMatchValue64 */  

/*=============================================================================

FUNCTION Timetick_GetInterruptVectorNum

DESCRIPTION   See DDITimetick.h

=============================================================================*/

DALResult Timetick_GetInterruptVectorNum
(
  TimetickClientCtxt *pCtxt,
  uint32 *num
)
{
  *num = pCtxt->pTimetickDevCtxt->QTimerIntVectNum;

  return DAL_SUCCESS;
 }/* END Timetick_GetInterruptVectorNum */

