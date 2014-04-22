#ifndef TIME_GENOFF_V_H
#define TIME_GENOFF_V_H
/*=============================================================================

 T I M E   S E R V I C E   G E N E R I C  F R A M E W O R K  S U B S Y S T E M

GENERAL DESCRIPTION
 Implements Generic framework to handle Time bases. 

EXTERNALIZED FUNCTIONS
  time_genoff_pre_init
    Pre -Initializes Generic offset
  
  time_genoff_post_init
    Completes initialization sequence of Generic offset

  time_genoff_add_base_subsys
    Adds a genoff instance as base_subsystem for another genoff instance

  time_genoff_attach_cb
    Attaches call back which are called during after set operation

  time_genoff_set_remote_genoff
    Updates genoff in other proc

  time_genoff_add_init_call
    Adds initialization sequence for the genoff in consideration

  time_genoff_updates_per_storage
    Updates persistent storage info

  time_genoff_get_pointer
    Returns the Pointer to different time bases

  time_genoff_set_generic_offset
    Sets Generic offset.

  time_genoff_get_offset
    Gets Generic offset

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Should be initialized with following sequence
    
  1) time_genoff_pre_init
  2) time_genoff_add_init_func ( Only if init_func is needed  ) 
  3) time_genoff_add_base_subsys ( Only if base_subsys is needed ) 
  4) time_genoff_updates_per_storage ( Only on Modem Proc, If NV is not used )
  5) time_genoff_attach_cb ( Only if cb_func is needed )
  6) time_genoff_post_init
 

Copyright (c) 2009 - 2018 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/services/time/src/time_genoff_v.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/26/11   din     Fixed compiler warnings.
08/20/10   din     Added a new optimized time_genoff_get_optimized call.
06/23/10   din     Added new time bases and attached a proxy base for a base.
05/18/10   din     Shifted time_remote_opr_type to time_genoff.h
02/19/10   din     Moved genoff RPC calls from modem proc to TIME_IPC task.
10/07/09   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "time_genoff.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


/*--------------------------------------------------------------------------
  Linting...
--------------------------------------------------------------------------*/

/*lint -esym(715,unused_*)  parameters prefixed with unused_ are not used */

/*-----------------------------------------------------------------------------
  Time Bases static information
-----------------------------------------------------------------------------*/

/* Threshold to update time base */
#define TIME_GENOFF_INTERNAL_UPDATE_THRESHOLD_MS 10

#ifdef FEATURE_QMI_TIME_REMOTE_CLNT
typedef struct
{
  boolean leap_sec_valid;
  uint8   leap_sec_value;
}time_genoff_leap_sec_info_type;
#endif

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/



/*=============================================================================

FUNCTION TIME_GENOFF_PRE_INIT

DESCRIPTION
  Pre -Initializes Generic offset. 

DEPENDENCIES
  Should be followed by other initialization calls and finally followed by 
  time_genoff_post_init 

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_pre_init
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff,

  /* Time base type for the Genoff in consideration */
  time_bases_type base  
);



/*=============================================================================

FUNCTION TIME_GENOFF_POST_INIT

DESCRIPTION
  Completes initialization sequence of generic offset

DEPENDENCIES
  Should be preceeded by time_genoff_pre_init and other init calls

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_post_init
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff
);



/*=============================================================================

FUNCTION TIME_GENOFF_IS_VALID

DESCRIPTION
  Checks if genoff is valid

DEPENDENCIES
  None

RETURN VALUE
  TRUE if valid, FALSE if not valid

SIDE EFFECTS
  None

=============================================================================*/

boolean time_genoff_is_valid
( 
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff
);

/*=============================================================================

FUNCTION TIME_GENOFF_SHUTDOWN_NOTIFICATION_FN

DESCRIPTION
  Write the final updates to efs file item on shutdown event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_genoff_shutdown_notification_fn( void );


/*=============================================================================

FUNCTION TIME_GENOFF_ADD_BASE_SUBSYS

DESCRIPTION
  Adds time_genoff1 as base_subsys for time_genoff

DEPENDENCIES
  Should be called between time_genoff_pre_init() and time_genoff_post_init() 

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_add_base_subsys 
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff,

  /* Subsys base */
  time_bases_type base 
);



/*=============================================================================

FUNCTION TIME_GENOFF_ADD_PROXY_BASE

DESCRIPTION
  Adds base1 as proxy_subsys for base

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_add_proxy_base
(
  /* Base in consideration */
  time_bases_type base,

  /* Proxy base */
  time_bases_type base1 
);



