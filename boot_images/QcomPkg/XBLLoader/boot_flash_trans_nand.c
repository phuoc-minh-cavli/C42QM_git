/*=============================================================================

                                   Boot Loader
                         NAND Flash Translation Module

GENERAL DESCRIPTION
  This file contains the implementation for the boot-specific NAND flash
  translation module/layer.

Copyright 2010, 2012, 2015-2019, 2021 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/27/21   rrwt    Fixed preamble_cookie page_per_block for AUTODETECT_PAGE_SIZE_MAGIC_NUM128
10/01/19   aus     Fixed issue so that parallel read size is always PARALLEL_LOAD_SIZE
10/01/19   aus     Fixed issue when block 0 of partition is bad block
08/14/19   aus     Updated to support parallel load for multi-page
12/04/18   rohik   Added write api for NAND for FOTA feature
12/28/17   rohik   Added whitelist protection and removed clobber protection
10/05/15   ck      Corrected copyright
06/06/12   kedara  Fix compilation issue for boot_flash_trans_vtbl_type.
09/21/10   plc     Initial port in for SB3.0
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <string.h>
#include "boot_flash_target.h"
#include "boot_flash_trans_nand.h"
#include "boot_flash_dev_nand_if.h"
#include "boot_util.h"
#include "miparti.h"
#include "flash.h"
#include "boot_target.h"
#include "boot_parallel_check.h"
#include "sbl1_target.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/* General purpose page buffer, must be defined externally */
extern uint8 bl_page_buf[];
static boot_boolean preamble_present = FALSE;
static uint32 preamble_block = 0;
static uint32 pages_per_block = 64;

/* Definition of private NAND flash translation module data structure */
typedef struct boot_flash_trans_nand_type
{
  /*----------------------------------- 
   * Public module interface 
   *---------------------------------*/
  /* This _must_ be the first element of the structure. */
  boot_flash_trans_if_type pub_if;

  /* NAND flash module variables */
  struct fs_device_data *nand_if;       /* Pointer to NAND flash driver */
  uint32 region_base_addr;              /* Base offset of the flash region
                                           in bytes */
  uint32 region_size;                   /* Maximum size of the flash region
                                           in bytes */
  boolean sloppy_copy;                  /* Whether to do the next copy sloopy */

  /* Cached NAND flash module variables */
  uint32 block_size_bytes_shift_bits;
  uint32 block_count;
  uint32 ru_size_bytes;
  uint32 ru_size_shift_bits;
  
  /* Variables for maintaining a callback function in between page reads */
  boot_flash_trans_copy_callback_func_type copy_callback_func;
  void *copy_callback_data;

  /* Variables to optimize flash offset to physical address conversions */
  uint32 last_converted_logical_offset;  /* The last logical offset that was
                                            converted to a physical address */
  uint32 last_converted_physical_addr;   /* The last physical address that was
                                            converted from a logical offset */
} boot_flash_trans_nand_type;


/* The boot translation factory private data structure */
typedef struct boot_flash_trans_nand_factory_private_type
{
  /*----------------------------------- 
   * Public module interface 
   *   Note: This _must_ be the first element of the structure.
   *---------------------------------*/
  boot_flash_trans_factory_if_type pub_if;

  /* Array of the boot translation private data structures allocated
   * by factory */
  boot_flash_trans_nand_type trans_priv[BOOT_FLASH_TRANS_MAX_INSTANCES];

  /* Array indicating which elements of the boot translation private data
   * array are in use */
  boolean trans_priv_inuse[BOOT_FLASH_TRANS_MAX_INSTANCES];

} boot_flash_trans_nand_factory_private_type;


/* The NAND translation factory private data */
static boot_flash_trans_nand_factory_private_type fact_priv;


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


