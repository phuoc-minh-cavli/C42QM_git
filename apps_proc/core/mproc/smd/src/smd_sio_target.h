#ifndef SMD_SIO_TARGET_H
#define SMD_SIO_TARGET_H
/*===========================================================================

   Architecture Specific Shared Memory Driver SIO Interface Header File

 Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.  All Rights
 Reserved.

 This file should only be included by smd_sio.c
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_sio_target.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/12   bt      Remove smd_target.h, move processor defs to smd_proc.h.
12/19/11   bt      Initial revision for 8974, exact same as 8960.
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "smd.h"  /* smd channel types */
#include "smd_proc.h"


/*===========================================================================

                           MACROS 
                    
===========================================================================*/
#define SMD_SIO_NUM_PROCS 3

/*===========================================================================

                          TYPEDEF
                    
===========================================================================*/
/* this type is used to translate a sio device major id to a channel id */
/* DO NOT MODIFY this struct without changing smd_sio_channel_types below */
typedef struct
{
  uint32            major_id;
  smd_channel_type  channel_type;
}smd_sio_major_id_translation_type;

/* Declare dummy timer callback type for LPASS Q6 */
#ifdef SMD_LPASS_PROC
typedef uint32 timer_cb_data_type;
#endif /* SMD_QDSP_PROC */

/*===========================================================================

                       CONSTANTS

===========================================================================*/
static const smd_sio_major_id_translation_type 
  smd_sio_channel_types[SMD_SIO_NUM_PROCS + 1] = 
#if defined(SMD_MODEM_PROC)
{
  {SIO_MAJOR_SMD,          SMD_APPS_MODEM},
  {SIO_MAJOR_SMD_TO_APPS,  SMD_APPS_MODEM},
  {SIO_MAJOR_SMD_TO_MODEM, SMD_INVALID_CHANNEL_TYPE},
  {SIO_MAJOR_SMD_TO_QDSP6,  SMD_MODEM_QDSP}
};
#elif defined(SMD_APPS_PROC)
{
  {SIO_MAJOR_SMD,           SMD_APPS_MODEM},
  {SIO_MAJOR_SMD_TO_APPS,  SMD_INVALID_CHANNEL_TYPE},
  {SIO_MAJOR_SMD_TO_MODEM, SMD_APPS_MODEM},
//  {SIO_MAJOR_SMD_TO_QDSP6,  SMD_APPS_QDSP}
};
#elif defined(SMD_LPASS_PROC)
{
  {SIO_MAJOR_SMD,           SMD_INVALID_CHANNEL_TYPE},
  {SIO_MAJOR_SMD_TO_APPS,  SMD_APPS_QDSP},
  {SIO_MAJOR_SMD_TO_MODEM, SMD_MODEM_QDSP},
  {SIO_MAJOR_SMD_TO_QDSP6,  SMD_INVALID_CHANNEL_TYPE}
};
#else
 #error "Unknown processor type"
#endif

#endif /* SMD_SIO_TARGET_H */
