/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/gstimer.c_v   1.2   01 Jul 2002 09:30:12   jault  $   
$Header: //components/rel/mmcp.mpss/6.1.10/nas/services/src/gstimer.c#1 $    $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/19/01   TS      Now sends an error when an invalid timer command is received.
07/01/02   jca     Modified function gs_enquire_timer(). 
09/11/03   kwa     Added INTLOCK and INTFREE to all functions to eliminate
                   reentrancy.  Reentrant calls to gs_clear_timer and
                   gs_cancel_timer created a condition where an attempt was
                   made to delete a NULL timer pointer.
02/11/04   ks      Added ERR_FATAL when a NULL pointer is passed in gs_modify_timer
                   Added new function gs_timer_exists() to validate a timer.
02/22/04   ks      Added a NULL check before processing the timer in 
                   gs_modify_timer() and removed the stringent ERR_FATAL.
===========================================================================*/

/* Include files */

#include "customer.h"
#include "rex.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "ms_timer_v.h"
#include "msg.h"
#include "err.h"
#include "mm_v.h"
#include "timer.h"
#include "nasutils.h"


#ifdef FEATURE_DUAL_SIM
#define GS_MAX_TIMER_CLIENT            GS_TIMER_QUEUE_MN_CNM_SUB2
#elif defined(FEATURE_NAS_GW)
#define GS_MAX_TIMER_CLIENT            GS_TIMER_QUEUE_IMS
#else 
#define GS_MAX_TIMER_CLIENT            GS_TIMER_QUEUE_IMS
#endif 

#define GS_LOCAL_SUB 0
#ifdef FEATURE_GS_MULTIPLEX_TIMER
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

static timer_type gs_timers_multiplex_timer_sim[MAX_NAS_STACKS];
#define gs_timers_multiplex_timer  gs_timers_multiplex_timer_sim[reg_as_id]

static timer_type gs_timers_multiplex_non_def_timer_sim[MAX_NAS_STACKS];
#define gs_timers_multiplex_non_def_timer  gs_timers_multiplex_non_def_timer_sim[reg_as_id]

#else
static timer_type gs_timers_multiplex_non_def_timer;

static timer_type gs_timers_multiplex_timer;

#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/
#endif
gs_timer_debug_info_T  gs_timer_debug_buffer[GS_MAX_DEBUG_BUFFER];

timer_info_T task_timer_info[GS_MAX_TIMER_CLIENT+1][MAXNO_TIMERS];

#ifdef FEATURE_GS_MULTIPLEX_TIMER

timer_client_info_nas_T *task_timer_info_head= NULL;

timer_client_info_nas_T *task_timer_info_non_def_head = NULL;



#define MIN_GS_MULTIPLEX_VALUE 500
#endif
timer_group_type nas_timer_group_non_deferrable;
uint16 gs_timer_debug_buffer_index = 0;


/*******************************************************************************
 *
 *  Function name: gs_start_timer
 *  -------------------------------
 *  Description:
 *  ------------
 *
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *  IN         num_milli_secs
 *  IN         data
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/

gs_status_T   gs_start_timer(gs_timer_queue_id_T queue_id,
                             timer_id_T        timer_id,
                             rex_timer_cb_type func_ptr,        
                             milli_secs_T      milli_secs,
                             byte              data)
{
   unsigned long param;
   gs_status_T   status = GS_FAILURE;
   
   if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
   {
       rex_enter_crit_sect(&timer_crit_sect);

       /* Check to see if timer already exists */
       if(task_timer_info[queue_id][timer_id].timer != NULL)
       {
          /* Stop timer */
          (void) rex_clr_timer(task_timer_info[queue_id][timer_id].timer);

          /* Deallocate timer */
          rex_delete_timer_ex(task_timer_info[queue_id][timer_id].timer);
    
          /* Make sure to mark this timer as inactive */
          task_timer_info[queue_id][timer_id].timer = NULL;
       }
    
       /* Copy data to param */
       param     = data;
    
       /* Copy timer id to param */
       param |= timer_id << 16;
    
       /* Allocate timer */
       task_timer_info[queue_id][timer_id].timer = rex_create_timer_ex(func_ptr, param);
    
       /* If memory was available */  
       if(task_timer_info[queue_id][timer_id].timer != NULL)
       {  
          /* Start Timer */
          (void) rex_set_timer(task_timer_info[queue_id][timer_id].timer, milli_secs);
    
          /* Return Success */
          status = GS_SUCCESS;
       }
    
       rex_leave_crit_sect(&timer_crit_sect);
    }
    else
    {
       /* Raise error condition */
       ERR("ERROR: Invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);

       status = GS_FAILURE;
    }

    return status;
}

#ifdef FEATURE_GS_MULTIPLEX_TIMER
void gs_start_multiplex_timer(uint64 milli_secs,boolean non_def)
{
  if(non_def == FALSE)  
  {
    if (timer_get_64(&gs_timers_multiplex_timer, T_MSEC))
    {
       MSG_HIGH_DS_0(GS_LOCAL_SUB,"=GS= Restart gs_timers_multiplex_timer");
	   (void) timer_clr
	   (
	     &gs_timers_multiplex_timer,
	     T_MSEC
	   );
    }
    else
    {
     MSG_HIGH_DS_1(GS_LOCAL_SUB,"=GS= Start gs_timers_multiplex_timer %lu", milli_secs);
    }
  
    (void)timer_set_64
    (
      &gs_timers_multiplex_timer,
      milli_secs,
      0,
      T_MSEC
    );
  }
  else
  {
    if (timer_get_64(&gs_timers_multiplex_non_def_timer, T_MSEC))
    {
      MSG_HIGH_DS_0(GS_LOCAL_SUB,"=GS= Restart gs_timers_multiplex_timer");
 	  (void) timer_clr
	  (
	    &gs_timers_multiplex_non_def_timer,
	    T_MSEC
	   );
    }
    else
    {
       MSG_HIGH_DS_1(GS_LOCAL_SUB,"=GS= Start gs_timers_multiplex_timer %lu", milli_secs);
    }
  
    (void)timer_set_64
    (
      &gs_timers_multiplex_non_def_timer,
      milli_secs,
      0,
      T_MSEC
     );
  }
}

