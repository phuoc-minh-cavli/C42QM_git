#ifndef GLINK_SIO_TASK_H
#define GLINK_SIO_TASK_H

/*===========================================================================

                    GLINK TASK Header File

 Copyright (c) 2015-2016,2018 by Qualcomm Technologies, Incorporated.  
 All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE



when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/10/16   ap      Rework to move all handling into worker threads
02/10/16   ap      Do not process any commands till the link is up.
07/24/15   db      Modified return type of glink_sio_thread_cb function from 
                   void to int32 to return error code
08/29/14   db      initial version 

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "queue.h"
#include "smem_type.h"
#include "glink_sio_proc.h"
#include "glink_os_utils.h"
#include "glink.h"

/*===========================================================================

                        TYPEDEFS

===========================================================================*/

/* function pointer for thread callback function  */
/* this function will be passed in glink_command 
 * and called back from glink thread              */
typedef int32 ( *glink_sio_thread_cb_fn )( void * );

/* glink command structure */
typedef struct
{
  glink_sio_thread_cb_fn glink_sio_thread_cb;     /* thread callback function */
  void *glink_sio_thread_priv_data;               /* private data for thread callback function */
} glink_sio_command_type;

/* structure to create queue of commands */
typedef struct
{
  q_link_type             link;
  glink_sio_command_type  cmd;
  /* Set to TRUE if this was a dynamic allocation */
  boolean                 dynamic;
} glinki_sio_command_type;

/*===========================================================================

                        FUNCTION DECLARATION

===========================================================================*/
/* function definition to be exposed */
void glink_sio_notify
(
  glink_sio_thread_cb_fn glink_sio_thread_cb,
  void *priv,
  boolean allow_duplicate
);

#endif /* GLINK_SIO_TASK_H */

