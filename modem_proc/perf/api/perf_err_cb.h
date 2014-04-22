#ifndef __PERF_ERR_CB_H__
#define __PERF_ERR_CB_H__

/*===========================================================================
 * FILE:         perf_err_cb.h
 *
 * DESCRIPTION:  This file provides API to register pre_STM callbacks
 *
 * Copyright (c) 2015, 2016 Qualcomm Technologies Incorporated.
 ===========================================================================*/

/*==========================================================================

  EDIT HISTORY FOR MODULE

  $Header: 

  when       who     what, where, why
  --------   ---     --------------------------------------------------------
  10/26/15   rr      Initial revision
  06/22/16   rr      Included API to which delay registration of err callbacks
                     to pre_STM
  ===========================================================================*/

typedef void (*perf_err_cb_ptr_u32)(uint32);
typedef void (*perf_err_cb_ptr)(void);

int perf_err_crash_cb_reg_pre_STM(perf_err_cb_ptr_u32 cb);
int perf_err_crash_cb_dereg_pre_STM(perf_err_cb_ptr_u32 cb);

int perf_err_cb_register_in_pre_STM(perf_err_cb_ptr cb);
int perf_err_cb_dereg_in_pre_STM(perf_err_cb_ptr cb);

int perf_err_cb_register_in_pre_STM_u32(perf_err_cb_ptr_u32 cb);
int perf_err_cb_dereg_in_pre_STM_u32(perf_err_cb_ptr_u32 cb);

#endif
