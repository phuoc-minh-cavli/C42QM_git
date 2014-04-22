/******************************************************************************
 * flash_ftl_core.c
 *
 * Flash Translation layer (FTL).
 * This layer is responsible for bad block management and logical to
 * physical block conversation etc.
 *
 *
 * Copyright (c) 2016-2017, 2019, 2022 QUALCOMM Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *
 *****************************************************************************/
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
 * 2022-09-15   sk      Skip FTL bad block check for qdsp_swap partition
 * 2019-06-26   sl      avoid KW error
 * 2019-06-25   sl      corrected flash result checks
 * 2017-03-10   svl     Added support to copy data of bad block to next
                        good block before marking it as bad. Also corrected
                        pad block calculation and changed name lba to more 
                        appropriate "lpa".
 * 2016-12-02   svl     Added new lpa_sized buffer for flash dma operations.
 * 2016-11-11   svl     Review comments incorporated.
 * 2016-10-03   svl     Flash multi page read/write support added.
 * 2016-08-24   svl     FTL core layer support added.
 *=======================================================================*/

#include "DALFramework.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "flash.h"
#include "flash_ftl.h"
#include "flash_ftl_util.h"
#include "flash_ftl_core.h"

#include "assert.h"

/****************************************************************
 * Data structure definitions
 ***************************************************************/
// #define FLASH_FTL_CORE_PARTI_PAD_BLOCK_FACTOR     ((float)1/(float)16) as per discussion with target team formula is changed to 1/32
#define FLASH_FTL_CORE_PARTI_PAD_BLOCK_FACTOR     ((float)1/(float)32)
#define FLASH_FTL_CORE_PARTI_PAD_ADJ_BLOCKS       (2)

#define FLASH_FTL_CORE_PART_COVERSION_FACTOR      ((float)1+(float)FLASH_FTL_CORE_PARTI_PAD_BLOCK_FACTOR)
#define FLASH_FTL_CORE_GOOD_BLOCKS(blocks)                                         \
                          ((((float)blocks/FLASH_FTL_CORE_PART_COVERSION_FACTOR)+1)\
                          - FLASH_FTL_CORE_PARTI_PAD_ADJ_BLOCKS)

/// map each block to each bit of a byte
#define FLASH_FTL_CORE_BBT_SIZE(block_count)      (block_count/8+1)

/// to check if a block is bad
#define FLASH_FTL_CORE_BLOCK_IS_BAD(bbt, block)   (bbt[block/8] & (1 << (block%8)))

/// to mark a block as bad
#define FLASH_FTL_CORE_MARK_BADBLOCK(bbt, block)  (bbt[block/8] |= (1 << (block%8)))

/// initialize page data io vectors
#define INIT_PAGE_DATA_IOVEC(iovec,page_size,page,page_count)                 \
                                        iovec.main_size_bytes   = page_size;  \
                                        iovec.spare_size_bytes  = 0;          \
                                        iovec.total_page_count  = page_count; \
                                        iovec.start_page        = page;


/// initialize page data descriptors
// #define INIT_BUF_DESC(desc, iovec, data)                                      \
      // desc[FLASH_FTL_CORE_MAIN_BUF_DESC].user        = iovec.total_page_count;     \
      // desc[FLASH_FTL_CORE_MAIN_BUF_DESC].size        = iovec.main_size_bytes;      \
      // desc[FLASH_FTL_CORE_MAIN_BUF_DESC].VirtualAddr = (uint32)data;               \
      // desc[FLASH_FTL_CORE_SPARE_BUF_DESC].user       = iovec.total_page_count;     \
      // desc[FLASH_FTL_CORE_SPARE_BUF_DESC].size       = 0;                          \
      // desc[FLASH_FTL_CORE_SPARE_BUF_DESC].VirtualAddr= FLASH_MEM_ADDR_NOT_SPECIFIED;

#define FLASH_FTL_CORE_MAX_MULTI_PAGE (64)

