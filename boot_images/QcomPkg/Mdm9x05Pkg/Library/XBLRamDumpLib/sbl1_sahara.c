/*=============================================================================

                        Secondary Boot Loader Sahara
                              Source File
GENERAL DESCRIPTION
  This file provides the implementation to enter the Sahara Protocol.

Copyright (c) 2011-2019 Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/14/19   aus     Transmit command packet in two parts 
04/18/19   c_vkuppa Fix for Ram dump PIPO test case failure
12/28/17   rohik   Added whitelist protection and removed clobber protection
01/12/17   sj      Added support for saving the logdump partition with PC dump
10/14/16   digant  reconfigure pshold to do warmreset before wdt reset
09/06/16   digant  roll-back to use secure wdog to retain Pimem contents
03/09/16   kpa     use pshold instead of secure wdog to reset since we are in EL1
08/06/15   as      Use shared functions
08/01/15   kpa     use dcache_flush_region instead of mmu_flush_cache
07/09/15   kpa     Update secure wdog reset logic for pimem dumps.
07/08/15   kpa     Support secure watchdog bite to preserve pimem dump
01/05/14   kpa     Remove unsupported api's for fetching info from mbn header.
11/20/14   ck      Refactored USB to use new agnostic USB extern interface
08/08/14   ck      Added 64 bit support
07/09/14   tj      Fixes for bounds check on hash buffer during compute hash offsets.
06/20/14   kedara  Support for llvm compilation.
05/07/14   kedara  Fix compiler warnings.
03/25/14   jz      Ported sbl_elf_optimized_segment_load stub for MSM's
03/18/14   ck      Replaced defunct boot_hw_warm_reset with call to boot_hw_reset
03/18/14   ck      Updated boot_hw_reset calls as they now take a reset type parameter
02/04/14   ck      Removed exit_ptr as it's not needed in Bear family.
11/12/13   kedara  Added sbl_sahara_skip_image_load as placeholder.
11/07/13   sve	   updated sbl_sahara_unrecoverable_error_handler to fix sbl1_error_handler
10/18/13   dhaval  zero initialize global buffers before starting elf download
10/15/13   kedara  Update dispatch table. Add NULL ptr for unused image load api's.
09/13/13   aus     Added changes to support SBL crash dumps in flashless boot
09/03/13   aus     Added support for clearing all downloaded data on auth failure
08/02/13   aus     Do not enumerate in mass storage mode if ram dump is not allowed by TZ
07/24/13   aus     Do a warm reset in unrecoverable error handler
06/24/13   aus     Skip loading EFS images if header is corrupted
06/18/13   sl      Added bulk mode type to boot_qhsusb_al_bulk_init()
06/11/13   kedara  Move image loading data structs to uncached ddr region.
06/05/13   aus     Clear the whitelist table of the image in case of authentication failures
05/31/13   kedara  Autheticate MBA as regular elf for non-MSA boot, 
                   remove sbl_sahara_mba_authenticate.
04/29/13   kedara  Update boot_auth_load_mba_header to check secure boot status
04/16/13   kedara  Added sbl_sahara_image_load_post_proc to support mba authentication
03/29/13   sy      Move serial_num to global uncache so usb can read the result via Sahara protocol
03/28/13   dhaval  Updates to support sha256 
02/19/13   kedara  Added sbl_sahara_get_elf_header, sbl_sahara_get_program_header
01/18/13   dh      Add sbl_sahara_unrecoverable_error_handler
11/08/12   kedara  Updates to enable hashing and authentication of elf's.
11/06/12   dh      Allow multiple calls to sbl_sahara_bulk_init
10/23/12   kpa     Move elf ZI init to boot_sahara_receive_image_elf.
10/03/12   kpa     Initialized cur_image in sbl_sahara_shared_data to fix mem
                   dumps over sahara for nand boot.
09/21/12   kpa     Added new function to obtain the image src field value 
                   from the image header.
                   Modified the auth function to return the status.
08/02/12   kpa     Enable Flashless boot for 9x25.
06/28/12   aus     Updated return value of boot_sahara_entry
05/17/12   dh      In sbl_sahara_bulk_init, only initialize HSUSB core
05/02/12   aus     Added support for Serial Number read in command mode
09/28/11   aus     Use whitelist clobber table protection for Sahara
09/29/11   dh      Ported to 8974, switch hsusb call to extern driver interface
09/15/11   aus     Removed warnings
08/29/11   aus     Flush the cache before jumping into sahara
08/24/11   aus     Added support for secure boot
07/13/11   aus     Added QHSUSB support for Sahara 
07/12/11   aus     Added support to decide between HSIC and HSUSB based on boot option
06/27/11   aus     Initial version to support Sahara

=============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_loader.h"
#include "boot_sahara.h"
#include "boot_util.h"
#include "boothw_target.h"
#include "boot_dload.h"
#include "boot_dload_debug.h"
#include "sbl1_hw.h"
#include BOOT_PBL_H
#include "boot_extern_hsuart_interface.h"
#include "boot_extern_qusb_interface.h"
#include "boot_cache_mmu.h"
#include "boot_hash_buffer.h"
#include "boot_shared_functions_consumer.h"
#include "boot_hash_buffer.h"
#include "boot_hash.h"
#include "boot_flash_dev_if.h"
#include "boot_transport.h"
#include "boot_authenticator.h"
#include "boot_cache.h"

#define MAX_IMG_BUFFERS 8

extern uint32 raw_partition_addr;
extern uint32 raw_partition_len ;

extern uint8 qsee_partition_id[];
extern boot_flash_trans_if_type * trans_if ;

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
static uint32 sbl_sahara_bulk_init(void);
static void sbl_sahara_bulk_poll(void);
static void sbl_sahara_bulk_shutdown(void);
static uint32 sbl_get_max_packet_size(void);
static uint32 sbl_get_max_raw_data_transfer_size(void);
static uint8* sbl_sahara_exec_cmd(uint32 client_command,
                                  uint32 *resp_length,
                                  uint32 *status);
static uint32 sbl_sahara_tx_bulk (uint8 *rx_buf, 
                                  uint32 len, 
                                  sahara_rx_cb_type rx_cb, 
                                  uint32 * err_code, 
                                  enum boot_sahara_packet_type type);
static uint32 sbl_sahara_rx_bulk (uint8 *rx_buf, 
                                  uint32 len, 
                                  sahara_rx_cb_type rx_cb, 
                                  uint32 * err_code, 
                                  enum boot_sahara_packet_type type);
static uint64 sbl_sahara_mem_debug_copy_restricted (const uint64 addr,
                                                    uint64 len);
static boolean sbl_sahara_is_auth_enabled(void);
static uint64 sbl_sahara_get_bin_header_size(void);
static uint64 sbl_sahara_get_bin_image_size(const uint8 *header);
static uint64 sbl_sahara_get_bin_code_size(const uint8 *header);
static uint32 sbl_sahara_get_bin_cert_chain_size(const uint8 *header);
static uint32 sbl_sahara_get_bin_signature_size(const uint8 *header);
static void sbl_sahara_unrecoverable_error_handler(void);
static enum boot_sahara_status sbl_sahara_image_load_post_proc(void);
static boolean sbl_sahara_skip_image_load(void);
static boolean sbl_elf_optimized_segment_load ( void );

/* Definitions for Sahara Authentication table */
static boolean sbl_sahara_init_hash(uint32 segment_index);
static boolean sbl_sahara_update_hash(const void *buf_ptr,
                                       uint32 buf_size);
