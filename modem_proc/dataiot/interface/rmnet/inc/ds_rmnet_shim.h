#ifndef _DS_RMNET_SHIM_H
#define _DS_RMNET_SHIM_H
/*===========================================================================

                 D S _ R M N E T _ S H I M . H

DESCRIPTION

  RMNET physical devices - Definitions header file.

EXTERNALIZED FUNCTIONS

Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/rmnet/inc/ds_rmnet_shim.h#1 $
  $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/24/13    wc     Initial file
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "ds_headers.h"
#include "ds_rmnet_xport.h"

/*===========================================================================

                                 DEFINITIONS

===========================================================================*/


/*===========================================================================

                           EXTERNAL FUNCTIONS 

===========================================================================*/

/*===========================================================================
  FUNCTION RMNET_SHIM_GET_PHYS_SIO_PORT()

  DESCRIPTION
    This function assigns a physical SIO port

  PARAMETERS
    None

  RETURN VALUE
    Physical SIO port

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
sio_port_id_type rmnet_shim_get_phys_sio_port
(
  void
);

/*===========================================================================
  FUNCTION RMNET_SHIM_FREE_PHYS_SIO_PORT()

  DESCRIPTION
    This function frees a physical SIO port

  PARAMETERS
    sio_port: physical SIO port id 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_shim_free_phys_sio_port
(
  sio_port_id_type  sio_port
);

/*===========================================================================
  FUNCTION RMNET_SHIM_GET_LOGICAL_SIO_PORT()

  DESCRIPTION
    This function assigns a logical SIO port

  PARAMETERS
    None

  RETURN VALUE
    Logical SIO port

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
sio_port_id_type rmnet_shim_get_logical_sio_port
(
  void
);

/*===========================================================================
  FUNCTION RMNET_SHIM_FREE_LOGICAL_SIO_PORT()

  DESCRIPTION
    This function frees a logical SIO port

  PARAMETERS
    sio_port: logical SIO port id 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_shim_free_logical_sio_port
(
  sio_port_id_type  sio_port
);

/*===========================================================================
  FUNCTION RMNET_SHIM_SET_PER_MAPPING()

  DESCRIPTION
    This function configs driver of SIO to peripheral mapping

  PARAMETERS
    phys_xport:   rmnet phys xport structure

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_shim_set_per_mapping
(
  rmnet_phys_xport_type   * phys_xport
);

/*===========================================================================
  FUNCTION RMNET_SHIM_SET_DATA_FORMAT()

  DESCRIPTION
    This function configs driver's data format

  PARAMETERS
    phys_xport:   rmnet phys xport structure
    data_format:  pointer to data format

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_shim_set_data_format
(
  rmnet_phys_xport_type   * phys_xport,
  rmnet_data_format_type  * data_format
);

/*===========================================================================
  FUNCTION RMNET_SHIM_SET_PORT_MAPPING()

  DESCRIPTION
    This function configs driver of logical to physical port mapping

  PARAMETERS
    xport_info:   rmnet xport structure

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_shim_set_port_mapping
(
  rmnet_xport_type        * xport_info
);

/*===========================================================================
  FUNCTION RMNET_SHIM_REGISTER_QMAP_CMD_CB()

  DESCRIPTION
    This function registers QMAP cmd callbacks

  PARAMETERS
    None

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_shim_register_qmap_cmd_cb
(
  void
);

/*===========================================================================
  FUNCTION RMNET_SHIM_SEND_QMAP_CMD()

  DESCRIPTION
    This function sends a QMAP cmd

  PARAMETERS
    None

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_shim_send_qmap_cmd
(
  rmnet_xport_type  * xport_info,
  uint8               cmd,
  uint8               cmd_type,
  dsm_item_type     * pkt,
  uint32            * tx_id,
  boolean             is_retx
);
/*===========================================================================
  FUNCTION RMNET_SHIM_NOTIFY_TETHERED_CALL_STATE()

  DESCRIPTION
    This function notifies the driver if the first tethered call is coming up/
    last tethered call is being torn down

  PARAMETERS
    phys_xport: phys xport info
    is_call_up: is call coming up

  RETURN VALUE
    NONE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_shim_notify_tethered_call_state
(
  rmnet_phys_xport_type  * phys_xport,
  boolean                  is_call_up
);

/*===========================================================================
  FUNCTION RMNET_SHIM_CHECK_LOGICAL_PORT()

  DESCRIPTION
    This function checks if a logical SIO port is opened

  PARAMETERS
    port_id   :Port which needs to be checked if open by A2 

  RETURN VALUE

   Boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

boolean rmnet_shim_check_logical_port
(
  sio_port_id_type	port_id
);

#endif /* _DS_RMNET_SHIM_H */