void gs_timer_expiry(unsigned long param)
{

   gs_timer_queue_id_T		queue_id;
   timer_id_T				timer_id;
   timer_client_info_nas_T *task_info_head= task_timer_info_head;
   timer_client_info_nas_T *task_info_head_next=NULL;
   uint64 mili_sec=0;
   queue_id = task_timer_info_head->queue_id;
   timer_id = task_timer_info_head->timer_id;
   task_timer_info_head = task_info_head->link.next_ptr;
   if(task_timer_info[queue_id][timer_id].timer != NULL)
   {
       (void)timer_undef((timer_type *)task_timer_info[queue_id][timer_id].timer);        
           /* Make sure to mark this timer as inactive */
        task_timer_info[queue_id][timer_id].timer = NULL;
   }
   MSG_HIGH_DS_3(GS_LOCAL_SUB,"=GS= Expired gs_timer_expiry %lu queue_id %d timer_id %d", param,queue_id,timer_id);
   (void)task_info_head->cb_ptr(param);
   if(task_timer_info_head)
   {
     while(task_timer_info_head->timer_count == 0 && task_timer_info_head != 0)
     {
       queue_id = task_timer_info_head->queue_id;
       timer_id = task_timer_info_head->timer_id;
       (void)task_timer_info_head->cb_ptr(task_timer_info_head->data);
       task_info_head = task_timer_info_head;
       task_timer_info_head = task_timer_info_head->link.next_ptr;
       modem_mem_free(task_info_head, MODEM_MEM_CLIENT_NAS);
       if(task_timer_info[queue_id][timer_id].timer != NULL)
       {
           (void)timer_undef((timer_type *)task_timer_info[queue_id][timer_id].timer);        
           /* Make sure to mark this timer as inactive */
           task_timer_info[queue_id][timer_id].timer = NULL;
       }
     }
     if(task_timer_info_head != NULL)
     {
        param = task_timer_info_head->data;
        param |=  task_timer_info_head->timer_id << 16;
	timer_def(&gs_timers_multiplex_timer, 
				  NULL,
				  rex_self(),
				  0,
				  gs_timer_expiry,
				  param);
        gs_start_multiplex_timer(task_timer_info_head->timer_count,FALSE);
        mili_sec = task_timer_info_head->timer_count;
        task_timer_info_head->timer_count = 0;
        task_info_head = task_timer_info_head->link.next_ptr;
        while(task_info_head)
        {
          task_info_head->timer_count -= mili_sec;
	  MSG_HIGH_DS_5(GS_LOCAL_SUB,"=GS=  Timer current entry queue_id %d timer_id %d milli_secs %lu data %d non_deferrable %d", task_info_head->queue_id,task_info_head->timer_id,task_info_head->timer_count,task_info_head->data,task_info_head->non_deferrable);
          task_info_head=task_info_head->link.next_ptr;
        }
      }
   }
}


void gs_timer_expiry_non_def(unsigned long param)
{
   gs_timer_queue_id_T 	 queue_id;
   timer_id_T				 timer_id;
   unsigned long param_curr ;
   timer_client_info_nas_T *task_info_head= task_timer_info_non_def_head;
   uint64 mili_sec=0;
    queue_id = task_timer_info_non_def_head->queue_id;
    timer_id = task_timer_info_non_def_head->timer_id;
    task_timer_info_non_def_head = task_info_head->link.next_ptr;
    modem_mem_free(task_info_head, MODEM_MEM_CLIENT_NAS);
    if(task_timer_info[queue_id][timer_id].timer != NULL)
    {
       (void)timer_undef((timer_type *)task_timer_info[queue_id][timer_id].timer);        
           /* Make sure to mark this timer as inactive */
        task_timer_info[queue_id][timer_id].timer = NULL;
    }
    MSG_HIGH_DS_3(GS_LOCAL_SUB,"=GS= Expired gs_timer_expiry_non_def %lu queue_id %d timer_id %d", param,queue_id,timer_id);
   (void)task_info_head->cb_ptr(param);   //here param should be data+timer id which gs will post

    if(task_timer_info_non_def_head)
    {
       while(task_timer_info_non_def_head->timer_count == 0 && task_timer_info_non_def_head != 0)
       {
	    queue_id = task_timer_info_non_def_head->queue_id;
	    timer_id = task_timer_info_non_def_head->timer_id;
            param_curr = task_timer_info_non_def_head->data;
            param_curr |= task_timer_info_non_def_head->timer_id <<16;
            (void)task_timer_info_non_def_head->cb_ptr(param_curr);
            task_info_head = task_timer_info_non_def_head;
            task_timer_info_non_def_head = task_timer_info_non_def_head->link.next_ptr;
	    modem_mem_free(task_info_head, MODEM_MEM_CLIENT_NAS);
	    if(task_timer_info[queue_id][timer_id].timer != NULL)
            {
              (void)timer_undef((timer_type *)task_timer_info[queue_id][timer_id].timer);  // need to check      
              /* Make sure to mark this timer as inactive */
              task_timer_info[queue_id][timer_id].timer = NULL;
            }
        }
        if(task_timer_info_non_def_head != NULL)
        {
           param_curr = task_timer_info_non_def_head->data;
           param_curr |=  task_timer_info_non_def_head->timer_id << 16;
           timer_group_set_deferrable(&nas_timer_group_non_deferrable,FALSE);
           timer_def(&gs_timers_multiplex_non_def_timer, 
			 &nas_timer_group_non_deferrable,
			 rex_self(),
			 0,
			 gs_timer_expiry_non_def,
			 param_curr);
           gs_start_multiplex_timer(task_timer_info_non_def_head->timer_count,TRUE);
           mili_sec = task_timer_info_non_def_head->timer_count;
           task_timer_info_non_def_head->timer_count = 0;
           task_info_head = task_timer_info_non_def_head->link.next_ptr;
           while(task_info_head)
           {
             task_info_head->timer_count -= mili_sec;
             MSG_HIGH_DS_5(GS_LOCAL_SUB,"=GS=  Timer current entry queue_id %d timer_id %d milli_secs %lu data %d non_deferrable %d", task_info_head->queue_id,task_info_head->timer_id,task_info_head->timer_count,task_info_head->data,task_info_head->non_deferrable);
             task_info_head = task_info_head->link.next_ptr;
           }
	 }
    }  
}



