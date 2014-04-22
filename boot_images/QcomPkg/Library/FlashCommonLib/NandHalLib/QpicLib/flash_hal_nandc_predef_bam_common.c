/*=============================================================================
 *
 * FILE:      flash_hal_nandc_predef_bam_common.c
 *
 * DESCRIPTION: This file contains functions which are common for regular reads as well as pagescope/multipage read.
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
 * when         who     what, where, why
 * ---------   ---     ----------------------------------------------
 * 08/08/19     sl      Add pagescope/multipage non-blocking read support.
 * 06/11/19     sl      Added SPI NAND support
 * 10/25/18     sa      Initial Revision
 *==================================================================*/

/*===================================================================
 *                     Include Files
 ====================================================================*/

#include "flashi_hal_nandc_predef_bam.h"

/* Return the cache maintanance information for predef operations */
void hal_nandc_get_cache_info(struct flash_hal_cache_info *mem_info)
{
  /* Please refer the above memory allocation table for details on
     cache maintanance information.

     The region table is populated in such a way that all the read buffers
     are together and write buffers together. Also the multi related
     buffers grouped together for easy maintanance.
   */ 
    mem_info->read_buffer_count = 1;
    mem_info->read_mem =
      &nandc_predef_bam_data.mem_info.region[NANDC_BAM_REG_READ_BUFF_REGION];
    mem_info->write_buffer_count = 1;
    mem_info->write_mem =
      &nandc_predef_bam_data.mem_info.region[NANDC_BAM_CMDBLOCK_BUFF_REGION];
}

/*
 * Set the config buffers
 */
void hal_nandc_bam_set_cfg(struct flash_hal_ctlr_data *priv_data)
{
  struct flash_hal_op_cmd *op_cmd  = nandc_predef_bam_data.cmd_list.cfg_list;
  struct flash_hal_op_cmd *op_wcmd = nandc_predef_bam_data.cmd_list.wcfg_list;

  /* Configure CFG0 and CFG1 Registers */
  op_cmd->data = priv_data->cfg0;
  op_cmd++;
  op_cmd->data = priv_data->cfg1;
  op_cmd++;

  op_wcmd->data = priv_data->cfg0;
  op_wcmd++;
  op_wcmd->data = priv_data->cfg1;
  op_wcmd++;

  /* Configure ECC CFG register */
  op_cmd->data = priv_data->ecc_cfg | FLASH_NAND_DEV0_ECC_CFG_ECC_SW_RESET;
  op_cmd++;

  op_wcmd->data = priv_data->ecc_cfg | FLASH_NAND_DEV0_ECC_CFG_ECC_SW_RESET;
  op_wcmd++;

    /* Configure ECC CFG register */
  op_cmd->data = priv_data->ecc_cfg;
  op_cmd++;
  op_wcmd->data = priv_data->ecc_cfg;
  op_wcmd++;

  /* Reset the Erased CW detect status */
  op_cmd++;

  /* Configure Erased CW detectcfg register */
  FLASH_ENABLE_ERASED_CW_DETECT(priv_data);
  op_cmd->data = priv_data->erased_cw_det_cfg; 
}

/*
 * Set the command register
 */
void hal_nandc_bam_set_cmd(uint32 cmd, uint32 operation, 
  struct flash_hal_ctlr_data *priv_data )
{
  struct flash_hal_op_cmd *op_cmd  = nandc_predef_bam_data.cmd_list.cmd_exec_list;
  struct nandc_predef_command_list_index *index_list = &nandc_predef_bam_data.index_list;

  if(priv_data->dev_type == FLASH_HAL_DEVICE_NAND_SPI)
  {
    /* Set these flags: SPI_TRANSFER_MODE, SPI_WP, SPI_HOLD*/
    if ((NANDC_PAGE_WRITE_CMD == operation) || (NANDC_PAGE_READ_CMD == operation))
    {
      cmd |= SPI_MODE_ENABLE_FOR_RW_CMD;
    }
    else
    {
      cmd |= SPI_MODE_ENABLE_FOR_NON_RW_CMD;
    }
  }

