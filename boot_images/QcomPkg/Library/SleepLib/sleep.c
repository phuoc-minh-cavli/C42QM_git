/*===========================================================================

  Copyright (c) 2018, 2019 by Qualcomm Technologies, Inc. All Rights Reserved.

  ===========================================================================*/

#include "sleep.h"

/* In SBL only WFI is the only sleep mode supported  */
void power_wfi(void)
{
	__asm("wfi"); 
}

