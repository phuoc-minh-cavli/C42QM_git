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

  time_genoff_opr
   Does time set/get operation on the basis of Inputs

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Should be initialized with following sequence

  1) time_genoff_pre_init
  2) time_genoff_add_init_func ( Only if init_func is needed  ) 
  3) time_genoff_add_base_subsys ( Only if base_subsys is needed ) 
  4) time_genoff_updates_per_storage ( Only on Modem Proc, If NV is not used )
  5) time_genoff_attach_cb ( Only if cb_func is needed )
  6) time_genoff_post_init
 
    
  Copyright (c) 2009-2018,2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/services/time/src/time_genoff.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/03/14   psu     Update diag macros using optimized versions.
04/26/11   din     Fixed compiler warnings.
03/11/11   ab      Removed compiler warnings.
08/20/10   din     Added a new optimized time_genoff_get_optimized call.
07/12/10   din     Proxy base for a base is updated directly. Added validity member in 
                   genoff_structure to control remote updates.
07/12/10   din     Used internal features for better readability.
06/23/10   din     Added new time bases and attached a proxy base for a base.
04/19/10   din     Prevented Multiple efs_writes for same file. 
02/22/10   din     Fixed compiler error incase IMAGE_MODEM_PROC is not defined.
02/19/10   din     Moved genoff RPC calls from modem proc to TIME_IPC task.
12/13/09   din     Changed sclk64_to_timestamp to sclk_to_timestamp_64.
12/08/09   din     Removed compilation warnings.
10/07/09   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "time_genoff_v.h"

#if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_TIME_NO_RPC)
#include "time_remote.h"
#endif /* FEATURE_MULTIPROCESSOR && !FEATURE_TIME_NO_RPC */

#include <stringl/stringl.h>
#include "time_svc.h"
#include "time_jul.h"
#include "time_conv.h"
#include "time_efs.h"
#include "time_ipc.h"
#include "timetick_sclk64.h"
#include "msg.h"
#include "stringl/stringl.h"
#include "amssassert.h"
#include "queue.h"
#include "osal.h"
#include "log.h"
#include "timetick.h"
#include "rcinit.h"
#include "time_fn_map.h"

#ifdef FEATURE_QMI_TIME_REMOTE_CLNT
#include "time_service_v01.h"
#include "qmi_time_client.h"
#endif
                           
#include "time_fn_map.h"


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/
/* Size to be written, 64 bits for all offsets */
#define ITEM_SIZE                        8
#define TIME_GENOFF_REMOTE_UPDATE_MS     10 
#define PMIC_RTC_RESET_CHECK_TIME_THRESHOLD   60000 /* 60k msecs = 1 min */
/*--------------------------------------------------------------------------
  Linting...
--------------------------------------------------------------------------*/

/*lint -esym(715,unused_*)  parameters prefixed with unused_ are not used */


/*-----------------------------------------------------------------------------
  Macros used for converting to/from ts_timestamp format
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  Generic offset information
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
  Time Bases static information
-----------------------------------------------------------------------------*/

static time_fn_map_fp_type *time_fn_map_efs_fp;

int64 time_genoff_last_efs_write_val;

time_genoff_struct_type              ats_bases[ATS_MAX];

#ifdef FEATURE_TIME_MULTI_SERVER_PROC
static ats_cfg_event_info_s_type         cfg_event_info;
#endif 

#if(defined FEATURE_TIME_MULTI_SERVER_PROC || defined FEATURE_QMI_TIME_REMOTE)

/* Queue to store base to be remoted through TIME IPC TASK */
static q_type                            rpc_q;

typedef enum queue_enum
{
NOT_IN_QUEUE = 0,
IN_QUEUE
} queue_q;

typedef struct rpc_struct_type
{
  /* Link for the next queue */
  q_link_type                            link;

  /* time base stored as data in the node of queue */
  time_bases_type                        base;

  /* Enum to check if element is in queue or outside queue */
  queue_q                                queue;

} rpc_struct;

static rpc_struct                        rpc_base[ATS_MAX];

#endif /* FEATURE_TIME_MULTI_SERVER_PROC */

static uint8 time_genoff_update_remote_modem = 1;

static uint8 time_genoff_1x_time_updated = 0;

#ifdef FEATURE_QMI_TIME_REMOTE_CLNT
extern qmi_client_type time_qmi_client_handle;
static time_genoff_leap_sec_info_type time_genoff_leap_sec_info={FALSE,0};
#endif

static time_delta_report_log_type time_delta_log;

#ifdef FEATURE_QMI_TIME_REMOTE
static uint32 time_genoff_fps_initialized = 0;
#endif /*FEATURE_QMI_TIME_REMOTE*/

#if (defined FEATURE_TIME_MULTI_SERVER_PROC || defined FEATURE_QMI_TIME_REMOTE)
static time_fn_map_fp_type *time_genoff_fps;
#endif

static uint32 time_genoff_efs_reset_bases = 0;
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
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Mark time_genoff as not initialized */
  time_genoff->initialized = FALSE;  
  
  /* Mark time_genoff as not valid */
  time_genoff->valid = FALSE;  
  
  /* Initially mark time_genoff with no subsys */
  time_genoff->reqd_base_genoff = FALSE ;

  /* Initially set Generic Offset as 0 */
  time_genoff->generic_offset = 0;  

  /* Initially set sum_generic_offset as 0 */
  time_genoff->sum_generic_offset = 0;  

  /* Initially set number_of_cb_func as 0 */
  time_genoff->number_of_cb_func = 0;

  /* Initialize time-base type */
  time_genoff->bases_type =  base;

  /* Initially set init_func as NULL */
  time_genoff->init_func = NULL;
      
  /* Initially set storage spec as uninitilized */
  time_genoff->per_storage_spec.initialized = FALSE;
  
  /* Initially set remote_call_needed as FALSE */
  time_genoff->remote_call_needed = FALSE;

  /* Initially set allow_remote_updates as TRUE */
  time_genoff->allow_remote_updates = TRUE;

  /* Initially set proxy base with ATS_INVALID */
  time_genoff->proxy_base = ATS_INVALID ;

#if (defined FEATURE_TIME_MULTI_SERVER_PROC || defined FEATURE_QMI_TIME_REMOTE)
  /* Initialize rpc_q */
  q_init ( &rpc_q );

#endif /* FEATURE_TIME_MULTI_SERVER_PROC */

} /*  time_genoff_pre_init */



/*=============================================================================

FUNCTION TIME_GENOFF_CB_UPDATE_SUM_GEN_OFF

DESCRIPTION
  Updates Sum of Generic offset in ts_val format  

DEPENDENCIES
  None

RETURN VALUE
  TRUE if valid, FALSE if not valid

SIDE EFFECTS
  None

=============================================================================*/