/*=========================================================================

FUNCTION  GET_NEXT_GOOD_FLASH_AREA

DESCRIPTION
  Function to advance the given physical flash address to the start of
  the next good area of flash, by skipping over any bad blocks.

DEPENDENCIES
  None
  
RETURN VALUE
  The size of the next good area of flash if one was found, otherwise zero.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 get_next_good_flash_area
(
  boot_flash_trans_nand_type *trans_priv,  /* Pointer to our private type */
  uint32 *phys_flash_addr                  /* Location where the physical
                                              flash address is stored */
)
{
  /* Get a shorter convenience pointer to the NAND device interface */
  uint32 block, good_flash_addr, page, last_page;
  struct fs_device_data * const nand_if = trans_priv->nand_if;
  boot_boolean block_found, xbl_image = FALSE;
  uint32 preamble_cookie;
  boot_sbl_magic_num_type *sbl_magic_num = NULL;
  int rc;

    
  /* Compute the block number corresponding to the physical flash address */
  block = *phys_flash_addr >> trans_priv->block_size_bytes_shift_bits;
  xbl_image = boot_get_ramdump_flag();
  block_found = FALSE;

  if ((read_parallel == READ_PARALLEL_WITH_CHECK) || (read_parallel == READ_SEQUENTIAL_WITH_CHECK))
  {
	  rc = nand_if->wait_for_read( nand_if);
      BFA_NAND_VERIFY( rc == FS_DEVICE_DONE );
	  read_parallel = READ_SEQUENTIAL;
  }

  while(block_found == FALSE)
  {
    /* Loop through the bad block check until we find a good block */
    while ( ( block < trans_priv->block_count ) && 
            nand_if->bad_block_check( nand_if, block ) != FS_DEVICE_OK)
    {
       ++block;
    }
	
	if(xbl_image == TRUE) 
	{
		good_flash_addr = block << trans_priv->block_size_bytes_shift_bits;
	    page = good_flash_addr >> trans_priv->ru_size_shift_bits;
		rc = nand_if->read_page( nand_if, page, bl_page_buf );

		if(preamble_present == TRUE)
		{
		    preamble_cookie = *((uint32 *)(bl_page_buf + PREAMBLE_OFFSET));
		    if( (preamble_cookie == AUTODETECT_PAGE_SIZE_MAGIC_NUM_DEFAULT) ||
			    (preamble_cookie == AUTODETECT_PAGE_SIZE_MAGIC_NUM64) ||
				(preamble_cookie == AUTODETECT_PAGE_SIZE_MAGIC_NUM128) )
				{
					if(preamble_cookie != AUTODETECT_PAGE_SIZE_MAGIC_NUM_DEFAULT)
					{
						if(preamble_cookie == AUTODETECT_PAGE_SIZE_MAGIC_NUM128)
						{
							pages_per_block = 128;
						}
						last_page = page + pages_per_block - 1;
						rc = nand_if->read_page( nand_if, last_page, bl_page_buf );
						if(rc != FS_DEVICE_DONE)
						{
							++block;
						}
						else
						{
							block_found = TRUE;
						}
					}
					else
					{
						block_found = TRUE;
					}
				}
		    else
			   ++block;
		}
		else
		{
			sbl_magic_num = (boot_sbl_magic_num_type *)bl_page_buf;
			if( (sbl_magic_num->sbl_magic1 == SBL_MAGIC_NUM_1) &&
			    (sbl_magic_num->sbl_magic2 == SBL_MAGIC_NUM_2) &&
				(sbl_magic_num->sbl_magic3 == SBL_MAGIC_NUM_3) )
				{
					block_found = TRUE;
				}
			else
				++block;
		}
	}
	else
	{
		block_found = TRUE;
	}
  }

      /* make sure that we haven't passed the end of the flash */
      if( block >= trans_priv->block_count )
      {
        /* We have gone past the end of the NAND */
        return 0;
      }

   if(preamble_present == TRUE)
   {
    preamble_block = block;
    preamble_present = FALSE;
   }   

  /* Set the next good address equal to the next good block plus the
     offset into the good block (e.g. FSBL/OSBL images uses the first page
     of every block to store bad block magic) */
  good_flash_addr = ( block << trans_priv->block_size_bytes_shift_bits );    
   
  /* If the next good address is greater than the current address, use the
     next good address (this accounts for *phys_flash_addr potentially not
     being block-aligned) */
  if ( good_flash_addr > *phys_flash_addr )
  {
    *phys_flash_addr = good_flash_addr;
  }
  
  /* block non-zero phys addr + elf segment loading */
  block = *phys_flash_addr >> BLOCK_SIZE_IN_IMAGE;  
 
  
  if (xbl_image)
  { 
      if((block == preamble_block) && ((*phys_flash_addr - (block << BLOCK_SIZE_IN_IMAGE)) < PREAMBLE_SIZE))
            *phys_flash_addr = (block << BLOCK_SIZE_IN_IMAGE) + PREAMBLE_SIZE;
      else if((*phys_flash_addr - (block << BLOCK_SIZE_IN_IMAGE)) < sizeof(boot_sbl_magic_num_type))
            *phys_flash_addr = (block << BLOCK_SIZE_IN_IMAGE) + sizeof(boot_sbl_magic_num_type);
    
  }
  
  /* Return the size of the remainder of the good block */
  return ( ( block + 1 ) << BLOCK_SIZE_IN_IMAGE ) - *phys_flash_addr;
  
} /* get_next_good_flash_area() */

