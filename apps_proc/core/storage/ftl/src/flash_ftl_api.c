/**********************************************************************
 * flash_ftl_api.c
 *
 * This layer provides utilities tools for FTL core APIs
 *
 *
 * Copyright (c) 2016-2017, 2019-2020 QUALCOMM Technologies Incorporated.
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
 * 2020-03-16   sl      Added boundary check for partition name length.
 * 2019-01-24   sl      Correct parameter check.
 * 2019-01-09   sl      Added parameter check.
 * 2017-03-10   svl     Added parameter check for read and write APIs.
 * 2016-12-02   svl     Used uncached dma_buffer instead of cached
                        temp_buffer in flash_ftl_read_bytes.
 * 2016-11-11   svl     Review comments incorporated.
 * 2016-10-03   svl     Cleared client handle in close, added comment in read.
 * 2016-09-14   svl     FTL API layer support added.
 *=======================================================================*/

#include <flash_ftl.h>
#include <flash_ftl_types.h>
#include "flash_ftl_osal.h"
#include "flash_ftl_util.h"
#include "flash_ftl_core.h"

/*****************************************************************************
 * Data structure definitions
 *****************************************************************************/

#define FLASH_FTL_ENTER_CRIT_SECT()                         \
      do                                          \
      {                                           \
        if(FALSE == flash_ftl_initialized)        \
        {                                         \
          return FLASH_FTL_NOT_INIT;              \
        }                                         \
        flash_ftl_osal_enter_critical_section();  \
      } while (0)

#define FLASH_FTL_EXIT_CRIT_SECT(status)                    \
      do                                          \
      {                                           \
        if(TRUE == flash_ftl_initialized)         \
        {                                         \
          flash_ftl_osal_leave_critical_section();\
        }                                         \
        return status;                            \
      } while (0)

uint32        flash_ftl_initialized=FALSE;
ftl_pvt_ctx_t *flash_ftl_clients[FLASH_FTL_MAX_PARTITIONS_SUPPORTED];

/*****************************************************************************
 * APIs
 *****************************************************************************/

FLASH_FTL_STATUS
flash_ftl_read_lpa( flash_ftl_client_t handle,
                    uint32 start_lpa,
                    uint32 lpa_count,
                    uint8  *data)
{
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;
  ftl_pvt_ctx_t *client = (ftl_pvt_ctx_t *)handle;
  uint32 total_avail_lpas = 0;

  FLASH_FTL_ENTER_CRIT_SECT();

  if(!handle || !data || !lpa_count)
  {
    FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_INVALID_PARAM);
  }

  total_avail_lpas = client->info.lpa_count;
  if(
      (start_lpa >= total_avail_lpas) ||
      (start_lpa > (total_avail_lpas - lpa_count))
    )
  {
    FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_INVALID_PARAM);
  }

  ret_status = flash_ftl_core_read_lpas((ftl_pvt_ctx_t *)handle,
                                        start_lpa, lpa_count, data);
  FLASH_FTL_EXIT_CRIT_SECT(ret_status);
}

FLASH_FTL_STATUS
flash_ftl_write_lpa(  flash_ftl_client_t handle,
                      uint32 start_lpa,
                      uint32 lpa_count,
                      uint8 *data)
{
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;
  uint32 total_avail_lpas = 0;

  FLASH_FTL_ENTER_CRIT_SECT();

  if(!handle || !data || !lpa_count)
  {
    FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_INVALID_PARAM);
  }

  total_avail_lpas = ((ftl_pvt_ctx_t *)handle)->info.lpa_count;
  if(
      (start_lpa >= total_avail_lpas) ||
      (start_lpa > (total_avail_lpas - lpa_count))
    )
  {
    FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_INVALID_PARAM);
  }

  ret_status = flash_ftl_core_write_lpas((ftl_pvt_ctx_t *)handle,
                                          start_lpa, lpa_count, data);
  FLASH_FTL_EXIT_CRIT_SECT(ret_status);
}

FLASH_FTL_STATUS
flash_ftl_erase_block(  flash_ftl_client_t handle,
                        uint32 start_block,
                        uint32 block_count)
{
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;
  ftl_pvt_ctx_t *client = (ftl_pvt_ctx_t *)handle;
  uint32 total_avail_blocks = 0;

  FLASH_FTL_ENTER_CRIT_SECT();

  if(!handle || !block_count)
  {
    FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_INVALID_PARAM);
  }

  total_avail_blocks = client->info.erase_block_count;

  if(
      (start_block >= total_avail_blocks) ||
      (start_block > (total_avail_blocks - block_count))
    )
  {
    FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_INVALID_PARAM);
  }

  ret_status = flash_ftl_core_erase_block((ftl_pvt_ctx_t *)handle,
                                          start_block,
                                          block_count);

  FLASH_FTL_EXIT_CRIT_SECT(ret_status);
}