static void time_genoff_cb_update_sum_gen_off
( 
  /* Base that has registered for callback */
  void  *registered_genoff_ptr,
  
  /* Pointer to Genoff in consideration */
  int64 data
)
{
  /* Pointer to 'base' in consideration */
  time_genoff_ptr ptime_genoff, ptime_genoff_local;

  /* Pointer to sum_generic_offset */
  uint64   sum_generic_offset_local=0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( registered_genoff_ptr == NULL )
  {
    return;
  }
  
  /* Set data to zero , to avoid compiler warning */
  data = 0 * data ;

  /* Get pointer to base in consideration */
  ptime_genoff = (time_genoff_ptr)registered_genoff_ptr;

  ptime_genoff_local = ptime_genoff;

  /* Copy generic offset to sum_generic_offset */
  sum_generic_offset_local = ptime_genoff->generic_offset;

  while ( ptime_genoff->reqd_base_genoff != FALSE )
  {
    /* Get pointer to subsys_base in consideration */
    ptime_genoff = time_genoff_get_pointer( ptime_genoff->subsys_base );

    /* Add generic offset of subsys_base to sum_generic_offset */
    sum_generic_offset_local += ptime_genoff->generic_offset;
  }

  /* Update the sum generic offset field*/
  ptime_genoff_local->sum_generic_offset = sum_generic_offset_local;
 
} /* time_genoff_cb_update_sum_gen_off */



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
)
{
	boolean time_genoff_base_reset_done = FALSE;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( time_genoff->init_func != NULL )
  {
    /* Call Initialization Sequence */
    time_genoff->init_func();
  }

  #ifdef FEATURE_TIME_SINGLE_MULTI_SERVER_PROC
  
  /* If persistent storage spec is updated */
  if ( time_genoff->per_storage_spec.initialized == TRUE )
  {
    /* Read generic offset from efs item file */
    if ( TIME_EFS_RW_STAT_OK != 
         time_efs_rw( time_genoff->per_storage_spec.f_name,
                      &(time_genoff->generic_offset),
                      TIME_EFS_RW_CMD_RD, time_genoff->bases_type  ))
    {
      MSG_1(MSG_SSID_ATS, MSG_LEGACY_ERROR,"time_genoff_init : Error reading EFS file, base: 0x%x", time_genoff->bases_type);
    }
  }

  #endif /* FEATURE_TIME_SINGLE_MULTI_SERVER_PROC */

  if ( time_genoff->bases_type == ATS_PSM )
  { 
    /* Mark time_genoff as being initialized */
    time_genoff->initialized = TRUE;
    
    return;
  }
  

    /* Reinitialize generic offsets if total time is coming out to be invalid or
           the PMIC RTC time is read as very small value during bootup, since
           it could mean that the target was reset & PMIC RTC was also started
           from 0. This is a temporary fix until PMIC team provides a better
           alternative to check the PMIC RTC reset status from time-services.
       */
  if((time_genoff->bases_type != ATS_RTC) 
	      && (ats_bases[ATS_RTC].generic_offset < PMIC_RTC_RESET_CHECK_TIME_THRESHOLD))
      {
        time_genoff->generic_offset = 0;
        time_genoff->sum_generic_offset = 0;

        /* Re-initialize sum_generic_offset in case total time was invalid */
            time_genoff_cb_update_sum_gen_off ( time_genoff, 0 );

        /* Update the global var to indicate which base got their offsets reset */
        time_genoff_efs_reset_bases |= 1<<(time_genoff->bases_type) ;

        /* Flag to update EFS file item later */
        time_genoff_base_reset_done = TRUE;

        /* Print messages for the offset reset done information */
        MSG_1(MSG_SSID_ATS, MSG_LEGACY_ERROR,"TIME_DEBUG: time_genoff_post_init: Generic offsets reset: base=0x%d",
                   time_genoff->bases_type);
      }


    if(time_genoff_base_reset_done == TRUE)
	 {
	   if ( TIME_EFS_RW_STAT_OK != 
                  time_efs_rw( time_genoff->per_storage_spec.f_name,
                            &( time_genoff->generic_offset ),
                               TIME_EFS_RW_CMD_WR, time_genoff->bases_type ))
        {
           MSG_1(MSG_SSID_ATS, MSG_LEGACY_ERROR,"time_genoff_init : Error writing into EFS file, base: 0x%x", time_genoff->bases_type);
        }
	 }


  /* Add callback for all offsets except ATS_RTC */
  if ( time_genoff->bases_type != ATS_RTC )
  {
    time_genoff_attach_cb( ATS_RTC, 
                           (time_genoff_t2_cb_type)&time_genoff_cb_update_sum_gen_off, 
                           time_genoff );
  }

  /* Initialize sum_generic_offset */
  time_genoff_cb_update_sum_gen_off ( (void*)time_genoff, 0 );

  /* Mark time_genoff as being initialized */
  time_genoff->initialized = TRUE;

 
} /* time_genoff_post_init */



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
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  return (time_genoff->valid);
}

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
)
{
  /* Pointer to 'base' in consideration */
  time_genoff_ptr ptime_genoff;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if ( base >= ATS_MAX ) 
    return 0;
 
  /* Get pointer to base in consideration */
  ptime_genoff = time_genoff_get_pointer(base);
  return ptime_genoff->generic_offset;
}


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
)
{
  if ( genoff_ptr == NULL ) 
    return 0;
 
  return genoff_ptr->generic_offset;
}


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
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Set Base Genoff */
  time_genoff->subsys_base = base;

  /* Set reqd_base_genoff to TRUE */
  time_genoff->reqd_base_genoff = TRUE;

} /* time_genoff_add_base_subsys */



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
)
{
  /* Pointer to Genoff in consideration */
  time_genoff_ptr ptime_genoff;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get pointer to base */
  ptime_genoff = time_genoff_get_pointer( base );

  if ( ptime_genoff != NULL )
  {
    /* Set proxy Genoff */
    ptime_genoff->proxy_base = base1;
  }

} /* time_genoff_add_base_subsys */



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
)
{
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff;
          
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get pointer to base_cb */
  time_genoff = time_genoff_get_pointer( base_cb );

  if ( time_genoff->number_of_cb_func < MAX_CB_FUNC )
  {
    /* Attach Callback */
    time_genoff->cb_func[ time_genoff->number_of_cb_func ] = cb;

    /* Keep record of registered base */
    time_genoff->registered_genoff[time_genoff->number_of_cb_func++] = registered_genoff_ptr;
  }

} /* time_genoff_attach_cb */


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
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Update remote_call_needed with value */
  time_genoff->remote_call_needed = value;

} /* time_genoff_set_remote_call_needed */



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
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Update allow_remote_updates with value */
  time_genoff->allow_remote_updates = value;

  /* Update remote_call_needed with value */
  time_genoff_set_remote_call_needed( time_genoff, value );

} /* time_genoff_set_allow_remote_updates */



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
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Return allow_remote_updates */
  return time_genoff->allow_remote_updates ;

} /* time_genoff_get_allow_remote_updates */




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
)
{
  #ifdef FEATURE_QMI_TIME_REMOTE_CLNT
  int rc;
  time_set_genoff_offset_req_msg_v01  time_req;
  time_set_genoff_offset_resp_msg_v01 time_resp;
  #endif

  #ifdef FEATURE_TIME_MULTI_SERVER_PROC

  /* Information to be updated on other proc */
  ats_cfg_event_info_s_type   * cfg_event_ptr = &cfg_event_info; 

  #endif /* FEATURE_TIME_MULTI_SERVER_PROC */
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifndef FEATURE_MULTIPROCESSOR
  return ;
  #elif defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_TIME_NO_RPC) 
  /* FEATURE_MULTIPROCESSOR && !FEATURE_TIME_NO_RPC */

  if ( time_genoff->remote_call_needed != TRUE )
    return;

  /* Check to control remote updates during intial updates to apps proc */
  if ( time_genoff->generic_offset == 0 )
    return;

  #ifdef FEATURE_TIME_MULTI_SERVER_PROC
  
  /* Update base type */
  cfg_event_ptr->ats_genoff_update.base_type = time_genoff->bases_type;
  
  /* Update offset for base */
  cfg_event_ptr->ats_genoff_update.offset = time_genoff->generic_offset;
  
  /* Notify and call remote call */
  ats_cfg_event_notify( ATS_CFG_EVENT_GENOFF_CHANGE, &cfg_event_info );

  #else /* FEATURE_TIME_MULTI_SERVER_PROC */
 
  time_remote_atom_genoff_set_generic_offset( time_genoff->bases_type, time_genoff->generic_offset );
  
  #endif /* FEATURE_TIME_MULTI_SERVER_PROC */
  #elif defined FEATURE_QMI_TIME_REMOTE_CLNT  
    /*decide on message id*/
    MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Setting time on modem..base=%d,offset=%d",
            time_genoff->bases_type, time_genoff->generic_offset);

    time_req.base = (time_service_base_enum_v01)time_genoff->bases_type;
    time_req.generic_offset = time_genoff->generic_offset;

    /*send the data over to modem*/
    rc = qmi_client_send_msg_sync(time_qmi_client_handle,
                                   QMI_TIME_GENOFF_SET_REQ_V01,
                                   &time_req,
                                   sizeof(time_req),
                                   &time_resp,
                                   sizeof(time_resp),
                                   1000);
    if(rc != QMI_NO_ERR) 
    {
      MSG(MSG_SSID_ATS, MSG_LEGACY_ERROR,"Could not set time on modem");
    }
  #endif /* FEATURE_MULTIPROCESSOR */

} /* time_genoff_set_remote_genoff */



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