static boolean sbl_sahara_finalize_hash(const void *buf_ptr,
                                        uint32 buf_size,
                                        uint32 segment_index);
static boolean sbl_sahara_authenticate(uint8 *header_ptr);
static boolean sbl_sahara_is_auth_enabled(void);
static void sbl_sahara_unrecoverable_error_handler(void);
static void sbl_sahara_reset(void);
static enum boot_sahara_status sbl_sahara_image_load_post_proc(void);
static boolean sbl_sahara_skip_image_load(void);
static boolean sbl_elf_optimized_segment_load ( void );
static enum boot_sahara_status sbl_sahara_preprocess_header ( 
                                      struct segment_record* sahara_elf_phdrs,
                                      uint32 numsegments,
                                      uint32 *segment_start_offset,
                                      uint32 *remaining_size);
static enum boot_sahara_status sbl_sahara_get_orig_hash_index(
                                      uint32 sorted_index,
                                      uint32 *original_index_ptr);

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

volatile boolean disable_optimized_load = FALSE;
static uint8 ALIGN(32) sbl_sahara_mem_debug_buffer[SAHARA_MAX_MEMORY_DATA_SIZE_IN_BYTES];
extern uint8 sahara_packet_buffer[SAHARA_MAX_PACKET_SIZE_IN_BYTES];
static sec_img_auth_whitelist_area_t sbl_white_list_area[MAX_IMG_BUFFERS];

/* Authentication flag from config table */
static boot_boolean auth;

/* current segment index for hashing operations */
static uint32 curr_segment_index = 0xFFFFFFFF;




/* Delclare a data respond buffer of 4 byte */
static uint8 sahara_cmd_exec_data_resp_buf[4] SECTION(".bss.BOOT_UNCACHED_DDR_ZI_ZONE"); 

/* ELF image info - filled in by sbl_sahara_entry */
static struct boot_sahara_elf_image_info sbl_sahara_elf_image_info SECTION(".bss.BOOT_UNCACHED_DDR_ZI_ZONE");

/* BIN image info - filled in by sbl_sahara_entry */
static struct boot_sahara_binary_image_info sbl_sahara_bin_image_info SECTION(".bss.BOOT_UNCACHED_DDR_ZI_ZONE");
  
/* Store the ELF program headers */
static struct progressive_boot_block elf_prog_headers SECTION(".bss.BOOT_UNCACHED_DDR_ZI_ZONE");
extern uint32 sbl_hash_len SECTION(".bss.BOOT_UNCACHED_DDR_ZI_ZONE");


/* Stores information on what image to load */  
static struct boot_sahara_shared_data sbl_sahara_shared_data = 
{
  0,
  SAHARA_IMAGE_UNKNOWN,
  &sbl_sahara_bin_image_info,
  FALSE,
  FALSE
};

/* SBL Sahara dispatch table */
static struct boot_sahara_dispatch_tbl sbl_sahara_dispatch_tbl = 
{
  SAHARA_INTERFACE_DISPATCH_VERSION,
  sbl_sahara_bulk_init,
  sbl_sahara_bulk_shutdown,
  sbl_sahara_bulk_poll,
  (sahara_rx_bulk_type) sbl_sahara_rx_bulk,
  (sahara_tx_bulk_type) sbl_sahara_tx_bulk,
  sbl_get_max_packet_size,
  sbl_sahara_reset,
  boot_whitelist_address_range_check,
  sbl_sahara_exec_cmd,
  sbl_get_max_raw_data_transfer_size,
  sbl_sahara_is_auth_enabled,
  qmemcpy,
  qmemset,
  sbl_sahara_get_bin_header_size,
  NULL,
  NULL,
  sbl_sahara_get_bin_image_size,
  sbl_sahara_get_bin_code_size,
  sbl_sahara_get_bin_cert_chain_size,
  sbl_sahara_get_bin_signature_size,
  NULL,
  sbl_sahara_unrecoverable_error_handler,
  sbl_sahara_image_load_post_proc,
  sbl_sahara_skip_image_load,  
  boot_get_hash_segment_buffer,
  sbl_elf_optimized_segment_load,
  NULL,
  NULL,
};

/* Authentication interface for Sahara */
static struct boot_sahara_auth_tbl sbl_sahara_auth_tbl =
{
  SAHARA_INTERFACE_AUTH_VERSION,
  sbl_sahara_init_hash,
  sbl_sahara_update_hash,
  sbl_sahara_finalize_hash,
  sbl_sahara_authenticate
};

/* SBL Sahara memory debug function table */
static struct boot_sahara_mem_debug_tbl sbl_sahara_mem_debug_tbl = 
{
  dload_mem_debug_supported,
  dload_mem_debug_init,
  dload_mem_debug_table_addr,
  dload_mem_debug_len,
  dload_mem_debug_verify_addr,
  dload_mem_debug_is_restricted_addr,
  sbl_sahara_mem_debug_copy_restricted
};

static struct boot_sahara_interface sbl_sahara_interface = 
{
  SAHARA_INTERFACE_VERSION,
  SAHARA_STATUS_SUCCESS,
  &sbl_sahara_shared_data,
  SAHARA_MODE_IMAGE_TX_PENDING,
  &sbl_sahara_dispatch_tbl,
  &sbl_sahara_auth_tbl,
  &sbl_sahara_mem_debug_tbl
};

#ifdef SAHARA_OVER_UART
static boot_packet_interface sbl_packet_interface = 
{
  PACKET_MINOR_ID,
  PACKET_MAJOR_ID,
  boot_hsuart_initialize,
  boot_hsuart_poll,
  boot_hsuart_read,
  boot_hsuart_write,
  boot_hsuart_get_max_packet_size,
  boot_hsuart_get_max_raw_data_transfer_size,
};
#endif

/* Pointers to hash table entries */
const uint8 *expected_hash_ptr[MI_PBT_MAX_SEGMENTS];

/*Program header table original segment index's */
uint32 original_segment_index[MI_PBT_MAX_SEGMENTS];

/* Security image type and shared data to be passed to the auth routine */
static uint32 image_sec_type;
static whitelst_tbl_entry_type *whitelist_tbl;

/*===========================================================================

                      PRIVATE FUNCTION DECLARATIONS

===========================================================================*/

