/*
   DESCRIPTION
   This file contains the defintions of voice diag utility

   Copyright (C) 2016, 2018 QUALCOMM Technologies Incorporated.
   All rights reserved.
   Qualcomm Confidential and Proprietary.

   $Header: //components/rel/avs.mpss/8.0.c12/vsd/utils/src/voice_diag.c#1 $
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
#include "drv_api.h"

/* APR APIs. */
#include "apr_errcodes.h"
#include "apr_log.h"

/* VSD APIs. */
#include "voice_diag_api.h"
#include "voice_diag_i.h"
#include "vs_testclient.h"
#include "vs.h"

/*****************************************************************************
 * GLOBALS                                                              *
 ****************************************************************************/

static const diagpkt_user_table_entry_type voicediag_tbl[] =
{
  { VOICE_DIAG_LOOPBACK_COMMAND, VOICE_DIAG_LOOPBACK_COMMAND, 
    voicediag_loopback_cmd_processing },
};

/****************************************************************************
 * INTERNAL ROUTINES                                                 *
 ****************************************************************************/

VOID_DIAG_PACKED_PTR voicediag_loopback_cmd_processing (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
)
{
  uint32_t rc = APR_EOK;
  voice_diag_loopback_rsp_t* rsp_pkt = NULL;
  uint8_t cmd = ( ( voice_diag_loopback_cmd_req_t* ) req_pkt )->cmd;
  vs_test_loopback_cmd_t cmd_param;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "voicediag_loopback_cmd_processing(): Command:(%d)", cmd );

  switch ( cmd ) 
  {
    case VOICEDIAG_ITEST_CMD_LOOPBACK_INIT:
      rc = vs_test_call( DRV_CMDID_INIT, NULL, 0); 
      break;
      
    case VOICEDIAG_ITEST_CMD_LOOPBACK_START_VOCODER:
      rc = vs_test_call( VS_ITEST_CMD_LOOPBACK_START, NULL, 0 );
      break;
      
    case VOICEDIAG_ITEST_CMD_LOOPBACK_STOP_VOCODER:
      rc = vs_test_call( VS_ITEST_CMD_LOOPBACK_STOP, NULL, 0 );
      break;
      
    case VOICEDIAG_ITEST_CMD_LOOPBACK_SET_VOCODER_PROPERTY:
    {
      cmd_param.payload1 = ( ( voice_diag_loopback_cmd_req_t* ) req_pkt )->payload1;
      cmd_param.payload2 = ( ( voice_diag_loopback_cmd_req_t* ) req_pkt )->payload2;
      cmd_param.payload3 = ( ( voice_diag_loopback_cmd_req_t* ) req_pkt )->payload3;
      cmd_param.payload4 = ( ( voice_diag_loopback_cmd_req_t* ) req_pkt )->payload4;
      cmd_param.payload5 = ( ( voice_diag_loopback_cmd_req_t* ) req_pkt )->payload5;

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "voicediag_loopback_cmd_processing(): payload1(%d), payload2(%d)",
             cmd_param.payload1, cmd_param.payload2 );
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "voicediag_loopback_cmd_processing(): payload3(%d), payload4(%d), payload5(%d)", 
             cmd_param.payload3, cmd_param.payload4, cmd_param.payload5 );
      rc = vs_test_call( VS_ITEST_CMD_LOOPBACK_SET_VOC_PROPERTY, &cmd_param, sizeof( cmd_param ) );
    }
    break;

    case VOICEDIAG_ITEST_CMD_LOOPBACK_DEINIT:
      rc = vs_test_call( DRV_CMDID_DEINIT, NULL, 0);
      break;

    default:
      rc = APR_EUNSUPPORTED;
      break;
  }

  rsp_pkt = ( voice_diag_loopback_rsp_t* )diagpkt_subsys_alloc( DIAG_SUBSYS_MVS,
                                          VOICE_DIAG_LOOPBACK_COMMAND,
                                          sizeof( voice_diag_loopback_rsp_t ) );
  if ( rsp_pkt == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "voicediag_loopback_cmd_processing(): Diag Packet could not be allocated");
    return NULL;
  }

  rsp_pkt->result = rc;
  diagpkt_commit( ( void* )rsp_pkt );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "voicediag_loopback_cmd_processing(): Rsp(0x%08x)",  rsp_pkt->result );

  return rsp_pkt;
}

static int32_t voice_diag_register_with_diag ( void )
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
       "voice_diag_register_with_diag: Registering to DIAG_SUBSYS_VOICE" );

  DIAGPKT_DISPATCH_TABLE_REGISTER( DIAG_SUBSYS_MVS, voicediag_tbl );
  return APR_EOK;
}

/****************************************************************************
 * EXTERNAL API ROUTINES                                                    *
 ****************************************************************************/

static int32_t voice_diag_init ( void )
{
  int32_t rc;
  rc = voice_diag_register_with_diag( );

  return rc;
}

int32_t voice_diag_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  int32_t rc;

  switch ( cmd_id )
  {
    case DRV_CMDID_INIT:
      rc = voice_diag_init( );
      break;

    case DRV_CMDID_DEINIT:
      rc = APR_EOK;
      break;

    default:
      rc = APR_EUNSUPPORTED;
      break;
  }

  return rc;
}