  op_cmd->data = cmd;
  if (operation == NANDC_PAGE_READ_CMD)
  {
    if(priv_data->qpic_major_version >= FLASH_HAL_QPIC_CTRL_VERSION_2_0)
    {
      op_cmd  = &nandc_predef_bam_data.cmd_list.locate_list[4];
    }
    else
    {
      op_cmd  = &nandc_predef_bam_data.cmd_list.locate_list[2];
    }
    op_cmd->data = cmd;
    op_cmd  = &nandc_predef_bam_data.cmd_list.cfg_list[index_list->nandc_bam_rcmd_cfg_cmd_list];  
    op_cmd->data = cmd;
  }
  else if (operation == NANDC_PAGE_WRITE_CMD)
  {
    op_cmd  = &nandc_predef_bam_data.cmd_list.wcfg_list[6];
    op_cmd->data = cmd;
  }
}

/*
 * Set the address buffers for page operations
 */
void hal_nandc_bam_set_addr( struct flash_hal_ctlr_data *priv_data,
  uint32 row_val, uint32 *addr0, uint32 *addr1, uint32 num_pages)
{
  uint32 addr0_val = 0x0;
  uint32 addr1_val = 0x0;
  uint32 row_shift = 0;

  row_shift = sizeof(uint32) - priv_data->col_count;
  
  addr0_val = 0;
  addr0_val |= row_val << (priv_data->col_count * 8);
  addr1_val = row_val >> (row_shift * 8);

  /* ADDR0 = 'col_count' bytes of '0' and remaining bytes with row_val */
  *addr0 = addr0_val;

  /* ADDR1 = remaining bytes of row_val */
  *addr1 = addr1_val;

}

/*
 * Set the buffer pointers for read
 */