FLASH_FTL_STATUS
flash_ftl_read_bytes( flash_ftl_client_t handle,
                      uint32 offset_from_part_in_bytes,
                      uint32 data_size_in_bytes,
                      uint8 *data)
{
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;
  ftl_pvt_ctx_t *client = (ftl_pvt_ctx_t *)handle;
  uint8 *temp_data_ptr = data;
  uint32 start_lpa = 0, lpa_count = 0, offset_from_start_of_lpa = 0;
  uint32 lpa_size = 0, part_size_in_bytes = 0, tmp_data_size = 0;

  FLASH_FTL_ENTER_CRIT_SECT();

  if((NULL == handle) || (NULL == data) || (0 == data_size_in_bytes))
  {
    FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_INVALID_PARAM);
  }

  /* get info from the handle and fill client context */
  lpa_size = client->info.lpa_size_in_kbytes * SIZE_OF_1_KBYTE;
  part_size_in_bytes = client->info.lpa_count * lpa_size;

  if(part_size_in_bytes < data_size_in_bytes)
  {
    FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_FAIL);
  }

  if(
      (offset_from_part_in_bytes >= part_size_in_bytes) ||
      (offset_from_part_in_bytes > (part_size_in_bytes - data_size_in_bytes))
    )
  {
    FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_INVALID_PARAM);
  }

  /*
    Explanation for below conditions :
    This use cases show that offset can be lpa aligned or unaligned,
    so can be the size.

        1. The first loop checks if offset is not lpa aligned and reads data till
        start of next lpa.
            <-------LPA-------><-------LPA-------><-------LPA------->
            ___________________ __________________ _________________
           |   |   |           |                  |                 |
           |   |<1>|   LPA     |      LPA         |      LPA        |
           |___|___|___________|__________________|_________________|


        2. The second loop reads data in multiples of lpas.
            ___________________ __________________ _________________
           |                   |                  |                 |
           |<-------------2--->|      LPA         |      LPA        |
           |___________________|__________________|_________________|

        3. If there is any data remaining after second loop it should be less than
        an lpa. The third loop reads this remaining data.
            ___________________ __________________ _________________
           |                   |       |          |                 |
           |                   |<--3-->|    LPA   |      LPA        |
           |___________________|_______|__________|_________________|

        4. If the api is called with start offset lpa unaligned and 
           data to be read is in multiples of lpa then below is the
           flow.
            ___________________ __________________ _________________
           |   |               |                  |           |     |
           |   |<---------1--->|<-----LPA--2----->|<--3--LPA->|     |
           |___|_______________|__________________|___________|_____|

  */

  /*-------------CONDITION SHOWN IN 1st DIAGRAM-----------------*/
  offset_from_start_of_lpa = offset_from_part_in_bytes%lpa_size;
  if(offset_from_start_of_lpa)
  {
    /* read single lpa if offset is not lpa aligned */
    /* read in temp buffer and then copy relevent data to user buffer */
    start_lpa = offset_from_part_in_bytes/lpa_size;
    lpa_count = 1;

    flash_ftl_memset(client->dma_buffer, 0xAA, lpa_size);
    ret_status = flash_ftl_core_read_lpas
                  (client, start_lpa, lpa_count, client->dma_buffer);
    if(FLASH_FTL_OK != ret_status)
    {
      FLASH_FTL_EXIT_CRIT_SECT(ret_status);
    }

    tmp_data_size = lpa_size - offset_from_start_of_lpa;
    if(data_size_in_bytes <= tmp_data_size)
    {
      /* reading data less than a lpa
         from an offset not aligned to lpa size */
      ret_status = flash_ftl_memscpy(
                      temp_data_ptr,
                      data_size_in_bytes,
                      (client->dma_buffer + offset_from_start_of_lpa),
                      data_size_in_bytes
                  );
      if(FLASH_FTL_OK != ret_status)
      {
        FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_FAIL);
      }

      /* we have read all data now, so just return from the func */
      FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_OK);
    }
    else
    {
      ret_status = flash_ftl_memscpy(
                        temp_data_ptr,
                        tmp_data_size,
                        (client->dma_buffer + offset_from_start_of_lpa),
                        tmp_data_size
                   );
      if(FLASH_FTL_OK != ret_status)
      {
        FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_FAIL);
      }

      /* update data ptr */
      temp_data_ptr += tmp_data_size;
      data_size_in_bytes -= tmp_data_size;
      offset_from_part_in_bytes += tmp_data_size;
    }
  }

  /*-------------CONDITION SHOWN IN 2nd DIAGRAM-----------------*/
  /* check if remaining data is in multiple of lpa_size */
  lpa_count = (data_size_in_bytes/lpa_size);
  if(lpa_count)
  {
    /* read data in multiple of lpas */
    start_lpa = offset_from_part_in_bytes/lpa_size;
    ret_status = flash_ftl_core_read_lpas(client, start_lpa,
                                          lpa_count, temp_data_ptr);
    if(FLASH_FTL_OK != ret_status)
    {
      FLASH_FTL_EXIT_CRIT_SECT(ret_status);
    }

    /* update data ptr */
    temp_data_ptr += (lpa_count * lpa_size);
    data_size_in_bytes -= (lpa_count * lpa_size);
    offset_from_part_in_bytes += (lpa_count * lpa_size);
  }

  /*-------------CONDITION SHOWN IN 3rd DIAGRAM-----------------*/
  /* check for remaining data */
  if(data_size_in_bytes)
  {
    /* this means there is data less than a lpa yet to be read */
    /* so read single lpa for remaining data */
    start_lpa = offset_from_part_in_bytes/lpa_size;
    lpa_count = 1;
    flash_ftl_memset(client->dma_buffer, 0xBB, lpa_size);
    ret_status = flash_ftl_core_read_lpas(client, start_lpa,
                                          lpa_count, client->dma_buffer);
    if(FLASH_FTL_OK != ret_status)
    {
      FLASH_FTL_EXIT_CRIT_SECT(ret_status);
    }

    ret_status =  flash_ftl_memscpy(temp_data_ptr, data_size_in_bytes,
                                    client->dma_buffer, data_size_in_bytes);
    if(FLASH_FTL_OK != ret_status)
    {
      FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_FAIL);
    }
  }

  FLASH_FTL_EXIT_CRIT_SECT(ret_status);
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

  FLASH_FTL_ENTER_CRIT_SECT();

  if ((NULL == handle) || (NULL == info))
  {
    FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_INVALID_PARAM);
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
    FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_FAIL);
  }

  info->maker_id                   = client->info.maker_id;
  info->device_id                  = client->info.device_id;
  info->lpa_count                  = client->info.lpa_count;
  info->lpa_size_in_kbytes         = client->info.lpa_size_in_kbytes;
  info->erase_block_count          = client->info.erase_block_count;
  info->erase_block_size_in_kbytes = client->info.erase_block_size_in_kbytes;

  FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_OK);
}

