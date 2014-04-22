/*============================================================================
  FILE:         vdd_dig_lpr.c

  OVERVIEW:     This file provides the LPR definition for the VDD_DIG clock 
                low-power modes.

  DEPENDENCIES: 

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/

#include "stdint.h"
#include "rpm.h"
#include "rpmclient.h"
#include "kvp.h"
/* #include "pm_npa_device_smps.h" */

static kvp_t* p_vdd_kvp;
static uint32_t vdd_req;
void vdd_dig_init( void )
{
  /* 2 resources * 3 fields (set,key,value) * 4 bytes per field */
  p_vdd_kvp = kvp_create(2*4*3);
  vdd_req = 1;
  kvp_put(p_vdd_kvp, /* PM_NPA_KEY_SOFTWARE_ENABLE */ 0x6E657773,
          sizeof(vdd_req), (void *)&vdd_req);
  kvp_put(p_vdd_kvp, /* PM_NPA_KEY_CORNER_LEVEL_KEY */ 0x6E726F63,
          sizeof(vdd_req), (void *)&vdd_req);

  /* Preallocating memory for the resources kvp we vote during sleep */
  rpm_preallocate_resource_kvp( RPM_LDO_A_REQ,    /* resource type */
                                10,               /* resource id */
                                0x6E726F63,       /* key */
                                sizeof(vdd_req)   /* expected length */ );
  
  rpm_preallocate_resource_kvp( RPM_LDO_A_REQ,    /* resource type */
                                12,               /* resource id */
                                0x6E657773,       /* key */
                                sizeof(vdd_req)   /* expected length */ );

  return;
}

void vdd_dig_min_enter( uint64_t wakeup_tick )
{
//  kvp_reset(p_vdd_kvp);
//  rpm_post_request(RPM_SLEEP_SET, RPM_LDO_A_REQ, /*vddcx == */ 10, p_vdd_kvp);
//  kvp_reset(p_vdd_kvp);
//  rpm_post_request(RPM_SLEEP_SET, RPM_LDO_A_REQ, /*vddmx == */ 12, p_vdd_kvp);
  return;
}

void vdd_dig_min_exit( void )
{
  return;
}