void time_genoff_remote_through_time_ipc(void)
{
  
#if (defined FEATURE_TIME_MULTI_SERVER_PROC || defined FEATURE_QMI_TIME_REMOTE)

  /* Pointer to Genoff in consideration */
  time_genoff_ptr             ptime_genoff;

  rpc_struct                  *rpc_base_ptr;
      
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get base */
  rpc_base_ptr = ( rpc_struct * ) q_get( &rpc_q );
    
  while ( rpc_base_ptr != NULL )
  {
    /* Mark entry as not in queue */
    rpc_base_ptr->queue = NOT_IN_QUEUE;
        
    /* Get pointer to genoff */
    ptime_genoff= time_genoff_get_pointer( rpc_base_ptr->base );

    #ifdef FEATURE_QMI_TIME_REMOTE
    /* Initialize Function Pointers to eliminate dependencies */
    if ( !time_genoff_fps_initialized )
    {
      time_genoff_fps = time_fn_map_get_fp();
      time_genoff_fps_initialized++;
    }
    
    /* Send QMI notification if QMI is available */
    if ( time_genoff_fps->time_server_call_ind_fp != NULL )
    {
      MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Calling time_server_call_ind: base=%d,offset=%d",
                ptime_genoff->bases_type, ptime_genoff->generic_offset);
      time_genoff_fps->time_server_call_ind_fp(ptime_genoff->bases_type,
                                              ptime_genoff->generic_offset);
    }
    #else
    /* Set remote updates */
    time_genoff_set_remote_genoff( ptime_genoff );
    #endif

    /* Get base */
    rpc_base_ptr = ( rpc_struct * ) q_get( &rpc_q );

  } /* while ( rpc_base_ptr != NULL ) */

#endif /* FEATURE_TIME_MULTI_SERVER_PROC */

} /* time_genoff_remote_through_time_ipc */




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
)
{
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update initialization sequence */
  time_genoff->init_func = func;

} /* time_genoff_add_init_call */



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
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialze storage specs */
  time_genoff->per_storage_spec.initialized = TRUE;
  
  /* Update threshhold */
  time_genoff->per_storage_spec.threshold   = thold;
  
  /* Copy the name of efs item file */
  strlcpy( (time_genoff->per_storage_spec.f_name), name, 20 );

} /* time_genoff_updates_per_storage */




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
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( base < ATS_MAX );

  return &(ats_bases[base]) ;
     
} /* time_genoff_get_pointer */



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
)
{
  /* delta_ms between prev and new offset */
  int32           delta_ms;

  /* Iteration variable */
  uint8           iter;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( time_genoff->bases_type == ATS_PSM )
  {
    time_genoff->generic_offset = offset;

    /* Time is now valid since it's set */
    time_genoff->valid = TRUE;

    return;
  }
  
  /* Calculate delta_ms */
  delta_ms = offset - time_genoff->generic_offset;
  
  /* Set Generic OFfset */
  time_genoff->generic_offset = offset;

  /* Set sum_generic_offset */
  time_genoff_cb_update_sum_gen_off ( (void*)time_genoff, 0 );

  /* Time is now valid since it's set */
  time_genoff->valid = TRUE;

  /* Call callback functions */
  for( iter = 0; iter < time_genoff->number_of_cb_func; iter ++)
  {
    ( time_genoff->cb_func[iter] )( time_genoff->registered_genoff[iter], 
                            delta_ms );
  }

} /* time_genoff_set_generic_offset */



/*=============================================================================

FUNCTION TIME_GENOFF_GET_OPTIMIZED

DESCRIPTION
  Gets Genoff Time for the time base in consideration, only if time_genoff is 
  initialized

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void time_genoff_get_optimized
(
  /* Pointer to time_genoff_args_type with genoff_ptr in consideration */
  time_genoff_args_ptr         pargs
)
{

  /* Stores time */
  uint64                       temp_time;
  
  /* Pointer to genoff in consideration */
  time_genoff_ptr              ptime_genoff;
   
  /*to store the generic offset in a temp variable*/
  uint32     old_generic_offset = 0;

  /*temporary timestamp value*/
  time_type                    ts_val_temp; 

  /* This variable is used to erase TIME_GENOFF_TIMETICK_VALID from getting onto stack */
  volatile uint32 timetick_valid_flag = TIME_GENOFF_TIMETICK_VALID;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Get pointer to genoff */
  if( pargs->operation & TIME_GENOFF_OPR_EXTERNAL_MASK )
  {
    ptime_genoff = (time_genoff_ptr)pargs->ext_genoff_ptr;;
  }
  else
  {
    ptime_genoff = time_genoff_get_pointer( pargs->base );
  }
  
  /* If ptime_genoff is not initilazed */
  if ( ptime_genoff->initialized == FALSE )
  {
    qw_set( pargs->ts_val, 0, 0 );
    return;
  }

  do
  {    
    /*initialize the variable to zero. This is important
     since if do-while loop iterates more than once, we 
     need to start with a zero*/
    temp_time = 0;

    /*record the generic offset before we begin*/
    old_generic_offset = ptime_genoff->sum_generic_offset>>32;

	if ( pargs->dal_timetick_val_valid != timetick_valid_flag)
	{
	  /* Erase TIME_GENOFF_TIMETICK_VALID from getting onto stack.
         This will make sure there will not be any issues if clients
		 doesnt explicitly set dal_timetick_val_valid to 0 if they dont use it */
	  timetick_valid_flag = 0;
      /* Get time from sclk */
      time_sclk_get(ts_val_temp, (uint64)-1);
    }
    else
    {
      timetick_valid_flag = 0;
      time_sclk_get(ts_val_temp, pargs->dal_timetick_val);
    }

    /*convert it to msec */
    time_to_ms(ts_val_temp);

    /*copy it over in temp_time*/
    qw_equ_misaligned((void*)&temp_time, ts_val_temp);
  
    /* Copy the ts_val before converting to timestamp */
    temp_time = temp_time + ptime_genoff->sum_generic_offset ;    

  }while(FALSE==osal_atomic_compare_and_set(
        (osal_atomic_word_t *)&old_generic_offset,
                old_generic_offset,
                (ptime_genoff->sum_generic_offset)>>32));

  qw_equ_misaligned( (void*)pargs->ts_val, (void*)&temp_time );   
  time_conv_ms_to_timestamp(pargs->ts_val);
  
} /* time_genoff_get_optimized */



