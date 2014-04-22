#ifndef DIAGCOMM_IO_SIO_H 
#define DIAGCOMM_IO_SIO_H
/*==========================================================================

              Diag I/O Wrapper APIs for Interprocessor Communication and I/O 

General Description
  
Copyright (c) 2011-2014,2016, 2019 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diagcomm_io_sio.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------  
01/10/19   kdey    Porting of Qsockets
07/20/16   nk      Changes to support ThreadX 
01/27/14   is      Command/response separation
01/07/14   xy      Added fusion DCI feature 
10/11/13   sa      Added specific dtr cb function for Master Diag
10/01/13   xy      Removed code under FEATURE_WINCE 
05/15/12   is      Allow "build time" mask queries from Master Diag 
03/04/12   sg      Update functions to support control channel
03/04/11   is      Support for Diag over SMD-Lite

===========================================================================*/

#include "customer.h"

#ifdef DIAG_SIO_SUPPORT
#include "comdef.h"
#include "diagcomm_io.h"   /* For diagcomm_io_conn_type */
#include "sio.h"           /* For sio_bitrate_type */


/* For smd channel */
void diagcomm_smd_sio_set_port( diagcomm_io_conn_type * conn );
void diag_smd_dtr_changed_cb( void * cb_data );

/* For sio channel */
void diag_dtr_enable( diagcomm_io_conn_type * conn );
void diag_dtr_changed_cb(void);
void diagcomm_sio_open_cb( sio_port_id_type port_id );
#ifdef DIAG_CONSUMER_API
  void diagcomm_sio_fusion_dci_open_cb( sio_port_id_type port_id );
#endif /* #ifdef DIAG_CONSUMER_API */
void diagcomm_sio_set_port( diagcomm_io_conn_type * conn );

void diagcomm_process_inbound_data( diagcomm_port_type port_type,
                                   diagcomm_enum_port_type channel_type );
void diagcomm_process_inbound_data_dci( diagcomm_enum_port_type port_num);



/* For ctrl channel */
void diagcomm_ctrl_sio_set_port( diagcomm_io_conn_type * conn );

dsm_item_type ** diagcomm_sio_data_to_dsm(uint8* pkt_ptr, uint32 pkt_len);

dsm_item_type ** diagcomm_sio_data_to_dsm_dci(uint8* pkt_ptr, uint32 pkt_len);


#endif /* DIAG_SIO_SUPPORT */

#endif /* DIAGCOMM_IO_SIO_H */
