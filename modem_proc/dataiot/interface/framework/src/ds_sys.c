/******************************************************************************
  @file    ds_sys.c
  @brief   Data Service System Event Ioctl Configuration implementation

  DESCRIPTION
  This file contains implementation of Data Service System Configuration API
  for registering and processing events and Ioctl callbacks.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2020 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
  $Header: //components/rel/dataiot.mpss/2.0/interface/framework/src/ds_sys.c#4 $ $DateTime: 2023/05/09 08:36:53 $ $Author: pwbldsvc $ 

  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/11/20   sp      Added support for MSISDN info for VZW PCO requirements.
05//18     mks     First Version of ds_sys module
================================================================*/

/*-------------------------------------------------------------
                           INCLUDE FILES
----------------------------------------------------------------*/
#include "ds_headers.h"
#include "ds_crit_sect.h"
#include "ds_sys_ioctl.h"
#include "ds_sys_event.h"
#include "ds_sys_eventi.h"
#include "ds_fwki.h"
#include <stringl/stringl.h>
#include "queue.h"

/*==============================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE                

=================================================================*/

ds_crit_sect_type ds_sys_crit_section;

static ds_sys_tech_func_type 
		   ds_sys_ioctl_tech_func_type[DS_SYS_TECH_ALL];

/*Cache for all system technology configuration data*/
 static ds_sys_confi_tech_all   ds_conf_tech_all;

/*Cache for 3gpp configuration data*/
static ds_sys_confi_tech_3gpp  ds_conf_tech_3gpp;

/** 
  @brief This is sys event queue for all the callbacks registered for all the
         events
*/
q_type  ds_sys_eventi_q;

/*==================================================================
                        Private Function Prototypes                         
==================================================================*/

/*=================================================================
  FUNCTION  DS_SYS_EVENTI_INVOKE_CBACKS()

  DESCRIPTION
  @brief  This function invokes all callbacks for given event.

  PARAMETERS

  @param[in] tech_type            States Technology type
  @param[in] event                Name of event
  @param[in] event_info           Event information 
  
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===================================================================*/
void ds_sys_eventi_invoke_cbacks
(
  ds_sys_tech_enum_type                          tech_type,
  uint64                                              event,
  void                                          *event_info
)
{
  q_type                                       *event_q_ptr = NULL;
  void                                         *event_buf_ptr;
  void                                         *next_event_buf_ptr;
  ds_sys_eventi_buf_type                       *ds_sys_event_buf_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - -*/
  
  /*-----------------------------------------------------------------
   Make sure the callback invocation is protected under critical section to
   avoid invalid memory accesses
  -----------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&ds_sys_crit_section);
  event_q_ptr = &ds_sys_eventi_q;
 
  /*-----------------------------------------------------------------
    Traverse the queue of callbacks for this event
  -----------------------------------------------------------------*/
  event_buf_ptr = q_check(event_q_ptr);
  
  while (event_buf_ptr != NULL)
  {
    next_event_buf_ptr =
    q_next(event_q_ptr, &((q_generic_item_type *) event_buf_ptr)->link);

    ds_sys_event_buf_ptr = (ds_sys_eventi_buf_type *) event_buf_ptr;

    if ((ds_sys_event_buf_ptr->tech_type == tech_type ) && 
        (ds_sys_event_buf_ptr->event_mask & event))
    {
      /* 
         Only one of the two Callbacks would be called. Either the
         the callback event_cback_f_ex_ptr or event_cback_f_ptr
      */
      if(NULL != ds_sys_event_buf_ptr->event_cback_f_ptr)    
      {
        ds_sys_event_buf_ptr->event_cback_f_ptr
        (
          tech_type,
          event,
          event_info,
          ds_sys_event_buf_ptr->user_data_ptr
        );
      }   
    }
    event_buf_ptr = next_event_buf_ptr;
  } /* while (items in queue) */
  DS_LEAVE_CRIT_SECTION(&ds_sys_crit_section);
} /* ds_sys_eventi_invoke_cbacks() */

/*==================================================================
  FUNCTION  DS_SYS_EVENTI_CHECK_EVENT_CBACK_EXIST()

  DESCRIPTION
  @brief  This function check if already callback existe for the event.

  PARAMETERS

  @param[in] tech_type            States Technology type
  @param[in] event                Name of event
  @param[in] event_cback_f_ptr    Event function pointer 
   
  RETURN VALUE
    Boolean  TRUE or FALSE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

==================================================================*/
boolean ds_sys_eventi_check_event_cback_exist
(
  ds_sys_tech_enum_type				  tech_type,
  uint64                                  event,
  ds_sys_event_cback_f_ptr_type		  event_cback_f_ptr
)
{
  q_type                                 *event_q_ptr = NULL;
  void                                   *event_buf_ptr;
  void                                   *next_event_buf_ptr;
  ds_sys_eventi_buf_type                 *ds_sys_event_buf_ptr;
  uint64                         maskval =0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_MSG3(MSG_LEGACY_HIGH,"ds_sys_eventi_check_event_cback_exist(): event %d "
          "tech %d, cb 0x%p",
                           event, tech_type, event_cback_f_ptr);

  event_q_ptr = &ds_sys_eventi_q;
  
  /*-----------------------------------------------------------------
    Traverse the queue of callbacks for this event
  ----------------------------------------------------------------*/
  event_buf_ptr = q_check(event_q_ptr);
  while (event_buf_ptr != NULL)
  {
    ds_sys_event_buf_ptr = (ds_sys_eventi_buf_type *) event_buf_ptr;

    /*---------------------------------------------------------------
      if event callback is present in queue for event and tech
      return true
    ---------------------------------------------------------------*/
    if (ds_sys_event_buf_ptr->tech_type == tech_type && 
       ((event_cback_f_ptr != NULL && 
        ds_sys_event_buf_ptr->event_cback_f_ptr == event_cback_f_ptr)))
    {
      
      maskval = ds_sys_event_buf_ptr->event_mask|event;
      ds_sys_event_buf_ptr->event_mask = maskval;
      DS_MSG3(MSG_LEGACY_ERROR,"ds_sys_eventi_check_event_cback_exist():"
               "registration recieved for unregistered event event %d"
               "cache event %d 0x%p",
                event, ds_sys_event_buf_ptr->event_mask,
                event_cback_f_ptr);
      return TRUE;
    }
    next_event_buf_ptr =
    q_next(event_q_ptr, &((q_generic_item_type *) event_buf_ptr)->link);
    event_buf_ptr = next_event_buf_ptr;
  } /* while (items in queue) */
  return FALSE;
}

/*====================================================== ===========
  FUNCTION  DS_SYS_EVENTI_ALLOC_CBACK_BUF()

  DESCRIPTION
  @brief  This function alloctes a new callback for a event.

  PARAMETERS

  @param[in] tech_type            States Technology type
  @param[in] event_cback_f_ptr    Event function pointer 
  @param[in] user_data_ptr        User data pointer 
  @param[in] event_name           Event Name
   
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

==================================================================*/
void *ds_sys_eventi_alloc_cback_buf
(
  ds_sys_tech_enum_type             tech_type,
  ds_sys_event_cback_f_ptr_type     event_cback_f_ptr,
  void                             *user_data_ptr,
  uint64                                 event_name
)
{
  ds_sys_eventi_buf_type           *buf_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_ERROR,"ds_sys_eventi_alloc_cback_buf(): Allocating event"
                           " buffer" );
  if (event_cback_f_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_sys_eventi_alloc_cback_buf(): Callback param "
                            "is null");
    return NULL;
  }

  /*----------------------------------------------------------------
    Allocate a new event callback buffer.
  -----------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(buf_ptr, sizeof(ds_sys_eventi_buf_type), 
                           ds_sys_eventi_buf_type*);
  if (buf_ptr == NULL)
  {
    return buf_ptr;
  }
  memset(buf_ptr,0,sizeof(ds_sys_eventi_buf_type));
  /*----------------------------------------------- ----------------
    Initialize the event buffer
  -----------------------------------------------------------------*/
  (void) q_link(buf_ptr, &(buf_ptr->link));
  buf_ptr->tech_type            = tech_type;
  buf_ptr->event_cback_f_ptr    = event_cback_f_ptr;
  buf_ptr->user_data_ptr        = user_data_ptr;
  buf_ptr->event_q_ptr          = NULL;
  buf_ptr->event_mask           = event_name;
  return buf_ptr;

} /* ds_sys_eventi_alloc_cback_buf() */

