#ifndef __CSD_H__
#define __CSD_H__
/**
    @file   csd.h
    @brief  
  Contains Core Sound Driver's API. These APIs are public to any client that
  wants to set the audio features on Qualcomm MSM
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
  when        who         what, where, why
  --------    -----       ---------------------------------------------------
  08/22/14    jy          Increment CSD_API_VER_MINOR to 0x12 for adding API
                          CSD_VM_IOCTL_SET_HDVOICE_MODE 
  05/30/14    rp          Increment VER_REV from 0x01 to 0x02 for adding Topology
                          member in the Calibration Feature ID.
  01/28/14    dc          Increment CSD API version to 01.17.00 for addition of
                          Audio+ post-processing topologies (Virtualizer, Bass Boost
                          and Reverb).						  
  01/02/14    ts          Increment CSD API version to 01.16.00 for addition of
                          setting feature set ID used for voice calibration 
                          on a device pair.
  08/16/13    jy          Increment VER_REV from 0x00 to 0x01 for comment update
                          on CSD_VC_IOCTL_SET_DEVICE_CONFIG and 
                          CSD_VC_IOCTL_SET_DEVICE_CONFIG_V2
  05/29/13    peterlee    Increment CSD API version to 01.15.00 
                          for HDMI Auto Lipsync Correction.
  05/15/13    ac          Increment CSD API version to 01.14.00 for addition of:
                          1. CSD_DEV_CMD_SET_EVT_CB API for device event callback.
						  2. Rename csd_dev_mbhc.h to csd_dev_mbhc_ioctl.h.
  05/07/13	  dc		  Increment CSD API version to 01.13.00 for addition of:
						  1. Adding return code in CSD AS/AC V2 API. 
						  2. Skipping initial and trailiing silence samples for 
							 audio playback. 
  04/19/13    re          Increment the CSD API versoion ID to 01.12.00 for addition of 
                          set VC volume API. 
  04/10/13    dc          Increment CSD API version ID to 01.11.00
                          for MP2 Playback and Digital Volume Control API addition.
  04/05/13    jy          Increment CSD API version ID to 01.10.00
                                for ADPCM recoding API addition
  12/23/12    jk          Increment CSD API version ID to 01.09.00. 
                                for CSD_VM_IOCTL_PAUSE_VOICE API addition
  01/18/13    suns        Add new CSD Audio Stream and Audio Context open type. 
  12/20/12    suns        Increment CSD API version ID to 01.07.00.
  09/11/12    suns        Increment CSD API version ID to 01.05.00.
  06/04/12    sd          (Tech Pubs) Edited/added Doxygen markup and comments.
  03/15/11    sd          (Tech Pubs) Edited/added Doxygen markup and comments.
  10/21/10    jzhou        Doxygen comments update
  08/17/10    jzhou        Doxygen comments update
  06/01/10    jzhou        Initial Creation

  =========================================================================*/

#include "mmdefs.h"  
#include "csd_status.h"

/** @addtogroup csd_api
  @{
*/

/** Major version of the CSD API. */ 
#define CSD_API_VER_MAIN  0x01
/** Minor version of the CSD API. */
#define CSD_API_VER_MINOR 0x12
/** Minor revision of the CSD API. */
#define CSD_API_VER_REV   0x02
/** Macro to get the major version number of the CSD API. */
#define CSD_VER_GET_MAIN(x) ((x&0xFF000000)>>24);
/** Macro to get the minor version number of the CSD API. */
#define CSD_VER_GET_MINOR(x)((x&0xFF0000)>>16);
/** Macro to get the minor revision number of the CSD API. */
#define CSD_VER_GET_REV(x)  (x&0xFF);

/**
  Returns the CSD version number in uint32_t number.

  @return
  CSD version number.

  @dependencies
  None.
*/
uint32_t csd_query_driver_version(void);


/**
  @latexonly \label{csdInit} @endlatexonly

  Initializes the CSD. The second call adds the reference 
  count only.

  This function can be called by multiple CSD clients at the same time.

  @return 
  CSD status code; see Section @latexonly \ref{sec:csdStatusCodes}@endlatexonly 
  for details.

  @dependencies
  This function must be called before the CSD can provide any services.
*/
uint32_t csd_init(void);

/** 
  @latexonly \label{csdDeinit} @endlatexonly

  Detaches the client from the CSD. An internal reference count is used to 
  track how many CSD clients are attached to the CSD. The CSD releases all the 
  resources only when the reference count reaches zero. 
    
  @return 
  CSD status code; see Section @latexonly \ref{sec:csdStatusCodes}@endlatexonly 
  for details. 
    
  @dependencies 
  After this function call, the client may not call any CSD functions except 
  csd_init().  
*/
uint32_t csd_deinit(void);

