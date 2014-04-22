/*!
  @file
  perf_metric.h

  @brief
  This module contains interface to get perf metrics.

*/

/*=============================================================================

  Copyright (c) 2018 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly 
  reserved. By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. 
 
$Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/19/18   jk      Inital Checkin

=============================================================================*/
#ifndef PERF_METRIC_H
#define PERF_METRIC_H

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

/*=============================================================================

                              MACROS

=============================================================================*/

/*=============================================================================

                         DEFINITIONS AND TYPES

=============================================================================*/

/*! @brief Perf Metric Event Counter */
typedef struct
{
  /*! @brief Pcycles event counter */
  uint32        pcycles[4];

  /*! @brief packets event counter */
  uint32        packets;

  /*! @brief BLC latency event counter */
  uint32        blc_latcnt;

  /*! @brief BLC transaction event counter */
  uint32        blc_txncnt;

  /*! @brief q6 frequency*/
  uint32       q6_freq;

  /*! @brief ddr frequency*/
  uint32       ddr_freq;
}perf_metric_event_counter_s;

/*! @brief Perf Metric Computed Metric */
typedef struct
{
  /*! @brief MPPS computed value */
  uint32 mpps;

  /*! @brief CPP computed value */
  uint32 cpp;

  /*! @brief BLC computed value */
  uint32 blc;

}perf_metric_comp_metric_s;

/*=============================================================================

                           EXTERN VARIABLES

=============================================================================*/

/*===========================================================================

                           FUNCTION PROTOTYPES

=============================================================================*/

/*=============================================================================

  FUNCTION:  perf_metric_init

=============================================================================*/
/*!
    @brief
    Initalize PERF metric and configure registers/counters if needed.

    @detail

    @return
    None
 
    @note

    @see related_function()

*/
/*===========================================================================*/
void perf_metric_init
(
   void
);

/*=============================================================================

  FUNCTION:  perf_metric_update

=============================================================================*/
/*!
    @brief
	Fetch the perf metric from the registers/counters.

    @detail

    @return
    None

    @note

    @see related_function()

*/
/*===========================================================================*/
void perf_metric_update
(
   perf_metric_event_counter_s   *evt
);

/*=============================================================================

  FUNCTION:  perf_metric_compute

=============================================================================*/
/*!
    @brief
	Compute the perf metrics from 2 raw counter/register values.

    @detail

    @return
    None

    @note

    @see related_function()

*/
/*===========================================================================*/
void perf_metric_compute
(
  perf_metric_event_counter_s   *evt1,
  perf_metric_event_counter_s   *evt2,
  perf_metric_comp_metric_s     *res,
  uint32 time_in_ms
);

/*=============================================================================

  FUNCTION:  perf_metric_destroy

=============================================================================*/
/*!
    @brief
    Destroy PERF metric and and de-configure registers/counters if needed.

    @detail

    @return
    None
 
    @note

    @see related_function()

*/
/*===========================================================================*/
void perf_metric_destroy
(
   void
);

#endif /* PERF_METRIC_H */