/* ============================================================================
**  Function : sbl_sahara_clear_downloaded_data
** ============================================================================
*/
/*!
*   @brief
*   Clears ELF header, Program headers, sahara packet buffer, hash table buffer 
*   and memory region where data was written to.
*   
*   @param  None
*   
*   @par Dependencies
*   
*   @par Side Effects
*   All the downloaded data will be set to 0
*   
*   @retval None
*   
*   @sa None
*/
static void sbl_sahara_clear_downloaded_data( void )
{
  /* reset the flag tracking hash table authentication result for current elf */
  boot_set_hash_table_auth_result(FALSE);

  if ( sbl_sahara_elf_image_info.hash_table != NULL )
  {
    /* Clear hash table buffer */
    qmemset(sbl_sahara_elf_image_info.hash_table, 0x0, boot_get_hash_buffer_size());
  }
  
  if ( sbl_sahara_elf_image_info.prog_headers != NULL )
  {
    /* Clear program headers */
    qmemset(sbl_sahara_elf_image_info.prog_headers, 0x0, sizeof(struct progressive_boot_block));
  }
  
  if ( expected_hash_ptr != NULL )
  {
    /* Clear hash table buffer */
    qmemset(expected_hash_ptr, 0x0, sizeof(expected_hash_ptr));
  }
  
  /* Clear Elf header */
  qmemset(&(sbl_sahara_elf_image_info.elf_header), 0x0, sizeof(Elf32_Ehdr));

  /* Clear sahara packet buffer */
  qmemset(&sahara_packet_buffer, 0x0, sizeof(sahara_packet_buffer));

  /* reset segment_index */  
  curr_segment_index = 0xFFFFFFFF;
}

/* ============================================================================
**  Function : sbl_sahara_init_hash
** ============================================================================
*/
/*!
*   @brief
*   Initializes the context for computing the hash of a given set of data.
*   
*   @details
*   Initializes the context for computing the hash of a given set of data.   
*   
*   @param  None
*   
*   @par Dependencies
*   This must be called before calling sbl_sahara_update_hash or
*   sbl_sahara_finalize_hash.
*   
*   @par Side Effects
*   None
*   
*   @retval returns TRUE if no error occurred; FALSE otherwise
*   
*   @sa None
*/
static boolean sbl_sahara_init_hash( uint32 segment_index )
{
  boolean status = FALSE;
  /* reset segment_index */  
  curr_segment_index = segment_index;

  /* Initialize hash context */   
  if (BL_ERR_NONE != boot_parallel_hashInit((uint32)image_sec_type))
    sbl_sahara_clear_downloaded_data();
  else 
    status = TRUE;

  return status;
}

/* ============================================================================
**  Function : sbl_sahara_update_hash
** ============================================================================
*/
/*!
*   @brief
*   Computes the hash of the input buffer and updates the internal digest
*   buffer.
*   
*   @details
*   Computes the hash of the input buffer and updates the internal digest
*   buffer.
*   
*   @param *buf_ptr   -  [IN] Input buffer to hash
*   @param buf_size   -  [IN] Size of data to hash
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns TRUE if no error occurred; FALSE otherwise
*   
*   @sa None
*/
static boolean sbl_sahara_update_hash
(
  const void *buf_ptr,
  uint32 buf_size
)
{
  boolean status = FALSE;

  if (BL_ERR_NONE != boot_parallel_hashUpdate((uint32)image_sec_type, buf_ptr, buf_size, curr_segment_index))
    sbl_sahara_clear_downloaded_data();
  else
    status = TRUE;

  return status;
}

/* ============================================================================
**  Function : sbl_sahara_finalize_hash
** ============================================================================
*/
/*!
*   @brief
*   Finalizes the hash context and returns the resulting digest buffer.
*   
*   @details
*   Finalizes the hash context and returns the resulting digest buffer.
*   
*   @param *buf_ptr   -  [IN] Input buffer to hash
*   @param buf_size   -  [IN] Size of data to hash
*   @segment_index    -  [IN] segment_index being hashed
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns hash digest buffer
*   
*   @sa None
*/
static boolean sbl_sahara_finalize_hash
(
  const void *buf_ptr,
  uint32 buf_size,
  uint32 segment_index
)
{ 
  boolean status = FALSE;
  
  /* Passing NULL for buf_ptr and 0 as buf_size */
  if (BL_ERR_NONE != boot_parallel_hashFinal((uint32)image_sec_type, NULL, 
                                    0, segment_index))
  {
    sbl_sahara_clear_downloaded_data();
  }
  else
  {
    status = TRUE;
  }
  return status;
}

/* ============================================================================
**  Function : sbl_sahara_is_zeros
** ============================================================================
*/
/*!
*   @brief
*   Determines if all bytes in the input array of bytes are all zero or not.
*   
*   @details
*   This function parses each byte of data starting at the input address
*   location to see if the value stored at that location is 0. This
*   continues until all bytes are read (specified by size input).
*   
*   @param *addr   -  [IN] Address location to start reading from
*   @param size    -  [IN] Number of bytes to compare
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns TRUE if all bytes are zero; FALSE otherwise
*   
*   @sa None
*/
static boolean sbl_sahara_is_zeros( const void *addr, uint32 size )
{
  const uint8 *start_addr = (uint8*)addr;
  const uint8 *end_addr = start_addr + size;
  boolean retval = TRUE;

  /* Check if each byte is zero */
  do
  {
    if ( *start_addr != 0 )
    {
      retval = FALSE;
      break;
    }
    start_addr++;
  }while(start_addr < end_addr);

  return retval;
}

