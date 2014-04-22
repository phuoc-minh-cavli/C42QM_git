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
  {300000},
  {600000},
  {748800},
  {998400},
  {1190400}
};

/* The lookup tables of requested mips and the corresponding 
   CPU frequency(In KHz)  and Bus AXI Frequency.(In MBps) 
*/

/* Chip Version 1 */
DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE primaryClockPlanDataV1[]=
{
  {300,  300000,  125*8},
  {384,  384000,  125*8},
  {600,  600000,  250*8},
  {749,  748800,  333*8},
  {999,  998400,  333*8},
  {1190, 1190400, 333*8}
};

/* Chip Version 2 */
DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE primaryClockPlanDataV2[]=
{
  {300, 300000, 133*8},
  {384, 384000, 133*8},
  {600, 600000, 266*8},
  {749, 748800, 333*8},
  {999,  998400,  333*8},
  {1190, 1190400, 333*8}
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
  primaryClockPlanDataV1,
  /* The number of performance levels int he clock plan for this target. */
  sizeof(primaryClockPlanDataV1)/sizeof(DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE),
  /* The 9x25 target specific frequency conversion functions. */
  NULL,
  NULL,
  NULL,
  19200000    /* This is the freq. of the QTimer source (Chip XO clock) */
};


DCVS_TARGET_CLOCK_PLAN_INFO_TYPE* DCVSTargetClockPlanInfoGet(void)
{
  DalChipInfoVersionType chipVersion;
  CORE_VERIFY(chipVersion = DalChipInfo_ChipVersion());

  if (chipVersion >= 0x00020000)
  {
    dcvsTargetClockPlan.clockPlanTable = primaryClockPlanDataV2;
  }
  /* Return the local clock plan info of the target. */
  return(&dcvsTargetClockPlan);
}

