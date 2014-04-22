#ifndef __CSD_VOICE_DEBUG_H__
#define __CSD_VOICE_DEBUG_H__

/**
    @file  csd_voice_debug.h
    @brief Contains voice debug API definitions.
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
     Copyright (C) 2011-12 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
    This section contains comments describing changes made to this module.
    Notice that changes are listed in reverse chronological order.

    $Header: //components/rel/audio.tx/2.0/audio/api/csd/inc/csd_voice_debug.h#1 $
    $DateTime: 2018/12/25 22:39:07 $
    $Author: pwbldsvc $
    $Change: 17929171 $
    $Revision: #1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/04/12   sd      (Tech Pubs) Edited/added Doxygen markup and comments.
03/15/11   sd      (Tech Pubs) Edited/added Doxygen markup and comments.
=============================================================================*/

#include "mmdefs.h"

/** @cond 
*/

/**
  @brief Payload for #CSD_OPEN_DEBUG_VOICE_CONTROL.
*/
struct csd_debug_voice_open_control_t
{
  uint32_t* ret_status;
    /**< Returns the Open status. */
};

/* Type definition for #csd_debug_voice_open_control_t. */
typedef struct csd_debug_voice_open_control_t csd_debug_voice_open_control_t;

/** @endcond */

#endif /* __CSD_VOICE_DEBUG_H__ */
