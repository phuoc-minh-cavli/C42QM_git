/**====================================================================
 *
 * FILE:        flash_hal_nandc_predef_bam_exec.c
 *
 * SERVICES:    This prepares a predefined sequence for BAM transfer
 *
 * DESCRIPTION: The perdefined BAM support in flash HAL is to improve
 *              throughput with pre-allocated BAM lists.
 *
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2012-2015,2017-2019 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

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
 * 08/08/19     sl      Add pagescope/multipage non-blocking read support.
 * 06/11/19     sl      Added support for pagescope/multipage.
 * 04/03/19     sl      Add non-blocking page read support
 * 02/08/19     sl      Enable SPI NAND quad speed support
 * 01/03/19     sl      Corrected read cfg list initialization
 * 11/29/18     sl      added support for SPI NAND
 * 08/24/18     sa      Changed FLASH_HAL_QPIC_CTRL_VERSION to FLASH_HAL_QPIC_CTRL_VERSION_2_0
 * 07/09/18     sa      Added QPIC_NAND_READ_LOCATION_LAST_CW_n register.
 * 02/27/18     eo      Set correct BBM value in BB marker read 
 * 08/14/17     eo      Add QPIC boost timing mode support 
 * 08/03/15     sb      Enhance ONFI param page read to read multiple copies
 * 11/28/14     sb      Use 0x35 cmd for ONFI parameter page read
 * 08/20/14     sb      Cleanup the BB marker read API
 * 09/03/13     eo      Include target specific HWIO register mapping
 * 10/02/12     sv      Add Transfer Steps config
 * 09/04/12     sv      Initial Revision
 *===================================================================*/

#include "flashi_hal_nandc_predef_bam.h"

struct flash_bam_xfer_desc_list flash_bam_queued_tx_cmd_list;
struct flash_bam_xfer_desc_list flash_bam_queued_tx_read_list;

/*
 * Set the address buffers for block operations
 */
static void nandc_set_block_addr( struct flash_hal_ctlr_data *priv_data,
  uint32 *addr0, uint32 *addr1, uint32 start_block, uint32 pages_per_block,
  uint32 num_blocks)
{
  if(priv_data->dev_type == FLASH_HAL_DEVICE_NAND_SPI)
  {
    /*
      By design Serial NAND asks for page address to erase block
      So for SPI NAND addr0
      [15:0] = CA:
      [21:16] = PA: PAGE ADDRESS AS PER SPEC, For Erase it should be 0
      [31:22] = BA:  BLOCK ADDRESS
    */
    *addr0 = start_block << (priv_data->col_count * 8);
  }
  else
  {
    /* We use only ADDR0 register for block addressing */
    *addr0 = start_block;
  }

  /* Clear ADDR1 */
  *addr1 = 0;
}

/*
 * Set the buffer pointers for write
 */
static void nandc_set_write_buffers(
  struct flash_hal_predef_page_data *page_data,
  struct flash_hal_page_layout *page_layout,
  struct flash_hal_op_desc *write_desc,
  struct flash_hal_ctlr_data *priv_data,
  uint32 *num_write_desc)
{
  int32 spare_len = 0, spare_copy_len = 0;
  uint32 page_unit_count = 0;
  uintnt main_addr = 0;
  uintnt spare_addr = 0;
  struct flash_hal_buffer_desc *buff_desc;
  struct flash_hal_op_desc *write_desc_ptr;
  const struct flash_hal_page_unit_layout *page_unit_layout;
  uint32 page_unit_layout_cnt = 0;
  
  /* NOTE: This code is optimized to write entire
           page data every time. This supports the below
           scenarios,
             - Write all main data bytes
             - Write all main and spare bytes
          No partial main writes are supported by this code.
          Partial spare writes are supported.
   */

  buff_desc = page_data->buff_desc;
  write_desc_ptr = write_desc;

  main_addr = (uintnt) buff_desc->buff_paddr;
  spare_addr = (uintnt) (buff_desc + 1)->buff_paddr;

  spare_len = page_data->spare_size_bytes;
  page_unit_layout = page_layout->unit_layout;
  page_unit_count = page_layout->page_unit_count;
  page_unit_layout_cnt = page_unit_layout->num_code_word;
  
  /* Set the copy command list with main/spare
     offsets and length from the page layout
     structure */
  while (page_unit_layout_cnt)
  {
    write_desc_ptr->buf_pa = main_addr;
    write_desc_ptr->buf_size = page_unit_layout->main_len_bytes;
    write_desc_ptr->flags = FLASH_BAM_DESC_FLAG_EOT; 
      
    main_addr += page_unit_layout->main_len_bytes;

    /* Copy only the required number of bytes */
    spare_copy_len =
      (spare_len > ((int32) page_unit_layout->udata_len_bytes)) ?
      page_unit_layout->udata_len_bytes : spare_len;

    if (spare_copy_len)
    {
      write_desc_ptr->flags = 0;
      write_desc_ptr++;       
      write_desc_ptr->buf_pa = spare_addr;
      write_desc_ptr->buf_size = spare_copy_len;
      write_desc_ptr->flags = FLASH_BAM_DESC_FLAG_EOT; 
    }
    
    if (spare_copy_len <  page_unit_layout->udata_len_bytes)
    {
      write_desc_ptr->flags = 0;
      write_desc_ptr++;       
      write_desc_ptr->buf_pa = 
        GET_PHYS_ADDR(nandc_predef_bam_data.reg_read_buff.spare_fill_buffer);
      write_desc_ptr->buf_size = 
        page_unit_layout->udata_len_bytes - spare_copy_len;
      write_desc_ptr->flags = FLASH_BAM_DESC_FLAG_EOT; 
    }

    if(--page_unit_layout_cnt == 0)
	{
	  page_unit_count--;
	  if (page_unit_count)
      {
        page_unit_layout++;
	    page_unit_layout_cnt = page_unit_layout->num_code_word;
      }
	}

    write_desc_ptr++;
  }

  *num_write_desc = write_desc_ptr - write_desc;
}

