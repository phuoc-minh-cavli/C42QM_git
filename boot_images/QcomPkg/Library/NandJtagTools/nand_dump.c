/*=======================================================================
 * FILE:        nand_dump.c
 *
 * SERVICES:    Functions to dump file system area
 *
 * DESCRIPTION
 *  These functions dump the named partition and copy the data back to the
 *  T32 debugger. There is also support to dump a MIBIB image.
 *
 * Copyright (c) 2008-2010, 2012, 2014, 2019 QUALCOMM Technologies Incorporated.
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
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 06/11/19     sl      Added support for multipage read.
 * 04/16/19     sl      commented info prints to speed up dump script
 * 07/14/14     sb      Move page buffers to Code RAM to support non DDR based tools
 * 09/11/12     eo      Remove miboot.h
 * 09/10/10     bb      Fix for unaligned memcpy failure with RVCT 4.0
 * 08/12/10     nr      Moved nand_log.h reference to nand_wrapper.h
 * 11/03/09     wt      Included header file for the definition of dump_device_info
 * 09/18/09     op      Make DM buffer 32byte alignment to avoid any misaligned
 * 06/18/09     bb      Include bad block data during partition dump in RAW mode
 * 04/17/09     eo      Fix partition block limit check in dump partition.
 * 02/25/09     eo      Cleanup mibib_page_buffer initialization.
 * 02/20/09     jz      Use FLASH_PROG_SUCCESS/FAIL return codes for nand_tools
 * 10/29/08     mm      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "DALDeviceId.h"
#include "flash_miparti.h"

#include "flash_prog.h"
#include "nand_config.h"
#include "nand_param.h"
#include "nand_wrapper.h"
#include "nand_core.h"
#include "nand_tools.h"
#include "nand_dump.h"

/* TODO: Utility routine includes */
#include <string.h>
#include "stdio.h"

#define NAND_DUMP_VERSION 1
enum nand_dump_ecc_mode
{
  NAND_DUMP_ECC_DISABLED = 0,
  NAND_DUMP_ECC_ENABLED
};
enum nand_dump_bad_block_mode
{
  NAND_DUMP_INCLUDE_BAD_BLOCK = 0,
  NAND_DUMP_SKIP_BAD_BLOCK
};
enum nand_dump_spare_mode
{
  NAND_DUMP_INCLUDE_SPARE_DATA = 0,
  NAND_DUMP_SKIP_SPARE_DATA
};
#define NAND_DUMP_HEADER_SIZE        0x200
#define NAND_DUMP_BAD_BLOCK_PATTERN  0xBDBDBDBD
#define NAND_DUMP_READ_ERROR_PATTERN 0xFAFAFAFA
#define NAND_DUMP_PARTI_END_PATTERN  0xEDEDEDED
#define NAND_DUMP_ERASED_PAGE_PATTERN  0xEEEEEEEE
struct nand_dump_header_info
{
  uint32 dump_version;
  uint32 ecc_enabled;
  uint32 dump_page_size;
  uint32 skip_bad_block;
  uint32 skip_spare_data;
  uint32 bad_block_pattern;
  uint32 read_error_pattern;
  uint32 erased_page_pattern;
  uint32 partition_end_pattern;
  char parti_name[FLASH_PART_NAME_LENGTH];
  struct flash_info flash_info;
};
struct nand_dump_header_info nand_dump_header;
uint32 flash_prog_dump_parti_new(void);
uint32 flash_prog_init_parti_dump_new(void);
uint32 flash_prog_dump_read_page(uint32 page, uint32 page_num, void *main_data,
  void *spare_data);
/*===========================================================================
                           LOCAL DATA
===========================================================================*/

struct dump_device_info dump_dev_info_param;

struct dump_device_info *dump_dev = &dump_dev_info_param;

/* If flash tools get compiles as part of bootloaders, where the data caches is
 * setup, the 32 bytes aligment changes of source buffers here would make them
 * more cache line friendly and avoid any misaligned boot data structure that
 * gets invalidated
 */
/* Buffer used in generating the MIBIB image */
extern unsigned char mibib_pg_buf[MAX_PAGE_SIZE];

/* Security mode required in generation of the MIBIB signature */
static uint32 sec_mode = 0;

void flash_prog_dump_set_pattern_32(void *data, uint32 size_in_bytes, uint32 pattern)
{
   uint32 i = 0, size_in_dword;
   uint32 *data_32;
   data_32 = (uint32 *) data;
   size_in_dword = size_in_bytes / 4;
   for (i = 0; i < size_in_dword; i++)
   {
     *data_32 = pattern;
   data_32++;
   }
}
/***********************************************************************
This function initializes the structure for the partition dump .

**********************************************************************/