/*==================================================================
  FUNCTION    DS_SYS_CHECK_3GPP_BIT_IS_SET()

  DESCRIPTION
  @brief  This function checks if the 3gpp event bit are set in the mask .

  PARAMETERS

  @param[in] mask          Mask to check 
   
  RETURN VALUE
    Boolean TRUE or FALSE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===================================================================*/
boolean ds_sys_check_3gpp_bit_is_set(uint64 mask)
{
  if((mask > DS_SYS_EVENT_3GPP_MIN_EX) && (mask < DS_SYS_EVENT_3GPP_MAX_EX))
  {
   return TRUE;
  }
 return FALSE;
}

/*==================================================================
  FUNCTION DS_SYS_  CHECK_SYSTEM_BIT_IS_SET()

  DESCRIPTION
  @brief  This function checks if the system event bits are set in the mask.

  PARAMETERS

  @param[in] mask                 Mask to check 
   
  RETURN VALUE
    Boolean TRUE or FALSE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===================================================================*/
boolean ds_sys_check_system_bit_is_set(uint64 mask)
{
  if((mask > DS_SYS_EVENT_COMMON_MIN_EX) &&(mask < DS_SYS_EVENT_COMMON_MAX_EX))
  {
   return TRUE;
  }
 return FALSE;
}

/*==================================================================
  FUNCTION  DS_SYS_CHECK_AND_ALLOC_EVENT()

  DESCRIPTION
  @brief  This function checks and alloctes a new event callback if not 
           already present.

  PARAMETERS

  @param[in] mask                 Mask to check 
  @param[in] tech_type            States Technology type
  @param[in] event_cback_f_ptr    Event function pointer 
  @param[in] user_data_ptr        User data pointer 
  @param[in] dss_errno            errno to be filled
  
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===================================================================*/
void ds_sys_check_and_alloc_event
(
 uint64                                     mask,
 ds_sys_tech_enum_type                      tech_type, 
 ds_sys_event_cback_f_ptr_type              event_cback_f_ptr,
 void                                      *user_data_ptr,
 int16                                     *dss_errno
)
{ 
  void                                 *buf;  
  ds_sys_eventi_buf_type               *buf_ptr;  
  boolean                               event_cb_exist;
  ds_sys_system_status_type            *system_status_ex_val =NULL;
  ds_sys_event_msisdn_info_type        *msisdn_info_val = NULL;
  ds_sys_3gpp_roaming_status_type       roaming_status;
  ds_sys_roaming_disallowed_info_type  *roaming_disallowed_info =NULL;
  int                         i =0;
/*--------------------------------------------------------------------------------------*/
  
  DS_ENTER_CRIT_SECTION(&ds_sys_crit_section);
  do	
  {
     event_cb_exist = ds_sys_eventi_check_event_cback_exist
                      (
                        tech_type, 
                        mask, 
                        event_cback_f_ptr
                      );
  
     if (event_cb_exist) 
     {
       *dss_errno = DS_EEOF;
        DS_MSG2(MSG_LEGACY_ERROR,"ds_sys_check_and_alloc_event():"
                "Event CB already existing for "
                "event %d and tech %d", mask, tech_type);
        break;
     } 
     buf = ds_sys_eventi_alloc_cback_buf
           (
             tech_type, 
             event_cback_f_ptr, 		   
             user_data_ptr,
             mask
           );
      if (NULL == buf)
      {
         *dss_errno = DS_NOMEMORY;
          DS_MSG1(MSG_LEGACY_ERROR,"ds_sys_check_and_alloc_event():"
                  "Err : No memory %d", *dss_errno);
          goto bail;
     
      }
      buf_ptr = (ds_sys_eventi_buf_type*)buf;
      
  /*-------------------------------------------------------------------------
     Make sure that this is not already queued - if not mark it as such
  -------------------------------------------------------------------------*/
   if (Q_ALREADY_QUEUED(&(buf_ptr->link)))
   {
    *dss_errno = DS_EINVAL;
     DS_MSG2(MSG_LEGACY_ERROR,"ds_sys_check_and_alloc_event():"
             "Event Callback is already queued for"
             "event %d tech %d", mask, tech_type);
     goto bail;
   }
 /*------------------------------------------------------------------
  As the contents of the buffer are supposed to be set by ds_sys
  functions it should NEVER have a NULL callback
 -------------------------------------------------------------------*/
  if (buf_ptr->event_cback_f_ptr == NULL)
  {
    *dss_errno = DS_EFAULT;
    DS_MSG2(MSG_LEGACY_ERROR,"ds_sys_check_and_alloc_event():"
            "Event Callback is null event %d "
            "tech %d", mask, tech_type);
     goto bail;
  }

    buf_ptr->event_q_ptr = &ds_sys_eventi_q;
    q_put(&ds_sys_eventi_q, &(buf_ptr->link));
  }while(0);

  /*----------------------------------------------------------------
   For DS_SYS_EVENT_SYSTEM_STATUS we want to make sure that if 
   system conf changes between user registers 
   for event, we want to make sure that user doesnt miss the
   indication if system conf changed while 
   user is registering for the event
  ------------------------------------------------ ---------------*/
  
  if (mask & DS_SYS_EVENT_SYSTEM_STATUS)
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(system_status_ex_val, 
                             sizeof(ds_sys_system_status_type),
                              ds_sys_system_status_type*);
   
    if (NULL == system_status_ex_val)
    {
      *dss_errno = DS_NOMEMORY;
       goto bail;
    }
    memset(system_status_ex_val, 0, sizeof(ds_sys_system_status_type));
  
    DS_MSG1(MSG_LEGACY_HIGH,"ds_sys_check_and_alloc_event(): Retrieving  val"
            "for DS_SYS_EVENT_SYSTEM_STATUS tech type %d",
                    tech_type );
    
    /* If the cache is NULL, set apn and sys to zero. If not then load 
       the data from cache */
       if (NULL == ds_conf_tech_all.system_status_ex_ptr)
       {
         DS_MSG0(MSG_LEGACY_HIGH,"ds_sys_check_and_alloc_event() "
                         "sys status ex cache ptr is NULL");
         system_status_ex_val->num_avail_apns = 0;
         system_status_ex_val->num_avail_sys  = 0;
       }
       else
       {
         /*ds_sys_conf_update_from_sys_status_ex_cache
         (
           system_status_ex_val, 
           ds_conf_tech_all.system_status_ex_ptr
         );*/
         memscpy(system_status_ex_val,sizeof(ds_sys_system_status_type),
                 ds_conf_tech_all.system_status_ex_ptr,
                 sizeof(ds_sys_system_status_type));
       } 
       event_cback_f_ptr(tech_type,
                         DS_SYS_EVENT_SYSTEM_STATUS,
                         system_status_ex_val,
                         user_data_ptr);

     DS_SYSTEM_HEAP_MEM_FREE(system_status_ex_val);

    }
   if(mask & DS_SYS_EVENT_3GPP_ROAMING_STATUS)
   {
     roaming_status.roaming_status = ds_conf_tech_3gpp.\
                                     roaming_status.roaming_status;

     event_cback_f_ptr (tech_type,
                        DS_SYS_EVENT_3GPP_ROAMING_STATUS,
                       &roaming_status,
                        user_data_ptr);
   }
   if(mask & DS_SYS_EVENT_3GPP_ROAMING_DISALLOWED_INFO_EX)
   {
      DS_SYSTEM_HEAP_MEM_ALLOC(roaming_disallowed_info, 
                               sizeof(ds_sys_roaming_disallowed_info_type),
                               ds_sys_roaming_disallowed_info_type*);
   
    if (NULL == roaming_disallowed_info)
    {
       *dss_errno = DS_NOMEMORY;
        goto bail;
    }
     memset(roaming_disallowed_info, 0, 
            sizeof(ds_sys_roaming_disallowed_info_type));
    /* If the cache is NULL, set apn and sys to zero. If not then load 
       the data from cache */
     if (NULL == ds_conf_tech_3gpp.roaming_disallowed_info_ptr)
      { 
       DS_MSG0(MSG_LEGACY_HIGH,"ds_sys_check_and_alloc_event() "
               "roaming disallowed info cache is NULL");
        roaming_disallowed_info->is_roaming_disallowed = FALSE; 
	   roaming_disallowed_info->num_apn_names =0;
     }
     else
     {
        roaming_disallowed_info->is_roaming_disallowed =
          ds_conf_tech_3gpp.roaming_disallowed_info_ptr->is_roaming_disallowed;

        roaming_disallowed_info->num_apn_names = 
          ds_conf_tech_3gpp.roaming_disallowed_info_ptr->num_apn_names;
 
      for(i =0;i<roaming_disallowed_info->num_apn_names;i++)
      {
          memscpy(roaming_disallowed_info->apn_list[i].apn_name,
                  DS_SYS_MAX_APN_LEN, 
                  ds_conf_tech_3gpp.roaming_disallowed_info_ptr->\
                  apn_list[i].apn_name, 
                  DS_SYS_MAX_APN_LEN);
        } 
     }
    event_cback_f_ptr (tech_type,
                       DS_SYS_EVENT_3GPP_ROAMING_DISALLOWED_INFO_EX,
                       roaming_disallowed_info,
                       user_data_ptr);

     DS_SYSTEM_HEAP_MEM_FREE(roaming_disallowed_info);
   }
  if (mask & DS_SYS_EVENT_APN_MSISDN_INFO_CHANGE)
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(msisdn_info_val, 
                             sizeof(ds_sys_event_msisdn_info_type),
                             ds_sys_event_msisdn_info_type*);
    if (NULL == msisdn_info_val)
    {
      *dss_errno = DS_NOMEMORY;
       goto bail;
    }
    memset(msisdn_info_val, 0, sizeof(ds_sys_event_msisdn_info_type));
    if(ds_conf_tech_all.msisdn_info_ptr != NULL)
    {
      if((strncmp((char*)user_data_ptr,
                   ds_conf_tech_all.msisdn_info_ptr->apn_name,DS_SYS_MAX_APN_LEN) == 0)
                   || (strncmp((char*)user_data_ptr,
                   ds_conf_tech_all.msisdn_info_ptr->resolved_apn_name, DS_SYS_MAX_APN_LEN) == 0) )
      {
        memscpy(msisdn_info_val,sizeof(ds_sys_event_msisdn_info_type),
                 ds_conf_tech_all.msisdn_info_ptr,
                 sizeof(ds_sys_event_msisdn_info_type));
        event_cback_f_ptr(tech_type,
                         DS_SYS_EVENT_APN_MSISDN_INFO_CHANGE,
                         msisdn_info_val,
                         user_data_ptr);
      }
    }
    DS_SYSTEM_HEAP_MEM_FREE(msisdn_info_val);
  }
  
  bail:
 DS_LEAVE_CRIT_SECTION(&ds_sys_crit_section);
 return ;  
}

