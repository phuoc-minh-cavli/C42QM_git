/**
  @file policyman_nas.h

  @brief Interface to Policy Manager for NAS APIs
*/

/*
  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/mmcp.mpss/6.1.10/api/policyman_nas.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------

===========================================================================*/

#ifndef _POLICYMAN_NAS_H_
#define _POLICYMAN_NAS_H_

#include "policyman.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include <sys/types.h>
#include "sys.h"


#ifdef __cplusplus
}
#endif


/*=============================================================================
  API to return current MCC.
=============================================================================*/

/*-------- policyman_get_current_mcc --------*/
/**
@brief  
  This API returns the current MCC based on which Policy Manager is taking a decision 
  to stay in HOME or ROAM configuration. API return value is controlled via XML and 
  client should read MCC only when POLICYMAN_STATUS_SUCCESS is returned.
  In actuality this may not be the MCC on which capability is decided for reqeusted 
  subscription but it is still the best guess.

@param[in] subsId: Subscription Id for which MCC is required
@param[in] pMcc  : Pointer to a container to store MCC value

@return
  POLICYMAN_STATUS_SUCCESS if valid MCC is returned.
  Relevant error is returned otherwise.  
*/

policyman_status_t  policyman_get_current_mcc(
  sys_modem_as_id_e_type subsId,
  sys_mcc_type           *pMcc
);


#endif