/*
 * Prepare the probe command pointer list
 */
void hal_nandc_bam_spi_probe_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_buffer_desc *buff_desc,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status)
{
  struct flash_hal_ctlr_data *priv_data;
  struct flash_hal_op_cmd *op_cmd;
  struct nandc_predef_cmd_list *cmd_list;
  struct flash_bam_xfer_desc_list tx_cmd_list;
  int result = 0;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  dev_info->ctlr_ops.flash_hal_configure(priv_data, status);

  /*---- Set Miscellaneous command lists -----*/
  op_cmd = &(nandc_predef_bam_data.cmd_list.misc_list[NANDC_BAM_MISC0_CMD_LIST]);
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_SPI_CFG_PADDR;
  op_cmd->data = 0;
  op_cmd++;

  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_SPI_CFG_PADDR;
  op_cmd->data = 0x1DB6C00D;
  op_cmd++;

  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_SPI_CFG_PADDR;
  op_cmd->data = 0xDB6C00D;
  op_cmd++;

  op_cmd->reg_addr = HAL_QPIC_NANDC_FLASH_DEV_CMD0_PADDR;
  op_cmd->data = 0X1080D8D8;
  op_cmd++;

  op_cmd->reg_addr = HAL_QPIC_NANDC_FLASH_DEV_CMD1_PADDR;
  op_cmd->data = 0XF00F3000;
  op_cmd++;

  op_cmd->reg_addr = HAL_QPIC_NANDC_FLASH_DEV_CMD2_PADDR;
  op_cmd->data = 0XF0FF709F;
  op_cmd++;

  op_cmd->reg_addr = HAL_QPIC_NANDC_FLASH_DEV_CMD3_PADDR;
  op_cmd->data = 0X3F310015;
  op_cmd++;

  op_cmd->reg_addr = HAL_QPIC_NANDC_FLASH_DEV_CMD7_PADDR;
  op_cmd->data = 0x04061F0F;
  op_cmd++;

  op_cmd->reg_addr = HAL_QPIC_NANDC_SPI_BUSY_CHECK_WAIT_CNT_PADDR;
  op_cmd->data = 0X010;
  op_cmd++;

  /* reset value:0x2AD40000,
      change NUM_ADDR_CYCLES to 3, SPARE_SIZE_BYTES to 0,
      DISABLE_STATUS_AFTER_WRITE to 1
      then value for dev0_cfg0 becomes 0X00540010
    NOTE: this value is different from HPG value where cw_cnt is 3
  */
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_DEV0_CFG0_PADDR;
  op_cmd->data = 0X00540010;
  op_cmd++;

  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_DEVn_CFG1_PADDR(0);
  op_cmd->data = 0X87476B1;
  op_cmd++;

  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_DEV0_ECC_CFG_PADDR;
  op_cmd->data = 0X02000001;
  op_cmd++;

#ifdef LOADER_CODE
  /* 1. Update command VLD register */
  op_cmd->reg_addr = HAL_QPIC_NANDC_FLASH_DEV_CMD_VLD_PADDR;
  op_cmd->data = priv_data->dev_vld;
  op_cmd++;
#endif

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
  /* Set the FLASH_FEATURE to remove BP */
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_FEATURES_PADDR;
  op_cmd->data = FLASH_SPI_NAND_SR1_DATA;
  op_cmd++;

  /* Set addr 0xA0 for BP for set features */
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_ADDR0_PADDR;
  op_cmd->data = FLASH_SPI_NAND_SR1_ADDR;
  op_cmd++;

  /* Set command registers for set features
     This same entry is utilized by both disable WP and disable ECC sequences
  */
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_CMD_PADDR;
  op_cmd->data = 0xB800000E;
  op_cmd++;

  /* Set 0x8 in FLASH_FEATURE to disable ecc in NAND */
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_FEATURES_PADDR;
  op_cmd->data = FLASH_SPI_NAND_SR2_DATA;
  op_cmd++;

  /*
    Set addr 0xb0 for the FLASH_FEATURE to disable ecc in NAND
    and set BUF mode to 1 i.e. continuous read mode
  */
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_ADDR0_PADDR;
  op_cmd->data = FLASH_SPI_NAND_SR2_ADDR;
  op_cmd++;

  /* Set command registers to fetch ID */
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_CMD_PADDR;
  op_cmd->data = SPI_MODE_ENABLE_FOR_NON_RW_CMD | FLASH_NAND_ID_FETCH_CMD;
  op_cmd++;

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

  /*---- Set Command registers -----*/
  hal_nandc_bam_set_cmd(FLASH_NAND_RESET_FLASH_CMD, NANDC_NON_PAGE_CMD, priv_data);

  /*----- Configure read ID copy -----*/
  cmd_list = &nandc_predef_bam_data.cmd_list;
  op_cmd = &(cmd_list->copyid_list[0]);

  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_READ_ID_PADDR;
  op_cmd->data = (uintnt) buff_desc->buff_paddr;

  tx_cmd_list.desc_list_ptr = (bam_iovec_type *)
    nandc_predef_bam_data.cmd_ptr_list[NANDC_FF_90_PROBE].op_list;
  tx_cmd_list.desc_count = 
    nandc_predef_bam_data.cmd_ptr_list[NANDC_FF_90_PROBE].op_list_count;
 
  result = hal_nandc_bam_exec_op(&tx_cmd_list, NULL, NULL, NULL);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_spi_probe_pprocess(priv_data, *op_list, status);
  }
}