/*=============================================================================

FUNCTION TIME_GENOFF_GET

DESCRIPTION
  Gets Genoff Time for the time base in consideration, only if time_genoff is 
  initialized

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void time_genoff_get
(
  /* Pointer to time_genoff_args_type with genoff_ptr in consideration */
  time_genoff_args_ptr         pargs
)
{
  /* Temporary genoff argument to get time from base genoff*/
  time_genoff_args_struct_type base_genoff_args;

  /* Stores uptime */
  uint64                       uptime;
  
  /* Pointer to genoff in consideration */
  time_genoff_ptr              ptime_genoff;
  
  /* This variable is used to erase TIME_GENOFF_TIMETICK_VALID from getting onto stack */
  volatile uint32 timetick_valid_flag = TIME_GENOFF_TIMETICK_VALID;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Get pointer to genoff */
  if( pargs->operation & TIME_GENOFF_OPR_EXTERNAL_MASK )
  {
    ptime_genoff = (time_genoff_ptr)pargs->ext_genoff_ptr;;
  }
  else
  {
    ptime_genoff = time_genoff_get_pointer( pargs->base );
  }
  
  /* If ptime_genoff is not initilazed */
  if ( ptime_genoff->initialized == FALSE )
  {
    qw_set( pargs->ts_val, 0, 0 );
    return;
  }
  
  if ( ptime_genoff->reqd_base_genoff == FALSE )
  {
    if ( pargs->dal_timetick_val_valid != timetick_valid_flag )
    {
   /* Erase TIME_GENOFF_TIMETICK_VALID from getting onto stack.
      This will make sure there will not be any issues if clients
	  doesnt explicitly set dal_timetick_val_valid to 0 if they dont use it */
	  timetick_valid_flag = 0;
	  
      /* Get stime from Timetick64 */
      if ( pargs->base_source == TIMETICK_SCLK64 )  
      {
        uptime = timetick_sclk64_get();

        /* Reset the ts_val before converting to timestamp */
        memset(pargs->ts_val, 0, sizeof(time_type));

        timetick_sclk64_add_xo( pargs->ts_val, uptime );
      }
      /* Get time from time_sclk  */
      else if ( pargs->base_source == TIME_SCLK )  
      {
        /*  Get Time from sclk */
        time_sclk_get( pargs->ts_val, (uint64)-1 );
      }
    }
    else
    {
	  timetick_valid_flag = 0;
	  
      timetick_sclk64_add_xo(pargs->ts_val, pargs->dal_timetick_val); 
    }
  }
  else
  {
    /* Update base_genoff to retrieve ts_val */
    base_genoff_args.base        = ptime_genoff->subsys_base ;
    base_genoff_args.base_source = pargs->base_source;
    base_genoff_args.ts_val      = pargs->ts_val;
      
    /* Get Time from base Genoff */
    time_genoff_get( &base_genoff_args );
   }

  /* Add offset to timestamp */
  time_conv_add_offset_to_timestamp( pargs->ts_val, ptime_genoff->generic_offset );
  
} /* time_genoff_get */



/*=============================================================================

FUNCTION TIME_GENOFF_WRITE_TO_EFS

DESCRIPTION
  Checks and calls function to write the offset to efs.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void time_genoff_write_to_efs
(
  /* Pointer to genoff in consideration */
  time_genoff_ptr         ptime_genoff,

  /* Delta between Old and New Genoff time, in ms */
  int64                   delta_ms
)
{
  #ifdef FEATURE_TIME_SINGLE_MULTI_SERVER_PROC
  
  /* If persistent storage spec is updated and delta_ms is greater 
     then threshold */
  if ( ptime_genoff->per_storage_spec.initialized == TRUE 
       && delta_ms > ptime_genoff->per_storage_spec.threshold )
  {
    /* Write generic offset into efs item file */
    if ( TIME_EFS_RW_STAT_OK != 
         time_efs_rw( ptime_genoff->per_storage_spec.f_name,
                      &( ptime_genoff->generic_offset ),
              TIME_EFS_RW_CMD_WR, ptime_genoff->bases_type ))
    {
      MSG_1(MSG_SSID_ATS, MSG_LEGACY_ERROR,"time_genoff_init : Error writing into EFS file, base: 0x%x", ptime_genoff->bases_type);
    }
  }

  #endif /* FEATURE_TIME_SINGLE_MULTI_SERVER_PROC */
}

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
void time_genoff_shutdown_notification_fn( void )
{
  /* index for iterating all the ATS bases */
  uint32                   index;

  /* Generic offset pointer */
  time_genoff_ptr         ptime_genoff;
  
  /* Efs_* return size variable */
  int                            return_size;
  
  /* Absolute path of efs_item file */
  char                           file_name[48];

  /* keep only for debugging purpose */
  MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"time_genoff_shutdown_notification_fn");
  
  /* Initialize Fn Map */
  time_fn_map_efs_fp = time_fn_map_get_fp();

  if(time_fn_map_efs_fp->efs_put_fp)
   {
     /* Update the efs file entries for generic offsets for all the bases */
     for (index = 0 ; index < ATS_MAX  ; index++)
      {
        /* Get the pointer to time_base instance */
        ptime_genoff = time_genoff_get_pointer( (time_bases_type)index );
        
        if( ptime_genoff == NULL )
         {
           return ;
         }
        
        /* Attach absolute path to filename */
        snprintf( file_name , sizeof(file_name),
	       "/nvm/alpha/item_file/time/ats_%d", index );
	    
        /* Write into efs_item file */
        return_size = time_fn_map_efs_fp->efs_put_fp( file_name, &(ptime_genoff->generic_offset), ITEM_SIZE,
                                                    O_CREAT|O_AUTODIR, 0777);
        MSG_2(MSG_SSID_ATS, MSG_LEGACY_ERROR,"writing EFS file ret: 0x%x base: 0x%x", return_size,index);
		
	  }	
   }
 else
   MSG(MSG_SSID_ATS, MSG_LEGACY_ERROR,"EFS support is not present");	 
	 
} /* time_genoff_shutdown_notification_fn */


