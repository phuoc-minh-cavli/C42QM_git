
/*
===========================================================================

FILE:   SpiULog.c

DESCRIPTION:
    This file contains functions for SPI logging  driver
 
===========================================================================

        Edit History



When     Who    What, where, why
-------- ---    -----------------------------------------------------------
06/15/16 ska  Ported for TN
09/11/13 sg     Updated common logging mechanism
11/02/11 sg     Created

===========================================================================
        Copyright c 2011 - 2016 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "SpiLog.h"

/*  Handle to the log */
ULogHandle gspiDynLogHandle = NULL;              

static uint32 spiDynLogBufSize;  

/* There are there log levels.
 * Refer SpiLog.h for details.
 * Default it level SPI_LOG_LEVEL_INFO, 
 * but during debugging the levels can be 
 * increased through JTAG.
 */
static uint32 uSpiLogLevel = SPI_LOG_LEVEL_ERROR; 

static boolean gInit_done = FALSE;


// Initialization 
void  SpiLog_Init(void)
{
   if (gInit_done)
   {
      return ;
   }

   gInit_done = TRUE;
   
   /* Size of Spi Log - Defaults to 2048 so that logging is enabled */
   spiDynLogBufSize = 1024;

   ULogFront_RealTimeInit(     &gspiDynLogHandle,      /* Log Handle*/
                  "SPI_Log",         /* Name of the log */
                  spiDynLogBufSize,  /* Initial Buffer. Normally
                                         set to 0 for a disabled log. */
                  ULOG_MEMORY_LOCAL, /* Local memory (normal case) */
                  ULOG_LOCK_OS);     // An OS lock is provided.  The log can
                                     // be written from more than one thread.
                                     // OS locks are assumed to work across
                                     // cores in the case where one OS is
                                     // managing all cores.

}

uint32 SpiLog_GetLogLevel(void)
{
   return(uSpiLogLevel);
}

void* SpiLog_GetUlogHandle(void)
{
   return(gspiDynLogHandle);
}

void SpiLog_SetLogLevel(uint32 uLogLevel)
{
	uSpiLogLevel = (uLogLevel > SPI_LOG_LEVEL_MAX) ? 
									SPI_LOG_LEVEL_MAX : uLogLevel;
}