static __inline void INIT_BUF_DESC (
    page_buff_desc_t *buffer_desc,
    struct flash_page_iovec *iovec,
    unsigned char* addr)
{
  DALSysMemDescBuf *main_desc_buff;
  DALSysMemDescBuf *spare_desc_buff;

  DALFW_MemDescInit (NULL, (DALSysMemDescList *)buffer_desc, 1);

  // main_desc_buff = DALFW_MemDescBufPtr((DALSysMemDescList *)buffer_desc, FLASH_FTL_CORE_MAIN_BUF_DESC);
  main_desc_buff = &(buffer_desc->BufInfo[FLASH_FTL_CORE_MAIN_BUF_DESC]);
  ASSERT (main_desc_buff != NULL);
  main_desc_buff->user = iovec->total_page_count;
  main_desc_buff->size = iovec->total_page_count*iovec->main_size_bytes;
  main_desc_buff->VirtualAddr = (uint32)addr;

  spare_desc_buff = &(buffer_desc->BufInfo[FLASH_FTL_CORE_SPARE_BUF_DESC]);
  ASSERT (spare_desc_buff != NULL);
  spare_desc_buff->user = iovec->total_page_count;
  spare_desc_buff->size = 0;
  spare_desc_buff->VirtualAddr = DALSYS_MEM_ADDR_NOT_SPECIFIED;
  spare_desc_buff->PhysicalAddr = DALSYS_MEM_ADDR_NOT_SPECIFIED;

  buffer_desc->dwNumDescBufs = 2;
}

#define FTL_RW_PART_NAME "qdsp_swap"

/* 0 means success, any other value means failure */
static uint32 cmp_part_name_result = FLASH_FTL_FAIL;

/****************************************************************
 * Local Functions
 ***************************************************************/

static FLASH_FTL_STATUS
flash_ftl_core_update_part_info(ftl_pvt_ctx_t *client)
{
  struct flash_info info;
  enum flash_block_state block_state;
  uint32 block = 0, end_block = 0;
  uint32 total_bb_count = 0, total_good_block_count = 0;
  uint32 img_block_count = 0;

  /* get info from the handle and fill client context */
  flash_get_info(client->flash_handle, FLASH_DEVICE_INFO,  &info);
  if(!info.device_name)
  {
    return FLASH_FTL_FAIL;
  }

  /* fill partition specific info in client ctx */
  if(FLASH_FTL_OK != flash_ftl_memscpy(client->info.device_name,
                      flash_ftl_strlen((const uint8 *)info.device_name),
                      info.device_name,
                      flash_ftl_strlen((const uint8 *)info.device_name)))
  {
    return FLASH_FTL_FAIL;
  }

  client->info.maker_id  = info.maker_id;
  client->info.device_id = info.device_id;

  /* bad block tbl (bbt) should not be more than total blocks in this partition */
  client->bbt = 
      (uint8*)flash_ftl_malloc(FLASH_FTL_CORE_BBT_SIZE(info.partition.block_count));
  if(NULL == client->bbt)
  {
    return FLASH_FTL_OUT_OF_MEM;
  }
  flash_ftl_memset(client->bbt, 0, FLASH_FTL_CORE_BBT_SIZE(info.partition.block_count));

  /* ------------ BAD BLOCK TABLE START ------------ */
  block = 0;  /* start block of this partition */
  end_block = block + info.partition.block_count;

  /* now check for good and bad blocks */
  for ( ; block < end_block ; block++ )
  {
    if (DAL_SUCCESS == flash_block_get_state( client->flash_handle,
                                              block,
                                              &block_state))
    {
      /* if block good update total_block_count */
      if (FLASH_BLOCK_OK == block_state)
      {
        total_good_block_count++;
      }
      else
      {
        /* update bad block count and bbt */
        FLASH_FTL_CORE_MARK_BADBLOCK(client->bbt, block);
        total_bb_count++;
      }
    }
    else
    {
      /* printf("" get block state failed\n"); */
      return FLASH_FTL_FAIL;
    }
  }

  if(total_bb_count == info.partition.block_count)
  {
    /* all blocks are bad */
    return FLASH_FTL_OUT_OF_GOOD_BLOCKS;
  }
  /* ------------ BAD BLOCK TABLE END ------------ */

  img_block_count = FLASH_FTL_CORE_GOOD_BLOCKS(info.partition.block_count);

  /* @note: As per FTL bad block check, the number of bad blocks cannot
   * exceed the number of reserved pad blocks as per FTL formula.
   * The RW partitions which do not store image binaries contain more usable
   * blocks. In order to let the RW partition client use the good blocks as 
   * long as enough of them are present, the check is being skipped for 
   * Read-Write partitions. Currently only such partition is "qdsp_swap". */
  if ((total_bb_count > info.partition.block_count - img_block_count)
       && (cmp_part_name_result != FLASH_FTL_OK))
  {
    return FLASH_FTL_OUT_OF_GOOD_BLOCKS;
  }

   /* Resetting value to default failure case */
   cmp_part_name_result = FLASH_FTL_FAIL;

  client->info.erase_block_count = img_block_count;
  client->info.lpa_count
              = client->info.erase_block_count * info.partition.pages_per_block;
  client->info.lpa_size_in_kbytes
              = (info.partition.page_size_bytes / SIZE_OF_1_KBYTE);
  client->info.erase_block_size_in_kbytes
              = client->info.lpa_size_in_kbytes * info.partition.pages_per_block;

  return FLASH_FTL_OK;
}

