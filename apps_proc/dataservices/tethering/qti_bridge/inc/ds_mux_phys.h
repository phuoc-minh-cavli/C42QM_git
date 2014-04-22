#ifndef DS_MUX_PHYS_H
#define DS_MUX_PHYS_H
/*===========================================================================

                                 D S   M U X   P H Y S

                                 H E A D E R   F I L E

DESCRIPTION
  This is the external header file for the MUX Task. This file
  contains all the functions, definitions and data types needed for MUX
  task to interface to the SIO for physical port connection.


  Copyright (c) 2016,2021 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/inc/ds_mux_phys.h#2 $
  $DateTime: 2021/05/05 04:35:02 $
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

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*--------------------------------------------------------------------------
      Physical connection control block: Data types to hold physical SIO connection info  
---------------------------------------------------------------------------*/
typedef struct
{

  dsm_watermark_type       tx_wmk;
  /* Transmit Watermark, to write to the SIO Port */
  
  q_type                   tx_q;
  /* Queue for the tx_wmk */
  
  boolean                  is_tx_flow_controlled;
  /* Tx Flow control, If true then cannot send any more data */

  boolean                  is_rx_flow_controlled;
  /* RX Flow control, if true then cannot receive any more data
     * DS MUX send FCon message */

  dsm_watermark_type       rx_wmk;
  /* Receive Watermark, to receive from the SIO Port */

  q_type                   rx_q;
  /* Queue for the rx_wmk */

  /*
   * SIO Stream ID, handle to talk to SIO lib
   */
  sio_stream_id_type       stream_id;
  /* SIO Stream ID  */
  
  boolean                  dte_ready_asserted;
  /* connection is ready to Rx and Tx data  */

  sio_port_id_type          sio_port;         
    
  /* sio_port id associated with device ID*/

  uint8                     rdm_oprt_retry;
  /* retry RDM operation*/

  qurt_timer_t  timer_handle;

  qurt_timer_attr_t  timer_attr;
  /* retry  timer*/
  
} ds_mux_phys_sio_conn_info_type;

typedef enum
{
  DS_MUX_PHYS_RDM_OPRT_MIN     = 0,
  DS_MUX_PHYS_RDM_ASSIGN_PORT  = 1,
  DS_MUX_PHYS_RDM_CLOSE_PORT   = 2,
}ds_mux_phys_rdm_oprt_enum_type;

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

void ds_mux_phys_init(void);

void ds_mux_phys_deinit(void);


/*===========================================================================
FUNCTION      DS_MUX_PHYS_CONN_INIT

DESCRIPTION   Function to initialize the module. This function needs to be
              called once when passive init triggers it. All one time initializations
              will happen in this module.

PARAMETERS    none.

DEPENDENCIES  none.

RETURN VALUE  SUCCESS/FAILURE.

SIDE EFFECTS  none.
===========================================================================*/

ds_mux_result_enum_type ds_mux_phys_conn_init(void);

/*===========================================================================
FUNCTION      DS_MUX_PHYS_CONN_INIT

DESCRIPTION   Function to open physical SIO port. This function needs to be
              called once when Active init triggers it. All one time initializations
              will happen in this module.

PARAMETERS    none.

DEPENDENCIES  none.

RETURN VALUE  SUCCESS/FAILURE.

SIDE EFFECTS  none.
===========================================================================*/
ds_mux_result_enum_type ds_mux_phys_port_connect
(
  boolean             is_cmux_supported,
  sio_stream_id_type  stream_id
);

void ds_mux_phys_rdm_open_handler
(
  sio_port_id_type port_id
);


dsm_watermark_type* ds_mux_phy_get_rx_wmk (void);

ds_mux_result_enum_type ds_mux_phys_transmit
( 
  dsm_item_type            *item_ptr
);


#endif /* DS_MUX_PHYS_H */
