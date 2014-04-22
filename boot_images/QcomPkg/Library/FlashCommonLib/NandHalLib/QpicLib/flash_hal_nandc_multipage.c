/*=============================================================================
 *
 * FILE:      flash_hal_multipage.c
 *
 * DESCRIPTION: This file contains implementation for multipage reads.
 *
 *
 * Copyright (c) 2018-2019 Qualcomm Technologies Incorporated.
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
 *
 * when         who     what, where, why
 * ---------   ---     ----------------------------------------------
 * 08/08/19     sl      Add pagescope/multipage non-blocking read support.
 * 07/23/19     sl      enabled common code for pagescope and mp
 * 06/11/19     sl      Added SPI NAND support
 * 10/25/18     sa      Initial Revision
 *==================================================================*/

/*===================================================================
 *                     Include Files
 ====================================================================*/

#include "flashi_hal_nandc_predef_bam.h"

#if defined(FLASH_ENABLE_MULTIPAGE)

boolean hal_nandc_bam_is_multipage_enabled(uint32 version)
{
  if(version < FLASH_HAL_QPIC_CTRL_VERSION_2_0)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*
 * Does the post processing for multipage read.
 */
void hal_nandc_bam_00_30_x1_multipage_pprocess(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_op_list *op_list, enum flash_hal_status *status, uint32 page_count)
{
  int32 cw_count, erase_cw_count = 0;
  uint32 status_val = 0, mask = FLASH_NANDC_STATUS_MASK;
  uint32 *status_buffer, *erased_cw_det_status_buffer;
  struct nandc_predef_reg_read_buff *reg_read_buff;
  uint32 no_of_pages = page_count;

  *status = FLASH_HAL_SUCCESS;

  reg_read_buff = &nandc_predef_bam_data.reg_read_buff;

  status_buffer = (uint32 *) reg_read_buff->multipage_status_buffer;
  erased_cw_det_status_buffer = status_buffer + 1;

  /* For each page,
  - Check for status bytes
  - Check for Erase page
  - Check for ECC bytes
  - Check for Spare Udata bytes (if applicable)
  - Check for Erased CW detect status register
  */
  while(no_of_pages)
  {
    cw_count = priv_data->cw_cnt - 1;
    while (cw_count >= 0)
    {
      /* Check for flash status bytes */
      status_val = (cw_count << FLASH_NAND_STATUS_CW_CNTR_SHFT) |
      FLASH_NAND_STATUS_OP_DONE;

      /* If there is an error, look for erased page */
      if ((*status_buffer & mask) != status_val)
      {
        if  (FLASH_NAND_STATUS_MPU_ERROR_MASK ==
            (*status_buffer & (FLASH_NAND_STATUS_MPU_ERROR_MASK & mask)))
        {
          *status = FLASH_HAL_FAIL_MPU_ERR;
          break;
        }

        /*
          Now check for erase CW status. In case of pagescope feature,
          instead of buffer being written for the register
          "QPIC_NAND_ERASED_CW_DETECT_STATUS", qpic only gets the status
          value of that register due to which the status for CW/Page
          gets reflected in bytes '0-3' instead of being reflected in
          bytes '4-7'. So shifting it by 4.
        */
        if (((*erased_cw_det_status_buffer << 4) & 
              FLASH_NANDC_ERASED_CW_DET_STATUS_MASK) ==
              FLASH_ERASED_CW_DETECT_STATUS)
        {
          *status = FLASH_HAL_ERASED_PAGE;
          erase_cw_count++;
        }
        else
        {
          *status = FLASH_HAL_CHECK_ECC;
          break;
        }
      }
      /* Status is processed, so resetting it for another read */
      memset((void *)status_buffer, 0x0, sizeof(uint32));

      status_buffer +=  NO_STATUS_BUFFER_ENABLED;
      erased_cw_det_status_buffer = status_buffer + 1;
      cw_count--;
    }
    no_of_pages--;
  }

  if (erase_cw_count == priv_data->cw_cnt * page_count)
  {
    *status = FLASH_HAL_ERASED_PAGE;
  }
}

/* Multipage read
 *
 * Note: Please make sure that you pass enough memory to this
 *       API. We are determining the memory used only at the
 *       end of this function. This may lead to memory corruption.
 */
enum flash_hal_status hal_nandc_bam_00_30_x1_multipage_init(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *mem_data, uint32 *used_buff_size)
{
  uintnt buff_vaddr = 0;
  uintnt buff_paddr = 0;
  struct nandc_predef_cmd_list *cmd_list;
  struct nandc_predef_cmdptr_mem_data *cmdptr_mem_data;
  struct flash_hal_predef_op_list *op_list;
  struct flash_hal_op_desc  *cmd_desc_vaddr_ptr;

  if(FALSE == hal_nandc_bam_is_multipage_enabled(priv_data->qpic_major_version))
  {
    return FLASH_HAL_SUCCESS;
  }

  cmd_list = &nandc_predef_bam_data.cmd_list;
  cmdptr_mem_data =
  &nandc_predef_bam_data.mem_info.cmd_ptr[NANDC_00_30_x1_MULTIPAGE_READ];
  op_list = &nandc_predef_bam_data.cmd_ptr_list[NANDC_00_30_x1_MULTIPAGE_READ];

  /* Flash HAL creates a list which is BAM friendly. ie,
  All the buffers and addresses in the pre-allocated list
  will be with physical addresses. And all the command
  pointer list and command list are aligned to 32 byte
  boundary. Please note that the command pointer list
  is virtual. */

  /* Get a 32 byte aligned buffer for the command pointer list */

  buff_paddr = mem_data->buff_paddr;
  buff_paddr = FLASH_ALIGN_32(buff_paddr);

  buff_vaddr = mem_data->buff_vaddr;
  buff_vaddr = FLASH_ALIGN_32(buff_vaddr);

  cmdptr_mem_data->mem_data.buff_vaddr = buff_vaddr;
  cmdptr_mem_data->mem_data.buff_paddr = buff_paddr;
  cmdptr_mem_data->seq_id = NANDC_00_30_x1_MULTIPAGE_READ;

  cmd_desc_vaddr_ptr = (struct flash_hal_op_desc *)buff_vaddr;

  /*--------------------------------------------------------------
  |  The command pointer list for Multipage Read has the following format,
  |
  |  - config (cfg0/cfg1/ecc buf/cs/erased cw det cfg) + Lock Pipe
  |  - Set NUM_PAGES,  Auto Status enable
  |  - Set Locate Registers
  |  - Exec
  |  - Set NUM_PAGES to 0 + Unlock
  |
  *-------------------------------------------------------------*/

  /* Lock Pipe and Set config registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->cfg_list[0]);
  cmd_desc_vaddr_ptr->buf_size
    = sizeof(struct flash_hal_op_cmd) * 
      (nandc_predef_bam_data.reg_count.total_nandc_config_list_registers - 2);
  /* Do not include the exec command.*/
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_LCK;
  cmd_desc_vaddr_ptr++;

  /* Multipage Descriptors. This will include 'num_pages' as well as 'AUTO_STATUS_ENABLE' */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->mp_enable_list[1]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2;
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
  cmd_desc_vaddr_ptr++;

  /* Configure Locate Register for Data Read and set cmd - exec registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->locate_list[3]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 3; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_NWD;
  cmd_desc_vaddr_ptr++;

  /* set auto status to 0 */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->auto_status_list[1]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd); 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
  cmd_desc_vaddr_ptr++;

  /*  Multipage Descriptors.
      This will se the num_pages back to 0.
      After this we will unlock the pipe.
  */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->mp_enable_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) ;
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_INT | FLASH_BAM_DESC_FLAG_UNLCK;
  cmd_desc_vaddr_ptr++;

  /* Get the memory use and store it */
  *used_buff_size = (uintnt)cmd_desc_vaddr_ptr - mem_data->buff_vaddr;
  cmdptr_mem_data->mem_data.buff_size = *used_buff_size;

  /* Initialize the BAM list for read */
  #ifdef LOADER_CODE
    op_list->op_list = (uint32 *) cmdptr_mem_data->mem_data.buff_paddr;
  #else
    op_list->op_list = (uint32 *) cmdptr_mem_data->mem_data.buff_vaddr;
  #endif

  op_list->op_list_count = ((uint32)((uintnt)cmd_desc_vaddr_ptr - buff_vaddr)) /
  sizeof(struct flash_hal_op_desc);
  op_list->seq_id = NANDC_00_30_x1_MULTIPAGE_READ;
  op_list->prev_op_count = 1;

  /* If memory used is more than given, return failure. */
  return ((*used_buff_size > mem_data->buff_size) ?
  FLASH_HAL_NO_MEM_ERROR : FLASH_HAL_SUCCESS);
}

