#ifndef BUILDFEATURES_H
#define BUILDFEATURES_H
/*==============================================================================
  @file BuildFeatures.h

  ULog Features. Customize this file for codebases that need less or specific
  options enabled.

  Copyright (c) 2011-2012, 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/core.mpss/3.10/services/diag/diagbuffer/config/ULogBuildFeatures.h#3 $ */
/* when       who     what, where, why
 --------   ---     ---------------------------------------------------------
 02/20/19   kdey    Added compilation flag for page-only image */


/******************************************************************************
Default ULog uses Realtime and Raw, Tees, Listing, Allocation, Read and Parse support.
******************************************************************************/
#define ULOG_PKG_FULL


/******************************************************************************
32bit or 64 bit timestamps.
******************************************************************************/
#define ULOG_64BIT_TIME


/******************************************************************************
QDSS support for the QDSS Trace API
******************************************************************************/
//#define DEFEATURE_ULOG_STM

#if defined FEATURE_BUILD_PAGEONLY_IMAGE || defined FEATURE_BUILD_SENSORONLY_IMAGE 
  #define DEFEATURE_ULOG_STM
#endif /* FEATURE_BUILD_PAGEONLY_IMAGE */
/******************************************************************************
Diag backend support for listing, reading and changing transport.
******************************************************************************/
//#define DEFEATURE_ULOG_DIAG
// Set the processor ID for the ULog Diag Subsystem
#define ULOGDIAG_PROCESSOR_ID  ULOGDIAG_PROCESSOR_MODEM


#if defined(ULOG_PKG_ALTERNATE)
// This package means that all standard source files will be compiled out.
// The selection of the alternate implementation is outside this file.  This 
// section MUST list all features.
// 
#define DEFEATURE_ULOG_CORE
#define DEFEATURE_ULOG_MSG_FORMAT
#define DEFEATURE_ULOG_TEE
#define DEFEATURE_ULOG_READ
#define DEFEATURE_ULOG_LIST
#define DEFEATURE_ULOG_ALLOCATE
#define DEFEATURE_ULOG_FRONT_REALTIME
#define DEFEATURE_ULOG_FRONT_RAW


#elif defined(ULOG_PKG_FULL)
// This package defines the maximum set of functionality.  It is 
// used by default, and provides the largest set of log retrieval
// methods.

#elif defined (ULOG_PKG_SMALL)
// This package defines the minimum set of functionality.  It is 
// used on images with extreme memory requirements, and no need to 
// retrieve the log through means other than the debugger.
//
#define DEFEATURE_ULOG_MSG_FORMAT
#define DEFEATURE_ULOG_TEE
#define DEFEATURE_ULOG_READ
#define DEFEATURE_ULOG_LIST
#define DEFEATURE_ULOG_ALLOCATE

#endif


#endif // BUILDFEATURES_H





