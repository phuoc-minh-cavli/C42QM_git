#ifndef QAPI_TIMER_ID_H
#define QAPI_TIMER_ID_H 

/*
Copyright (c) 2017,2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*===========================================================================

                            Edit History

  $Header: //components/rel/core.tx/6.0/api/services/qapi_timer_id.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/24/17   abh      Changes to support TIMER QAPIs with DAM framework
===========================================================================*/

#include "qapi_txm_base.h" 

/* Timer TXM IDs start from TXM_QAPI_TIMERS_BASE */
enum
 {
    TXM_QAPI_TIMER_DEF	= TXM_QAPI_TIMERS_BASE,
    TXM_QAPI_TIMER_SET,
    TXM_QAPI_TIMER_STOP,
    TXM_QAPI_TIMER_UNDEF,
    TXM_QAPI_TIMER_GET_INFO,
    TXM_QAPI_TIMER_SLEEP,
    TXM_QAPI_TIME_GET,
    TXM_QAPI_TIMER_GET_CBINFO,
    TXM_QAPI_TIMER_GET_USER_HANDLE,
    TXM_QAPI_GET_TIMER_INACCURACY
 } txm_timer_ids;

#endif /* QAPI_TIMER_CODES_H */