/* ============================================================================
**  Function : sbl_sahara_elf_compute_hash_offsets
** ============================================================================
*/
/*!
*   @brief
*   This computes the hash table entry offsets for each segment listed in the
*   program headers.
*    
*   @details
*   Based on whether a segment is PAGED or NON-PAGED, the offset for the
*   corresponding hash entry in the hash table will be computed. If the
*   segment is paged, then the next segment offset will be computed based
*   on the number of 4K pages in the paged segment. If the segment is not
*   paged, the next segment offset will be offset by the hash digest size.
*   The resulting offsets will be stored in a global array.
*   
*   @param *hash_tbl_ptr   -  [IN] Pointer to the hash table
*   @param num_segments    -  [IN] Number of program headers
*   @param *prog_hdr_ptr   -  [IN] Pointer to the program headers
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void sbl_sahara_elf_compute_hash_offsets
(
  const uint8 *hash_tbl_ptr,                /* Pointer to hash table */
  const uint32 num_segments,                /* Number of program headers */
  const struct segment_record *prog_hdr_ptr /* Pointer to program headers */
)
{
  /* Set segment record pointer initially equal to start of program headers */
  const struct segment_record *seg_ptr = prog_hdr_ptr;
  uint32 segment;               /*Index into program headers */
  uint32 hash_tbl_offset = 0;   /* Current offset into the hash table */
  uint32 hash_digest_size;      /* Size of hash entry in bytes */

  /* Now go through all the segments and compute the hash table offsets */
  BL_VERIFY( num_segments < MI_PBT_MAX_SEGMENTS,BL_ERR_INVALID_HASH_PTR|BL_ERROR_GROUP_BOOT );
  hash_digest_size = sbl_hash_len;

  /* Loop through all the ELF segments */
  for( segment = 0, hash_tbl_offset = 0; segment < num_segments; 
       ++segment, ++seg_ptr )
  {
  
	/* Validate that the hash ptr offsets do not overflow beyond the allocated buffer and that there is atleast one 
	   hash_digest_size of data left over in the buffer for the last hash ptr offset */
	BL_VERIFY(((((uintnt)hash_tbl_ptr + hash_tbl_offset) <= ((uintnt)hash_tbl_ptr + HASH_SEGMENT_SIZE - hash_digest_size)) && 
	           (((uintnt)hash_tbl_ptr + hash_tbl_offset) >= (uintnt)hash_tbl_ptr)), BL_ERR_INTEGER_OVERFLOW|BL_ERROR_GROUP_BOOT);
    /* Set the hash offset for the current segment */
    expected_hash_ptr[segment] = hash_tbl_ptr + hash_tbl_offset;

    /* Determine if segment is a paged or non-paged segment */
    if ( MI_PBT_PAGE_MODE_VALUE(seg_ptr->memory_attribute) == 
         MI_PBT_PAGED_SEGMENT )
    {
            /* Skip the offsets corresponding to paged segments */
      if ( seg_ptr->size != 0 )
      {
		/* Paged segments have a hash entry for each 4K page of data.
         Thus these hash entries must be skipped to find the hash
         entry for the next segment. */

		/* Compute number of 4K pages by rounding up */
      int num_4k_pages = ( ( (int) seg_ptr->size - 1 ) / SIZE4K ) + 1;
      
	  hash_tbl_offset += num_4k_pages * hash_digest_size;
      }
    }
    else if ( MI_PBT_PAGE_MODE_VALUE(seg_ptr->memory_attribute) == 
              MI_PBT_NON_PAGED_SEGMENT )
    {
      /* If the hash table segment has an entry for itself in the hash table,
         it must be zeros. For compatibility, check if the hash table segment
         was added to the ELF file after the hash table was generated and thus
         there is no entry in the hash table for itself.  In this case, the
         hash table segment will be the first ELF segment and the first entry
         in the hash table would not be all zeros.  Don't skip over the
         entry in the hash table for the hash table segment if the entry
         doesn't exist. */
      
      if ( ( MI_PBT_SEGMENT_TYPE_VALUE(seg_ptr->memory_attribute) != 
             MI_PBT_HASH_SEGMENT ) ||
           ( sbl_sahara_is_zeros(hash_tbl_ptr + hash_tbl_offset, 
                                  hash_digest_size) ) )
      {
        /* Point to hash for the next segment in the table */
        hash_tbl_offset += hash_digest_size;
      }
    }
    else
    {
      /* Fail if the segment is not PAGED or NON_PAGED */
      BL_VERIFY( 0x0, BL_ERR_INVALID_ELF_FORMAT|BL_ERROR_GROUP_BOOT );
    }
  }
}

/*===========================================================================
**  Function :  sbl_sahara_populate_sbl_info
** ==========================================================================
*/
/*!
* 
* @brief
*   This function populates meta data pointers to be passed to sec img auth
*   lib
*
* @par Dependencies
*   None 
* 
* @retval
*   SEC_IMG_AUTH_SUCCESS - If ELF images is verified successfully
*   ERROR VALUES - In case of failure.
* 
*/
static uint32 sbl_sahara_populate_sbl_info (sec_img_auth_elf_info_type *sbl_elf_info, 
                                sec_img_auth_whitelist_area_param_t *sbl_white_list_param,
                                whitelst_tbl_entry_type * config_entry_whitelist_table )
{
  uint32 index = 0;
  sec_img_auth_error_type status = SEC_IMG_AUTH_FAILURE;

  do
  {  
    sbl_white_list_area[0].start = (uintnt)0x0;
    sbl_white_list_area[0].end   = (uintnt)0xFFFFFFFFFFFFFFFF;
    sbl_white_list_param->area = &sbl_white_list_area[0];
    sbl_white_list_param->count = 0x1;

    if(NULL != config_entry_whitelist_table)
    {
      while((0x0 !=config_entry_whitelist_table->size) &&
            (index < (MAX_IMG_BUFFERS -1))
           )
      {
        /*Fill up the secImgAuth whitelist */
        sbl_white_list_area[index].start = config_entry_whitelist_table->start_addr;
        sbl_white_list_area[index].end = (config_entry_whitelist_table->start_addr + 
                                          config_entry_whitelist_table->size);
        config_entry_whitelist_table++;
        index++;      
      }
      
      if(config_entry_whitelist_table->start_addr != 0x0)
      { 
        /* At this point we should reach end of config entry whitelist
           and have null value unless the index maxed out the limit
        */
        status = SEC_IMG_AUTH_MI_BOOT_INVALID_SIZE;
        break;
      }
      sbl_white_list_param->count = index;
    }

    sbl_elf_info->elf_hdr      = (void *)&(sbl_sahara_elf_image_info.elf_header);
    sbl_elf_info->prog_hdr     = (void *)(sbl_sahara_get_program_header());
    sbl_elf_info->hash_seg_hdr = boot_get_hash_segment_buffer();

    /*If we reach this point all is good */
    status = SEC_IMG_AUTH_SUCCESS;
  }while(0);

  return status;
}


/* ============================================================================
**  Function : sbl_sahara_authenticate
** ============================================================================
*/
/*!
*   @brief
*   Function to authenticate an image given the image header.
*    
*   @details
*   Function to authenticate an image given the image header.
*   
*   @param *header_ptr   -  [IN] Pointer to image header
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  TRUE if authentication was successful or authentication was disabled,
*            FALSE otherwise
*   
*   @sa None
*/
static boolean sbl_sahara_authenticate
(
  uint8 *header_ptr
)
{
  boolean status = FALSE;
  bl_error_boot_type sec_img_auth_error_status = BL_ERR_IMG_SECURITY_FAIL;
  sec_img_auth_error_type populate_sbl_info;
  sec_img_auth_elf_info_type sbl_elf_info;
  sec_img_auth_whitelist_area_param_t sbl_white_list_param;

  BL_VERIFY( sbl_sahara_shared_data.expected_image_type == SAHARA_IMAGE_TYPE_ELF, 
            (uint16)sec_img_auth_error_status|BL_ERROR_GROUP_BOOT );

  /* Set the image whitelist for whitelist based image loading */
    BL_VERIFY(boot_set_active_img_whitelist_table(whitelist_tbl),
              BL_ERR_IMG_SECURITY_FAIL); 

  //load elf image info
  populate_sbl_info = (sec_img_auth_error_type) sbl_sahara_populate_sbl_info(&sbl_elf_info, &sbl_white_list_param,
                                        whitelist_tbl);
  BL_VERIFY( populate_sbl_info == SEC_IMG_AUTH_SUCCESS, BL_ERR_IMG_SECURITY_FAIL );

  /* Authenticate the image metadata (hashtable)*/ 
	  
  sec_img_auth_error_status = boot_auth_image_hashtable
                              ((uint32)image_sec_type,
                                &sbl_elf_info,
                                &sbl_white_list_param
                              );
  BL_VERIFY( sec_img_auth_error_status == BL_ERR_NONE, 
            (uint16)sec_img_auth_error_status|BL_ERROR_GROUP_BOOT ); 

  sec_img_auth_error_status = BL_ERR_NONE;
  
  /* Authenticate the image */ 
  if (BL_ERR_NONE != sec_img_auth_error_status)
  {
     /* set the flag tracking hash table authentication to FALSE for current elf */
     boot_set_hash_table_auth_result(FALSE);
     sbl_sahara_clear_downloaded_data();
  }
  else
  {
    /* set the flag tracking hash table authentication to TRUE for current elf */
    boot_set_hash_table_auth_result(TRUE);
     status = TRUE;
  }
  return status;
}
  
