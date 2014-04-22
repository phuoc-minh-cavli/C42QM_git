#ifndef DSIHOSTSYSTEM_H
#define DSIHOSTSYSTEM_H
/*=============================================================================

  File: DSIHostSystem.h
  

     Copyright (c) 2010-2016, 2018 Qualcomm Technologies, Inc.
     All Rights Reserved.
     Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "dsiHost.h"
#include "MDPSystem.h"

/* --- DO NOT ADD HEADER FILES HERE! --- */

/* -----------------------------------------------------------------------
** Macros and Defines
-------------------------------------------------------------------------- */

/*
   HWIO_SET_FLD: set a given field in a register
   io = name of register
   field = field inside register
   fldVal = value to set in the given field
*/
#define HWIO_SET_FLD(io, field, fldVal)             ((fldVal << HWIO_MMSS_##io##_##field##_SHFT) & HWIO_MMSS_##io##_##field##_BMSK)


/* -----------------------------------------------------------------------
** Enumerations
-------------------------------------------------------------------------- */


/*!
 *\b DSI_LogLevel
 *
 * DSI Log level enumerations
 * 
 */
typedef enum 
{
  DSI_LOGLEVEL_ERROR,
  DSI_LOGLEVEL_WARNING,
  DSI_LOGLEVEL_INFO,
} DSI_LogLevel;



/* -----------------------------------------------------------------------
** Functionas
-------------------------------------------------------------------------- */

/* Clear memory
*/
void DSI_OSAL_MemZero(void *pBuffer, uint32 uSizeInBytes);

/* Copy memory
*/
void DSI_OSAL_MemCpy(void *pDst, void *pSrc, uint32 uSizeInBytes);

/* Sleep/halt processor for amount of time in miliseconds
*/
void DSI_OSAL_SleepMs(uint32 ms);

/* Sleep/halt processor for amount of time in miliseconds
*/
void DSI_OSAL_SleepUs(uint32 us);


/* Debug/log messages
*/
#define DSI_Log_Message(level, fmt, ...)                                  MDP_Log_Message(MDP_LOGLEVEL_WARN, fmt, ##__VA_ARGS__) 


/* Memory barrier macro, create a CPU memory barrier to ensure all commands prior to this are completed */
#define DSI_OSAL_MB                                                       MDP_OSAL_MB



/* -----------------------------------------------------------------------
**
** FUNCTION: dsiSystem_ConfigDevicePll()
**
** DESCRIPTION:
**   TBD
**
** INPUT:
**   TBD
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiSystem_ConfigDevicePll(
    DSI_HandleType          hDevice,
    DSI_DisplayConfigType  *psDisplayConfig);

//-------------------------------------------------------------------------------------------------
//  dsiSystem_DisableClock
//
//  @brief
//      Vote for DSI clock off.
//
//  @params
//      [IN] hDevice
//          DSI device handle.
//
//  @return
//      None.
//-------------------------------------------------------------------------------------------------
//
DSI_StatusType dsiSystem_DisableClock(DSI_HandleType          hDevice);

#ifdef __cplusplus
}
#endif

#endif  /* #define DSIHOSTSYSTEM_H */