void hal_nandc_bam_set_read_buffers(
  struct flash_hal_predef_page_data *page_data,
  struct flash_hal_page_layout *page_layout,
  struct flash_hal_op_desc *read_desc,
  struct flash_hal_ctlr_data *priv_data,
  uint32 *num_read_desc)
{
  uint32 spare_copy_len = 0;
  uint32 page_unit_count = 0;
  uintnt main_addr = 0, spare_addr = 0;
  uint32 spare_len = 0;
  struct flash_hal_buffer_desc *buff_desc;
  const struct flash_hal_page_unit_layout *page_unit_layout;
  struct flash_hal_op_desc *read_desc_ptr;
  struct flash_hal_op_cmd *op_cmd;
  uint32 page_unit_layout_cnt = 0;
  struct nandc_predef_command_list_index *index_list = &nandc_predef_bam_data.index_list;
  struct nandc_predef_cmd_list *cmd_list = &nandc_predef_bam_data.cmd_list;
  uint32 page_to_read = page_data->total_pages;

  /* NOTE: This code is optimized to read entire
  page data every time. This supports the below
  scenarios,
  - Read all main data bytes
  - Read all main and spare bytes
  No partial reads are supported by this code.
  */

  /* Set main and spare buffer pointers */
  buff_desc = page_data->buff_desc;
  read_desc_ptr = read_desc;

  main_addr = (uintnt) buff_desc->buff_paddr;
  spare_addr = (uintnt) (buff_desc + 1)->buff_paddr;

  spare_len = page_data->spare_size_bytes;

  page_unit_layout = page_layout->unit_layout;
  page_unit_count = page_layout->page_unit_count;

  /* Config Locate Register for Data Read */
  op_cmd = cmd_list->locate_list;
  page_unit_layout_cnt = page_unit_layout->num_code_word;

  /* Config Locate Register for the first code words - This is sufficient for now
  * Need to be revisited if new layouts are introduced */
  op_cmd->data = FLASH_NAND_READ_LOCATION_n_LAST | 
  page_unit_layout->main_len_bytes << FLASH_NAND_READ_LOCATION_n_SIZE_SHIFT;
  /* Update Locate Register in cfg list */
  cmd_list->cfg_list[index_list->nandc_bam_loc_cfg_cmd_list].data = op_cmd->data;

  if(priv_data->qpic_major_version >= FLASH_HAL_QPIC_CTRL_VERSION_2_0)
  {
    op_cmd++;
    op_cmd->data = FLASH_NAND_READ_LOCATION_LAST_CW_n_LAST | 
    page_unit_layout->main_len_bytes << FLASH_NAND_READ_LOCATION_LAST_CW_n_SIZE_SHIFT;
    /* Update Locate Register in cfg list */
    cmd_list->cfg_list[index_list->nandc_bam_loc_cfg_cmd_list+1].data = op_cmd->data;
  }  

  op_cmd++;

  while(page_to_read)
  {
    page_unit_layout = page_layout->unit_layout;
    page_unit_count = page_layout->page_unit_count;
    page_unit_layout_cnt = page_unit_layout->num_code_word;
    while (page_unit_layout_cnt)
    {
      read_desc_ptr->buf_pa = main_addr;
      read_desc_ptr->buf_size = page_unit_layout->main_len_bytes;
      read_desc_ptr->flags = FLASH_BAM_DESC_FLAG_INT;

      if ((page_unit_count == 1) && (page_unit_layout_cnt == 1) && page_to_read ==1)
      {
        /* Config Locate Register for the last code words - This is sufficient for now
        * Need to be revisited if new layouts are introduced */
        op_cmd->data = FLASH_NAND_READ_LOCATION_n_LAST | 
        (page_unit_layout->main_len_bytes << 
        FLASH_NAND_READ_LOCATION_n_SIZE_SHIFT);

        if(priv_data->qpic_major_version >= FLASH_HAL_QPIC_CTRL_VERSION_2_0)
        {
          op_cmd++;
          op_cmd->data = FLASH_NAND_READ_LOCATION_LAST_CW_n_LAST | 
          (page_unit_layout->main_len_bytes << 
          FLASH_NAND_READ_LOCATION_LAST_CW_n_SIZE_SHIFT);
        }
      }
      spare_copy_len = (spare_len > page_unit_layout->udata_len_bytes) ?
      page_unit_layout->udata_len_bytes : spare_len;
      if (spare_copy_len)
      {
        read_desc_ptr->flags = 0;
        read_desc_ptr++;
        read_desc_ptr->buf_pa = spare_addr;
        read_desc_ptr->buf_size = spare_copy_len;
        read_desc_ptr->flags = FLASH_BAM_DESC_FLAG_INT;

        /* Decrement the op_cmd to overwrite the right descriptor */
        op_cmd--;

        /* check if we need to read less that 16 user bytes and any issue will be there */
        op_cmd->data = FLASH_NAND_READ_LOCATION_n_LAST | 
        ((page_unit_layout->main_len_bytes + spare_copy_len) << 
        FLASH_NAND_READ_LOCATION_n_SIZE_SHIFT);

        if(priv_data->qpic_major_version >= FLASH_HAL_QPIC_CTRL_VERSION_2_0)
        {
          op_cmd++;
          op_cmd->data = FLASH_NAND_READ_LOCATION_LAST_CW_n_LAST | 
          ((page_unit_layout->main_len_bytes + spare_copy_len) << 
          FLASH_NAND_READ_LOCATION_LAST_CW_n_SIZE_SHIFT);
        }
      }
      spare_len -= spare_copy_len;
      main_addr += page_unit_layout->main_len_bytes;
      spare_addr += spare_copy_len;

      if(--page_unit_layout_cnt == 0)
      {
        page_unit_count--;
        if (page_unit_count)
        {
          page_unit_layout++;
          page_unit_layout_cnt = page_unit_layout->num_code_word;
        }
      }
      read_desc_ptr++;
    }
    page_to_read--;
  }

  *num_read_desc = read_desc_ptr - read_desc;
}

