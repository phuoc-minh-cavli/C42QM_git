/*==============================================================================
  FILE:         rsc.c

  OVERVIEW:     This file implements some of the APIs defined in rsc public 
                header file to interact with RSC SEQUENCER HW block(s).

  DEPENDENCIES: None
  
                Copyright (c) 2016-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/rsc/common/rsc.c#1 $
$DateTime: 2018/12/28 00:36:13 $
==============================================================================*/
#include "comdef.h"
#include "rsc.h"
#include "rsc_seq_hal.h"
#include "rsc_hal_bsp.h"
#include "rsc_internal.h"
#include "CoreVerify.h"
#include "rsc_hwio.h"

/*=======================================================================
 *                             LOCAL FUNCTIONS
 *======================================================================*/
/**
 * rsc_instance_init
 *
 * @brief Populate RSC instance parameters for respective RSC instance i.e apss.
 *
 * @return RSC error code (@see rsc_result_t)
 */
static rsc_result_t rsc_instance_init(void)
{
  uint32        nInst;
  rsc_result_t  result = RSC_SUCCESS;

  /* Target specific init call to initialize RSC instances */
  rsc_target_init();

  /* Copy commands to the sequencer memory */
  result = rsc_setup();
  
  for(nInst=0; nInst < g_rscCount; nInst++)
  {
    RSC_MUTEX_CREATE(nInst);
  }

  return result;
}

/*=======================================================================
 *                             GLOBAL FUNCTIONS
 *======================================================================*/
/*
 * rsc_set_low_power_mode
 */
rsc_result_t rsc_set_low_power_mode(rsc_type type, rsc_low_power_mode_type modeId)
{
  rsc_modes_t *mode;
  uint32      modeIndex;
  uint32      base;

  if(!RSC_VALID_TYPE(type))
  {
    return RSC_INVALID_HANDLE;
  }

  base = g_rscs[type].addr;

  if(RSC_SUCCESS != rsc_mode_index(type, modeId, &modeIndex))
  {
    return RSC_INVALID_PARAM;
  }

  mode = &g_rscs[type].modes[modeIndex];

  /* Set the valid bit to make s/w writtem ADDR field to be
   * used instead of using HW solver provided address. */
  rsc_hal_set_seq_override_start_addr_valid(base, 0x1);
  
  /* Write the address of the lpm mode to be kicked */
  rsc_hal_set_seq_override_start_addr(base, mode->start_addr);

  return RSC_SUCCESS;
}

/**
 * rsc_driver_init
 *
 * @brief Populate available RSC instances. This information is useful for the
 *        clients to initialize respective subsystem RSC.
 *
 * @note This function must be called before calling any other RSC function.
 */
void rsc_driver_init(void)
{
  /* Target specific init call to initialize RSC instances */
  rsc_instance_init();
  return;
}