/*=========================================================================

FUNCTION  GET_PHYSICAL_FLASH_ADDR

DESCRIPTION
  Function to convert the given logical flash offset to a physical flash
  address, by skipping over any bad blocks.
  
DEPENDENCIES
  None
  
RETURN VALUE
  The physical flash address corresponding to the given logical offset

SIDE EFFECTS
  None

===========================================================================*/
static uint32 get_physical_flash_addr
(
  boot_flash_trans_nand_type *trans_priv,  /* Pointer to our private type */
  uint32 byte_offset,                       /* Flash offset to convert to a
                                              physical address */
  uint32 *first_copy_area
)
{
  /* Initialize the current physical flash address to the base of the region */
  uint32 phys_flash_addr = trans_priv->region_base_addr;
  /* The size of the current good flash area */
  uint32 area_size, block;
  boot_boolean xbl_image = FALSE;
 
  /* First check if the flash offset we want to convert follows the previous
     flash offset that we converted.  If so, then we can just start where
     the previous logical to physical conversion left off. */
  if ( byte_offset >= trans_priv->last_converted_logical_offset )
  {
    byte_offset -= trans_priv->last_converted_logical_offset;
    phys_flash_addr = trans_priv->last_converted_physical_addr;

    /* Save the flash offset we are converting, to save time if the next
       conversion follows this one */
    trans_priv->last_converted_logical_offset += byte_offset;
  }
  else
  {
    /* Save the flash offset we are converting, to save time if the next
       conversion follows this one */
    trans_priv->last_converted_logical_offset = byte_offset;
  }
  
  block =  phys_flash_addr >> BLOCK_SIZE_IN_IMAGE;

  xbl_image = boot_get_ramdump_flag();

  if((xbl_image == TRUE) && (block == 0))
  {
    preamble_present = TRUE;
  }
  
  if (xbl_image == FALSE)
  {   
    area_size = (( block + 1 ) << BLOCK_SIZE_IN_IMAGE ) - phys_flash_addr;
    if ( area_size > byte_offset )
    {
     *first_copy_area = area_size - byte_offset;
     area_size = byte_offset;
    }
    phys_flash_addr += area_size;
    byte_offset -= area_size;
 }
  
  /* Keep looping while the remaining flash offset is greater than zero */
  do
  {
    /* Find the next good area of flash (next good block) */
    area_size = get_next_good_flash_area( trans_priv, &phys_flash_addr );
    BFA_NAND_VERIFY2( area_size > 0, NULL );

    /* If the current good area is greater than the remaining flash offset,
       adjust down the current area size */
    if ( area_size > byte_offset )
    {
      *first_copy_area = area_size - byte_offset;
      area_size = byte_offset;
    }

    /* Subtract the current area size from the flash offset and advance
       the physical flash address */
    phys_flash_addr += area_size;
    byte_offset -= area_size;
  } while ( byte_offset > 0 );


  /* Return the physical address and save the converted physical address
     to save time if the next conversion follows this one sequentially. */
  return ( trans_priv->last_converted_physical_addr = phys_flash_addr );

} /* get_physical_flash_addr() */