/*==================================================================
  FUNCTION  DS_SYS_CHECK_AND_DEALLOC_EVENT()

  DESCRIPTION
   @brief  This function checks and deletes the event function  
           ptr from the global queue.

  PARAMETERS

  @param[in] tech_type            States Technology type  
  @param[in] mask                 Mask to check 
  @param[in] event_cback_f_ptr    Event function pointer 
  @param[in] user_data_ptr        User data pointer 
  @param[in] dss_errno            errno to be filled	
  
  RETURN VALUE
  @return  0                      Success.    

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

==================================================================*/

int ds_sys_check_and_dealloc_event
(
  ds_sys_tech_enum_type                   tech_type,  
  ds_sys_event_cback_f_ptr_type           event_cback_f_ptr,
  uint64                                  mask,
  int16                                  *dss_errno
)
{
  q_type                                 *event_q_ptr = NULL;  
  void                                   *event_buf_ptr;
  void                                   *next_event_buf_ptr;
  ds_sys_eventi_buf_type                 *ds_sys_event_buf_ptr = NULL;  
  uint64                                  maskval;
/*----------------------------------------------------------------*/
  
  DS_ENTER_CRIT_SECTION(&ds_sys_crit_section);
 
  do
  {
   event_q_ptr = &ds_sys_eventi_q;
/*------------------------------------------------------- ---------
  Traverse the queue of callbacks for this event mask
----------------------------------------------------- -----------*/
  event_buf_ptr = q_check(event_q_ptr);
  while (event_buf_ptr != NULL)
  {
    next_event_buf_ptr =
    q_next(event_q_ptr, &((q_generic_item_type *) event_buf_ptr)->link);

    ds_sys_event_buf_ptr = (ds_sys_eventi_buf_type *) event_buf_ptr;

    if (ds_sys_event_buf_ptr->tech_type == tech_type)
    {
      if(event_cback_f_ptr != NULL && 
         ds_sys_event_buf_ptr->event_cback_f_ptr == event_cback_f_ptr)
      {
        if(ds_sys_event_buf_ptr->event_mask == mask)
        {
          DS_MSG3(MSG_LEGACY_ERROR,"ds_sys_check_and_dealloc_event,"
                  "both masks are same deregistering the events"
                  "ds_sys_event_buf_ptr->event_mask %d , event %d cb 0x%p",
                   ds_sys_event_buf_ptr->event_mask,mask,event_cback_f_ptr);
          break;
        }
        else
        {
         //Unset the bits recieved in the dereg mask
          maskval = (ds_sys_event_buf_ptr->event_mask & mask);
          ds_sys_event_buf_ptr->event_mask =
                         (ds_sys_event_buf_ptr->event_mask^maskval);
              DS_LEAVE_CRIT_SECTION(&ds_sys_crit_section);
          return 0;
        }
      }
    }
    event_buf_ptr = next_event_buf_ptr;
  } /* while (items in queue) */

 if (event_buf_ptr == NULL || ds_sys_event_buf_ptr == NULL)
 {
   *dss_errno = DS_EFAULT;
    DS_MSG2(MSG_LEGACY_ERROR,"ds_sys_check_and_dealloc_event():"
            "cb 0x%p cb_ex 0x%p for event %d" 
            "not found", event_cback_f_ptr, mask);
   
   DS_LEAVE_CRIT_SECTION(&ds_sys_crit_section);
   return -1;
 }

/*------------------------------------------------------------------
  make sure that this event is on a queue - if not mark it as such
------------------------------------------------------------------*/
 if (!Q_ALREADY_QUEUED(&(ds_sys_event_buf_ptr->link)))
 {
   *dss_errno = DS_EINVAL;
   DS_MSG1(MSG_LEGACY_ERROR,"ds_sys_check_and_dealloc_event():"
                            "Event %d not present in queue",
                             mask);
   DS_LEAVE_CRIT_SECTION(&ds_sys_crit_section);
   return -1;
 }

/*------------------------------------------------------------------
  make sure we have a valid queue to remove from - if so use the queue
  function to remove the item from the queue.
-------------------------------------------------------------------*/
    #ifdef FEATURE_Q_NO_SELF_QPTR
  q_delete(event_q_ptr, &(ds_sys_event_buf_ptr->link));
    #else
  q_delete(&(ds_sys_event_buf_ptr->link));
    #endif

  ds_sys_eventi_free_cback_buf((void *)event_buf_ptr);
 }while(0);
 
 DS_LEAVE_CRIT_SECTION(&ds_sys_crit_section); 
 return 0;
}

