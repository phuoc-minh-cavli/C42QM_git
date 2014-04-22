#ifndef _APR_QSOCKET_H_
#define _APR_QSOCKET_H_

/*
  Copyright (C) 2010-2012,2016 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/4.0/datalink/apr_qsocket/inc/apr_qsocket.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"
#include "qsocket.h"
#include "aprv2_ipc_config.h"

/******************************************************************************
 * Defines                                                                    *
 *****************************************************************************/
#define APR_QSOCKET_MAX_PORTS ( QSOCKET_PORTS )

int32_t apr_qsocket_tx(uint32_t port_index, void *buf, uint32_t length);


#endif /*_APR_QSOCKET_H_*/

