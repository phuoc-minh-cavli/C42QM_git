#ifndef FLASH_BAM_WRAPPER_H
#define FLASH_BAM_WRAPPER_H

/*=======================================================================
 * FILE:        flash_bam_wrapper.h
 *
 * SERVICES:    Header file for bam wrapper functions
 *
 * DESCRIPTION: Header file shared across all NAND drivers
 *
 * Copyright (c) 2012-2013,2018, 2019 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 06/11/19     sl      Added support for multipage with interrupts.
 * 04/03/19     sl      Add non-blocking page read support
 * 11/29/18     sl      Increased BAM cmd desc for SPI NAND
 * 07/13/18     sa      Passed qpic_version as argument to flash_bam_init.
 * 03/25/13     eo      Add NAND_BAM_BASE_ADDR define
 * 12/04/12     sv      Changes to Support 4K Page NAND
 * 09/30/12     sv      Update BAM Pipe
 * 07/27/12     sv      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "bam.h"
#include "DALSys.h"
#include "DALSysTypes.h"
#include "msmhwiobase.h"

/*-----------------------------------------------------------------------
 *           Definitions and Constants
 *----------------------------------------------------------------------*/

/***********************************************************************
 *           Generic Defines
 **********************************************************************/
#define FLASH_BAM_SUCCESS   0
#define FLASH_BAM_FAIL      0xFFFFFFFF

#define FLASH_BAM_MAX_POLL_COUNT 100000  /* check and finetune this */
#define NAND_BAM_BASE_PHY_ADDR (QPIC_QPIC_XPU2_BASE_PHYS + 0x4000)
#define NAND_BAM_BASE_ADDR (QPIC_QPIC_XPU2_BASE + 0x4000)
#define FLASH_BAM_IRQ   164  /* Set to 0 for polling mode */

#define FLASH_BAM_DEFAULT_MASK  0xFFFFFFFF