/*==================================================================
  FUNCTION  DS_SYS_EVENTI_FREE_CBACK_BUF()

  DESCRIPTION
   @brief  This function frees the event buffer from heap.

  PARAMETERS

   @param[in] buf            Pointer to the buffer to be freed
  
  RETURN VALUE
   None
  
  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===================================================================*/
void ds_sys_eventi_free_cback_buf
(
  void * buf
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG1(MSG_LEGACY_HIGH,"ds_sys_eventi_free_cback_buf(): buf 0x%p", buf);

  if (buf == NULL)
  {
    DS_ERROR_FATAL("ds_sys_eventi_free_cback_buf():"
                   "Buffer in input parameter is null");
    return;
  }
  /*-----------------------------------------------------------------
    Validate the buffer that is passed in not already on a queue
  -------------------------------------------------- --------------*/
  if (Q_ALREADY_QUEUED(&(((ds_sys_eventi_buf_type*)buf)->link)))
  {
    DS_ERROR_FATAL("ds_sys_eventi_free_cback_buf():"
                   "Buffer passed in already in queue");
    return;
  }
  /*----------------------------------------------------------------
    Invalidate the buffer so that the caller cannot use it without
    explicitly allcating it again.
  -----------------------------------------------------------------*/
  ((ds_sys_eventi_buf_type *) buf)->event_cback_f_ptr = NULL;
  DS_SYSTEM_HEAP_MEM_FREE(buf);
} /* ps_sys_eventi_free_cback_buf() */

/*================================================= ================
  FUNCTION  DS_SYS_EVENTI_CBACK_CMD_HANDLER()

  DESCRIPTION
   @brief  This function is the command handler for all events and 
             registered with the ds task

  PARAMETERS

   @param[in] cmd		   Pointer to the ds command buffer
  
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None

==========================================================================*/
void ds_sys_eventi_cback_cmd_handler
(  
  ds_command_type    *cmd
)
{
  ds_sys_eventi_cmd_type          * cmd_event_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - -*/
  
  if (NULL == cmd || DS_CMD_SYS_EVENT_HANDLER != cmd->hdr.cmd_id)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_sys_eventi_cback_cmd_handler(): Invalid args:"
                            "cmd %d", cmd);
    DS_ASSERT(0);
  return;
  }

  cmd_event_info = (ds_sys_eventi_cmd_type *)cmd->cmd_payload_ptr;;

  ds_sys_eventi_invoke_cbacks(cmd_event_info->tech_type, 
                                cmd_event_info->event, 
                                cmd_event_info->event_info
                             );

  DS_SYSTEM_HEAP_MEM_FREE(cmd_event_info->event_info);

  DS_MSG1(MSG_LEGACY_LOW,"Processed cmd %d", cmd->hdr.cmd_id);

} /*ds_sys_eventi_cback_cmd_handler*/


/*================================================================
  FUNCTION  DS_SYS_COMMON_IOCTL_HDLR()

  DESCRIPTION
   @brief  This function handles all the common ioctls.

  PARAMETERS

   @param[in] ioctl_name	 Ioctl name
   @param[in] arg_val_ptr	 Pointer to the ioctl result buffer	   
   @param[in] dss_errno      Pointer to the dss_errno
  
  RETURN VALUE
  @return  0                 Success.    

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===================================================================*/

int16 ds_sys_common_ioctl_hdlr
(
  ds_sys_ioctl_enum_type           ioctl_name,
  void                            *arg_val_ptr,
  int16                           *dss_errno
)
{
  ds_sys_system_status_type       *sys_status = NULL;
/*------------------------------------------------------------*/
  if(arg_val_ptr ==NULL)
  {
     DS_MSG1(MSG_LEGACY_ERROR,"ds_sys_common_ioctl_hdlr(): "
                              "caller %d arg_val_ptr is NULL", ioctl_name);
    *dss_errno = DS_EINVAL;
     return -1;
  }
 
  switch(ioctl_name)
  {
   case DS_SYS_IOCTL_SYS_STATUS:
   {
     sys_status = (ds_sys_system_status_type *)arg_val_ptr;
     if(NULL != ds_conf_tech_all.system_status_ex_ptr)
     {
       memscpy(sys_status ,sizeof(ds_sys_system_status_type),
               ds_conf_tech_all.system_status_ex_ptr,
               sizeof(ds_sys_system_status_type));
     }
     else
     {
       DS_MSG1(MSG_LEGACY_ERROR,"ds_sys_common_ioctl_hdlr(): "
                                "caller %d No info is available cache is NULL",
                                ioctl_name);
       return 0;
     }
    break;
   }
   case DS_SYS_IOCTL_GET_GLOBAL_STATS:
   {
      DS_MSG1(MSG_LEGACY_HIGH,"ds_sys_common_ioctl_hdlr(): ioctl for "
              "DS_SYS_IOCTL_GET_GLOBAL_STATS %d",DS_SYS_IOCTL_GET_GLOBAL_STATS);
     ds_fwk_get_global_stats((ds_sys_ioctl_global_stats_type *)arg_val_ptr);
     break;
    }    
   default:
    break;
  }
  return 0;
}