/* ============================================================================
**  Function : sbl_sahara_image_load_post_proc
** ============================================================================
*/
/*!
*  @brief
*  Function for post processing after an image load.
*    
*  @details
*  Function for post processing after an image load.
*   
*  @param
*    None
*   
*  @par Dependencies
*    None
*   
*  @par Side Effects
*    None
*   
*  @retval  SAHARA_STATUS_SUCCESS if post processing was successful else
*           error code as indicated by boot_sahara_status
*   
*  @sa None
*/
static enum boot_sahara_status sbl_sahara_image_load_post_proc (void)
{
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS;
  
  /* Placeholder for any post image-load steps specific for an image */

  return status;
}
  
/* ============================================================================
**  Function : sbl_elf_optimized_segment_load
** ============================================================================
*/
/*!
*  @brief
*  Function to configure elf loading logic.
*    
*  @details
*  Stubbed out. Not supported for 8084
*   
*  @param
*  None
*   
*  @par Dependencies
*  None
*   
*  @par Side Effects
*  None
*   
*  @retval  Boolean. True if single read request is to be sent 
*           for loading all elf segments
*   
*  @sa None
*/  
static boolean sbl_elf_optimized_segment_load (void)
{
  return FALSE;
}

/* ============================================================================
**  Function : sbl_sahara_key_segment_compare
** ============================================================================
*/
/*!
*  @brief
*  Function to compare two segment records in program header table.
*    
*  @details
*  This api compares two segments and returns comparision result.
*  segments are compared based upon offset values in elf
*   
*  @param *key_value_segment_ptr   -  [IN] Pointer to key segment
*
*  @param *sahara_elf_phdrs        -  [IN] Pointer to other segment
*                                           to be compared
*  
*  @param sort_index               -  [IN] Index for other segment.
* 
*  @par Dependencies
*  None
*   
*  @par Side Effects
*  None
*   
*  @retval  True   key value segment is lesser than other segment
*           False  In all other cases
*   
*  @sa 
*  None
*/ 
static boolean sbl_sahara_key_segment_compare
( 
  struct segment_record *key_value_segment_ptr,
  struct segment_record *sahara_elf_phdrs,
  int sort_index
 )
{
  boolean result = FALSE;
  if(sort_index >=0)
  {
    if(key_value_segment_ptr->segment_offset < 
  	      sahara_elf_phdrs[sort_index].segment_offset)
    {
      result = TRUE;
    }
  }
  return result;
}

/* ============================================================================
**  Function : sbl_sahara_preprocess_header
** ============================================================================
*/
/*!
*  @brief
*  Function to preprocess elf headers.
*    
*  @details
*  This api sorts the program header based upon offset length, so 
*  segments can be requested, lowest offset first.
*   
*  @param *sahara_elf_phdrs   -  [IN] Pointer to program header table
*
*  @param numsegments         -  [IN] Number of segments in program header table
*
*  @param *segment_start_offset -  [OUT] Pointer to first segment
*                                    offset
*  
*  @param *remaining_size   -  [OUT] Pointer to size of remaining elf image
*                               ie size from first segment start to last segment
*                               end
* 
*  @par Dependencies
*    None
*   
*  @par Side Effects
*    None
*   
*  @retval  SAHARA_STATUS_SUCCESS if post processing was successful else
*           error code as indicated by boot_sahara_status
*   
*  @sa None
*/ 
static enum boot_sahara_status sbl_sahara_preprocess_header 
( 
  struct segment_record *sahara_elf_phdrs,
  uint32 numsegments,
  uint32 *segment_start_offset,
  uint32 *remaining_size
 )
{
  enum boot_sahara_status status = SAHARA_NAK_TARGET_INIT_FAILURE;
  int32 current_index =0, sort_index =0;
  uint32 start_index = 0, end_index = 0;
  struct segment_record key_value_segment;
  boolean valid_load_seg_found = FALSE;
  
  if(sahara_elf_phdrs!= NULL && segment_start_offset!= NULL &&
     remaining_size != NULL )
  {
    original_segment_index[0]=0;  

    /* Insertion sort program header table */
    for (current_index = 1; 
         (current_index < numsegments); 
         current_index++)
    {
      /* Ignore 0 size segment */
      if ((sahara_elf_phdrs[current_index].size == 0) && (sahara_elf_phdrs[current_index].mem_size == 0))
      {
        original_segment_index[current_index] = current_index;
        continue;
      }

      qmemcpy(&key_value_segment, &sahara_elf_phdrs[current_index], 
              sizeof(key_value_segment));

      sort_index = current_index-1;
      /* Compare if key_value_segment < sahara_elf_phdrs[sort_index] */
      /* Move 0 size segment to the end */
      while ((sort_index >= 0) &&
             (((sahara_elf_phdrs[sort_index].size == 0) && (sahara_elf_phdrs[sort_index].mem_size == 0)) ||
             (TRUE == sbl_sahara_key_segment_compare(&key_value_segment,
                                                     sahara_elf_phdrs,
                                                     sort_index))))
      {
        qmemcpy( &sahara_elf_phdrs[sort_index+1],
                &sahara_elf_phdrs[sort_index],
                sizeof(sahara_elf_phdrs[sort_index+1]));
      
        original_segment_index[sort_index+1] = original_segment_index[sort_index];
        sort_index = sort_index-1;
      }
      qmemcpy( &sahara_elf_phdrs[sort_index+1],
              &key_value_segment,
              sizeof(sahara_elf_phdrs[sort_index+1]));    
      original_segment_index[sort_index+1] = current_index;
    }
    
    /*Find first loadable segment offset in sorted table. Compute 
      remaining image size from the offset till end of image */
    *remaining_size = 0;
    *segment_start_offset = 0;
    
    for(current_index = 0; current_index < numsegments; current_index++)
    {
      /* Find very first and last loadable segments with non-zero filesize  */
      if( boot_sahara_elf_segment_is_loadable(
          sahara_elf_phdrs[current_index].type,
          sahara_elf_phdrs[current_index].memory_attribute) &&
          (sahara_elf_phdrs[current_index].size !=0)
        )
      { 
        if(valid_load_seg_found == FALSE)
        { 
          /*If flag is false this is first valid loadable segment */
          start_index = current_index;
          valid_load_seg_found = TRUE;
        }
        if(valid_load_seg_found == TRUE)
      { 
          /*If only one segment end index same as start, else 
            keep track of latest valid loadable segment
          */
          end_index = current_index;
        }
      }
    }
    if(valid_load_seg_found == TRUE)
    {
      *segment_start_offset = sahara_elf_phdrs[start_index].segment_offset;
      *remaining_size = sahara_elf_phdrs[end_index].segment_offset +
                        sahara_elf_phdrs[end_index].size - 
                        sahara_elf_phdrs[start_index].segment_offset;    
    }
    status = SAHARA_STATUS_SUCCESS;       
  }
  return status;
}
  