/*=============================================================================

FUNCTION TIME_GENOFF_REMOTE_OFFSET

DESCRIPTION
  Checks and calls function to remote offset

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void time_genoff_remote_offset
(
  /* Pointer to genoff in consideration */
  time_genoff_ptr         ptime_genoff,

  /* Delta between Old and New Genoff time, in ms */
  int64                   delta_ms
)
{
  if ((delta_ms > TIME_GENOFF_REMOTE_UPDATE_MS) || 
     (delta_ms < ((-1)*TIME_GENOFF_REMOTE_UPDATE_MS)))
  {
    #if(defined FEATURE_TIME_MULTI_SERVER_PROC || defined FEATURE_QMI_TIME_REMOTE)

    /* Allocate base */
    rpc_base[ptime_genoff->bases_type].base =  ptime_genoff->bases_type;
    
    if ( rpc_base[ptime_genoff->bases_type].queue == NOT_IN_QUEUE )
    {
      /* Mark entry as in queue */
      rpc_base[ptime_genoff->bases_type].queue = IN_QUEUE;

      /* Link roc_data to rpc_q */
      (void) q_link( &rpc_base[ptime_genoff->bases_type], 
             &rpc_base[ptime_genoff->bases_type].link );
    
      /* Enqueue node in rpc_q */
      q_put( &rpc_q, &rpc_base[ptime_genoff->bases_type].link);
         
      /* Set signal to TIME_IPC task for further processing */
      if ( time_ipc_qurt_sig_ptr != NULL )
      {
		qurt_signal_set(time_ipc_qurt_sig_ptr,TIME_IPC_GENOFF_UPDATE);
      }
      else
      {
        time_ipc_initialize_signal_object();
		qurt_signal_set(time_ipc_qurt_sig_ptr,TIME_IPC_GENOFF_UPDATE);
      }
    }

    /*update the remote modem processor if we have to*/
    /*if it is a set operation, and if the base was TOD, then call
     the below function to send over the time to the server modem*/
    #if(defined FEATURE_EXPORT_TIME_REMOTE_MM_FUSION || defined FEATURE_EXPORT_TIME_REMOTE_MM_FUSION_CLNT)
    if(ATS_TOD ==  ptime_genoff->bases_type && time_genoff_get_remote_modem_update()) 
    {
      MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"mm_debug_fusion:Queuing up request for remote modem update");
      if ( time_ipc_qurt_sig_ptr != NULL )
      {
		qurt_signal_set(time_ipc_qurt_sig_ptr,TIME_IPC_SEND_REMOTE_MODEM_UDPATE);
      }
      else
      {
        time_ipc_initialize_signal_object();
		qurt_signal_set(time_ipc_qurt_sig_ptr,TIME_IPC_SEND_REMOTE_MODEM_UDPATE);		
      }
    } /* End if(ATS_TOD ==  ptime_genoff->bases_type ) */

    if(ATS_USER ==  ptime_genoff->bases_type && time_genoff_get_remote_modem_update()) 
    {
      MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"mm_debug_fusion:Queuing up request for remote modem update");
      if ( time_ipc_qurt_sig_ptr != NULL )
      {
		qurt_signal_set(time_ipc_qurt_sig_ptr,TIME_IPC_SEND_REMOTE_USER_UDPATE);				
      }
      else
      {
        time_ipc_initialize_signal_object();
		qurt_signal_set(time_ipc_qurt_sig_ptr,TIME_IPC_SEND_REMOTE_USER_UDPATE);				
      }
    } /* End if(ATS_USER ==  ptime_genoff->bases_type ) */

    if(ATS_USER_TZ_DL ==  ptime_genoff->bases_type && time_genoff_get_remote_modem_update()) 
    {
      MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"mm_debug_fusion:Queuing up request for remote modem update");
      if ( time_ipc_qurt_sig_ptr != NULL )
      {
		qurt_signal_set(time_ipc_qurt_sig_ptr,TIME_IPC_SEND_REMOTE_USER_TZ_DL_UDPATE);
      }
      else
      {
        time_ipc_initialize_signal_object();
		qurt_signal_set(time_ipc_qurt_sig_ptr,TIME_IPC_SEND_REMOTE_USER_TZ_DL_UDPATE);				
      }
    } /* End if(ATS_USER_TZ_DL ==  ptime_genoff->bases_type ) */


    #endif

   #elif (defined IMAGE_APPS_PROC || (defined T_MDM9X00 && !defined FEATURE_QDSP6)|| defined FEATURE_QMI_TIME_REMOTE_CLNT)
   
   /* Update Remote Generic offset */
   time_genoff_set_remote_genoff( ptime_genoff );
   
   #endif /* IMAGE_APPS_PROC */
    
  }
}

/*=============================================================================

FUNCTION TIME_GENOFF_SEND_TOD_ERROR_NOTIFICATION

DESCRIPTION
  Sends F3 message and log packet whenever the delta in the last TOD
  vs. the TOD being set is greater than the threshold.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void time_genoff_send_tod_error_notification
(
  uint64 old_ms,
  uint64 new_ms,
  int64  delta_ms
)
{
  uint8 *log_ptr;

  if (delta_ms > 10 )
  {
    /*then send out the log packet*/
    /*populate the log params here*/
    time_delta_log.u_Version=(uint8)0;
    time_delta_log.old_ms = old_ms;
    time_delta_log.new_ms = new_ms;
    time_delta_log.delta = delta_ms;
    time_delta_log.qurt_tid = (uint32)qurt_thread_get_id();	
	qurt_thread_get_name(&time_delta_log.tcb_string[0],20);
    log_ptr = (uint8 *) log_alloc (LOG_TIME_TOD_ERROR_NOTIFY_C,
          sizeof(time_delta_report_log_type) + sizeof(log_hdr_type));

    /*Print the F3 error message, we can use timetick_get() since this wont be called often */
    MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR,"TOD: Delta from last TOD set = (%+ldms), task: %s, timetick_32: 0x%x",delta_ms, time_delta_log.tcb_string, timetick_get());

    if (log_ptr != NULL) 
    {
        memscpy((void*)(log_ptr + sizeof(log_hdr_type)), 
                sizeof(time_delta_report_log_type), 
                (void*)&(time_delta_log),  
                sizeof(time_delta_report_log_type));
        log_commit(log_ptr);
    }

  }
}

