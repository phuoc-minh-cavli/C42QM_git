#ifndef __APRV2_IPC_I_H__
#define __APRV2_IPC_I_H__

/*
  Copyright (C) 2009-2010, 2011 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary

  $Header: //components/rel/audio.tx/2.0/apr/core/inc/aprv2_ipc_i.h#2 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"
#include "aprv2_packet.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

#define APR_IPC_PACKET_SIZE ( 4096 )


APR_INTERNAL int32_t aprv2_ipc_init ( void );
APR_INTERNAL int32_t aprv2_ipc_deinit ( void );

APR_INTERNAL int32_t aprv2_ipc_is_domain_local ( uint16_t domain_id );
APR_INTERNAL int32_t aprv2_ipc_send ( aprv2_packet_t* packet );
APR_INTERNAL int32_t aprv2_ipc_free (aprv2_packet_t *packet);

#endif /* __APRV2_IPC_I_H__ */

