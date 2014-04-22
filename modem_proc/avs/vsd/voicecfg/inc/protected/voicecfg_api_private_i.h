#ifndef __VOICECFG_API_PRIVATE_I_H__
#define __VOICECFG_API_PRIVATE_I_H__

/**
  @file  voicecfg_api_private_i.h
  @brief This file contains item IDs supported by voice configuration.  
*/

/*
   Copyright (C) 2013, QUALCOMM Technologies Incorporated.
   All rights reserved.
   QUALCOMM Proprietary/GTDR.

   $Header: //components/rel/avs.mpss/8.0.c12/vsd/voicecfg/inc/protected/voicecfg_api_private_i.h#1 $
   $Author: pwbldsvc $
*/

/* GUID to capture the memory reduction in CVD. If enabled will retain the
 * CVD sessions after the end of the call, else CVD sessions would be
 * cleared after the end of the call.
 */
#define VOICECFG_CFG_IS_RETAIN_CVD_AFTER_RELEASE_OPTIMIZATION_DISABLED ( 0x0001305A )

#endif /* __VOICECFG_API_PRIVATE_I_H__ */