/*=============================================================================

FUNCTION TIME_GENOFF_SET_HELPER

DESCRIPTION
  Continuation of function time_genoff_set()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void time_genoff_set_helper
(
  /* Pointer to time_genoff_args_type with genoff_ptr in consideration */
  time_genoff_args_ptr         pargs,

  /* Old Genoff time, in ms */
  time_type       old_ts_val_ms
)
{

  /* Delta between Old and New Genoff time, in ms */
  int64           delta_ms;

  /* Iteration variable */
  uint8           iter;
  
  /* Pointer to genoff in consideration */
  time_genoff_ptr              ptime_genoff, ptime_genoff1,ptime_genoff2;

  uint64 new_ms = 0;
  uint64 old_ms = 0;

  /* Suppress compiler warning */
  time_genoff_1x_time_updated = time_genoff_1x_time_updated;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
  /* Get pointer to genoff */
  if( pargs->operation & TIME_GENOFF_OPR_EXTERNAL_MASK )
  {
    ptime_genoff = (time_genoff_ptr)pargs->ext_genoff_ptr;
  }
  else
  {
    ptime_genoff = time_genoff_get_pointer( pargs->base );
  }
  
  if ( ptime_genoff->initialized == FALSE )
  {
    return;
  }

  qw_equ_misaligned((void*)&new_ms,pargs->ts_val);
  qw_equ_misaligned((void*)&old_ms,old_ts_val_ms);
  delta_ms = new_ms - old_ms;

  if(delta_ms < 0 )
  {
    MSG_1(MSG_SSID_ATS, MSG_LEGACY_ERROR,"Trying to set a time in past. Difference in msec =%d ",delta_ms);
  }

  /* Calculate new Generic offset */
  ptime_genoff->generic_offset += delta_ms;  

  /* Call callback functions */
  for( iter = 0; iter < ptime_genoff->number_of_cb_func; iter ++)
  {
    ( ptime_genoff->cb_func[iter] )( ptime_genoff->registered_genoff[iter], 
                             delta_ms );
  }

  if (delta_ms < 0 )
  {
    delta_ms *= -1;
  }

  /* If base is TOD then call fn to print error message */
    if( ATS_TOD == pargs->base )
    {
    time_genoff_send_tod_error_notification(old_ms,new_ms,delta_ms);      
  }


  /* Set sum_generic_offset value */
  time_genoff_cb_update_sum_gen_off( (void*)ptime_genoff, 0 );

  /* Write offset to efs */
  time_genoff_write_to_efs( ptime_genoff, delta_ms );

  /* Remote offset */
  time_genoff_remote_offset( ptime_genoff, delta_ms );

  /*If 1x is setting time, then set the proxy base for ATS_3GPP to false. 
   This is because 1X has a more accurate notion of time and we do not want
   that time to be overwritten by 3GPP*/
  if( ATS_1X == pargs->base)
  {
     time_genoff_1x_time_updated=1;
     ptime_genoff2=time_genoff_get_pointer( ATS_3GPP );
     ptime_genoff2->proxy_base=ATS_INVALID;
  }

   /* Mark valid as TRUE */
   ptime_genoff->valid = TRUE;

  if ( ptime_genoff->proxy_base != ATS_INVALID )
  {
     /* Get pointer to genoff */
     ptime_genoff1 = time_genoff_get_pointer( ptime_genoff->proxy_base );

     /*If the proxy base was ATS_TOD then we need to store the params*/
     if(ATS_TOD == ptime_genoff->proxy_base) 
       {
       new_ms = ptime_genoff->generic_offset;
       old_ms = ptime_genoff1->generic_offset;
       delta_ms = new_ms - old_ms; 
       time_genoff_send_tod_error_notification(old_ms,new_ms,delta_ms);        
     }
     
     /* Copy ptime_genoff offset value to ptime_genoff1 */
     ptime_genoff1->generic_offset = ptime_genoff->generic_offset; 
     
     ptime_genoff1->sum_generic_offset = ptime_genoff->sum_generic_offset;

     /* Write offset to efs */
     time_genoff_write_to_efs( ptime_genoff1, delta_ms );

     /* Remote offset */
     time_genoff_remote_offset( ptime_genoff1, delta_ms );

     /* Mark valid as TRUE */
     ptime_genoff1->valid = TRUE;
  
  }

} /* time_genoff_set_hepler */



/*=============================================================================

FUNCTION TIME_GENOFF_SET

DESCRIPTION
  Sets Genoff Time for the time base in consideration, only if ptime_genoff is 
  initialized

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void time_genoff_set
(
  /* Pointer to time_genoff_args_type with genoff_ptr in consideration */
  time_genoff_args_ptr         pargs
)
{
  /* Old Genoff time, in ms */
  time_type       old_ts_val_ms;

  /* Arguments to be passed to base_genoff */
  time_genoff_args_struct_type temp_genoff_args;

  /* Operation needed to get the current time based on external or internal */
  time_genoff_opr_type         temp_opr;

  /* Pointer to genoff in consideration */
  time_genoff_ptr              ptime_genoff;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
  /* Get pointer to genoff */
  if( pargs->operation & TIME_GENOFF_OPR_EXTERNAL_MASK )
  {
    ptime_genoff = (time_genoff_ptr)pargs->ext_genoff_ptr;
    temp_opr = T_GET_EXT;    
    temp_genoff_args.ext_genoff_ptr = pargs->ext_genoff_ptr;
  }
  else
  {
    ptime_genoff = time_genoff_get_pointer( pargs->base );
    temp_opr = T_GET;
  }
  
  if ( ptime_genoff->initialized == FALSE )
  {
    return;
  }

  /* Get Current Time in Genoff in msecs */
  temp_genoff_args.base                     = pargs->base;
  temp_genoff_args.base_source              = pargs->base_source;
  temp_genoff_args.ts_val                   = old_ts_val_ms;
  temp_genoff_args.unit                     = TIME_MSEC;
  temp_genoff_args.operation                = temp_opr;
  temp_genoff_args.dal_timetick_val         = pargs->dal_timetick_val;
  temp_genoff_args.dal_timetick_val_valid   = pargs->dal_timetick_val_valid;
  time_genoff_opr( &temp_genoff_args );

  /* Continue setting in time */
  time_genoff_set_helper( pargs, old_ts_val_ms );

} /* time_genoff_set */


/*=============================================================================
 
FUNCTION timer_inaccuracy_set
 
DESCRIPTION
  Set inaccuracy value to ATS_PSM base.
 
DEPENDENCIES
  None
 
RETURN VALUE
  None
 
SIDE EFFECTS
  None

=============================================================================*/
static void timer_inaccuracy_set( time_genoff_args_ptr pargs )
{
  time_genoff_ptr ptime_genoff;
  int64 msecs = 0;
  int64 delta_ms = 0;
  int64 delta_write_to_efs = 0;

  ptime_genoff = time_genoff_get_pointer( pargs->base );

  if (!ptime_genoff->initialized)
  {
    MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"ATS_PSM not initialized");
    return;
  }

  qw_equ_misaligned((void*)&msecs, pargs->ts_val);

  delta_ms = msecs - ptime_genoff->generic_offset;
  ptime_genoff->generic_offset = msecs;

  /* Log the time set operation */
  #ifdef FEATURE_TIME_DEBUG
  time_genoff_set_logging(ptime_genoff,
						  ptime_genoff->generic_offset,
					      pargs->base);
  #endif /* FEATURE_TIME_DEBUG */

  delta_write_to_efs = ptime_genoff->generic_offset - time_genoff_last_efs_write_val;

  /* Write offset to efs */
  if ((delta_write_to_efs > ptime_genoff->per_storage_spec.threshold) || 
      (delta_write_to_efs < ((-1)*ptime_genoff->per_storage_spec.threshold)))
  {
    if ( TIME_EFS_RW_STAT_OK != time_efs_rw( ptime_genoff->per_storage_spec.f_name,
                         &( ptime_genoff->generic_offset ), TIME_EFS_RW_CMD_WR, ptime_genoff->bases_type ))
    {
      MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR,"Error writing into EFS file bases_type=0x%x  new_ms=0x%.8x%.8x",
            ptime_genoff->bases_type,
            (uint32)(ptime_genoff->generic_offset>>32),
            (uint32)(ptime_genoff->generic_offset & 0xFFFFFFFF));
    }
    else
 	 {
 	   /* Update the last written value in the efs file item */
 		time_genoff_last_efs_write_val = ptime_genoff->generic_offset ;
 	 }
  }

  /* Remote offset */
  time_genoff_remote_offset( ptime_genoff, delta_ms );
 
  /* Mark valid as TRUE */
  ptime_genoff->valid = TRUE;
}


