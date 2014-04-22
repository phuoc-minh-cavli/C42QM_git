/**********************************************************************
 * NandFtlApi.c
 *
 * This layer provides utilities tools for FTL core APIs
 *
 *
 * Copyright (c) 2016 QUALCOMM Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *
 **********************************************************************/
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
 * 2016-12-02   svl     used uncached dma_buffer instead of cached
                        temp_buffer in flash_ftl_read_bytes
 * 2016-11-11   svl     Review comments incorporated.
 * 2016-10-03   svl     cleared client handle in close, added comment in read.
 * 2016-09-14   svl     FTL API layer support added
 *=======================================================================*/

#include <NandFtl.h>
#include <NandFtlTypes.h>
#include <NandFtlApi.h>
#include "NandFtlOsal.h"
#include "NandFtlUtil.h"
#include "NandFtlCore.h"

/*****************************************************************************
 * Data structure definitions
 *****************************************************************************/

#define FLASH_FTL_ENTER()                         \
      do                                          \
      {                                           \
        if(FALSE == flash_ftl_initialized)        \
        {                                         \
          return FLASH_FTL_NOT_INIT;              \
        }                                         \
        flash_ftl_osal_enter_critical_section();  \
      } while (0)

#define FLASH_FTL_EXIT(status)                    \
      do                                          \
      {                                           \
        if(TRUE == flash_ftl_initialized)         \
        {                                         \
          flash_ftl_osal_leave_critical_section();\
        }                                         \
        return status;                            \
      } while (0)

static uint32        flash_ftl_initialized=FALSE;
static ftl_pvt_ctx_t   *flash_ftl_clients[FLASH_FTL_MAX_PARTITIONS_SUPPORTED];

/*****************************************************************************
 * APIs
 *****************************************************************************/

FLASH_FTL_STATUS
flash_ftl_read_lba( flash_ftl_client_t handle,
                    uint32 start_lba,
                    uint32 lba_count,
                    uint8  *data)
{
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;

  FLASH_FTL_ENTER();

  if(!handle || !data || !lba_count)
  {
    FLASH_FTL_EXIT(FLASH_FTL_INVALID_PARAM);
  }

  ret_status = flash_ftl_core_read_lbas((ftl_pvt_ctx_t *)handle,
                                        start_lba, lba_count, data);
  FLASH_FTL_EXIT(ret_status);
}

FLASH_FTL_STATUS
flash_ftl_write_lba(  flash_ftl_client_t handle,
                      uint32 start_lba,
                      uint32 lba_count,
                      uint8 *data)
{
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;

  FLASH_FTL_ENTER();

  if(!handle || !data || !lba_count)
  {
    FLASH_FTL_EXIT(FLASH_FTL_INVALID_PARAM);
  }

  ret_status = flash_ftl_core_write_lbas((ftl_pvt_ctx_t *)handle,
                                          start_lba, lba_count, data);
  FLASH_FTL_EXIT(ret_status);
}

FLASH_FTL_STATUS
flash_ftl_erase_block(  flash_ftl_client_t handle,
                        uint32 start_block,
                        uint32 block_count)
{
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;
  ftl_pvt_ctx_t *client = (ftl_pvt_ctx_t *)handle;
  uint32 erase_block_count = 0;

  FLASH_FTL_ENTER();

  if(!handle || !block_count)
  {
    FLASH_FTL_EXIT(FLASH_FTL_INVALID_PARAM);
  }

  /* Erase all reserve good blocks if all available good blocks via ftl are 
   * requested to be erased. */ 
  if((start_block == 0) && (block_count == client->info.erase_block_count))
  {
    flash_ftl_core_get_block_count((ftl_pvt_ctx_t *)handle, &erase_block_count);
  }
  else
  {
    erase_block_count = block_count;
  }

  ret_status = flash_ftl_core_erase_block((ftl_pvt_ctx_t *)handle,
                                          start_block, erase_block_count);

  FLASH_FTL_EXIT(ret_status);
}

