/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostics User space packet processing

General Description
  Routines to handle User space packet processing

Copyright (c) 2017-2020 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            Edit History

  $Header: //components/rel/core.tx/6.0/services/diag/LSM/qapi/src/qapi_diag_pkt.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/24/18   gn      Removed qurt_txm_qapi_fwk.h file reference (unused)
12/12/17   gn      Fixed issue in QAPI calls from Kernel space
11/27/17   gn      Fixed KW error (Possible attempt to access element 128 of array diagpkt_user_space_table)
10/31/17   gn      Changes to support MMU enablement and command deregistration
04/10/17   gn      DIAG QAPI header file name change to qapi_diag.h
03/30/17   gn      Changes to support DIAG QAPIs with DAM framework
===========================================================================*/

#ifdef QAPI_TXM_SOURCE_CODE

#include "tx_api.h"
#include "qapi_diag.h"
#include "qapi_diag_codes.h"
#include "comdef.h"
#include "stdarg.h"
#include "diagpkt.h"
#include "diagdiag_v.h"
#include "diagi_v.h"
#include <stdlib.h>
#include "queue.h"
#include "diagi.h"
#include "qurt_txm_uid.h"
#include "qurt_memory.h"

/* 
	Static array that stores addresses of structures of type diagpkt_user_space_table_type having input and 
	output buffer pointers and lengths of user space clients registered for Diagpkt service. To support MMU 
	enablement this structure also need to save user space function pointer that has to be added runtime. So
	this structure is not constant anymore.
*/
static diagpkt_user_space_table_type * diagpkt_user_space_table[DIAGPKT_USER_SPACE_TBL_SIZE];
extern const diagpkt_master_table_type * diagpkt_master_table[DIAGPKT_MSTR_TBL_SIZE];

extern uint8 diagpkt_master_tbl_index;

/* Number of entries in diagpkt_user_space_table */
uint8 diagpkt_user_space_tbl_used = 0;


static int diagpkt_user_space_tbl_init_begin = FALSE;
int diagpkt_user_space_tbl_initialized = FALSE;
extern osal_mutex_arg_t diagpkt_tbl_reg_lock;
	
PACK(void *)
diagpkt_user_space_alloc (diagpkt_cmd_code_type code, unsigned int length);

PACK(void *)
diagpkt_user_space_err_rsp(diagpkt_cmd_code_type code, PACK(void *) req_pkt, uint16 req_len);


/* ==========================================================================
FUNCTION DIAGPKT_USER_SPACE_TBL_INIT

DESCRIPTION Initializes the user space table with NULL entries
============================================================================= */
boolean diagpkt_user_space_tbl_init (void)
{
  int i = 0;
  int return_val = TRUE;
  
  if(diagpkt_user_space_tbl_initialized==FALSE)
  {
	if(!osal_atomic_compare_and_set((osal_atomic_word_t*)&diagpkt_user_space_tbl_init_begin, FALSE, TRUE))
	  return_val = FALSE;

	for (i = 0; i < DIAGPKT_USER_SPACE_TBL_SIZE; i++)
	{
	  diagpkt_user_space_table[i] = NULL;
	}

	diagpkt_user_space_tbl_initialized = TRUE;	  
  }
    return return_val;
} /* diagpkt_user_space_tbl_init */