/*=========================================================================

**  Function :  boot_flash_trans_nand_read

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to copy a region of data from a logical byte address
*   to the destination address.  Given the logical range to copy,
*   this function converts that request to page reads and skips
*   bad blocks, while ensuring that the copy does not overwrite
*   a protected bootloader memory region.
* 
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the operation succeeded, FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
static boolean boot_flash_trans_nand_read
(
  flash_trans_if_type *trans_if,  /* Pointer to the translation interface */
  void   *dest_addr,              /* Destination address */
  uint32  byte_offset,            /* Logical source byte address */
  uint32  size,                   /* Size to copy in bytes */
  uint32 *rem_size
)
{
  /* Cast the public interface to our private type */
  boot_flash_trans_nand_type * const trans_priv =
    (boot_flash_trans_nand_type*)trans_if;
  /* Get a shorter convenience pointer to the flash device interface */
  struct fs_device_data * const nand_if = trans_priv->nand_if;
  uint8 *data_ptr = (uint8*) dest_addr;  /* The current destination pointer */
  uint32 phys_flash_addr;  /* The current physical flash offset in bytes */
  uint8 *callback_dest_addr;  /* The current read pointer to pass to callback */
  uint32 callback_dest_size;  /* The current read size to pass to callback */
  uint32 area_size = 0; 
  uint32 page_offset, page, num_pages, copy_size, mpages=0, max_pages=0, rem_pages=0;

  int rc;

  BFA_NAND_VERIFY( trans_if != NULL );

  /* Next translate the logical flash offset to the physical flash address */
  phys_flash_addr = get_physical_flash_addr( trans_priv, byte_offset, &area_size);
  
  /* Copy pages from flash while there is data left to copy */
  while ( size > 0 )
  {
    
    /* Make sure that we have not exceeded to size of the region */
    if ( trans_priv->region_size > 0 )
    {
      BFA_NAND_VERIFY( ( phys_flash_addr + size ) <= 
                       ( trans_priv->region_base_addr + 
                         trans_priv->region_size ) );
    }

    /* Save the flash offset corresponding to the current physical address,
       to save time if the next logical to physical conversion follows
       the current flash offset sequentially */
    trans_priv->last_converted_physical_addr = phys_flash_addr;
    trans_priv->last_converted_logical_offset = byte_offset;
    byte_offset += area_size;
    
    /* Adjust the good area size down if it is larger than the total copy size */
    if ( size < area_size )
    {
      area_size = size;
    }

    /* Save the RAM address and size that we are going to process in this
       iteration of the loop, and pass this to the callback function after
       we have read the pages from NAND */
    callback_dest_addr = data_ptr;
    callback_dest_size = area_size;
    
    /* Decrement the total copy size here while we have the full size of the
       good area */
    size -= area_size;

    /* Compute the page number and offset into the first page based on the 
       current physical flash address */
    page = phys_flash_addr >> trans_priv->ru_size_shift_bits;
    page_offset = phys_flash_addr - ( page << trans_priv->ru_size_shift_bits );

    /* First, if the initial offset is not page-aligned, then we need to load
       the data into a page buffer and copy the data from the buffer into memory */
    if ( page_offset > 0 )
    {
      /* If we allow the copy to be sloppy, then roll back the offset into 
         the page so that we copy the whole first page including the
         "leading pad".  This is the legacy QCSBL behavior. */
      if ( trans_priv->sloppy_copy )
      {
        phys_flash_addr -= page_offset;
        data_ptr -= page_offset;
        area_size += page_offset;
        page_offset = 0;
        
        /* Double check that the slooppy address range is still valid */
        /*status = boot_clobber_check_local_address_range( 
                   &trans_priv->pub_if.clobber_tbl, data_ptr, area_size );
        BFA_NAND_VERIFY( status );*/
      }
      else
    {      
      /* Set the copy size to be the remainder of the first page */
        copy_size = trans_priv->ru_size_bytes - page_offset;
        if ( copy_size > area_size )
        {
          copy_size = area_size;
        }

      /* Copy the partial page to a temporary buffer and then to the dest */
      rc = nand_if->read_page( nand_if, page, bl_page_buf );
      qmemcpy( data_ptr, bl_page_buf + page_offset, copy_size );
	  *rem_size += copy_size;
    
      /* If all we had to copy was the first partial page, then break
         here explicitly for efficieny */
      if ( size == 0 && area_size == copy_size )
      {
        break;
      }
	  
      area_size -= copy_size;
      data_ptr += copy_size;
      phys_flash_addr += copy_size;
      ++page; 
      break;	  
    }
    }

    /* Next we read as many whole pages as are left in the current good area */
    num_pages = area_size >> trans_priv->ru_size_shift_bits;
    copy_size = 1 << trans_priv->ru_size_shift_bits;

    /* If we allow the copy to be sloppy, then copy the entire last page
       including the "trailing pad", even if we are only interested in
       a partial last page.  This is the legacy QCSBL behavior. */
    if ( trans_priv->sloppy_copy && copy_size < area_size )
    {
      ++num_pages;
      copy_size = num_pages << trans_priv->ru_size_shift_bits;
      area_size = copy_size;
      
      /* Double check that the new sloppy address range is still valid */
     /* status = boot_clobber_check_local_address_range( 
                 &trans_priv->pub_if.clobber_tbl, data_ptr, copy_size );
      BFA_NAND_VERIFY( status ); */
    }

    /* Perform a multi-page read for contiguous whole pages */
    if ( num_pages > 0 )
    { 
      max_pages = PARALLEL_LOAD_SIZE >> trans_priv->ru_size_shift_bits; 
      rem_pages = *rem_size >> trans_priv->ru_size_shift_bits;
      

      if(is_load_parallel)
      {
        mpages = (num_pages > (max_pages - rem_pages))? (max_pages - rem_pages) : num_pages; 

        switch (read_parallel) {
        case(READ_SEQUENTIAL):
        {
		  rc = nand_if->start_read( nand_if, page, data_ptr, mpages);
          BFA_NAND_VERIFY( rc == FS_DEVICE_DONE );	
          rc = nand_if->wait_for_read( nand_if);
          BFA_NAND_VERIFY( rc == FS_DEVICE_DONE );
          num_pages = num_pages-mpages;
		  copy_size = (mpages << trans_priv->ru_size_shift_bits);

		  if(num_pages > max_pages)
             read_parallel = READ_PARALLEL_WITH_NO_CHECK;

          break;
        }
        case(READ_PARALLEL_WITH_NO_CHECK):
        {
          rc = nand_if->start_read( nand_if, page, data_ptr, mpages);
          BFA_NAND_VERIFY( rc == FS_DEVICE_DONE );	
          num_pages = num_pages-mpages;
		  copy_size = (mpages << trans_priv->ru_size_shift_bits);
		  
		  if(num_pages <= max_pages) 
		    read_parallel = READ_SEQUENTIAL_WITH_CHECK;
          else
            read_parallel = READ_PARALLEL_WITH_CHECK;
          break;
        }
        case(READ_PARALLEL_WITH_CHECK):
        {
          rc = nand_if->wait_for_read( nand_if);
          BFA_NAND_VERIFY( rc == FS_DEVICE_DONE );
		  rc = nand_if->start_read( nand_if, page, data_ptr, mpages);
          BFA_NAND_VERIFY( rc == FS_DEVICE_DONE );
          num_pages = num_pages-mpages;
		  copy_size = (mpages << trans_priv->ru_size_shift_bits);

		  if(num_pages <= max_pages) 
             read_parallel = READ_SEQUENTIAL_WITH_CHECK;

          break;
        }
        case(READ_SEQUENTIAL_WITH_CHECK):
        {
          rc = nand_if->wait_for_read( nand_if);
          BFA_NAND_VERIFY( rc == FS_DEVICE_DONE );
		  rc = nand_if->start_read( nand_if, page, data_ptr, mpages);
          BFA_NAND_VERIFY( rc == FS_DEVICE_DONE );	
          rc = nand_if->wait_for_read( nand_if);
          BFA_NAND_VERIFY( rc == FS_DEVICE_DONE );
    	    
          num_pages = num_pages-mpages;
		  copy_size = (mpages << trans_priv->ru_size_shift_bits);
			
          read_parallel = READ_SEQUENTIAL;
          break;
        }
        default:
          BFA_NAND_VERIFY( rc == FS_DEVICE_DONE );
          break;
      }
	}
    else
    {
	  rc = nand_if->read_mpages( nand_if, page,  num_pages, data_ptr, NULL);
      BFA_NAND_VERIFY( rc == FS_DEVICE_DONE );
	  copy_size = (num_pages << trans_priv->ru_size_shift_bits);
	  num_pages=0;
    }
    
    area_size -= copy_size;
    data_ptr += copy_size;
    phys_flash_addr += copy_size;
    *rem_size += copy_size;
    break;
  }

    /* Next, if there is a partial page left in the good flash area, we need
       to load the data into a page buffer and copy from the buffer into memory */
    if ( area_size > 0 )
    {
      copy_size = area_size;
	  
	  if (is_load_parallel)
      {
         if ((read_parallel == READ_PARALLEL_WITH_CHECK) || (read_parallel == READ_SEQUENTIAL_WITH_CHECK))
         {
	        rc = nand_if->wait_for_read( nand_if);
            BFA_NAND_VERIFY( rc == FS_DEVICE_DONE );
	        read_parallel = READ_SEQUENTIAL;
         }
      }

	  /* Copy the partial page to a temporary buffer and then to the dest */
      rc = nand_if->read_page( nand_if, page, bl_page_buf );
      qmemcpy( data_ptr, bl_page_buf, copy_size );

      data_ptr += copy_size;
      phys_flash_addr += copy_size;
	  *rem_size += copy_size;
	  break;
    }

    /* If the callback function has been set, call the callback function
       with the location and size of the area of RAM that was just read
       from flash */
    if ( trans_priv->copy_callback_func != NULL )
    {
      trans_priv->copy_callback_func( trans_priv->copy_callback_data, 
                                      callback_dest_addr, 
                                      callback_dest_size );
    }

    /* Get the next good area of flash */
  area_size = get_next_good_flash_area( trans_priv, &phys_flash_addr );
  BFA_NAND_VERIFY( area_size > 0 );

  }

  /* Reset the sloppy copy flag so that we don't do a sloppy copy next
     time unless that function is invoked explicitly */
  trans_priv->sloppy_copy = FALSE;

  return TRUE;
} /* boot_flash_trans_nand_read() */


