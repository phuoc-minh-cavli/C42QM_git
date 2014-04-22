/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostics User space call processing

General Description
  Routines to handle User space calls using DAM framework

Copyright (c) 2017, 2019-2020 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            Edit History

  $Header: //components/rel/core.tx/6.0/services/diag/LSM/qapi/src/qapi_diag.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/12/17   gn      Fixed issue in QAPI calls from Kernel space
10/31/17   gn      Changes to support MMU enablement and command deregistration
04/28/17   gn      Initialized qapi_status in get_qapi_status
03/24/17   gn      Changes to support DIAG delayed reponses on User space
02/24/17   gn      Changes to support DIAG QAPIs with DAM framework
===========================================================================*/

#ifdef QAPI_TXM_SOURCE_CODE

#include "tx_api.h"
#include "qurt_txm_qapi_fwk.h"
#include "qapi_diag.h"
#include "qapi_diag_codes.h"
#include "comdef.h"
#include "stdarg.h"
#include "diagdiag_v.h"
#include "diagi_v.h"

void *diagpkt_user_space_fp[DIAGPKT_USER_SPACE_TBL_SIZE];
extern uint8 diagpkt_user_space_tbl_used;

/* Internal function. Get QAPI status codes from boolean */
qapi_Status_t get_qapi_status(boolean status)
{
   qapi_Status_t qapi_status = QAPI_ERROR;
   switch(status)
   {
      case TRUE:
         qapi_status = QAPI_OK;
         break;
      case FALSE:
         qapi_status = QAPI_ERROR;
         break;
	  default:
	  	 break;
   }
   return qapi_status;
}

