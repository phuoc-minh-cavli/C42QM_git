#ifndef __VOICE_DIAG_I_H__
#define __VOICE_DIAG_I_H__

/**
  @file  voice_diag_i.h
  @brief This file contains private definitions of voice diag utility.
*/

/*
  ============================================================================

   Copyright (C) 2016 - 2017 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //components/rel/avs.mpss/8.0.c12/vsd/utils/inc/private/voice_diag_i.h#1 $
  $Author: pwbldsvc $

  ============================================================================
*/

/***************************************************************************
  Include files for Module                                                 *
****************************************************************************/

/* SYSTEM UTILITY APIs. */
#include "mmdefs.h"

/* Core APIs */
#include "diagpkt.h"

/* APR APIs. */
#include "apr_comdef.h"
#include "apr_lock.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define VOICE_DIAG_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "voice_diag:%d error[%d]", __LINE__, rc, 0 ); } }

#define VOICE_DIAG_LOOPBACK_COMMAND ( 0 )

#define VOID_DIAG_PACKED_PTR PACKED void* PACKED_POST

#define VOICEDIAG_ITEST_CMD_LOOPBACK_INIT ( 0 )
#define VOICEDIAG_ITEST_CMD_LOOPBACK_DEINIT ( 1 )
#define VOICEDIAG_ITEST_CMD_LOOPBACK_START_VOCODER ( 2 )
#define VOICEDIAG_ITEST_CMD_LOOPBACK_STOP_VOCODER ( 3 )
#define VOICEDIAG_ITEST_CMD_LOOPBACK_SET_VOCODER_PROPERTY ( 4 )

/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/

#if defined( T_WINNT ) || defined( FEATURE_WINCE )
   #error code not present
#endif

typedef PACKED struct PACKED_POST voice_diag_loopback_cmd_req_t
{
  diagpkt_subsys_header_type header;
  uint8_t cmd;
  uint8_t payload1;
  uint8_t payload2;
  uint8_t payload3;
  uint8_t payload4;
  uint8_t payload5;
}
  voice_diag_loopback_cmd_req_t;

typedef PACKED struct PACKED_POST voice_diag_loopback_rsp_t
{
  diagpkt_subsys_header_type header;
  uint32_t result;
}
  voice_diag_loopback_rsp_t;

/****************************************************************************
 * FORWARD PROTOTYPES                                                       *
 ****************************************************************************/

VOID_DIAG_PACKED_PTR voicediag_loopback_cmd_processing (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
);

#endif /* __VOICE_DIAG_I_H__ */