/*
  This func assumes that bbt table has correct info on bad blocks
  and that there are no new bad blocks apart from the ones mentioned
  in client->bbt.
  Explanation on finding good block :
  Basically we skip the bad blocks and count the good blocks to give
  logical to physical mapping. Here physical doesn't mean physical address on nand.
  Physical means that if logical block is 10th then actual block in nand
  could be 11 or 20 depending on the bad blocks from 0 to 10.

  e.g.
    in a partition with 10 actual blocks :
    if block numbers 3, 5 and 6 are bad then below would be lba to pba mapping :
    lba-pba : 0-0, 1-1, 2-2, 3-4, 4-7, 5-8, 6-9

    3rd logical block would be 4th phys block,
    similarly 4th logical block would be 7th phys block.
*/
static FLASH_FTL_STATUS
flash_ftl_core_get_pba_from_lba( ftl_pvt_ctx_t *client, uint32 lba, uint32 *pba)
{
  FLASH_FTL_STATUS ret_status = FLASH_DEVICE_DONE;
  uint32 block_num = 0, i = 0;
  struct flash_info info;

  if((NULL == client) || (NULL == pba))
  {
    return FLASH_FTL_INVALID_PARAM;
  }

  /* get info from the handle and fill client context */
  flash_get_info(client->flash_handle, FLASH_DEVICE_INFO,  &info);

  /* get phys block (count good blocks and skip bad blocks) */
  for(i = 0, block_num = 0; i < info.partition.block_count; i++)
  {
    if(FALSE == FLASH_FTL_CORE_BLOCK_IS_BAD(client->bbt, i))
    {
      /* if good block and didn't match lba then count till we get our block */
      if(block_num != lba)
      {
        block_num++;
      }
      else
      {
        *pba = i;
        return FLASH_FTL_OK;
      }
    }
    else
    {
      /* else skip bad blocks */
      ret_status = FLASH_FTL_FAIL;
    }
  }

  if(block_num >= info.partition.block_count)
  {
    /* given lba is not a valid block */
    ret_status = FLASH_FTL_INVALID_PARAM;
  }
  else
  {
    if( i >= info.partition.block_count )
    {
      /* given lba is not a valid block */
      ret_status = FLASH_FTL_OUT_OF_GOOD_BLOCKS;
    }
  }

  return ret_status;
}

static FLASH_FTL_STATUS
flash_ftl_core_mark_bad_block(ftl_pvt_ctx_t *client, uint32 block)
{
  FLASH_FTL_STATUS ret_status = FLASH_DEVICE_DONE;

  ret_status = flash_block_set_state( client->flash_handle,
                                      block, FLASH_BLOCK_BAD);
  if (ret_status != FLASH_DEVICE_DONE)
  {
    /* printf("[%s,%d] erase failed with error : %d!!\n", __func__, __LINE__, ret_status); */
    ret_status = FLASH_FTL_FAIL;
  }
  else
  {
    ret_status = FLASH_FTL_OK;
  }

  /* mark block as bad in our BBT anyways */
  FLASH_FTL_CORE_MARK_BADBLOCK(client->bbt, block);

  return ret_status;
}