/* Diag handler called by kernel when a DIAG QAPI is called from user space */
qapi_Status_t qapi_diag_handler(uint32_t request_ID, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, uint32_t arg6, uint32_t arg7, uint32_t arg8, uint32_t arg9, uint32_t arg10, uint32_t arg11, uint32_t arg12)
{
  qapi_Status_t ret_val;
  int qurt_ret_val = 0;
#ifdef FEATURE_MMU_ENABLED  
  void *user_cb = NULL;
  void *manager_cb = NULL;
#endif 

  diagpkt_master_table_type *tbl_ptr = NULL;
  diagpkt_user_space_table_type *user_tbl_ptr = NULL;

  switch(request_ID) {
	  
  case TXM_QAPI_DIAG_DISPATCH_TABLE_REGISTER:
	tbl_ptr=  (diagpkt_master_table_type*)malloc(sizeof(diagpkt_master_table_type));
	if(!tbl_ptr) // Heap is exhausted
	  return QAPI_ERROR;
	
	user_tbl_ptr = (diagpkt_user_space_table_type*)malloc(sizeof(diagpkt_user_space_table_type));
	if(!user_tbl_ptr) // Heap is exhausted
	{
	  free(tbl_ptr);
	  return QAPI_ERROR;
	}  

	qurt_ret_val = qurt_copy_from_user((void*)tbl_ptr, sizeof(diagpkt_master_table_type), (void*)arg1 , sizeof(diagpkt_master_table_type));
    if(qurt_ret_val!=QURT_EOK)
    {
        free(tbl_ptr);
		free(user_tbl_ptr);
		return QAPI_ERROR;
    }	

	qurt_ret_val = qurt_copy_from_user((void*)user_tbl_ptr, sizeof(diagpkt_user_space_table_type), (void*)arg2 , sizeof(diagpkt_user_space_table_type));
    if(qurt_ret_val!=QURT_EOK)
    {
        free(tbl_ptr);
		free(user_tbl_ptr);
		return QAPI_ERROR;
    }	

	
#ifdef FEATURE_MMU_ENABLED  
    qurt_ret_val = qurt_copy_from_user((void*)&user_cb, sizeof(tbl_ptr->user_table->user_func_ptr), (void*)&(tbl_ptr->user_table->user_func_ptr), sizeof(tbl_ptr->user_table->user_func_ptr));
    if(qurt_ret_val!=QURT_EOK)
    {
      free(tbl_ptr);
	  free(user_tbl_ptr);
	  return QAPI_ERROR;
    }  

    /* configure the user callback and get mapped callback that can be used in kernel space
	and save it to diag_pkt_user_space_table_entry */
    qurt_qapi_callback_configure(QC_DIAG_CALLBACK_CMD_RSP_ID, // Callback ID
	  					   (void *)tbl_ptr, // Use master table entry pointer as handle
						   0,	// id (unused)
						   (void*)user_cb, // user callback
						   (void *)arg3, // diagpkt_user_space_callback
						   (void**)&manager_cb, // Mapped callback to be used in kernel space
						   NULL);		
	/* Save the manager callback returned by module manager to user_tbl_ptr */   
	/* Not using qurt_copy_to/from_user as this is a copy from kernel space to kernel space */
	diagpkt_user_space_fp[diagpkt_user_space_tbl_used] = manager_cb;

	/* Not using qurt_copy_to/from_user as this is a copy from kernel space to kernel space */
	user_tbl_ptr->user_func_ptr =  (uint32 (*)(uint32))diagpkt_user_space_fp[diagpkt_user_space_tbl_used];

#else	
    /* Save the user_cb provided by the client in user_tbl_ptr if MMU is not enabled */
    qurt_ret_val = qurt_copy_from_user((void*)&(user_tbl_ptr->user_func_ptr), sizeof(user_tbl_ptr->user_func_ptr), (void*)&(tbl_ptr->user_table->user_func_ptr), sizeof(tbl_ptr->user_table->user_func_ptr);
    if(qurt_ret_val!=QURT_EOK)
    {
      free(tbl_ptr);
	  free(user_tbl_ptr);
	  return QAPI_ERROR;
    }  
#endif	
	ret_val = qapi_user_space_tbl_reg_append_proc(tbl_ptr, user_tbl_ptr);
	break;

  case TXM_QAPI_DIAGPKT_GET_NEXT_DELAYED_RSP_ID:
	ret_val = qapi_diagpkt_get_next_delayed_rsp_id((uint16_t*)arg1);
	break;

  case TXM_QAPI_DIAGPKT_COMMIT:
	ret_val = qapi_diagpkt_commit((uint32_t*)arg1, (uint32_t) arg2, (uint32_t) arg3);
	break;

  case TXM_QAPI_DIAG_DISPATCH_TABLE_DEREGISTER:
	ret_val = qapi_user_space_tbl_dereg((diagpkt_user_table_entry_type*)arg1);
	break;	
	
  case TXM_QAPI_DIAG_MSG:
	ret_val = qapi_msg_send((const msg_const_type *)arg1, (uint32_t)arg2, (uint32_t)arg3, (uint32_t)arg4, (uint32_t)arg5, (uint32_t)arg6, (uint32_t)arg7, (uint32_t)arg8, (uint32_t)arg9, (uint32_t)arg10, (uint32_t)arg11, (uint32_t)arg12);
	break;
  
  case TXM_QAPI_DIAG_MSG_SPRINTF:
	ret_val = qapi_msg_sprintf((const msg_const_type *)arg1, (uint32_t)arg2, (uint32_t)arg3, (uint32_t)arg4, (uint32_t)arg5, (uint32_t)arg6, (uint32_t)arg7, (uint32_t)arg8, (uint32_t)arg9, (uint32_t)arg10, (uint32_t)arg11, (uint32_t)arg12);
	break;

  case TXM_QAPI_DIAG_LOG_SUBMIT:
	ret_val = qapi_log_submit((void*)arg1);
	break;

  case TXM_QAPI_DIAG_LOG_SET_LENGTH:
	ret_val = qapi_log_set_length((void*)arg1, (uint16)arg2);
	break;

  case TXM_QAPI_DIAG_LOG_SET_CODE:
	ret_val = qapi_log_set_code((void*)arg1, (log_code_type)arg2);
	break;

  case TXM_QAPI_DIAG_LOG_SET_TIMESTAMP:
	ret_val = qapi_log_set_timestamp((void*)arg1);
	break;

  case TXM_QAPI_DIAG_LOG_STATUS:
	ret_val = qapi_log_status((log_code_type)arg1);
	break;

  case TXM_QAPI_DIAG_EVENT_REPORT:
	ret_val = qapi_event_report((event_id_enum_type)arg1);
	break;

  case TXM_QAPI_DIAG_EVENT_REPORT_PAYLOAD:
	ret_val = qapi_event_report_payload((event_id_enum_type)arg1, (uint8)arg2, (void*)arg3);
	break;
	
  default:
	ret_val = QAPI_ERROR;
	break;
  }
  return ret_val;
}

/* Shim API for user_space_tbl_reg_append_proc */
qapi_Status_t qapi_user_space_tbl_reg_append_proc (diagpkt_master_table_type * tbl_ptr, diagpkt_user_space_table_type * user_space_tbl_ptr)
{
   qapi_Status_t ret_val = QAPI_ERROR;

   
   diagpkt_user_space_tbl_reg_append_proc(tbl_ptr,user_space_tbl_ptr);
   ret_val = QAPI_OK;
    
   return ret_val;
}

/* Shim API for diagpkt_get_next_delayed_rsp_id */
qapi_Status_t qapi_diagpkt_get_next_delayed_rsp_id(uint16_t* delay_rsp_id)
{
   qapi_Status_t ret_val = QAPI_ERROR;

   #ifdef FEATURE_MMU_ENABLED
     int qurt_ret_val = QURT_EOK;
     uint16 id = 0;
   #endif 

   if(delay_rsp_id)
   {
     /* This if case handles the case when the qapi is called from user space */
     #ifdef FEATURE_MMU_ENABLED
     if(!qurt_check_if_kernel_address((void*)delay_rsp_id))
     {
       id = diagpkt_get_delayed_rsp_id();
	   qurt_ret_val = qurt_copy_to_user((void*)delay_rsp_id,sizeof(uint16),(void*)&id,sizeof(uint16));
       if(qurt_ret_val!=QURT_EOK)
	 	 return QAPI_ERROR;
     } 
	 else // If qapi is called from kernel space
	 #endif /* FEATURE_MMU_ENABLED */
	 {
	   *delay_rsp_id = diagpkt_get_delayed_rsp_id();;
	 }

	 ret_val = QAPI_OK;
   }
   
   return ret_val;
}

/* Shim API for diagpkt_user_space_commit */
qapi_Status_t qapi_diagpkt_commit(uint32_t* outbuf, uint32_t rsp_len, uint32_t rsp_flag)
{
   qapi_Status_t ret_val = QAPI_ERROR;
   
   if(outbuf && rsp_len>0)
   {
	 if(diagpkt_user_space_commit(outbuf, rsp_len, rsp_flag))
		 ret_val = QAPI_OK;
   }   
   return ret_val;
}

/* Shim API for diagpkt_user_space_tbl_dereg */
qapi_Status_t qapi_user_space_tbl_dereg (diagpkt_user_table_entry_type * tbl_ptr)
{
   qapi_Status_t ret_val = QAPI_ERROR;

   if(tbl_ptr)
   {
     diagpkt_user_space_tbl_dereg(tbl_ptr);
	 ret_val = QAPI_OK;
   }   
   return ret_val;
}


/* Shim API for msg_send */
qapi_Status_t qapi_msg_send(const msg_const_type * const_blk, uint32_t num_args, ...)
{
	qapi_Status_t ret_val = QAPI_ERROR;
	va_list arg_list;
	int i = 0;
	uint32_t arg[9];
	uint8 args_count = MIN(num_args,9);
	#ifdef FEATURE_MMU_ENABLED
	  int qurt_ret_val = QURT_EOK;
    #endif /* FEATURE_MMU_ENABLED */
	if(const_blk)
	{
	    #ifdef FEATURE_MMU_ENABLED
	    if(!qurt_check_if_kernel_address((void*)const_blk))
	    {
	      /* Checking if the const structure is within the address space of the module */
		  qurt_ret_val = qurt_check_if_module_address((qurt_addr_t)const_blk,sizeof(const msg_const_type*));
		  /* Return error if the const structure is not within the range of the moduele address space */
		  if(qurt_ret_val!=TRUE)
		    return QAPI_ERROR;
	    }  
		#endif /* FEATURE_MMU_ENABLED */

		va_start( arg_list, num_args );
		for (i = 0; i < args_count; i++)
		{
			arg[i] = va_arg(arg_list, uint32_t);
		}
		va_end(arg_list);

		switch(args_count)
		{
			case 0:
				msg_send(const_blk);
				break;
			case 1:
				msg_send_1(const_blk, arg[0]);
				break;	
			case 2:
				msg_send_2(const_blk, arg[0], arg[1]);
				break;	
			case 3:
				msg_send_3(const_blk, arg[0], arg[1], arg[2]);
				break;	
			case 4:
				msg_send_var(const_blk, args_count, arg[0], arg[1], arg[2], arg[3]);
				break;	
			case 5:
				msg_send_var(const_blk, args_count, arg[0], arg[1], arg[2], arg[3], arg[4]);
				break;	
			case 6:
				msg_send_var(const_blk, args_count, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]);
				break;	
			case 7:
				msg_send_var(const_blk, args_count, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6]);
				break;	
			case 8:
				msg_send_var(const_blk, args_count, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6], arg[7]);
				break;
			case 9:
				msg_send_var(const_blk, args_count, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8]);
				break;					
		}		
		ret_val =  QAPI_OK;
	}
	return ret_val;
}

