#ifndef RR_THERMAL_H
#define RR_THERMAL_H

/** @file */

/*============================================================================
  @file rr_thermal.h

  @brief This is the header file for rr_thermal.c
         This module contains the functionality for thermal mitigation procedures.

                Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_thermal.h#3 $ */
/* $DateTime: 2018/10/16 07:03:13 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_RR_THERMAL_MITIGATION
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief Updates MM with new paramters as required
 
  @param can_send_service_ind Set to TRUE if a RR_SERVICE_IND can be sent to MM
*/ 
extern void rr_thermal_update(boolean can_send_service_ind  );

/**
  @brief Indicates if thermal mitigation is blocking PS access or not 
   
  @return TRUE is PS is allowed; FALSE otherwise 
*/ 
extern boolean rr_thermal_ps_allowed( );

/**
  @brief Called when NV data becomes ready
*/ 
extern void rr_thermal_nv_ready(void);

/**
  @brief Initialisation function. Should be called once at RR task start
*/ 
extern void rr_thermal_task_start_init(void);
#endif /* FEATURE_RR_THERMAL_MITIGATION */

#endif /* #ifndef RR_THERMAL_H */