static FLASH_FTL_STATUS
flash_ftl_core_bad_block_backup(ftl_pvt_ctx_t *client,
                                struct flash_info *info,
                                uint32 logical_bad_block,
                                uint32 phys_failed_page,
                                uint32 *backed_up_block_ptr)
{
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;
  uint32 done = 0, i = 0;
  unsigned char *buffer = NULL;
  struct flash_page_iovec  page_data_iovec;
  // struct flash_mem_desc_buf page_buff_desc[FLASH_FTL_CORE_MAX_NUM_BUFS];
  uint32 page_size = 0, num_pages_to_write = 1;
  uint32 bad_block_start_page = 0, phys_good_block_start_page = 0;
  uint32 phys_good_block = 0, phys_bad_block = 0, logical_good_block = 0;

  if(!backed_up_block_ptr || !client || !info)
  {
    return FLASH_FTL_INVALID_PARAM;
  }

  logical_good_block = logical_bad_block + 1;
  page_size = info->partition.page_size_bytes;
  buffer = (unsigned char*)flash_ftl_aligned_malloc(page_size);
  ret_status = flash_ftl_core_get_pba_from_lba( client,
                                                logical_bad_block,
                                                &phys_bad_block);
  if(ret_status != FLASH_FTL_OK)
  {
    flash_ftl_aligned_free(buffer);
    return ret_status;
  }
  bad_block_start_page = phys_bad_block * info->partition.pages_per_block;

  INIT_PAGE_DATA_IOVEC(page_data_iovec, page_size, bad_block_start_page, 1);

  num_pages_to_write = (phys_failed_page%info->partition.pages_per_block);
  do
  {
    // block has to be erased before writing
    ret_status = flash_ftl_core_erase_block(client, logical_good_block, 1);
    if(ret_status != FLASH_FTL_OK)
    {
      flash_ftl_aligned_free(buffer);
      return ret_status;
    }

    /*
      get next good block, bad_block is logical block that has gone bad
       however as bad_block is not marked as bad we need to use bad_block+1
       to search next physical good block
    */
    ret_status = flash_ftl_core_get_pba_from_lba( client,
                                                  logical_good_block,
                                                  &phys_good_block);
    if(ret_status != FLASH_FTL_OK)
    {
      flash_ftl_aligned_free(buffer);
      return ret_status;
    }

    phys_good_block_start_page = phys_good_block * info->partition.pages_per_block;

    /* read contents of current block till the failed page and copy to the next good block */
    for(i = 0; i < num_pages_to_write ; i++)
    {
      /* read 1 page at a time from the bad block */
      page_data_iovec.start_page = bad_block_start_page + i;
      INIT_BUF_DESC(&client->page_buff_desc, &page_data_iovec, buffer);
      ret_status = flash_read_pages(client->flash_handle,
                                    (enum page_read_opcode) FLASH_READ_MAIN,
                                    (DALSysMemDescList *)&client->page_buff_desc,
                                    &page_data_iovec);
      if (ret_status != FLASH_DEVICE_DONE)
      {
        if(FLASH_DEVICE_FAIL_PAGE_ERASED != ret_status)
        {
          /* printf("[%s,%d] read failed with error : %d!!\n", */
          /* __func__, __LINE__, ret_status); */
          flash_ftl_aligned_free(buffer);
          return  FLASH_FTL_FAIL;
        }
        else  /* if(FLASH_DEVICE_FAIL_PAGE_ERASED == ret_status) */
        {
          /* skip copying erased block */
          continue;
        }
      }

      // write page in the next good block
      page_data_iovec.start_page = phys_good_block_start_page + i;
      INIT_BUF_DESC(&client->page_buff_desc, &page_data_iovec, buffer);
      ret_status = flash_write_pages(client->flash_handle,
                                     (enum page_write_opcode) FLASH_WRITE_MAIN,
                                     (DALSysMemDescList *)&client->page_buff_desc,
                                     &page_data_iovec);
      if (ret_status != FLASH_DEVICE_DONE)
      {
        if(ret_status != FLASH_DEVICE_FAIL)
        {
          /* ERROR FATAL HERE */
          /* printf("[%s,%d] erase failed !!\n", __func__, __LINE__); */
          /* something went wrong or invalid parameter passed */
          flash_ftl_aligned_free(buffer);
          return ret_status;
        }
        else
        {
          /* the block assumed to be good for write has gone bad
              mark it as bad and continue to the next good block
              for write
          */
          flash_ftl_core_mark_bad_block(client, phys_good_block);
          break;
        }
      }
    }

    if(i != num_pages_to_write)
    {
      /* find next good block to backup data */
      done = 0;
    }
    else
    {
      done = 1;
    }
  }while(!done);

  *backed_up_block_ptr = phys_good_block;
  flash_ftl_aligned_free(buffer);
  return FLASH_FTL_OK;
}