void gs_adjust_multiplex_entry(timer_client_info_nas_T *client_info ,uint64 curr_timer,uint64 mili_secs)
{

    timer_client_info_nas_T *task_timer= NULL;
	timer_client_info_nas_T **task_timer_head= NULL;
	timer_client_info_nas_T *task_timer_prev= NULL;
	int64 diff_timer_count = curr_timer - mili_secs;
	if(	client_info->non_deferrable == TRUE)
	{
	  task_timer = task_timer_info_non_def_head;
	  task_timer_head = &task_timer_info_non_def_head;
	}
	else
	{
	  task_timer = task_timer_info_head;
	  task_timer_head = &task_timer_info_head;
	}
	if (diff_timer_count > 0)
	{
      while(task_timer!= NULL)
      {
        task_timer->timer_count += diff_timer_count;   //need to add run
        task_timer = task_timer->link.next_ptr;
      }
	  client_info->link.next_ptr = *task_timer_head;
	  client_info->link.prev_ptr = NULL;
	  (*task_timer_head)->link.prev_ptr= client_info;
	  *task_timer_head = client_info; 
	  client_info->timer_count = 0;//Reelok
         gs_start_multiplex_timer(mili_secs,client_info->non_deferrable );
	}
	else
	{
	  diff_timer_count = (mili_secs - curr_timer);
	  while(task_timer != NULL )
	  {
	    //if((task_timer->link.next_ptr)->timer_count < (uint64)diff_timer_count)
		if(task_timer->timer_count < (uint64)diff_timer_count)
	    {
	      task_timer_prev = task_timer;
	      task_timer = task_timer->link.next_ptr;
	    }
		else
		{
		  break;
		}
	  }
	  if(task_timer_prev!=  NULL)
	  {
	    client_info->link.next_ptr = task_timer;;
        task_timer_prev->link.next_ptr = client_info;
		client_info->link.prev_ptr = task_timer_prev;
	  }
	  else if(task_timer!=NULL)
	  {
	    task_timer->link.next_ptr = client_info;
            MSG_HIGH_DS_5(GS_LOCAL_SUB,"=GS=  Timer current entry queue_id %d timer_id %d milli_secs %lu data %d non_deferrable %d", task_timer->queue_id,task_timer->timer_id,task_timer->timer_count,task_timer->data,task_timer->non_deferrable);
            client_info->link.prev_ptr = task_timer_prev;
	  }
	  client_info->timer_count = diff_timer_count;
	}
}
void gs_add_multiplex_entry(timer_client_info_nas_T *client_info,uint64 curr_timer,uint64 milli_secs, boolean non_def)
{
  boolean adjust_org = TRUE;
  if(client_info != NULL)
  {
   if(non_def==FALSE)
   {
      if(task_timer_info_head == NULL)
      {
       client_info->timer_count = 0;
       task_timer_info_head = client_info;
	   gs_start_multiplex_timer(milli_secs,non_def);
	   adjust_org = FALSE;
      }
   }
   else
   {
      if(task_timer_info_non_def_head == NULL)
      {
       client_info->timer_count = 0;
       task_timer_info_non_def_head = client_info;
	   gs_start_multiplex_timer(milli_secs,non_def);
	   adjust_org = FALSE;
      }
   }
   if(adjust_org == TRUE)
   {
     gs_adjust_multiplex_entry(client_info,curr_timer,milli_secs);
   }
  }
}

