/*
   DESCRIPTION
   This file contains the defintions of csd diag utility

   Copyright (C) 2017 QUALCOMM Technologies Incorporated.
   All rights reserved.
   Qualcomm Confidential and Proprietary.

   $Header: //components/rel/audio.tx/2.0/audio/audio_diag/src/audio_diag.c#1 $
   $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

/* SYSTEM UTILITY APIs. */
#include "msg.h"
#include "err.h"
#include "comdef.h"
#include "diagbuf.h"
#include "diagpkt.h"
#include "diagcmd.h"

/* APR APIs. */
#include "apr_errcodes.h"
#include "apr_log.h"

/* CSD APIs. */
#include "audio_diag_i.h"

/*****************************************************************************
 * GLOBALS                                                              *
 ****************************************************************************/

#define AUDIO_DEBUG_CMD_ID_START 0
#define AUDIO_DEBUG_CMD_ID_END   0

#define AUDIO_DEBUG_CMD_ID_CALL_CONTINUE 0

extern boolean csd_debug_call_continue;

/****************************************************************************
 * INTERNAL ROUTINES                                                 *
 ****************************************************************************/

static const diagpkt_user_table_entry_type audio_diag_tbl[] =
{
  { 
    AUDIO_DEBUG_CMD_ID_START,
    AUDIO_DEBUG_CMD_ID_END, 
    audiodiag_debug_cmd_processing
  }
};

VOID_DIAG_PACKED_PTR audiodiag_debug_cmd_processing (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
)
{
  uint32_t rc = APR_EOK;
  audio_diag_debug_cmd_rsp_t* rsp_pkt = NULL;
  uint8_t cmd = ( ( audio_diag_debug_cmd_req_t* ) req_pkt )->cmd;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "audiodiag_debug_cmd_processing(): Command:(%d)", cmd );

  switch ( cmd ) 
  {
    case AUDIO_DEBUG_CMD_ID_CALL_CONTINUE:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "audiodiag_debug_cmd_processing(): payload1(%d) ",
             ( ( audio_diag_debug_cmd_req_t* ) req_pkt )->payload1 );
      csd_debug_call_continue = TRUE;

    }
    break;

    default:
      rc = APR_EUNSUPPORTED;
      break;
  }

  rsp_pkt = ( audio_diag_debug_cmd_rsp_t* )diagpkt_subsys_alloc( DIAG_SUBSYS_APR,
                                          VOICE_DIAG_LOOPBACK_COMMAND,
                                          sizeof( audio_diag_debug_cmd_rsp_t ) );
  if ( rsp_pkt == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "csddiag_call_continue_cmd_processing(): Diag Packet could not be allocated");
    return NULL;
  }

  rsp_pkt->result = rc;
  diagpkt_commit( ( void* )rsp_pkt );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "csddiag_call_continue_cmd_processing(): Rsp(0x%08x)",  rsp_pkt->result );

  return rsp_pkt;
}

static int32_t audio_diag_register_with_diag ( void )
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
       "audio_diag_register_with_diag: Registering to DIAG_SUBSYS_VOICE" );
  DIAGPKT_DISPATCH_TABLE_REGISTER( DIAG_SUBSYS_APR, audio_diag_tbl );

  return APR_EOK;
}

/****************************************************************************
 * EXTERNAL API ROUTINES                                                    *
 ****************************************************************************/

int32_t audio_diag_init ( void )
{
  int32_t rc;
  rc = audio_diag_register_with_diag( );

  return rc;
}