/*****************************************************************************
 * APIs
 *****************************************************************************/

FLASH_FTL_STATUS
flash_ftl_core_open(ftl_pvt_ctx_t **part_handle, const uint8 * part_name)
{
  ftl_pvt_ctx_t *client = NULL;
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;

  client = (ftl_pvt_ctx_t*)flash_ftl_malloc(sizeof(ftl_pvt_ctx_t));
  if(NULL == client)
  {
    return FLASH_FTL_OUT_OF_MEM;
  }
  flash_ftl_memset(client, 0, sizeof(ftl_pvt_ctx_t));

  /* call attach to get a valid handle */
  ret_status = flash_device_attach( DALDEVICEID_FLASH_DEVICE_1,
                                    &(client->flash_handle));
  if(DAL_SUCCESS != ret_status)
  {
    return FLASH_FTL_FAIL;
  }

  /* use the handle to open the partition */
  ret_status = flash_open_partition(client->flash_handle,
                                    (const unsigned char *)part_name);
  if(DAL_SUCCESS != ret_status)
  {
    return FLASH_FTL_FAIL;
  }

  /* Compare the partition name */
   if (FLASH_FTL_OK != (flash_ftl_strcmp(part_name,
                       (const uint8*)FTL_RW_PART_NAME,
                       &cmp_part_name_result)))
   {
      return FLASH_FTL_INVALID_PARAM;
   }

  /* check partition for good blocks and update bad block info in BBT */
  ret_status = flash_ftl_core_update_part_info(client);
  if(FLASH_FTL_OK != ret_status)
  {
    flash_ftl_free(client);
    return ret_status;
  }

  client->dma_buffer = flash_ftl_aligned_malloc(
                        client->info.lpa_size_in_kbytes * SIZE_OF_1_KBYTE);
  if(NULL == client->dma_buffer)
  {
    flash_ftl_free(client->bbt);
    flash_ftl_free(client);
    return FLASH_FTL_OUT_OF_MEM;
  }

  *part_handle = client;

  return FLASH_FTL_OK;
}


FLASH_FTL_STATUS
flash_ftl_core_close(ftl_pvt_ctx_t **part_handle)
{
  ftl_pvt_ctx_t *client = NULL;

  client = *part_handle;
  flash_device_close(client->flash_handle);
  flash_device_detach(client->flash_handle);
  if(client->dma_buffer)
  {
    flash_ftl_aligned_free(client->dma_buffer);
    client->dma_buffer = NULL;
  }
  if(client->bbt)
  {
    flash_ftl_free(client->bbt);
    client->bbt = NULL;
  }
  client->flash_handle = NULL;
  flash_ftl_free(client);
  *part_handle = NULL;

  return FLASH_FTL_OK;
}