/* ============================================================================
**  Function : sbl_sahara_rx_bulk
** ============================================================================
*/
/*!
*   @brief
*   Wrapper for the receive bulk function.
*   
*   @details
*   Wrapper for the receive bulk function.
*   
*   @param  rx_buf -     [IN] - buffer to fill with incoming data
*   @param  len    -     [IN] - buffer length
*   @param  rx_cb  -     [IN] - Callback function from Sahara
*   @param  p_err_code - [OUT] - error code
*   @param  type       - [OUT] - type of packet
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns number of bytes received
*   
*   @sa None
*/
static uint32 sbl_sahara_rx_bulk 
(
  uint8 *rx_buf, 
  uint32 len, 
  sahara_rx_cb_type rx_cb, 
  uint32 * err_code, 
  enum boot_sahara_packet_type type
)
{
#ifdef SAHARA_OVER_UART	
  return boot_transport_protocol_pkt_rx(rx_buf, len, rx_cb, err_code);
#else  
  return boot_usb_al_bulk_receive(rx_buf, len, rx_cb, err_code);
#endif
}

/* ============================================================================
**  Function : sbl_sahara_tx_bulk
** ============================================================================
*/
/*!
*   @brief
*   Wrapper for the transmit bulk function.
*   
*   @details
*   Wrapper for the transmit bulk function.
*   
*   @param  tx_buf -     [IN] - buffer having data to be transmitted
*   @param  len    -     [IN] - buffer length
*   @param  tx_cb  -     [IN] - Callback function from Sahara
*   @param  p_err_code - [OUT] - error code
*   @param  type       - [OUT] - type of packet
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns number of bytes transmitted
*   
*   @sa None
*/

static void sahara_tx_cb
(
  uint32 bytes_written, 
  uint32 err_code
){}

static uint32 sbl_sahara_tx_bulk 
(
  uint8 *tx_buf, 
  uint32 len, 
  sahara_tx_cb_type tx_cb, 
  uint32 * err_code, 
  enum boot_sahara_packet_type type
)
{
#ifdef SAHARA_OVER_UART	
  uint32 result=0;
  struct sahara_packet_command* sahara_tx_buffer = (struct sahara_packet_command *) tx_buf;
   
  if(type == SAHARA_PACKET_TYPE_COMMAND)
  {
    /* It is a command packet. Send the first 8 bytes and then the rest */ 
    result = boot_transport_protocol_pkt_tx((uint8 *)sahara_tx_buffer, sizeof(struct sahara_packet_command), 
                                             sahara_tx_cb, (uint32 *)err_code);    
	
    if(*err_code == BULK_SUCCESS)
	{
	  /* Send the remaining bytes of the packet */		
	  result += boot_transport_protocol_pkt_tx((uint8 *)(tx_buf+sizeof(struct sahara_packet_command)), 
	                              sahara_tx_buffer->length-sizeof(struct sahara_packet_command), 
								  sahara_tx_cb, (uint32 *)err_code);  		
	}	  
  } 
  else
  {
	/* Data packet */
 	result = boot_transport_protocol_pkt_tx(tx_buf, len, tx_cb, err_code); 
  }
	
  /* Call back into Sahara */
  tx_cb(result, *err_code);
  return result; 
#else  
  return boot_usb_al_bulk_transmit(tx_buf, len, tx_cb, err_code);
#endif
}

/* ============================================================================
**  Function : sbl_sahara_bulk_init
** ============================================================================
*/
/*!
*   @brief
*   Initialize HS-USB clocks and PHY.
*   
*   @details
*   This function first disables data cache and then proceeds to initialize
*   the HS-USB clocks and PHY.
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  BULK_SUCCESS if no error
*   
*   @sa None
*/
static uint32 sbl_sahara_bulk_init( void )
{
#ifdef SAHARA_OVER_UART
  return (boot_transport_layer_init(&sbl_packet_interface));
#else  
  boot_qusb_al_bulk_init(QUSB_MAX_SPEED_SUPER);

  return BULK_SUCCESS;
#endif  
}

/* ============================================================================
**  Function : sbl_sahara_bulk_poll
** ============================================================================
*/
/*!
*   @brief
*   This function polls for data in the RX pipe
*   
*   @details
*   This function polls for data in the RX pipe
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void sbl_sahara_bulk_poll( void )
{
#ifdef SAHARA_OVER_UART	
  boot_hsuart_poll();
  return;
#else
  boot_usb_al_bulk_poll();
  return;
#endif  
}

/* ============================================================================
**  Function : sbl_sahara_bulk_shutdown
** ============================================================================
*/
/*!
*   @brief
*   Initialize HS-USB clocks and PHY.
*   
*   @details
*   This function first disables data cache and then proceeds to initialize
*   the HS-USB clocks and PHY.
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  BULK_SUCCESS if no error
*   
*   @sa None
*/
static void sbl_sahara_bulk_shutdown( void )
{
#ifdef SAHARA_OVER_UART	
	boot_hsuart_close();
#else	
	boot_usb_al_bulk_shutdown();
#endif
}


/* ============================================================================
**  Function : sbl_get_max_packet_size
** ============================================================================
*/
/*!
*   @brief
*   Initialize HS-USB clocks and PHY.
*   
*   @details
*   This function first disables data cache and then proceeds to initialize
*   the HS-USB clocks and PHY.
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  BULK_SUCCESS if no error
*   
*   @sa None
*/
static uint32 sbl_get_max_packet_size( void )
{
#ifdef SAHARA_OVER_UART	
	return boot_hsuart_get_max_packet_size();
#else	
	return boot_usb_al_bulk_get_max_packet_size();
#endif
}


/* ============================================================================
**  Function : sbl_get_max_raw_data_transfer_size
** ============================================================================
*/
/*!
*   @brief
*   Initialize HS-USB clocks and PHY.
*   
*   @details
*   This function first disables data cache and then proceeds to initialize
*   the HS-USB clocks and PHY.
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  BULK_SUCCESS if no error
*   
*   @sa None
*/
static uint32 sbl_get_max_raw_data_transfer_size( void )
{
#ifdef SAHARA_OVER_UART	
	return boot_hsuart_get_max_raw_data_transfer_size();
#else	
	return boot_usb_al_bulk_get_max_raw_data_size();
#endif
}