/* Shim API for msg_sprintf */
qapi_Status_t qapi_msg_sprintf(const msg_const_type * const_blk, uint32_t num_args, ...)
{
	qapi_Status_t ret_val = QAPI_ERROR;
	va_list arg_list;
	int i = 0;
	uint32_t arg[9] = {0};
	uint8 args_count = MIN(num_args,9);
	#ifdef FEATURE_MMU_ENABLED
	  int qurt_ret_val = QURT_EOK;
	#endif 
	
	if(const_blk)
	{
	    #ifdef FEATURE_MMU_ENABLED
	    if(!qurt_check_if_kernel_address((void*)const_blk))
	    {
	      /* Checming if the const structure is within the address space of the module */
		  qurt_ret_val = qurt_check_if_module_address((qurt_addr_t)const_blk,sizeof(const msg_const_type*));
		  /* Return error if the const structure is not within the range of the moduele address space */
		  if(qurt_ret_val!=TRUE)
		    return QAPI_ERROR;
	    }  
		#endif /* FEATURE_MMU_ENABLED */

		va_start( arg_list, num_args );
		for (i = 0; i < args_count; i++)
		{
			arg[i] = va_arg(arg_list, uint32_t);
		}
		va_end(arg_list);

		switch(args_count)
		{
			case 0:
				msg_sprintf(const_blk);
				break;
			case 1:
				msg_sprintf(const_blk, arg[0]);
				break;	
			case 2:
				msg_sprintf(const_blk, arg[0], arg[1]);
				break;	
			case 3:
				msg_sprintf(const_blk, arg[0], arg[1], arg[2]);
				break;	
			case 4:
				msg_sprintf(const_blk, arg[0], arg[1], arg[2], arg[3]);
				break;	
			case 5:
				msg_sprintf(const_blk, arg[0], arg[1], arg[2], arg[3], arg[4]);
				break;	
			case 6:
				msg_sprintf(const_blk, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]);
				break;	
			case 7:
				msg_sprintf(const_blk, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6]);
				break;	
			case 8:
				msg_sprintf(const_blk, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6], arg[7]);
				break;
			case 9:
				msg_sprintf(const_blk, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8]);
				break;					
		}		
		ret_val =  QAPI_OK;
	}
	return ret_val;
}