FLASH_FTL_STATUS
flash_ftl_core_read_lpas(ftl_pvt_ctx_t *client, uint32 start_page, uint32 num_of_pages, uint8 *data)
{

  FLASH_FTL_STATUS ret_status = FLASH_DEVICE_DONE;
  struct flash_page_iovec  page_data_iovec;
  uint32 start_block_num = 0, tmp_block_num = 0, pba = 0;
  uint32 page_size = 0, page_count = 0, total_pages_to_read=num_of_pages;
  struct flash_info info;

  if((NULL == client) || (NULL == data) || (0 == num_of_pages))
  {
    return FLASH_FTL_INVALID_PARAM;
  }

  /* get info from the handle and fill client context */
  flash_get_info(client->flash_handle, FLASH_DEVICE_INFO,  &info);

  /* get phys block (count good blocks and skip bad blocks) */
  start_block_num = (start_page/info.partition.pages_per_block);
  ret_status = flash_ftl_core_get_pba_from_lba(client, start_block_num, &pba);
  if(FLASH_FTL_OK != ret_status)
  {
    return ret_status;
  }

  /* Initialize the page data IOVEC structure */
  page_size = info.partition.page_size_bytes;
  start_page += (pba - start_block_num) * info.partition.pages_per_block;

  page_count = (pba+1)*info.partition.pages_per_block - start_page;
  if(page_count > total_pages_to_read)
  {
    page_count = total_pages_to_read;
  }

  INIT_PAGE_DATA_IOVEC(page_data_iovec, page_size, start_page, page_count);

  while( total_pages_to_read )
  {
    /* Initialize buffer descriptors */
    INIT_BUF_DESC(&client->page_buff_desc, &page_data_iovec, data);

    ret_status = flash_read_pages(client->flash_handle,
                                  (enum page_read_opcode) FLASH_READ_MAIN,
                                  (DALSysMemDescList *)&client->page_buff_desc,
                                  &page_data_iovec);

    if (ret_status != FLASH_DEVICE_DONE)
    {
      if(FLASH_DEVICE_FAIL_PAGE_ERASED != ret_status)
      {
          /* printf("[%s,%d] read failed with error : %d!!\n", */
          /* __func__, __LINE__, ret_status); */
          return  FLASH_FTL_FAIL;
      }
    }

    /* update src for next read */
    total_pages_to_read -= page_count;
    if(0 == total_pages_to_read)
    {
      break;
    }

    /* update start_page according to difference in old phys block and new phys block */
    page_data_iovec.start_page += page_count;

    /* check if new start page is in next block */
    tmp_block_num = page_data_iovec.start_page / info.partition.pages_per_block;

    if(tmp_block_num != pba)
    {
      /* reading from next block */
      start_block_num++;

      /* get next phys block */
      ret_status = flash_ftl_core_get_pba_from_lba(client, start_block_num, &pba);
      if(FLASH_FTL_OK != ret_status)
      {
        return ret_status;
      }

      page_data_iovec.start_page += (pba-tmp_block_num)*info.partition.pages_per_block;
    }

    /* update dest for next read */
    data += (page_size*page_count);

    if(total_pages_to_read < FLASH_FTL_CORE_MAX_MULTI_PAGE)
    {
      page_count = total_pages_to_read;
    }
    else
    {
      page_count = FLASH_FTL_CORE_MAX_MULTI_PAGE;
    }

    /* update remaining pages to read */
    page_data_iovec.total_page_count  = page_count;
  }

  return FLASH_FTL_OK;
}