FLASH_FTL_STATUS flash_ftl_open(  flash_ftl_client_t *handle,
                                  const uint8 *part_name)
{
  uint8 i = 0, temp_id = 0;
  ftl_pvt_ctx_t *temp_client = NULL;
  uint32 part_name_len = 0;
  FLASH_FTL_STATUS ret_status = FLASH_FTL_OK;

  if((NULL == part_name) || (NULL == handle) )
  {
    FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_INVALID_PARAM);
  }

  part_name_len = flash_ftl_strlen((const uint8*)part_name);
  if((0 == part_name_len) || (part_name_len >= FTL_DEVICE_NAME_SIZE))
  {
    FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_INVALID_PARAM);
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

  FLASH_FTL_ENTER_CRIT_SECT();

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
                          (FTL_DEVICE_NAME_SIZE-1),
                          part_name,
                          part_name_len);
        flash_ftl_clients[temp_id]->part_name[part_name_len]=0;
        if(FLASH_FTL_OK != ret_status)
        {
          FLASH_FTL_EXIT_CRIT_SECT(ret_status);
        }

        *handle = (flash_ftl_client_t)temp_client;
      }
    }
  }

  FLASH_FTL_EXIT_CRIT_SECT(ret_status);
}

FLASH_FTL_STATUS flash_ftl_close( flash_ftl_client_t *handle )
{
  uint32 i = 0;
  ftl_pvt_ctx_t *client=NULL;

  if((NULL == handle) ||  (NULL == *handle))
  {
    return FLASH_FTL_INVALID_PARAM;
  }

  client = (ftl_pvt_ctx_t *)*handle;
  FLASH_FTL_ENTER_CRIT_SECT();

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
    /* passed already closed handle or invalid handle */
    FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_INVALID_PARAM);
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

  FLASH_FTL_EXIT_CRIT_SECT(FLASH_FTL_OK);
}