/*
 * Prepare the probe command pointer list
 */
void hal_nandc_bam_FF_90_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_buffer_desc *buff_desc,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status)
{
  struct flash_hal_ctlr_data *priv_data;
  struct flash_hal_op_cmd *op_cmd;
  struct nandc_predef_cmd_list *cmd_list;
  uint32 *addr0, *addr1;
  struct flash_bam_xfer_desc_list tx_cmd_list;
  int result = 0;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  dev_info->ctlr_ops.flash_hal_configure(priv_data, status);

  /* Set address bytes */
  op_cmd = nandc_predef_bam_data.cmd_list.addr_list;
  addr0 = &(op_cmd->data);
  op_cmd++;
  addr1 = &(op_cmd->data);

  hal_nandc_bam_set_addr(priv_data, 0, addr0, addr1, 1);

  /*---- Set config registers -----*/

  FLASH_CW_PER_PAGE(priv_data, 0);
  FLASH_UD_SIZE(priv_data, 0);
  FLASH_NUM_ADDR_CYCLES(priv_data, 1);

  hal_nandc_bam_set_cfg(priv_data);

  /*---- Set Command registers -----*/
  hal_nandc_bam_set_cmd(FLASH_NAND_RESET_FLASH_CMD, NANDC_NON_PAGE_CMD, priv_data);

  /*---- Set Miscellaneous command lists -----*/
  op_cmd = &(nandc_predef_bam_data.cmd_list.misc_list[NANDC_BAM_MISC0_CMD_LIST]);

#ifdef LOADER_CODE
  /* 1. Update command VLD register */
  op_cmd->reg_addr = HAL_QPIC_NANDC_FLASH_DEV_CMD_VLD_PADDR;
  op_cmd->data = priv_data->dev_vld;

  /* 2. Change the command register */
  (++op_cmd)->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_CMD_PADDR;
#else
  /* 2. Change the command register */
   op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_CMD_PADDR;
#endif
   op_cmd->data = FLASH_NAND_ID_FETCH_CMD;

  /* 3. Exec register */
  (++op_cmd)->reg_addr = HAL_QPIC_NANDC_NANDC_EXEC_CMD_PADDR;
  op_cmd->data = 1;

  /*----- Configure read ID copy -----*/
  cmd_list = &nandc_predef_bam_data.cmd_list;
  op_cmd = &(cmd_list->copyid_list[0]);

  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_READ_ID_PADDR;
  op_cmd->data = (uintnt) buff_desc->buff_paddr;

  tx_cmd_list.desc_list_ptr = (bam_iovec_type *)
    nandc_predef_bam_data.cmd_ptr_list[NANDC_FF_90_PROBE].op_list;
  tx_cmd_list.desc_count = 
    nandc_predef_bam_data.cmd_ptr_list[NANDC_FF_90_PROBE].op_list_count;
 
  result = hal_nandc_bam_exec_op(&tx_cmd_list, NULL, NULL,NULL);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_FF_90_pprocess(priv_data, *op_list, status);
  }
}

/*
 * Prepare the probe command pointer list
 */
void hal_nandc_bam_probe_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_buffer_desc *buff_desc,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status)
{
  struct flash_hal_ctlr_data *priv_data;
  priv_data = &dev_info->config;
  if(priv_data->dev_type == FLASH_HAL_DEVICE_NAND_SPI)
  {
    hal_nandc_bam_spi_probe_exec(dev_info,  buff_desc,  op_list, status);
  }
  else
  {
    hal_nandc_bam_FF_90_exec(dev_info,  buff_desc,  op_list, status);
  }
}

/*
 * Prepare the probe command pointer list
 */
void hal_nandc_bam_90_onfi_id_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_onfi_data *probe_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status)
{
  uint32 addr0_val =0, addr1_val = 0;
  struct flash_hal_ctlr_data *priv_data;
  struct flash_hal_op_cmd *op_cmd;
  struct nandc_predef_cmd_list *cmd_list;
  uint32 *addr0, *addr1;
  struct flash_bam_xfer_desc_list tx_cmd_list;
  int result = 0;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  dev_info->ctlr_ops.flash_hal_configure(priv_data, status);

  /* Set address buffers */
  addr0_val = probe_data->col_offset;
  addr0_val |= probe_data->page << (priv_data->col_count * 8);
  addr1_val = probe_data->page >> ((4 - priv_data->col_count) * 8);

  /* Set address bytes */
  op_cmd = nandc_predef_bam_data.cmd_list.addr_list;
  addr0 = &(op_cmd->data);
  op_cmd++;
  addr1 = &(op_cmd->data);

  *addr0 = addr0_val;
  *addr1 = addr1_val;

  /*---- Set config registers -----*/

  FLASH_CW_PER_PAGE(priv_data, 0);
  FLASH_UD_SIZE(priv_data, 0);
  FLASH_NUM_ADDR_CYCLES(priv_data, probe_data->addr_cycles);

  hal_nandc_bam_set_cfg(priv_data);

  /*---- Set Command registers -----*/
  hal_nandc_bam_set_cmd(FLASH_NAND_ID_FETCH_CMD, NANDC_NON_PAGE_CMD, priv_data);

  /*----- Configure read ID copy -----*/
  cmd_list = &nandc_predef_bam_data.cmd_list;
  op_cmd = &(cmd_list->copyid_list[0]);

  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_READ_ID_PADDR;
  op_cmd->data = (uintnt) probe_data->buff_desc.buff_paddr;

  tx_cmd_list.desc_list_ptr = (bam_iovec_type *)
    nandc_predef_bam_data.cmd_ptr_list[NANDC_90_ONFI_ID].op_list;
  tx_cmd_list.desc_count =
    nandc_predef_bam_data.cmd_ptr_list[NANDC_90_ONFI_ID].op_list_count;

  result = hal_nandc_bam_exec_op(&tx_cmd_list, NULL, NULL, NULL);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_90_onfi_id_pprocess(priv_data, *op_list, status);
  }
}

