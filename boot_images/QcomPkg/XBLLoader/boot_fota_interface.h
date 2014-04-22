#ifndef BOOT_FOTA_INTERFACE_H
#define BOOT_FOTA_INTERFACE_H

/*=============================================================================

                              Boot Loader

GENERAL DESCRIPTION
  This module performs binary image and MPSS loading.

Copyright 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/24/17   sj     Initial revision.
============================================================================*/

/*===========================================================================

                           INCLUDE FILES
						   
===========================================================================*/
#include "boot_comdef.h"

/*=============================================================================

                              DEFINITIONS

=============================================================================*/
/* Cookie that define download from FOTA partition */

#define FOTA_COOKIE_VAL 0xAA
#define FOTA_COOKIE_OFFSET 0x0

enum 
{
	BOOT_FROM_MAIN_PARTITION =1,
	BOOT_FROM_FOTA_PARTITION,
	BOOT_CANCEL_LOAD
};

enum
{
	APPS_IMAGE_INDEX,
	MPSS_IMAGE_INDEX,
	//all more image before it
    LAST_IMAGE_INDEX	
};



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

void boot_update_imem_fota_info ( uint32 image_index , uint32 bootType);

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

boolean boot_set_imem_fota_info( void );

/*===========================================================================
**  Function :  boot_set_fota_partition_cookie
** ==========================================================================
*/
/*!
* 
* @brief
*  This function set the fota partition cookue on the flash 
*
* @param[in] -
*
* 
*/

boolean boot_set_fota_partition_cookie ( void );

/*===========================================================================
**  Function :  boot_init_fota_cookie
** ==========================================================================
*/
/*!
* 
* @brief
*  This function initialize the ram buffer of fota cookie
*
* @param[in] -
*
* 
*/

void boot_init_fota_cookie ( void);

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
void boot_clear_image_fota_cookie (uint32 image_index);


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
void boot_clear_fota_cookie(void);


/*===========================================================================
**  Function :  boot_check_fota_partition_cookie
** ==========================================================================
*/
/*!
* 
* @brief
*  This function  check whether fota cookie is set for a image
*
* @param[in] - image_index is the image offset in the fota cookie
*
* 
*/
boolean boot_check_fota_partition_cookie (uint32 image_index);

#endif /* BOOT_LOADER_H */