/*=========================================================================

**  Function :  boot_flash_trans_nand_sloppy_read

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to copy a region of data from a logical byte address
*   to the destination address.  Given the logical range to copy,
*   this function converts that request to page reads and skips
*   bad blocks, while ensuring that the copy does not overwrite
*   a protected bootloader memory region. This function allows the
*   copy operation to round the start and end addresses of the region
*   to copy up or down to the nearest page boundaries for efficiency.
* 
* @par Dependencies
*   None
* 
* @retval
*   TRUE if the operation succeeded, FALSE otherwise
* 
* @par Side Effects
*   This function may adjust the start address of the copy down to the next
*   lowest page boundary, and adjust the size of the copy up to match the
*   next highest page boundary.  This means that this function may copy data
*   to RAM below the buffer pointer and beyond the limit of the buffer
*   as defined by the size parameter.  This function should not be used
*   to copy data into a fixed buffer, only to load whole images from flash
*   to RAM, where it is safe to copy whole pages from flash to RAM.
* 
*/
static boolean boot_flash_trans_nand_sloppy_read
(
  flash_trans_if_type *trans_if,  /* Pointer to the translation interface */
  void   *dest_addr,              /* Destination address */
  uint32  byte_offset,            /* Logical source byte address */
  uint32  size                    /* Size to copy in bytes */
)
{
  /* Cast the pointer to the public interface to our private type */
  boot_flash_trans_nand_type * const trans_priv =
    (boot_flash_trans_nand_type*)trans_if;
	uint32 rem_size = 0;
	boolean success = TRUE;

  /* Verify out input parameter */
  BFA_NAND_VERIFY( trans_if != NULL );

  /* Set the status variable to use a sloppy copy and then use the
     main flash copy routine */
  trans_priv->sloppy_copy = TRUE;
  
  while(rem_size < size)
  {
  success =  boot_flash_trans_nand_read( trans_if, dest_addr,
                                     byte_offset, size, &rem_size);
  }
									 
	return(success);								 
} /* boot_flash_trans_nand_sloppy_read() */
/*=========================================================================

**  Function :  boot_flash_trans_nand_write

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to copy a region of data from a destination address 
*   to a logical byte address.  
* 
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the operation succeeded, FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
static boolean boot_flash_trans_nand_write
(
  flash_trans_if_type *trans_if,  /* Pointer to the translation interface */
  void   *dest_addr,              /* Destination address */
  uint32  byte_offset,            /* Logical source byte address */
  uint32  size                    /* Size to copy in bytes */
)
{
  /* Cast the public interface to our private type */
  boot_flash_trans_nand_type * const trans_priv =
    (boot_flash_trans_nand_type*)trans_if;

  /* Get a shorter convenience pointer to the flash device interface */
  struct fs_device_data * const nand_if = trans_priv->nand_if;
  
  uint8 *data_ptr = (uint8*) dest_addr;  /* The current destination pointer */
  
  uint32 phys_flash_addr;  /* The current physical flash offset in bytes */
  uint8 *callback_dest_addr;  /* The current read pointer to pass to callback */
  uint32 callback_dest_size;  /* The current read size to pass to callback */
  uint32 area_size = 0;
  uint32 block, page_offset, page, num_pages, copy_size;  /* Block count */
  int rc;

  BFA_NAND_VERIFY( trans_if != NULL );

  /* Next translate the logical flash offset to the physical flash address */
  phys_flash_addr = get_physical_flash_addr( trans_priv, byte_offset, &area_size);
  
  /* Copy pages to flash while there is data left to copy */
  while ( size > 0 )
  {
    BFA_NAND_VERIFY( area_size > 0 );

    /* Make sure that we have not exceeded to size of the region */
    if ( trans_priv->region_size > 0 )
    {
      BFA_NAND_VERIFY( ( phys_flash_addr + size ) <= 
                       ( trans_priv->region_base_addr + 
                         trans_priv->region_size ) );
    }

    /* Save the flash offset corresponding to the current physical address,
       to save time if the next logical to physical conversion follows
       the current flash offset sequentially */
    trans_priv->last_converted_physical_addr = phys_flash_addr;
    trans_priv->last_converted_logical_offset = byte_offset;
    byte_offset += area_size;
    
    /* Adjust the good area size down if it is larger than the total copy size */
    if ( size < area_size )
    {
      area_size = size;
    }

    /* Save the RAM address and size that we are going to process in this
       iteration of the loop, and pass this to the callback function after
       we have written the pages to NAND */
    callback_dest_addr = data_ptr;
    callback_dest_size = area_size;
    
    /* Decrement the total copy size here while we have the full size of the
       good area */
    size -= area_size;

    /* Compute the page number and offset into the first page based on the 
       current physical flash address */
    page = phys_flash_addr >> trans_priv->ru_size_shift_bits;
    page_offset = phys_flash_addr - ( page << trans_priv->ru_size_shift_bits );
  
    /* Compute the block number corresponding to the physical flash address */
    block = phys_flash_addr >> trans_priv->block_size_bytes_shift_bits;

    /* Check page is erased, if not then erase the entire block before write */
    if(nand_if->is_erased( nand_if, page) != TRUE)
    {
	   rc = nand_if->erase_block( nand_if, block); 
	   BFA_NAND_VERIFY( rc == FS_DEVICE_DONE );
    }
	
    /* First, if the initial offset is not page-aligned */
    if ( page_offset > 0 )
    {
       /* Set the copy size to be the remainder of the first page */
       copy_size = trans_priv->ru_size_bytes - page_offset;
       if ( copy_size > area_size )
       {
         copy_size = area_size;
       }
	   
      /* Copy the data from dest into the partial page */
      rc = nand_if->write_page( nand_if, page, data_ptr );
	  BFA_NAND_VERIFY( rc == FS_DEVICE_DONE );
	  
      /* If all we had to copy was the first partial page, then break
         here explicitly for efficieny */
      if ( size == 0 && area_size == copy_size )
      {
        break;
      }
      area_size -= copy_size;
      data_ptr += copy_size;
      phys_flash_addr += copy_size;
      ++page;      
    }

    /* Next we write as many whole pages as are left in the current good area */
    num_pages = area_size >> trans_priv->ru_size_shift_bits;
    copy_size = num_pages << trans_priv->ru_size_shift_bits;

    /* Perform page writes for contiguous whole pages */
    while ( num_pages > 0 )
    {
      rc = nand_if->write_page( nand_if, page, data_ptr);
      BFA_NAND_VERIFY( rc == FS_DEVICE_DONE );
	  
      area_size -= copy_size;
      data_ptr += copy_size;
      phys_flash_addr += copy_size;
      page++;
      num_pages--;
    }
    
    /* Next, if there is a partial page left in the good flash area, we need
       to write the data into the partial page */
    if ( area_size > 0 )
    {
      copy_size = area_size;	
	  
      rc = nand_if->write_page( nand_if, page, data_ptr );
      BFA_NAND_VERIFY( rc == FS_DEVICE_DONE );
	  
      data_ptr += copy_size;
      phys_flash_addr += copy_size;
    }

    /* If the callback function has been set, call the callback function
       with the location and size of the area of RAM that was just written
       to the flash */
    if ( trans_priv->copy_callback_func != NULL )
    {
      trans_priv->copy_callback_func( dest_addr, 
                                      callback_dest_addr, 
                                      callback_dest_size );

      dest_addr = (uint8 *)dest_addr + callback_dest_size;
    }
	/* Get the next good area of flash */
    area_size = get_next_good_flash_area( trans_priv, &phys_flash_addr );
  }

  return TRUE;
} /* boot_flash_trans_nand_write() */