/* ==========================================================================
FUNCTION DIAGPKT_USER_SPACE_TBL_REG_APPEND_PROC

DESCRIPTION
  Registers the user table given to the diagpkt master table, creates new entry in 
  diagpkt_user_space_table with user_space_tbl_ptr. 
  Updates the port field of master table entry with the index of its entry in 
  diagpkt_user_space_table.
============================================================================= */
void
diagpkt_user_space_tbl_reg_append_proc (diagpkt_master_table_type * tbl_ptr, diagpkt_user_space_table_type * user_space_tbl_ptr)
{
   uint8 i = 0;
   
   #ifdef FEATURE_MMU_ENABLED
     int qurt_ret_val = 0;
   #endif
   
   osal_lock_mutex(&diagpkt_tbl_reg_lock);

   if(diagpkt_user_space_tbl_used == DIAGPKT_MSTR_TBL_SIZE)
   {
      /* diagpkt_user_space_table ran out of space */
      osal_unlock_mutex(&diagpkt_tbl_reg_lock);
	  return;
   }

   /* Find a free entry in diagpkt_user_space_table */
   for(i = 0; i < DIAGPKT_MSTR_TBL_SIZE; i++)
     if(!diagpkt_user_space_table[i])
	 	break;   

   /* Invalid index */
   if(i >= DIAGPKT_MSTR_TBL_SIZE)
     return;

   /* Update the port field of master table entry with the index of user space table */
   tbl_ptr->port = i;
   
   /* If input buffer of user space table is a kernel address, save the func_ptr in user space table structure 
   (this pointer will be accessed in diagpkt_process_user_space_command). This can happen if the qapi is called.*/
   #ifdef FEATURE_MMU_ENABLED
     if(qurt_check_if_kernel_address((qurt_addr_t *)user_space_tbl_ptr->inbuf))
     {
       qurt_ret_val = qurt_copy_from_user((void*)&(user_space_tbl_ptr->user_func_ptr), sizeof(user_space_tbl_ptr->user_func_ptr), (void*)&(tbl_ptr->user_table->user_func_ptr), sizeof(tbl_ptr->user_table->user_func_ptr));
       if(qurt_ret_val!=QURT_EOK)
	     return;
     }
   #endif
   
   /* Update the user_space_tbl_ptr in diagpkt_user_space_table */
   /* Not using qurt_copy_to/from_user as it is a kernel to kernel copy */
   diagpkt_user_space_table[i] = user_space_tbl_ptr;
   diagpkt_user_space_tbl_used++;
   
   osal_unlock_mutex(&diagpkt_tbl_reg_lock);
   
   /* Register master table entry */
   diagpkt_tbl_reg(tbl_ptr);
} /* diagpkt_user_space_tbl_reg_append_proc */

/*===========================================================================

FUNCTION DIAGPKT_PROCESS_USER_SPACE_COMMAND

DESCRIPTION
  This procedure copies the command to the user space client's input buffer 
  and calls its handler with the length of the command. Client copies the 
  response to its output buffer and calls qapi_diagpkt_commit passing  the 
  length of the response written. Memory of length returned by client is
  allocated in diag_rsp_heap and the response is copied from the client's 
  output buffer to the diag_rsp_heap. qurt_copy_to_user_ext API is used to
  copy the command as the copy is not done in the module's context.

===========================================================================*/

void diagpkt_process_user_space_command (diagpkt_user_table_entry_type *tbl_entry, void *req_pkt, uint16 pkt_len, uint8 index)
{
	uint32 inbuf_len = 0;
	uint32 *inbuf = NULL, *outbuf = NULL;
	void *rsp_pkt = NULL;
	uint32_t (*user_cb) (uint32_t);
	int qurt_ret_val = 0;
	TXM_MODULE_INSTANCE *module_instance;
	
	/* Retrive user space client input and buffer pointer and sizes from diagpkt_user_space_table */
	inbuf        = diagpkt_user_space_table[index]->inbuf;
	inbuf_len    = diagpkt_user_space_table[index]->inbuf_len;
	outbuf       = diagpkt_user_space_table[index]->outbuf;
	user_cb      = (uint32_t(*)(uint32_t))diagpkt_user_space_table[index]->user_func_ptr;
	
	/* Sanity checks for buffer pointers and function pointer */
	if(inbuf && outbuf && user_cb)
	{
		/* Check if command received can fit in the buffer provided by the client */
		if(pkt_len < inbuf_len)
		{
			/* Getting the pointer to the pointer of module instance */
     		qurt_ret_val = qurt_txm_module_instance_get(&module_instance);  
			if(qurt_ret_val != QURT_EOK)
			  return ;

			/* Copy the command to input buffer of client. Here ext API needs to be used since this copy will not be happening 
			   in the context of the module. This will work both when the QAPI is called from user space and kernel space.*/
			qurt_ret_val = qurt_copy_to_user_ext(module_instance,inbuf,pkt_len, req_pkt, pkt_len);
			if(qurt_ret_val != QURT_EOK)
			  return ;
			
			if(qurt_ret_val==QURT_EOK)
			{
			  (user_cb)(pkt_len);
			}
		}
	}	
	return;
} /* diagpkt_process_user_space_command */

/*===========================================================================

FUNCTION diagpkt_user_space_commit

DESCRIPTION
  This procedure processes the user space client's response and commits the response
  if all the sanity checks are passed. In case of any failure generates an error response
  and commits it.
===========================================================================*/