/* Set the multipage mode command */
static void nandc_set_mp_mode(int count)
{
  struct flash_hal_op_cmd *op_cmd  = NULL;

  op_cmd = &nandc_predef_bam_data.cmd_list.locate_list[4];  

  if(1 == count)
  {
    /* Use pagescope for 1 page */
    op_cmd->data = op_cmd->data | HWIO_NAND_FLASH_CMD_PAGE_SCOPE_CMD_BMSK;
  }
  else
  {
    /* Use multipage for more than 1 page */
    op_cmd->data = op_cmd->data | HWIO_NAND_FLASH_CMD_MULTI_PAGE_CMD_BMSK;
  }
}

/* Set the number of pages to read using multipage feature.
    The count start from '0', so set the register to '7' to read '8' pages.*/
static void nandc_set_page_count(int count)
{  
  struct flash_hal_op_cmd *op_cmd  = nandc_predef_bam_data.cmd_list.mp_enable_list;

  /* Increment here to point to the 2nd numpages descriptor */
  op_cmd++;

  /* Set the number of pages. Page count starts from 0, so set it to 1 less number.*/
  op_cmd->data = (count - 1);
}

/*
 * Prepare the Multipage Exec command list
 */
void hal_nandc_bam_00_30_x1_multipage_exec(flash_hal_dev_info *dev_info,
      struct flash_hal_predef_page_data *page_data,
      struct flash_hal_predef_op_list **op_list,
      enum flash_hal_status *status)
{
  uint32 cmd = 0, cw_count = 0;
  struct flash_hal_page_layout *page_layout;
  struct flash_hal_ctlr_data *priv_data;
  struct flash_hal_op_cmd *op_cmd;
  uint32 *addr0 = NULL, *addr1 = NULL;
  uint32 num_read_desc = 0;
  struct flash_bam_xfer_desc_list tx_cmd_list;
  struct flash_bam_xfer_desc_list tx_read_list;
  struct flash_bam_xfer_desc_list tx_status_list;
  struct flash_hal_predef_op_list *cmd_op_list;	
  int result;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  if(FALSE == hal_nandc_bam_is_multipage_enabled(priv_data->qpic_major_version))
  {
    *status = FLASH_HAL_MULTIPAGE_NOT_SUPPORTED;
    return;
  }

