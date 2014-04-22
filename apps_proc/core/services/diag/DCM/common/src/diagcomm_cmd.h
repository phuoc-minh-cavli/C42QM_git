#ifndef DIAGCOMM_CMD_H 
#define DIAGCOMM_CMD_H
/*==========================================================================

              Diagnostic Communications for Diag Command/Response Channel

General Description
  
Copyright (c) 2014,2016,2019 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diagcomm_cmd.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/10/19   kdey    Porting of Qsockets
07/20/16   nk      Changes to support ThreadX
03/28/14   is      Resolve 4k response issue
02/21/14   is      Command/response separation

===========================================================================*/

#include "diagcomm_io.h"     /* For diagcomm_io_conn_type, diagcomm_enum_port_type */


/* ------------------------------------------------------------------------
** Function prototypes
** ------------------------------------------------------------------------ */

boolean diagcomm_cmd_open( diagcomm_port_type port_type, diagcomm_enum_port_type channel );
void diagcomm_cmd_close( diagcomm_port_type port_type, diagcomm_enum_port_type channel );
void diagcomm_cmd_init( diagcomm_port_type port_type, diagcomm_enum_port_type channel );
uint32 diagcomm_cmd_get_rx_wmq_cnt( diagcomm_port_type port_type, diagcomm_enum_port_type port_num );
dsm_item_type ** diagcomm_cmd_hdlc_encode_to_dsm(uint8* pkt_ptr, uint32 pkt_len);
dsm_item_type * diagcomm_cmd_outbound_alloc_hdlc( unsigned int *count, boolean is_chain );
void diagcomm_send_cmd( void *req_pkt, uint16 pkt_len, diagcomm_enum_port_type port_num, boolean send_dci );

void diagcomm_process_cmd( void );

void diagcomm_process_inbound_cmd( diagcomm_port_type port_type, diagcomm_enum_port_type channel_type );



#endif //DIAGCOMM_CMD_H