/*
 * Prepare the parameter page read command pointer list
 */
void hal_nandc_bam_00_EC_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_onfi_data *page_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status)
{
  uint32 addr0_val =0, addr1_val = 0;
  int32 cw_count = 0;
  struct flash_hal_op_cmd *op_cmd;
  struct flash_hal_op_desc *read_desc, *read_desc_ptr;
  struct flash_hal_ctlr_data *priv_data;
  struct flash_bam_xfer_desc_list tx_cmd_list, tx_read_list;
  uint32 *addr0, *addr1;
  uintnt main_addr = 0;
  uint32 cw_offset=528;
  int result;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  dev_info->ctlr_ops.flash_hal_configure(priv_data, status);

  /* Set address bytes */
  addr0_val = page_data->col_offset;
  addr0_val |= page_data->page << (priv_data->col_count * 8);
  addr1_val = page_data->page >> ((4 - priv_data->col_count) * 8);

  /* Set address bytes */
  op_cmd = nandc_predef_bam_data.cmd_list.addr_list;
  addr0 = &(op_cmd->data);
  op_cmd++;
  addr1 = &(op_cmd->data);

  *addr0 = addr0_val;
  *addr1 = addr1_val;

  /*---- Set config registers -----*/

  /* Set the codeword count */
  cw_count = priv_data->cw_cnt - 1;
  FLASH_CW_PER_PAGE(priv_data, cw_count);

  /* Set the number of address cycles */
  FLASH_NUM_ADDR_CYCLES(priv_data, page_data->addr_cycles);

  hal_nandc_bam_set_cfg(priv_data);

  /* Set command registers */
  hal_nandc_bam_set_cmd(FLASH_NAND_PARAMETER_PAGE_READ_CMD, NANDC_NON_PAGE_CMD, priv_data);

  /* Config Locate Register for Data Read */
  op_cmd = nandc_predef_bam_data.cmd_list.locate_list;
  op_cmd->data = FLASH_NAND_READ_LOCATION_n_LAST | 
      (cw_offset << FLASH_NAND_READ_LOCATION_n_SIZE_SHIFT);

  if(priv_data->qpic_major_version >= FLASH_HAL_QPIC_CTRL_VERSION_2_0)
  { 
    op_cmd++;
    /* Config Locate Register Last CW for Data Read */
    op_cmd->data = FLASH_NAND_READ_LOCATION_LAST_CW_n_LAST | 
      (cw_offset << FLASH_NAND_READ_LOCATION_LAST_CW_n_SIZE_SHIFT);
  }

  /* Data Descriptor list */
  read_desc =  nandc_predef_bam_data.desc.read_copy_desc;
  read_desc_ptr = read_desc;

  main_addr = (uintnt) page_data->buff_desc.buff_paddr;

  while (cw_count >= 0)
  {
    read_desc_ptr->buf_pa = main_addr;
    read_desc_ptr->buf_size = cw_offset;
    read_desc_ptr->flags = FLASH_BAM_DESC_FLAG_INT;

    main_addr += cw_offset;

    read_desc_ptr++;
    cw_count--;
  }

  /* Command descriptor list */
  tx_cmd_list.desc_list_ptr = (bam_iovec_type *)
    nandc_predef_bam_data.cmd_ptr_list[NANDC_00_EC_READ].op_list;
  tx_cmd_list.desc_count =
    nandc_predef_bam_data.cmd_ptr_list[NANDC_00_EC_READ].op_list_count;

  tx_read_list.desc_list_ptr = (bam_iovec_type *)read_desc;
  tx_read_list.desc_count = read_desc_ptr - read_desc;

  result = hal_nandc_bam_exec_op(&tx_cmd_list, &tx_read_list, NULL, NULL);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_00_EC_pprocess(priv_data, *op_list, status);
  }
}

#ifdef LOADER_CODE
/*
 * Prepare the Transfer Steps config command pointer list
 */
void hal_nandc_set_xfer_cfg_exec(flash_hal_dev_info *dev_info, 
  struct flash_hal_xfer_cfg *cfg_data, enum flash_hal_status *status)
{
  struct flash_hal_op_cmd *op_cmd;
  struct flash_bam_xfer_desc_list tx_cmd_list;
  int result = 0;
  struct flash_hal_ctlr_data *priv_data;

  *status = FLASH_HAL_FAIL;
  priv_data = &dev_info->config;

  /*
    According to HPG Setting Xfer steps and spi_num_addr_cycles
    is part of initialization flow before reset.
    However these values differ from NAND part to part.
    sitting in QPIC layer we won't know which NAND we don't
    know which NAND is connected.
    So we are not following HPG init sequence.
    Instead we reset and read id of NAND in probe,
    then based on NAND ID we get Xfer steps and spi_num_addr_cycles
    and configure them in this function.
    Since Xfer steps and spi_num_addr_cycles are required for read/write/erase
    functionality this function should be called immidiately after probe
    and before any other call to nand.
  */
  /* set QPIC_SPI_NUM_ADDR_CYCLES */
  op_cmd = &(nandc_predef_bam_data.cmd_list.misc_list[0]);
  op_cmd->reg_addr = HAL_QPIC_NANDC_SPI_NUM_ADDR_CYCLES_PADDR;
  op_cmd->data = priv_data->spi_num_addr_cycles;

