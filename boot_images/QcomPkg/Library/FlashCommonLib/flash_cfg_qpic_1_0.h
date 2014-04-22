#ifndef FLASH_CFG_QPIC_1_0_H
#define FLASH_CFG_QPIC_1_0_H

/*=============================================================================
 *
 * FILE:      flash_cfg_qpic_1.0.h
 *
 * DESCRIPTION: This file contains defines for multipage and pagescope features.
 *
 *
 * Copyright (c) 2018 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/FlashCommonLib/flash_cfg_qpic_1_0.h#1 $ $DateTime: 2019/06/17 00:13:57 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ---------   ---     ----------------------------------------------
 * 10/25/18     sa      Initial Revision
 *==================================================================*/

/*===================================================================
 *                     Include Files
 ====================================================================*/

#include <HALcomdef.h>

#define NAND_BAM_STATUS_PIPE  3
#define NAND_BAM_CMD_PIPE   2
#define NAND_BAM_READ_PIPE  1
#define NAND_BAM_WRITE_PIPE 0

/* BAM DESCRIPTOR FIFO SIZE defines. 
 * Currently this is sufficient to support 2K and 4K page. Current usage for
 * 4K page is 20 for CMD DESC, 10 for READ DESC, 10 for Write DESC. The DESC 
 * FIFO size should hold atleast 1 descriptor more than the Max desc as per 
 * the BAM driver design. These values need to be updated if there is any 
 * change in the code which increases the max BAM descriptors. */
#define FLASH_BAM_STATUS_DESC_FIFO_SZ  (0)
#define FLASH_BAM_CMD_DESC_FIFO_SZ  (22 * 8)
#define FLASH_BAM_READ_DESC_FIFO_SZ  (12 * 8)
#define FLASH_BAM_WRITE_DESC_FIFO_SZ  (12 * 8)

/* Externs for pagescope and multipage global variable.
    Using thi variable we hould be able to diable/enable pagecope/multipage runtime.*/
extern boolean flash_qpic_multipage_enabled;
extern boolean flash_qpic_pagescope_enabled;

/* This does not apply for 1.0 qpic.
*/
#define NO_STATUS_BUFFER_ENABLED 0


/* Variable to define MULTI R/W support counts. 
    On qpic1.0 we only support reading 1 page at a time.*/
#define FLASH_MULTI_R_SUPPORT_COUNT 1
#define FLASH_MULTI_W_SUPPORT_COUNT 1

#endif