/**
  Defines the open type for the csd_open() function. 
*/
enum csd_open_code
{
    CSD_OPEN_NOT_USED                    = 0, 
      /**< Not used. */
    CSD_OPEN_AUDIO_STREAM                = 1, 
      /**< Instructs the CSD to return an audio stream handle. */
    CSD_OPEN_VOICE_STREAM                = 2, 
      /**< Instructs the CSD to return a voice stream handle. */
    CSD_OPEN_AUDIO_CONTEXT               = 3, 
      /**< Instructs the CSD to return an audio context handle. */
    CSD_OPEN_VOICE_CONTEXT               = 4, 
      /**< Instructs the CSD to return a voice context handle.  */
    CSD_OPEN_DEVICE_CONTROL              = 5, 
      /**< Instructs the CSD to return a device control handle.  */  
    CSD_OPEN_DEBUG_DEVICE_CONTROL        = 6, 
      /**< Device control for debug/real-time calibration purposes. */
    CSD_OPEN_DEBUG_AUDIO_STREAM_CONTROL  = 7, 
      /**< Audio stream control for debug/real-time calibration purposes. */
    CSD_OPEN_DEBUG_AUDIO_CONTEXT_CONTROL = 8, 
      /**< Audio context control for debug/real-time calibration purposes. */
    CSD_OPEN_DEBUG_VOICE_CONTROL     = 9, 
      /**< Voice context control for debug/real-time calibration purposes. */
    CSD_OPEN_VOICE_MANAGER               = 10,  
      /**< Instructs the CSD to open the voice manager. */    
    CSD_OPEN_AUDIO_LICENSE_MANAGER       = 11,
      /**< Instructs the CSD to open the audio license manager. */
    CSD_OPEN_AUDIO_STREAM_V2             = 12,  
      /**< Instructs the CSD to return an enhanced audio stream handle with 
           low latncy and programmable processing pipeline bitwidth support. */
    CSD_OPEN_AUDIO_CONTEXT_V2            = 13,  
      /**< Instructs the CSD to return an enhanced audio context handle with 
           low latncy and programmable processing pipeline bitwidth support. */
    CSD_OPEN_MAX_NUM              
      /**< Maximum number. */
};

/**
  @latexonly \label{csdOpen} @endlatexonly

  Opens a handle for the client to communicate with the CSD. The csd_open() 
  function is a synchronous function.

  @datatypes
  #csd_open_code

  @param[in]  code        Open code.
  @param[in]  params      Open structure based on the open code.
  @param[in]  size        Size of the open structure.
    
  @return
  A positive number is the CSD handle.\n
  0 -- Invalid handle or other error.
    
  @dependencies
  None.

  @structures
  #csd_as_open
  #csd_vs_open_t
  #csd_ac_open
  #csd_vc_open_full_control_t
*/
uint32_t csd_open(enum csd_open_code code, void* params, uint32_t size);

/**
  @latexonly \label{csdClose} @endlatexonly

  Closes the CSD stream or session using the handle.
    
  @param[in] handle Handle to be closed.
    
  @return 
  CSD status code; see Section @latexonly \ref{sec:csdStatusCodes}@endlatexonly 
  for details.

  @dependencies
  None.
*/
uint32_t csd_close(uint32_t handle);

/**
  @latexonly \label{csdWrite} @endlatexonly

  Sends the data buffer to the CSD for rendering.

  @param[in] handle  Handle for the playback stream.
  @param[in] params  Payload for csd_write().
  @param[in] size    Size of the payload for csd_write().
     
  @return
  A positive number is the byte count of the encoded data that has been 
  consumed by the CSD. \n
  0 -- No free buffer is available. This is for a heap memory type only.\n 
  -1 -- Error occurred in sending the buffer.

  @dependencies
  None.
*/
int32_t csd_write(uint32_t handle, void* params, uint32_t size);

/**
  @latexonly \label{csdRead} @endlatexonly

  Retrieves the recording data from the CSD in a heap memory type case. When 
  the PMEM memory type is used, this function passes the free buffer to the DSP.

  @param[in]    handle    Handle for the recording stream.
  @param[in,out]  params    Payload for csd_read().  
  @param[in]    size    Size of the payload for csd_read().

  @return
  A positive number is the byte count for the data received. This return value 
  is for heap memory only.\n
  0 -- Depending on the memory type:\n
       - For heap, no buffer is available. 
       - For PMEM, successful push of the data buffer.
  @return
  -1 -- Error occurred. This is for the PMEM memory type only.

  @dependencies
  None.
*/
int32_t csd_read(uint32_t handle, void* params, uint32_t size);
/**
  @latexonly \label{csdIoctl} @endlatexonly

  Issues a control command from the client to the CSD for a specific 
  handle.

  @param[in] handle      Handle for the CSD.
  @param[in] cmd         Control command ID for the CSD handle.
  @param[in,out] params  Payload for the control command.
  @param[in] size        Size of the payload for the control command.

  @return 
  CSD status code; see Section @latexonly \ref{sec:csdStatusCodes}@endlatexonly 
  for details.

  @dependencies
  None.
*/
uint32_t csd_ioctl(uint32_t handle, uint32_t cmd, void* params, uint32_t size);

void csd_pmic_gpiotwo_on(void);

void csd_pmic_gpiotwo_off(void);

/** @} */  /* end_addtogroup csd_api */

#endif /* __CSD_H__ */