/*==================================================================
  FUNCTION  DS_SYS_EVENT_IND()

  DESCRIPTION
   @brief  This function is called when an registerd event occurs..

  PARAMETERS

   @param[in] tech_type            States Technology type  
   @param[in] mask                 Mask to check 
   @param[in] event_info           Event info data 
  
  RETURN VALUE
  @return  0                        Success.    

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

=================================================================*/
int16 ds_sys_event_ind
(
  ds_sys_tech_enum_type                tech_type,
  ds_sys_event_enum_type               event_name,
  void                                *event_info
)
{
  ds_sys_system_status_type             *system_status_ex_ptr;
  int                                    ret_val;
  ds_sys_eventi_cmd_type                *cmd_data_info;
  boolean                                invoke_cb = FALSE;
  uint32                                 cnt_pdn;
  uint32                                 cnt_network;
  ds_sys_eventi_info_type               *sys_evt_info_ptr;
  ds_sys_3gpp_roaming_status_type       *roaming_status_ptr;
  ds_sys_roaming_disallowed_info_type   *roaming_disallowed_info_ptr;
  ds_sys_pdn_throttle_info_type         *pdn_throttle_info;
  ds_sys_event_msisdn_info_type         *msisdn_info_ptr;
  //ds_sys_3gpp_ul_data_delivery_ind_info_type        *ul_data_delivery_info_ptr;
  int                                    i =0;  
  uint32                                 num_bytes = 0;
  ds_command_type                       *cmd_ptr =NULL;
  uint64                                 mask=0;
  void                                  *event_buf_ptr = NULL;
  ds_sys_eventi_buf_type                *ds_sys_event_buf_ptr;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG2(MSG_LEGACY_HIGH,"ds_sys_event_ind(): event  %d tech %d",
          event_name, tech_type);

  DS_SYSTEM_HEAP_MEM_ALLOC(sys_evt_info_ptr, sizeof(ds_sys_eventi_info_type),   
                           ds_sys_eventi_info_type*);      
   if (NULL == sys_evt_info_ptr)
   {
       return -1;
   }
  memset(sys_evt_info_ptr, 0,sizeof(ds_sys_eventi_info_type));
  switch(event_name)
  {
    case DS_SYS_SYSTEM_STATUS_EV: 
    { 
      memscpy(sys_evt_info_ptr,
             sizeof(ds_sys_system_status_type),
             (ds_sys_system_status_type *)event_info,
             sizeof(ds_sys_system_status_type));
  
      system_status_ex_ptr = (ds_sys_system_status_type *) sys_evt_info_ptr;

      DS_MSG2(MSG_LEGACY_ERROR,"ds_sys_event_ind :num of rats %d num of pdns %d", 
            system_status_ex_ptr->num_avail_sys,
            system_status_ex_ptr->num_avail_apns);
  
      for (cnt_network=0; cnt_network < system_status_ex_ptr->num_avail_sys; 
           cnt_network++)
      {
        DS_MSG3(MSG_LEGACY_HIGH,"ds_sys_event_ind:"
               "technology %d rat value %d so mask 0x%x",
                system_status_ex_ptr->avail_sys[cnt_network].technology,
                system_status_ex_ptr->avail_sys[cnt_network].rat_value,
                system_status_ex_ptr->avail_sys[cnt_network].so_mask);
      }
  
      for (cnt_pdn=0; cnt_pdn < system_status_ex_ptr->num_avail_apns; 
           cnt_pdn++)
      {
        DS_MSG2(MSG_LEGACY_ERROR,"ds_sys_event_ind pdn name %s"
                "available sys for pdn %d",
                 system_status_ex_ptr->apn_pref_sys_info[cnt_pdn].apn_name,
                 system_status_ex_ptr->apn_pref_sys_info[cnt_pdn].num_avail_sys);
  
        if(system_status_ex_ptr->apn_pref_sys_info[cnt_pdn].num_avail_sys > 0)
        {
          DS_MSG3(MSG_LEGACY_ERROR,"ds_sys_event_ind pdn name %s "
                  "pref tech %d pref rat %d pref so mask 0x%x",
                   system_status_ex_ptr->apn_pref_sys_info[cnt_pdn].\
                   avail_sys[0].technology,
                   system_status_ex_ptr->apn_pref_sys_info[cnt_pdn].\
                   avail_sys[0].rat_value,
                   system_status_ex_ptr->apn_pref_sys_info[cnt_pdn].\
                   avail_sys[0].so_mask);
         }
       }   
       if (NULL == ds_conf_tech_all.system_status_ex_ptr)
       {
         DS_SYSTEM_HEAP_MEM_ALLOC(ds_conf_tech_all.system_status_ex_ptr, 
                                   sizeof(ds_sys_system_status_type),
                                   ds_sys_system_status_type*);
         if (NULL == ds_conf_tech_all.system_status_ex_ptr)
         {
           goto bail;
         }
       }

       memset(ds_conf_tech_all.system_status_ex_ptr, 
	     0, 
             sizeof(ds_sys_system_status_type)
             );
      
       memscpy(ds_conf_tech_all.system_status_ex_ptr,
               sizeof(ds_sys_system_status_type),
                event_info,sizeof(ds_sys_system_status_type));
       mask = DS_SYS_EVENT_SYSTEM_STATUS;
       invoke_cb = TRUE;	
       break;
   }
   case DS_SYS_3GPP_ROAMING_DISALLOWED_INFO_EV:
   {
      memscpy(sys_evt_info_ptr,
              sizeof(ds_sys_roaming_disallowed_info_type),
              (ds_sys_roaming_disallowed_info_type *)event_info,
              sizeof(ds_sys_roaming_disallowed_info_type));
  
    /* compare the cache and user input */
      roaming_disallowed_info_ptr  = (ds_sys_roaming_disallowed_info_type*)
                                      event_info;
    /* calculate the size stored in cache */
      num_bytes = sizeof(roaming_disallowed_info_ptr->is_roaming_disallowed) + 
                  sizeof(roaming_disallowed_info_ptr->num_apn_names) + 
                  (roaming_disallowed_info_ptr->num_apn_names * 
                  sizeof(ds_sys_apn_name_type));

      DS_MSG2(MSG_LEGACY_HIGH,"ds_sys_event_ind : Roaming disallowed info:"
              "is_roaming_disallowed %d, num_apn_names %d",
               roaming_disallowed_info_ptr->is_roaming_disallowed,
              roaming_disallowed_info_ptr->num_apn_names); 
                 
     for(cnt_pdn = 0; cnt_pdn < roaming_disallowed_info_ptr->num_apn_names;
         cnt_pdn++)
     {
       DS_MSG2(MSG_LEGACY_HIGH,"ds_sys_event_ind :"
               "apn[%d]: %s",
                cnt_pdn,
                roaming_disallowed_info_ptr->apn_list[cnt_pdn].apn_name);
     }
     if(NULL != ds_conf_tech_3gpp.roaming_disallowed_info_ptr)
     {
       ret_val   = memcmp(ds_conf_tech_3gpp.roaming_disallowed_info_ptr,
                         (ds_sys_roaming_disallowed_info_type*)event_info,
                          num_bytes);
     }
     else
       ret_val =-1;
     
     if (ret_val != 0)
     {
        /*---------------------------------------------- ----------
          Process the configuration
        -----------------------------------------------------------*/
        
     /* Free the memory if the roaming disallowed cache ptr is not null. 
     Because of memory optimization we recalculate the cache size
     based on number of apnz */
       if(NULL != ds_conf_tech_3gpp.roaming_disallowed_info_ptr)
       {
         DS_SYSTEM_HEAP_MEM_FREE(ds_conf_tech_3gpp.roaming_disallowed_info_ptr);
       }             
      /* Reallocate memory for the cache */                              
       DS_SYSTEM_HEAP_MEM_ALLOC(ds_conf_tech_3gpp.roaming_disallowed_info_ptr,
                                num_bytes,
                                ds_sys_roaming_disallowed_info_type*);
       if (NULL == ds_conf_tech_3gpp.roaming_disallowed_info_ptr)
       {
          goto bail;
       }
       memset(ds_conf_tech_3gpp.roaming_disallowed_info_ptr, 
              0, 
              num_bytes);
  /* Update the cache with the new values */
       memscpy(ds_conf_tech_3gpp.roaming_disallowed_info_ptr,
               num_bytes,
               (ds_sys_roaming_disallowed_info_type *) event_info,
                num_bytes);  

       mask = DS_SYS_EVENT_3GPP_ROAMING_DISALLOWED_INFO_EX;
       invoke_cb = TRUE;
    }
    break;
   }
   case DS_SYS_3GPP_ROAMING_STATUS_EV:
   {
      memscpy(sys_evt_info_ptr, 
             sizeof(ds_sys_3gpp_roaming_status_type),
             (ds_sys_3gpp_roaming_status_type *) event_info,
              sizeof(ds_sys_3gpp_roaming_status_type));
   
     roaming_status_ptr = (ds_sys_3gpp_roaming_status_type*)sys_evt_info_ptr;
   
     DS_MSG1(MSG_LEGACY_HIGH,"ds_sys_event_ind : Roaming status %d",
             roaming_status_ptr->roaming_status);

      if(ds_conf_tech_3gpp.roaming_status.roaming_status != 
       ((ds_sys_3gpp_roaming_status_type*)event_info)->roaming_status)
      {
          ds_conf_tech_3gpp.roaming_status.roaming_status =
         ((ds_sys_3gpp_roaming_status_type*)event_info)->roaming_status;
          mask = DS_SYS_EVENT_3GPP_ROAMING_STATUS;
          invoke_cb = TRUE;
     }
     break;
   }
   case DS_SYS_3GPP_PDN_THROTTLE_INFO_EV:
   {
      memscpy(sys_evt_info_ptr,
              sizeof(ds_sys_pdn_throttle_info_type),
             (ds_sys_pdn_throttle_info_type *) event_info,
              sizeof(ds_sys_pdn_throttle_info_type));

     pdn_throttle_info  = (ds_sys_pdn_throttle_info_type *)sys_evt_info_ptr;
     DS_MSG1(MSG_LEGACY_HIGH,"ds_sys_event_ind : pdn throttle"
                            " info num_apn is %d",
                             pdn_throttle_info->num_apn);
     
     for(i =0; i<pdn_throttle_info->num_apn;i++)
     {
        DS_MSG3(MSG_LEGACY_HIGH,"ds_sys_event_ind : pdn throttle info "
                "is_v6_throtled  is %d, is_ipv4_throttled %d"
                "is_non_ip_throttled %d ",
                pdn_throttle_info->pdn_throttle_info[i].is_ipv6_throttled,
                pdn_throttle_info->pdn_throttle_info[i].is_ipv4_throttled,
                pdn_throttle_info->pdn_throttle_info[i].is_non_ip_throttled);
     }
      num_bytes= sizeof(pdn_throttle_info->num_apn)+
                 pdn_throttle_info->num_apn *(
                 sizeof(ds_sys_pdn_ctxt_throttle_info_type));
 
      if(ds_conf_tech_3gpp.pdn_throttle_info !=NULL)
      {
        ret_val   = memcmp(ds_conf_tech_3gpp.pdn_throttle_info,
                           (ds_sys_pdn_throttle_info_type*)event_info,
                           num_bytes);
      }
      else
         ret_val =-1;
      if(ret_val != 0)
      {
        /* Free the memory if the pdn throttle info ptr is not null. 
      Because of memory optimization we recalculate the cache size
      based on number of apnz */
         if(NULL != ds_conf_tech_3gpp.pdn_throttle_info)
         {
            DS_SYSTEM_HEAP_MEM_FREE(ds_conf_tech_3gpp.pdn_throttle_info);
         }           
    /* Reallocate memory for the cache */                              
         DS_SYSTEM_HEAP_MEM_ALLOC(ds_conf_tech_3gpp.pdn_throttle_info,
                                 num_bytes, ds_sys_pdn_throttle_info_type*);
         if (NULL == ds_conf_tech_3gpp.pdn_throttle_info)
         {
           goto bail;
         }
  /* Update the cache with the new values */
         memscpy(ds_conf_tech_3gpp.pdn_throttle_info,
                 num_bytes,
                 (ds_sys_pdn_throttle_info_type *) event_info,
                  num_bytes);
 
        mask = DS_SYS_EVENT_3GPP_PDN_THROTTLE_INFO;
        invoke_cb = TRUE;
     }
     break;
   }
   case DS_SYS_DATA_ACTIVITY_RESUMED_EV:
   {
      invoke_cb = TRUE;
      mask = DS_SYS_EVENT_DATA_ACTIVITY_RESUMED;
      DS_MSG1(MSG_LEGACY_HIGH,"ds_sys_event_ind(): invocation for"
                   "DS_SYS_DATA_ACTIVITY_RESUMED_EV %d",DS_SYS_DATA_ACTIVITY_RESUMED_EV);
      break;
   }
#ifdef QWES_FEATURE_ENABLE
   case DS_SYS_EVENT_QTEELS_AVAILABLE_EV:
   {
     memscpy(sys_evt_info_ptr, 
             sizeof(ds_sys_event_qteels_avail_info_type),
             (ds_sys_event_qteels_avail_info_type *) event_info,
              sizeof(ds_sys_event_qteels_avail_info_type));

     mask = DS_SYS_EVENT_QTEELS_SERVICE_AVAILABLE;
     invoke_cb = TRUE;
     break;
   }

   case DS_SYS_EVENT_CAAS_FEATURE_STATUS_EV:
   {
     memscpy(sys_evt_info_ptr, 
             sizeof(ds_sys_caas_feature_status_list_type),
             (ds_sys_caas_feature_status_list_type *) event_info,
              sizeof(ds_sys_caas_feature_status_list_type));

     mask = DS_SYS_EVENT_CAAS_FEATURE_STATUS;
     invoke_cb = TRUE;
     break;
   }

   case DS_SYS_EVENT_CAAS_SYNC_COMPLETE_BROADCAST_EV:
   {
     memscpy(sys_evt_info_ptr, 
             sizeof(ds_sys_event_sync_complete_broadcast_type),
             (ds_sys_event_sync_complete_broadcast_type *) event_info,
              sizeof(ds_sys_event_sync_complete_broadcast_type));

     mask = DS_SYS_EVENT_CAAS_SYNC_COMPLETE_BROADCAST;
     invoke_cb = TRUE;
     break;
   }
#endif
    case DS_SYS_EVENT_APN_MSISDN_INFO_CHANGE_EV:
    {
       memscpy(sys_evt_info_ptr, 
             sizeof(ds_sys_event_msisdn_info_type),
             (ds_sys_event_msisdn_info_type *) event_info,
              sizeof(ds_sys_event_msisdn_info_type));
        msisdn_info_ptr = (ds_sys_event_msisdn_info_type *) sys_evt_info_ptr;	
        if (NULL == ds_conf_tech_all.msisdn_info_ptr)
        {
          DS_SYSTEM_HEAP_MEM_ALLOC(ds_conf_tech_all.msisdn_info_ptr, 
                                    sizeof(ds_sys_event_msisdn_info_type),
                                    ds_sys_event_msisdn_info_type*);
          if (NULL == ds_conf_tech_all.msisdn_info_ptr)
          { 
             DS_MSG0(MSG_LEGACY_HIGH,"ds_sys_event_ind():memory not created");
             goto bail;
          }
        }
        memset(ds_conf_tech_all.msisdn_info_ptr, 
	       0, 
               sizeof(ds_sys_event_msisdn_info_type)
               );
      
        memscpy(ds_conf_tech_all.msisdn_info_ptr,
                 sizeof(ds_sys_event_msisdn_info_type),
                 event_info,sizeof(ds_sys_event_msisdn_info_type));
        event_buf_ptr = q_check(&ds_sys_eventi_q);
	 
        while(event_buf_ptr != NULL)
        {
          ds_sys_event_buf_ptr = (ds_sys_eventi_buf_type*)event_buf_ptr;
          if(ds_sys_event_buf_ptr->event_mask == DS_SYS_EVENT_APN_MSISDN_INFO_CHANGE)
          {
            if((strncmp((char*)ds_sys_event_buf_ptr->user_data_ptr,
                        msisdn_info_ptr->apn_name,DS_SYS_MAX_APN_LEN) == 0)
                        || (strncmp((char*)ds_sys_event_buf_ptr->user_data_ptr,
                        msisdn_info_ptr->resolved_apn_name, DS_SYS_MAX_APN_LEN) == 0) )
            {
              mask = DS_SYS_EVENT_APN_MSISDN_INFO_CHANGE;
              invoke_cb = TRUE;
              break;
            }
          }
          event_buf_ptr =
                q_next(&ds_sys_eventi_q, &((q_generic_item_type *) event_buf_ptr)->link);
        }
        break;
      }

     case DS_SYS_EVENT_3GPP_UL_DATA_DELIVERY_EV:
     {
       memscpy(sys_evt_info_ptr, 
               sizeof(ds_sys_3gpp_ul_data_delivery_ind_info_type),
               (ds_sys_3gpp_ul_data_delivery_ind_info_type *) event_info,
               sizeof(ds_sys_3gpp_ul_data_delivery_ind_info_type));
       mask = DS_SYS_EVENT_3GPP_UL_DATA_DELIVERY_INFO;
       invoke_cb = TRUE;
       break;
    }

   default:
   {
     DS_SYSTEM_HEAP_MEM_FREE(sys_evt_info_ptr);
     DS_MSG1(MSG_LEGACY_ERROR,"ds_sys_event_ind(): Invalid Ev %d ", event_name);
     return -1;
   }

 bail:
   DS_ERR_FATAL("ds_sys_event_ind() : Mem alloc failed");
   return -1;
  }
  
  if(invoke_cb == TRUE)
  {
  /*--------------------------------------------------------------
    Get a DS command buffer
   -------------------------------------------------------------*/
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_sys_eventi_cmd_type));
    if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
    { 
      DS_SYSTEM_HEAP_MEM_FREE(sys_evt_info_ptr);
      return -1;
    }
    cmd_ptr->hdr.cmd_id = DS_CMD_SYS_EVENT_HANDLER;
    cmd_data_info = (ds_sys_eventi_cmd_type*)cmd_ptr->cmd_payload_ptr;
    cmd_data_info->event           = mask;
    cmd_data_info->tech_type       = tech_type;
    cmd_data_info->event_info      = (void *)sys_evt_info_ptr;
    ds_put_cmd(cmd_ptr);
  }
  else
    DS_SYSTEM_HEAP_MEM_FREE(sys_evt_info_ptr);
  return 0;
}/* ps_sys_event_ind() */


