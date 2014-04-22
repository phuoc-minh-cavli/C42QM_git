/*
===========================================================================

FILE:   I2cSysExt.c

DESCRIPTION:
    This file contains the implementation for the QUP OS Services 
    Interface: IQupI2cDevice 

===========================================================================

        Edit History

$Header: //components/rel/core.tx/6.0/buses/i2c/src/dev/I2cSysExtL4.c#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
09/02/16 PR     Removed unused code.
09/26/11 LK     Created

===========================================================================
        Copyright c 2011-2016 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "I2cSys.h"


/**
   @brief Allocate physical memory
 
   This function allocates physical memory
  
   @param[in] pVirtMem  Pointer to virtual memory.
   @param[in] uSize     Size of memory buffer.
 
   @return I2CSYS_RESULT_OK on success, error code on error
 */
uint32 I2CSYS_VirtToPhys
(
   uint8       *pVirtMem,
   uint32       uSize
)
{
    /* TODO - Not needed/used for ThreadX */
    return 0;
}