gs_status_T  gs_start_multiplex_timer_ext
(
      timer_client_info_nas_T    *client_info,
      timer_type               *timer_p,
	  gs_non_def_timer_cb_ptr  func_ptr,		
	  uint64				   milli_secs,
	  unsigned long					   param,
	  boolean				   non_deferrable,
	  boolean                  is_multiplex
)
{
    uint64 curr_timer ;
	gs_status_T gs_status = GS_FAILURE;
	if(NULL != timer_p)
	{
	  if( non_deferrable == TRUE )
	  {
	    curr_timer =timer_get_64(&gs_timers_multiplex_non_def_timer, T_MSEC);
		if(curr_timer == 0)
		{
		  timer_group_set_deferrable(&nas_timer_group_non_deferrable,FALSE);
		  timer_def(&gs_timers_multiplex_non_def_timer, 
			   &nas_timer_group_non_deferrable,
			   rex_self(),
			   0,
			   func_ptr,
			   param);
		}
	  }
	  else
	  {
	    curr_timer =timer_get_64(&gs_timers_multiplex_timer, T_MSEC);
	    if(curr_timer == 0)
	    {
		  timer_def(&gs_timers_multiplex_timer, 
			   NULL,
			   rex_self(),
			   0,
			   func_ptr,
			   param);
	    }
	  }
	  if(is_multiplex == TRUE)
	  {
 //       client_info->cb_ptr = func_ptr;
		client_info->data = (param & 0x0000FFFF);
		client_info->timer_count = milli_secs;
		client_info->non_deferrable= non_deferrable;
		client_info->link.next_ptr = NULL;
		client_info->link.prev_ptr = NULL;
	    gs_add_multiplex_entry(client_info,curr_timer,milli_secs,non_deferrable);
	  } 
	  else
	  {
	   ( void) timer_set_64
           (
            timer_p,
             milli_secs,
            0,
            T_MSEC
           );
	  }
	  gs_status = GS_SUCCESS;
        }
 	return  gs_status;
}
#endif
void gs_add_debug_buffer
(
	gs_queue_id_T			  queue_id,
	timer_id_T				  timer_id,
	gs_non_def_timer_cb_ptr   func_ptr, 	   
	uint64					  milli_secs,
	byte					  data,
	boolean 				  non_deferrable,
	gs_timer_action           action
	
)
{
   if(gs_timer_debug_buffer_index >= GS_MAX_DEBUG_BUFFER)
  {
    gs_timer_debug_buffer_index = 0;
  }
  gs_timer_debug_buffer[gs_timer_debug_buffer_index].queue_id = queue_id;
  gs_timer_debug_buffer[gs_timer_debug_buffer_index].timer_id = timer_id;
  gs_timer_debug_buffer[gs_timer_debug_buffer_index].cb_ptr= func_ptr;
  gs_timer_debug_buffer[gs_timer_debug_buffer_index].timer_count= milli_secs;
  gs_timer_debug_buffer[gs_timer_debug_buffer_index].data = data;
  gs_timer_debug_buffer[gs_timer_debug_buffer_index].non_deferrable = non_deferrable;
  gs_timer_debug_buffer[gs_timer_debug_buffer_index].action = action;

  gs_timer_debug_buffer_index++;
}
/*******************************************************************************
 *
 *  Function name: gs_start_timer_ext
 *  -------------------------------
 *  Description:
 *  ------------
 *  Generic timer start function. Takes extra parameter to specify if 
 *  the timer is non-deferrable
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *  IN         num_milli_secs
 *  IN         data
 *  IN         non_deferrable
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/

gs_status_T   gs_start_timer_ext
( 
  gs_timer_queue_id_T      queue_id,
  timer_id_T               timer_id,
  gs_non_def_timer_cb_ptr  func_ptr,        
  uint64                   milli_secs,
  byte                     data,
  boolean                  non_deferrable 
)
{
  unsigned long param;
  gs_status_T   status = GS_FAILURE;
  timer_type *timer_p = NULL;
  timer_client_info_nas_T *client_info = NULL;
  MSG_HIGH_DS_5(GS_LOCAL_SUB,"=GS=  gs_start_timer_ext queue_id %d timer_id %d milli_secs %lu data %d non_deferrable %d", queue_id,timer_id,milli_secs,data,non_deferrable);
  gs_add_debug_buffer(queue_id,timer_id,func_ptr,milli_secs,data,non_deferrable,TIMER_ADD);

  if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
  {
    rex_enter_crit_sect(&timer_crit_sect);
#ifdef FEATURE_GS_MULTIPLEX_TIMER
    if(queue_id == GS_TIMER_QUEUE_IMS || 
       queue_id == GS_TIMER_QUEUE_MM)
#endif
    {
      /* Check to see if timer already exists */
      if(task_timer_info[queue_id][timer_id].timer != NULL)
      {
        /* Deallocate timer memory */
        (void)timer_undef((timer_type *)task_timer_info[queue_id][timer_id].timer);        
        modem_mem_free(task_timer_info[queue_id][timer_id].timer, MODEM_MEM_CLIENT_NAS);
    
        /* Make sure to mark this timer as inactive */
        task_timer_info[queue_id][timer_id].timer = NULL;
      }
    
      /* Copy data to param */
      param     = data;
    
      /* Copy timer id to param */
      param |= timer_id << 16;
    
      /* Allocate timer */
      timer_p = (timer_type *) modem_mem_calloc( 1, sizeof( timer_type ), MODEM_MEM_CLIENT_NAS );
      if(NULL != timer_p)
      {
        task_timer_info[queue_id][timer_id].timer = timer_p;
		#ifdef FEATURE_GS_MULTIPLEX_TIMER
        task_timer_info[queue_id][timer_id].timer_type = GS_LEGACY;
		#endif
        if( non_deferrable == TRUE )
        {
            timer_group_set_deferrable(&nas_timer_group_non_deferrable,FALSE);
            timer_def(timer_p, 
                     &nas_timer_group_non_deferrable,
                     rex_self(),
                     0,
                     func_ptr,
                     param);
         }
         else 
         {            
            timer_def(timer_p,    
                      NULL, 
                      rex_self(),
                      0,
                      func_ptr,
                      param);
         }
         /* Start Timer */
         (void) timer_set_64(task_timer_info[queue_id][timer_id].timer,
                                         milli_secs,0,T_MSEC);
         /* Return Success */
          status = GS_SUCCESS;
      //  status = gs_start_multiplex_timer_ext(NULL,timer_p,func_ptr,milli_secs,param,non_deferrable,FALSE);
      }
      else
      {
         ERR("ERR: Memory not available to allocate for timer.",0,0,0);
         status = GS_FAILURE;
      }
    }
#ifdef FEATURE_GS_MULTIPLEX_TIMER
    else
    {			/* Check to see if timer already exists */
      if(task_timer_info[queue_id][timer_id].timer != NULL)
      {
       /* Deallocate timer memory */
       (void)timer_undef((timer_type *)task_timer_info[queue_id][timer_id].timer); 	   
       /* Make sure to mark this timer as inactive */
       task_timer_info[queue_id][timer_id].timer = NULL;
      }
       param = data;
       param |=	timer_id << 16;
       client_info = (timer_client_info_nas_T *)modem_mem_calloc( 1, sizeof( timer_client_info_nas_T ), MODEM_MEM_CLIENT_NAS );
       if(client_info != NULL)
       {
         client_info->queue_id = queue_id;
         client_info->timer_id = timer_id;
         client_info->cb_ptr= func_ptr;
         if(non_deferrable == FALSE)
         {
	  status = gs_start_multiplex_timer_ext(client_info,&gs_timers_multiplex_timer,gs_timer_expiry,milli_secs,param,non_deferrable,TRUE);
         }
         else
         { 
           status = gs_start_multiplex_timer_ext(client_info,&gs_timers_multiplex_non_def_timer,gs_timer_expiry_non_def,milli_secs,param,non_deferrable,TRUE);
         }
         if(status == GS_SUCCESS)
         {
           if(non_deferrable == FALSE)
           {
              task_timer_info[queue_id][timer_id].timer = &gs_timers_multiplex_timer;
              task_timer_info[queue_id][timer_id].timer_type = GS_MULTIPLEX_DEF;
           }
           else
           {
              task_timer_info[queue_id][timer_id].timer = &gs_timers_multiplex_non_def_timer;
              task_timer_info[queue_id][timer_id].timer_type = GS_MULTIPLEX_NON_DEF;
           }
         }
      }
      else
      {
        ERR("ERR: Memory not available to allocate for timer.",0,0,0);
        status = GS_FAILURE;
      }
    }
