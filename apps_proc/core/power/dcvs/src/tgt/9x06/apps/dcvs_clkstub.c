/**********************************************************************
 * dcvs_clkstub.c
 *
 * Copyright (C) 2011-2013 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 **********************************************************************/

#include "dcvs_target.h"
#include "dcvs_core.h"
#include "DDIChipInfo.h"
#include "CoreVerify.h"

DCVS_LOCAL_CLOCK_PLAN_DATA_TYPE localClockPlanData[]=
{ 
  {400000},
  {800000},
  {998400},
  {1094400},
  {1190400},
  {1248000},
  {1305600},
};

/* The lookup tables of requested mips and the corresponding 
   CPU frequency(In KHz)  and Bus AXI Frequency.(In MBps) 
*/

/* Chip Version 1 */
DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE primaryClockPlanData[]=
{
  {400,   400000,   120*8},
  {800,   800000,   240*8},
  {998,   998400,   300*8},
  {1094,  1094400,  300*8},
  {1190, 1190400,   300*8},
  {1248,  1248000,  300*8},
  {1305,  1305600,  300*8}  
};

DCVS_LOCAL_CLOCK_PLAN_DATA_TYPE* DCVSTargetClockPlanLocalGet(void)
{
  /* Return the local clock plan table */
  return(localClockPlanData);
}

uint32 DCVSTargetClockPlanLocalNumLevelsGet(void)
{
  /* Return the number of entries in the local clock plan table. */
  return(sizeof(localClockPlanData)/sizeof(DCVS_LOCAL_CLOCK_PLAN_DATA_TYPE));
}

DCVS_TARGET_CLOCK_PLAN_INFO_TYPE dcvsTargetClockPlan = 
{
  /* The clock plan table having mappings of mips and processor frequency.*/
  primaryClockPlanData,
  /* The number of performance levels int he clock plan for this target. */
  sizeof(primaryClockPlanData)/sizeof(DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE),
  NULL,
  NULL,
  NULL,
  19200000    /* This is the freq. of the QTimer source (Chip XO clock) */
};


DCVS_TARGET_CLOCK_PLAN_INFO_TYPE* DCVSTargetClockPlanInfoGet(void)
{
  /* Return the local clock plan info of the target. */
  return(&dcvsTargetClockPlan);
}