/*=========================================================================

**  Function :  boot_flash_trans_nand_set_copy_callback

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to set the callback function that will be called intermittently
*   when copying data from flash; the pointer to the last data read from flash
*   and the data size will be passed to the callback when it is called.
*   
* @par Dependencies
*   None
* 
* @retval
*   TRUE if the operation succeeded, FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
static boolean boot_flash_trans_nand_set_copy_callback
(
  boot_flash_trans_if_type *trans_if, /* Pointer to the translation interface */
  boot_flash_trans_copy_callback_func_type callback_func, /* Callback func ptr */
  void *callback_data                 /* Data to pass to callback function */
)
{
  /* Cast the pointer to the public interface to our private type */
  boot_flash_trans_nand_type * const trans_priv =
    (boot_flash_trans_nand_type*)trans_if;

  /* Verify out input parameter */
  BFA_NAND_VERIFY( trans_if != NULL );

  /* Save the callback function and data */
  trans_priv->copy_callback_func = callback_func;
  trans_priv->copy_callback_data = callback_data;

  return TRUE;
  
} /* boot_flash_trans_nand_set_copy_callback() */


/*=========================================================================

**  Function :  boot_flash_trans_nand_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to initialize an instance of the boot NAND flash translation module.
* 
* @par Dependencies
*   None
*   
* @retval
*   Pointer to the boot flash translation public interface.
* 
* @par Side Effects
*   None
* 
*/
static void boot_flash_trans_nand_init
(
  boot_flash_trans_nand_type *trans_priv,  /* Pointer to the private boot
                                              NAND flash translation structure */
  uint32 region_base_offset,               /* Base offset of the flash region */
  uint32 region_size                       /* Maximum size of the flash region */
)
{
  static const boot_module_version_type vsn =
  {
    BOOT_FLASH_TRANS_NAND_NAME,
    BOOT_MODULE_VERSION( BOOT_FLASH_TRANS_IF_VERSION,
                         BOOT_FLASH_TRANS_NAND_VERSION )
  };
  static boot_flash_trans_vtbl_type vtbl =
  {
    {
    boot_flash_trans_nand_read,
      boot_flash_trans_nand_sloppy_read,
        NULL /* get_logical_addr() not set*/
  },
    boot_flash_trans_nand_set_copy_callback,
    NULL,
    boot_flash_trans_nand_write
  };

  BL_VERIFY( trans_priv != NULL, BL_ERR_NULL_PTR_PASSED );

  /* Setup the module version structure and virtual function table */
  trans_priv->pub_if.vsn = &vsn;
  trans_priv->pub_if.vtbl = &vtbl;

  /* Cache the pointer to the NAND flash driver interface */
  trans_priv->nand_if = boot_flash_dev_nand_get_nand_if();
  BFA_NAND_VERIFY2( trans_priv->nand_if != NULL, NULL );

  trans_priv->sloppy_copy = FALSE;

  /* Cache NAND flash module variables */
  trans_priv->ru_size_bytes =
    trans_priv->nand_if->page_size( trans_priv->nand_if );
  trans_priv->ru_size_shift_bits = 
    boot_get_bit_set_position( trans_priv->ru_size_bytes );
  trans_priv->block_size_bytes_shift_bits =
    boot_get_bit_set_position( 
      trans_priv->nand_if->block_size( trans_priv->nand_if ) ) + 
    trans_priv->ru_size_shift_bits;
  trans_priv->block_count =
    trans_priv->nand_if->block_count( trans_priv->nand_if );

  /* Store the location and size of the flash region */
  trans_priv->region_base_addr = region_base_offset;
  trans_priv->region_size = region_size;

  /* Initialize the copy callback function pointer and data */
  trans_priv->copy_callback_func = NULL;
  trans_priv->copy_callback_data = NULL;

  /* Reset the flash offset optimization variables */
  trans_priv->last_converted_physical_addr = trans_priv->region_base_addr;
  trans_priv->last_converted_logical_offset = 0;

} /* boot_flash_trans_nand_init() */


