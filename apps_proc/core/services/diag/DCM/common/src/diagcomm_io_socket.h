#ifndef DIAGCOMM_IO_SOCKET_H 
#define DIAGCOMM_IO_SOCKET_H
/*==========================================================================

              Diag I/O Wrapper APIs for Interprocessor Communication and I/O 

General Description
  
Copyright (c) 2015 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diagcomm_io_socket.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/08/15   ps      Diag over sockets


===========================================================================*/
#if defined(DIAG_SOCKET_SUPPORT)

#include "diagcomm_io.h"         /* For diagcomm_io_conn_type */

#define READ_TASK_PRIORITY 5

int diagcomm_io_socket_init(diagcomm_io_conn_type *ctrl_conn);

int diagcomm_io_socket_open(diagcomm_io_conn_type * conn);

void diagcomm_io_socket_close(diagcomm_io_conn_type * conn);

int diagcomm_io_socket_send(diagcomm_io_conn_type *conn, void *buf, uint32 length );

void * diagcomm_io_socket_create_thread(const char *name, void * main_func, void *arg, int stacksize, int prio);

boolean diagcomm_io_socket_check_support(diagcomm_io_conn_type *conn);


#endif /* DIAG_SOCKET_SUPPORT */

#endif //DIAGCOMM_IO_SOCKET_H


