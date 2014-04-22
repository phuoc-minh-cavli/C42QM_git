#ifndef _CSD_AS_DEBUG_H_
#define _CSD_AS_DEBUG_H_
/**
	@file      csd_as_debug.h
    @brief
	Contains as debug/real-time calibration ioctls
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

/** Audio stream debug IOCTL start. */
#define CSD_DEBUG_CMD_AS_BEGIN			0x00012200
/** Audio stream information query. */
#define CSD_DEBUG_CMD_AS_INFO_QUERY		0x00012201
/** Audio stream debug IOCTL end. */
#define CSD_DEBUG_CMD_AS_END		    0x000122FF

/**	@brief Audio stream information record. */
struct csd_debug_as_info {
   uint32_t    session_id; /**< Session ID. */
   uint32_t    stream_id;  /**< Audio stream ID. */
   uint32_t    op_code;    /**< Operations code. */
   uint32_t    app_type;   /**< Application type. */
   uint32_t    rx_type;    /**< Playback application type. */
   uint32_t    tx_type;    /**< Recording application type. */
   uint32_t    topology_id;/**< POPP topology ID. */
};
/* Type definition for #csd_debug_as_info_t. */
typedef struct csd_debug_as_info csd_debug_as_info_t;

/**
  @brief Return structure for an audio stream information query. 
*/
struct csd_debug_as_query
{
   uint32_t as_handle;               /**< Audio stream handle. */

   /** Audio stream information. */
   struct csd_debug_as_info as_info; 
};
/* Type definition for #csd_debug_as_query_t. */
typedef struct csd_debug_as_query csd_debug_as_query_t;

/** @endcond */

#endif /* _CSD_AS_DEBUG_H_ */

