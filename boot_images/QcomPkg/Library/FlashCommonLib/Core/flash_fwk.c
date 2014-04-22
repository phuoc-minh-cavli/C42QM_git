/*=============================================================================
 *
 * FILE:      flash_dal_fwk.c
 *
 * DESCRIPTION: Function definitions specific to Flash DAL framework
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2008-2013,2016 QUALCOMM Technologies Incorporated.
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
 *  $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/FlashCommonLib/Core/flash_fwk.c#1 $ $DateTime: 2018/08/01 23:51:05 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 07/11/16     eo      Update custom EEPROM support
 * 01/29/13     sv      Fix KW error - Memory leak issue
 * 09/05/12     bb/sv   Call entry layer APIs directly
 * 03/19/12     sv      Add SPI NOR power management support
 * 03/19/12     eo      Support for erase blocks by size
 * 09/15/10     bb      Added support for multiple device IDs
 * 05/11/10     eo      Added NOR API support
 * 12/28/09     sc      Added 2x support in DAL
 * 02/23/09     bb      Read/Write API changes with iovec design changes
 * 11/24/08     mm      Support for UUID
 * 08/28/08     dp      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_fwk.h"
#include "flash_osal.h"
#include "flash_nand_entry.h"

/***********************************************************************
 *   Data structures
 ***********************************************************************/	
static flash_driver_ctxt_t flash_internal_clients;

/***********************************************************************
 *    Functions
 ***********************************************************************/
static void flash_init_interface(flash_client_ctxt* client_ctxt, uint8 client_idx);

/*------------------------------------------------------------------------------
 *   Following functions are defined in base IQI CS Interface.
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Following functions are extended in flash_handle Interface available to flash
  clients

  open_partition      =  flash_fwk_open_partition,
  get_info            =  flash_fwk_get_info,
  get_status          =  flash_fwk_get_status,
  erase_blocks        =  flash_fwk_erase_blocks,
  erase_suspend       =  flash_fwk_erase_suspend,
  erase_resume        =  flash_fwk_erase_resume,
  block_set_state     =  flash_fwk_block_set_state,
  block_get_state     =  flash_fwk_block_get_state,
  write_pages         =  flash_fwk_write_pages,
  write_partial_page  =  flash_fwk_write_partial_page,
  read_pages          =  flash_fwk_read_pages,
  read_otp_page       =  flash_fwk_read_otp_page,
  copy_pages          =  flash_fwk_copy_pages
  get_uuid            = flash_fwk_get_uuid,
  erase_blocks_by_size = flash_fwk_erase_blocks_by_size,
  set_power_mode      =  flash_fwk_set_power_mode
------------------------------------------------------------------------------*/
/* This function intializes the flash handle function table vtbl.
 * It fills in teh API for the DALDevice function table used by
 * the DAL framework as a part of attach, detach, open, close,
 * init, deint.  It also intializes the APIs for the client
 * interface
 */
static void flash_init_interface(flash_client_ctxt* client_ctxt, uint8 client_idx)
{
  /*-----------------------------------------------------------------------
     Depending upon client type setup the vtables (entry points)
   -------------------------------------------------------------------------*/
  /* Initialize to device ctxt from probe */
  flash_internal_clients.flash_handle[client_idx].flash_dev_ctxt = 
    flash_internal_clients.flash_handle[0].flash_dev_ctxt;
}
	
/* This function invokes the flash_dal_get_info which in turn calls
 * the get_info API of the flash driver
 */
NAND_STATUS nand_get_info(nand_handle_t handle, struct nand_info *nand_info)
{
  int status = NAND_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_client_ctxt *)handle);

  status = nand_entry_get_info(client_ctxt, nand_info);

  return status;
}

/* This function invokes the flash_dal_erase_blocks which in turn
 * invokes the client context erase block API.
 */
NAND_STATUS nand_erase(nand_handle_t handle, uint32 start_block, uint32 block_count)
{
  int status = NAND_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_client_ctxt *)handle);

  status = nand_entry_erase_blocks(client_ctxt, start_block, block_count);

  return status;
}

/* This function invokes the flash_dal_flash_dal_block_set_state which
 *  inturn invokes the client context flash_dal_block_set_state API.
 */
NAND_STATUS nand_block_set_state(nand_handle_t handle, uint32 block,
  enum nand_block_state block_state)
{
  int status = NAND_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_client_ctxt *)handle);

  status = nand_entry_block_set_state(client_ctxt, block, block_state);

  return status;
}

/* This function invokes the flash_dal_flash_dal_block_get_state which
 * inturn invokes the client context flash_dal_block_get_state API.
 */
NAND_STATUS nand_block_get_state(nand_handle_t handle, uint32 block, 
  enum nand_block_state *block_state)
{
  int status = NAND_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_client_ctxt *)handle);

  status = nand_entry_block_get_state(client_ctxt, block, block_state);

  return status;
}