#endif
    rex_leave_crit_sect(&timer_crit_sect);
  }
  else
  {
    /* Raise error condition */
    ERR("ERROR: Invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);
    status = GS_FAILURE;
  }
  return status;
}
/*******************************************************************************
 *
 *  Function name: gs_clear_timer
 *  -------------------------------
 *  Description:
 *  ------------
 *
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

void gs_clear_timer(gs_timer_queue_id_T     queue_id,
                    timer_id_T        timer_id)
{
    if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
    {
        rex_enter_crit_sect(&timer_crit_sect);

        if(task_timer_info[queue_id][timer_id].timer != NULL)
        {
            /* Deallocate timer */
            rex_delete_timer_ex(task_timer_info[queue_id][timer_id].timer);

            /* Make sure to mark this timer as inactive */
            task_timer_info[queue_id][timer_id].timer = NULL;
         }
         else
         {
            /* Raise error condition */
            ERR("ERROR: Attempting to stop invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);
         }

         rex_leave_crit_sect(&timer_crit_sect);
    }
    else
    {
         /* Raise error condition */
         ERR("ERROR: Invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);
    }
}

/*******************************************************************************
 *
 *  Function name: gs_clear_timer_ext
 *  -------------------------------
 *  Description:
 *  ------------
 *  Generic timer clear function. Takes extra parameter to specify if 
 *  the timer is non-deferrable
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

void gs_clear_timer_ext
( 
  gs_timer_queue_id_T  queue_id,
  timer_id_T           timer_id
)
{
   MSG_HIGH_DS_2(GS_LOCAL_SUB,"=GS=  gs_clear_timer_ext queue_id %d timer_id %d", queue_id,timer_id);

  if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
  {
    rex_enter_crit_sect(&timer_crit_sect);

    if(task_timer_info[queue_id][timer_id].timer != NULL 
#ifdef FEATURE_GS_MULTIPLEX_TIMER
     && task_timer_info[queue_id][timer_id].timer_type == GS_LEGACY
#endif
    )
    {
      /* Deallocate timer */
      (void)timer_undef((timer_type *)task_timer_info[queue_id][timer_id].timer); 
      modem_mem_free(task_timer_info[queue_id][timer_id].timer, MODEM_MEM_CLIENT_NAS);

      /* Make sure to mark this timer as inactive */
      task_timer_info[queue_id][timer_id].timer = NULL;
    }
    else
    {
      /* Raise error condition */
      ERR("ERROR: Attempting to stop invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);
    }

    rex_leave_crit_sect(&timer_crit_sect);
  }
  else
  {
    /* Raise error condition */
    ERR("ERROR: Invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);
  }
}