  if ((0 == page_data->total_pages)
  || (FLASH_MULTI_R_SUPPORT_COUNT < page_data->total_pages))
  {
    *status = FLASH_HAL_FAIL;
    return;
  }
  cmd_op_list = &nandc_predef_bam_data.cmd_ptr_list[NANDC_00_30_x1_MULTIPAGE_READ];
  dev_info->ctlr_ops.flash_hal_configure(priv_data, status);

  /* Get page layout */
  dev_info->ctlr_ops.flash_hal_get_page_layout(priv_data, &page_layout);

  /* Set address bytes */
  op_cmd = &nandc_predef_bam_data.cmd_list.cfg_list[nandc_predef_bam_data.
  index_list.nandc_bam_raddr0_cfg_cmd_list];
  addr0 = &(op_cmd->data);
  op_cmd++;
  addr1 = &(op_cmd->data);

  hal_nandc_bam_set_addr(priv_data, page_data->start_page, addr0, addr1, 1);

  /*---- Set config registers -----*/
  cw_count = priv_data->cw_cnt - 1;

  /* Set the codeword count */
  FLASH_CW_PER_PAGE(priv_data, cw_count);

  /* Set the number of address cycles */
  FLASH_NUM_ADDR_CYCLES(priv_data, (priv_data->row_count +
  priv_data->col_count));

  hal_nandc_bam_set_cfg(priv_data);

