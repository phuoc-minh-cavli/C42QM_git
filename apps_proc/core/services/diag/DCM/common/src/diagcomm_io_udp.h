#ifndef DIAGCOMM_IO_UDP_H 
#define DIAGCOMM_IO_UDP_H
/*==========================================================================

              Diag I/O Wrapper APIs for Interprocessor Communication and I/O 

General Description
  
Copyright (c) 2015 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diagcomm_io_udp.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/08/15   ps      Created file for Diag over sockets


===========================================================================*/
#include "diagcomm_io.h"
#include "diagpkt.h"

#if defined(DIAG_SOCKET_SUPPORT)

typedef struct
{
  char buf[3*DIAG_MAX_RX_PKT_SIZ];
  uint32 used;
}diag_udp_read_buf_type;


void diagcomm_io_udp_init(void);
void diagcomm_io_udp_open_init(diagcomm_io_conn_type * conn);
int diagcomm_io_udp_open(diagcomm_io_conn_type * conn);

int diagcomm_io_udp_send(diagcomm_io_conn_type * conn,void *buf, unsigned int total_pkt_length);
void diagcomm_io_udp_close(diagcomm_io_conn_type * conn);
int diagcomm_io_udp_get_srv_addr (ipcr_name_t name, struct qsockaddr_ipcr *srv_addr, int timeout);

#endif

#endif //DIAGCOMM_IO_UDP_H
