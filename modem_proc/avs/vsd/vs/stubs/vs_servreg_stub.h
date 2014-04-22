#ifndef __VS_SERVREG_STUB_H__
#define __VS_SERVREG_STUB_H__

/**
  @file  vs_servreg_stub.h
  @brief This file contains stub defintions of service registry interface
*/

/*
  ============================================================================

   Copyright (C) 2013-2018 QUALCOMM Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
  
   $Header: //components/rel/avs.mpss/8.0.c12/vsd/vs/stubs/vs_servreg_stub.h#1 $
   $Author: sduddell 
*/

/****************************************************************************
  Include files for Module
****************************************************************************/
#include "mmdefs.h"
#include "qurt_anysignal.h"

typedef void* SERVREG_DL_HANDLE;
typedef void* SERVREG_MON_HANDLE;
typedef char * SERVREG_NAME;
#define SERVREG_NULL         ((void*)0)

typedef enum
{
   SERVREG_SERVICE_STATE_DOWN   = 0x0FFFFFFF,           /* Logically exists right at this instance but not expected to respond */
   SERVREG_SERVICE_STATE_UP     = 0x1FFFFFFF,           /* Logically exists right at this instance */
   SERVREG_SERVICE_STATE_UNINIT = 0x7FFFFFFF            /* Logically never has existed */
}SERVREG_SERVICE_STATE;

typedef enum 
{
   SERVREG_SUCCESS        = 0,
   SERVREG_SYNCED         = 1,
   SERVREG_FAILURE        = -1,
   SERVREG_IN_PROGRESS    = -2,
   SERVREG_OVERFLOW       = -3,
   SERVREG_INVALID_HANDLE = -4,
   SERVREG_INVALID_PARAM  = -5,
   SERVREG_NO_MEMORY      = -6
}SERVREG_RESULT;


SERVREG_SERVICE_STATE servreg_get_servloc_availability(void);

SERVREG_SERVICE_STATE servreg_get_service_curr_state(SERVREG_MON_HANDLE sr_mon_handle);

int32_t servreg_get_transaction_id(SERVREG_MON_HANDLE sr_mon_handle);

SERVREG_RESULT servreg_set_ack(SERVREG_MON_HANDLE sr_mon_handle, int32_t transaction_id);

SERVREG_RESULT servreg_register_servloc_availability_signal_qurt(qurt_anysignal_t* signal, uint32_t mask);

SERVREG_DL_HANDLE servreg_alloc_domainlist_handle(void);

SERVREG_RESULT servreg_get_domainlist(SERVREG_NAME service_name, SERVREG_DL_HANDLE dlhandle);

SERVREG_NAME servreg_get_local_soc_scope(void);

uint32_t servreg_get_num_entries(SERVREG_DL_HANDLE dlhandle);

SERVREG_RESULT servreg_get_entry(SERVREG_DL_HANDLE dlhandle, uint32_t offset, char** domain);

SERVREG_RESULT servreg_free_domainlist_handle(SERVREG_DL_HANDLE dlhandle);

SERVREG_RESULT servreg_register_listener_qurt(SERVREG_MON_HANDLE sr_mon_handle, qurt_anysignal_t* signal, uint32_t mask);

SERVREG_RESULT servreg_deregister_listener_qurt(SERVREG_MON_HANDLE sr_mon_handle, qurt_anysignal_t* signal, uint32_t mask);

SERVREG_RESULT servreg_free_monitor_handle(SERVREG_MON_HANDLE sr_mon_handle);

SERVREG_MON_HANDLE servreg_alloc_monitor_handle(SERVREG_NAME domain, SERVREG_NAME service);

#endif  /* __VS_SERVREG_STUB_H__ */