/* ============================================================================
**  Function : sbl_sahara_exec_cmd
** ============================================================================
*/
/*!
*   @brief
*   Executes a command based on the input client_command and set the
*   resp_buffer with the corresponding response.
*   
*   @details
*   This function decodes the client_command and executes the corresponding
*   functions that are supported by SBL. The pointer to the response data
*   is returned, along with the length and status of execution.
*   
*   @param client_command   -  [IN    ] ID of command to execute
*   @param *resp_length     -  [IN/OUT] Response length for executed command
*   @param *status          -  [IN/OUT] Status of executed command
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns pointer to response data.
*   
*   @sa None
*/
static uint8* sbl_sahara_exec_cmd
(
  uint32 client_command,
  uint32 *resp_length,
  uint32 *status
)
{
  /* Store client command responses */
  uint32 resp_buff;
  enum boot_sahara_status exec_status = SAHARA_STATUS_SUCCESS;

  /* Check for maximum size required for current commands supported
     and input parameters */
  if (resp_length == NULL)
  {
    /* ERROR: Invalid parameter passed to command execution */
    exec_status = SAHARA_NAK_EXEC_CMD_INVALID_PARAM;
  }
  else
  {
    /* Execute command based on input client_command */
    switch(client_command)
    {
      case SAHARA_EXEC_CMD_SERIAL_NUM_READ:
      resp_buff=HWIO_INF( QFPROM_CORR_PTE_ROW1_LSB, SERIAL_NUM);
      *resp_length = sizeof(resp_buff);

		  /* Flush cache  */
      dcache_flush_region(&resp_buff, sizeof(resp_buff));
      
      qmemcpy(&sahara_cmd_exec_data_resp_buf,&resp_buff,*resp_length); 

        break;	
		
      default:
        *resp_length = 0;
        /* ERROR: Unsupported client command received */
        exec_status = SAHARA_NAK_EXEC_CMD_UNSUPPORTED;
        break;
    }
  }

  *status = exec_status;
  return (uint8 *)&sahara_cmd_exec_data_resp_buf; 
}

/* ============================================================================
**  Function : sbl_sahara_mem_debug_copy_restricted
** ============================================================================
*/
/*!
*   @brief
*   Copies the data to a temporary buffer for a restricted region
*    
*   @details
*   Copies the data to a temporary buffer for a restricted region
*      
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  Pointer to the buffer
*   
*   @sa None
*/
static uint64 sbl_sahara_mem_debug_copy_restricted
(
  const uint64 addr,
  uint64 len
)
{
  uint64 mem_debug_buffer_addr = 0;
  uint32 rem_size = 0; 
  // modified the function for reading the special region or raw region
  if((addr >= (uint64)raw_partition_addr) && (addr+len <= ((uint64)raw_partition_addr + raw_partition_len)))
  {
	  rem_size = len;
  	 /* If  partition exists */
	if(trans_if != NULL  &&  len <=SAHARA_MAX_MEMORY_DATA_SIZE_IN_BYTES)
	{
		boot_set_entry_as_img_whitelist((uintnt)sbl_sahara_mem_debug_buffer , (uintnt)len);	
			
		boot_flash_trans_read(trans_if,
					(void *)sbl_sahara_mem_debug_buffer,
					addr-(uint64)raw_partition_addr,
					len,
					IMAGE_BODY_TYPE,
					&rem_size);

		boot_invalidate_img_whitelist();
		
		mem_debug_buffer_addr = (uint64)&sbl_sahara_mem_debug_buffer[0];
   	
    }
  }
  else if ((addr > 0) && (len <= SAHARA_MAX_MEMORY_DATA_SIZE_IN_BYTES))
  {
     /* Copy over data to temporary buffer */
     qmemcpy(sbl_sahara_mem_debug_buffer,
             (const uint8*)addr, 
             len);
     mem_debug_buffer_addr = (uint64)&sbl_sahara_mem_debug_buffer[0];
  }
  return mem_debug_buffer_addr;
}

/* ============================================================================
**  Function : sbl_sahara_is_auth_enabled
** ============================================================================
*/
/*!
*   @brief
*   Returns whether image authentication is enabled or not.
*    
*   @details
*   Returns whether image authentication is enabled or not.
*      
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  TRUE if authentication is enabled,
*            FALSE otherwise
*   
*   @sa None
*/
static boolean sbl_sahara_is_auth_enabled(void)
{  
  return (boolean) auth; 
}

static uint64 sbl_sahara_get_bin_header_size(void)
{
  return sizeof(mi_boot_image_header_type);
}

static uint64 sbl_sahara_get_bin_image_size(const uint8 *header)
{
  mi_boot_image_header_type* image_header = (mi_boot_image_header_type*)header;
  return image_header->image_size;
}

static uint64 sbl_sahara_get_bin_code_size(const uint8 *header)
{
  mi_boot_image_header_type* image_header = (mi_boot_image_header_type*)header;
  return image_header->code_size;
}

static uint32 sbl_sahara_get_bin_cert_chain_size(const uint8 *header)
{
  mi_boot_image_header_type* image_header = (mi_boot_image_header_type*)header;
  return image_header->cert_chain_size;
}

static uint32 sbl_sahara_get_bin_signature_size(const uint8 *header)
{
  mi_boot_image_header_type* image_header = (mi_boot_image_header_type*)header;
  return image_header->signature_size;
}

static boolean sbl_sahara_skip_image_load(void)
{
  /* Do not error on header corruption for these images instead move to loading the
     next image */
  if ((sbl_sahara_shared_data.expected_image == RAMFS1_IMG) || 
      (sbl_sahara_shared_data.expected_image == RAMFS2_IMG) ||
      (sbl_sahara_shared_data.expected_image == BACKUP_RAMFS_IMG) ||
      (sbl_sahara_shared_data.expected_image == ACDB_IMG))
  {
    return TRUE;
  }
  else
    return FALSE;
}

static void sbl_hw_reset_config (void)
{
  /*Set reset controller to enter second pass state after 
    warm reset and start from bootrom. sdi not entered since
    apcs start addr reg is cleared.

    If we see the FSM is in first pass, we cannot trigger wdog bite
    reset so force the first pass complete state for FSM

    Below changes needed to support use case of taking ramdump even after a 
    usb plugout-plugin event, in case its not complete
  */
  
  //if(HWIO_INF(GCC_RESET_FSM_STATUS, FSM_STATE) == 0X6)
   // HWIO_OUTF(GCC_RESET_FSM_CTRL, FIRST_PASS_COMPLETE, 0X1);

  /* Configure GCC_RESET_DEBUG_ENABLE */    
  //HWIO_OUT(GCC_RESET_DEBUG_ENABLE,0x76);
      
}

