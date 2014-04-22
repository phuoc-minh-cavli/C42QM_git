/*=============================================================================

                              Boot Loader

GENERAL DESCRIPTION
  This module performs binary image and MPSS loading.

Copyright 2017-2018, 2019 by Qualcomm Technologies, Inc. All Rights Reserved.
=============================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/0.2/QcomPkg/XBLLoader/boot_fota_interface.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/19   aus       Updated flash API
12/04/18   rohik     Initial revision.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <string.h>
#include "boot_fota_interface.h"
#include "boot_sbl_if.h"
#include "boot_logger.h"
#include "boot_flash_dev_if.h"
#include "boot_target.h"
#include "boot_shared_imem_cookie.h"

static uint8 fota_cookie[LAST_IMAGE_INDEX] ;

static boolean fota_cookie_write  = FALSE;
static uint8 imem_fota_info[LAST_IMAGE_INDEX] ;
extern uint8 fota_cookie_partition_name[PARTITION_ID_MAX_SIZE];


/*===========================================================================
**  Function :  boot_init_fota_cookie
** ==========================================================================
*/
/*!
* 
* @brief
*  This function initialize the ram buffer of fota cookie
*
* @param[in] - none
*
* 
*/

void boot_init_fota_cookie ()
{
	boot_flash_trans_if_type *trans_if;      /* translation interface */
    boolean success = FALSE;
	static boolean fota_init = FALSE;
	uint32 rem_size = 0;
     
   do
   {
      /* If we did the check before, we do not want to re-open the partition and 
       read it again. Just use the value from the last read */
      if(fota_init == TRUE)
        break;

      /* Configure the target image */
      boot_flash_configure_target_image(fota_cookie_partition_name);
    
      /* Initialize flash translation interface to read backup partition */
      trans_if = boot_flash_dev_open_image(GEN_IMG);
   
      /* If partition not found, boot normally */
      if(trans_if == NULL)   
      {
        success = FALSE;
        break;
      }
	  
	  boot_set_entry_as_img_whitelist((uintnt)fota_cookie , (uintnt)sizeof(fota_cookie));
	  
      while(rem_size < sizeof(fota_cookie))
	{	
      /* Read the recovery cookie from the base of the partition */
      success = boot_flash_trans_read( trans_if, 
                                       fota_cookie,
                                       FOTA_COOKIE_OFFSET /* offset */, 
                                       sizeof(fota_cookie),
									   IMAGE_BODY_TYPE, &rem_size);   

      /* Remove the local hole */
     // boot_clobber_remove_local_hole( boot_flash_trans_get_clobber_tbl_ptr( trans_if ),
       //                               fota_cookie, sizeof(fota_cookie) ); 

      BL_VERIFY( success, BL_ERR_OTHERS );
	}
	  
	  boot_invalidate_img_whitelist();

      /* close device handle associated with image_id */
      boot_flash_dev_close_file( GEN_IMG );

      /* Release the translation layer resource */
      boot_flash_dev_close_image( &trans_if );
       
    } while(0);

   fota_init = TRUE;
  
}



/*===========================================================================
**  Function :  boot_check_fota_partition_cookie
** ==========================================================================
*/
/*!
* 
* @brief
*   This function opens fota cookie partition and checks if we need to boot image from   
*   fota partitions
*
* @par Image index (APPS/MPSS/etc)
*   None
* 
* @retval
*   boolean - TRUE - fota mode
*             FALSE - normal mode
* 
* @par Side Effects
*   None
* 
*/
boolean boot_check_fota_partition_cookie ( uint32 image_index )
{
   
   if( fota_cookie[image_index] == FOTA_COOKIE_VAL)
   {   
        fota_cookie_write = TRUE; 
		return TRUE; 
   }  
   else  
   {	   
 	return FALSE;
   }	
}

/*===========================================================================
**  Function :  boot_clear_image_fota_cookie
** ==========================================================================
*/
/*!
* 
* @brief
*  This function  clear the the ram buffer of fota cookie for the image
*
* @param[in] - image index is the index of image in fota cookie buffer
*
* 
*/
void boot_clear_image_fota_cookie(uint32 image_index)
{
	fota_cookie[image_index] = 0x0;
}

/*===========================================================================
**  Function :  boot_clear_fota_cookie
** ==========================================================================
*/
/*!
* 
* @brief
*  This function  clear the the ram buffer of fota cookie 
*
* @param[in] - 
*
* 
*/
void boot_clear_fota_cookie(void)
{
	memset(fota_cookie,0x0,sizeof(fota_cookie));	
}

/*===========================================================================
**  Function :  boot_set_fota_partition_cookie
** ==========================================================================
*/
/*!
* 
* @brief
*  This function set the fota partition cookue on the flash 
*
* @param[in] - none
* @retval
*   boolean - TRUE - recovery mode
*             FALSE - normal mode
* 
*/
boolean boot_set_fota_partition_cookie ( void )
{
   boot_flash_trans_if_type *trans_if;      /* translation interface */
   boolean success = FALSE;
  
   if(fota_cookie_write ==FALSE)
   {
	     return FALSE;
   }	   

   do
   {	  
      /* Configure the target image */
      boot_flash_configure_target_image(fota_cookie_partition_name);
    
      /* Initialize flash translation interface to write backup partition */
      trans_if = boot_flash_dev_open_image(GEN_IMG);
   
      /* If partition not found, boot normally */
      if(trans_if == NULL)   
      {
        success = FALSE;
        break;
      }	
	  
      /* Write the recovery cookie to the base of the partition */
      success = boot_flash_trans_write( trans_if, 
                                       fota_cookie,
                                       FOTA_COOKIE_OFFSET /* offset */, 
                                       sizeof(fota_cookie) );  


      BL_VERIFY( success, BL_ERR_OTHERS );									   
	  
      /* close device handle associated with image_id */
      boot_flash_dev_close_file( GEN_IMG );

      /* Release the translation layer resource */
      boot_flash_dev_close_image( &trans_if );

    } while(0);

   return success;	
}


/*===========================================================================
**  Function :  boot_update_imem_fota_info
** ==========================================================================
*/
/*!
* 
* @brief
*  This function update the imem information of boot partition of image
*
* @param[in] 
*  uint32 image_index  - image_index offset of the image in imem fota info 
*  uint32 bootType   -   boot from normal or fota partition
*
* 
* 
*/
void boot_update_imem_fota_info ( uint32 image_index , uint32 boot_type)
{
		imem_fota_info[image_index] = boot_type;  
}
/*===========================================================================
**  Function :  boot_set_imem_fota_info
** ==========================================================================
*/
/*!
* 
* @brief
*  This function set the imem information of boot 
*
* @param[in] -
*
* 
*/

boolean boot_set_imem_fota_info( void )
{
	memcpy(boot_shared_imem_cookie_ptr->fota_info_cookie,imem_fota_info,sizeof(imem_fota_info));
    return TRUE;	
}										   
	
