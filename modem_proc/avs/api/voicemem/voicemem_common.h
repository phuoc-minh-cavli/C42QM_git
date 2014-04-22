#ifndef __VOICEMEM_COMMON_H__
#define __VOICEMEM_COMMON_H__

/**
  @file voicemem_common.h
  @brief This file contains the VOICEMEM common APIs.
*/

/*===========================================================================
Copyright (C) 2013, 2018 Qualcomm Technologies, Inc.
Confidential and Proprietary - Qualcomm Technologies, Inc. 
All Rights Reserved.
===========================================================================*/

/*===========================================================================

  $Header: //components/rel/avs.mpss/8.0.c12/api/voicemem/voicemem_common.h#1 $
  $Author: pwbldsvc $

===========================================================================*/


/****************************************************************************
 * INITIALIZATION/DEINITIALIZATION APIS                                     *
 ****************************************************************************/

/**
  Initializes VOICEMEM.
 
  @par Payload
  None.

  @return
  #VSD_EOK in case of success.

  @dependencies
  None.

  @comments
  None.
*/
#define VOICEMEM_CMD_INIT ( 0x00012A6F )


/**
  Deinitializes VOICEMEM.
 
  @par Payload
  None.

  @return
  #VSD_EOK in case of success.

  @dependencies
  None.

  @comments
  None.
*/
#define VOICEMEM_CMD_DEINIT ( 0x00012A72 )


/****************************************************************************
 * COMMON APIS                                                              *
 ****************************************************************************/

/**
  Allocates a memory region of default type.
 
  @par Payload
  #voicemem_cmd_alloc_default_t

  @return
  #VSD_EOK in case of success.

  @dependencies
  None.

  @comments
  The allocated memory region must be treated by the client as cached.
*/
#define VOICEMEM_CMD_ALLOC_DEFAULT ( 0x00012EE7 )

/** @brief Type definition for #voicemem_cmd_alloc_default_t.
*/
typedef struct voicemem_cmd_alloc_default_t voicemem_cmd_alloc_default_t;

/** @brief Payload structure for the #VOICEMEM_CMD_ALLOC_DEFAULT command.
*/
struct voicemem_cmd_alloc_default_t
{
  uint32_t size;
    /**< Size of the memory to be allocated in bytes. */
  uint32_t* ret_voicemem_handle;
    /**< Returned VOICEMEM handle for the allocated memory region. */
  void** ret_virt_addr;
    /**< Returned virtual start address of the allocated memory region. */
  uint8_t client_name[31];
    /**< Null-terminated client name string. The client must provide 
         a name when allocating memory. This string cannot be zero-length. */
};


/**
  Invalidates cache in an allocted memory region at the specified offset and
  size.
 
  @par Payload
  #voicemem_cmd_cache_invalidate_t.

  @return
  #VSD_EOK in case of success.

  @dependencies
  Memory region must first be allocated via the #VOICEMEM_CMD_ALLOC_DEFAULT
  command.

  @comments
  None.
*/
#define VOICEMEM_CMD_CACHE_INVALIDATE ( 0x00012EE8 )

/** @brief Type definition for #voicemem_cmd_cache_invalidate_t.
*/
typedef struct voicemem_cmd_cache_invalidate_t voicemem_cmd_cache_invalidate_t;

/** @brief Payload structure for the #VOICEMEM_CMD_CACHE_INVALIDATE command.
*/
struct voicemem_cmd_cache_invalidate_t
{
  uint32_t voicemem_handle;
    /**< VOICEMEM memory region handle. */
  void* virt_addr;
    /**< Address at which to invalidate. */
  uint32_t size;
    /**< Size to invalidate. */
};


/**
  Flushes cache in an allocted memory region at the specified offset and
  size.
 
  @par Payload
  #voicemem_cmd_cache_flush_t.

  @return
  #VSD_EOK in case of success.

  @dependencies
  Memory region must first be allocated via the #VOICEMEM_CMD_ALLOC_DEFAULT
  command.

  @comments
  None.
*/
#define VOICEMEM_CMD_CACHE_FLUSH ( 0x00012EE9 )

