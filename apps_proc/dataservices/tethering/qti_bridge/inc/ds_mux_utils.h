#ifndef DS_MUX_UTILS_H
#define DS_MUX_UTILS_H
/*===========================================================================

                                 D S   M U X   U T I L S

                                 H E A D E R   F I L E

DESCRIPTION
  This is the external header file for the MUX UTILS . This file
  contains all the functions, definitions and data types needed 
  for decoding and encoding MUX packets.


  Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/inc/ds_mux_utils.h#1 $
  $DateTime: 2019/11/26 00:18:17 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/16   ad      initial version
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#ifndef MOB_TX
#include "sio.h"

#else
#include "sio_mob.h"
#endif

#include "ds_mux_defs.h"
#include "ds_mux_io.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/* Initial value of FCS according to spec.
 */
#define DS_MUX_UTIL_FCS_INIT_VAL        0xFFu

/* Number of elements in crc table 
 */
#define DS_MUX_UTIL_CRC_TABLE_ELEMENTS          256


ds_mux_result_enum_type ds_mux_util_decode_input
( 
  dsm_item_type                  *item_ptr,
  ds_mux_io_rx_info              *rx_info
);

ds_mux_result_enum_type ds_mux_util_encode_cmd_input
( 
  dsm_item_type                  **item_ptr,
  ds_mux_io_frame_type            *frame

);

ds_mux_result_enum_type ds_mux_util_encode_data_input
( 
  dsm_item_type                  **item_ptr,
  ds_mux_io_frame_type            *frame

);

ds_mux_result_enum_type ds_mux_util_encode_msg_input
( 
  dsm_item_type            **item_ptr,
  ds_mux_io_frame_type     *frame
);


void ds_mux_util_prepare_cmd_frame
(
  uint8                       dlci,
  ds_mux_frame_type_enum_type frame_type,
  boolean                     is_command,
    boolean                   is_poll,
  ds_mux_io_frame_type       *output_frame
);

void ds_mux_util_prepare_data_frame
(
  uint8                       dlci,
  ds_mux_frame_type_enum_type frame_type,
  ds_mux_io_frame_type       *output_frame
);

ds_mux_frame_type_enum_type ds_mux_util_convert_frame_type
( 
  ds_mux_subset_enum_type cmux_frame_type
);

ds_mux_io_frame_struct_type* ds_mux_util_alloc_frame (void);

ds_mux_result_enum_type ds_mux_util_process_power_save_flag
(
  dsm_item_type                  *item_ptr,
  ds_mux_io_rx_info              *rx_info
);

ds_mux_result_enum_type ds_mux_util_process_frame_after_wake_up
(
  dsm_item_type                  *item_ptr,
  ds_mux_io_rx_info              *rx_info
);


#endif /* DS_MUX_UTILS_H */