FLASH_FTL_STATUS
flash_ftl_read_bytes(flash_ftl_client_t handle,
                                      uint32 offset_from_part_in_bytes,
                                      uint32 data_size_in_bytes,
                                      uint8 *data)
{
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;
  ftl_pvt_ctx_t *client = (ftl_pvt_ctx_t *)handle;
  uint8 *temp_data_ptr = data;
  uint32 start_lba = 0, lba_count = 0, offset_from_start_of_lba = 0;
  uint32 lba_size = 0, part_size_in_bytes = 0, tmp_data_size = 0;

  FLASH_FTL_ENTER();

  if((NULL == handle) || (NULL == data) || (0 == data_size_in_bytes))
  {
    FLASH_FTL_EXIT(FLASH_FTL_INVALID_PARAM);
  }

  /* get info from the handle and fill client context */
  lba_size = client->info.lba_size_in_kbytes * SIZE_OF_1_KBYTE;
  part_size_in_bytes = client->info.lba_count * lba_size;

  if(part_size_in_bytes < data_size_in_bytes)
  {
    FLASH_FTL_EXIT(FLASH_FTL_FAIL);
  }

  if(
      (offset_from_part_in_bytes >= part_size_in_bytes) ||
      (offset_from_part_in_bytes > (part_size_in_bytes - data_size_in_bytes))
    )
  {
    FLASH_FTL_EXIT(FLASH_FTL_INVALID_PARAM);
  }

  /*
    Explanation for below conditions :
    This use cases show that offset can be lba aligned or unaligned,
    so can be the size.

        1. The first loop checks if offset is not lba aligned and reads data till
        start of next lba.
            <-------LBA-------><-------LBA-------><-------LBA------->
            ___________________ __________________ _________________
           |   |   |           |                  |                 |
           |   |<1>|   LBA     |      LBA         |      LBA        |
           |___|___|___________|__________________|_________________|


        2. The second loop reads data in multiples of lbas.
            ___________________ __________________ _________________
           |                   |                  |                 |
           |<-------------2--->|      LBA         |      LBA        |
           |___________________|__________________|_________________|

        3. If there is any data remaining after second loop it should be less than
        an lba. The third loop reads this remaining data.
            ___________________ __________________ _________________
           |                   |       |          |                 |
           |                   |<--3-->|    LBA   |      LBA        |
           |___________________|_______|__________|_________________|

        4. If the api is called with start offset lba unaligned and 
           data to be read is in multiples of lba then below is the
           flow.
            ___________________ __________________ _________________
           |   |               |                  |           |     |
           |   |<---------1--->|<-----LBA--2----->|<--3--LBA->|     |
           |___|_______________|__________________|___________|_____|

  */

  /*-------------CONDITION SHOWN IN 1st DIAGRAM-----------------*/
  offset_from_start_of_lba = offset_from_part_in_bytes%lba_size;
  if(offset_from_start_of_lba)
  {
    /* read single lba if offset is not lba aligned */
    /* read in temp buffer and then copy relevent data to user buffer */
    start_lba = offset_from_part_in_bytes/lba_size;
    lba_count = 1;

    flash_ftl_memset(client->dma_buffer, 0xAA, lba_size);
    ret_status = flash_ftl_core_read_lbas
                  (client, start_lba, lba_count, client->dma_buffer);
    if(FLASH_FTL_OK != ret_status)
    {
      FLASH_FTL_EXIT(ret_status);
    }

    tmp_data_size = lba_size - offset_from_start_of_lba;
    if(data_size_in_bytes <= tmp_data_size)
    {
      /* reading data less than a lba
         from an offset not aligned to lba size */
      ret_status = flash_ftl_memscpy(
                      temp_data_ptr,
                      data_size_in_bytes,
                      (client->dma_buffer + offset_from_start_of_lba),
                      data_size_in_bytes
                  );
      if(FLASH_FTL_OK != ret_status)
      {
        FLASH_FTL_EXIT(FLASH_FTL_FAIL);
      }

      /* we have read all data now, so just return from the func */
      FLASH_FTL_EXIT(FLASH_FTL_OK);
    }
    else
    {
      ret_status = flash_ftl_memscpy(
                        temp_data_ptr,
                        tmp_data_size,
                        (client->dma_buffer + offset_from_start_of_lba),
                        tmp_data_size
                   );
      if(FLASH_FTL_OK != ret_status)
      {
        FLASH_FTL_EXIT(FLASH_FTL_FAIL);
      }

      /* update data ptr */
      temp_data_ptr += tmp_data_size;
      data_size_in_bytes -= tmp_data_size;
      offset_from_part_in_bytes += tmp_data_size;
    }
  }

  /*-------------CONDITION SHOWN IN 2nd DIAGRAM-----------------*/
  /* check if remaining data is in multiple of lba_size */
  lba_count = (data_size_in_bytes/lba_size);
  if(lba_count)
  {
    /* read data in multiple of lbas */
    start_lba = offset_from_part_in_bytes/lba_size;
    ret_status = flash_ftl_core_read_lbas(client, start_lba,
                                          lba_count, temp_data_ptr);
    if(FLASH_FTL_OK != ret_status)
    {
      FLASH_FTL_EXIT(ret_status);
    }

    /* update data ptr */
    temp_data_ptr += (lba_count * lba_size);
    data_size_in_bytes -= (lba_count * lba_size);
    offset_from_part_in_bytes += (lba_count * lba_size);
  }

  /*-------------CONDITION SHOWN IN 3rd DIAGRAM-----------------*/
  /* check for remaining data */
  if(data_size_in_bytes)
  {
    /* this means there is data less than a lba yet to be read */
    /* so read single lba for remaining data */
    start_lba = offset_from_part_in_bytes/lba_size;
    lba_count = 1;
    flash_ftl_memset(client->dma_buffer, 0xBB, lba_size);
    ret_status = flash_ftl_core_read_lbas(client, start_lba,
                                          lba_count, client->dma_buffer);
    if(FLASH_FTL_OK != ret_status)
    {
      FLASH_FTL_EXIT(ret_status);
    }

    ret_status =  flash_ftl_memscpy(temp_data_ptr, data_size_in_bytes,
                                    client->dma_buffer, data_size_in_bytes);
    if(FLASH_FTL_OK != ret_status)
    {
      FLASH_FTL_EXIT(FLASH_FTL_FAIL);
    }
  }

  FLASH_FTL_EXIT(ret_status);
}

