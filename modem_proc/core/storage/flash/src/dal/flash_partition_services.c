/*=============================================================================
 *
 * FILE:      flash_partition_services.c
 *
 * DESCRIPTION: Function and data structure declarations specific to the NAND
 *             partition layer
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2013, 2018-2019 QUALCOMM Technologies Incorporated.
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
 *  $Header: //components/rel/core.mpss/3.10/storage/flash/src/dal/flash_partition_services.c#3 $ $DateTime: 2019/05/09 05:51:00 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 05/08/19     sl      Update smem with new partition table
 * 10/12/18     eo      Set mibib search parameters based on mibib partition info
 * 10/12/18     eo      Support partition names without "0:"
 * 10/01/13     sb      Add bad block management
 * 07/02/13     sb      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_nand_partition.h"
#include <string.h>
#include "flash_mibib.h"
#include "flash_miparti.h"
#include "flash_micrc.h"
#include "flash_dal_util.h"
#include "crc.h"

#include "flash_nand.h"
#include "flash_nand_entry.h"

#include "smem.h"
#define GET_RAW_PAGEBUF(client_ctxt) GET_PDATA(client_ctxt)->raw_page_buffer

#define GET_FLASH_DRV_MEMORY(client_ctxt)                          \
     GET_PDATA(client_ctxt)->flash_mem_info->handle_drv_mem

/* DAL Memory descriptor for page operations */
static DALSYS_MEM_DESC_LIST_OBJECT(parti_buff_desc, 2);
static struct flash_page_iovec page_data_iovec;

/*------------------------------------------------------------------------------
  Local declarations.
 -----------------------------------------------------------------------------*/

static __inline void init_page_param(flash_client_ctxt *client_ctxt,
  uint32 page, uint32 page_count, struct flash_page_iovec *page_data)
{
  /* This function assumes that we will do on FLASH_READ_MAIN
     in this file */
  page_data->main_size_bytes =
    client_ctxt->flash_dev_ctxt->dev_info.u.nand.page_size_bytes;
  page_data->spare_size_bytes = 0;
  page_data->start_page = page;
  page_data->total_page_count = page_count;
}

static __inline void init_buffer_descriptor(flash_client_ctxt *client_ctxt,
  uint32 buff_addr, uint32 page_count, dalsys_mem_desc_list *buffer_desc)
{
  DALSysMemDescBuf *desc_buff;

  DALFW_MemDescInit((void*)GET_FLASH_DRV_MEMORY(client_ctxt),
    (DALSysMemDescList *) buffer_desc, 2);

  desc_buff = DALFW_MemDescBufPtr(buffer_desc, 0);

  if (desc_buff)
  {
    /* set the size and address for main */
    desc_buff->VirtualAddr = buff_addr;
    desc_buff->PhysicalAddr = flash_vtop_get_physical(buff_addr);
    desc_buff->size = 
      client_ctxt->flash_dev_ctxt->dev_info.u.nand.page_size_bytes;
    desc_buff->user = page_count;

    /* set the size for spare to 0 */
    desc_buff++;
    desc_buff->size = 0;
    desc_buff->user = page_count;
  }
}

static int nand_parti_write_pages(flash_client_ctxt *client_ctxt, uint32 page,
  uint32 write_type, void *buffer)
{
  int result = FLASH_DEVICE_DONE;

  /* Initialize buffer descriptors */
  init_buffer_descriptor(client_ctxt, (uint32) buffer, 1,
    (DALSysMemDescList *) &parti_buff_desc);

  /* Initialize the page data IOVEC structure */
  init_page_param(client_ctxt, page, 1, &page_data_iovec);

  result = nand_entry_write_pages(client_ctxt, (enum page_write_opcode) write_type,
    (DALSysMemDescList *) &parti_buff_desc, &page_data_iovec);

  return result;
}

/*
 * This function compares a partition enty name to a string to find a match.
 */
