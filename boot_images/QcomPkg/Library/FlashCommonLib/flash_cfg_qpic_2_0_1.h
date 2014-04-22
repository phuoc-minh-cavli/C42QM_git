#ifndef FLASH_CFG_QPIC_2_0_1_H
#define FLASH_CFG_QPIC_2_0_1_H  //{

/*=============================================================================
 *
 * FILE:      flash_cfg_qpic_2_0_1.h
 *
 * DESCRIPTION: This file contains defines for multipage and pagescope features.
 *
 *
 * Copyright (c) 2019 Qualcomm Technologies Incorporated.
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
 *
 * when         who     what, where, why
 * ---------   ---     ----------------------------------------------
 * 08/08/19     sl      Increase mp read count for parallel read support.
 * 07/23/19     sl      cleanup
 * 07/15/19     sl      optimize read page count
 * 06/11/19     sl      Initial Revision
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
 * 4K page is 20 for CMD DESC, 70 for READ DESC/STATUS DESC, 10 for Write DESC. 
 * Since we now support multipage reads only, with max number of pages to be read as 8, 
 * read desc size increased. The DESC FIFO size should hold atleast 1 descriptor more than 
 * the Max desc as per the BAM driver design. These values should be updated if there is any 
 * change in the code which increases the max BAM descriptors.
 * UPDATE: For SPI NAND max number of desc for CMD DESC is increased to 29
 * adding +1=30
 */
#define FLASH_BAM_STATUS_DESC_FIFO_SZ  (FLASH_MULTI_R_SUPPORT_COUNT * 8 * 8)
#define FLASH_BAM_CMD_DESC_FIFO_SZ  (30 * 8)
#define FLASH_BAM_READ_DESC_FIFO_SZ  (FLASH_MULTI_R_SUPPORT_COUNT * 8 * 8)
#define FLASH_BAM_WRITE_DESC_FIFO_SZ  (12 * 8)

/* Externs for pagescope and multipage global variable.
    Using this variable we should be able to diable/enable pagecope/multipage runtime.*/

#if defined(FLASH_QPIC_LEGACY_SUPPORT)
  #define NO_STATUS_BUFFER_ENABLED 0
  #define FLASH_MULTI_R_SUPPORT_COUNT 1
  #define FLASH_MULTI_W_SUPPORT_COUNT 1

#else
  /* Right now only 2 buffers are enabled for autostatus : 
    NAND_FLASH_STATUS_EN
    NAND_ERASED_CW_DETECT_STATUS_EN
  */
  #define NO_STATUS_BUFFER_ENABLED 2 

  /* Variable to define MULTI R/W support counts.
     Using this define we can increase/reduce the number of page that can be read using multipage.
     If multipage feature is disabled we should change the Read count to '1' */
  #define FLASH_MULTI_R_SUPPORT_COUNT 32
  #define FLASH_MULTI_W_SUPPORT_COUNT 1

  /* Comment this feature if multipage needs to be disabled */
  #define FLASH_ENABLE_MULTIPAGE

#endif

#endif  //}FLASH_CFG_QPIC_2_0_1_H
