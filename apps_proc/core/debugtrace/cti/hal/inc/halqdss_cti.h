#ifndef HALQDSS_CTI_H
#define HALQDSS_CTI_H

/****************************************************************************
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 *
 * FILE:            core/debugtrace/cti/hal/inc/halqdss_cti.h
 *
 * DESCRIPTION:     Public HAL interface for CTI.
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

#include "HALcomdef.h"

/****************************************************************************
 * Public Function Declarations
 ****************************************************************************/

/*
 * There is more than 1 CTI block, so we need to initialize the base
 * address of the specified CTI block. All CTI registers are accessed
 * as offsets to the base address.
 */
void HAL_qdss_cti_SetBaseAddress(uint32 base_addr);


/*
 * All below functions will operate on the current CTI block address and thus
 * assume that HAL_qdss_cti_SetBaseAddress() has been called.
 */

/* Unlock write access to CTI registers */
void HAL_qdss_cti_UnlockAccess(void);

/* Lock write access to CTI registers. */
void HAL_qdss_cti_LockAccess(void);

/* Enable CTI mapping logic. */
void HAL_qdss_cti_Enable(void);

/* Disable CTI mapping logic. */
void HAL_qdss_cti_Disable(void);

/*
 * Map/Unmap an input trigger to a channel.
 * 
 * @param enable    Map or unmap. A value of 1 indicates 'map'.
 * @param trigger   Trigger number.
 * @param channel   Channel number.
 */
void HAL_qdss_cti_Set_CTIINEN(uint8 enable, uint8 trigger, uint8 channel);

/*
 * Map/Unmap an output trigger to a channel.
 * 
 * @param enable    Map or unmap. A value of 1 indicates 'map'.
 * @param trigger   Trigger number.
 * @param channel   Channel number.
 */
void HAL_qdss_cti_Set_CTIOUTEN(uint8 enable, uint8 trigger, uint8 channel);

/*
 * Reads the current channel mappings for the given input trigger.
 *
 * @param trigger   Trigger number.
 * 
 * @return          Bitmap of enabled channels for this trigger. 
 */
uint32 HAL_qdss_cti_Get_CTIINEN(uint8 trigger);

/*
 * Reads the current channel mappings for the given output trigger.
 *
 * @param trigger   Trigger number.
 * 
 * @return          Bitmap of enabled channels for this trigger. 
 */
uint32 HAL_qdss_cti_Get_CTIOUTEN(uint8 trigger);

/* 
 * Reset all channel mappings for the given input trigger.
 *
 * @param trigger   Trigger number.
 */
void HAL_qdss_cti_Reset_CTIINEN(uint8 trigger);

/*
 * Reset all channel mappings for the given output trigger.
 *
 * @param trigger   Trigger number.
 */ 
void HAL_qdss_cti_Reset_CTIOUTEN(uint8 trigger);


void HAL_qdss_cti_AppSet(uint8 channel);
void HAL_qdss_cti_AppClear(uint8 channel);
void HAL_qdss_cti_AppPulse(uint8 channel);

#endif /* HALQDSS_CTI_H */