  /* Set command registers */
  if(priv_data->dev_type == FLASH_HAL_DEVICE_NAND_SPI)
  {
    /* FLASH_NAND_PAGE_READ_ALL_CMD(0x34) does not support ECC disable mode
    in current QPIC version (2_0_1) so we use FLASH_NAND_PAGE_READ_CMD(0x32) instead */
    cmd = (priv_data->ecc_state == FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE)?
    FLASH_NAND_PAGE_READ_ECC_CMD:FLASH_NAND_PAGE_READ_CMD;
  }
  else
  {
    cmd = (priv_data->ecc_state == FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE)?
    FLASH_NAND_PAGE_READ_ECC_CMD:FLASH_NAND_PAGE_READ_ALL_CMD;
  }

  hal_nandc_bam_set_cmd(cmd, NANDC_PAGE_READ_CMD, priv_data);

  /* Set the page count */
  nandc_set_page_count(page_data->total_pages);

  /* Set the multipage feature bit */
  nandc_set_mp_mode(page_data->total_pages);

  /* Configure buffer pointers */
  hal_nandc_bam_set_read_buffers(page_data, page_layout, 
  nandc_predef_bam_data.desc.read_copy_desc, priv_data, &num_read_desc);

  /* Command descriptor list */
  tx_cmd_list.desc_list_ptr = (bam_iovec_type *)cmd_op_list->op_list;
  tx_cmd_list.desc_count = cmd_op_list->op_list_count;

  /* Data Descriptor list */
  tx_read_list.desc_list_ptr = (bam_iovec_type *)
  nandc_predef_bam_data.desc.read_copy_desc;
  tx_read_list.desc_count = num_read_desc;

  /* Status descriptor list */
  tx_status_list.desc_list_ptr = (bam_iovec_type *)
  nandc_predef_bam_data.desc.auto_status_desc;
  tx_status_list.desc_count = priv_data->cw_cnt * (page_data->total_pages ); 

  result = hal_nandc_bam_exec_op(&tx_cmd_list, &tx_read_list, NULL, &tx_status_list);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_00_30_x1_multipage_pprocess(priv_data, cmd_op_list, status, page_data->total_pages);
  }
}

struct flash_bam_xfer_desc_list flash_bam_mqueued_tx_cmd_list;
struct flash_bam_xfer_desc_list flash_bam_mqueued_tx_read_list;
struct flash_bam_xfer_desc_list flash_bam_mqueued_tx_status_list;
uint8 flash_bam_mqueued_total_pages;

/*
 * Prepare the Multipage Exec command list
 */
void hal_nandc_bam_start_mpage_read_exec(flash_hal_dev_info *dev_info,
      struct flash_hal_predef_page_data *page_data,
      struct flash_hal_predef_op_list **op_list,
      enum flash_hal_status *status)
{
  uint32 cmd = 0, cw_count = 0;
  struct flash_hal_page_layout *page_layout;
  struct flash_hal_ctlr_data *priv_data;
  struct flash_hal_op_cmd *op_cmd;
  uint32 *addr0 = NULL, *addr1 = NULL;
  uint32 num_read_desc = 0;
  struct flash_hal_predef_op_list *cmd_op_list;	
  int result;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  if(FALSE == hal_nandc_bam_is_multipage_enabled(priv_data->qpic_major_version))
  {
    *status = FLASH_HAL_MULTIPAGE_NOT_SUPPORTED;
    return;
  }

  if ((0 == page_data->total_pages)
  || (FLASH_MULTI_R_SUPPORT_COUNT < page_data->total_pages))
  {
    *status = FLASH_HAL_FAIL;
    return;
  }
  cmd_op_list = &nandc_predef_bam_data.cmd_ptr_list[NANDC_00_30_x1_MULTIPAGE_READ];
  dev_info->ctlr_ops.flash_hal_configure(priv_data, status);

  /* Get page layout */
  dev_info->ctlr_ops.flash_hal_get_page_layout(priv_data, &page_layout);

  /* Set address bytes */
  op_cmd = &nandc_predef_bam_data.cmd_list.cfg_list[nandc_predef_bam_data.
  index_list.nandc_bam_raddr0_cfg_cmd_list];
  addr0 = &(op_cmd->data);
  op_cmd++;
  addr1 = &(op_cmd->data);

  hal_nandc_bam_set_addr(priv_data, page_data->start_page, addr0, addr1, 1);

  /*---- Set config registers -----*/
  cw_count = priv_data->cw_cnt - 1;

  /* Set the codeword count */
  FLASH_CW_PER_PAGE(priv_data, cw_count);

