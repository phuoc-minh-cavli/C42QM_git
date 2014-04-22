/***********************************************************************
 * qapi_ftl.c
 *
 * This layer is a wrapper on top of FTL core APIs
 *
 *
 * Copyright (c) 2017, 2019 QUALCOMM Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *
 ***********************************************************************/

/*=============================================================================
 * 
 *                         EDIT HISTORY FOR MODULE
 * 
 *   This section contains comments describing changes made to the module.
 *   Notice that changes are listed in reverse chronological order.
 * 
 * when         who   what, where, why
 * ----------   ---   ---------------------------------------------------------
 * 2019-07-24   sl     Restrict customers to remove/Modify QAPI.
 * 2019-04-10   sl     Deprecate user space QAPI_FTL support.
 * 2017-06-11   svl    Handle MMU enablement.
 * 2017-03-10   svl    Create.
 * 
 * ==========================================================================*/

#include "qurt_txm_qapi_fwk.h"
#include "qurt_memory.h"
#include "qapi_ftl.h"
#include "flash_ftl.h"

/*===========================================================================
                                   MACROS
  ===========================================================================*/



/*===========================================================================
                          LOCAL FUNCTIONS
  ===========================================================================*/

static qapi_Status_t
  qapi_error_for_ftl_error(FLASH_FTL_STATUS FTL_Status)
{
  qapi_Status_t qapi_status = QAPI_ERROR;

  switch(FTL_Status)
  {
    case FLASH_FTL_OK:
    {
      qapi_status = QAPI_OK;
    }
    break;
    case FLASH_FTL_FAIL:
    {
      qapi_status = QAPI_ERROR;
    }
    break;
    case FLASH_FTL_NOT_INIT:
    {
      qapi_status = QAPI_FTL_NOT_INIT;
    }
    break;
    case FLASH_FTL_INVALID_PARAM:
    {
      qapi_status = QAPI_ERR_INVALID_PARAM;
    }
    break;
    case FLASH_FTL_OUT_OF_MEM:
    {
      qapi_status = QAPI_ERR_NO_MEMORY;
    }
    break;
    case FLASH_FTL_OUT_OF_GOOD_BLOCKS:
    {
      qapi_status = QAPI_FTL_OUT_OF_GOOD_BLOCKS;
    }
    break;
    default:
    {
      qapi_status = QAPI_FTL_ERR_UNKNOWN;
    }
  }
  return qapi_status;
}

UINT
  qapi_FTL_handler(UINT id, UINT a, UINT b, UINT c, UINT d)
{
  return (UINT)QAPI_ERR_NOT_SUPPORTED;
}

/*===========================================================================
                          FUNCTIONS
  ===========================================================================*/

qapi_Status_t
  qapi_FTL_Open( qapi_FTL_client_t *handle, const uint8_t *part_name )
{
  FLASH_FTL_STATUS FTL_Status = FLASH_FTL_FAIL;
  FTL_Status = flash_ftl_open(handle, part_name);
  return qapi_error_for_ftl_error(FTL_Status);
}

qapi_Status_t
  qapi_FTL_Close(qapi_FTL_client_t *handle)
{
  FLASH_FTL_STATUS FTL_Status = FLASH_FTL_FAIL;
  FTL_Status = flash_ftl_close(handle);
  return qapi_error_for_ftl_error(FTL_Status);
}

qapi_Status_t
  qapi_FTL_Get_info (
                      qapi_FTL_client_t handle,
                      qapi_FTL_info_t *info
                    )
{
  FLASH_FTL_STATUS FTL_Status = FLASH_FTL_FAIL;
  FTL_Status = flash_ftl_get_info(handle, (flash_ftl_info_t *)info);
  return qapi_error_for_ftl_error(FTL_Status);
}

qapi_Status_t
  qapi_FTL_Read_lpa (
                      qapi_FTL_client_t handle,
                      uint32_t lpa, uint32_t lpa_count, uint8_t *buffer
                    )
{
  FLASH_FTL_STATUS FTL_Status = FLASH_FTL_FAIL;
  FTL_Status = flash_ftl_read_lpa(handle, lpa, lpa_count, buffer);
  return qapi_error_for_ftl_error(FTL_Status);
}

qapi_Status_t
  qapi_FTL_Write_lpa(
                      qapi_FTL_client_t handle,
                      uint32_t lpa, uint32_t lpa_count, uint8_t *buffer
                    )
{
  FLASH_FTL_STATUS FTL_Status = FLASH_FTL_FAIL;
  FTL_Status = flash_ftl_write_lpa(handle, lpa, lpa_count, buffer);
  return qapi_error_for_ftl_error(FTL_Status);
}

qapi_Status_t
  qapi_FTL_Erase_block(
                        qapi_FTL_client_t handle,
                        uint32_t erase_block, uint32_t erase_block_count
                      )
{
  FLASH_FTL_STATUS FTL_Status = FLASH_FTL_FAIL;
  FTL_Status = flash_ftl_erase_block(handle, erase_block, erase_block_count);
  return qapi_error_for_ftl_error(FTL_Status);
}
