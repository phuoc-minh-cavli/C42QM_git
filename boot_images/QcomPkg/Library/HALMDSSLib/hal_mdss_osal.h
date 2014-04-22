/*! \file */

/*
===========================================================================

FILE:         hal_mdss_osal.h

DESCRIPTION:  HAL MDSS OS Abstraction Layer module
  

===========================================================================
Copyright (c) 2012- 2013 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/
#ifndef _HAL_MDSS_OSAL_H
#define _HAL_MDSS_OSAL_H

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "hal_mdss.h"
/* !! ALERT !!
 * DO NOT INCLUDE ANY HEADER FILES OTHER THAN hal_mdss.h
 */

/* -----------------------------------------------------------------------
** Preprocessor Defintions and Constants
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Enumerations and Data structures
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------- */

/****************************************************************************
*
** FUNCTION: HAL_MDSS_OSAL_SleepMs()
*/
/*!
* \brief
*     Sleep in milliseconds
*
* \param [in] uMilliSeconds           - Time in milliseconds to be waited
*
* \retval NONE
*
****************************************************************************/
void HAL_MDSS_OSAL_SleepMs(uint32 uMilliSeconds);

/****************************************************************************
*
** FUNCTION: HAL_MDSS_OSAL_MemSet()
*/
/*!
* \brief
*   The \b HAL_MDSS_OSAL_MemSet function fills the buffer with the uFillValue
*
* \param [in]   pBuffer           - Input buffer pointer
* \param [in]   uFillValue        - Value to be filled into the buffer
* \param [in]   uSizeInBytes      - Buffer size to be filled
*
* \return MDSS_Status
*
****************************************************************************/
void HAL_MDSS_OSAL_MemSet(char *pBuffer, uint8 uFillValue, uint32 uSizeInBytes);

#endif // _HAL_MDSS_OSAL_H
