#ifndef __CLOCKSWEVT_H__
#define __CLOCKSWEVT_H__
/*
===========================================================================
*/
/**
  @file ClockSWEVT.h

  This header provides a variety of preprocessor definitions, type, data, and
  function declarations for QDSS event logging.

  This file provides an abstraction layer for QDSS tracer features.

*/
/*
  ====================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR

  ====================================================================
  $Header: //components/rel/core.tx/6.0/systemdrivers/clock/src/ClockSWEVT.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  11/02/12   frv     Created.

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


#ifdef CLOCK_TRACER_SWEVT
#include "tracer.h"
#include "ClockSWEventId.h"
#endif


/*=========================================================================
      Macros
==========================================================================*/


#ifdef CLOCK_TRACER_SWEVT
#define Clock_SWEvent(...)  \
  tracer_event_simple_vargs(__VA_ARGS__);
#else
#define Clock_SWEvent(...)
#endif


#endif // __CLOCKSWEVT_H__