static int nand_parti_name_matches(flash_partentry_t partentry,
  const unsigned char *name)
{
  if (strncmp(partentry->name, (const char *)name, 
    FLASH_PART_NAME_LENGTH) == 0)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*
 * This function checks the partition boundaries of MIBIB & EFS2.
 * Boundaries of these partitions shouldn’t be changing.
 */
static int nand_parti_check_boundaries(flash_partable_t parti1,
                            flash_partable_t parti2)
{
  uint16 i,j;

  for (i=0; i<parti1->numparts; i++)
  {
    if ((strncmp((const char *)(&parti1->part_entry[i].name), "0:MIBIB", 8) == 0) ||
        (strncmp((const char *)(&parti1->part_entry[i].name), "mibib", 6) == 0))
    {
      for (j=0; j<parti2->numparts; j++)
      {
        if ((strncmp((const char *)(&parti2->part_entry[j].name), "0:MIBIB", 8) == 0) ||
            (strncmp((const char *)(&parti2->part_entry[j].name), "mibib", 6) == 0))
        {
          if ((parti1->part_entry[i].offset != parti2->part_entry[j].offset) ||
              (parti1->part_entry[i].length != parti2->part_entry[j].length))
        
          {
            return FALSE;
          }
        }
      }
    }  
    if ((strncmp((const char *)(&parti1->part_entry[i].name), "0:EFS2", 7) == 0) ||
        (strncmp((const char *)(&parti1->part_entry[i].name), "efs2", 5) == 0))
    {
      for (j=0; j<parti2->numparts; j++)
      {
        if ((strncmp((const char *)(&parti2->part_entry[j].name), "0:EFS2", 7) == 0) ||
            (strncmp((const char *)(&parti2->part_entry[j].name), "efs2", 5) == 0))
        {
          if ((parti1->part_entry[i].offset != parti2->part_entry[j].offset) ||
              (parti1->part_entry[i].length != parti2->part_entry[j].length))
        
          {
            return FALSE;
          }
        }
      }
    }
  }
  return TRUE;
}

/* 
 * This fundtion ensure the partition ordering is preserved.  This can be done by 
 * comparing partition names between new partition table and existing partition table
 * and ignoring the “scrub” partition name
 */
static int nand_parti_check_order(flash_partable_t parti1,
                            flash_partable_t parti2)
{
  uint16 i=0,j=0;

  if (parti1->numparts != parti2->numparts)
  {
    return FALSE;
  }

  /* We can use number of partitions from either, since they are equal */
  while(i<parti1->numparts && j<parti2->numparts)
  {
    FLASHLOG(4,("fptae:  Comparing %s to %s \n", (char *)
      &parti1->part_entry[i].name, (char *)&parti2->part_entry[j].name));

    if (!nand_parti_name_matches(&parti1->part_entry[i],
                                     (const unsigned char *)parti2->part_entry[j].name))
    {
      FLASHLOG(3,("fptae:  Names do not match\n"));
      return FALSE;
    }
    if ((parti1->part_entry[i].attrib1 != parti2->part_entry[j].attrib1) ||
        (parti1->part_entry[i].attrib2 != parti2->part_entry[j].attrib2) ||
        (parti1->part_entry[i].attrib3 != parti2->part_entry[j].attrib3))
    {
      return FALSE;
    }

    i++;
    j++;

    if ((strncmp((const char *)(&parti1->part_entry[i].name), "0:SCRUB", 8) == 0) ||
        (strncmp((const char *)(&parti1->part_entry[i].name), "scrub", 7) == 0))
    {
     i++;
    } 
    if ((strncmp((const char *)(&parti2->part_entry[j].name), "0:SCRUB", 8) == 0) ||
        (strncmp((const char *)(&parti2->part_entry[j].name), "scrub", 7) == 0))
    {
      j++;
    }
  }

  /* Every field matches exactly, indicate success. */
  return TRUE;
}


/*
 * This function checks flash open handles to make sure no partitions
 * are accessed while partition table update in progress
 */
#if 0 
static int nand_parti_check_client_handles(flash_client_ctxt *client_ctxt)
{
  uint32 num_open_handles = client_ctxt->flash_dev_ctxt->refs;

  if(num_open_handles > 2)
  {
    return FALSE;
  }
  
  if ((strncmp(client_ctxt->client_data.partition_name, (const char *)MIBIB_PARTI_NAME, 
    FLASH_PART_NAME_LENGTH) == 0) ||
    (strncmp(client_ctxt->client_data.partition_name, (const char *)"mibib", 6) == 0))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
#endif

/* 
 * This function validates newly updated MIBIB partition table
 */
static int nand_parti_validate(flash_client_ctxt *client_ctxt, flash_partable_t usr_parti_ptr)
{
  flash_partable_t nand_parti_ptr =
    ( flash_partable_t )client_ctxt->flash_dev_ctxt->parti_ptr;

  if ( (usr_parti_ptr->magic1 != FLASH_PART_MAGIC1) ||
    (usr_parti_ptr->magic2 != FLASH_PART_MAGIC2))
  {
    FLASHLOG (4,("fnibm: Usr parti magic mismatch.\n"));
    return FALSE;
  }

#if 0  
  if(!nand_parti_check_client_handles(client_ctxt))
  {
    FLASHLOG(4,("Other partitions are opened\n"));
    return FALSE;
  }
#endif

  if(!nand_parti_check_order (nand_parti_ptr, usr_parti_ptr))
  {
    FLASHLOG(4,("Order of user partition tbl doesn't match with one in flash\n"));
    return FALSE;
  }

  if(!nand_parti_check_boundaries (nand_parti_ptr, usr_parti_ptr))
  {
    FLASHLOG(4,("Boundaries of MIBIB&EFS2 don't match with MIBIB&EFS2 in flash\n"));
    return FALSE;
  }
  return TRUE;
}

static flash_partentry_t nand_parti_find_entry(flash_partable_t parti_ptr,
  const unsigned char *parti_name)
{
  int entries = parti_ptr->numparts;
  flash_partentry_t entry;
  int i;

  for (i=0; i<entries; i++)
  {
    entry = &parti_ptr->part_entry[i];
    FLASHLOG(5,("\nPartition 0x%x ::  \n", i));
    FLASHLOG(5,("\tName is %s\n", entry->name));
    FLASHLOG(5,("\tStart is 0x%x\n", entry->offset));
    FLASHLOG(5,("\tLength is 0x%x\n", entry->length));
    FLASHLOG(5,("\tFlash is 0x%x\n", entry->which_flash));

    if (nand_parti_name_matches(entry, parti_name))
    {
      return entry;
    }
  }

  return(flash_partentry_t)0;
}

/**
* Update the MIBIB partition table in the device and driver
*
* @param handle [IN]
*   DAL interface handle
*
* @param partition_table [IN]
*   Pointer to newly updated MIBIB partition table.
*   MIBIB partition table entries are required to be ordered.
*
* @return int [OUT]
*   Result of the operation.
*
*/
int flash_partition_table_update (flash_handle_t handle, flash_partable_t parti_table)
{
  int latest_mibib_block = -1, new_mibib_block = -1;
  int mibib_parti_search_min = -1, mibib_parti_search_len = -1;
  int cur_block = 0;
  uint32 page, page_size_in_bits = 0;
  uint32 crc32 = 0x0UL;
  mi_boot_info_t mibib_magic;
  flash_micrc_data_t mibib_crc;
  struct flash_block_vector block_vector;
  int erase_result_data;
  int result = FLASH_DEVICE_DONE;
  enum flash_block_state blk_state;
  unsigned char *page_buf  = NULL;
  flash_partentry_t entry_ptr = NULL;
  flash_client_ctxt *client_ctxt;
  flash_partable_t smem_aarm_partition_table;
  
  if((handle == NULL) || (parti_table == NULL))
  {
    return FLASH_DEVICE_FAIL;
  }

  client_ctxt = ((flash_handle *)handle)->flash_client_ctxt;
  
  page_buf = (unsigned char *) (GET_RAW_PAGEBUF(client_ctxt));

  latest_mibib_block = client_ctxt->flash_dev_ctxt->mibib_info.new_mibib_block;
  
  if(!nand_parti_validate( client_ctxt, parti_table ))
  {
    FLASHLOG(4,("validating new partition table is failed\n"));
    return FLASH_DEVICE_FAIL;
  }
  
  /* At this point, flash driver should have read and validated the MIBIB partition table as part of its
   * nand initialization.  As such, MIBIB partition offset and size can be extracted and used to limit 
   * MIBIB block search within the MIBIB partition region only. 
   */
  if (client_ctxt->flash_dev_ctxt->parti_ptr != NULL)
  {
    entry_ptr = nand_parti_find_entry(client_ctxt->flash_dev_ctxt->parti_ptr, 
                                      (unsigned const char *)MIBIB_PARTI_NAME);  
    if (entry_ptr != (flash_partentry_t)0)
    {
      /* Found the MIBIB partition. Init MIBIB block search parameters. */
      mibib_parti_search_min = entry_ptr->offset;
      mibib_parti_search_len = entry_ptr->length;
    }
    else
    {
      FLASHLOG (4, ("Error: MIBIB paritition not found. \n"));
      return FLASH_DEVICE_FAIL;
    }
  }
  else
  {
    FLASHLOG (4, ("Error: MIBIB paritition table unknown in driver. \n"));
    return FLASH_DEVICE_FAIL;
  }
  
  /* Start looking from first MIBIB block */
  new_mibib_block = mibib_parti_search_min;
 
  while (new_mibib_block <= (mibib_parti_search_min + mibib_parti_search_len))
  {
    cur_block = (new_mibib_block - mibib_parti_search_min);

    if (FLASH_DEVICE_DONE != nand_entry_block_get_state(client_ctxt,
                                 cur_block, &blk_state ))
    {
      FLASHLOG (4, ("Get Block Bad Failure.\n"));
      return FLASH_DEVICE_FAIL;
    }

    if ((blk_state == FLASH_BLOCK_BAD)||(new_mibib_block == latest_mibib_block))
    {
      new_mibib_block++;
    }
    else
    {
      /* erase the block */
      block_vector.start_block = cur_block;
      block_vector.block_count = 1;
      block_vector.result_vector = &erase_result_data;
      result = nand_entry_erase_blocks(client_ctxt, &block_vector, 1);
      if(result != FLASH_DEVICE_DONE)
      {
        if (result != FLASH_DEVICE_INVALID_PARAMETER)
        {
          if (FLASH_DEVICE_DONE != nand_entry_block_set_state(client_ctxt,
                                       cur_block, FLASH_BLOCK_BAD ))
          {
            FLASHLOG (4, ("Error: Set Block Bad Failure.\n"));
            return FLASH_DEVICE_FAIL;
          }
          new_mibib_block++;
          continue;
        }
        else
        {
          FLASHLOG (4, ("Error: device failed during erase\n"));
          return FLASH_DEVICE_FAIL;
        }
      }

      page_size_in_bits =
        client_ctxt->flash_dev_ctxt->dev_info.u.nand.page_size_bytes * 8;

      /* Synchronize with other clients */
      DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);

      DALSYS_memset((void *)page_buf, 0xFF,
          client_ctxt->flash_dev_ctxt->dev_info.u.nand.page_size_bytes);

      mibib_magic = (mi_boot_info_t)page_buf;
      mibib_magic->magic1 = MIBIB_MAGIC1;
      mibib_magic->magic2 = MIBIB_MAGIC2;
      mibib_magic->version = MIBIB_VERSION;
      mibib_magic->age = (client_ctxt->flash_dev_ctxt->mibib_age) + 1;

      page = (cur_block * 
        client_ctxt->flash_dev_ctxt->dev_info.u.nand.pages_per_block);

      result = nand_parti_write_pages(client_ctxt, 
                   page, FLASH_WRITE_MAIN, page_buf);
      if(result != FLASH_DEVICE_DONE)
      {
        if ((result != FLASH_DEVICE_INVALID_PARAMETER) &&
            (result != FLASH_DEVICE_NO_MEM))
        {
          if (FLASH_DEVICE_DONE != nand_entry_block_set_state(client_ctxt,
                                       cur_block, FLASH_BLOCK_BAD ))
          {
            FLASHLOG (4, ("Error: Set Block Bad Failure.\n"));
            return FLASH_DEVICE_FAIL;
          }
          new_mibib_block++;
          continue;
        }
        else
        {
          FLASHLOG (4, ("Error: device failed during write\n"));
          return FLASH_DEVICE_FAIL;
        }
      }
      crc32 = crc_32_calc((uint8 *)page_buf, (uint16)page_size_in_bits, crc32);

      page++;

      DALSYS_memset((void *)page_buf, 0xFF,
          client_ctxt->flash_dev_ctxt->dev_info.u.nand.page_size_bytes);
      DALSYS_memcpy((void *)page_buf, (void *)parti_table,
          sizeof(struct flash_partition_table));

      result = nand_parti_write_pages(client_ctxt, 
                   page, FLASH_WRITE_MAIN, page_buf);
      if(result != FLASH_DEVICE_DONE)
      {
        if ((result != FLASH_DEVICE_INVALID_PARAMETER) &&
            (result != FLASH_DEVICE_NO_MEM))
        {
          if (FLASH_DEVICE_DONE != nand_entry_block_set_state(client_ctxt,
                                       cur_block, FLASH_BLOCK_BAD ))
          {
            FLASHLOG (4, ("Error: Set Block Bad Failure.\n"));
            return FLASH_DEVICE_FAIL;
          }
          new_mibib_block++;
          continue;
        }
        else
        {
          FLASHLOG (4, ("Error: device failed during write\n"));
          return FLASH_DEVICE_FAIL;
        }
      }
      crc32 = crc_32_calc((uint8 *)page_buf, (uint16)page_size_in_bits, crc32);

      DALSYS_memset((void *)page_buf, 0xFF,
          client_ctxt->flash_dev_ctxt->dev_info.u.nand.page_size_bytes);

      mibib_crc = (flash_micrc_data_t)page_buf;
      mibib_crc->magic1 = FLASH_MIBIB_CRC_MAGIC1;
      mibib_crc->magic2 = FLASH_MIBIB_CRC_MAGIC2;
      mibib_crc->version = FLASH_MIBIB_CRC_VERSION;
      mibib_crc->crc = crc32;
   
      page = page + 2;

      result = nand_parti_write_pages(client_ctxt, 
                    page, FLASH_WRITE_MAIN, page_buf);
      if(result != FLASH_DEVICE_DONE)
      {
        if ((result != FLASH_DEVICE_INVALID_PARAMETER) &&
            (result != FLASH_DEVICE_NO_MEM))
        {
          if (FLASH_DEVICE_DONE != nand_entry_block_set_state(client_ctxt,
                                       cur_block, FLASH_BLOCK_BAD ))
          {
            FLASHLOG (4, ("Error: Set Block Bad Failure.\n"));
            return FLASH_DEVICE_FAIL;
          }
          new_mibib_block++;
          continue;
        }
        else
        {
          FLASHLOG (4, ("Error: device failed during write\n"));
          return FLASH_DEVICE_FAIL;
        }
      } 
      DALSYS_memcpy(client_ctxt->flash_dev_ctxt->parti_ptr,
      (const void *)parti_table, sizeof(struct flash_partition_table));
      
      smem_aarm_partition_table = smem_alloc(SMEM_AARM_PARTITION_TABLE,  sizeof(struct flash_partition_table));
	  if (smem_aarm_partition_table == NULL)
      {
        FLASHLOG (4, ("Error: Failed to initialise shared memory pointer\n"));
      }
      else
      {
        DALSYS_memcpy((void *)smem_aarm_partition_table,
        (const void *)parti_table, sizeof(struct flash_partition_table));
      }

      client_ctxt->flash_dev_ctxt->mibib_age++;

      client_ctxt->flash_dev_ctxt->mibib_info.new_mibib_block
        = new_mibib_block;

      DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

      return FLASH_DEVICE_DONE;
    }  
  }
  /* Error fatal if good blocks are not found in MIBIB partition */
  DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
        "Flash: MIBIB blocks are bad.!");
  return FLASH_DEVICE_FAIL;
}

/**
* Get the MIBIB partition table
*
* @param handle [IN]
*   DAL interface handle
*
* @param partition_table [OUT]
*   Pointer to buffer to read in MIBIB partition table.
*
* @return int [OUT]
*   Result of the operation.
*
*/
int flash_partition_table_get (flash_handle_t handle, flash_partable_t parti_table)
{
  flash_client_ctxt *client_ctxt;

  if((handle == NULL) || (parti_table == NULL))
  {
    return FLASH_DEVICE_FAIL;
  }
  
  client_ctxt = ((flash_handle *)handle)->flash_client_ctxt;

  DALSYS_memcpy((void *)parti_table, (void *)client_ctxt->flash_dev_ctxt->parti_ptr, 
        sizeof(struct flash_partition_table));

  return FLASH_DEVICE_DONE;
}

