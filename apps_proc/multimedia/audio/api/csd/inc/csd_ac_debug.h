#ifndef _CSD_AC_DEBUG_H_
#define _CSD_AC_DEBUG_H_
/**
	@file      csd_ac_debug.h
    @brief  
	Contains audio context debug API.
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

/** Audio context debug IOTCL ID start. */
#define CSD_DEBUG_CMD_AC_BEGIN		0x00012100

/** Audio context handle query. */
#define CSD_DEBUG_CMD_AC_QUERY      0x00012101

/** Audio context information query. */
#define CSD_DEBUG_CMD_AC_INFO_QUERY	0x00012102

/** Audio context and audio stream mapping query. */
#define CSD_DEBUG_CMD_AC_AS_ASSOCIATION_QUERY	0x00012103

/** Audio context debug IOCTL ID end. */
#define CSD_DEBUG_CMD_AC_END		0x000121FF

/** 
  @brief Gets the number of active audio context handles.
*/
struct csd_debug_ac_query
{
   uint32_t num_acs;  /**< Number of active audio contexts. */
};
/* Type definition for #csd_debug_ac_query_t. */
typedef struct csd_debug_ac_query csd_debug_ac_query_t;

/** @brief Each audio context record. */
struct csd_debug_ac_info
{
   uint32_t dev_id;       /**< Device ID. */
   uint32_t ac_handle;    /**< Audio context handle. */
   uint32_t copp_id;      /**< Common object pre/postprocessing ID. */
   uint32_t ac_category;  /**< Audio context category (Default: 0). */
   uint32_t ac_mode;      /**< Indicates whether the audio context is to drop 
                               the sample. */
   uint32_t num_as_handles; /**< Number of audio stream handles. */
};
/* Type definition for #csd_debug_ac_info_t.  */
typedef struct csd_debug_ac_info csd_debug_ac_info_t;

/**
  @brief Return strucutre for the audio context information.
*/
struct csd_debug_active_ac_info_query
{
   uint32_t num_entries;				/**< Number of entries. */

   /** Audio context information list. */
   struct csd_debug_ac_info* entries;	
};
/* Type definition for #csd_debug_active_ac_info_query_t. */
typedef struct csd_debug_active_ac_info_query csd_debug_active_ac_info_query_t;

/**
  @brief Return structure for the audio context and audio stream mapping.
*/
struct csd_debug_as_ac_association_query
{
   uint32_t ac_handle;              /**< Audio context handle input. */
   uint32_t num_as_handles;         /**< Number of audio stream handles. */
   uint32_t* as_handles;            /**< Audio stream handle list. */
};
/* Type definition for #csd_debug_as_ac_association_query_t. */
typedef struct csd_debug_as_ac_association_query csd_debug_as_ac_association_query_t;

/** @endcond */

#endif /* _CSD_AC_DEBUG_H_ */