/*******************************************************************************
 *
 *  Function name: gs_cancel_timer
 *  ------------------------------
 *  Description:
 *  ------------
 *  Cancels a timer. This function is left in for backward compatibility
 *  with existing code. It simply calls gs_modify_timer with the 
 *  appropriate parameter
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/

gs_status_T gs_cancel_timer(gs_timer_queue_id_T   queue_id,
                            timer_id_T      timer_id)
{
    gs_status_T status = GS_FAILURE;

    if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
    {
        rex_enter_crit_sect(&timer_crit_sect);

        /* Check to see if timer exists */
        if(task_timer_info[queue_id][timer_id].timer != NULL)
        {
           /* Stop timer */
           (void) rex_clr_timer(task_timer_info[queue_id][timer_id].timer);

           /* Deallocate timer */
           rex_delete_timer_ex(task_timer_info[queue_id][timer_id].timer);
        
           /* Make sure to mark this timer as inactive */
           task_timer_info[queue_id][timer_id].timer = NULL;
        }

        rex_leave_crit_sect(&timer_crit_sect);

       status = GS_SUCCESS;
    }
    else
    {
       /* Raise error condition */
       ERR("ERROR: Invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);

       status = GS_FAILURE;
    }

    /* Return Success */
    return status;
}
#ifdef FEATURE_GS_MULTIPLEX_TIMER
gs_status_T gs_cancel_multi_timer_ext(gs_timer_queue_id_T   queue_id,
                            timer_id_T      timer_id, boolean timer_non_def)
{ 
  gs_status_T gs_status = GS_FAILURE;
  
  timer_client_info_nas_T *task_info_head;
  uint64 curr_timer, delta;
  if(timer_non_def == TRUE)
  {
    task_info_head = task_timer_info_non_def_head;
   curr_timer = timer_get_64(&gs_timers_multiplex_non_def_timer, T_MSEC);
  }
  else
  {
    task_info_head = task_timer_info_head;
	curr_timer = timer_get_64(&gs_timers_multiplex_timer, T_MSEC);
  }
  if(task_info_head== NULL)
  { 
    return gs_status;
  }
  while(task_info_head!= NULL)
  {
    if(task_info_head->queue_id == queue_id && task_info_head->timer_id == timer_id)
    {
      break;
    }
	else
	{
	  task_info_head= task_info_head->link.next_ptr;
	}
  }
  if(task_info_head != NULL)
  {
    timer_client_info_nas_T *task_info_prev = NULL;
    timer_client_info_nas_T *task_info_next = NULL;
    task_info_prev = task_info_head->link.prev_ptr;
    task_info_next = task_info_head->link.next_ptr;
    if(task_info_prev!=NULL)
    {
      task_info_prev->link.next_ptr = task_info_next;
      if(task_info_next != NULL)
      task_info_next->link.prev_ptr = task_info_prev;
      modem_mem_free(task_info_head, MODEM_MEM_CLIENT_NAS);
      if(task_timer_info[queue_id][timer_id].timer != NULL)
      {
        (void)timer_undef((timer_type *)task_timer_info[queue_id][timer_id].timer);        
         /* Make sure to mark this timer as inactive */
         task_timer_info[queue_id][timer_id].timer = NULL;
      }
     }
     else
	{
	  modem_mem_free(task_info_head, MODEM_MEM_CLIENT_NAS);
	  if(task_timer_info[queue_id][timer_id].timer != NULL)
	  {
		(void)timer_undef((timer_type *)task_timer_info[queue_id][timer_id].timer);		  
		 /* Make sure to mark this timer as inactive */
		 task_timer_info[queue_id][timer_id].timer = NULL;
	  }
	  if(task_info_next == NULL)
	  {
		  if(timer_non_def == TRUE)
          {
            task_timer_info_non_def_head = NULL;
          }
          else
          {
            task_timer_info_head = NULL;
          }
	  }
	  else
	  {
	    if(timer_non_def == TRUE)
        {
          task_timer_info_non_def_head = task_info_head= task_info_next;
		 
        }
        else
        {
            task_timer_info_head =  task_info_head = task_info_next;
        }
		delta = task_info_head->timer_count;
		task_info_head->timer_count += curr_timer;
		
		gs_start_multiplex_timer(task_info_head->timer_count,timer_non_def);
		task_info_head->link.prev_ptr = NULL;
		task_info_next = task_info_head->link.next_ptr;
		curr_timer = delta;
		while(task_info_next)
	    {
	      task_info_next->timer_count = (task_info_next->timer_count - curr_timer);
		  task_info_next = task_info_next->link.next_ptr;
	    }
	    task_info_head->timer_count = 0;

	  }
	  
	}
	gs_status = GS_SUCCESS;
  }
  return gs_status;
}
#endif
/*******************************************************************************
 *
 *  Function name: gs_cancel_timer_ext
 *  ------------------------------
 *  Description:
 *  ------------
 *  Generic timer cancel function. Takes extra parameter to specify if 
 *  the timer is non-deferrable
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/

gs_status_T gs_cancel_timer_ext
(
  gs_timer_queue_id_T  queue_id,
   timer_id_T        timer_id
)
{
	gs_status_T status = GS_FAILURE;
	gs_add_debug_buffer(queue_id,timer_id,NULL,0,0,0,TIMER_CANCLE);
	MSG_HIGH_DS_2(GS_LOCAL_SUB,"=GS=  gs_cancel_timer_ext queue_id %d timer_id %d", queue_id,timer_id);

	if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
	{
	  rex_enter_crit_sect(&timer_crit_sect);
	
	  /* Check to see if timer exists */
	  if(task_timer_info[queue_id][timer_id].timer != NULL)
	  {
		#ifdef FEATURE_GS_MULTIPLEX_TIMER
	      MSG_HIGH_DS_1(GS_LOCAL_SUB,"=GS=  gs_cancel_timer_ext timer type %d", task_timer_info[queue_id][timer_id].timer_type);
          if(task_timer_info[queue_id][timer_id].timer_type == GS_LEGACY)
		#endif
          {
             /* Deallocate timer memory */
             (void)timer_undef((timer_type *)task_timer_info[queue_id][timer_id].timer); 
             modem_mem_free(task_timer_info[queue_id][timer_id].timer, MODEM_MEM_CLIENT_NAS);
             /* Make sure to mark this timer as inactive */
             task_timer_info[queue_id][timer_id].timer = NULL;
             status = GS_SUCCESS;
          }
#ifdef FEATURE_GS_MULTIPLEX_TIMER
          else if(task_timer_info[queue_id][timer_id].timer_type == GS_MULTIPLEX_DEF)
	      {
             status= gs_cancel_multi_timer_ext(queue_id,timer_id,FALSE);
          }
          else if(task_timer_info[queue_id][timer_id].timer_type == GS_MULTIPLEX_NON_DEF)
          {
             status= gs_cancel_multi_timer_ext(queue_id,timer_id,TRUE);
          }
          else
          {
			/* Raise error condition */
			ERR("ERROR: Attempting to stop invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);
          }
#endif
	  }
	  else
	  {
			/* Raise error condition */
		ERR("ERROR: Invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);
	
		 status = GS_FAILURE;
	  }
	  rex_leave_crit_sect(&timer_crit_sect);
	}
	else
	{
	  /* Raise error condition */
	  ERR("ERROR: Invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);
	
	  status = GS_FAILURE;
	}
	
	/* Return Success */
	return status;
}
/*******************************************************************************
 *
 *  Function name: gs_modify_timer
 *  ------------------------------
 *  Description:
 *  ------------
 *  Modifies a timer, by sending a ModifyTimer message to the timer task,
 *  with an appropriate parameter.
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *  IN         action
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/

gs_status_T gs_modify_timer( gs_timer_queue_id_T         queue_id,
                             timer_id_T            timer_id,
                             timer_modify_action_T action)
{
    gs_status_T   status = GS_FAILURE;

    if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
    {
        rex_enter_crit_sect(&timer_crit_sect);

        /* Check to see if timer exists */
        if(task_timer_info[queue_id][timer_id].timer != NULL)
        {
          switch(action)
          {
            case TIMER_MODIFY_PAUSE:
              rex_pause_timer(task_timer_info[queue_id][timer_id].timer);
              break;
                     
            case TIMER_MODIFY_RESUME:
              rex_resume_timer(task_timer_info[queue_id][timer_id].timer);
              break;
                     
            case TIMER_MODIFY_CANCEL:
              (void) gs_cancel_timer(queue_id, timer_id);
              break;

            case TIMER_MODIFY_ENQUIRE:
              ERR("ERR: Invalid timer action request %d",action,0,0);
              break;
                     
            default:
              ERR("ERR: Invalid timer action request %d",action,0,0);
              break;
          }
        }

       rex_leave_crit_sect(&timer_crit_sect);

       status = GS_SUCCESS;
    }     
    else
    {
       /* Raise error condition */
       ERR("ERROR: Invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);

       status = GS_FAILURE;
    }

    return status;
}
#ifdef FEATURE_GS_MULTIPLEX_TIMER
gs_status_T gs_modify_multiplex_timer_ext
( 
  gs_timer_queue_id_T    queue_id,
  timer_id_T             timer_id,
  GS_timer_type        timer_typ,
  timer_modify_action_T    action
)
{
  timer_client_info_nas_T *task_info_head;
  timer_client_info_nas_T *task_info_prev=NULL;
  timer_client_info_nas_T *client_info = NULL;
  gs_status_T gs_status = GS_FAILURE;

  boolean  non_def = FALSE;
  uint64 curr_timer;
  unsigned long param;
  timer_type *timer_p = NULL;
  if(timer_typ == GS_MULTIPLEX_DEF)
  {
    task_info_head = task_timer_info_head;
	non_def = FALSE;
	curr_timer = timer_get_64(&gs_timers_multiplex_timer, T_MSEC);
  }
  else if(timer_typ == GS_MULTIPLEX_NON_DEF)
  {
    task_info_head = task_timer_info_non_def_head;
	non_def = TRUE;
	curr_timer = timer_get_64(&gs_timers_multiplex_non_def_timer, T_MSEC);
  }
  else
  {
    ERR("ERR: Invalid timer action request %d %d %don Multiplex",action, queue_id,timer_id);
	return gs_status;
  }
  while(task_info_head!= NULL)
  {
    if(task_info_head->queue_id == queue_id && task_info_head->timer_id == timer_id)
    {
      break;
    }
	else
	{
	  task_info_prev = task_info_head;
	  task_info_head = task_info_head->link.next_ptr;
	}
  }
  if(task_info_head!=NULL)
  {
    
	client_info = (timer_client_info_nas_T *)modem_mem_calloc( 1, sizeof( timer_client_info_nas_T ), MODEM_MEM_CLIENT_NAS );
	if(client_info != NULL)
	{
	  client_info->queue_id = queue_id;
	  client_info->timer_id = timer_id;
	  client_info->cb_ptr = task_info_head->cb_ptr;
      client_info->timer_count = task_info_head->timer_count + curr_timer;
	  client_info->non_deferrable = non_def;
	  client_info->data = task_info_head->data;
    }
	gs_status = gs_cancel_multi_timer_ext(queue_id,timer_id,non_def);
	if(gs_status == GS_SUCCESS)
	{
	        /* Check to see if timer already exists */
      if(task_timer_info[queue_id][timer_id].timer != NULL)
      {
        /* Deallocate timer memory */
        (void)timer_undef((timer_type *)task_timer_info[queue_id][timer_id].timer);        
         modem_mem_free(task_timer_info[queue_id][timer_id].timer, MODEM_MEM_CLIENT_NAS);
    
        /* Make sure to mark this timer as inactive */
         task_timer_info[queue_id][timer_id].timer = NULL;
      }
    
      /* Copy data to param */
      param     = client_info->data ;
    
      /* Copy timer id to param */
      param |= client_info->timer_id  << 16;
    
      /* Allocate timer */
      timer_p = (timer_type *) modem_mem_calloc( 1, sizeof( timer_type ), MODEM_MEM_CLIENT_NAS );
      if(NULL != timer_p)
      {
        task_timer_info[queue_id][timer_id].timer = timer_p;
		#ifdef FEATURE_GS_MULTIPLEX_TIMER
		task_timer_info[queue_id][timer_id].timer_type = GS_LEGACY;
		#endif
        gs_status = gs_start_multiplex_timer_ext(NULL,timer_p,client_info->cb_ptr, client_info->timer_count,param,client_info->non_deferrable,FALSE);
		timer_pause(task_timer_info[queue_id][timer_id].timer);
      }
      else
      {
         ERR("ERR: Memory not available to allocate for timer.",0,0,0);
         gs_status = GS_FAILURE;
      }
	  
	}
	else
	{
	    ERR("ERR: Invalid timer action request %d %d %don Multiplex",action, queue_id,timer_id);
	   gs_status = GS_FAILURE;;
	}
	modem_mem_free(client_info, MODEM_MEM_CLIENT_NAS);
  }
  else
  {
    ERR("ERR: Invalid timer action request %d %d %don Multiplex",action, queue_id,timer_id);
	gs_status = GS_FAILURE;;
  }

  return gs_status;
}
#endif
/*******************************************************************************
 *
 *  Function name: gs_modify_timer_ext
 *  ------------------------------
 *  Description:
 *  ------------
 *  Generic timer modify function. Takes extra parameter to specify if 
 *  the timer is non-deferrable
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *  IN         action
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/

gs_status_T gs_modify_timer_ext
(
   gs_timer_queue_id_T            queue_id,
   timer_id_T               timer_id,
   timer_modify_action_T    action
)
{
	gs_status_T   status = GS_FAILURE;
	gs_add_debug_buffer(queue_id,timer_id,NULL,0,(byte)action,0,TIMER_MODIFY);
	MSG_HIGH_DS_2(GS_LOCAL_SUB,"=GS=  gs_modify_timer_ext queue_id %d timer_id %d", queue_id,timer_id);

	if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
	{
	  rex_enter_crit_sect(&timer_crit_sect);
	
	  /* Check to see if timer exists */
	  if(task_timer_info[queue_id][timer_id].timer != NULL)
	  {
		#ifdef FEATURE_GS_MULTIPLEX_TIMER
		MSG_HIGH_DS_1(GS_LOCAL_SUB,"=GS=  gs_modify_timer_ext  timer type %d", task_timer_info[queue_id][timer_id].timer_type);
		#endif
#ifdef FEATURE_GS_MULTIPLEX_TIMER
		if(task_timer_info[queue_id][timer_id].timer_type != GS_LEGACY &&
		   action == TIMER_MODIFY_PAUSE )
		{ 
		  status = gs_modify_multiplex_timer_ext(queue_id,timer_id,task_timer_info[queue_id][timer_id].timer_type,action);
		}
		else
#endif
		{
		  switch(action)
		  {
			case TIMER_MODIFY_PAUSE:
			  timer_pause(task_timer_info[queue_id][timer_id].timer);
			  break;
					   
			case TIMER_MODIFY_RESUME:
			  timer_resume(task_timer_info[queue_id][timer_id].timer);
			  break;
					   
			case TIMER_MODIFY_CANCEL:
			  (void) gs_cancel_timer_ext(queue_id, timer_id);
			  break;
	
			case TIMER_MODIFY_ENQUIRE:
			   ERR("ERR: Invalid timer action request %d",action,0,0);
			   break;
					   
			default:
			   ERR("ERR: Invalid timer action request %d",action,0,0);
			   break;
		   }
		   status = GS_SUCCESS;
		}
	  }
	
	  rex_leave_crit_sect(&timer_crit_sect);
	}	  
	else
	{
	  /* Raise error condition */
	  ERR("ERROR: Invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);
	
	  status = GS_FAILURE;
	}
	
    /* Return Success */
	return status;
}