  /*---- Set Transfer Steps Register data -----*/
  op_cmd = &(nandc_predef_bam_data.cmd_list.xfer_steps_list[0]);

  if((cfg_data != NULL) && 
    (cfg_data->xfer_reg_count == NAND_MAX_XFER_STEPS_CNT))
  {
    uint32 *xfer_data = NULL;
    uintnt i = 0;

	xfer_data = cfg_data->xfer_reg_data;
    for ( i = 0; i < cfg_data->xfer_reg_count; i++)
    {
      op_cmd->data = *xfer_data++;
	  op_cmd++;
    }

    /* Set the transfer boot mode bit */
    if (cfg_data->xfer_boost_mode & 0x1)
    {
      op_cmd->data = (1 << FLASH_NAND_CTRL_BOOST_MODE_SHFT) \
                     | FLASH_NAND_CTRL_BAM_MODE_EN;
    }
    else
    {
      op_cmd->data = FLASH_NAND_CTRL_BAM_MODE_EN;
    }
    /* 416: This Value is Calculated as Per HW CR (QCTDD01075436) */
    op_cmd->data |= (416<<FLASH_NAND_CTRL_RD_DELAY_CNTR_VAL_SHFT);

    tx_cmd_list.desc_list_ptr = (bam_iovec_type *)
      nandc_predef_bam_data.cmd_ptr_list[NANDC_XFER_STEPS_CFG].op_list;
    tx_cmd_list.desc_count = 
      nandc_predef_bam_data.cmd_ptr_list[NANDC_XFER_STEPS_CFG].op_list_count;
 
    result = hal_nandc_bam_exec_op(&tx_cmd_list, NULL, NULL, NULL);

    if (result == FLASH_BAM_SUCCESS)
    {
      *status = FLASH_HAL_SUCCESS;
    }
  }
}
#endif

/*
 * Prepare the read command pointer list
 */
void hal_nandc_bam_00_30_x1_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_page_data *page_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status)
{
  uint32 cmd = 0, cw_count = 0;
  struct flash_hal_page_layout *page_layout;
  struct flash_hal_ctlr_data *priv_data;
  struct flash_hal_op_cmd *op_cmd;
  uint32 *addr0 = NULL, *addr1 = NULL;
  uint32 num_read_desc = 0;
  struct flash_bam_xfer_desc_list tx_cmd_list;
  struct flash_bam_xfer_desc_list tx_read_list;
  struct flash_hal_predef_op_list *cmd_op_list;
  int result;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  if (1 != page_data->total_pages)
  {
    *status = FLASH_HAL_FAIL;
    return;
  }

  cmd_op_list = &nandc_predef_bam_data.cmd_ptr_list[NANDC_00_30_x1_READ];
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
    in current QPIC version (2_0_1) so we use FLASH_NAND_PAGE_READ_CMD(0x32) for all */
    cmd = (priv_data->ecc_state == FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE)?
      FLASH_NAND_PAGE_READ_ECC_CMD:FLASH_NAND_PAGE_READ_CMD;
  }
  else
  {
    cmd = (priv_data->ecc_state == FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE)?
      FLASH_NAND_PAGE_READ_ECC_CMD:FLASH_NAND_PAGE_READ_ALL_CMD;
  }

  hal_nandc_bam_set_cmd(cmd, NANDC_PAGE_READ_CMD, priv_data);

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

  result = hal_nandc_bam_exec_op(&tx_cmd_list, &tx_read_list, NULL, NULL);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_00_30_x1_pprocess(priv_data, cmd_op_list, status);
  }
}

/*
 * Queue request to read a single page
 */
void hal_nandc_bam_start_page_read_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_page_data *page_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status)
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

  if (1 != page_data->total_pages)
  {
    *status = FLASH_HAL_FAIL;
    return;
  }

  cmd_op_list = &nandc_predef_bam_data.cmd_ptr_list[NANDC_00_30_x1_READ];
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
    in current QPIC version (2_0_1) so we use FLASH_NAND_PAGE_READ_CMD(0x32) for all */
    cmd = (priv_data->ecc_state == FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE)?
      FLASH_NAND_PAGE_READ_ECC_CMD:FLASH_NAND_PAGE_READ_CMD;
  }
  else
  {
    cmd = (priv_data->ecc_state == FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE)?
      FLASH_NAND_PAGE_READ_ECC_CMD:FLASH_NAND_PAGE_READ_ALL_CMD;
  }

  hal_nandc_bam_set_cmd(cmd, NANDC_PAGE_READ_CMD, priv_data);

  /* Configure buffer pointers */
  hal_nandc_bam_set_read_buffers(page_data, page_layout, 
    nandc_predef_bam_data.desc.read_copy_desc, priv_data, &num_read_desc);

  /* Command descriptor list */
  flash_bam_queued_tx_cmd_list.desc_list_ptr = (bam_iovec_type *)cmd_op_list->op_list;
  flash_bam_queued_tx_cmd_list.desc_count = cmd_op_list->op_list_count;
  
  /* Data Descriptor list */
  flash_bam_queued_tx_read_list.desc_list_ptr = (bam_iovec_type *)
    nandc_predef_bam_data.desc.read_copy_desc;
  flash_bam_queued_tx_read_list.desc_count = num_read_desc;

  result = flash_bam_start_exec_op(&flash_bam_queued_tx_cmd_list,
                              &flash_bam_queued_tx_read_list,
                              NULL, NULL);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
}

