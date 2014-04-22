/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements functionality to save offsets into efs item file.

EXTERNALIZED FUNCTIONS
  time_efs_rw 
    Local function that provides IO access for generic offsets in EFS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
    None

Copyright (c) 2009 - 2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.


=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/services/time/src/time_efs.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/03/14   psu     Update diag macros using optimized versions.
11/23/10   din     Changed to handle allocation failure in malloc.
04/27/10   din     Fixed compiler warnings.
04/19/10   din     Prevented Multiple efs_writes for same file. Reinsertion of 
                   same element was prevented.
03/11/10   din     Moved efs write to Time IPC task.
03/10/10   din     Removed efs_unlink() as efs_put() overwites the item file.
17/10/09   din     File created. 

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "rcinit_qurt.h"
#include "time_efs.h"
#include "time_ipc.h"
#include "queue.h"
#include "msg.h"
#include "fs_public.h"
#include "time_fn_map.h"
#include <string.h>
#include <stringl/stringl.h>
#include <stdio.h>
#include <stdlib.h>

/*=============================================================================

                         DATA DEFINITIONS
=============================================================================*/

#define MAX_FILE_NAME_SIZE              16 
/* Size to be written, 64 bits for all offsets */
#define ITEM_SIZE                        8 

/* Queue to store base to be remoted through TIME IPC TASK */
static q_type                            efs_rw_q;

static time_fn_map_fp_type *time_fn_map_efs_fp;
static uint32 time_fn_map_init = 0;

typedef enum queue_enum
{
NOT_IN_QUEUE = 0,
IN_QUEUE
} queue_q;

typedef struct efs_rw_struct_type
{
  /* Link for the next queue */
  q_link_type                            link;

  /* Name of the file to be written "absolute path" */
  char                                   file_name[MAX_FILE_NAME_SIZE];

  /* Pointer to data to be read/written */
  void                                   * data_ptr;

  /* Enum to check if element is in queue or outside queue */
  queue_q                                queue;

} efs_rw_struct;

static efs_rw_struct                     efs_rw [ ATS_MAX +4 ];

static boolean efs_rw_init = FALSE;

/*=============================================================================

                         FUNCTION DEFINITIONS
=============================================================================*/