int hal_nandc_bam_exec_op(struct flash_bam_xfer_desc_list *tx_cmd_list, 
  struct flash_bam_xfer_desc_list *tx_read_list, 
  struct flash_bam_xfer_desc_list *tx_write_list,
  struct flash_bam_xfer_desc_list *tx_status_list)
{
  int result;
  struct flash_hal_mem_data *mem_data;
  uint32 temp_count = 0;
  struct flash_hal_cache_info mem_info;
	
  hal_nandc_get_cache_info(&mem_info);
  
  mem_data = &mem_info.read_mem[0];

  /* Do the cache maintanence - cache invalidate - read buffers*/
  while (temp_count++ < mem_info.read_buffer_count)
  {
    flash_hal_cache_inval(mem_data->buff_vaddr, mem_data->buff_size);
    mem_data++;
  }

  /* Do the cache maintanence - cache clean - write buffers*/
  temp_count = 0;
  mem_data = &mem_info.write_mem[0];

  while (temp_count++ < mem_info.write_buffer_count)
  {
    flash_hal_cache_clean(mem_data->buff_vaddr, mem_data->buff_size);
    mem_data++;
  }
  
  result = flash_bam_transfer(tx_cmd_list, tx_read_list, tx_write_list, tx_status_list);

  while (temp_count++ < mem_info.read_buffer_count)
  {
    flash_hal_cache_inval(mem_data->buff_vaddr, mem_data->buff_size);
    mem_data++;
  }

  return result;
}

int flash_bam_start_exec_op(struct flash_bam_xfer_desc_list *tx_cmd_list,
  struct flash_bam_xfer_desc_list *tx_read_list, 
  struct flash_bam_xfer_desc_list *tx_write_list,
  struct flash_bam_xfer_desc_list *tx_status_list)
{
  int result;
  struct flash_hal_mem_data *mem_data;
  uint32 temp_count = 0;
  struct flash_hal_cache_info mem_info;
	
  hal_nandc_get_cache_info(&mem_info);
  
  mem_data = &mem_info.read_mem[0];

  /* Do the cache maintanence - cache invalidate - read buffers*/
  while (temp_count++ < mem_info.read_buffer_count)
  {
    flash_hal_cache_inval(mem_data->buff_vaddr, mem_data->buff_size);
    mem_data++;
  }

  /* Do the cache maintanence - cache clean - write buffers*/
  temp_count = 0;
  mem_data = &mem_info.write_mem[0];

  while (temp_count++ < mem_info.write_buffer_count)
  {
    flash_hal_cache_clean(mem_data->buff_vaddr, mem_data->buff_size);
    mem_data++;
  }

  result = flash_bam_start_transfer(tx_cmd_list, tx_read_list, tx_write_list, tx_status_list);

  return result;
}

int flash_bam_wait_for_exec_op(struct flash_bam_xfer_desc_list *tx_cmd_list,
  struct flash_bam_xfer_desc_list *tx_read_list, 
  struct flash_bam_xfer_desc_list *tx_write_list,
  struct flash_bam_xfer_desc_list *tx_status_list)
{
  int result;
  struct flash_hal_mem_data *mem_data;
  uint32 temp_count = 0;
  struct flash_hal_cache_info mem_info;
	
  hal_nandc_get_cache_info(&mem_info);
  
  mem_data = &mem_info.read_mem[0];

  result = flash_bam_wait_for_xfer_completion(tx_cmd_list,
                                              tx_read_list,
                                              tx_write_list,
                                              tx_status_list);

  while (temp_count++ < mem_info.read_buffer_count)
  {
    flash_hal_cache_inval(mem_data->buff_vaddr, mem_data->buff_size);
    mem_data++;
  }

  return result;
}

