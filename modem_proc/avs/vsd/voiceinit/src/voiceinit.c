/*
  Copyright (C) 2013-2017, 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.mpss/8.0.c12/vsd/voiceinit/src/voiceinit.c#1 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/
/* Test APIs. */
#ifdef AVS_MPSS_TEST 
#include "qurt_elite.h"
#include "voice_dsm_if.h"
#endif

/* CORE APIs. */
#include "rcinit.h"
#if !defined ( WINSIM ) && !defined ( AVS_MPSS_TEST ) 
#include "rcevt_sigosal.h"
#endif
#include "msg.h"
#include "err.h"
#include "DALSys.h"

/* APR APIs. */
#include "apr_api.h"
#include "apr_errcodes.h"
#include "apr_errcodes.h"
#include "apr_event.h"
#include "apr_thread.h"
#include "apr_misc.h"

/* Voice COMMON APIs. */
#include "drv_api.h"
#include "vs_task.h"

/* Vocoder Services APIs. */
#include "vs.h"

/* Voice Adapter APIs. */
#include "gva_if.h"
#include "wva_if.h"
#include "tva_if.h"
#include "cva_if.h"

/* Voice Agent APIs. */
#include "vagent.h"

#include "voicemem.h"
#include "voicecfg_api.h"

/* VOICE UTILS APIs. */
#include "voice_diag_api.h"

/***************************************************************************
* VOICE DRIVERS INIT/DEINIT ROUTINES                                       *
****************************************************************************/
 
int32_t voice_init ( void )
{
  { /* Initialize DAL. */
    DALSYSConfig dalsyscfg = { NULL, DALREMOTING_CLIENTS_SERVERS, NULL };
    DALSYS_InitMod( &dalsyscfg );
  }

  { /* Initialize the voice drivers and services. */
    ( void ) voicecfg_call( DRV_CMDID_INIT, NULL, 0 );
    ( void ) voicemem_call( DRV_CMDID_INIT, NULL, 0 );
    ( void ) vs_call( DRV_CMDID_INIT, NULL, 0 );

    ( void ) vagent_call( DRV_CMDID_INIT, NULL, 0 );

    ( void ) gva_call( DRV_CMDID_INIT, NULL, 0 );
    ( void ) tva_call( DRV_CMDID_INIT, NULL, 0 );
    ( void ) wva_call( DRV_CMDID_INIT, NULL, 0 );
    ( void ) cva_call( DRV_CMDID_INIT, NULL, 0 );

    ( void ) voice_diag_call( DRV_CMDID_INIT, NULL, 0 );

#ifdef AVS_MPSS_TEST 
    /* AVS MPSS Testfwk makes use of dsm queues for W/TD packet exchange.  
     * Note: dsm_deinit API is not exposed by core team. 
     */
    ( void ) dsm_init();
#endif
  }


  return APR_EOK;
}

int32_t voice_postinit ( void )
{
  /* Post-initialize voice drivers and services. */
  ( void ) voicecfg_call( DRV_CMDID_POSTINIT, NULL, 0 );
  ( void ) vs_call( DRV_CMDID_POSTINIT, NULL, 0 );

  ( void ) vagent_call( DRV_CMDID_POSTINIT, NULL, 0 );

  ( void ) gva_call( DRV_CMDID_POSTINIT, NULL, 0 );
  ( void ) tva_call( DRV_CMDID_POSTINIT, NULL, 0 );
  ( void ) wva_call( DRV_CMDID_POSTINIT, NULL, 0 );
  ( void ) cva_call( DRV_CMDID_POSTINIT, NULL, 0 );

  return APR_EOK;
}

int32_t voice_predeinit ( void )
{
  /* Pre-deinitialize voice drivers and services in reverse order of
   * initialization).
   */
  ( void ) cva_call( DRV_CMDID_PREDEINIT, NULL, 0 );
  ( void ) wva_call( DRV_CMDID_PREDEINIT, NULL, 0 );
  ( void ) tva_call( DRV_CMDID_PREDEINIT, NULL, 0 );
  ( void ) gva_call( DRV_CMDID_PREDEINIT, NULL, 0 );
  ( void ) vagent_call( DRV_CMDID_PREDEINIT, NULL, 0 );


  ( void ) vs_call( DRV_CMDID_PREDEINIT, NULL, 0 );
  ( void ) voicecfg_call( DRV_CMDID_PREDEINIT, NULL, 0 );

  return APR_EOK;
}

int32_t voice_deinit ( void )
{
  /* De-initialize voice drivers and services in reverse order of 
     initialization. */

  ( void ) voice_diag_call( DRV_CMDID_DEINIT, NULL, 0 );

  ( void ) cva_call( DRV_CMDID_DEINIT, NULL, 0 );
  ( void ) wva_call( DRV_CMDID_DEINIT, NULL, 0 );
  ( void ) tva_call( DRV_CMDID_DEINIT, NULL, 0 );
  ( void ) gva_call( DRV_CMDID_DEINIT, NULL, 0 );
  ( void ) vagent_call( DRV_CMDID_DEINIT, NULL, 0 );

  ( void ) vs_call( DRV_CMDID_DEINIT, NULL, 0 );

  ( void ) voicemem_call( DRV_CMDID_DEINIT, NULL, 0 );
  ( void ) voicecfg_call( DRV_CMDID_DEINIT, NULL, 0 );

  return APR_EOK;
}

/****************************************************************************
 * SINGLE ENTRY POINT                                                       *
 ****************************************************************************/

int32_t voiceinit_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  int32_t rc = APR_EOK;

  switch ( cmd_id )
  {
    case DRV_CMDID_INIT:
      rc  =  voice_init( );
      break;

    case DRV_CMDID_POSTINIT:
      rc  =  voice_postinit( );
      break;

    case DRV_CMDID_PREDEINIT:
      rc = voice_predeinit( );
      break;

    case DRV_CMDID_DEINIT:
      rc = voice_deinit();
      break;

    default:
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
              "voiceinit_call(): Unsupported cmd ID (0x%08x)", cmd_id );
       rc = APR_EUNSUPPORTED;
     break;
  }

  return rc;
}

void voiceinit_task
(
  dword ignored
  /* Parameter received from Main Control task - ignored.
   *
   * lint -esym(715,ignored)
   * Have lint not complain about the ignored parameter 'ignored' which is
   * specified to make this routine match the template for rex_def_task().
   */
)
{
  ( void ) voiceinit_call( DRV_CMDID_INIT, NULL, 0 );
  ( void ) voiceinit_call( DRV_CMDID_POSTINIT, NULL, 0 );

  return;
}

