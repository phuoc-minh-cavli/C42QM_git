#ifndef FLASH_FTL_OSAL_H
#define FLASH_FTL_OSAL_H

/**********************************************************************
 * flash_ftl_osal.h
 *
 * OS abstraction layer for Flash Translation layer (FTL).
 * This layer provides OS/Platform support to be used by FTL core APIs
 *
 *
 * Copyright (c) 2016 QUALCOMM Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *
 **********************************************************************/
/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 2016-11-11   svl     Review comments incorporated.
 * 2016-09-14   svl     FTL utility layer support added
 *=======================================================================*/

#include <flash_ftl_types.h>

/**
 * initialize osal layer
 *
 * @return FLASH_FTL_STATUS [OUT]
 *    FLASH_FTL_OK if success
 *    FLASH_FTL_FAIL if fail
 */
void
flash_ftl_osal_init(void);

/**
 * deinitialize osal layer
 *
 */
void
flash_ftl_osal_deinit(void);

/**
 * flash_ftl_osal_enter_critical_section - enter critical section
 *
 */
void
flash_ftl_osal_enter_critical_section(void);

/**
 * flash_ftl_osal_leave_critical_section - leave critical section
 *
 */
void
flash_ftl_osal_leave_critical_section(void);

#endif /* #ifndef FLASH_FTL_OSAL_H */
