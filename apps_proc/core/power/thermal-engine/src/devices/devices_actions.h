/*===========================================================================

  Copyright (c) 2013, 2015 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

===========================================================================*/

#ifndef __DEVICE_ACTIONS_H__
#define __DEVICE_ACTIONS_H__

#include "thermal.h"

void vdd_rstr_init(void);
int vdd_restriction_request(int request);

void mx_min_request_init(void);
int mx_min_request(int request);

void cx_min_request_init(void);
int cx_min_request(int request);

#ifdef CPR_COLD_REQUEST
void cpr_cold_request_init(void);
int cpr_cold_request(int request);
#endif

void cpu_request_init(void);
int cpu_request(int request);

#endif  /* __DEVICE_ACTIONS_H__ */
