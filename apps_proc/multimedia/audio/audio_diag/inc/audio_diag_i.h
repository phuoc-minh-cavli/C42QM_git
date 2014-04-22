#ifndef __AUDIO_DIAG_I_H__
#define __AUDIO_DIAG_I_H__

/**
  @file  audio_diag_i.h
  @brief This file contains private definitions of voice diag utility.
*/

/*
  ============================================================================

   Copyright (C) 2017 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //components/rel/audio.tx/2.0/audio/audio_diag/inc/audio_diag_i.h#1 $
  $Author: pwbldsvc $

  ============================================================================
*/

/***************************************************************************
  Include files for Module                                                 *
****************************************************************************/

#include "diagcmd.h"
#include "diagpkt.h"

/* SYSTEM UTILITY APIs. */
#include "mmdefs.h"


/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define AUDIO_DIAG_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "audio_diag:%d error[%d]", __LINE__, rc, 0 ); } }

#define VOICE_DIAG_LOOPBACK_COMMAND ( 0 )

#define VOID_DIAG_PACKED_PTR PACKED void* PACKED_POST


/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/

typedef PACKED struct PACKED_POST audio_diag_debug_cmd_req_t
{
  diagpkt_subsys_header_type header;
  uint8_t cmd;
  uint8_t payload1;
}
  audio_diag_debug_cmd_req_t;

typedef PACKED struct PACKED_POST audio_diag_debug_cmd_rsp_t
{
  diagpkt_subsys_header_type header;
  uint32_t result;
}
  audio_diag_debug_cmd_rsp_t;

/****************************************************************************
 * FORWARD PROTOTYPES                                                       *
 ****************************************************************************/

VOID_DIAG_PACKED_PTR audiodiag_debug_cmd_processing (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
);

int32_t audio_diag_init ( void );


#endif /* __AUDIO_DIAG_I_H__ */