/*===============================================     =================
  FUNCTION  DS_SYS_IOCTL_REGSITERI_CBACK()

  DESCRIPTION
    @brief    This function allows client to register callback for of 
              particular   technology 
  
  PARAMETERS
     @param[in] tech_type             States Technology type
      @param[in] tech_cback_f_ptr      Callback function 
     @param[out] ps_errno             DS_EFAULT if parameter is invalid
                                   DS_EINVAL if operation is invalid
                                   DS_NOMEMORY if memory allocation fails
   RETURN VALUE
     @return   0         On Success.
     @return  -1           On failure. 
  
   COMMENTS
     @code
     Example
      3GPP modehandler registers for ioctl callback as follow 
       
      int16  ioctl_3gpp_cback(...)
      {
        .....
      }
     
      ds_sys_ioctl_lte_attach_pdn_params_type * ioctl_info;
     
      ret_val =  ds_sys_ioctl_register_tech_cback(DS_SYS_TECH_3GPP,
                                              &ioctl_3gpp_cback,
                                              &ps_errno);
     
      if(0 > ret_val)
      {
        LOG_MSG_ERROR_1("Callback reg failed, err %d", ps_errno);
    }
  
      @endcode 
    *
  DEPENDENCIES
    None
    
  SIDE EFFECTS
    None
  ==================================================================*/