/*=========================================================================

**  Function :  boot_flash_trans_nand_factory_open

** ==========================================================================
*/
/*!
* 
* @brief
*   Requests the factory to open an instance of the boot translation module.
* 
* @par Dependencies
*   None
* 
* @retval
*   Pointer to the opened boot translation interface
* 
* @par Side Effects
*   None
* 
*/
static boot_flash_trans_if_type *boot_flash_trans_nand_factory_open
(
  uint32 region_base_offset,      /* Base offset of the flash region */
  uint32 region_size              /* Maximum size of the flash region */
)
{
  int i;
  boot_flash_trans_nand_type *trans_priv = NULL;  /* Pointer to the private
                                                     data structure */

  for ( i = 0; i < BOOT_FLASH_TRANS_MAX_INSTANCES; i++ )
  {
    if ( !fact_priv.trans_priv_inuse[i] )
    {
      trans_priv = &fact_priv.trans_priv[i];
      fact_priv.trans_priv_inuse[i] = TRUE;
      break;
    }
  }
  BL_VERIFY( trans_priv != NULL, BL_ERR_RESOURCE_NOT_AVAILABLE|BL_ERROR_GROUP_BOOT );

  /* Initialize the NAND translation module */
  boot_flash_trans_nand_init( trans_priv, region_base_offset, region_size );

  return &trans_priv->pub_if;
} /* boot_flash_trans_nand_factory_open() */