/* Shim API for log_submit */
qapi_Status_t qapi_log_submit(void* ptr)
{
	boolean ret_val = FALSE;
	log_header_type log_hdr; // log header will be copied to this pointer
	int qurt_ret_val = QURT_EOK;
	if(ptr)
	{
	  /* Copying the log header */
	  qurt_ret_val = qurt_copy_from_user((void*)&log_hdr,sizeof(log_header_type),ptr,sizeof(log_header_type));
	  if(qurt_ret_val!=QURT_EOK)
	  	return QAPI_ERROR;
	  ret_val = log_submit(&log_hdr);	  
	}
	return (get_qapi_status(ret_val));
}

/* Shim API for log_set_length */
qapi_Status_t qapi_log_set_length(void* ptr, uint16 length)
{
	qapi_Status_t ret_val = QAPI_ERROR;
	#ifdef FEATURE_MMU_ENABLED
	  log_header_type log_hdr; // log header will be copied to this pointer
	  int qurt_ret_val = QURT_EOK;
	#endif /* FEATURE_MMU_ENABLED */
	if(ptr)
	{
	    // If qapi is called from user space
	    #ifdef FEATURE_MMU_ENABLED
	    if(!qurt_check_if_kernel_address(ptr))
	    {
	      /* Copying the log header */
	      qurt_ret_val = qurt_copy_from_user((void*)&log_hdr,sizeof(log_header_type),ptr,sizeof(log_header_type));
		  if(qurt_ret_val!=QURT_EOK)
			return QAPI_ERROR;
		  log_set_length(&log_hdr, length);

		  /* Now copy the log header back to user space */
		  qurt_ret_val = qurt_copy_to_user((void*)ptr,sizeof(log_header_type),(void*)&log_hdr,sizeof(log_header_type));
		  if(qurt_ret_val!=QURT_EOK)
			return QAPI_ERROR;
		}
		else // qapi called from kernel
		#endif /* FEATURE_MMU_ENABLED */
		{
		  log_set_length(ptr,length);
		}
		
		ret_val =  QAPI_OK;
	}
	return ret_val;
}

