#ifndef QMI_PDC_SVC_H
#define QMI_PDC_SVC_H
/*==============================================================================

   Q M I   P E R S I S T E N T   D E V I C E   C O N F I G   S E R V I C E
 
GENERAL DESCRIPTION
  Persistent Device Configuration (PDC) QMI service header file

Copyright (c) 2012-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/mcfg_qmi/inc/qmi_pdc_svc.h#1 $ 
$DateTime: 2020/02/19 23:07:19 $ 
$Author: pwbldsvc $ 
$Change: 22644646 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
04/30/10 mj   Initial creation

==============================================================================*/

#include "qmi_csi.h"
#include "mcfg_common.h"
#include "mcfg_client.h"

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/


/*==============================================================================
  Typedefs
==============================================================================*/


/*==============================================================================
  Variables
==============================================================================*/


/*============================================================================== 
 
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
 
==============================================================================*/

#ifndef REMOVE_QMI_PDC_CONFIG_CHANGE_IND_V01
/*===========================================================================

  FUNCTION pdc_generate_config_change_ind

  DESCRIPTION
    This function generates and sends QMI_PDC indications to all clients that
    have registered for the config file change event.

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    type      [in] type of config file
    id        [in] ID of new config file

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void pdc_generate_config_change_ind
( 
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id
);
#endif

#ifndef REMOVE_QMI_PDC_REFRESH_IND_V01
/*===========================================================================
  FUNCTION pdc_generate_refresh_event_ind

  DESCRIPTION
    This function generates and sends QMI_PDC indications to all clients that
    have registered for the refresh event registartion type(START/COMPLETE).

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    event_reg_type [in] mcfg_client_event_e_type
    slot_index     [in] slot ID for refresh event
    sub_index      [in] sub_ID for refresh event

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void pdc_generate_refresh_event_ind
(
  mcfg_client_event_e_type event_reg_type,
  mcfg_refresh_index_type   slot_index,
  mcfg_refresh_index_type   sub_index
);
#endif

#ifndef REMOVE_QMI_PDC_ACTIVATE_CONFIG_IND_V01
/*===========================================================================

  FUNCTION pdc_send_cached_activate_config_ind

  DESCRIPTION
    This function generates and sends QMI_PDC activation indications after
    Refresh

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    slot    [in] slot index for refresh

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void pdc_send_cached_activate_config_ind
( 
  uint8             slot_index,
  mcfg_error_e_type mcfg_err
);
#endif

/*===========================================================================

  FUNCTION pdc_svc_init

  DESCRIPTION
    This function registers the PDC service with the CSI framework.  The service
    is also registered with QSAP so that it is available via QMUX framework.

  DEPENDENCIES
    QCSI/QSAP must be initialized

  PARAMETERS
    os_params [in] task parameters for the service

  RETURN VALUE
    Service handle from the CSI framework

  SIDE EFFECTS
    Registers with the CSI framework and QSAP

===========================================================================*/
qmi_csi_service_handle pdc_svc_init
(
  qmi_csi_os_params *os_params
);

#endif /* QMI_PDC_SVC_H */

