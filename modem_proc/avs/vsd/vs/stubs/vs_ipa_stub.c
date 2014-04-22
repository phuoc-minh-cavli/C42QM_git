
/**
  @file  vs_ipa_stub.c
  @brief This file contains internal definitions of VS
*/

/*
  ============================================================================

   Copyright (C) 2018 QUALCOMM Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.mpss/8.0.c12/vsd/vs/stubs/vs_ipa_stub.c#1 $
   $Author: pwbldsvc $

*/

/****************************************************************************
  Include files for Module
****************************************************************************/
#include "vs_ipa_stub.h"
#include "vs_ipa_dma_stub.h"
#include "vs_ipa_vote_stub.h"


int32 ipa_dma_state_get(ipa_dma_state_e *state)
{
  return IPA_ERR_FAIL;
}


int32 ipa_dma_open(ipa_dma_ch_param_t *ch_param)
{
  return IPA_ERR_FAIL;
}

int32 ipa_dma_close(uint32 ch_handle)
{
  return IPA_ERR_FAIL;
}

int32 ipa_dma_host_to_device(uint32 ch_handle, ipa_dma_req_t *req)
{
  return IPA_ERR_FAIL;
}

int32 ipa_dma_device_to_host(uint32 ch_handle, ipa_dma_req_t *req)
{
  return IPA_ERR_FAIL;
}

ipa_err_code_e ipa_vote(ipa_vote_client_type_e client_type, uint32 options)
{
  return IPA_ERR_FAIL;
}

ipa_err_code_e ipa_devote(ipa_vote_client_type_e client_type)
{
  return IPA_ERR_FAIL;
}