/*=========================================================================
**  Function :  boot_flash_trans_nand_factory_close

** ==========================================================================
*/
/*!
* 
* @brief
*   Requests the factory to close an instance of the boot translation module.
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void boot_flash_trans_nand_factory_close
(
  boot_flash_trans_if_type **p_trans_if
)
{
  int i;

  BL_VERIFY( p_trans_if != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

  for ( i = 0; i < BOOT_FLASH_TRANS_MAX_INSTANCES; i++ )
  {
    if ( *p_trans_if == (boot_flash_trans_if_type*) &fact_priv.trans_priv[i] )
    {
      BL_VERIFY( fact_priv.trans_priv_inuse[i], BL_ERR_RESOURCE_NOT_AVAILABLE|BL_ERROR_GROUP_BOOT );
      fact_priv.trans_priv_inuse[i] = FALSE;
      *p_trans_if = NULL;
      return;
    }
  }

  /* If we have gotten here then the specified translation interface cannot be
   * found in the array */
  BL_ERR_FATAL( BL_ERR_NAND_TRANS_INTERFACE_NOT_FOUND|BL_ERROR_GROUP_BOOT );
} /* boot_flash_trans_nand_factory_close() */

/*=========================================================================

**  Function :  boot_flash_trans_nand_factory_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Initializes the NAND boot translation factory.
* 
* @par Dependencies
*   None
*   
* @retval
*   Pointer to the NAND translation factory interface
* 
* @par Side Effects
*   None
* 
*/
boot_flash_trans_factory_if_type* boot_flash_trans_nand_factory_init( void )
{
  static const boot_module_version_type vsn =
  {
    BOOT_FLASH_TRANS_NAND_FACTORY_NAME,
    BOOT_MODULE_VERSION( BOOT_FLASH_TRANS_FACTORY_IF_VERSION,
                         BOOT_FLASH_TRANS_NAND_FACTORY_VERSION )
  };
  static boot_flash_trans_factory_vtbl_type vtbl = 
  {
    boot_flash_trans_nand_factory_open,
    boot_flash_trans_nand_factory_close
  };
  static boolean init_done = FALSE;

  /* Only initialize the factory module once, if init() is called subsequently
   * it will return a pointer to the live module interface */
  if ( !init_done )
  {
    /* Setup the module version structure and virtual function table */
    fact_priv.pub_if.vsn = &vsn;
    fact_priv.pub_if.vtbl = &vtbl;

    /* Zero out the inuse marker for the private translation structure */
    memset( fact_priv.trans_priv_inuse, 0, sizeof(fact_priv.trans_priv_inuse) );

    init_done = TRUE;
  }

  /* Return pointer to the one NAND translation factory interface */
  return &fact_priv.pub_if;
} /* boot_flash_trans_nand_factory_init() */

