
/**
  @file  vs_servreg_stub.c
  @brief This file contains internal definitions of VS
*/

/*
  ============================================================================

   Copyright (C) 2013-2018 QUALCOMM Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.mpss/8.0.c12/vsd/vs/stubs/vs_servreg_stub.c#1 $
   $Author: pwbldsvc $

*/

/****************************************************************************
  Include files for Module
****************************************************************************/

#include "vs_servreg_stub.h"

SERVREG_SERVICE_STATE servreg_get_servloc_availability(void)
{
  return SERVREG_FAILURE;
}

SERVREG_SERVICE_STATE servreg_get_service_curr_state(SERVREG_MON_HANDLE sr_mon_handle)
{
  return SERVREG_FAILURE;
}

int32_t servreg_get_transaction_id(SERVREG_MON_HANDLE sr_mon_handle)
{
  return NULL;
}

SERVREG_RESULT servreg_set_ack(SERVREG_MON_HANDLE sr_mon_handle, int32_t transaction_id)
{
  return SERVREG_FAILURE;
}

SERVREG_RESULT servreg_register_servloc_availability_signal_qurt(qurt_anysignal_t* signal, uint32_t mask)
{
  return SERVREG_FAILURE;
}

SERVREG_DL_HANDLE servreg_alloc_domainlist_handle(void)
{
  return NULL;	
}

SERVREG_RESULT servreg_get_domainlist(SERVREG_NAME service_name, SERVREG_DL_HANDLE dlhandle)
{
  return SERVREG_FAILURE;
}

SERVREG_NAME servreg_get_local_soc_scope(void)
{
  return NULL;
}

uint32_t servreg_get_num_entries(SERVREG_DL_HANDLE dlhandle)
{
  return 0;
}

SERVREG_RESULT servreg_get_entry(SERVREG_DL_HANDLE dlhandle, uint32_t offset, char** domain)
{
  return SERVREG_FAILURE;
}

SERVREG_RESULT servreg_free_domainlist_handle(SERVREG_DL_HANDLE dlhandle)
{
  return SERVREG_FAILURE;
}

SERVREG_MON_HANDLE servreg_alloc_monitor_handle(SERVREG_NAME domain, SERVREG_NAME service)
{
  return NULL;
}

SERVREG_RESULT servreg_register_listener_qurt(SERVREG_MON_HANDLE sr_mon_handle, qurt_anysignal_t* signal, uint32_t mask)
{
  return SERVREG_FAILURE;
}

SERVREG_RESULT servreg_deregister_listener_qurt(SERVREG_MON_HANDLE sr_mon_handle, qurt_anysignal_t* signal, uint32_t mask)
{
  return SERVREG_FAILURE;
}

SERVREG_RESULT servreg_free_monitor_handle(SERVREG_MON_HANDLE sr_mon_handle)
{
  return SERVREG_FAILURE;
}
