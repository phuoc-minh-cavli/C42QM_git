#ifndef _CSD_ALM_IOCTL_H_
#define _CSD_ALM_IOCTL_H_
/**
	@file      csd_alm_ioctl.h
    @brief     Contains audio license manager IOCTL definitions
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The CSD_mainpage.dox file contains all file/group descriptions that are 
      in the output PDF generated using Doxygen and LaTeX. To edit or update 
      any of the file/group text in the PDF, edit the CSD_mainpage.dox file or 
      contact Tech Pubs.
===========================================================================*/
/*-----------------------------------------------------------------------------
     Copyright (C) 2012 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------------*/
/*===========================================================================
                      EDIT HISTORY FOR MODULE

    This section contains comments describing changes made to this module.
    Notice that changes are listed in reverse chronological order.

    $Header: //components/rel/audio.tx/2.0/audio/api/csd/inc/csd_alm_ioctl.h#1 $
    $DateTime: 2018/12/25 22:39:07 $
    $Author: pwbldsvc $
    $Change: 17929171 $
    $Revision: #1 $

	when		who			what, where, why
	--------	-----		---------------------------------------------------
    06/04/12    sd          (Tech Pubs) Edited/added Doxygen markup and comments.

============================================================================*/

/** @addtogroup csd_alm
  @{
*/

/** 
   @name Audio License Manager IOCTL Commands
   @{ 
 */ 

/** Begins the audio license manager IOCTL commands. */
#define CSD_ALM_ID_START                0x000113C0

/** Loads the dolby license file. 

  @payload
  #csd_alm_dolby_license

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_ALM_CMD_SET_DOLBY_LICENSE   0x000113C0

/** Ends of the audio license manager IOCTL commands. */  
#define CSD_ALM_ID_END                  0x000113FF
  
/** @} */ /* end_name Audio License Manager IOCTL Commands */

/**
  @brief Payload for #CSD_ALM_CMD_SET_DOLBY_LICENSE.
*/
struct csd_alm_dolby_license {
    uint32_t    num_id_parameters;  /**< Number of ID parameters. 
                                         Only one is currently supported. */
    uint32_t    *id_parameters;     /**< Array of ID parameters. */
    uint32_t    size_license_buf;   /**< License data size in bytes. */
    uint8_t     *license_buf;       /**< Pointer to license data. */
};
/* Type definition for #csd_alm_dolby_license_t. */ 
typedef struct csd_alm_dolby_license csd_alm_dolby_license_t;

/** @} */ /* end_addtogroup csd_alm */

#endif /* _CSD_ALM_IOCTL_H_*/