int16 ds_sys_ioctl_registeri_cback
(  
 ds_sys_tech_enum_type                        tech_type,
 ds_sys_ioctl_tech_cback_f_ptr_type	          tech_cback_f_ptr,
 int16                                       *ps_errno
)
{
  DS_MSG2(MSG_LEGACY_ERROR,"ds_sys_ioctl_registeri_cback(): Tech %d, "
                           "CB 0x%p", tech_type, tech_cback_f_ptr);
  if (ps_errno == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_sys_ioctl_registeri_cback(): Invalid arg "
                             "ps_errno");
    return -1;
  }

  if (tech_type != DS_SYS_TECH_3GPP)
  {
    *ps_errno = DS_EFAULT;
     DS_MSG0(MSG_LEGACY_ERROR,"ds_sys_ioctl_registeri_cback(): Invalid arg "
                              "passed");
     return -1;
  }
  
 if (tech_cback_f_ptr != NULL )
 {
    ds_sys_ioctl_tech_func_type[tech_type].tech_f_ptr = 
     tech_cback_f_ptr;
 }
  else 
  {
   *ps_errno = DS_EFAULT;  
    return -1;
  }
  return 0 ;
}

/*=================================================================
  FUNCTION  DS_SYS_EVENT_REG()

  DESCRIPTION
   @brief  This function register a particular event with the 
           ds_sys module .
           events DS_SYS_EVENT_SYSTEM_STATUS
           DS_SYS_EVENT_3GPP_ROAMING_STATUS
           

  PARAMETERS

  @param[in] tech_type            States Technology type  
  @param[in] mask                 Mask to check 
  @param[in] event_cback_f_ptr    Event function pointer 
  @param[in] user_data_ptr        User data pointer 
  @param[in] dss_errno            errno to be filled
   
  RETURN VALUE
  @return  0             SUCCESS
         -1              FAILURE
  
  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
int16 ds_system_event_reg
(
  ds_sys_tech_enum_type                       tech_type,
  uint64                                      event_mask,
  ds_sys_event_cback_f_ptr_type               event_cback_f_ptr,
  void                                       *user_data_ptr,
  int16                                      *dss_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - -   - - -*/
  DS_MSG3(MSG_LEGACY_HIGH,"ds_system_event_reg(): "
                         "event %d tech %d cb 0x%p, caller 0x%p",
                          event_mask, tech_type, event_cback_f_ptr);

  if (dss_errno == NULL ) 
  {
    goto bail;
  }
  if((event_mask == 0) ||(event_cback_f_ptr ==NULL))
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_system_event_reg(): Invalid argument"
            "ds_errno or mask or event_cback_f_ptr passed ");
    
   *dss_errno = DS_EFAULT;
    goto bail;
  }
  
  *dss_errno = 0;
  
  switch (tech_type)
  {
    case DS_SYS_TECH_3GPP:
    {  
      if((ds_sys_check_3gpp_bit_is_set(event_mask) == FALSE) || 
         (ds_sys_check_system_bit_is_set(event_mask) == TRUE))
      {
         DS_MSG1(MSG_LEGACY_ERROR,"ds_system_event_reg():mask "
                 "is not a 3gpp event mask %d",
                  event_mask);
         goto bail;
      }
      else
      {
        ds_sys_check_and_alloc_event(event_mask,
                                     tech_type,
                                     event_cback_f_ptr,
                                     user_data_ptr,
                                     dss_errno);
        if(*dss_errno != 0)
            goto bail;
      } 
      break;
    }
    case DS_SYS_TECH_ALL:
    {
      if((ds_sys_check_system_bit_is_set(event_mask) == FALSE) || 
         (ds_sys_check_3gpp_bit_is_set(event_mask) == TRUE))
      {
         DS_MSG1(MSG_LEGACY_ERROR,"ds_system_event_reg(): mask "
                "is not a system mask %d",
                 event_mask);
         goto bail;
      }
      else
      {
        ds_sys_check_and_alloc_event(event_mask,
                                     tech_type,
                                     event_cback_f_ptr,
                                     user_data_ptr,
                                     dss_errno);
       if(*dss_errno != 0)
         goto bail;

       /* Special Case: to inform framework that a new client has been 
       registered for data activity resumed event*/
        if ( event_mask & DS_SYS_EVENT_DATA_ACTIVITY_RESUMED )
        { 
           DS_MSG1(MSG_LEGACY_HIGH,"ds_system_event_reg(): reg of  "
                   "DS_SYS_EVENT_DATA_ACTIVITY_RESUMEDk %d",DS_SYS_EVENT_DATA_ACTIVITY_RESUMED);
          ds_fwk_data_activity_client_status_ind(TRUE);
        }
      }
    }
    break;
    default:
    {     
      DS_MSG1(MSG_LEGACY_ERROR,"ds_system_event_reg(): Invalid tech %d",
                                tech_type);
      goto bail;
    }
  }
  return 0;
bail:
  return -1;
}/* ds_sys_event_reg() */