FLASH_FTL_STATUS flash_ftl_init(void)
{
  flash_ftl_osal_init();
  flash_ftl_initialized = TRUE;
  return FLASH_FTL_OK;
}

FLASH_FTL_STATUS flash_ftl_deinit(void)
{
  if(TRUE == flash_ftl_initialized)
  {
    flash_ftl_initialized = FALSE;
    flash_ftl_osal_deinit();
  }
  return FLASH_FTL_OK;
}

FLASH_FTL_STATUS flash_ftl_get_info(flash_ftl_client_t handle,
                                    flash_ftl_info_t *info)
{
  ftl_pvt_ctx_t *client;

  FLASH_FTL_ENTER();

  if ((NULL == handle) || (NULL == info) || (NULL == info->device_name))
  {
    FLASH_FTL_EXIT(FLASH_FTL_INVALID_PARAM);
  }

  /* validate handle */
  client = (ftl_pvt_ctx_t *)handle;

  /* get info from the handle and fill client context */
  if(FLASH_FTL_OK != flash_ftl_memscpy(
                     info->device_name,
                     flash_ftl_strlen((const uint8 *)client->info.device_name),
                     client->info.device_name,
                     flash_ftl_strlen((const uint8 *)client->info.device_name)
                    )
    )
  {
    FLASH_FTL_EXIT(FLASH_FTL_FAIL);
  }

  info->maker_id                   = client->info.maker_id;
  info->device_id                  = client->info.device_id;
  info->lba_count                  = client->info.lba_count;
  info->lba_size_in_kbytes         = client->info.lba_size_in_kbytes;
  info->erase_block_count          = client->info.erase_block_count;
  info->erase_block_size_in_kbytes = client->info.erase_block_size_in_kbytes;

  FLASH_FTL_EXIT(FLASH_FTL_OK);
}

