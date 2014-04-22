#ifndef __VCSSWEVT_H__
#define __VCSSWEVT_H__
/*
===========================================================================
*/
/**
  @file VCSSWEVT.h

  This header provides a variety of preprocessor definitions, type, data, and
  function declarations for QDSS event logging.

  This file provides an abstraction layer for QDSS tracer features.

*/
/*
  ====================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/core.tx/6.0/systemdrivers/vcs/src/VCSSWEVT.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


#ifdef VCS_TRACER_SWEVT
#include "tracer.h"
#include "VCSSWEventId.h"
#endif


/*=========================================================================
      Macros
==========================================================================*/


#ifdef VCS_TRACER_SWEVT
#define VCS_SWEvent(...)  \
  tracer_event_simple_vargs(__VA_ARGS__);
#else
#define VCS_SWEvent(...)
#endif


#endif // __VCSSWEVT_H__