/*==================================================================
  FUNCTION  DS_SYS_EVENT_DEREG()

  DESCRIPTION
   @brief  This function deregisters the event from the queue.
           DS_SYS_EVENT_SYSTEM_STATUS,      DS_SYS_EVENT_3GPP_ROAMING_STATUS
           

  PARAMETERS

  @param[in] tech_type            States Technology type  
  @param[in] mask                 Mask to check 
  @param[in] event_cback_f_ptr    Event function pointer 
  @param[in] dss_errno            errno to be filled	
  
  RETURN VALUE
  @return  0                      Success.    
         -1                       Fail

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
==================================================================*/
int16 ds_system_event_dereg
(
  ds_sys_tech_enum_type                  tech_type,
  uint64                                 mask,
  ds_sys_event_cback_f_ptr_type          event_cback_f_ptr, 
  int16                                 *dss_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - -*/
  DS_MSG3(MSG_LEGACY_HIGH,"ds_system_event_dereg(): "
                           "event %d tech %d cb 0x%p, caller_ptr",
                            mask, tech_type, event_cback_f_ptr);
  if (dss_errno == NULL ) 
  {
    return -1;
  }

  if ((mask == 0)||(event_cback_f_ptr ==NULL))
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_system_event_dereg():"
            "Invalid argument ps_errno or mask or event_cback_f_ptr passed");    
    
   *dss_errno = DS_EFAULT;
    goto bail;
  }

  *dss_errno = 0;
  
  switch (tech_type)
  {
    case DS_SYS_TECH_3GPP:
    {  
      if((ds_sys_check_3gpp_bit_is_set(mask) == FALSE) || 
         (ds_sys_check_system_bit_is_set(mask) == TRUE))
      {
         DS_MSG1(MSG_LEGACY_ERROR,"ds_system_event_dereg(): "
                 "3gpp event_name %d",
                  mask);
         goto bail;
      }
      else
      {
        if(ds_sys_check_and_dealloc_event(tech_type,
                                          event_cback_f_ptr,
                                          mask,
                                          dss_errno) !=0 )
         {
          DS_MSG1(MSG_LEGACY_ERROR,"ds_system_event_dereg():"
                 "ds_sys_check_and_dealloc_event failed %d",
                  mask);
          goto bail;
         }
      }
      break;
    }
    case DS_SYS_TECH_ALL:
    {
     if((ds_sys_check_system_bit_is_set(mask) == FALSE) || 
        (ds_sys_check_3gpp_bit_is_set(mask) == TRUE))
  {
         DS_MSG1(MSG_LEGACY_ERROR,"ds_system_event_dereg(): "
                 "system event_name %d",
                  mask);
         goto bail;
  }
  else
  {
      /* Special Case: to inform FWK that a client has been 
       de-registered for data activity resumed event*/
        if ( mask & DS_SYS_EVENT_DATA_ACTIVITY_RESUMED )
        { 
           DS_MSG1(MSG_LEGACY_HIGH,"ds_system_event_dereg(): dereg of"
                   "DS_SYS_EVENT_DATA_ACTIVITY_RESUMED %d",DS_SYS_EVENT_DATA_ACTIVITY_RESUMED);
          ds_fwk_data_activity_client_status_ind(FALSE);
        }
        if(ds_sys_check_and_dealloc_event(tech_type,
                                          event_cback_f_ptr,
                                          mask,
                                          dss_errno) !=0 )
        {
          DS_MSG1(MSG_LEGACY_ERROR,"ds_system_event_dereg():"
                  "ds_sys_check_and_dealloc_event failed %d",
                   mask);
          goto bail;
        }
     }
     break;
    }
    default:
    {     
      DS_MSG1(MSG_LEGACY_ERROR,"ds_system_event_dereg(): Invalid tech %d",
                              tech_type);
      goto bail;
    } 
  }  
  return 0;
 bail:
  *dss_errno = DS_EFAULT;
  return -1;
} /* ds_sys_event_dereg() */

/*===========================================================================
  FUNCTION  DS_SYS_IOCTL()

  DESCRIPTION
  @brief  This function performs the operation for particular ioctl

   PARAMETERS
   @param[in] tech_type             States Technology type
  @param[in] ioctl_name           Operation name
  @param[in] arg_val_ptr          Pointer to operation specific
  @param[out] dss_errno           Error code is filled when return value is not
                                  success. Possible values are
                                  @li DS_EFAULT if parameter is invalid
                                  @li DS_EINVAL if operation is invalid
                                  @li DS_NOMEMORY if memory allocation fails

  RETURN VALUE
    @return  DSS_SUCCESS         On Success.
  @return  DSS_ERROR              On failure.

  DEPENDENCIES
    None

  SIDE EFFECTS
   None

  COMMENTS
   @code
   
   Example
  Connection manager can tell modehandlers whether to allow dun call for
  particular call id  or not.
     
  ds_sys_ioctl_dun_ctrl_response_type * ioctl_info;
  ioctl_info->call_id = xxx;
  ioctl_info->allow_call = TRUE;
   
  ret_val =  ds_sys_ioctl(DS_SYS_TECH_3GPP,
                          DS_SYS_IOCTL_DUN_CTRL_RESPONSE,
                          ioctl_info,
                          &ds_errno);
    if(0 > ret_val)
    {
    LOG_MSG_ERROR("Ioctl failed, err %d", ds_errno,0,0);
     }
     
  @endcode
===========================================================================*/

int16 ds_sys_ioctl
(
  ds_sys_tech_enum_type            tech_type,
  ds_sys_ioctl_enum_type           ioctl_name,
  void                            *arg_val_ptr,
  int16                           *dss_errno
)
{
  int16 result = 0; 
 /*----------------------------------------------------------------*/ 
 DS_MSG2(MSG_LEGACY_ERROR,"ds_system_ioctl(): "
         "ioctl_name %d tech_type %d", ioctl_name,tech_type);

 switch (tech_type)
 {
   case DS_SYS_TECH_3GPP:
   {  
    if (ioctl_name >= DS_SYS_IOCTL_3GPP_MAX || 
        ioctl_name < DS_SYS_IOCTL_3GPP_MIN)
    {
       goto bail;
    }
    break;
   }
   case DS_SYS_TECH_ALL:
   {
     if(ioctl_name >= DS_SYS_IOCTL_COMMON_MAX ||
        ioctl_name <= DS_SYS_IOCTL_COMMON_MIN )
     {
       goto bail;
     }
    break;
   }
   default:
   {
     goto bail;
   }
  }
  if(tech_type == DS_SYS_TECH_ALL)
  {
    result =  ds_sys_common_ioctl_hdlr(ioctl_name,arg_val_ptr,dss_errno); 
  }
  else
  {
    if (ds_sys_ioctl_tech_func_type[tech_type]. \
                     tech_f_ptr != NULL)
    {  
   /* Route it to specific modehandler. 
      MH should use this extended callback
       to register */
       result = ds_sys_ioctl_tech_func_type[tech_type].tech_f_ptr 
                (ioctl_name, arg_val_ptr, dss_errno );
     }
     else
     {
        *dss_errno = DS_EOPNOTSUPP;    
         DS_MSG1(MSG_LEGACY_ERROR,"ds_system_ioctl():"
                 "Tech %d has not registered function",
                  tech_type);
         goto bail;
      }
   }
  return result;
 bail:
   return -1;
}

/*==================================================================
  FUNCTION  DS_SYS_INIT()

  DESCRIPTION
   @brief  This function inits the ds_sys module.         It inits the global event queue and
           also registers the command handler         with DS task and inits the ds_sys crit section

  PARAMETERS
   None
   
  RETURN VALUE
   None    

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===================================================================*/
void ds_sys_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - - -*/
/*-----------------------------------------------------------------
   Initialize global event queue
 -----------------------------------------------------------------*/
   (void) q_init(&ds_sys_eventi_q);
  (void) ds_register_cmd_hdlr(DS_MODULE_DS_SYS, 
                              ds_sys_eventi_cback_cmd_handler);
    /*------------------------------------------------------------
    Initialize the DS SYS critical section
    --------------------------------------------------------------*/
    DS_INIT_CRIT_SECTION(&ds_sys_crit_section);
}