static void sbl_hw_reset_wdog (void)
{
  /*Set reset controller to enter second pass state after
    warm reset and start from bootrom. sdi not entered since
    apcs start addr reg is cleared.

    If we see the FSM is in first pass, we cannot trigger wdog bite
    reset so force the first pass complete state for FSM
  */
  
  /*static uint32 reset_reason[5] = {0};
  reset_reason[0] = HWIO_IN(GCC_RESET_STATUS);	
  HWIO_OUT(GCC_RESET_STATUS, reset_reason[0]);
  
 reset_reason[1] = HWIO_IN(GCC_RESET_FSM_STATUS);	
  HWIO_OUT(GCC_RESET_FSM_STATUS, reset_reason[1]);
  
  if(HWIO_INF(GCC_RESET_FSM_STATUS, FSM_STATE) == 0X6)
    HWIO_OUTF(GCC_RESET_FSM_CTRL, FIRST_PASS_COMPLETE, 0X1);

   Note: FIRST_PASS_TMR does not seem to move FSM to 2nd pass state */
 // HWIO_OUT(GCC_RESET_DEBUG_ENABLE,0x76);
  /* Set minimum timer value */
 // HWIO_OUT(GCC_FIRST_PASS_TIMEOUT, 0x10000);
  /*Enable first pass timer */
 // HWIO_OUTF(GCC_RESET_FSM_CTRL,FIRST_PASS_TMR_EN,0x1);

  HWIO_OUTF(MPM2_WDOG_CTL_REG, WDOG_EN, 0);
  
  
  /*Cause secure watchdog bite */
  /* Program bark and bite timeouts. */
  HWIO_OUTF(MPM2_WDOG_BARK_VAL_REG, WDOG_BARK_VAL, 0x2);
  while(HWIO_INF(MPM2_WDOG_BARK_VAL_REG, SYNC_STATUS) == 0);


  HWIO_OUTF(MPM2_WDOG_BITE_VAL_REG, WDOG_BITE_VAL, 0x1);
  while(HWIO_INF(MPM2_WDOG_BITE_VAL_REG, SYNC_STATUS) == 0);

  HWIO_OUTF(MPM2_WDOG_CTL_REG, WDOG_EN, 1);
  HWIO_OUTF(MPM2_WDOG_CTL_REG, WDOG_CLK_EN, (uint32)1);

  /* Reset to make sure the count starts at 0. */
  HWIO_OUT(MPM2_WDOG_RESET_REG, 1);
}

static void sbl_sahara_unrecoverable_error_handler(void)
{
  /* when sbl sahara encounters an unrecoverable error such as cable unplugged, 
     shutdown usb hw and reboot into dload mode*/
  sbl_sahara_bulk_shutdown();
  boot_dload_set_cookie();
  
  /* Usually at this point we only call boot_hw_reset(BOOT_WARM_RESET_TYPE)
    to warm reset. However we also need to configure GCC_RESET_DEBUG_ENABLE
    to ensure PIMEM is retained across warm reset.*/
  boot_hw_reset(BOOT_WARM_RESET_TYPE);
}

static void sbl_sahara_reset(void)
{
  /* if sahara status indicates failure, then we need to save SBL crash dumps */
  if (sbl_sahara_interface.sahara_status != SAHARA_STATUS_SUCCESS)
  {
    /* this function is not going to return */
    sbl_error_handler();
  }
  
  if(trans_if != NULL)
  {   
	boot_flash_dev_close_image(&trans_if);  
  }	
  boot_hw_reset(BOOT_HARD_RESET_TYPE);
}

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* ============================================================================
**  Function : sbl_sahara_get_interface
** ============================================================================
*/
/*!
*   @brief
*   Returns pointer to the Sahara interface.
*   
*   @details
*   Returns pointer to the Sahara interface.
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns pointer to the Sahara interface.
*   
*   @sa None
*/
struct boot_sahara_interface* sbl_sahara_get_interface( void )
{
  return &sbl_sahara_interface;
}

/* ============================================================================
**  Function : sbl_sahara_entry
** ============================================================================
*/
/*!
*   @brief
*   This function provides an entry into the Sahara protocol from SBL1.
*   This function performs any initialization required and jumps into the
*   protocol to transfer images directly to memory.
*   
*   @details
*   This function provides an entry into the Sahara protocol from SBL1.
*   This function performs any initialization required and jumps into the
*   protocol to transfer images directly to memory.
*   
*   @param none
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
void sbl_sahara_entry ( 
   mi_boot_image_header_type *header_ptr,
   uint32 image_id,
   uint32 image_type,
   uint32 sec_image_type,
   boot_boolean auth_flag,
   boot_boolean jump_flag,      
   whitelst_tbl_entry_type * config_entry_whitelist_table
)
{ 
  sbl_sahara_shared_data.expected_image = image_id;
  sbl_sahara_shared_data.expected_image_type = (enum boot_sahara_image_type)image_type;  
  auth = auth_flag;
  image_sec_type = sec_image_type;
  whitelist_tbl = config_entry_whitelist_table;

#ifdef SAHARA_OVER_UART
  sbl_sahara_interface.sahara_mode = SAHARA_MODE_IMAGE_TX_COMPLETE;
#else  
  sbl_sahara_interface.sahara_mode = SAHARA_MODE_MEMORY_DEBUG;
#endif
  
  /* Set the cur_image pointer based on ELF or Binary */
  if(image_type == SAHARA_IMAGE_TYPE_ELF)
  {
    sbl_sahara_shared_data.cur_image = &sbl_sahara_elf_image_info; 
    
    /* reset the flag tracking hash table authentication result for current elf */
    boot_set_hash_table_auth_result(FALSE);

    /* Clear hash table buffer */
    qmemset(boot_get_hash_segment_buffer(), 0x0, boot_get_hash_buffer_size());

    /* Clear program headers */
    qmemset(&elf_prog_headers, 0x0, sizeof(struct progressive_boot_block));

    /* Clear Elf header */
    qmemset(&(sbl_sahara_elf_image_info.elf_header), 0x0, sizeof(Elf32_Ehdr));

    /* Initialize program header location */
    sbl_sahara_elf_image_info.prog_headers = &elf_prog_headers;
    sbl_sahara_elf_image_info.prog_headers_size = sizeof(struct progressive_boot_block);
  }
  else
  {
    sbl_sahara_shared_data.cur_image = &sbl_sahara_bin_image_info;   
    sbl_sahara_bin_image_info.header = (uint8 *)header_ptr;	
  }
  
  /* Download image to memory */
  BL_VERIFY(boot_sahara_entry(&sbl_sahara_interface), BL_ERR_BOOT_SAHARA_INIT_FAIL|BL_ERROR_GROUP_BOOT);
  BL_VERIFY((sbl_sahara_interface.sahara_status == SAHARA_STATUS_SUCCESS), BL_ERR_OTHERS|BL_ERROR_GROUP_BOOT);	
}

/* ============================================================================
**  Function : sbl_sahara_get_elf_header
** ============================================================================
*/
/*!
*   @brief
*   This function returns elf header base address.
*   
*   @details
*   This function returns elf header base address for the last loaded elf image
*   
*   @param none
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
uint64 sbl_sahara_get_elf_header()
{
  return (uint64)&(sbl_sahara_elf_image_info.elf_header);
}

/* ============================================================================
**  Function : sbl_sahara_get_program_header
** ============================================================================
*/
/*!
*   @brief
*   This function returns program header buffer base address.
*   
*   @details
*   This function returns program header buffer base address
*   for the last loaded elf image.
*   
*   @param none
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
uint64 sbl_sahara_get_program_header()
{
  return (uint64)&(sbl_sahara_elf_image_info.prog_headers->segment[0]);
}