/*******************************************************************************
 *
 *  Function name: gs_timer_exists
 *  -------------------------------
 *  Description:
 *  ------------
 *  This function checks if a timer is valid (Not NULL) based on the passed
 *  queue_id and timer_id. 
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *
 *  Returns:
 *  --------
 *  TRUE = IF the timer is valid, hence FALSE
 *
 ******************************************************************************/
boolean gs_timer_exists
(
   gs_timer_queue_id_T queue_id,
   timer_id_T    timer_id
)
{
    boolean timer_exists = FALSE;
      
    if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
    {
        rex_enter_crit_sect(&timer_crit_sect);

        /* Check to see if timer exists */
        if (task_timer_info[queue_id][timer_id].timer != NULL)
        {
           timer_exists = TRUE;
        }   
  
        rex_leave_crit_sect(&timer_crit_sect);
    }
    else
    {
        timer_exists = FALSE;
    }

    return timer_exists;
}

/*******************************************************************************
 *
 *  Function name: gs_enquire_timer
 *  -------------------------------
 *  Description:
 *  ------------
 *  Requests details of the current state of a timer. It is recommended
 *  that the timer is paused before this call is made.
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/

milli_secs_T gs_enquire_timer
(
   gs_timer_queue_id_T queue_id,
   timer_id_T    timer_id
)
{
    milli_secs_T time_remaining;

    if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
    {
      
        rex_enter_crit_sect(&timer_crit_sect);

        /* Check to see if timer exists */
        if (task_timer_info[queue_id][timer_id].timer != NULL)
        {
           time_remaining = (milli_secs_T) rex_get_timer( task_timer_info[queue_id][timer_id].timer );
        }
        else
        {
           time_remaining = 0;
        }

        rex_leave_crit_sect(&timer_crit_sect);
    }
    else
    {
        time_remaining = 0;
    }  

    return time_remaining;
}
#ifdef FEATURE_GS_MULTIPLEX_TIMER
uint64 gs_enquire_mult_timer_ext(	gs_timer_queue_id_T queue_id,
                                        timer_id_T			 timer_id,
                                        GS_timer_type        timer_type)
{
    timer_client_info_nas_T *task_info_head;
    uint64 curr_timer;
    if(timer_type == GS_MULTIPLEX_NON_DEF)
    {
      task_info_head = task_timer_info_non_def_head;
      curr_timer = timer_get_64(&gs_timers_multiplex_non_def_timer, T_MSEC);
      MSG_HIGH_DS_1(GS_LOCAL_SUB,"=GS= Expired gs_enquire_mult_timer_ext curr_timer time %lu ", curr_timer);

    }
    else if(timer_type == GS_MULTIPLEX_DEF)
    {
      task_info_head = task_timer_info_head;
      curr_timer = timer_get_64(&gs_timers_multiplex_timer, T_MSEC);
      MSG_HIGH_DS_1(GS_LOCAL_SUB,"=GS= Expired gs_enquire_mult_timer_ext curr_timer time %lu ", curr_timer);

    }
	while(task_info_head!=NULL)
	{
         if(task_info_head->queue_id == queue_id && task_info_head->timer_id == timer_id)
         {
          break;
          }
	  else
	  {
	    task_info_head= task_info_head->link.next_ptr;
	  }
        }
	

	if(task_info_head!=NULL)
	{
          MSG_HIGH_DS_1(GS_LOCAL_SUB,"=GS= Expired gs_enquire_mult_timer_ext curr_timer time %lu ", (curr_timer+ task_info_head->timer_count));
	  return (curr_timer+ task_info_head->timer_count);
	}
	return 0;
}
#endif
/*******************************************************************************
 *
 *  Function name: gs_enquire_timer_ext
 *  -------------------------------
 *  Description:
 *  ------------
 *  Generic timer enquire function. Takes extra parameter to specify if 
 *  the timer is non-deferrable
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/

uint64 gs_enquire_timer_ext
(
  gs_timer_queue_id_T queue_id,
  timer_id_T           timer_id
)
{
	uint64 time_remaining;
	gs_add_debug_buffer(queue_id,timer_id,NULL,0,0,0,TIMER_CANCLE);
	MSG_HIGH_DS_2(GS_LOCAL_SUB,"=GS=  gs_enquire_timer_ext %lu queue_id %d timer_id %d", queue_id,timer_id);
	
	if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
	{
	  rex_enter_crit_sect(&timer_crit_sect);
#ifdef FEATURE_GS_MULTIPLEX_TIMER
	  MSG_HIGH_DS_1(GS_LOCAL_SUB,"=GS= gs_enquire_timer_ext timer_type %d", task_timer_info[queue_id][timer_id].timer_type);
	  if(task_timer_info[queue_id][timer_id].timer_type == GS_LEGACY)
	  {
#endif
		/* Check to see if timer exists */
		if (task_timer_info[queue_id][timer_id].timer != NULL)
		{
		  time_remaining = (uint64) timer_get_64(task_timer_info[queue_id][timer_id].timer, T_MSEC);
		}
		else
		{
		  time_remaining = 0;
		}
#ifdef FEATURE_GS_MULTIPLEX_TIMER
	  }
	  else
	  {
		time_remaining =  gs_enquire_mult_timer_ext
						  (
						   queue_id,
						   timer_id,
						   task_timer_info[queue_id][timer_id].timer_type
						  );
	   }
#endif
	  rex_leave_crit_sect(&timer_crit_sect);
	}
	else
	{
	  time_remaining = 0;
	}  
	MSG_HIGH_DS_1(GS_LOCAL_SUB,"=GS= Expired gs_enquire_timer_ext remaining time %lu ", time_remaining);
		
	return time_remaining;
}