FLASH_FTL_STATUS
flash_ftl_core_write_lpas( ftl_pvt_ctx_t *client, uint32 start_page, uint32 num_of_pages, uint8 *data)
{

  FLASH_FTL_STATUS ret_status = FLASH_DEVICE_DONE;
  struct flash_page_iovec  page_data_iovec;
  uint32 start_block_num = 0, pba = 0, page_size = 0;
  uint32 pages_per_block = 0, page_count = 0;
  uint32 total_pages_to_write = num_of_pages, tmp_block_num = 0, tmp_start_page = start_page;
//  struct flash_mem_desc_buf page_buff_desc[FLASH_FTL_CORE_MAX_NUM_BUFS];
  struct flash_info info;

  if((NULL == client) || (NULL == data) || (0 == num_of_pages))
  {
    return FLASH_FTL_INVALID_PARAM;
  }

  /* get info from the handle and fill client context */
  flash_get_info(client->flash_handle, FLASH_DEVICE_INFO,  &info);

  /* from the given page addr get the block which it is in */
  pages_per_block = info.partition.pages_per_block;
  start_block_num = (start_page/pages_per_block);
  /* get phys block (count good blocks and skip bad blocks) */
  ret_status = flash_ftl_core_get_pba_from_lba(client, start_block_num, &pba);
  if(FLASH_FTL_OK != ret_status)
  {
    return ret_status;
  }

  /* Initialize the page data IOVEC structure */
  page_size = client->info.lpa_size_in_kbytes*SIZE_OF_1_KBYTE;
  /* fix start page acc. to phys block */
  tmp_start_page += (pba-start_block_num)*pages_per_block;

  page_count = (pba+1)*info.partition.pages_per_block - tmp_start_page;
  if(page_count > total_pages_to_write)
  {
    page_count = total_pages_to_write;
  }

  INIT_PAGE_DATA_IOVEC( page_data_iovec, page_size, tmp_start_page, page_count);

  while( total_pages_to_write )
  {
    /* Initialize buffer descriptors */
    INIT_BUF_DESC(&client->page_buff_desc, &page_data_iovec, data);

    ret_status = flash_write_pages(client->flash_handle,
                                   (enum page_write_opcode) FLASH_WRITE_MAIN,
                                   (DALSysMemDescList *)&client->page_buff_desc,
                                   &page_data_iovec);

    if (ret_status != FLASH_DEVICE_DONE)
    {
      if(ret_status != FLASH_DEVICE_FAIL)
      {
        /* printf("[%s,%d] erase failed !!\n", __func__, __LINE__); */
        /* something went wrong or invalid parameter passed */
        return ret_status;
      }

      /* else write failed so mark the whole block as bad */
      tmp_block_num = page_data_iovec.start_page/pages_per_block;

      /*
        copy data from the block gone bad into the next good block
        also retrive the updated good block number
      */

      ret_status = flash_ftl_core_bad_block_backup( client, &info,
                                                    start_block_num,
                                                    page_data_iovec.start_page,
                                                    &pba);
      if(FLASH_FTL_OK != ret_status)
      {
        /* can't get next good block */
        return ret_status;
      }

      /* data backed up so we can mark given bad block as bad in BBT and NAND */
      flash_ftl_core_mark_bad_block(client, tmp_block_num);

      /* start over again with next good block */
      /* reset and copy pages already written in bad block to a good block */
      page_data_iovec.start_page += (pba-tmp_block_num)*pages_per_block;
    }
    else
    {
      /* update src for next write */
      total_pages_to_write -= page_count;
      if(0 == total_pages_to_write)
      {
        break;
      }

      page_data_iovec.start_page += page_count;
      tmp_block_num = page_data_iovec.start_page/pages_per_block;
      if(pba != tmp_block_num)
      {
        start_block_num++;
        ret_status = flash_ftl_core_get_pba_from_lba(client, start_block_num, &pba);
        if(FLASH_FTL_OK != ret_status)
        {
          return ret_status;
        }
        page_data_iovec.start_page += (pba-tmp_block_num)*pages_per_block;
      }

      /* update dest for next read */
      data += (page_size*page_count);

      if(total_pages_to_write < FLASH_FTL_CORE_MAX_MULTI_PAGE)
      {
        page_count = total_pages_to_write;
      }
      else
      {
        page_count = FLASH_FTL_CORE_MAX_MULTI_PAGE;
      }
      page_data_iovec.total_page_count  = page_count;
    }
  }

  return FLASH_FTL_OK;
}

FLASH_FTL_STATUS
flash_ftl_core_erase_block(ftl_pvt_ctx_t *client, uint32 erase_block, uint32 erase_block_count)
{
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;
  struct flash_block_vector block_vector;
  uint32 result_vector[1];
  uint32 i = 0, actual_erase_block = 0;
  struct flash_info info;

  if(!client || !erase_block_count)
  {
    return FLASH_FTL_INVALID_PARAM;
  }

  /* get info from the client and fill client context */
  flash_get_info(client->flash_handle, FLASH_DEVICE_INFO, &info);

  for(i=0; i < info.partition.block_count; i++)
  {
    /* get actual block to erase */
    ret_status = flash_ftl_core_get_pba_from_lba(client, erase_block, &actual_erase_block);
    if(FLASH_FTL_OK != ret_status)
    {
      break;
    }

    block_vector.start_block = actual_erase_block;
    block_vector.block_count = 1;
    block_vector.result_vector = (int *)&result_vector[0];

    ret_status = flash_erase_blocks(client->flash_handle, &block_vector, 1);
    if(FLASH_DEVICE_DONE != ret_status)
    {
      if(FLASH_DEVICE_FAIL == ret_status)
      {
        /* erase block failed so mark it as bad */
        flash_ftl_core_mark_bad_block(client, actual_erase_block);
      }
      else
      {
        /* printf("[%s,%d] erase failed !!\n", __func__, __LINE__); */
        /* something went wrong or invalid parameter passed */
        break;
      }
    }
    else
    {
      /* update the number of blocks successfully erased */
      erase_block_count--;
      ret_status = FLASH_FTL_OK;
      if(erase_block_count)
      {
        /* use next block */
        erase_block++;
      }
      else
      {
        /* all blocks erased */
        break;
      }
    }
  }

  if((FLASH_FTL_OK == ret_status) && erase_block_count)
  {
    return FLASH_FTL_OUT_OF_GOOD_BLOCKS;
  }

  return ret_status;
}