/*=============================================================================

FUNCTION TIME_EFS_RW

DESCRIPTION
  Local function that provides IO access for generic offsets in EFS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  For writes: Expect a overwrite of the entire value of what ever size

=============================================================================*/
time_efs_rw_stat_enum_type time_efs_rw 
(
  /* Name of the file to be written "absolute path" */
  const char                * file_name,

  /* Pointer to data to be read/written */
  void                      * data_ptr,

  /* Read/write command */
  time_efs_rw_cmd_enum_type rd_wr,

  /* Identifier for type of entry */
  uint8                     entry
)
{
  /* Efs_* return size variable */	
  int return_size;

  /* file name with absolute address */
  char f_name[48]={0};

  /* Initialize the return status with unexpected error */
  time_efs_rw_stat_enum_type status = TIME_EFS_RW_STAT_IO_ERR;

  /* Data to be enqeued in queue */
  efs_rw_struct * efs_rw_ptr ;

  /* Initialize efs_rw_q */
  if ( efs_rw_init == FALSE )
  {
    q_init( &efs_rw_q); 
    efs_rw_init = TRUE;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize Fn Map */
  if(time_fn_map_init == 0)
  {
    time_fn_map_efs_fp = time_fn_map_get_fp();
    time_fn_map_init++;
  }

  /* Incase of write */
  if(rd_wr == TIME_EFS_RW_CMD_WR)
  {
    /* Find space for element to be enqueued in queue */
    efs_rw_ptr = (efs_rw_struct *) &efs_rw[entry];
   
    snprintf( efs_rw_ptr->file_name , MAX_FILE_NAME_SIZE, 
		   "%s", file_name );

    if ( efs_rw_ptr->queue == NOT_IN_QUEUE )
    {
       /* Allocate space for data memeber to be wriiten in efs */
       efs_rw_ptr->data_ptr = (void * ) malloc( sizeof(int64) );

       if ( efs_rw_ptr->data_ptr == NULL )
       {
          MSG(MSG_SSID_ATS, MSG_LEGACY_ERROR," Malloc Failed. Time wil not be retained across \
			power cycle");
	  return status ;  
       }
       
       /* Copy data_ptr into allocated memory */
       memscpy( efs_rw_ptr->data_ptr, sizeof(int64), data_ptr, sizeof(int64) );

       /* Mark entry as IN_QUEUE */
       efs_rw_ptr->queue = IN_QUEUE;

       /* Link efs_rw_ptr to efs_rw_q */
       (void) q_link( efs_rw_ptr, &efs_rw_ptr->link );
    
       /* Enqueue node in efs_rw_q */
       q_put( &efs_rw_q, &efs_rw_ptr->link);

       /* Set signal to TIME_IPC task for further processing */
       #ifdef FEATURE_TIME_IPC_TASK
       if (time_ipc_qurt_sig_ptr != NULL)
       {
         (void) qurt_signal_set(time_ipc_qurt_sig_ptr,(uint32)TIME_IPC_EFS_WRITE);
       }
       else
       {
         time_ipc_initialize_signal_object();
		 (void) qurt_signal_set(time_ipc_qurt_sig_ptr,(uint32)TIME_IPC_EFS_WRITE);
       }
       #endif /*FEATURE_TIME_IPC_TASK*/
    }
    else if ( efs_rw_ptr->queue == IN_QUEUE ) 
    {
       /* Copy data_ptr into allocated memory */
       memscpy( efs_rw_ptr->data_ptr, sizeof(int64), data_ptr, sizeof(int64) );
    }
    status = TIME_EFS_RW_STAT_OK;	  
  }

  /* Incase of read */
  else if(rd_wr == TIME_EFS_RW_CMD_RD)
  {
    /* Generate the absolute path for the file */
    (void) snprintf( f_name, sizeof(f_name), 
		   "/nvm/alpha/item_file/time/%s", file_name );
  
    /* Obtain the contents from EFS */
    if(time_fn_map_efs_fp->efs_get_fp)
    {
      return_size = time_fn_map_efs_fp->efs_get_fp(f_name, data_ptr, ITEM_SIZE);
    }
    else
    {
      return_size = -1;
    }
      
    if (return_size == -1)
    {
      /* EFS read err */
      status = TIME_EFS_RW_STAT_RD_ERR;
    }
    else
    {
      status = TIME_EFS_RW_STAT_OK;
    }
  }

  return status;

} /* time_efs_rw */



/*=============================================================================

FUNCTION TIME_EFS_WRITE

DESCRIPTION
  Called from Time IPC task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  For writes: Expect a overwrite of the entire value of what ever size

=============================================================================*/

void time_efs_write()
{
   /* Element to be dequeued from queue */
   efs_rw_struct                  *efs_rw_ptr;

   /* Efs_* return size variable */	
   int                            return_size;

   /* Absolute path of efs_item file */
   char                           file_name[48];

   /* Temporary ptr to store data_ptr */
   void                           *ptr; 
	  
   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   /* Initialize Fn Map */
   if(time_fn_map_init == 0)
   {
     time_fn_map_efs_fp = time_fn_map_get_fp();
     time_fn_map_init++;
   }

   /* Get pointer to queue node */
   efs_rw_ptr = ( efs_rw_struct * ) q_get( &efs_rw_q );

   while ( efs_rw_ptr != NULL )
   {
   
    ptr = efs_rw_ptr->data_ptr;
    
    /* Mark entry as NOT_IN_QUEUE*/
    efs_rw_ptr->queue = NOT_IN_QUEUE;

    /* Attach absolute path to filename */
    snprintf( file_name , sizeof(file_name), 
		   "/nvm/alpha/item_file/time/%s", efs_rw_ptr->file_name );

    /* Write into efs_item file */
    if(time_fn_map_efs_fp->efs_put_fp)
    {
      return_size = time_fn_map_efs_fp->efs_put_fp( file_name, efs_rw_ptr->data_ptr, ITEM_SIZE, 
                                                   O_CREAT|O_AUTODIR, 0777);
    }
    else
    {
      return_size = -1;
    }

    /* Free the allocated memory */
    free( ptr );
        
    if (return_size == -1)
    {
      /* EFS write err, Time base restoration will fail in next power cycle */
      MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH," EFS write failed ");
    }

    efs_rw_ptr = ( efs_rw_struct * ) q_get( &efs_rw_q );
    
   } /* while ( efs_rw_ptr != NULL ) */

} /* time_efs_write */