uint32 flash_prog_init_parti_dump(void)
{
  return flash_prog_init_parti_dump_new();
#if 0
  KICK_WATCHDOG();

  sec_mode = prog_param.size;

  FLASHLOG(1, ("\n\n------ Initialize for partition dump ---------------\n"));

  FLASHLOG(1, ("Probing flash device:  "));

  /*
   * Probe will fill values in structure if it succeeds.
   * Currently, which_flash parameter is hard coded to 0, but in
   * future this will be dynamic.
   */
  KICK_WATCHDOG();

  /* Memset the partition name */
  DALSYS_memset(dump_dev->parti_name, 0, FLASH_PART_NAME_LENGTH);

  /* Copy the partition name */
  nand_memcpy(dump_dev->parti_name, prog_param.data,
    FLASH_PART_NAME_LENGTH);
  FLASHLOG(3, ("Dumping contents of partition %s\n", dump_dev->parti_name));

  /* Open the partition.  The code which does this is now
   * actually reading the partition information from the flash.
   */

  KICK_WATCHDOG();
  if (NULL == dump_dev->flash_handle)
  {
    /* Probe the device */
    if (DAL_SUCCESS != flash_device_attach(DALDEVICEID_FLASH_DEVICE_1,  //Device Id
      &(dump_dev->flash_handle)))                  //Handle
    {
      FLASHLOG(3, ("Error attaching to device driver \n"));
      SET_ERR (ERR_DEV_MISSING);
      return FLASH_PROG_FAIL;
    }
  }
  FLASHLOG(3, ("Device+Client Attached!\n"));

  if (DAL_SUCCESS == flash_open_partition(dump_dev->flash_handle,
    (const unsigned char *)dump_dev->parti_name))
  {
    FLASHLOG(3, ("flash_open_partition successful\n"));
  }
  else
  {
    FLASHLOG(3, ("flash_open_partition error\n"));
    SET_ERR (ERR_DEV_MISSING);
    return FLASH_PROG_FAIL;
  }

  if (DAL_SUCCESS == flash_get_info(dump_dev->flash_handle,
    FLASH_DEVICE_INFO, &dump_dev->flash_info))
  {
    FLASHLOG(3, ("- flash_get_info successful\n"));
  }
  else
  {
    FLASHLOG(3, ("- flash_get_info error\n"));
    SET_ERR (ERR_DEV_MISSING);
    return FLASH_PROG_FAIL;
  }

  /* Determine the block size depending on the options selected */
  dump_dev->read_stride = dump_dev->flash_info.partition.total_page_size_bytes;

  dump_dev->read_bsize_per_blk =
    dump_dev->flash_info.partition.pages_per_block * dump_dev->read_stride;

  FLASHLOG(3,("dump_init:  page size    0x%x\n",
    dump_dev->flash_info.partition.total_page_size_bytes));
  FLASHLOG(3,("dump_init:  page start  0x%x\n",
    dump_dev->flash_info.partition.physical_start_block * 
	dump_dev->flash_info.partition.pages_per_block));
  FLASHLOG(3,("dump_init:  num pages   0x%x\n",
    dump_dev->flash_info.partition.block_count * 
	dump_dev->flash_info.partition.pages_per_block));

  /* Return the number/size of blocks in the partition to the T32 script.*/
  prog_param.size = dump_dev->flash_info.partition.physical_start_block;
  prog_param.width = dump_dev->flash_info.partition.physical_start_block +
    dump_dev->flash_info.partition.block_count;
  prog_param.addr = dump_dev->flash_info.partition.block_count * 
    dump_dev->flash_info.partition.pages_per_block;
  prog_param.offset = dump_dev->flash_info.partition.total_page_size_bytes;

  return FLASH_PROG_SUCCESS;
#endif
}


/***********************************************************************
FUNCTION      flash_prog_dump_parti

DESCRIPTION   Dump the pages in the partition.


RETURN VALUE  FLASH_PROG_SUCCESS if successful else
              FLASH_PROG_FAIL
**********************************************************************/

