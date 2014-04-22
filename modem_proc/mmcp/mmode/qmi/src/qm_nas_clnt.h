#ifndef _QM_NAS_CLNT_H
#define _QM_NAS_CLNT_H

/*===========================================================================

                         QM_NAS_CLNT.H

DESCRIPTION

  QMI_MMODE header file for nas client information module.

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/25/13    sm     Created file
===========================================================================*/

#include "comdef.h"
#include "qm_comdef.h"
#include "qmi_nas_internal.h"
#include "network_access_service_v01.h"


/*===========================================================================
  FUNCTION QM_NAS_CLNT_RESET_INFO()

  DESCRIPTION
    This function clears out client specific information for the specified clid

  PARAMETERS
    clid: client to reset information for

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_reset_info( qmi_nasi_client_state_type *cl_sp );

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
/*===========================================================================
  FUNCTION QM_NAS_CLNT_PROCESS_CONFIG_SIG_INFO()

  DESCRIPTION
    This function sets the config_sig_info information for the specific client

  PARAMETERS
    clid: client to store config_sig_info information for
    config: information to store for client 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_process_config_sig_info( qmi_nasi_client_state_type cl_sp, nas_config_sig_info_req_msg_v01 *config );
#endif
/*===========================================================================
  FUNCTION QM_NAS_CLNT_PROCESS_CONFIG_SIG_INFO2()

  DESCRIPTION
    This function sets the config_sig_info information for the specific client

  PARAMETERS
    clid: client to store config_sig_info information for
    config: information to store for client 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_process_config_sig_info2( qmi_nasi_client_state_type *cl_sp, nas_config_sig_info2_req_msg_v01 *config );

/*===========================================================================
  FUNCTION QM_NAS_CLNT_GET_BINDING()

  DESCRIPTION
    This function retrieves the subscription for the given client.

  PARAMETERS
    cl_sp: client to query bound subscription for.

  RETURN VALUE
    enum qm_subs_e: bound subscription for a client, QM_SUBS_MIN if error.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
enum qm_subs_e qm_nas_clnt_get_binding( qmi_nasi_client_state_type* cl_sp );

/*===========================================================================
  FUNCTION QM_NAS_CLNT_CROSSED_THRESHOLD()

  DESCRIPTION
    This function determines if provided new value has crossed threshold or delta
    for specified client.

  PARAMETERS
    clid: client ID to check
    subscription: subscription this request is for
    rat: RAT to check thresholds for
    type: the measurement type to check threshold for
    new_value: the new value
    old_value: the previous value

  RETURN VALUE
    boolean: whether new value crossed a threshold from old value

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_nas_clnt_crossed_threshold( qmi_nasi_client_state_type* cl_sp, enum qm_subs_e subscription,
                                       enum qm_rat_e rat, enum qm_sig_type_e type,
                                       float new_value, float old_value );

#ifndef FEATURE_MMODE_QMI_DISABLE_IMS_EXT
/*===========================================================================
  FUNCTION QM_NAS_CLNT_SEND_IND_ALL()

  DESCRIPTION
    This function calls the QMI Framework API to send out the NAS service
    indication to all client that are registered for the IND.

  PARAMETERS
    clid: client ID to send ind to
    cmd: Which indication is being sent
    ind: pointer to dsm item to send

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_send_ind_all( uint16 cmd, void *cstruct, enum qm_subs_e subs );
#endif

#endif // !_QM_NAS_CLNT_H
