#ifndef __VS_H__
#define __VS_H__

/**
  @file vs.h
  @brief This is the public header file that Clients of Voice Services (VS)
         should include. This file includes all other VS public header files.
*/

/*
  ============================================================================
  Copyright (C) 2012, 2014, 2016, 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/*----------------------------------------------------------------------------
 * Include files for Module
 *--------------------------------------------------------------------------*/

#include "vs_errcodes.h"
#include "vs_common.h"
#include "vs_voc.h"
#include "vs_vocamr.h"
#include "vs_vocamrwb.h"
#include "vs_vocfr.h"
#include "vs_vocefr.h"
#include "vs_vochr.h"
#include "vs_vocevrca.h"
#include "vs_vocevrcb.h"
#include "vs_vocevrcnw.h"
#include "vs_vocevrcnw2k.h"
#include "vs_vocevrcwb.h"
#include "vs_vocqcelp13k.h"
#include "vs_vocg711alaw_v2.h"
#include "vs_vocg711mulaw_v2.h"
#include "vs_vocevs.h"
#include "vs_vocdtmf.h"
#include "vs_vocamrwb2.h"

#include "vs_tty.h"
#include "vs_hpcm.h"
#include "vs_pktexg.h"
#include "vs_param.h"

/***********************************************************************
  Export Macros
************************************************************************/

#ifdef __cplusplus
  #define VS_EXTERNAL extern "C"
#else
  #define VS_EXTERNAL extern
#endif /* __cplusplus */
  /* This marks an external API that is intended for public use. */

#ifdef __cplusplus
  #define VS_INTERNAL extern "C"
#else
  #define VS_INTERNAL extern
#endif /* __cplusplus */


/***********************************************************************
  VS SINGLE ENTRY POINT
************************************************************************/

/**
 * Provides a single API entry point into the voice driver.
 *
 * @param[in] cmd_id Command identifier to execute.
 * @param[in] params Command parameters.
 * @param[out] size Size of the command parameters in bytes.
 *
 * @return
 *  VS_EOK (0) when successful.
 */
VS_EXTERNAL uint32_t vs_call (
  uint32_t cmd_id,
    /**< Command ID. */
  void*  cmd_params,
    /**< Reference to command-ID specific payload structure.
      *  Client is responsible allocating memory required for 
      *  command payload. */  
  uint32_t size
    /**< Size of the cmd_param structure. */
);

#endif /* __VS_H__ */