uint32 flash_prog_dump_parti()
{
  return flash_prog_dump_parti_new();
#if 0
  uint32 current_page;
  char *data;
  int result = FLASH_DEVICE_DONE;

  if (NULL == dump_dev->flash_handle)
  {
    FLASHLOG(1, ("Error: Init function not called.\n"));
    SET_ERR (ERR_DEV_MISSING);
    return FLASH_PROG_FAIL;
  }

  data = (char *)&prog_param.data;
  current_page = prog_param.addr;
  
  FLASHLOG(3,("dump_page:  page 0x%x\n", current_page));

  result = flash_prog_read_page(dump_dev->flash_handle, &dump_dev->flash_info,
      current_page, FLASH_READ_RAW, mibib_pg_buf);

    if (DAL_SUCCESS != result)
    {
      /* if device is OneNAND, ignore page erased failure */
      if (!((FLASH_DEVICE_FAIL_PAGE_ERASED == result) &&
           (FLASH_ONENAND == dump_dev->flash_info.type)))
      {
        FLASHLOG(1, ("Error: read and udata failed\n"));
        FLASHLOG(3,("Page number 0x%x\n", current_page));
        SET_ERR (ERR_READ_FAIL);
        return FLASH_PROG_FAIL;
      }
    }

  nand_memcpy(data, &mibib_pg_buf[0],
    dump_dev->flash_info.partition.total_page_size_bytes);

  /* return the Block read */
  prog_param.addr = current_page;

  /* Return status */
  prog_param.width = 1;

  return FLASH_PROG_SUCCESS;
#endif
}


/*===========================================================================

  This function return a size in log base 2

===========================================================================*/
static uint8 flash_prog_get_log_base2(uint32 size)
{
  uint8 log_2_size=1;
  while (size > 2)
  {
    log_2_size++;
    size = size >> 1;
  }
  return log_2_size;
}

/*===========================================================================

  This function read a mibib block to a passed buffer

===========================================================================*/
uint32 flash_prog_dump_mibib_parti_block(void)
{
  int block;
  int result = FLASH_DEVICE_DONE;
  int current_page;
  char *data;
  int i;
  static int decode_key_stamped = 0;

  block = prog_param.addr;

  current_page = block * dump_dev->flash_info.partition.pages_per_block;

  data = (char *)&prog_param.data;

  /*FLASHLOG (3,("fdmpb:  block 0x%x\n", block));*/

  for (i=0; i<dump_dev->flash_info.partition.pages_per_block; i++)
  {
    result = flash_prog_read_page(dump_dev->flash_handle, &dump_dev->flash_info,
      current_page, FLASH_READ_MAIN, mibib_pg_buf);

    if (DAL_SUCCESS != result)
    {
      if (FLASH_DEVICE_FAIL_PAGE_ERASED == result)
      {
        DALSYS_memset(&mibib_pg_buf[0], 0xFF,
          dump_dev->flash_info.partition.total_page_size_bytes);
      }
      else
      {
        FLASHLOG(1, ("Error: read and udata failed\n"));
        FLASHLOG(3,("Page number 0x%x\n", current_page));
        SET_ERR (ERR_READ_FAIL);
        return FLASH_PROG_FAIL;
      }
    }
    else
    {
      mibib_pg_buf[dump_dev->flash_info.partition.page_size_bytes] =
        NAND_MIBIB_MBN_PAGE_WRITTEN_FLAG;
      DALSYS_memset(&mibib_pg_buf[
        dump_dev->flash_info.partition.page_size_bytes + 1], 0xFF,
        (dump_dev->flash_info.partition.total_page_size_bytes -
        dump_dev->flash_info.partition.page_size_bytes - 1));
    }

        /* Generates the mibib signature key. This signature is used by QFIT
         * tool to validate if the mibib.mbn generated is compatible to the
         * device the factory image is generated for. Thus we add this to the
         * dump so that the generated images and dumped images will match.*/
    if (decode_key_stamped == 0)
    {
      /* Decode key start flag */
      mibib_pg_buf[dump_dev->flash_info.partition.page_size_bytes+1] =
        NAND_MIBIB_MBN_DECODE_KEY_BEGIN;
      /* version */
      mibib_pg_buf[dump_dev->flash_info.partition.page_size_bytes+2] =
        NAND_MIBIB_MBN_DECODE_KEY_VERSION;
      /* page size in bytes log2*/
      mibib_pg_buf[dump_dev->flash_info.partition.page_size_bytes+3] =
        flash_prog_get_log_base2(dump_dev->flash_info.partition.page_size_bytes);
      /* block size in page log2*/
      mibib_pg_buf[dump_dev->flash_info.partition.page_size_bytes+4] =
        flash_prog_get_log_base2(dump_dev->flash_info.partition.pages_per_block);
      /* device size in blocks */
      mibib_pg_buf[dump_dev->flash_info.partition.page_size_bytes+5] =
        flash_prog_get_log_base2(dump_dev->flash_info.partition.block_count);
      /* Mibib size MIBIB dump size in blocks  */
      mibib_pg_buf[dump_dev->flash_info.partition.page_size_bytes+6] =
        (sec_mode) ? FLASH_MI_BLOCK_LAST - 1 : FLASH_MI_BLOCK_LAST;
      /* Decode key end flag */
      mibib_pg_buf[dump_dev->flash_info.partition.page_size_bytes+7] =
        NAND_MIBIB_MBN_DECODE_KEY_END;

      decode_key_stamped = 1;
    }
    nand_memcpy(data, &mibib_pg_buf[0],
      dump_dev->flash_info.partition.total_page_size_bytes);
    current_page++;
    data += dump_dev->flash_info.partition.total_page_size_bytes;
  }
  return FLASH_PROG_SUCCESS;
}

