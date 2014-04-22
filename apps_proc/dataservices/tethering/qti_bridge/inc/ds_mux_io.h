#ifndef DS_MUX_IO_H
#define DS_MUX_IO_H
/*===========================================================================

                                 D S   M U X   I O

                                 H E A D E R   F I L E

DESCRIPTION
  This is the external header file for the MUX IO. This file
  contains all the functions, definitions and data types needed   
  for MUX input - output processing

  Rx signal handler - Handle all incoming traffic from host
  Tx signal hanlder - Handle all outgoing traffic to host

  Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/inc/ds_mux_io.h#1 $
  $DateTime: 2019/11/26 00:18:17 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/16   ad      initial version
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#include "sio.h"
#include "ds_mux_defs.h"
#include "qti_bridge.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define DS_MUX_IO_WM_DEFAULT_LO   175000  //TBD
#define DS_MUX_IO_WM_DEFAULT_HI   350000  //TBD
#define DS_MUX_IO_WM_DEFAULT_DNE  400000  //TBD

typedef enum
{
  DS_MUX_IO_FRAME_INIT     = 0,
  DS_MUX_IO_FRAME_START_PARSED,
  DS_MUX_IO_FRAME_ADDR_PARSED,
  DS_MUX_IO_FRAME_CTRL_PARSED,
  DS_MUX_IO_FRAME_LEN_PARSED,
  DS_MUX_IO_FRAME_INFO_PARSED,
  DS_MUX_IO_FRAME_FCS_PARSED,
  DS_MUX_IO_FRAME_STOP_PARSED  
}ds_mux_io_parse_frame_state_enum_type;

/**
 *  A mux frame will be identified with this structure
 **/
typedef struct
{
  ds_mux_frame_type_enum_type frame_type;
  uint8                       dlci;
  uint8                       control_field;
  boolean                     command_response;
  boolean                     poll_final;
  uint16                      length;
  dsm_item_type*              information_ptr;
  uint8                       fcs;
} ds_mux_io_frame_type;


/*--------------------------------------------------------------------------
   decoded frames  queue.
---------------------------------------------------------------------------*/
typedef struct    
{
  q_link_type             link;      /* Link for queue */
  ds_mux_io_frame_type    frame;  /* pointer to frame*/
} ds_mux_io_frame_struct_type;

typedef struct
{
  q_type                                 decoded_frame_q;
  /* Queue will be holding FRAMES parsed  from last DSM items*/
  ds_mux_io_parse_frame_state_enum_type  curr_frame_state;
  /* currnet frame state */
  ds_mux_io_frame_struct_type           *curr_frame_ptr;  
  /* Current frame*/
}ds_mux_io_rx_info;


typedef struct
{
  
  q_type             dlci0_cmd_q;
  /* Hold all the commands assosiated with DLCI0 */
  q_type             dlciN_cmd_q;
  /* Hold all the commands assosiated with DLCIN */
  q_type             dlci0_msg_q;
  /* Hold all the messages assosiated with DLCI0 */
  q_type             dlciN_msg_q;
  /* Hold all the messages assosiated with DLCIN */

  dsm_watermark_type dlci0_cmd_wm;
  /* Hold all the commands assosiated with DLCI0 */
  dsm_watermark_type dlciN_cmd_wm;
  /* Hold all the commands assosiated with DLCIN */
  dsm_watermark_type dlci0_msg_wm;
  /* Hold all the messages assosiated with DLCI0 */
  dsm_watermark_type dlciN_msg_wm;
  /* Hold all the messages assosiated with DLCIN */
  ds_mux_io_rx_info   rx_info;
  /* physical port receive info */

} ds_mux_io_info_type;

void ds_mux_io_init(void);

void ds_mux_io_deinit(void);

ds_mux_result_enum_type ds_mux_io_initialize_wmk (void);

ds_mux_result_enum_type ds_mux_io_transmit_cmd_response
(
  ds_mux_io_frame_type    *frame
);

boolean ds_mux_io_tx_sig_hdlr
(
  ds_mux_sig_enum_type      sig,
  void                    * user_data_ptr
);

boolean ds_mux_io_rx_sig_hdlr
(
  ds_mux_sig_enum_type      sig,
  void                    * user_data_ptr
);

ds_mux_result_enum_type  ds_mux_io_transmit_data
(
  uint8               dlci_id,
  uint8               fc_flag,
  uint8               rtc_asserted_flag,
  uint8               cd_asserted_flag,
  dsm_item_type     **tx_pkt_ptr_ptr
);


ds_mux_result_enum_type ds_mux_io_transmit_msg_response
(
  ds_mux_io_frame_type    *frame
);


ds_mux_result_enum_type ds_mux_io_transmit_msg_cmd
( 
  ds_mux_io_frame_struct_type           *output_frame 
);

void  ds_mux_io_dlci0_cmd_non_empty_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
);

#endif /* DS_MUX_IO_H */