/*
 * wait for previously queued read page request to complete
 */
void hal_nandc_bam_wait_for_page_read_exec(flash_hal_dev_info *dev_info,
 enum flash_hal_status  *status)
{
  struct flash_hal_ctlr_data *priv_data;
  struct flash_hal_predef_op_list *cmd_op_list;
  int result;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  cmd_op_list = &nandc_predef_bam_data.cmd_ptr_list[NANDC_00_30_x1_READ];

  result = flash_bam_wait_for_exec_op(
                &flash_bam_queued_tx_cmd_list,
                &flash_bam_queued_tx_read_list,
                NULL, NULL);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_00_30_x1_pprocess(priv_data, cmd_op_list, status);
  }
}

/*
 * Prepare the write command pointer list
 */
void hal_nandc_bam_80_10_x1_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_page_data *page_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status)
{
  uint32 cmd = 0, cw_count = 0;
  struct flash_hal_page_layout *page_layout;
  struct flash_hal_ctlr_data *priv_data;
  struct flash_hal_op_cmd *op_cmd;
  uint32 *addr0, *addr1;
  uint32 num_write_desc = 0;
  struct flash_bam_xfer_desc_list tx_cmd_list;
  struct flash_bam_xfer_desc_list tx_write_list;
  struct flash_hal_predef_op_list *cmd_op_list;
  int result;

  *status = FLASH_HAL_SUCCESS;
  cmd_op_list =  &nandc_predef_bam_data.cmd_ptr_list[NANDC_80_10_x1_WRITE];

  priv_data = &dev_info->config;

  if (page_data->total_pages != 1)
  {
    *status = FLASH_HAL_FAIL;
    return;
  }

  dev_info->ctlr_ops.flash_hal_configure(priv_data, status);

  /* Get page layout */
  dev_info->ctlr_ops.flash_hal_get_page_layout(priv_data, &page_layout);

   /* Set address bytes */
  op_cmd = &nandc_predef_bam_data.cmd_list.wcfg_list[4];
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
    /* FLASH_NAND_PRG_PAGE_ALL_CMD(0x39) does not support ECC disable mode
    in current QPIC version (2_0_1) so we use PRG_PAGE_CMD(0x36) instead */
    cmd = FLASH_NAND_PRG_PAGE_CMD;
  }
  else
  {
    cmd = (FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE == priv_data->ecc_state)?
      FLASH_NAND_PRG_PAGE_CMD:FLASH_NAND_PRG_PAGE_ALL_CMD;
  }

  hal_nandc_bam_set_cmd(cmd, NANDC_PAGE_WRITE_CMD, priv_data);

  nandc_set_write_buffers(page_data, page_layout, 
    nandc_predef_bam_data.desc.write_copy_desc, priv_data, &num_write_desc);

  /* Command descriptor list */
  tx_cmd_list.desc_list_ptr = (bam_iovec_type *)cmd_op_list->op_list;
  tx_cmd_list.desc_count = cmd_op_list->op_list_count;
  
  /* Data Descriptor list */
  tx_write_list.desc_list_ptr = (bam_iovec_type *)
    nandc_predef_bam_data.desc.write_copy_desc;
  tx_write_list.desc_count = num_write_desc;

  result = hal_nandc_bam_exec_op(&tx_cmd_list, NULL, &tx_write_list,NULL);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_80_10_x1_pprocess(priv_data, *op_list, status);
  }
}


/*
 * Prepare the erase command pointer list
 */
void hal_nandc_bam_60_D0_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_block_data *block_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status)
{
  struct flash_hal_ctlr_data *priv_data;
  uint32 *addr0, *addr1;
  struct flash_hal_op_cmd *op_cmd;
  struct flash_bam_xfer_desc_list tx_cmd_list;
  struct flash_hal_predef_op_list *cmd_op_list;
  int result;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;
  cmd_op_list =  &nandc_predef_bam_data.cmd_ptr_list[NANDC_60_D0_ERASE];

  dev_info->ctlr_ops.flash_hal_configure(priv_data, status);

  /* Set address bytes */
  op_cmd = nandc_predef_bam_data.cmd_list.addr_list;
  addr0 = &(op_cmd->data);
  op_cmd++;
  addr1 = &(op_cmd->data);

  /* Set address bytes */
  nandc_set_block_addr(priv_data, addr0, addr1, block_data->start_addr,
    block_data->pages_per_block, block_data->num_blocks);

  /* Set the codeword count and UD size to zero */
  FLASH_UD_SIZE(priv_data, 0);
  FLASH_CW_PER_PAGE(priv_data, 0);

  /* Set the number of address cycles */
  FLASH_NUM_ADDR_CYCLES(priv_data, priv_data->row_count);

  /* Set config */
  hal_nandc_bam_set_cfg(priv_data);

  /* Set erase command */
  hal_nandc_bam_set_cmd(FLASH_NAND_BLOCK_ERASE_CMD, NANDC_NON_PAGE_CMD, priv_data);

  /* Command descriptor list */
  tx_cmd_list.desc_list_ptr = (bam_iovec_type *)cmd_op_list->op_list;
  tx_cmd_list.desc_count = cmd_op_list->op_list_count;

  result = hal_nandc_bam_exec_op(&tx_cmd_list, NULL, NULL, NULL);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_60_D0_pprocess(priv_data, cmd_op_list, status);
  }
}

