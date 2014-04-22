#ifndef _CSD_AC_H_
#define _CSD_AC_H_
/**
    @file      csd_ac.h
    @brief  
    Contains audio context API definitions
 
*/
/*-----------------------------------------------------------------------------
     Copyright (C) 2010-2012 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------------*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The CSD_mainpage.dox file contains all file/group descriptions that are 
      in the output PDF generated using Doxygen and LaTeX. To edit or update 
      any of the file/group text in the PDF, edit the CSD_mainpage.dox file or 
      contact Tech Pubs.
===========================================================================*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
    This section contains comments describing changes made to this module.
    Notice that changes are listed in reverse chronological order.
    when		who			what, where, why
    --------	-----		---------------------------------------------------
    01/18/13    suns        Add low latency and 24 bits Process Pipeline support.
    06/04/12    sd          (Tech Pubs) Edited/added Doxygen markup and comments.
    03/15/11    sd          (Tech Pubs) Edited/added Doxygen markup and comments.
    10/25/10	jzhou		Doxygen comments update
    08/17/10	jzhou		Doxygen comments update
    05/13/10	mingchen	initial draft 

    =========================================================================*/
#include "mmdefs.h"
#include "csd_ac_ioctl.h"

/** @addtogroup csd_ac
  @{
*/

/** 
    @name Audio Context Categories
    @{
*/ 
#define CSD_AC_CATEGORY_GENERIC_PLAYBACK      1	/**< Playback category. */
#define CSD_AC_CATEGORY_GENERIC_RECORDING     2	/**< Recording category. */
#define CSD_AC_CATEGORY_SYSTEM_SOUND          3	/**< System sound category. */
#define CSD_AC_CATEGORY_VOICE_RECOGNITION     4	/**< Voice recognition category. */
/** @} */ /* end_name Audio Context Categories */
/** 
    @name Audio Context Mode
    @{ 
*/ 
#define CSD_AC_MODE_NON_LIVE 0 /**< Non-live mode sample buffered in the AFE. */
#define CSD_AC_MODE_LIVE     1 /**< Live mode. The sample is not buffered if 
                                    the polling is not fast enough. */
/** @} */ /* end_name Audio Context Mode */

/**
  @brief Opens the audio context.
*/
struct csd_ac_open {
    uint32_t ac_category;  /**<	Audio context category. Supported values: \n
                                - 1 -- Playback category. 
                                - 2	-- Recording category. 
                                - 3	-- System sound category. 
                                - 4	-- Voice recognition category. */
    uint32_t ac_mode;      /**< Running mode for the audio context. Live mode
                                drops the data buffer. Non-live mode blocks if
                                there is no output buffer. Supported values: \n
                                - 0 -- Non-live mode sample buffered in the AFE. 
                                - 1 -- Live mode. The sample is not buffered if 
                                       the polling is not fast enough. */

    uint32_t dev_id;       /**< Device ID. Supported values are OEM-defined. */
    uint32_t sample_rate;  /**< Sample rate for the audio context. Supported
                                values: 8000, 16000, and 48000. */
};
/* Type definition for #csd_ac_open_t. */
typedef struct csd_ac_open csd_ac_open_t;

/** 
    @name Audio Context Common Post/Pre Process Pipeline Bit Width 
    @{ 
*/ 
#define CSD_AC_OPEN_PP_BITWIDTH_16       16    /**< 16 bits Post/Pre Processing Pipeline.*/
#define CSD_AC_OPEN_PP_BITWIDTH_24       24    /**< 24 bits Post/Pre Processing Pipeline.*/
/** @} */ /* end_name Audio Context Post/Pre Process Pipeline Bit Width. */
/** 
   @name Audio Context Open Mask V2
   @{ 
*/ 
#define CSD_AC_OPEN_MASK_LOW_LATENCY_MODE    0x00000001 
/**< Audio Context Low Latency Mode Mask. The session will be opened in low latency mode */
/** @} */ /* end_name Audio Context Open Mask */
/**
  @brief Opens the Audio Context V2.
*/
struct csd_ac_open_v2 {
    uint32_t ac_category;  /**<	Audio context category. Supported values: \n
                                - 1 -- Playback category. 
                                - 2	-- Recording category. 
                                - 3	-- System sound category. 
                                - 4	-- Voice recognition category. */
    uint32_t ac_mode;      /**< Running mode for the audio context. Live mode
                                drops the data buffer. Non-live mode blocks if
                                there is no output buffer. Supported values: \n
                                - 0 -- Non-live mode sample buffered in the AFE. 
                                - 1 -- Live mode. The sample is not buffered if 
                                       the polling is not fast enough. */

    uint32_t dev_id;       /**< Device ID. Supported values are OEM-defined. */
    uint32_t sample_rate;  /**< Sample rate for the audio context. Supported
                                values: 8000, 16000, and 48000. */
    uint32_t open_mask;    /**< Indicates whether the optional fields are valid.
                                Supported values:\n
                                     - 0 -- No Mask setting is applied. . 
                                     - 1 -- Mask setting is applied. 
                                            - Bit 0 -- Low Latency Mode Mask. 
                                            - Bit 1 to 31 -- Reserved = 0. */
    uint32_t bit_width;    /**< Bit width of the bit stream on the COPP pipeline. 
                                Supported values: 16 or 24. */
    uint32_t ret_status;   /**< Returns the Open status. */
};
/* Type definition for #csd_ac_open_t. */
typedef struct csd_ac_open_v2 csd_ac_open_v2_t;

/** @} */ /* end_addtogroup csd_ac */

#endif /* _CSD_AC_H_*/
