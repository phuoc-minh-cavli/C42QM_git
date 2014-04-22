#ifndef GLINK_SIO_TARGET_H
#define GLINK_SIO_TARGET_H

/*===========================================================================

   Architecture Specific GLINK SIO Interface Header File

 Copyright (c) 2011-2014,2018 by Qualcomm Technologies, Incorporated.  All Rights
 Reserved.

 This file should only be included by smd_sio.c
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/20/14   db      Initial version of glink_sio_target.h
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "smem_type.h"  /* smd channel types */
#include "glink_sio_proc.h"
#include "glink.h"
#include "sio.h"


/*===========================================================================

                           MACROS 
                    
===========================================================================*/

#define GLINK_SIO_TRANSPORT     "SMEM"

#define GLINK_SIO_NUM_PROCS     2
#define GLINK_SIO_INVALID_HOST  NULL

/* Max Glink packet size for stream type port for joining the multiple DSM items */
#define GLINK_SIO_STREAM_TX_MAX_SIZE  (2*1024)

/*===========================================================================

                           TYPEDEFS 
                    
===========================================================================*/

/* This structure provides name of remote SS based in SIO major id */
typedef struct 
{
  uint32      major_id;
  const char  *host_name;
} glink_sio_host_name_translation;
                  
typedef glink_sio_host_name_translation glink_sio_host_name_translation_type;

static const glink_sio_host_name_translation_type
              glink_sio_host_name[GLINK_SIO_NUM_PROCS + 1] = 
{
  {SIO_MAJOR_SMD,           GLINK_SIO_REMOTE_HOST},
  {SIO_MAJOR_SMD_TO_APPS,   GLINK_SIO_REMOTE_HOST},
  {SIO_MAJOR_SMD_TO_MODEM,  GLINK_SIO_INVALID_HOST} /* invalid entry should always 
						                                           be last  in array */
};
#endif /* GLINK_SIO_TARGET_H */ 
