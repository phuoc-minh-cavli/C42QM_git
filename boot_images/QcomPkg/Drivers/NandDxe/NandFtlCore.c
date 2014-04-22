/******************************************************************************
 * NandFtlCore.c
 *
 * Flash Translation layer (FTL).
 * This layer is responsible for bad block management and logical to
 * physical block conversation etc.
 *
 *
 * Copyright (c) 2016, 2018 QUALCOMM Technologies Incorporated.
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
 * 2018-06-03   eo     Corrected BBT table initialization
 * 2016-12-02   svl     added new lba_sized buffer for flash dma operations
 * 2016-11-11   svl     Review comments incorporated.
 * 2016-10-03   svl     flash multi page read/write support added
 * 2016-08-24   svl     FTL core layer support added
 *=======================================================================*/

#include "nand_api.h"
#include "NandFtl.h"
#include "NandFtlUtil.h"
#include "NandFtlCore.h"

#ifndef FLASH_FTL_ENABLE_ERASE_BEFORE_WRITE
  #define FLASH_FTL_ENABLE_ERASE_BEFORE_WRITE
#endif
/****************************************************************
 * Data structure definitions
 ***************************************************************/
//#define FLASH_FTL_CORE_PARTI_PAD_BLOCK_FACTOR     ((float)1/(float)16)
#define FLASH_FTL_CORE_PARTI_PAD_BLOCK_FACTOR     (625) // (1/16) * 10000
#define FLASH_FTL_CORE_PARTI_PAD_ADJ_BLOCKS       (2)

//#define FLASH_FTL_CORE_PART_COVERSION_FACTOR      ((float)1+(float)FLASH_FTL_CORE_PARTI_PAD_BLOCK_FACTOR)
//#define FLASH_FTL_CORE_GOOD_BLOCKS(blocks)                                         \
//                          ((((float)blocks/FLASH_FTL_CORE_PART_COVERSION_FACTOR)+1)\
//                          - FLASH_FTL_CORE_PARTI_PAD_ADJ_BLOCKS)
#define FLASH_FTL_CORE_FLOAT_TO_INT_FACTOR        (10000)
#define FLASH_FTL_CORE_PART_COVERSION_FACTOR      ((1*FLASH_FTL_CORE_FLOAT_TO_INT_FACTOR)\
	                                           +FLASH_FTL_CORE_PARTI_PAD_BLOCK_FACTOR)
#define FLASH_FTL_CORE_GOOD_BLOCKS(blocks)                                         \
                   ((((blocks*FLASH_FTL_CORE_FLOAT_TO_INT_FACTOR)/FLASH_FTL_CORE_PART_COVERSION_FACTOR)+1)\
                    - FLASH_FTL_CORE_PARTI_PAD_ADJ_BLOCKS)

/// map each block to each bit of a byte
#define FLASH_FTL_CORE_BBT_SIZE(block_count)      (block_count/8+1)

/// to check if a block is bad
#define FLASH_FTL_CORE_BLOCK_IS_BAD(bbt, block)   (bbt[block/8] & (1 << (block%8)))

/// to mark a block as bad
#define FLASH_FTL_CORE_MARK_BADBLOCK(bbt, block)  (bbt[block/8] |= (1 << (block%8)))

/// to mark a block as good
#define FLASH_FTL_CORE_MARK_GOODBLOCK(bbt, block) (bbt[block/8] = ~((~bbt[block/8]) | (1 << (block%8))))

#define FLASH_FTL_CORE_MAX_NUM_BUFS               (2)
#define FLASH_FTL_CORE_MAIN_BUF_DESC              (0)
#define FLASH_FTL_CORE_SPARE_BUF_DESC             (1)

/// initialize page data io vectors
#define INIT_PAGE_DATA_IOVEC(iovec,page_size,page,page_count)                 \
                                        iovec.main_size_bytes   = page_size;  \
                                        iovec.spare_size_bytes  = 0;          \
                                        iovec.total_page_count  = page_count; \
                                        iovec.start_page        = page;


