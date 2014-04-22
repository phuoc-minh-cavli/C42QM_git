#ifndef __CSD_VCOMMON_H__
#define __CSD_VCOMMON_H__
/**
  @file  csd_vcommon.h
  @brief Contains voice common definitions.
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
     Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
    This section contains comments describing changes made to this module.
    Notice that changes are listed in reverse chronological order.

    $Header: //components/rel/audio.tx/2.0/audio/api/csd/inc/csd_vcommon.h#1 $
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

/** @addtogroup csd_vcommon
@{ */

/****************************************************************************
 * CSD VOICE INTERNAL/EXTERNAL MACRO DEFINITIONS                            *
 ****************************************************************************/
/** @cond
*/
/* 
    @name CSD Voice Internal/External MACRO Definitions
    @{
*/

/*
  The internal MACRO indicates that an API is not intended for 
  public use. 
*/
#undef CSD_INTERNAL
#ifdef __cplusplus
  #define CSD_INTERNAL extern "C"
#else
  #define CSD_INTERNAL extern
#endif /* __cplusplus */

/*
  The external MACRO indicates that an API is intended for public use.
*/
#undef CSD_EXTERNAL
#ifdef __cplusplus
  #define CSD_EXTERNAL extern "C"
#else
  #define CSD_EXTERNAL extern
#endif /* __cplusplus */

/* @} */ /* end_name_group CSD Voice Internal/External MACRO Definitions */
/** @endcond */

/****************************************************************************
 * CSD VOICE STATE DEFINITIONS                                              *
 ****************************************************************************/

/** 
  @name CSD Voice State Definitions
  @{
*/

/** Indicates the Ready state. */
#define CSD_VOC_STATEID_READY ( 0x00011166 )

/** Indicates the Not Ready state. */
#define CSD_VOC_STATEID_NOT_READY ( 0x00011167 )

/** @} */ /* end_name_group CSD Voice State Definitions */

/****************************************************************************
 * CSD VOICE NETWORK DEFINITIONS                                            *
 ****************************************************************************/

/** 
  @name CSD Voice Network Definitions
  @latexonly \label{network_ID} @endlatexonly 
  @{
*/

/** Default network. */
#define CSD_NETWORK_ID_DEFAULT ( 0x00010037 )

/** CDMA narrowband network. */
#define CSD_NETWORK_ID_CDMA_NB ( 0x00010021 )

/** CDMA wideband network. */
#define CSD_NETWORK_ID_CDMA_WB ( 0x00010022 )

/** CDMA WideVoice network. */
#define CSD_NETWORK_ID_CDMA_WV ( 0x00011100 )

/** GSM narrowband network. */
#define CSD_NETWORK_ID_GSM_NB ( 0x00010023 )

/** GSM wideband network. */
#define CSD_NETWORK_ID_GSM_WB ( 0x00010024 )

/** GSM WideVoice network. */
#define CSD_NETWORK_ID_GSM_WV ( 0x00011101 )

/** WCDMA narrowband network. */
#define CSD_NETWORK_ID_WCDMA_NB ( 0x00010025 )

/** WCDMA wideband network. */
#define CSD_NETWORK_ID_WCDMA_WB ( 0x00010026 )

/** WCDMA WideVoice network. */
#define CSD_NETWORK_ID_WCDMA_WV ( 0x00011102 )

/** VoIP narrowband network. */
#define CSD_NETWORK_ID_VOIP_NB ( 0x00011240 )

/** VoIP wideband network. */
#define CSD_NETWORK_ID_VOIP_WB ( 0x00011241 )

/** VoIP WideVoice network. */
#define CSD_NETWORK_ID_VOIP_WV ( 0x00011242 )

/** @} */ /* end_name_group CSD Voice Network Definitions */

/****************************************************************************
 * CSD VOICE MEDIA TYPE DEFINITIONS                                         *
 ****************************************************************************/

/** 
  @name CSD Voice Media Type Definitions 
  @{
*/

/** No media type. */
#define CSD_MEDIA_ID_NONE ( 0x00010FC0 )

/** CDMA variable 13K vocoder modem format. */
#define CSD_MEDIA_ID_13K_MODEM ( 0x00010FC1 )

/** CDMA enhanced variable rate vocoder modem format. */
#define CSD_MEDIA_ID_EVRC_MODEM ( 0x00010FC2 )

/** CDMA fourth-generation narrowband vocoder modem format. */
#define CSD_MEDIA_ID_4GV_NB_MODEM ( 0x00010FC3 )

/** CDMA fourth-generation wideband vocoder modem format. */
#define CSD_MEDIA_ID_4GV_WB_MODEM ( 0x00010FC4 )

/** CDMA fourth-generation narrow-wide vocoder modem format. */
#define CSD_MEDIA_ID_4GV_NW_MODEM ( 0x00010FC5 )

/** UMTS adaptive multirate narrowband vocoder modem format. */
#define CSD_MEDIA_ID_AMR_NB_MODEM ( 0x00010FC6 )

/** UMTS adaptive multirate wideband vocoder modem format. */
#define CSD_MEDIA_ID_AMR_WB_MODEM ( 0x00010FC7 )

/** GSM enhanced full-rate vocoder modem format. */
#define CSD_MEDIA_ID_EFR_MODEM ( 0x00010FC8 )

/** GSM full-rate vocoder modem format. */
#define CSD_MEDIA_ID_FR_MODEM ( 0x00010FC9 )

/** GSM half-rate vocoder modem format. */
#define CSD_MEDIA_ID_HR_MODEM ( 0x00010FCA )

/** Linear pulse code modulation narrowband (16-bit, little-endian). */
#define CSD_MEDIA_ID_PCM_NB ( 0x00010FCB )

/** Linear pulse code modulation wideband (16-bit, little-endian). */
#define CSD_MEDIA_ID_PCM_WB ( 0x00010FCC )

/** G.711 A-law (contains two 10-millisecond vocoder frames). */
#define CSD_MEDIA_ID_G711_ALAW ( 0x00010FCD )

/** G.711 Mu-law (contains two 10-millisecond vocoder frames). */
#define CSD_MEDIA_ID_G711_MULAW ( 0x00010FCE )

/** G.729AB (contains two 10-millisecond vocoder frames. Refer to @latexonly \hyperref[S4]{[S4]}@endlatexonly 
 (Figure 4). */
#define CSD_MEDIA_ID_G729 ( 0x00010FD0 )

/** G.722 (contains one 20-millisecond vocoder frame). */
#define CSD_MEDIA_ID_G722 ( 0x00010FD1 )

/** @} */ /* end_name_group CSD Voice Media Type Definitions */

/** @} */ /* end_addtogroup csd_vcommon */

#endif /* __CSD_VCOMMON_H__ */