  /* Set the number of address cycles */
  FLASH_NUM_ADDR_CYCLES(priv_data, (priv_data->row_count +
  priv_data->col_count));

  hal_nandc_bam_set_cfg(priv_data);

  /* Set command registers */
  if(priv_data->dev_type == FLASH_HAL_DEVICE_NAND_SPI)
  {
    /* FLASH_NAND_PAGE_READ_ALL_CMD(0x34) does not support ECC disable mode
    in current QPIC version (2_0_1) so we use FLASH_NAND_PAGE_READ_CMD(0x32) instead */
    cmd = (priv_data->ecc_state == FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE)?
    FLASH_NAND_PAGE_READ_ECC_CMD:FLASH_NAND_PAGE_READ_CMD;
  }
  else
  {
    cmd = (priv_data->ecc_state == FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE)?
    FLASH_NAND_PAGE_READ_ECC_CMD:FLASH_NAND_PAGE_READ_ALL_CMD;
  }

  hal_nandc_bam_set_cmd(cmd, NANDC_PAGE_READ_CMD, priv_data);

  /* Set the page count */
  nandc_set_page_count(page_data->total_pages);

  /* Set the multipage feature bit */
  nandc_set_mp_mode(page_data->total_pages);

  /* Configure buffer pointers */
  hal_nandc_bam_set_read_buffers(page_data, page_layout, 
  nandc_predef_bam_data.desc.read_copy_desc, priv_data, &num_read_desc);

  /* Command descriptor list */
  flash_bam_mqueued_tx_cmd_list.desc_list_ptr = (bam_iovec_type *)cmd_op_list->op_list;
  flash_bam_mqueued_tx_cmd_list.desc_count = cmd_op_list->op_list_count;

  /* Data Descriptor list */
  flash_bam_mqueued_tx_read_list.desc_list_ptr = (bam_iovec_type *)
  nandc_predef_bam_data.desc.read_copy_desc;
  flash_bam_mqueued_tx_read_list.desc_count = num_read_desc;

  /* Status descriptor list */
  flash_bam_mqueued_tx_status_list.desc_list_ptr = (bam_iovec_type *)
  nandc_predef_bam_data.desc.auto_status_desc;
  flash_bam_mqueued_tx_status_list.desc_count = priv_data->cw_cnt * (page_data->total_pages);
  flash_bam_mqueued_total_pages = page_data->total_pages;

  result = flash_bam_start_exec_op(
                                    &flash_bam_mqueued_tx_cmd_list,
                                    &flash_bam_mqueued_tx_read_list,
                                    NULL,
                                    &flash_bam_mqueued_tx_status_list);
  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
}

/*
 * wait for previously queued read page request to complete
 */
void hal_nandc_bam_wait_for_mpage_read_exec(flash_hal_dev_info *dev_info,
 enum flash_hal_status  *status)
{
  struct flash_hal_ctlr_data *priv_data;
  struct flash_hal_predef_op_list *cmd_op_list;
  int result;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  cmd_op_list = &nandc_predef_bam_data.cmd_ptr_list[NANDC_00_30_x1_READ];

  result = flash_bam_wait_for_exec_op(
              &flash_bam_mqueued_tx_cmd_list,
              &flash_bam_mqueued_tx_read_list,
              NULL,
              &flash_bam_mqueued_tx_status_list);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_00_30_x1_multipage_pprocess(priv_data, cmd_op_list, status, flash_bam_mqueued_total_pages);
  }
}

#else  //}{

boolean hal_nandc_bam_is_multipage_enabled(uint32 version)
{
  return FALSE;
}

/* Multipage read
 *
 * Note: Please make sure that you pass enough memory to this
 *       API. We are determining the memory used only at the
 *       end of this function. This may lead to memory corruption.
 */
enum flash_hal_status hal_nandc_bam_00_30_x1_multipage_init(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *mem_data, uint32 *used_buff_size)
{
  return FLASH_HAL_SUCCESS;
}

/*
 * Prepare the Multipage Exec command list
 */
void hal_nandc_bam_00_30_x1_multipage_exec(flash_hal_dev_info *dev_info,
        struct flash_hal_predef_page_data *page_data,
        struct flash_hal_predef_op_list **op_list, enum flash_hal_status	*status)
{
  *status = FLASH_HAL_MULTIPAGE_NOT_SUPPORTED;
}

#endif  //}