/// initialize page data descriptors
#define INIT_BUF_DESC(desc, iovec, data)                                      \
      desc[FLASH_FTL_CORE_MAIN_BUF_DESC].user        = iovec.total_page_count;     \
      desc[FLASH_FTL_CORE_MAIN_BUF_DESC].size        = iovec.main_size_bytes;      \
      desc[FLASH_FTL_CORE_MAIN_BUF_DESC].VirtualAddr = (UINTN)data;               \
      desc[FLASH_FTL_CORE_SPARE_BUF_DESC].user       = iovec.total_page_count;     \
      desc[FLASH_FTL_CORE_SPARE_BUF_DESC].size       = 0;                          \
      desc[FLASH_FTL_CORE_SPARE_BUF_DESC].VirtualAddr= (0xFFFFFFFF);

#define FLASH_FTL_CORE_MAX_MULTI_PAGE (64)

/****************************************************************
 * Local Functions
 ***************************************************************/

static FLASH_FTL_STATUS
flash_ftl_core_update_part_info(ftl_pvt_ctx_t *client, const uint8 *part_name)
{
  struct nand_info info;
  enum nand_block_state block_state;
  uint32 block = 0, end_block = 0;
  uint32 total_bb_count = 0, total_good_block_count = 0;
  uint32 img_block_count = 0;
  uint32 exclude_raw_all_parti = 0xFF;

  /* get info from the handle and fill client context */
  nand_get_info(client->flash_handle, &info);
  if(!info.device_name)
  {
    return FLASH_FTL_FAIL;
  }

  /* fill partition specific info in client ctx */
  if(FLASH_FTL_OK != flash_ftl_memscpy(  client->info.device_name,
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
      (uint8*)flash_ftl_malloc(FLASH_FTL_CORE_BBT_SIZE(info.block_count));
  if(NULL == client->bbt)
  {
    return FLASH_FTL_OUT_OF_MEM;
  }
  flash_ftl_memset(client->bbt, 0, FLASH_FTL_CORE_BBT_SIZE(info.block_count));

  /* ------------ BAD BLOCK TABLE START ------------ */
  block = 0;  /* start block of this partition */
  end_block = block + info.block_count;

  /* Check if raw "all" partition is being opened */
  if (FLASH_FTL_OK != flash_ftl_strcmp((const uint8*)"all",
                                       (uint8*)part_name,
                                       (uint32*)&exclude_raw_all_parti))
  {
    //printf("strcmp failure \n");
    return FLASH_FTL_FAIL;
  }
  
  if (exclude_raw_all_parti)
  {
    /* now check for good and bad blocks */
    for ( ; block < end_block ; block++ )
    {
      if (NAND_DEVICE_DONE == nand_block_get_state(client->flash_handle,
                                                   block,
                                                   &block_state))
      {
        /* if block good update total_block_count */
        if (NAND_BLOCK_OK == block_state)
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
  }
  /* Skip bbt init for raw "all" partition, since clients not expected to access nand
       device via this internal user partition. */
  else
  {
    total_good_block_count = info.block_count;
  }

  if(total_bb_count == info.block_count)
  {
    /* all blocks are bad */
    return FLASH_FTL_OUT_OF_GOOD_BLOCKS;
  }
  /* ------------ BAD BLOCK TABLE END ------------ */


  img_block_count = FLASH_FTL_CORE_GOOD_BLOCKS(total_good_block_count);

  if(total_bb_count > (total_good_block_count-img_block_count))
  {
    return FLASH_FTL_OUT_OF_GOOD_BLOCKS;
  }

  client->info.erase_block_count = img_block_count;
  client->info.lba_count
              = client->info.erase_block_count * info.pages_per_block;
  client->info.lba_size_in_kbytes
              = (info.page_size_bytes / SIZE_OF_1_KBYTE);
  client->info.erase_block_size_in_kbytes
              = client->info.lba_size_in_kbytes * info.pages_per_block;

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
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;
  uint32 block_num = 0, i = 0;
  struct nand_info info;

  if((NULL == client) || (NULL == pba))
  {
    return FLASH_FTL_INVALID_PARAM;
  }

  /* get info from the handle and fill client context */
  nand_get_info(client->flash_handle, &info);

  /* get phys block (count good blocks and skip bad blocks) */
  for(i = 0, block_num = 0; i < info.block_count; i++)
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

  if(block_num >= info.block_count)
  {
    /* given lba is not a valid block */
    ret_status = FLASH_FTL_OUT_OF_GOOD_BLOCKS;
  }
  else
  {
    if( i >= info.block_count )
    {
      /* given lba is not a valid block */
      ret_status = FLASH_FTL_INVALID_PARAM;
    }
  }

  return ret_status;
}

static FLASH_FTL_STATUS
flash_ftl_core_mark_bad_block(ftl_pvt_ctx_t *client, uint32 block)
{
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;

  ret_status = nand_block_set_state( client->flash_handle,
                                      block, NAND_BLOCK_BAD);
  if (ret_status != NAND_DEVICE_DONE)
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

FLASH_FTL_STATUS
flash_ftl_core_api_set_block(ftl_pvt_ctx_t *client, uint32 block, enum nand_block_state block_state)
{
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;

  if (NAND_BLOCK_BAD == block_state)
  {
    /* mark block as bad in our BBT anyways */
    FLASH_FTL_CORE_MARK_BADBLOCK(client->bbt, block);
  }
  else if (NAND_BLOCK_OK == block_state)
  {
    /* mark block as good in our BBT anyways */
    FLASH_FTL_CORE_MARK_GOODBLOCK(client->bbt, block);
  }
  else
  {
    ret_status = FLASH_FTL_FAIL;
  }

  return ret_status;
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

  /* call open to get a valid handle on a given partition */
  ret_status = nand_open( 0, part_name, (nand_handle_t)&client->flash_handle);
  if(NAND_DEVICE_DONE != ret_status)
  {
    return FLASH_FTL_FAIL;
  }

  /* check partition for good blocks and update bad block info in BBT */
  ret_status = flash_ftl_core_update_part_info(client, part_name);
  if(FLASH_FTL_OK != ret_status)
  {
    flash_ftl_free(client);
    return ret_status;
  }

  client->dma_buffer = flash_ftl_aligned_malloc(
                        client->info.lba_size_in_kbytes * SIZE_OF_1_KBYTE);
  if(NULL == client->dma_buffer)
  {
    flash_ftl_free(client->bbt);
    flash_ftl_free(client->flash_handle);
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
  nand_close(client->flash_handle);
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
flash_ftl_core_read_lbas(ftl_pvt_ctx_t *client, uint32 start_page, uint32 num_of_pages, uint8 *data)
{

  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;
  uint32 start_block_num = 0, tmp_block_num = 0, pba = 0;
  uint32 page_size = 0, page_count = 0, total_pages_to_read=num_of_pages;
  //struct flash_mem_desc_buf page_buff_desc[FLASH_FTL_CORE_MAX_NUM_BUFS];
  //struct flash_page_iovec  page_data_iovec;
  struct nand_info info;

  if((NULL == client) || (NULL == data) || (0 == num_of_pages))
  {
    return FLASH_FTL_INVALID_PARAM;
  }

  /* get info from the handle and fill client context */
  nand_get_info(client->flash_handle, &info);

  /* get phys block (count good blocks and skip bad blocks) */
  start_block_num = (start_page/info.pages_per_block);
  ret_status = flash_ftl_core_get_pba_from_lba(client, start_block_num, &pba);
  if(FLASH_FTL_OK != ret_status)
  {
    return ret_status;
  }

  /* Initialize the page data IOVEC structure */
  page_size = info.page_size_bytes;
  start_page += (pba - start_block_num) * info.pages_per_block;

  page_count = (pba+1)*info.pages_per_block - start_page;
  if(page_count > total_pages_to_read)
  {
    page_count = total_pages_to_read;
  }

  //INIT_PAGE_DATA_IOVEC(page_data_iovec, page_size, start_page, page_count);

  /* Initialize buffer descriptors */
  //INIT_BUF_DESC(page_buff_desc, page_data_iovec, data);

  while( total_pages_to_read )
  {
    ret_status = nand_read(client->flash_handle,
                           NAND_ACCESS_MAIN,
                           start_page,
                           page_count, data, (page_count * page_size));

    if (ret_status != NAND_DEVICE_DONE)
    {
      if(NAND_DEVICE_FAIL_PAGE_ERASED != ret_status)
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
    //page_data_iovec.start_page += page_count;
    start_page += page_count;

    /* check if new start page is in next block */
    //tmp_block_num = page_data_iovec.start_page / info.pages_per_block;
    tmp_block_num = start_page / info.pages_per_block;

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

      //page_data_iovec.start_page += (pba-tmp_block_num)*info.pages_per_block;
      start_page += (pba-tmp_block_num)*info.pages_per_block;
    }

    /* update dest for next read */
    //page_buff_desc[FLASH_FTL_CORE_MAIN_BUF_DESC].VirtualAddr += (page_size*page_count);
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
    //page_data_iovec.total_page_count  = page_count;
    //page_buff_desc[FLASH_FTL_CORE_MAIN_BUF_DESC].user  = page_count;
    //page_buff_desc[FLASH_FTL_CORE_SPARE_BUF_DESC].user = page_count;
  }

  return FLASH_FTL_OK;
}

FLASH_FTL_STATUS
flash_ftl_core_write_lbas( ftl_pvt_ctx_t *client, uint32 start_page, uint32 num_of_pages, uint8 *data)
{

  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;
  uint32 start_block_num = 0, pba = 0, page_size = 0, pages_written_in_bb = 0;
  uint32 pages_per_block = 0, page_count = 0;
  uint32 total_pages_to_write = num_of_pages, tmp_block_num = 0, tmp_start_page = start_page;
 // struct flash_mem_desc_buf page_buff_desc[FLASH_FTL_CORE_MAX_NUM_BUFS];
 // struct flash_page_iovec  page_data_iovec;
  struct nand_info info;

  if((NULL == client) || (NULL == data) || (0 == num_of_pages))
  {
    return FLASH_FTL_INVALID_PARAM;
  }

  /* get info from the handle and fill client context */
  nand_get_info(client->flash_handle, &info);

  /* from the given page addr get the block which it is in */
  pages_per_block = info.pages_per_block;
  start_block_num = (start_page/pages_per_block);
  /* get phys block (count good blocks and skip bad blocks) */
  ret_status = flash_ftl_core_get_pba_from_lba(client, start_block_num, &pba);
  if(FLASH_FTL_OK != ret_status)
  {
    return ret_status;
  }

  /* Initialize the page data IOVEC structure */
  page_size = client->info.lba_size_in_kbytes*SIZE_OF_1_KBYTE;
  /* fix start page acc. to phys block */
  tmp_start_page += (pba-start_block_num)*pages_per_block;
  page_count = (pba+1)*info.pages_per_block - tmp_start_page;

  if(page_count > total_pages_to_write)
  {
    page_count = total_pages_to_write;
  }
  tmp_block_num = tmp_start_page / pages_per_block;

  //INIT_PAGE_DATA_IOVEC( page_data_iovec, page_size, tmp_start_page, page_count);

  /* Initialize buffer descriptors */
  //INIT_BUF_DESC(page_buff_desc, page_data_iovec, data);

  while( total_pages_to_write )
  {
#ifdef FLASH_FTL_ENABLE_ERASE_BEFORE_WRITE
	ret_status = nand_erase(client->flash_handle,
                                   tmp_block_num, 1);
    if (ret_status == NAND_DEVICE_DONE)
    {
      ret_status = nand_write(client->flash_handle,
                                     NAND_ACCESS_MAIN,
                                     tmp_start_page,
                                     page_count, data, (page_count * page_size));
    }
#else
    ret_status = nand_write(client->flash_handle,
                                   NAND_ACCESS_MAIN,
                                   tmp_start_page,
                                   page_count, data, (page_count * page_size));
#endif

    if (ret_status != NAND_DEVICE_DONE)
    {
      if(NAND_DEVICE_INVALID_PARAMETER == ret_status)
      {
        /* printf("[%s,%d] erase failed !!\n", __func__, __LINE__); */
        /* something went wrong or invalid parameter passed */
        return FLASH_FTL_INVALID_PARAM;
      }

      /* else write failed so mark the whole block as bad */
      //tmp_block_num = page_data_iovec.start_page/pages_per_block;
      tmp_block_num = tmp_start_page/pages_per_block;

      flash_ftl_core_mark_bad_block(client, tmp_block_num);

      /* start over again with next good block */
      ret_status = flash_ftl_core_get_pba_from_lba( client, start_block_num, &pba );
      if(FLASH_FTL_OK != ret_status)
      {
        /* can't get next good block */
        return ret_status;
      }

      /* reset and copy pages already written in bad block to a good block */
      if(tmp_block_num*pages_per_block > start_page)
      {
        //pages_written_in_bb = page_data_iovec.start_page - tmp_block_num*pages_per_block;
        pages_written_in_bb = tmp_start_page - tmp_block_num*pages_per_block;
      }
      else
      {
        //pages_written_in_bb = page_data_iovec.start_page - start_page;
        pages_written_in_bb = tmp_start_page - start_page;
      }

      total_pages_to_write += pages_written_in_bb;
      data -= (pages_written_in_bb*page_size);
      tmp_start_page = pba*pages_per_block;
      page_count += pages_written_in_bb;
      //page_buff_desc[FLASH_FTL_CORE_MAIN_BUF_DESC].VirtualAddr -= (pages_written_in_bb*page_size);
      //page_data_iovec.start_page = pba*pages_per_block;
      //page_data_iovec.total_page_count  = page_count;
      //page_buff_desc[FLASH_FTL_CORE_MAIN_BUF_DESC].user  = page_count;
      //page_buff_desc[FLASH_FTL_CORE_SPARE_BUF_DESC].user = page_count;
    }
    else
    {
      /* update src for next write */
      total_pages_to_write -= page_count;
      if(0 == total_pages_to_write)
      {
        break;
      }

      //page_data_iovec.start_page += page_count;
      //tmp_block_num = page_data_iovec.start_page/pages_per_block;
      tmp_start_page += page_count;
	  tmp_block_num = tmp_start_page/pages_per_block;
      if(pba != tmp_block_num)
      {
        start_block_num++;
        ret_status = flash_ftl_core_get_pba_from_lba(client, start_block_num, &pba);
        if(FLASH_FTL_OK != ret_status)
        {
          return ret_status;
        }
        //page_data_iovec.start_page += (pba-tmp_block_num)*pages_per_block;
        tmp_start_page += (pba-tmp_block_num)*pages_per_block;
      }

      /* update dest for next read */
      //page_buff_desc[FLASH_FTL_CORE_MAIN_BUF_DESC].VirtualAddr += (page_size*page_count);
      data += (page_size*page_count);

      if(total_pages_to_write < FLASH_FTL_CORE_MAX_MULTI_PAGE)
      {
        page_count = total_pages_to_write;
      }
      else
      {
        page_count = FLASH_FTL_CORE_MAX_MULTI_PAGE;
      }
      //page_data_iovec.total_page_count  = page_count;
      //page_buff_desc[FLASH_FTL_CORE_MAIN_BUF_DESC].user  = page_count;
      //page_buff_desc[FLASH_FTL_CORE_SPARE_BUF_DESC].user = page_count;
    }

    /* update block number to erase based on next available page to be written */	
    tmp_block_num = tmp_start_page/pages_per_block;
  }

  return FLASH_FTL_OK;
}

FLASH_FTL_STATUS
flash_ftl_core_get_block_count(ftl_pvt_ctx_t *handle, uint32 *erase_block_count)
{
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;
  ftl_pvt_ctx_t *client = (ftl_pvt_ctx_t *)handle;
  struct nand_info info;
  
  if(!handle || (erase_block_count == NULL))
  {
    ret_status = FLASH_FTL_INVALID_PARAM;
    return ret_status;
  }

  /* get info from the handle and fill client context */
  nand_get_info(client->flash_handle, &info);
  *erase_block_count = info.block_count;

  return ret_status;
}

FLASH_FTL_STATUS
flash_ftl_core_erase_block(ftl_pvt_ctx_t *handle, uint32 erase_block, uint32 erase_block_count)
{
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;
  ftl_pvt_ctx_t *client = (ftl_pvt_ctx_t *)handle;
  uint32 i = 0, pba = 0;
  struct nand_info info;

  if(!handle || !erase_block_count)
  {
    return FLASH_FTL_INVALID_PARAM;
  }

  /* get info from the handle and fill client context */
  nand_get_info(client->flash_handle, &info);

  for(i=0; i < info.block_count; i++)
  {
    /* get actual block to erase */
    ret_status = flash_ftl_core_get_pba_from_lba(client, erase_block, &pba);
    if(FLASH_FTL_OK != ret_status)
    {
      break;
    }

    ret_status = nand_erase(client->flash_handle, pba, 1);
    if(NAND_DEVICE_DONE != ret_status)
    {
      if(NAND_DEVICE_INVALID_PARAMETER != ret_status)
      {
        /* erase block failed so mark it as bad */
        flash_ftl_core_mark_bad_block(client, pba);
      }
      else
      {
        /* printf("[%s,%d] erase failed !!\n", __func__, __LINE__); */
        /* something went wrong or invalid parameter passed */
        ret_status = FLASH_FTL_INVALID_PARAM;
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
    ret_status = FLASH_FTL_OUT_OF_GOOD_BLOCKS;
  }

  return FLASH_FTL_OK;
}
