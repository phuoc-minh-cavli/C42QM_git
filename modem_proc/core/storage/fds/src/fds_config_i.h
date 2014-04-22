/***********************************************************************
 * fds_config_i.h
 *
 * Private Configuration items for FDS
 * Copyright (C) 2013 QUALCOMM Technologies, Inc.
 *
 * Private Configuration items for Flash Driver Service.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.10/storage/fds/src/fds_config_i.h#1 $ $DateTime: 2018/06/26 03:18:17 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-09-30   dks   Create

===========================================================================*/

#ifndef __FDS_CONFIG_I_H__
#define __FDS_CONFIG_I_H__

#include "fds_config.h"

/* Maximum number of clients the service can supoort at any given time. */
#ifndef FDS_MAX_CLIENTS_SUPPORTED
  #define FDS_MAX_CLIENTS_SUPPORTED (4)
#endif

/* Maximum events that the service needs to keep track of to send indications
   to registered clients. */
#define FDS_MAX_EVENTS_SUPPORTED (1)


#endif /* not __FDS_CONFIG_I_H__ */