/*=============================================================================

FUNCTION TIME_GENOFF_ATTACH_CB

DESCRIPTION
  Attaches call back which are called during after set operation

DEPENDENCIES
  Should be called between time_genoff_pre_init() and time_genoff_post_init() 

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_attach_cb
(
  /* Base to which callback needs to attached */
  time_bases_type  base_cb,

  /* Call back Func to be attached */
  time_genoff_t2_cb_type cb,

  /* Base that has registered for callback */
  time_genoff_ptr  registered_genoff_ptr
);


/*=============================================================================

FUNCTION TIME_GENOFF_SET_REMOTE_CALL_NEEDED

DESCRIPTION
  Sets remote_call_needed to TRUE/FALSE, to update/not update genoff offset 
  in another proc. Also updates genoff in other proc

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_set_remote_call_needed
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff,

  /* Value set to remote_call_needed */
  boolean         value
);



/*=============================================================================

FUNCTION TIME_GENOFF_SET_ALLOW_REMOTE_UPDATES

DESCRIPTION
  Sets allow_remote_updates to TRUE/FALSE, which is used to update 
  remote_call_needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_set_allow_remote_updates
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff,

  /* Value set to allow_remote_updates */
  boolean         value
);


/*=============================================================================

FUNCTION TIME_GENOFF_GET_ALLOW_REMOTE_UPDATES

DESCRIPTION
  Get allow_remote_updates .

DEPENDENCIES
  None

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None

=============================================================================*/

boolean time_genoff_get_allow_remote_updates
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff
);


/*=============================================================================

FUNCTION TIME_GENOFF_SET_REMOTE_GENOFF

DESCRIPTION
  Updates genoff in other proc

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_set_remote_genoff
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff
);


/*=============================================================================

FUNCTION TIME_GENOFF_REMOTE_THROUGH_TIME_IPC

DESCRIPTION
  Called from time_ipc to update genoff to another proc

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_remote_through_time_ipc(void);



/*=============================================================================

FUNCTION TIME_GENOFF_ADD_INIT_CALL

DESCRIPTION
  Adds initialization sequence for the genoff in consideration

DEPENDENCIES
  Should be called between time_genoff_pre_init() and time_genoff_post_init()

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_add_init_call
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff,

  /* Call to be added */
  time_genoff_t1_cb_type func
);



/*=============================================================================

FUNCTION TIME_GENOFF_UPDATE_PER_STORAGE

DESCRIPTION
  Updates persistent storage info 

DEPENDENCIES
  Should be called between time_genoff_pre_init() and time_genoff_post_init()

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_updates_per_storage
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff,

  /* File name for efs item file */
  char            *name,

  /* Threshold in ms for writing back in efs item file */
  int64           thold
);



/*=============================================================================

FUNCTION TIME_GENOFF_GET_POINTER                                 

DESCRIPTION
  Returns the Pointer to different time bases 

DEPENDENCIES
  None

RETURN VALUE
  The pointer to time_genoff_struct_type

SIDE EFFECTS
  None

=============================================================================*/

time_genoff_ptr time_genoff_get_pointer
( 
  /* Time bases whose pointer needs to be returned */
  time_bases_type               base
);



/*=============================================================================

FUNCTION TIME_GENOFF_SET_GENERIC_OFFSET                               

DESCRIPTION
  Sets Generic offset.

DEPENDENCIES
  Should be used only by remote calls

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_set_generic_offset
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff,

  /* Generic Offset to be set */
  int64           offset
);

/*=============================================================================

FUNCTION TIME_GENOFF_GET_OFFSET

DESCRIPTION
  Returns generic offset value of a given time base

DEPENDENCIES
  None

RETURN VALUE
  int64 value

SIDE EFFECTS
  None

=============================================================================*/
int64 time_genoff_get_offset
( 
    time_bases_type  base  
);

void time_genoff_set_server_modem_time
(
  time_bases_type base_to_be_updated
);

#ifdef FEATURE_QMI_TIME_REMOTE_CLNT
/*=============================================================================

FUNCTION TIME_GENOFF_SET_LEAP_SEC_INFO

DESCRIPTION
  This function sets the leap second info on apps

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_genoff_set_leap_sec_info
(
  time_genoff_leap_sec_info_type lp_sec
);
#endif
/*=============================================================================

FUNCTION TIME_GENOFF_GET_GENOFF_OFFSET

DESCRIPTION
  Returns generic offset value of a given external genoff pointer.

DEPENDENCIES
  None

RETURN VALUE
  int64 value

SIDE EFFECTS
  None

=============================================================================*/
int64 time_genoff_get_genoff_offset
( 
    time_genoff_ptr  genoff_ptr  
);

void time_genoff_set_server_modem_time
(
  time_bases_type base_to_be_updated
);

#endif /* TIME_GENOFF_V_H */
