/******************************************************************************
  @file: loc_for_constrained_tunc.h
  @brief:   This module provides an interface for the constrained tunc client 
            that runs in location middle ware
  

  DESCRIPTION

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
Copyright (c) 2018 Qualcomm Technologies, Inc. 
Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_constrained_tunc_client.h#1 $
$DateTime: 2020/07/21 21:23:44 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
09/11/14   sj      Initial version
======================================================================*/

#ifndef LOC_CONSTRAINED_TUNC_CLIENT_H
#define LOC_CONSTRAINED_TUNC_CLIENT_H

#include "location_service_iot_v02.h"
#include "loc_client.h"
#include "aries_os_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================
FUNCTION loc_mw_query_gnss_energy_consumed

DESCRIPTION
  This function requests the energy consumed so far by the GNSS engine.
 
PARAMETERS 
 
DEPENDENCIES

RETURN VALUE 

SIDE EFFECTS
===========================================================================*/
boolean loc_mw_query_gnss_energy_consumed
(
  locQmiShimCltInfoStructT *pz_loc_client_ptr,
  const qmiLocQueryGNSSEnergyConsumedReqMsgT_v02 *pz_msg,
  qmiLocGenRespMsgT_v02  *pz_qmi_resp
);


/*===========================================================================
FUNCTION loc_mw_handle_query_gnss_energy_consumed

DESCRIPTION
  This function requests the energy consumed so far by the GNSS engine.
 
PARAMETERS 
 
DEPENDENCIES

RETURN VALUE 

SIDE EFFECTS
===========================================================================*/
void loc_mw_handle_query_gnss_energy_consumed(loc_client_handle_type l_LocClientHandle);

#ifdef __cplusplus
}
#endif
#endif /* LOC_CONSTRAINED_TUNC_CLIENT_H */