#ifdef LOADER_CODE
/*
 * Prepare the Set/Get Feature command pointer list
 */
void hal_nandc_bam_EF_EE_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_feat_data *feat_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status)
{
  struct flash_hal_ctlr_data *priv_data;
  uint32 *addr0, *addr1;
  uint32 main_addr = 0;
  struct flash_hal_op_cmd *op_cmd;
  struct flash_hal_op_desc *rd_wr_desc, *rd_wr_desc_ptr;
  struct flash_bam_xfer_desc_list tx_cmd_list;
  struct flash_bam_xfer_desc_list tx_read_write_list;
  struct flash_hal_predef_op_list *cmd_op_list;
  int result = FLASH_BAM_FAIL;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;
  cmd_op_list =  &nandc_predef_bam_data.cmd_ptr_list[NANDC_EF_EE_FEATURE];

  dev_info->ctlr_ops.flash_hal_configure(priv_data, status);

  /* Set address bytes */
  op_cmd = nandc_predef_bam_data.cmd_list.addr_list;
  addr0 = &(op_cmd->data);
  op_cmd++;
  addr1 = &(op_cmd->data);

  /* Set address bytes */
  nandc_set_block_addr(priv_data, addr0, addr1, feat_data->feat_addr,
    0, 0);

  /* Set the codeword count to zero and UD size to 4 bytes */
  FLASH_CW_PER_PAGE(priv_data, 0);
  FLASH_UD_SIZE(priv_data, 4);
  FLASH_NUM_ADDR_CYCLES(priv_data, 1);

  /* Set config */
  hal_nandc_bam_set_cfg(priv_data);

  /*---- Set Miscellaneous command lists -----*/
  op_cmd = &(nandc_predef_bam_data.cmd_list.misc_list[NANDC_BAM_MISC0_CMD_LIST]);
  
  /* 1. Update command VLD register */
  op_cmd->reg_addr = HAL_QPIC_NANDC_FLASH_DEV_CMD_VLD_PADDR;
  op_cmd->data = 0xEC000000;
  op_cmd++;
  /* 2. Update command CMD0 register*/
  op_cmd->reg_addr = HAL_QPIC_NANDC_FLASH_DEV_CMD0_PADDR;
  op_cmd->data = FLASH_DEV_CMD0_SET_FEATURE_CMD; /* Set Feature */
  op_cmd++;
  /* 3. Update command CMD1 register*/
  op_cmd->reg_addr = HAL_QPIC_NANDC_FLASH_DEV_CMD1_PADDR;
  op_cmd->data = FLASH_DEV_CMD1_GET_FEATURE_CMD; /* Get Feature */
  op_cmd++;
  /* 4. Restore command VLD register */
  op_cmd->reg_addr = HAL_QPIC_NANDC_FLASH_DEV_CMD_VLD_PADDR;
  op_cmd->data = priv_data->dev_vld;
  op_cmd++;
  /* 5. Restore command CMD0 register*/
  op_cmd->reg_addr = HAL_QPIC_NANDC_FLASH_DEV_CMD0_PADDR;
  op_cmd->data = FLASH_DEV_CMD0_DEFAULT_CMD; /* Page Write */
  op_cmd++;
  /* 6. Restore command CMD1 register*/
  op_cmd->reg_addr = HAL_QPIC_NANDC_FLASH_DEV_CMD1_PADDR;
  op_cmd->data = FLASH_DEV_CMD1_DEFAULT_CMD; /*  Page Read */
   
  /* Command descriptor list */
  tx_cmd_list.desc_list_ptr = (bam_iovec_type *)cmd_op_list->op_list;
  tx_cmd_list.desc_count = cmd_op_list->op_list_count;

  /* Data Descriptor list */
  if (feat_data->cmd_id == FLASH_HAL_FEATURE_GET_CMD)
  {
    /* Config Locate Register for Data Read */
    op_cmd = nandc_predef_bam_data.cmd_list.locate_list;
    op_cmd->data = FLASH_NAND_READ_LOCATION_n_LAST | 
      (4 << FLASH_NAND_READ_LOCATION_n_SIZE_SHIFT);
	  
    if(priv_data->qpic_major_version >= FLASH_HAL_QPIC_CTRL_VERSION_2_0)
    {
      op_cmd++;
      /* Config Locate Register Last CW for Data Read */
      op_cmd->data = FLASH_NAND_READ_LOCATION_LAST_CW_n_LAST | 
      (4 << FLASH_NAND_READ_LOCATION_LAST_CW_n_SIZE_SHIFT);
    }
	  
    /* Set read command for Get Feature*/
    hal_nandc_bam_set_cmd(FLASH_NAND_PAGE_READ_CMD, NANDC_NON_PAGE_CMD, priv_data);

    rd_wr_desc =  nandc_predef_bam_data.desc.read_copy_desc;
    rd_wr_desc_ptr = rd_wr_desc;
    main_addr = (uint32) feat_data->feat_buff_addr;

    rd_wr_desc_ptr->buf_pa = main_addr;
    rd_wr_desc_ptr->buf_size = 4;
    rd_wr_desc_ptr->flags = FLASH_BAM_DESC_FLAG_INT;

    tx_read_write_list.desc_list_ptr = (bam_iovec_type *)rd_wr_desc;
    tx_read_write_list.desc_count = 1;
	
    result = hal_nandc_bam_exec_op(&tx_cmd_list, &tx_read_write_list, NULL,NULL);
  }
  else if (feat_data->cmd_id == FLASH_HAL_FEATURE_SET_CMD)
  {
    /* Config Locate Register not needed for set feature, set to 0 read bytes */
    op_cmd = nandc_predef_bam_data.cmd_list.locate_list;
    op_cmd->data = FLASH_NAND_READ_LOCATION_n_LAST | 
      (0 << FLASH_NAND_READ_LOCATION_n_SIZE_SHIFT);
	
    if(priv_data->qpic_major_version >= FLASH_HAL_QPIC_CTRL_VERSION_2_0)
    {
	  op_cmd++;
      /* Config Locate Register Last CW for Data Read */
      op_cmd->data = FLASH_NAND_READ_LOCATION_LAST_CW_n_LAST | 
      (0 << FLASH_NAND_READ_LOCATION_LAST_CW_n_SIZE_SHIFT);
  	}
	
    /* Set write command for Set Feature*/
    hal_nandc_bam_set_cmd(FLASH_NAND_PRG_PAGE_CMD, NANDC_NON_PAGE_CMD, priv_data);
	
    rd_wr_desc =  nandc_predef_bam_data.desc.write_copy_desc;
    rd_wr_desc_ptr = rd_wr_desc;
    main_addr = (uint32) feat_data->feat_buff_addr;

    rd_wr_desc_ptr->buf_pa = main_addr;
    rd_wr_desc_ptr->buf_size = 4;
    rd_wr_desc_ptr->flags = FLASH_BAM_DESC_FLAG_EOT;
	
    tx_read_write_list.desc_list_ptr = (bam_iovec_type *)rd_wr_desc;
    tx_read_write_list.desc_count = 1;

    result = hal_nandc_bam_exec_op(&tx_cmd_list, NULL, &tx_read_write_list, NULL);
  }

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_EF_EE_pprocess(priv_data, cmd_op_list, status);
  }
}
#endif
/*
 * Prepare the read bad block marker command pointer list
 */
