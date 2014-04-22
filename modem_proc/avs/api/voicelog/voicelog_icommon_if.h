#ifndef __VOICELOG_ICOMMON_IF_H__
#define __VOICELOG_ICOMMON_IF_H__

/**
  @file  voicelog_icommon_if.h
  @brief This file contains common interface definitions of Voice logging.
*/

/*
  ============================================================================

   Copyright (C) 2015, 2017 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //components/rel/avs.mpss/8.0.c12/api/voicelog/voicelog_icommon_if.h#1 $
  $Author: pwbldsvc $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------


  ============================================================================
*/

/****************************************************************************
  VOICE LOG COMMON INTERFACE DEFINES
****************************************************************************/

/**
 * Network ID: This identifies the RAT on which the voice call
 * set-up is done.
 *
 * NOTE: VS_VOC_CLIENT_ID_XXX GUIDs are being reused for G/W/Td/1X.
 */
#define VOICELOG_ICOMMON_NETWORK_ID_GSM     ( 0x00012E26 )
#define VOICELOG_ICOMMON_NETWORK_ID_WCDMA   ( 0x000130E2 )
#define VOICELOG_ICOMMON_NETWORK_ID_TDSCDMA ( 0x000130E3 )
#define VOICELOG_ICOMMON_NETWORK_ID_CDMA    ( 0x000130E4 )
#define VOICELOG_ICOMMON_NETWORK_ID_LTE     ( 0x000131E4 )
#define VOICELOG_ICOMMON_NETWORK_ID_WLAN    ( 0x000131E5 )
#define VOICELOG_ICOMMON_NETWORK_ID_NR      ( 0x0001334B )

/**
 * Media ID: This identifies the vocoder type using which voice 
 * call set-up is done.
 *
 * NOTE: VSS_MEDIA_ID_XXX GUIDs are being reused.
 */
#define VOICELOG_ICOMMON_MEDIA_ID_NONE          ( 0x00010FC0 )
#define VOICELOG_ICOMMON_MEDIA_ID_13K           ( 0x00010FC1 )
#define VOICELOG_ICOMMON_MEDIA_ID_EVRC_A        ( 0x00010FC2 )
#define VOICELOG_ICOMMON_MEDIA_ID_EVRC_B        ( 0x00010FC3 )
#define VOICELOG_ICOMMON_MEDIA_ID_EVRC_WB       ( 0x00010FC4 )
#define VOICELOG_ICOMMON_MEDIA_ID_EVRC_NW       ( 0x00010FC5 )
#define VOICELOG_ICOMMON_MEDIA_ID_EVRC_NW2K     ( 0x00010FD7 )
#define VOICELOG_ICOMMON_MEDIA_ID_AMR_NB        ( 0x00010FC6 )
#define VOICELOG_ICOMMON_MEDIA_ID_AMR_WB        ( 0x00010FC7 )
#define VOICELOG_ICOMMON_MEDIA_ID_EAMR          ( 0x00010FD4 )
#define VOICELOG_ICOMMON_MEDIA_ID_EFR           ( 0x00010FC8 )
#define VOICELOG_ICOMMON_MEDIA_ID_FR            ( 0x00010FC9 )
#define VOICELOG_ICOMMON_MEDIA_ID_HR            ( 0x00010FCA )
#define VOICELOG_ICOMMON_MEDIA_ID_EVS           ( 0x00010FD8 )
#define VOICELOG_ICOMMON_MEDIA_ID_G711_ALAW_V2  ( 0x000131A1 )
#define VOICELOG_ICOMMON_MEDIA_ID_G711_MULAW_V2 ( 0x000131A2 )


#endif /* __VOICELOG_ICOMMON_IF_H__ */
