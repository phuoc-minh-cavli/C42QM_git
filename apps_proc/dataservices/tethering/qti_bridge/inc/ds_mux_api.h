#ifndef DS_MUX_API_H
#define DS_MUX_API_H
/*===========================================================================

                                 D S   M U X   T A S K

                                 H E A D E R   F I L E

DESCRIPTION
  This is the external header file for the MUX Task. This file
  contains all the functions, definitions and data types needed for other
  tasks to interface to the MUX Task.

  Signal and command definitions for the MUX Task, and functions
  that should be used by other tasks to send commands to the MUX
  Task are defined here.

  Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/inc/ds_mux_api.h#1 $
  $DateTime: 2019/11/26 00:18:17 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/16   ad      initial version
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/
#include "ds_mux.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
ds_mux_result_enum_type  ds_mux_passive_init
(
  dlci_cmux_param_type    *dlci_param
);

ds_mux_result_enum_type  ds_mux_active_init
(
  sio_stream_id_type                  stream_id,
  ds_mux_ext_client_event_info_type  *event_info
);
ds_mux_result_enum_type  ds_mux_set_oprt_mode
(
  ds_mux_mode_enum_type    mode,
  dlci_cmux_param_type    *dlci_param
);

ds_mux_result_enum_type  ds_mux_set_subset
(
  ds_mux_subset_enum_type  subset,
  dlci_cmux_param_type    *dlci_param
);

ds_mux_result_enum_type  ds_mux_set_port_speed
(
  ds_mux_port_speed_enum_type    port_speed,
  dlci_cmux_param_type          *dlci_param
);

ds_mux_result_enum_type  ds_mux_set_N1
(
  uint16                   frame_size,
  dlci_cmux_param_type    *dlci_param
);

ds_mux_result_enum_type  ds_mux_set_T1
(
  uint16                   acknowledgement_timer,
  dlci_cmux_param_type    *dlci_param
);

ds_mux_result_enum_type  ds_mux_set_N2
(
  uint8                    num_re_transmissions,
  dlci_cmux_param_type    *dlci_param
);

ds_mux_result_enum_type  ds_mux_set_T2
(
  uint16                   response_timer,
  dlci_cmux_param_type    *dlci_param
);

ds_mux_result_enum_type  ds_mux_set_T3
(
  uint16                    wake_up_timer,
  dlci_cmux_param_type     *dlci_param
);

ds_mux_result_enum_type  ds_mux_set_K
(
  uint8                    window_size,
  dlci_cmux_param_type    *dlci_param
);



/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS FOR LOGICAL SIO PORT
===========================================================================*/

uint32 ds_mux_ext_sio_init
( 
  sio_register_device_driver_cb_type register_driver_cb_fn
);


#endif /* DS_MUX_API_H */