FLASH_FTL_STATUS flash_ftl_open( flash_ftl_client_t *handle,
                                   const uint8 *part_name)
{
  uint8 i = 0, temp_id = 0;
  ftl_pvt_ctx_t *temp_client = NULL;
  uint32 part_name_len = 0;
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;

  if((NULL == part_name) || (NULL == handle) )
  {
    FLASH_FTL_EXIT(FLASH_FTL_INVALID_PARAM);
  }

  part_name_len = flash_ftl_strlen((const uint8*)part_name);
  if(0 == part_name_len)
  {
    FLASH_FTL_EXIT(FLASH_FTL_INVALID_PARAM);
  }

  /* init ftl if not inited already */
  if(FALSE == flash_ftl_initialized)
  {
    ret_status = flash_ftl_init();
    if(FLASH_FTL_OK != ret_status)
    {
      return ret_status;
    }
  }

  FLASH_FTL_ENTER();

  *handle = NULL;
  temp_id = FLASH_FTL_MAX_PARTITIONS_SUPPORTED;
  for(i = 0; i < FLASH_FTL_MAX_PARTITIONS_SUPPORTED ; i++)
  {
    if(NULL != flash_ftl_clients[i])
    {
      /* clients will share handle for the same partition */
      /* so if req. partition's already opened,
         simply return existing handle */
      if(
          FLASH_FTL_OK != flash_ftl_strcmp(part_name,
                                           flash_ftl_clients[i]->part_name,
                                           (uint32*)&ret_status )
      )
      {
        break;
      }
      if(0 == ret_status)
      {
        *handle = (flash_ftl_client_t)flash_ftl_clients[i];
        flash_ftl_clients[i]->clnt_cnt++;
        ret_status = FLASH_FTL_OK;
        temp_id = i;
        break;
      }
    }
    else
    {
      /* save the empty entry to use later if req. part is not opened yet */
      if(FLASH_FTL_MAX_PARTITIONS_SUPPORTED == temp_id)
        temp_id = i;
    }
  }

  if(FLASH_FTL_MAX_PARTITIONS_SUPPORTED <= temp_id)
  {
    /*
      reached here so it means all partitions are aready opened
      and that the given partition name is not present in opened partitions
      so return failure to open given partition
    */
    if(FLASH_FTL_OK == ret_status)
    {
      ret_status = FLASH_FTL_FAIL;
    }
  }
  else
  {
    /*
      reached here so it means we either have a handle
      or we have to open a partition
    */
    if(NULL == *handle)
    {
      /*
        reached here that means the req. partition is not opened yet
        allocate memory for this partition
      */
      ret_status = flash_ftl_core_open(&temp_client, part_name);
      if(FLASH_FTL_OK == ret_status)
      {
        /* save the client handle for future ref. */
        flash_ftl_clients[temp_id] = temp_client;
        flash_ftl_clients[temp_id]->clnt_cnt = 1;

        ret_status = flash_ftl_memscpy(
                          flash_ftl_clients[temp_id]->part_name,
                          flash_ftl_strlen((const uint8*)part_name),
                          part_name,
                          flash_ftl_strlen((const uint8*)part_name));
        if(FLASH_FTL_OK != ret_status)
        {
          FLASH_FTL_EXIT(ret_status);
        }

        *handle = (flash_ftl_client_t)temp_client;
      }
    }
  }

  FLASH_FTL_EXIT(ret_status);
}

