/****************************************************************************
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 *
 * FILE:            core/debugtrace/cti/hal/src/halqdss_cti.c
 *
 * DESCRIPTION:     Implementation for CTI HAL functions.
 *
 *
 * EDIT HISTORY FOR MODULE
 * --------------------------------------------------------------------------
 *
 * when         who         what, where, why
 * ----------   -------     -----------------
 * 2014-01-24   rpoddar     Initial revision.
 *
 ****************************************************************************/

/****************************************************************************
 * Include Files
 ****************************************************************************/

#include "halqdss_cti.h"
#include "hwioqdss_cti.h"

/****************************************************************************
 * Data & Static Function Declarations
 ****************************************************************************/

volatile uint32 QDSS_CTI_BLOCK_BASE;

/*
 * Helper function for CTIINEN and CTIOUTEN registers because the
 * programmng logic for both are the same.
 */
static void HAL_qdss_Set_CTIEN(uint8 enable, uint32 addr, uint8 channel);

/****************************************************************************
 * Function Definitions 
 ****************************************************************************/

void HAL_qdss_cti_SetBaseAddress(uint32 base_addr)
{
    QDSS_CTI_BLOCK_BASE = base_addr;
}

void HAL_qdss_cti_UnlockAccess(void)
{
    HWIO_OUT(QDSS_CTI_LAR, 0xC5ACCE55);
}

void HAL_qdss_cti_LockAccess(void)
{
    HWIO_OUT(QDSS_CTI_LAR, ~0xC5ACCE55);
}

void HAL_qdss_cti_Enable(void)
{
    HWIO_OUT(QDSS_CTI_CONTROL, 0x1);
}

void HAL_qdss_cti_Disable(void)
{
    HWIO_OUT(QDSS_CTI_CONTROL, 0x0);
}

void HAL_qdss_cti_Set_CTIINEN(uint8 enable, uint8 trigger, uint8 channel)
{
    uint32 addr = HWIO_QDSS_CTI_INENn_ADDR(trigger);
    HAL_qdss_Set_CTIEN(enable, addr, channel);
}

void HAL_qdss_cti_Set_CTIOUTEN(uint8 enable, uint8 trigger, uint8 channel)
{
    uint32 addr = HWIO_QDSS_CTI_OUTENn_ADDR(trigger);
    HAL_qdss_Set_CTIEN(enable, addr, channel);
}

uint32 HAL_qdss_cti_Get_CTIINEN(uint8 trigger)
{
    return HWIO_INI(QDSS_CTI_INENn, trigger);
}

uint32 HAL_qdss_cti_Get_CTIOUTEN(uint8 trigger)
{
    return HWIO_INI(QDSS_CTI_OUTENn, trigger);
}

void HAL_qdss_cti_Reset_CTIINEN(uint8 trigger)
{
    HWIO_OUTI(QDSS_CTI_INENn, trigger, 0x0);
}
void HAL_qdss_cti_Reset_CTIOUTEN(uint8 trigger)
{
    HWIO_OUTI(QDSS_CTI_OUTENn, trigger, 0x0);
}

void HAL_qdss_cti_AppSet(uint8 channel)
{
   uint32 mask = 1;
   uint32 val = HWIO_IN(QDSS_CTI_APPSET);
   mask <<= channel;
   HWIO_OUT(QDSS_CTI_APPSET, val|mask);
}

void HAL_qdss_cti_AppClear(uint8 channel)
{
   uint32 mask = 1;
   uint32 val = HWIO_IN(QDSS_CTI_APPCLEAR);
   mask <<= channel;
   HWIO_OUT(QDSS_CTI_APPCLEAR, val|mask);
}

void HAL_qdss_cti_AppPulse(uint8 channel)
{
   uint32 mask = 1;
   uint32 val = HWIO_IN(QDSS_CTI_APPPULSE);
   mask <<= channel;
   HWIO_OUT(QDSS_CTI_APPPULSE, val|mask);
}


static void HAL_qdss_Set_CTIEN(uint8 enable, uint32 addr, uint8 channel)
{
    uint32 val;
    uint32 mask = 1;

    val = in_dword(addr);
    mask <<= channel;

    if (enable) {
        out_dword(addr, val | mask);
    } else {
        out_dword(addr, val & ~mask);
    }
}