/** @brief Type definition for #voicemem_cmd_cache_flush_t.
*/
typedef struct voicemem_cmd_cache_flush_t voicemem_cmd_cache_flush_t;

/** @brief Payload structure for the #VOICEMEM_CMD_CACHE_FLUSH command.
*/
struct voicemem_cmd_cache_flush_t
{
  uint32_t voicemem_handle;
    /**< VOICEMEM memory region handle. */
  void* virt_addr;
    /**< Address at which to flush. */
  uint32_t size;
    /**< Size to flush. */
};


/**
  Frees an allocated memory region.
 
  @par Payload
  #voicemem_cmd_free_t.

  @return
  #VSD_EOK in case of success.

  @dependencies
  None.

  @comments
  None.
*/
#define VOICEMEM_CMD_FREE ( 0x00012EEA )

/** @brief Type definition for #voicemem_cmd_free_t.
*/
typedef struct voicemem_cmd_free_t voicemem_cmd_free_t;

/** @brief Payload structure for the #VOICEMEM_CMD_FREE command.
*/
struct voicemem_cmd_free_t
{
  uint32_t voicemem_handle;
    /**< VOICEMEM memory region handle. */
};

/**
  Gets the ADSP IOVA and PCIe IOVA corresponding to a given SMEM addr.

  @param[in] memory handle.
  @param[in] SMEM addr.
  @param[out] ADSP IOVA
  @param[out] PCIe IOVA
  
  @return
  VSD_EOK when successful.
 
  @dependencies
  None.

  @comments
  None.
*/
#define VOICEMEM_CMD_GET_IOVA ( 0x00013342 )
typedef struct voicemem_cmd_get_iova_t voicemem_cmd_get_iova_t;

struct voicemem_cmd_get_iova_t
{
  uint32_t mem_handle;
  /**< VOICEMEM handle for the allocated memory region v*/
  
  uint64_t smem_addr;
  /**< SMEM addr from the base. */

  uint64_t* adsp_iova;
  /**< ADSP IOVA. */

  uint64_t* pcie_iova;
  /**< PCIE IOVA. */
};

/**
  Sets the address amd size information for ADSP mailbox memory.
  This is applicable only incase of fusion platform where Modem Q6 sits 
  on MDM and ADSP is part of APQ.

  @param[in] Adsp IOVA.
  @param[in] PCIe IOVA.
  @param[in] Mem Size.

  @return
  VSD_EOK when successful.
 
  @dependencies
  None.

  @comments
  None.
*/
#define VOICEMEM_CMD_SET_MAILBOX_MEMORY_CONFIG ( 0x00013341 )

/** @brief Type definition for
 *         #voicemem_cmd_set_mailbox_memory_config_t.
*/
typedef struct voicemem_cmd_set_mailbox_memory_config_t voicemem_cmd_set_mailbox_memory_config_t;

/** @brief Payload structure for the #VOICEMEM_CMD_SET_MAILBOX_MEMORY_CONFIG command.
*/
struct voicemem_cmd_set_mailbox_memory_config_t
{
  uint64_t mailbox_mem_address_adsp;
  /**< IO virtual address(understandable to ADSP) of Mailbox Memory carved out from APQ DDR 
       specifically for mailbox packet exchange between ADSP and MDM. */

  uint64_t mailbox_mem_address_pcie;
  /**< IO virtual address(understandable to PCIe) of Mailbox Memory carved out from APQ DDR 
       specifically for mailbox packet exchange between ADSP and MDM. */

  uint32_t mem_size;
  /**< Size (in bytes) of the Mailbox Memory carved out from APQ DDR.

       @values : 81920 bytes
   */
};

/****************************************************************************
 * CALL API                                                                 *
 ****************************************************************************/

/**
  Issues a command to VOICEMEM.
 
  @param[in] cmd_id       Command ID.
  @param[in,out] params   Command payload.
  @param[in] size         Size of the command payload.
   
  @return
  VSD_EOK when successful.
 
  @dependencies
  None.

  @comments
  None.
*/
VSD_PUBLIC vsd_status_t voicemem_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
);


#endif /* __VOICEMEM_COMMON_H__ */