FLASH_FTL_STATUS flash_ftl_close( flash_ftl_client_t *handle )
{
  uint32 i = 0;
  ftl_pvt_ctx_t *client=NULL;

  if((NULL == handle) ||  (NULL == *handle))
  {
    return FLASH_FTL_INVALID_PARAM;
  }

  client = (ftl_pvt_ctx_t *)handle;
  FLASH_FTL_ENTER();

  for(i = 0; i < FLASH_FTL_MAX_PARTITIONS_SUPPORTED; i++)
  {
    if(client == flash_ftl_clients[i])
    {
      /* found client */
      break;
    }
  }

  if( i >= FLASH_FTL_MAX_PARTITIONS_SUPPORTED )
  {
    // passed already closed handle or invalid handle
    FLASH_FTL_EXIT(FLASH_FTL_INVALID_PARAM);
  }

  if(client->clnt_cnt)
  {
    client->clnt_cnt--;
  }

  if(0 == client->clnt_cnt)
  {
    /* close partition */
    flash_ftl_core_close(&flash_ftl_clients[i]);
    for(i = 0; i < FLASH_FTL_MAX_PARTITIONS_SUPPORTED; i++)
    {
      if(flash_ftl_clients[i] != NULL)
      {
        break;
      }
    }
    if(FLASH_FTL_MAX_PARTITIONS_SUPPORTED == i)
    {
      /* all partitions are closed */
      flash_ftl_deinit();
    }
  }

  *handle = NULL;

  FLASH_FTL_EXIT(FLASH_FTL_OK);
}

FLASH_FTL_STATUS flash_ftl_api_deinit( void )
{
  uint32 i = 0;
  ftl_pvt_ctx_t *client=NULL;

  FLASH_FTL_ENTER();

  for(i = 0; i < FLASH_FTL_MAX_PARTITIONS_SUPPORTED; i++)
  {
    if(flash_ftl_clients[i] != NULL)
    {
      client = (ftl_pvt_ctx_t *)flash_ftl_clients[i];
      client->clnt_cnt = 0;
      /* close partition */
      flash_ftl_core_close(&flash_ftl_clients[i]);
    }
  }

  /* all partitions are closed */
  flash_ftl_deinit();

  FLASH_FTL_EXIT(FLASH_FTL_OK);
}

FLASH_FTL_STATUS
flash_ftl_api_set_block( nand_handle_t nand_handle,
                            uint32 block_num,
                            enum nand_block_state block_state)
{
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;
  uint32 i, temp_id;
  UINTN ftl_nand_handle = 0, io_nand_handle = 0;

  FLASH_FTL_ENTER();

  if(!nand_handle)
  {
    FLASH_FTL_EXIT(FLASH_FTL_INVALID_PARAM);
  }
  io_nand_handle = *(UINTN*)nand_handle;
  
  temp_id = FLASH_FTL_MAX_PARTITIONS_SUPPORTED;
  for(i = 0; i < FLASH_FTL_MAX_PARTITIONS_SUPPORTED ; i++)
  {
    if(NULL != flash_ftl_clients[i])
    {
      /* clients will share handle for the same partition */
      /* so if req. partition's already opened, simply use existing handle */
      ftl_nand_handle = *(UINTN*)flash_ftl_clients[i]->flash_handle;
      if(ftl_nand_handle == io_nand_handle)
      {
        temp_id = i;
        break;
      }
    }
  }
  
  if (temp_id < FLASH_FTL_MAX_PARTITIONS_SUPPORTED)
  {
    /* mark the whole block as bad */
    ret_status = flash_ftl_core_api_set_block(flash_ftl_clients[temp_id], 
                                              block_num,
                                              block_state);
  }
  else
  {
    ret_status = FLASH_FTL_OK;
  }

  FLASH_FTL_EXIT(ret_status);
}