/*=============================================================================
 
FUNCTION timer_inaccuracy_get
 
DESCRIPTION
  Get inaccuracy value from ATS_PSM base.
 
DEPENDENCIES
  None
 
RETURN VALUE
  None
 
SIDE EFFECTS
  None

=============================================================================*/
static void timer_inaccuracy_get(time_genoff_args_ptr pargs)
{
  time_genoff_ptr ptime_genoff;

  ptime_genoff = time_genoff_get_pointer( pargs->base );

  if (!ptime_genoff->initialized)
  {
    qw_set( pargs->ts_val, 0, 0 );
    return;
  }

  qw_equ_misaligned(pargs->ts_val, (void*)&ptime_genoff->generic_offset);
}



/*=============================================================================

FUNCTION TIME_GENOFF_OPR

DESCRIPTION
  Does time set/get operation on the basis of Inputs.

DEPENDENCIES
  Time to be set should never be less than uptime of the UE.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_opr
(
  /* Pointer to time_genoff_args_type with genoff_ptr in consideration */
  time_genoff_args_ptr         pargs
)
{
  /* Arguments to be passed to base_genoff */
  time_genoff_args_struct_type temp_genoff_args;

  /* Genoff time, in secs */
  time_type                    ts_val_secs;

  /* Genoff time in secs temp variable */
  uint32                       lsecs;

  /* Secure base to create */
  static int                   secure_base_number = 0;

  /* EFS item file name */
  char                         f_name[120];

  /* Pointer to ats_bases in consideration */
  time_genoff_ptr              ptime_genoff;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( pargs->base == ATS_PSM )
  {
    if ( pargs->operation & TIME_GENOFF_OPR_GET_MASK )  /* Get Operation */
    {
      timer_inaccuracy_get( pargs );
    }
    else if (pargs->operation & TIME_GENOFF_OPR_SET_MASK) /* Set Operation */
    {
      timer_inaccuracy_set( pargs );
    } 
    else
    {
      MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR,"Invalid pargs->operation=0x%.8x, base=0x%.8x from pargs=0x%.8x",pargs->operation,pargs->base,pargs);
    }

    return;
  }

  /* GET operation */
  if ( pargs->operation & TIME_GENOFF_OPR_GET_MASK )
  {

    /* unit of time */
    switch( pargs->unit )
    {
      case TIME_STAMP:
      {
        /* Get time in timestamp format */
        time_genoff_get_optimized( pargs );

    break;
      }
      case TIME_MSEC:
      {
        /* Get time in timestamp format */
        time_genoff_get_optimized( pargs );
    
    /* Convert time into msecs */
    time_conv_to_ms( pargs->ts_val );
    break;
      }
      case TIME_SECS:
      {
        /* Get time in timestamp format */
        time_genoff_get_optimized( pargs );

        /* Convert time into secs */
        lsecs = time_conv_to_sec( pargs->ts_val );

        /* Set the lower 32 bits as the value in seconds */
        qw_set(pargs->ts_val, 0, lsecs);
    break;
      }
      case TIME_JULIAN:
      {
        /* Get time in secs */
        temp_genoff_args.base                   = pargs->base;
        temp_genoff_args.base_source            = pargs->base_source;
        temp_genoff_args.ts_val                 = &ts_val_secs;
        temp_genoff_args.unit                   = TIME_SECS;
        if( TIME_GENOFF_OPR_EXTERNAL_MASK & pargs->operation )
        {
          temp_genoff_args.operation   = T_GET_EXT;
          temp_genoff_args.ext_genoff_ptr = pargs->ext_genoff_ptr;
        }
        else
        {
          temp_genoff_args.operation   = T_GET;
        }
        time_genoff_opr( &temp_genoff_args );

        lsecs = ts_val_secs[0];

        /* Convert time into julian */
        time_jul_from_secs( lsecs, pargs->ts_val);

    break;
      }
      case TIME_20MS_FRAME:
      {
        /* Get time in timestamp format */
    time_genoff_get( pargs );

    /* Convert time into 20ms frame time */
        time_conv_to_20ms_frame_time( pargs->ts_val );
    break;
      }
      default:
        MSG_1(MSG_SSID_ATS, MSG_LEGACY_ERROR,"Invalid time unit %d", pargs->unit);
        break;
    }
  }
  else if ( pargs->operation & TIME_GENOFF_OPR_SET_MASK ) /* SET operation */
  {
    if ( pargs->base == ATS_RTC )
      return;

    /* unit of time */
    switch( pargs->unit )
    {
      case TIME_STAMP:
      {
        /* convert time into msec format */
        time_conv_to_ms( pargs->ts_val );

        /* Set time in timestamp format */
        time_genoff_set( pargs );
    break;
      }
      case TIME_MSEC:
      {
        /* Set time in timestamp format */
        time_genoff_set( pargs );
    break;
      }
      case TIME_SECS:
      {
        uint64 ts64 = 0;
        void* old_ts_val_ptr = (void*)pargs->ts_val;

        /*do not assume that pargs->ts_val is aligned
         at 8-byte boundary*/
        qw_equ_misaligned((void*)&ts64,pargs->ts_val);

        /* convert time into msec format */
        ts64 = ts64 * 1000;

        /*set the pargs->ts_val to point to this
         new 64 bit variable*/
        pargs->ts_val = &ts64;
    
        /* Set time in timestamp format */
        time_genoff_set( pargs );

        /*restore the old pointer since ts64 will go
         out of scope*/
        pargs->ts_val = old_ts_val_ptr;
    break;
      }
      case TIME_JULIAN:
      {
        /* Convert time into julian */
          ts_val_secs[0] = time_jul_to_secs( pargs->ts_val );
        ts_val_secs[1] = 0;
        
          /* Set time in secs */
        temp_genoff_args.base                   = pargs->base;
        temp_genoff_args.base_source            = pargs->base_source;
        temp_genoff_args.ts_val                 = &ts_val_secs;
        temp_genoff_args.dal_timetick_val       = pargs->dal_timetick_val;
        temp_genoff_args.dal_timetick_val_valid = pargs->dal_timetick_val_valid;
        temp_genoff_args.unit                   = TIME_SECS;
        if( TIME_GENOFF_OPR_EXTERNAL_MASK & pargs->operation )
        {
          temp_genoff_args.operation   = T_SET_EXT;
          temp_genoff_args.ext_genoff_ptr = pargs->ext_genoff_ptr;
        }
        else
        {
          temp_genoff_args.operation   = T_SET;
        }
        time_genoff_opr( &temp_genoff_args );
        break;
      }
      default:
        MSG_1(MSG_SSID_ATS, MSG_LEGACY_ERROR,"Invalid time unit %d", pargs->unit);
        break;

    } /* End of switch statements */

  }
  else if ( pargs->operation & TIME_GENOFF_OPR_INIT_MASK ) 
  {
    /*
    1) time_genoff_pre_init
    2) time_genoff_add_init_func ( Only if init_func is needed  ) 
    3) time_genoff_add_base_subsys ( Only if base_subsys is needed ) 
    4) time_genoff_updates_per_storage ( Only on Modem Proc, If NV is not used )
    5) time_genoff_attach_cb ( Only if cb_func is needed )
    6) time_genoff_post_init 
    */ 

    /* If there is no pointer to an external structure, we will just return */
    if( pargs->ext_genoff_ptr == NULL )
    {
      return;
    }

    /* Increment the number of secure base to use */
    secure_base_number++;

    (void) snprintf( f_name,sizeof(f_name),"ats_secure_%d",secure_base_number );
    //ats_bases_init ( (time_bases_type)i, ATS_RTC, f_name);

    /* Get the pointer to time_base instance */
    ptime_genoff = (time_genoff_ptr)pargs->ext_genoff_ptr;

    /* Initialize ats_bases */
    // This won't work since we don't have a base...
    time_genoff_pre_init( ptime_genoff, ATS_PRIVATE );

    /* Add base subsys to ats_bases */
    time_genoff_add_base_subsys( ptime_genoff, ATS_RTC );

    #if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_TIME_NO_RPC)
    time_genoff_set_remote_call_needed( ptime_genoff, TRUE );
    #endif /* FEATURE_MULTIPROCESSOR && !FEATURE_TIME_NO_RPC */

    #ifndef IMAGE_APPS_PROC
    /* Initialize storage spec */
    time_genoff_updates_per_storage( ptime_genoff, f_name, 
        TIME_GENOFF_INTERNAL_UPDATE_THRESHOLD_MS );
    #endif /* IMAGE_APPS_PROC */

    /* Initialize ats_bases */
    time_genoff_post_init( ptime_genoff );



  }/* End init operation */

} /* time_genoff_opr */

