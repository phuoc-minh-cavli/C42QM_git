#ifndef _CSD_DEV_DEBUG_H_
#define _CSD_DEV_DEBUG_H_
/**
	@file      csd_dev_debug.h
    @brief
	Contains device debug API.
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
     Copyright (C) 2010-2012 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
	This section contains comments describing changes made to this module.
	Notice that changes are listed in reverse chronological order.
	when		who			what, where, why
	--------	-----		---------------------------------------------------
    06/04/12    sd          (Tech Pubs) Edited/added Doxygen markup and comments.
    03/15/11    sd          (Tech Pubs) Edited/added Doxygen markup and comments.
	10/25/10	jzhou		Doxygen comments update
    05/12/10   mingchen		initial draft 

	=========================================================================*/
#include "mmdefs.h"
/** @cond
*/


/** Device control IOCTL command start. */
#define CSD_DEBUG_CMD_DEV_BEGIN					0x00012000

/** IOCTL command to query active device handle. */
#define CSD_DEBUG_CMD_ACTIVE_DEV_QUERY			0x00012001

/** IOCTL commmand to query for the device information. */
#define CSD_DEBUG_CMD_ACTIVE_DEV_INFO_QUERY		0x00012002

/** IOCTL command to run the ADIE test. */
#define CSD_DEBUG_CMD_ADIE_RUN_TEST				0x00012003

/** IOCTL command to set the ADIE test sequence. */
#define CSD_DEBUG_CMD_ADIE_SET_TEST_SEQUENCE	0x00012004

/** IOCTL command to set the ADIE test profile. */
#define CSD_DEBUG_CMD_ADIE_SET_TEST_PROFILE		0x00012005

/** IOCTL command to query for the device information version 2 */
#define CSD_DEBUG_CMD_ACTIVE_DEV_INFO_QUERY_V2	0x00012006

/** Device control IOCTL command end. */
#define CSD_DEBUG_CMD_DEV_END   0x000120FF
/**
  @brief Gets the active device ID list.
*/
struct csd_debug_active_dev_query
{
   uint32_t num_devs;   /**< Returns the device ID list. */
};
/* Type definition for #csd_debug_active_dev_query_t. */
typedef struct csd_debug_active_dev_query csd_debug_active_dev_query_t;

/** @brief Device information record. */
struct csd_debug_dev_info
{
   uint32_t dev_id;          /**< Device ID. */
   uint32_t sample_rate;     /**< Device sample rate. */
   uint32_t bits_per_sample; /**< Bits per sample. */
   uint32_t port_id;         /**< AFE port ID. */
   uint32_t is_virtual_dev;  /**< Indicates whether the device is a virtual device. */
   uint32_t timing_mode;     /**< For virtual devices only: FTRT/AV timer. */
};
/* Type definition for #csd_debug_dev_info_t. */
typedef struct csd_debug_dev_info csd_debug_dev_info_t;

/**
  @brief Return structure for the device information.
*/
struct csd_debug_active_dev_info_query
{
   uint32_t num_entries;     /**< Number of information records. */

   /** Device information records. */
   struct csd_debug_dev_info* entries;
};
/* Type definition for #csd_debug_active_dev_info_query_t. */
typedef struct csd_debug_active_dev_info_query csd_debug_active_dev_info_query_t;

/** @brief Device information record v2. */
struct csd_debug_dev_info_v2
{
   uint32_t dev_id;             /**< Device ID. */
   uint32_t sample_rate;        /**< Device sample rate. */
   uint32_t bits_per_sample;    /**< Bits per sample. */
   uint32_t port_id;            /**< AFE port ID. */
   uint32_t is_virtual_dev;     /**< Indicates whether the device is a virtual device. */
   uint32_t timing_mode;        /**< For virtual devices only: FTRT/AV timer. */
   uint32_t adie_handle;        /**< ADIE handle. */   
};
/* Type definition for #csd_debug_dev_info_v2_t. */
typedef struct csd_debug_dev_info_v2 csd_debug_dev_info_v2_t;

/**
  @brief V2 API Return structure for the device information.
*/
struct csd_debug_active_dev_info_query_v2
{
   uint32_t num_entries;     /**< Number of information records. */

   /** Device information records v2. */
   struct csd_debug_dev_info_v2* entries;
};
/* Type definition for #csd_debug_active_dev_info_query_v2_t. */
typedef struct csd_debug_active_dev_info_query_v2 csd_debug_active_dev_info_query_v2_t;

/** @endcond */

#endif /* _CSD_DEV_DEBUG_H_ */