/* BAM Descriptor Flags */
#define FLASH_BAM_DESC_FLAG_INT     BAM_IOVEC_FLAG_INT  /*  Generate interrupt */
#define FLASH_BAM_DESC_FLAG_EOT     BAM_IOVEC_FLAG_EOT  /*  Generate end-of-transfer indication */
#define FLASH_BAM_DESC_FLAG_EOB     BAM_IOVEC_FLAG_EOB  /*  Generate end-of-block indication */
#define FLASH_BAM_DESC_FLAG_NWD     BAM_IOVEC_FLAG_NWD  /*  Generate notify when done */
#define FLASH_BAM_DESC_FLAG_CMD     BAM_IOVEC_FLAG_CMD  /*  Submit a command descriptor */
#define FLASH_BAM_DESC_FLAG_LCK     BAM_IOVEC_FLAG_LCK  /*  Lock the bam to the pipe's group 
                                                            (will remain locked until desc  
                                                            with unlock bit is submitted by a
                                                            pipe from the locking pipe's grp ) */
#define FLASH_BAM_DESC_FLAG_UNLCK   BAM_IOVEC_FLAG_UNLCK  /* Unlock the bam */
#define FLASH_BAM_DESC_FLAG_IMM     BAM_IOVEC_FLAG_IMM_CMD /*  Submit an immediate command descriptor */

#define FLASH_BAM_DESC_FLAG_NOSBMT  BAM_IOVEC_FLAG_NO_SUBMIT  /*  Do not submit descriptor to hardware */
#define FLASH_BAM_DESC_FLAG_DRVDFLT BAM_IOVEC_FLAG_DEFAULT    /*  Use driver default */

/* BAM Command Element Macros */
#define FLASH_BAM_CE_WRITE_OP          0
#define FLASH_BAM_CE_READ_OP           1
#define FLASH_BAM_CE_DEFAULT_MASK      0xFFFFFFFF

/* Macros for Command/Data Descriptor Filling */
#define FLASH_BAM_FILL_DD(DD_ADDR, DBUF_BASE, SIZE, FLAGS) \
  (*(bam_iovec_type*)(DD_ADDR)).buf_pa=(uint32)(DBUF_BASE); \
  (*(bam_iovec_type*)(DD_ADDR)).buf_size=(uint32)(SIZE); \
  (*(bam_iovec_type*)(DD_ADDR)).flags=(uint32)((FLAGS) & (~BAM_IOVEC_FLAG_CMD));
						 
#define FLASH_BAM_FILL_CD(CD_ADDR, CE_BASE, NUM_CE, FLAGS) \
  (*(bam_iovec_type*)(CD_ADDR)).buf_pa=(uint32)(CE_BASE); \
  (*(bam_iovec_type*)(CD_ADDR)).buf_size=(uint32)((NUM_CE)*sizeof(bam_ce_type)); \
  (*(bam_iovec_type*)(CD_ADDR)).flags=(uint32)(FLAGS)|BAM_IOVEC_FLAG_CMD

#define FLASH_BAM_DESC_SET_PA(DESC_ADDR, BUF_BASE) \
  (*(bam_iovec_type*)(DESC_ADDR)).buf_pa=(uint32)(BUF_BASE); \
  
#define FLASH_BAM_DESC_SET_BUF_SZ(DESC_ADDR, SIZE) \
  (*(bam_iovec_type*)(DESC_ADDR)).buf_size=(uint32)(SIZE); \

#define FLASH_BAM_DESC_SET_FLAGS(DESC_ADDR, FLAGS) \
  (*(bam_iovec_type*)(DESC_ADDR)).flags=(uint32)(FLAGS);

  /* Macros for Command Element Filling */
#define FLASH_BAM_FILL_CE(CE_BASE, INDEX ,REG, CMD, DATA) \
  (*((bam_ce_type*)(CE_BASE)+(INDEX))).reg_addr=((uint32)(REG)&0xFFFFFF); \
  (*((bam_ce_type*)(CE_BASE)+(INDEX))).command=(uint32)(CMD); \
  (*((bam_ce_type*)(CE_BASE)+(INDEX))).data=(uint32)(DATA); \
  (*((bam_ce_type*)(CE_BASE)+(INDEX))).mask=(uint32)(FLASH_BAM_DEFAULT_MASK); \
  (*((bam_ce_type*)(CE_BASE)+(INDEX))).reserved=(uint32)0x00000000

#define FLASH_BAM_CE_SET_REG_ADDR(CE_BASE, REG_ADDR) \
  (*((bam_ce_type*)(CE_BASE))).reg_addr=((uint32)(REG)&0xFFFFFF);
  
#define FLASH_BAM_CE_SET_CMD(CE_BASE, CMD) \
  (*((bam_ce_type*)(CE_BASE))).command=(uint32)(CMD);
  
#define FLASH_BAM_CE_SET_DATA(CE_BASE, DATA) \
  (*((bam_ce_type*)(CE_BASE))).data=(uint32)(DATA);
  
#define FLASH_BAM_CE_SET_MASK(CE_BASE, MASK) \
  (*((bam_ce_type*)(CE_BASE))).mask=(uint32)(MASK);
  
  
struct flash_bam_xfer_desc_list
{
  bam_iovec_type *desc_list_ptr;
  uintnt desc_count;
};

uint32 flash_bam_init(uint8 status_pipe_used);

/*
  This is a blocking API that queues desc list to BAM and
  returns only after the op is completed
*/
uint32 flash_bam_transfer(struct flash_bam_xfer_desc_list *cmd_desc_list,
  struct flash_bam_xfer_desc_list *read_desc_list,
  struct flash_bam_xfer_desc_list *write_desc_list,
  struct flash_bam_xfer_desc_list *status_desc_list);

/*
  This is a non blocking API that queues desc list to BAM and
  returns without waiting for the op to complete
  flash_bam_wait_for_transfer should be called explicitly
  to check the status of the queued op
*/
uint32 flash_bam_start_transfer(
  struct flash_bam_xfer_desc_list *cmd_desc_list,
  struct flash_bam_xfer_desc_list *read_desc_list,
  struct flash_bam_xfer_desc_list *write_desc_list,
  struct flash_bam_xfer_desc_list *status_desc_list);

/*
  This API waits for the previously started transfer to complete
  This API should only be used after calling flash_bam_start_transfer
*/
uint32 flash_bam_wait_for_xfer_completion(
  struct flash_bam_xfer_desc_list *cmd_desc_list,
  struct flash_bam_xfer_desc_list *read_desc_list,
  struct flash_bam_xfer_desc_list *write_desc_list,
  struct flash_bam_xfer_desc_list *status_desc_list);

uint32 flash_bam_deinit(void);

#endif  /* FLASH_BAM_WRAPPER_H */