/*=============================================================================

FUNCTION TIME_GENOFF_SET_SERVER_MODEM_TIME

DESCRIPTION
  This function sets the time on the remote modem. 
  If called from the server modem, it will call the callback function that the 
  client registered at bootup.
  If called from client, it will make an async call to the modem

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_genoff_set_server_modem_time
(
  time_bases_type base_to_be_updated
)
{

#ifdef FEATURE_EXPORT_TIME_REMOTE_MM_FUSION

  /*this feature is suppose to be only on modems*/
  #if((!defined T_MDM9X00 && defined IMAGE_MODEM_PROC) || defined FEATURE_QDSP6)

  /* Arguments to be sent to set time_rtc */
  time_genoff_args_struct_type    temp_genoff_args ;
  uint64                          msec=0;
  
  temp_genoff_args.base        = base_to_be_updated; 
  temp_genoff_args.base_source = TIME_SCLK;
  temp_genoff_args.ts_val      = (void *)(&msec);
  temp_genoff_args.unit        = TIME_MSEC;
  temp_genoff_args.operation   = T_GET;
  time_genoff_opr( &temp_genoff_args );

  /*increment the sequence number of the update. The corresponding 
   sequence number should be updated in time_remote_genoff_mm_cb()
   in time.c*/
  time_remote_update_inc_seq_num();
  MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"time_remote_update_seq_num: Remote modem update seq num =  %d",
            time_remote_update_get_seq_num());

  MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"remote modem update-base=%d,msec=0x%x %x",
            base_to_be_updated,(msec>>32)&0xFFFFFFFF,msec&0xFFFFFFFF);

  /*update the base and the offset info for the cfg_event_info var*/
  /*IMPORTANT !! - For updating the remote modem proc, we are not 
  sending the offset, but the "whole" time itself. */
  cfg_event_info.ats_genoff_update.offset = msec;
  cfg_event_info.ats_genoff_update.base_type = base_to_be_updated;
  ats_cfg_mm_event_notify(ATS_CFG_EVENT_GENOFF_CHANGE, &cfg_event_info );
  #endif

#elif (defined FEATURE_EXPORT_TIME_REMOTE_MM_FUSION_CLNT)
  /* Arguments to be sent to set time_rtc */
  time_genoff_args_struct_type    temp_genoff_args ;
     
    /* Time julian Type */
    time_julian_type                jul_val;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  temp_genoff_args.base        = base_to_be_updated; 
  temp_genoff_args.base_source = TIME_SCLK;
  temp_genoff_args.ts_val      = (void *)(&jul_val);
  temp_genoff_args.unit        = TIME_JULIAN;
  temp_genoff_args.operation   = T_GET;
  time_genoff_opr( &temp_genoff_args );

  /*increment the sequence number of the update. The corresponding 
   sequence number should be updated in time_remote_mm_set_julian()
   in time_remote_modem_mm_fusion.c*/
  time_remote_update_inc_seq_num();
  MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"time_remote_update_seq_num: Remote modem update seq num =  %d",
            time_remote_update_get_seq_num());

  if(ATS_TOD == base_to_be_updated)
  {
    /*send it over to the server modem*/  
    time_remote_mm_set_julian(&jul_val);
  }
  else if((ATS_USER == base_to_be_updated)
          || (ATS_USER_TZ_DL == base_to_be_updated))
  {
    time_remote_mm_set_time_bases(base_to_be_updated,&jul_val);
  }

  MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"mm_fusion_debug:Sending over time update to server modem");  
#endif
}

/*=============================================================================

FUNCTION TIME_GENOFF_SYNC_WITH_TIME_TOD

DESCRIPTION
  This function syncs is called from time_tod_set. The purpose of the 
  function is to sync the time maintained by time_tod module and the genoff
  module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_genoff_sync_with_tod
(
  time_type ts_val
)
{
  time_genoff_args_struct_type time_genoff_args;

  time_genoff_args.base        = ATS_TOD; 
  time_genoff_args.base_source = TIME_SCLK;
  time_genoff_args.ts_val      = ts_val;
  time_genoff_args.unit        = TIME_STAMP;
  time_genoff_args.operation   = T_SET;
  time_genoff_opr(&time_genoff_args);
}

/*=============================================================================

FUNCTION TIME_GENOFF_SET_REMOTE_MODEM_UPDATE

DESCRIPTION
  This function sets the static variable which is used to indicate whether
  the remote modem should be updated or not

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_genoff_set_remote_modem_update
(
  boolean update_remote_modem
)
{
  time_genoff_update_remote_modem = (uint8)update_remote_modem;
  MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"mm_fusion_debug:remote modem update set to %d",time_genoff_update_remote_modem);
}

/*=============================================================================

FUNCTION TIME_GENOFF_GET_REMOTE_MODEM_UPDATE

DESCRIPTION
  This function gets the static variable which is used to indicate whether
  the remote modem should be updated or not

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
boolean time_genoff_get_remote_modem_update
(
  void
)
{
  MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"mm_fusion_debug:remote modem update queried = %d",time_genoff_update_remote_modem);
  return (boolean)time_genoff_update_remote_modem; 
}


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
)
{
  time_genoff_leap_sec_info.leap_sec_valid = lp_sec.leap_sec_valid;
  time_genoff_leap_sec_info.leap_sec_value = lp_sec.leap_sec_value;
}

/*=============================================================================

FUNCTION TIME_GENOFF_GET_SYS_TIME_IN_SEC_WITH_LP_SEC

DESCRIPTION
  This function sets gets the system time alongwith leap second info

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
uint32 time_genoff_get_sys_time_in_sec_with_lp_sec
(
  void
)
{
  uint32 secs;


  secs = time_get_secs();

  /*if the leap sec info is valid, add the leap sec
   to system time*/
  if(time_genoff_leap_sec_info.leap_sec_valid) 
  {
    secs = secs + time_genoff_leap_sec_info.leap_sec_value;     
  }

  return secs;
}
#endif /*FEATURE_QMI_TIME_REMOTE_CLNT*/