uint32 flash_prog_init_parti_dump_new(void)
{
  uint32 ecc_disabled;
  uint32 skip_spare_data;
  uint32 skip_bad_block;
  KICK_WATCHDOG();
  sec_mode = prog_param.size;
  ecc_disabled = prog_param.width;
  skip_spare_data = prog_param.offset;
  skip_bad_block = prog_param.reserved;
  FLASHLOG(1, ("\n\n------ Initialize for partition dump ---------------\n"));
  FLASHLOG(1, ("Probing flash device:  "));
  KICK_WATCHDOG();
  DALSYS_memset(dump_dev->parti_name, 0, FLASH_PART_NAME_LENGTH);
  nand_memcpy(dump_dev->parti_name, prog_param.data,
    FLASH_PART_NAME_LENGTH);
  FLASHLOG(3, ("Dumping contents of partition %s\n", dump_dev->parti_name));
  KICK_WATCHDOG();
  if (NULL == dump_dev->flash_handle)
  {
    if (DAL_SUCCESS != flash_device_attach(DALDEVICEID_FLASH_DEVICE_1,  //Device Id
      &(dump_dev->flash_handle)))                  //Handle
    {
      FLASHLOG(3, ("Error attaching to device driver \n"));
      SET_ERR (ERR_DEV_MISSING);
      return FLASH_PROG_FAIL;
    }
  }
  FLASHLOG(3, ("Device+Client Attached!\n"));
  if (DAL_SUCCESS == flash_open_partition(dump_dev->flash_handle,
    (const unsigned char *)dump_dev->parti_name))
  {
    FLASHLOG(3, ("flash_open_partition successful\n"));
  }
  else
  {
    FLASHLOG(3, ("flash_open_partition error\n"));
    SET_ERR (ERR_DEV_MISSING);
    return FLASH_PROG_FAIL;
  }
  if (DAL_SUCCESS == flash_get_info(dump_dev->flash_handle,
    FLASH_DEVICE_INFO, &dump_dev->flash_info))
  {
    FLASHLOG(3, ("- flash_get_info successful\n"));
  }
  else
  {
    FLASHLOG(3, ("- flash_get_info error\n"));
    SET_ERR (ERR_DEV_MISSING);
    return FLASH_PROG_FAIL;
  }
  dump_dev->read_stride = dump_dev->flash_info.partition.total_page_size_bytes;
  dump_dev->read_bsize_per_blk =
    dump_dev->flash_info.partition.pages_per_block * dump_dev->read_stride;
  FLASHLOG(3,("dump_init:  page size    0x%x\n",
    dump_dev->flash_info.partition.total_page_size_bytes));
  FLASHLOG(3,("dump_init:  page start  0x%x\n",
    dump_dev->flash_info.partition.physical_start_block *
  dump_dev->flash_info.partition.pages_per_block));
  FLASHLOG(3,("dump_init:  num pages   0x%x\n",
    dump_dev->flash_info.partition.block_count *
  dump_dev->flash_info.partition.pages_per_block));
  prog_param.size = dump_dev->flash_info.partition.physical_start_block;
  prog_param.width = dump_dev->flash_info.partition.physical_start_block +
    dump_dev->flash_info.partition.block_count;
  prog_param.addr = dump_dev->flash_info.partition.block_count *
    dump_dev->flash_info.partition.pages_per_block;
  prog_param.reserved = dump_dev->flash_info.partition.pages_per_block;
  prog_param.offset = (ecc_disabled) ?
    dump_dev->flash_info.partition.total_page_size_bytes :
    dump_dev->flash_info.partition.page_size_bytes;
  DALSYS_memset(&nand_dump_header, 0, sizeof(nand_dump_header));
  nand_dump_header.dump_version = NAND_DUMP_VERSION;
  nand_dump_header.bad_block_pattern = NAND_DUMP_BAD_BLOCK_PATTERN;
  nand_dump_header.read_error_pattern = NAND_DUMP_READ_ERROR_PATTERN;
  nand_dump_header.erased_page_pattern = NAND_DUMP_ERASED_PAGE_PATTERN;
  nand_dump_header.partition_end_pattern = NAND_DUMP_PARTI_END_PATTERN;
  nand_dump_header.dump_page_size = prog_param.offset;
  nand_dump_header.ecc_enabled = (ecc_disabled)?
    NAND_DUMP_ECC_DISABLED : NAND_DUMP_ECC_ENABLED;
  nand_dump_header.skip_spare_data = (skip_spare_data)?
    NAND_DUMP_SKIP_SPARE_DATA : NAND_DUMP_INCLUDE_SPARE_DATA;
  nand_dump_header.skip_bad_block = (skip_bad_block)?
    NAND_DUMP_SKIP_BAD_BLOCK : NAND_DUMP_INCLUDE_BAD_BLOCK;
  nand_memcpy(nand_dump_header.parti_name, dump_dev->parti_name,
    FLASH_PART_NAME_LENGTH);
  nand_memcpy(&nand_dump_header.flash_info, &dump_dev->flash_info,
    sizeof(struct flash_info));
  return FLASH_PROG_SUCCESS;
}
uint32 flash_prog_dump_parti_new()
{
  uint32 page, num_pages=1;
  uint32 transfer_size, create_header;
  char *data;
  int result = FLASH_DEVICE_DONE;
  if (NULL == dump_dev->flash_handle)
  {
    FLASHLOG(1, ("Error: Init function not called.\n"));
    SET_ERR (ERR_DEV_MISSING);
    return FLASH_PROG_FAIL;
  }
  data = (char *)&prog_param.data;
  page = prog_param.addr;
  create_header = prog_param.offset;
  transfer_size = nand_dump_header.dump_page_size;
  /*FLASHLOG(3,("dump_page:  page 0x%x\n", page));*/
  DALSYS_memset((void *)mibib_pg_buf, 0, MAX_PAGE_SIZE);
  if (create_header)
  {
    nand_memcpy(&mibib_pg_buf[0], &nand_dump_header, sizeof(nand_dump_header));
    transfer_size = NAND_DUMP_HEADER_SIZE;
    nand_memcpy(data, &mibib_pg_buf[0], transfer_size);
  }
  else
  {
    num_pages = prog_param.width;
    result = flash_prog_dump_read_page(page, num_pages, data, NULL);
  }
  prog_param.addr = page;
  prog_param.width = num_pages;
  result = FLASH_DEVICE_DONE;
  return result;
}
uint32 flash_prog_dump_read_page(uint32 page, uint32 num_pages, void *main_data,
  void *spare_data)
{
  int result = FLASH_DEVICE_DONE;
  uint32 block = 0;
  enum flash_block_state block_state;
  if (nand_dump_header.ecc_enabled == NAND_DUMP_ECC_DISABLED)
  {
    result = flash_prog_read_mpages(dump_dev->flash_handle, &dump_dev->flash_info,
      page, num_pages, FLASH_READ_RAW, main_data);
  }
  else
  {
    block = page / dump_dev->flash_info.partition.pages_per_block;
    if (DAL_SUCCESS != flash_block_get_state(dump_dev->flash_handle,
                            block, &block_state))
    {
      result = FLASH_PROG_FAIL;
    }
    if ((FLASH_BLOCK_BAD == block_state) || (result == FLASH_PROG_FAIL))
    {
      flash_prog_dump_set_pattern_32((void *)main_data,
      nand_dump_header.dump_page_size, nand_dump_header.bad_block_pattern);
      return FLASH_DEVICE_DONE;
    }
    result = flash_prog_read_mpages(dump_dev->flash_handle, &dump_dev->flash_info,
      page, num_pages, FLASH_READ_MAIN, main_data);
  }
  if (DAL_SUCCESS != result)
  {
	if (FLASH_DEVICE_FAIL_PAGE_ERASED != result)
    {		
      flash_prog_dump_set_pattern_32((void *)main_data,
      nand_dump_header.dump_page_size, nand_dump_header.read_error_pattern);
	}
	else
	{
      flash_prog_dump_set_pattern_32((void *)main_data,
      nand_dump_header.dump_page_size, nand_dump_header.erased_page_pattern);
    }
  }
  return FLASH_DEVICE_DONE;
}