/* This function invokes the flash_dal_write_pages which in
 * turn invokes the client context write_pages API.
 */
NAND_STATUS nand_write(nand_handle_t handle,
  enum page_access_opcode opcode, uint32 start_page, uint32 page_count, void *buffer, uint32 buffer_size)
{
  int status = NAND_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_client_ctxt *)handle);

  status = nand_entry_write_pages(client_ctxt, opcode, start_page, page_count, buffer, buffer_size);

  return status;
}

/* This function invokes the flash_dal_read_pages which in turn invokes
 * the client context read_pages API.
 */
NAND_STATUS nand_read(nand_handle_t handle,
  enum page_access_opcode opcode, uint32 start_page, uint32 page_count, void *buffer, uint32 buffer_size)
{
  int status = NAND_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_client_ctxt *)handle);

  status = nand_entry_read_pages(client_ctxt, opcode, start_page, page_count, buffer, buffer_size);

  return status;
}

NAND_STATUS nand_open(uint32 dev_id, const uint8 *partition_name, 
  nand_handle_t *handle)
{
  int idx, err;
  int status = NAND_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt =	NULL;

  if (NULL == handle)
  {
    return status;
  }

  if (FALSE == flash_internal_clients.nand_initialized)
  {	
    flash_dev_ctxt *dev_ctxt = NULL; 

    client_ctxt = (flash_client_ctxt *)&flash_internal_clients.flash_handle[0];
    flash_memset(client_ctxt, 0x0, (sizeof(flash_handle) * 32));	

    /* Alocates memory for the flash client context */
    err = flash_malloc(sizeof(flash_dev_ctxt), (void **)&flash_internal_clients.flash_handle[0].flash_dev_ctxt);
    if (NAND_DEVICE_DONE != err)
    {
      return err;
    }	
    dev_ctxt = client_ctxt->flash_dev_ctxt;
    flash_memset(dev_ctxt, 0x0, sizeof(flash_dev_ctxt));

    /*-------------------------------------------------------------------------
      Initialize Synchronization Object for this device.
      This is used to synchronize between client calls and interrupt context
      on our variables and HAL.
      --------------------------------------------------------------------------*/
    if(NAND_DEVICE_DONE != flash_sync_create((void *)dev_ctxt))
    {
      return status;
    }
	
    status = nand_entry_device_probe(client_ctxt);

    if (NAND_DEVICE_DONE == status)
    {
      status = nand_entry_open_partition(client_ctxt,
                 (const unsigned char *)partition_name);
    }

    if (NAND_DEVICE_DONE == status)
    {
      /* Increment client references */
      client_ctxt->refs += 1;
      flash_internal_clients.clnt_cnt += 1;
      flash_internal_clients.nand_initialized = TRUE;
      *handle = (nand_handle_t *)client_ctxt;
    }
    else
		
    {
      handle = NULL;
    }
  }
  else
  {
    for(idx = 1; idx < FLASH_NUM_PART_ENTRIES; idx++)
    {
      const char *parti_name = &flash_internal_clients.flash_handle[idx].client_data.partition_name[0];

      client_ctxt = (flash_client_ctxt *)&flash_internal_clients.flash_handle[idx];
      if (strcmp(parti_name, (const char *)partition_name) == 0)
      {
        client_ctxt->refs += 1;
        flash_internal_clients.clnt_cnt += 1;
		
        /* Intialize the flash handle function table vtbl */
        flash_init_interface(client_ctxt, idx);

        status = NAND_DEVICE_DONE;
        break;
      }
      else if (client_ctxt->refs == 0)
      {
        client_ctxt->refs += 1;
        flash_internal_clients.clnt_cnt += 1;
        /* Intialize the flash handle function table vtbl */
        flash_init_interface(client_ctxt, idx);
		
        status = nand_entry_open_partition(client_ctxt,
                   (const unsigned char *)partition_name);
        break; 
      }
    }
    *handle = (NAND_DEVICE_DONE == status)?((nand_handle_t *)client_ctxt) : NULL;
  }
  
  return status;
}

NAND_STATUS nand_close(nand_handle_t handle)
{
  int status = NAND_DEVICE_DONE;
  flash_client_ctxt *client_ctxt = ((flash_client_ctxt *)handle);

  FLASHLOG(4, ("Flash close Called\n"));

  if (client_ctxt->refs > 0)
  {
    client_ctxt->refs -= 1;
    flash_internal_clients.clnt_cnt -= 1;
    if (!client_ctxt->refs)
    {
      status = nand_entry_close(client_ctxt);
    }
  }
  
  if (flash_internal_clients.clnt_cnt == 0)
  {
    status = nand_entry_deinit (client_ctxt);
  }
  
  return status;
}

int flash_fwk_attach(const char *pszArg, DALDEVICEID dev_id, 
  DalDeviceHandle **handle_dal_device)
{
  int status = NAND_DEVICE_DONE;
	
  FLASHLOG(4, ("Flash fwk attach\n"));
  return status;
}