void hal_nandc_bam_00_30_bb_marker_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_spare_data *page_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status)
{
  uintnt spare_addr = 0;
  uint32 addr0_val =0, addr1_val = 0;
  struct flash_hal_op_cmd *op_cmd;
  struct flash_hal_op_desc *read_desc;
  struct flash_hal_ctlr_data *priv_data;
  struct flash_bam_xfer_desc_list tx_cmd_list;
  struct flash_bam_xfer_desc_list tx_read_list;
  volatile uint32 *addr0, *addr1;
  struct flash_hal_predef_op_list *cmd_op_list;
  int result;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  dev_info->ctlr_ops.flash_hal_configure(priv_data, status);

  cmd_op_list = &nandc_predef_bam_data.cmd_ptr_list[NANDC_00_30_BB_READ];
  /* Set main and spare buffer pointers */
  spare_addr = (uintnt) page_data->buff_desc.buff_paddr;

  /* Set address buffers */
  addr0_val = page_data->col_offset;
  addr0_val |= page_data->page << (priv_data->col_count * 8);
  addr1_val = page_data->page >> ((4 - priv_data->col_count) * 8);

  op_cmd = nandc_predef_bam_data.cmd_list.addr_list;
  addr0 = &(op_cmd->data);
  op_cmd++;
  addr1 = &(op_cmd->data);

  *addr0 = addr0_val;
  *addr1 = addr1_val;

  /*---- Set config registers -----*/
  FLASH_DISABLE_BCH_ECC(priv_data);

  FLASH_CW_PER_PAGE(priv_data, 0);
  FLASH_UD_SIZE(priv_data, page_data->spare_size_bytes);
  FLASH_SPARE_SIZE_BYTES(priv_data, 0);
  FLASH_BAD_BLOCK_IN_SPARE_AREA(priv_data);
  FLASH_NUM_ADDR_CYCLES(priv_data, (priv_data->row_count+priv_data->col_count));

  hal_nandc_bam_set_cfg(priv_data);

  /* Set command registers */
  hal_nandc_bam_set_cmd(FLASH_NAND_PAGE_READ_CMD, NANDC_NON_PAGE_CMD, priv_data);

  /* Config Locate Register for Data Read */
  op_cmd = nandc_predef_bam_data.cmd_list.locate_list;

  /* Config Locate Register */
  op_cmd->data = FLASH_NAND_READ_LOCATION_n_LAST;
  op_cmd->data |= page_data->spare_size_bytes << FLASH_NAND_READ_LOCATION_n_SIZE_SHIFT;

  if(priv_data->qpic_major_version >= FLASH_HAL_QPIC_CTRL_VERSION_2_0)
  {
    /* Point to Locate Last CW Register for Data Read */
    op_cmd++;	
    /* Config Locate Register */
    op_cmd->data = FLASH_NAND_READ_LOCATION_LAST_CW_n_LAST;
    op_cmd->data |= page_data->spare_size_bytes << FLASH_NAND_READ_LOCATION_LAST_CW_n_SIZE_SHIFT;
  }

    /* Data Descriptor list */
  read_desc =  nandc_predef_bam_data.desc.read_copy_desc;

  read_desc->buf_pa = spare_addr;
  read_desc->buf_size =  page_data->spare_size_bytes;
  read_desc->flags =  BAM_IOVEC_FLAG_INT;

  tx_read_list.desc_list_ptr = (bam_iovec_type *)read_desc;
  tx_read_list.desc_count = 1;

  /* Command descriptor list */
  tx_cmd_list.desc_list_ptr = (bam_iovec_type *)cmd_op_list->op_list;
  tx_cmd_list.desc_count = cmd_op_list->op_list_count;

  result = hal_nandc_bam_exec_op(&tx_cmd_list, &tx_read_list, NULL,NULL);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_00_30_bb_marker_pprocess(priv_data, *op_list, status);
  }
}