/* Shim API for log_set_code */
qapi_Status_t qapi_log_set_code(void* ptr, log_code_type code)
{
	qapi_Status_t ret_val = QAPI_ERROR;
	#ifdef FEATURE_MMU_ENABLED
	  log_header_type log_hdr; // log header will be copied to this pointer
	  int qurt_ret_val = QURT_EOK;
	#endif
	if(ptr)
	{
	    // If qapi is called from user space
	    #ifdef FEATURE_MMU_ENABLED
	    if(!qurt_check_if_kernel_address(ptr))
	    {
	      /* Copying the log header */
	      qurt_ret_val = qurt_copy_from_user((void*)&log_hdr,sizeof(log_header_type),ptr,sizeof(log_header_type));
		  if(qurt_ret_val!=QURT_EOK)
			return QAPI_ERROR;
		  log_set_code(&log_hdr, code);

		  /* Now copy the log header back to user space */
		  qurt_ret_val = qurt_copy_to_user((void*)ptr,sizeof(log_header_type),(void*)&log_hdr,sizeof(log_header_type));
		  if(qurt_ret_val!=QURT_EOK)
			return QAPI_ERROR;
		}
		else //qapi called from kernel
		#endif /* FEATURE_MMU_ENABLED */
		{
		  log_set_code(ptr, code);
		}
		
		ret_val =  QAPI_OK;
	}
	return ret_val;
}

/* Shim API for log_set_timestamp */
qapi_Status_t qapi_log_set_timestamp(void* ptr)
{
	qapi_Status_t ret_val = QAPI_ERROR;

	#ifdef FEATURE_MMU_ENABLED
	  log_header_type log_hdr; // log header will be copied to this pointer
	  int qurt_ret_val = QURT_EOK;
	#endif
	if(ptr)
	{
	    // If qapi is called from user space
	    #ifdef FEATURE_MMU_ENABLED
	    if(!qurt_check_if_kernel_address(ptr))
	    {
	      /* Copying the log header */
	      qurt_ret_val = qurt_copy_from_user((void*)&log_hdr,sizeof(log_header_type),ptr,sizeof(log_header_type));
		  if(qurt_ret_val!=QURT_EOK)
			return QAPI_ERROR;
		  log_set_timestamp(&log_hdr);

		  /* Now copy the log header back to user space */
		  qurt_ret_val = qurt_copy_to_user((void*)ptr,sizeof(log_header_type),(void*)&log_hdr,sizeof(log_header_type));
		  if(qurt_ret_val!=QURT_EOK)
			return QAPI_ERROR;
	     }
		 else
		 #endif /* FEATURE_MMU_ENABLED */
		 {
		    log_set_timestamp(ptr);
		 }
		  
		  ret_val =  QAPI_OK;
	 }	  
	return ret_val;
}

/* Shim API for log_status */
qapi_Status_t qapi_log_status(log_code_type code)
{
	boolean ret_val = FALSE;
	ret_val = log_status(code);
	return (get_qapi_status(ret_val));
}

/* Shim API for event_report */
qapi_Status_t qapi_event_report(event_id_enum_type event_id)
{
	qapi_Status_t ret_val = QAPI_ERROR;
	if(event_id)
	{
		event_report(event_id);
		ret_val =  QAPI_OK;
	}
	return ret_val;	
}

/* Shim API for event_report_payload */
qapi_Status_t qapi_event_report_payload(event_id_enum_type event_id, uint8 length, void* data)
{
	qapi_Status_t ret_val = QAPI_ERROR;
	void* data_copy = NULL; // The payload will be copied to this buffer
	int qurt_ret_val = QURT_EOK;
	if(data)
	{
      data_copy = malloc(sizeof(uint8)*length); // Allocating space for the payload
      if(data_copy==NULL)
        return QAPI_ERROR;
      qurt_ret_val = qurt_copy_from_user(data_copy,length,data,length); // Copying the payload to the kernel buffer
      if(qurt_ret_val!=QURT_EOK)
      {
        free(data_copy);
        return QAPI_ERROR;
      }
		
       event_report_payload(event_id, length, data_copy);
       free(data_copy);
       ret_val =  QAPI_OK;
	}
	return ret_val;	
}

#endif
