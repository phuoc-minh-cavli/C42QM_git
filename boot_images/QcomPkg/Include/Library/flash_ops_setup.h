#ifndef FLASH_OPS_SETUP_H
#define FLASH_OPS_SETUP_H

/**********************************************************************
 * flash_ops_setup.h
 *
 * SERVICES:    Header file for flash operation setup used by EFS
 *
 * DESCRIPTION: Header file for flash operation setup used by EFS
 * 
 * Copyright (c) 2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Include/Library/flash_ops_setup.h#1 $ $DateTime: 2018/08/01 23:51:05 $ $Author: pwbldsvc $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
03/08/11     sv      Initial Revision
===========================================================================*/

/* Flash Operation Init */
void flash_ops_init(void);

/* Flash Operations Deinit */
void flash_ops_deinit(void);    

/* Flash Operations start */
void flash_ops_start(void);

/* Flash Operations end*/
void flash_ops_end(void);

#endif /* FLASH_OPS_SETUP_H */
