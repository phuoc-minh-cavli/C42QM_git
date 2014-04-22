#ifndef CPU_MONITOR_H
#define CPU_MONITOR_H
/********************************************************************
* cpu_monitor.h
*
* This module defines the interface for the CPU Monitor.
*
*
* Copyright (C) 2010 - 2011 by Qualcomm Technologies, Inc. All Rights Reserved.
* All Rights Reserved.
*
**********************************************************************/
/*=======================================================================
                        Edit History
$Header: //components/rel/core.tx/6.0/power/dcvs/inc/cpu_monitor.h#1 $ 
$DateTime: 2020/01/30 22:49:35 $

when       who     what, where, why
--------   ----    ---------------------------------------------------
01/24/11   ss      Initial checkin.
========================================================================*/
#include "dcvs_target.h"
#include "max_stub.h"

/**
  @brief CpuMonitorInitialize
  
  This function is used to initialize the CPU monitor 
 
  @param context: The DCVS data context.

  @return: None.

*/
void CpuMonitorInitialize(DCVS_CONTEXT* context);


/**
  @brief CpuMonitorStart
  
  This function is used to start the CPU monitor  if enabled.
 
  @param context: The DCVS data context.

  @return: None.

*/
void CpuMonitorStart(DCVS_CONTEXT* context);


#endif // CPU_MONITOR_H