void* diagpkt_user_space_commit (void * pkt, uint32 rsp_len, uint32 rsp_flag)
{
  void* rsp_pkt = NULL;
  uint8 cmd_code = 0;
  int qurt_ret_val = QURT_EOK;
  
  /* Sanity check for NULL pointer and zero response length */
  if(pkt==NULL || rsp_len==0)
  {
	return NULL;
  }

  /* Get the command code of the response packet */
  cmd_code = diagpkt_get_cmd_code(pkt);
		
  /* If rsp_flag is set to IMMEDIATE_RSP_FLAG, it is an immediate reponse and should be commited with diagpkt_commit */
  if(rsp_flag == IMMEDIATE_RSP_FLAG)
  {
	/* If cmd_code is DIAG_SUBSYS_CMD_F and response length is greater than or equal to sizeof(diagpkt_subsys_hdr_type) allocate memory using diagpkt_alloc
	   with DIAG_SUBSYS_CMD_VER_F(75) */
	if(cmd_code==DIAG_SUBSYS_CMD_F)
	{
	  if(rsp_len >= sizeof(diagpkt_subsys_hdr_type))
	    rsp_pkt = (void *) diagpkt_user_space_alloc(DIAG_SUBSYS_CMD_F, rsp_len); 
	}
	
	/* If cmd_code is DIAG_SUBSYS_CMD_VER_2_F and response length is greater than or equal to sizeof(diagpkt_subsys_hdr_type_v2) allocate memory using 
	   diagpkt_alloc with DIAG_SUBSYS_CMD_VER_2_F(128) */
  	else if(cmd_code==DIAG_SUBSYS_CMD_VER_2_F)
	{
	  if(rsp_len >= sizeof(diagpkt_subsys_hdr_type_v2))
		rsp_pkt = (void *) diagpkt_user_space_alloc(DIAG_SUBSYS_CMD_VER_2_F, rsp_len);	
	}
	
	/* If response is a subsystem command response type allocate memory using diagpkt_alloc with cmd_code of response */
	else 
		rsp_pkt = (void *) diagpkt_user_space_alloc(cmd_code, rsp_len);	
	
	/* If rsp_pkt is allocated copy the response from client's output buffer to the allocated memory and commit the response using diagpkt_commit */
	if(rsp_pkt)
	{
	  qurt_ret_val = qurt_copy_from_user((void *)rsp_pkt, rsp_len, (void *)pkt, rsp_len);
	  if(qurt_ret_val!=QURT_EOK)
	  	return NULL;
	  
      diagpkt_commit(rsp_pkt);
	}
  }
  
  /* If rsp_flag is set to DELAYED_RSP_FLAG, it is a delayed reponse and should be commited with diagpkt_delay_commit */
  else if(rsp_flag == DELAYED_RSP_FLAG)
  {
	/* If cmd_code is DIAG_SUBSYS_CMD_VER_2_F and response length is greater than or equal to sizeof(diagpkt_subsys_hdr_type_v2) allocate memory using 
	   diagpkt_delay_alloc with DIAG_SUBSYS_CMD_VER_2_F(128) */
	if(cmd_code==DIAG_SUBSYS_CMD_VER_2_F)
	{
	  if(rsp_len >= sizeof(diagpkt_subsys_hdr_type_v2))
		rsp_pkt = (void *) diagpkt_delay_alloc(DIAG_SUBSYS_CMD_VER_2_F, rsp_len);	
	}
	
	/* If rsp_pkt is allocated copy the response from client's output buffer to the allocated memory and commit the response using diagpkt_delay_commit */
	if(rsp_pkt)
	{
	  qurt_ret_val = qurt_copy_from_user((void *)rsp_pkt, rsp_len, (void *)pkt, rsp_len);
	  if(qurt_ret_val!=QURT_EOK)
	  	return NULL;
	  
	  diagpkt_delay_commit(rsp_pkt);
	}	
  }
  /* If rsp_flag neither IMMEDIATE_RSP_FLAG nor DELAYED_RSP_FLAG, it is an error flag and an error response has to be generated and commited with the given 
     error code in rsp_flag */
  else
  {
    rsp_pkt = (void *) diagpkt_user_space_err_rsp(rsp_flag, pkt, rsp_len);

	if(rsp_pkt)
	{
	  diagpkt_commit(rsp_pkt);
	}

	rsp_pkt = NULL;
  }
  
  /* Returns an allocated and committed response packet in case of valid response else returns NULL */
  return rsp_pkt;
}               /* diagpkt_user_space_commit */


/*===========================================================================

FUNCTION diagpkt_user_space_alloc

DESCRIPTION
   Allocates the specified number of bytes in the diag heap memory 
   for the response packet. If space is unavailable, NULL will be returned.
===========================================================================*/
PACK(void *)
diagpkt_user_space_alloc (diagpkt_cmd_code_type code, unsigned int length)
{
  diagpkt_q_type *item = NULL;
  diagpkt_hdr_type *pkt = NULL;
  PACK(uint16 *) pattern = NULL;    /* Overrun pattern. */
  unsigned char *p;
  unsigned int size = FPOS (diagpkt_q_type, rsp.pkt) + length + sizeof (uint16);

  item = (diagpkt_q_type *)diag_rsp_malloc(size);

  if (item)
  {
    /* This pattern is written to verify pointers elsewhere in this
       service  are valid. */
    item->rsp.pattern = DIAGPKT_HDR_PATTERN;    /* Sanity check pattern */

    /* length ==  size unless packet is resized later */
    item->rsp.size = length;
    item->rsp.length = length;

    pattern = (PACK(uint16 *)) ((&item->rsp.pkt)+length);

    p = (unsigned char *) pattern;
    p[0] = (DIAGPKT_OVERRUN_PATTERN >> 8) & 0xff;
    p[1] = (DIAGPKT_OVERRUN_PATTERN >> 0) & 0xff;
	
    pkt = (diagpkt_hdr_type *) & item->rsp.pkt;
  }

  if (pkt)
  {
    pkt->command_code = code;
  }
  return (PACK(void *)) pkt;

}               /* diagpkt_user_space_alloc */

/*===========================================================================

FUNCTION diagpkt_user_space_err_rsp

DESCRIPTION
   This function builds an error packet.
===========================================================================*/
PACK(void *)
diagpkt_user_space_err_rsp(diagpkt_cmd_code_type code, PACK(void *) req_pkt, uint16 req_len)
{
  DIAG_BAD_CMD_F_rsp_type *rsp;
  const unsigned int rsp_len = MIN (sizeof (DIAG_BAD_CMD_F_rsp_type),
               req_len + FPOS (DIAG_BAD_CMD_F_rsp_type, pkt));
  rsp = (DIAG_BAD_CMD_F_rsp_type *) diagpkt_user_space_alloc (code, rsp_len);

  if(req_pkt)
  {
	qurt_copy_from_user((void *)rsp->pkt, DIAGPKT_MAX_ERR, (void *)req_pkt,(rsp_len - FPOS (DIAG_BAD_CMD_F_rsp_type, pkt)));		              
  }

  return ((void *) rsp);
}               /* diagpkt_user_space_err_rsp */

/*===========================================================================

FUNCTION diagpkt_user_space_tbl_dereg

DESCRIPTION
   Deregisters user space client's dispatch table with the packet service. 
===========================================================================*/
void diagpkt_user_space_tbl_dereg(diagpkt_user_table_entry_type * tbl_ptr)
{
   uint16 size = 0;
   uint8 user_space_tbl_index = 0, i = 0; 
   
   osal_lock_mutex(&diagpkt_tbl_reg_lock);
   
   for(i = 0; i <= diagpkt_master_tbl_index; i++)
   {
     /* Reached end of master table */
     if(!diagpkt_master_table[i])
	 	break;
	 
	 /* Check for matching user table ptr */
	 if(diagpkt_master_table[i]->user_table == tbl_ptr)
	 {
	   /* Get the index to diagpkt_user_space_table */
	   user_space_tbl_index = diagpkt_master_table[i]->port;  

	   /* Mark the diagpkt_user_space_table entry as NULL */
	   diagpkt_user_space_table[user_space_tbl_index] = NULL;	   

	   /* Decrement indexes */
	   diagpkt_master_tbl_index--;
	   diagpkt_user_space_tbl_used--;
	   
	   /* Pull up master table entries */
	   size = (diagpkt_master_tbl_index - i + 1) * sizeof(diagpkt_master_table[0]);
	   memscpy(&diagpkt_master_table[i], size, &diagpkt_master_table[i+1], size);
	   diagpkt_master_table[diagpkt_master_tbl_index + 1] = NULL;
	 }	 
   }
   osal_unlock_mutex(&diagpkt_tbl_reg_lock);
} /* diagpkt_user_space_tbl_dereg */

#endif
