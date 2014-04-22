/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    Diag I/O Wrapper APIs for Interprocessor Communication and I/O

General Description

Copyright (c) 2015, 2019, 2022 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diagcomm_io_udp.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/19    kdey   Changes to prevent panic in apps when modem is not up
05/29/15    sa     Increased the control channel send timeout to default
                   timeout of 5seconds
04/08/15    ps     Created file to provide DIAG over sockets

===========================================================================*/
#include "diagcomm_io.h"
#include "diagcomm_io_udp.h"
#include "diagcomm_io_socket.h"
#include "assert.h"
#include "msg_diag_service.h"
#include <stdio.h>
#include <stdlib.h>
#include "diag_v.h"
#include "diagcomm_v.h"
#include "stringl.h"
#include "diag_cfg.h"
#include "time_svc.h"
#include "ULogFront.h"
#include "diagtarget.h"

#if defined(DIAG_SOCKET_SUPPORT)

#define THREAD_TASK_STACK_SIZE 4*1024 //4 //4k stacksize

#define UDP_WRITE_TIMEOUT 5000 //milliseconds


void diagcomm_io_udp_receive_data(void);
void diagcomm_io_udp_thread_main(void *data);

#define DIAG_CHANNEL_READ_BUF_SIZE 8*1024
#define DIAG_UDP_TIMEOUT_TIMER_SIG       0x00100000

osal_tcb_t *read_task_tcb;
osal_timer_t read_task_block_timer;


static uint32 diag_pfds_index = 0;
static struct qpollfd diag_pfds[DIAG_MAX_NUM_SOCKETS];
static diagcomm_io_conn_type *diag_sock_conn[DIAG_MAX_NUM_SOCKETS];

extern uint32 diag_dog_rpt_time_var;

/*===========================================================================
FUNCTION diagcomm_io_udp_get_srv_addr

DESCRIPTION
  This API gets the server address based on the port name


PARAMETERS
  name: remote port name
  srv_addr: (output) server address
  timeout: timeout to qpoll to look for the remote address

DEPENDENCIES
  The connection object should contain the parameters required to open the socket

RETURN VALUE
  rc > 1 : SUCCESS
  rc <=0 : Cannot find remote server

===========================================================================*/

int diagcomm_io_udp_get_srv_addr (ipcr_name_t name, struct qsockaddr_ipcr *srv_addr, int timeout)
{
  int ctrl_fd, rc = 0;
  struct qpollfd pfd;
  int time_to_wait = timeout;
  int poll_timeout = (int)diag_dog_rpt_time_var;
  boolean retry;

  /* Create a socket */
  ctrl_fd = qsocket(AF_IPC_ROUTER, QSOCK_DGRAM, 0);
  if(ctrl_fd < 0)
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Control FD creation failed rc = %d!\n",ctrl_fd);
    return -1;
  }
  /* Mark this as a control socket to listen on events
   * on the name server */
  rc = qsetsockopt(ctrl_fd, QSOL_IPC_ROUTER, QSO_IPCR_SET_CONTROL_PORT, NULL, 0);
  if(rc)
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Setting socket option failed with rc = %d\n", rc);
    qclose(ctrl_fd);
    return -1;
  }

  rc = qsetsockopt(ctrl_fd, QSOL_IPC_ROUTER, QSO_IPCR_SET_SERVICE_PREF, &name.service, sizeof(name.service));
  if(rc)
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Setting socket type pref failed with rc= %d\n", rc);
    qclose(ctrl_fd);
    return -1;
  }

  pfd.fd = ctrl_fd;
  pfd.events = QPOLLIN;
  pfd.revents = 0;

  while(1)
  {
    ipcr_ctrl_msg msg;
    unsigned int num_entries = 1;
    rc = ipcr_find_name(ctrl_fd, &name, srv_addr, NULL, &num_entries, 0);
    if(rc > 0)
    {
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Got service with addr <%u,%u>\n",
                srv_addr->address.addr.port_addr.node_id,
                srv_addr->address.addr.port_addr.port_id);
      rc = 0;
      break;
    }

    do {
      retry = FALSE;
      if((time_to_wait != -1) && (poll_timeout > time_to_wait))
      {
        poll_timeout = time_to_wait;
      }

      /* Pet the dog before polling for events on socket fd's */
      diag_kick_watchdog ();

      rc = qpoll(&pfd, 1, poll_timeout);
      if(rc == 0)
      {
        if(time_to_wait != -1)
        {
          time_to_wait -= poll_timeout;
        }
        if((time_to_wait > 0) || (time_to_wait == -1)) //-1 implies infinite timeout
        {
          MSG(MSG_SSID_DIAG, MSG_LEGACY_MED, "Retry Polling!\n");
          retry = TRUE;
        }
      }
    } while(retry);
    if(rc < 0)
    {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"poll returned error = %d\n", rc);
      break;
    }
    else if(rc == 0)
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "poll timedout!\n");
      rc = -1;
      break;
    }
    else
    {
      while(qrecv(ctrl_fd, &msg, sizeof(msg), QMSG_DONTWAIT) > 0)
      {
        /* Do nothing */
      }
    }
  }
  qclose(ctrl_fd);
  return rc;
}


/*===========================================================================
FUNCTION diagcomm_io_udp_open

DESCRIPTION
  Open the socket connection for the connection object.
This function should be called to open up the UDP connection. The same socket will be used to send or receive data
for the channel.

PARAMETERS
  conn - Connection object for the channel.

DEPENDENCIES
  The connection object should contain the parameters required to open the socket

RETURN VALUE
  DIAG_EOK : SUCCESS
  DIAG_EFAILED: Opening of socket connection failed
===========================================================================*/

int diagcomm_io_udp_open(diagcomm_io_conn_type * conn)
{
   int sock = -1;
   struct qsockaddr_ipcr srv_addr;
   int rc = 0;
   int wr_timeout = UDP_WRITE_TIMEOUT; //5000 milliseconds
   //Open CMD/Data/DCI CMD/DCI Data channels
  if(diag_pfds_index >= DIAG_MAX_NUM_SOCKETS)
  {
     MSG_1(MSG_SSID_DIAG, MSG_LEGACY_FATAL,"diagcomm_io_udp_open: Number of sockets opened exceeds limit: %d", diag_pfds_index);
     return DIAG_ELIMIT;
  }

  if(! conn->isServer)
  {
      if(diagcomm_io_udp_get_srv_addr(conn->addr.address.addr.port_name, &srv_addr, -1)) //Timeout infinite ms.
     {
       MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Getting server addr failed!\n");
       return DIAG_EFAILED;
     }
  }

  sock = qsocket(AF_IPC_ROUTER, QSOCK_DGRAM, 0);
  if (sock < 0)
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"Could not create UDP socket - %d", sock);
    return DIAG_EFAILED;
  }
  

  if(0 == conn->send_flags)
  {
     rc = qsetsockopt(sock, QSOL_SOCKET, QSO_SNDTIMEO, &wr_timeout, sizeof(wr_timeout));
     if(rc)
     {
       MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Setting socket option failed with rc = %d\n", rc);
       qclose(sock);
       return DIAG_EFAILED;
     }

  }
  if(! conn->isServer)
  {
     rc = qconnect(sock, (struct qsockaddr *)&srv_addr, sizeof(srv_addr));
     if(rc < 0)
     {
       MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Connect returned %d\n", rc);
       return DIAG_EFAILED;
     }
  }
  else
  {
     rc = qbind(sock, (struct qsockaddr *)&conn->addr, sizeof(conn->addr));
     if(rc < 0)
     {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Failed to bind address!\n");
        return DIAG_EFAILED;
     }

 }

  conn->sock_handle = sock;
  conn->connected = TRUE;
  if(conn->isServer)
  {
     conn->channel_connected = TRUE;
  }
  diagcomm_io_conn_changed( conn , TRUE);

  if((NULL == conn->buf) && (0 != conn->buf_length))
  {
     conn->buf = malloc(conn->buf_length);
  }

  
  if(diag_pfds_index < DIAG_MAX_NUM_SOCKETS)
  {
    diag_pfds[diag_pfds_index].fd = sock;
    diag_pfds[diag_pfds_index].events = QPOLLIN;
    diag_pfds[diag_pfds_index].revents = 0;
    diag_sock_conn[diag_pfds_index] = conn;

    diag_pfds_index++;
  }
  return DIAG_EOK;

}

/*===========================================================================
FUNCTION diagcomm_io_udp_open_init

DESCRIPTION
  Initialize the UDP read thread. If the thread cannot be created we assert since there is no recovery.

PARAMETERS
  conn - Connection object for the channel.

DEPENDENCIES
  The connection object should contain the parameters required to open the socket

RETURN VALUE
  None
===========================================================================*/
void diagcomm_io_udp_open_init(diagcomm_io_conn_type * conn)
{
   char thread_name[16];
   int return_val = 0;
   (void)snprintf( thread_name, sizeof(thread_name), "diagcomm_udp");
   read_task_tcb = diagcomm_io_socket_create_thread(thread_name, diagcomm_io_udp_thread_main, (void*)conn, THREAD_TASK_STACK_SIZE, READ_TASK_PRIORITY);
   return_val = osal_create_diag_timer(&read_task_block_timer, read_task_tcb , DIAG_UDP_TIMEOUT_TIMER_SIG);

   /* Assert since there is no recovery from this error */
   ASSERT(OSAL_SUCCESS == return_val);
}


/*===========================================================================
FUNCTION diagcomm_io_udp_close

DESCRIPTION
  Close the socket connection

PARAMETERS
  conn - Connection object for the channel.

DEPENDENCIES
  The connection object should contain the parameters required to open the socket

RETURN VALUE
  None
===========================================================================*/
void diagcomm_io_udp_close(diagcomm_io_conn_type * conn)
{
   (void)qclose(conn->sock_handle);
}


/*===========================================================================
FUNCTION diagcomm_io_udp_close

DESCRIPTION
  This function should be called to send data through the previously connected socket associated with
  the diagcomm_io_conn_type.  Data will be written on the calling thread's context and can block if all transmit queues
  are full.   The number of bytes written will be returned.  A negative number would indicate an error condition.
  Currently the send is done in DIAG task context.

PARAMETERS
  conn - Connection object for the channel.

DEPENDENCIES
  The connection object should contain the parameters required to open the socket

RETURN VALUE
  None
===========================================================================*/
int diagcomm_io_udp_send(diagcomm_io_conn_type * conn,void *buf, unsigned int total_pkt_length)
{
  //The send is done in diag task context.
  int write_bytes = 0;

  ASSERT(total_pkt_length);
  if (conn->io_type == DIAGCOMM_IO_TYPE_IPCR_UDP)
  {
    if(conn->isServer)
    {
	  
      write_bytes = qsendto(conn->sock_handle, buf, total_pkt_length, conn->send_flags, (struct qsockaddr *)&conn->addr, sizeof(conn->addr));

    }
    else
    {
       write_bytes = qsend(conn->sock_handle, buf, total_pkt_length, conn->send_flags);
    }
  }
  else
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"diagcomm_io_udp_send:: Unsupported io_type: %d", conn->io_type);
    ASSERT(0);
  }

  return write_bytes;
}

//This is the main function for the UDP use case
//The main loop will perform a blocking read for data.  When data is received, it will determine
//the source of the data, and notify that data has been received
void diagcomm_io_udp_thread_main(void *data)
{
  diagcomm_io_udp_receive_data();
}


void diagcomm_udp_block_timeout(void)
{
  int return_val = 0;
  osal_sigs_t return_sigs = 0;

  return_val = osal_set_timer(&read_task_block_timer, 10);
  ASSERT(OSAL_SUCCESS == return_val);
  
  return_sigs = osal_thread_wait (read_task_tcb, DIAG_UDP_TIMEOUT_TIMER_SIG);
  if( return_sigs & DIAG_UDP_TIMEOUT_TIMER_SIG )
  {
	osal_reset_timer(&read_task_block_timer);
	return_val = osal_reset_sigs(read_task_tcb, DIAG_UDP_TIMEOUT_TIMER_SIG, &return_sigs);
	ASSERT(OSAL_SUCCESS == return_val);
  }
}

//This function performs the actual reading from the socket associated with the udp_data_type.  Since the UDP protocal
//sends and receives packetized data, no further processing is necessary.  All data is guaranteed to be a full DIAG packet.
//If further enhancments related to reliablility need to be implemented, this is the area where it should be implemented.
void diagcomm_io_udp_receive_data(void)
{
  int i, rc = 0;
  int num_recv = -1;
  qsocklen_t socklen;
  diagcomm_io_conn_type *conn;

  while(TRUE)
  {
    rc = qpoll(&diag_pfds[0], diag_pfds_index, 100);
    if(rc < 0)
    {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"qpoll returned %d\n", rc);
      break;
    }
    else if(rc == 0)
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "POll timedout!\n");
      continue;
    }
    else
    {
      for(i=0;i<diag_pfds_index;i++)
      {
        if(diag_pfds[i].revents & QPOLLIN)
        {
           conn = diag_sock_conn[i];
           socklen = sizeof(conn->addr);

           if(conn->used == 0)
           {
              num_recv = qrecvfrom(diag_pfds[i].fd, conn->buf, conn->buf_length, conn->recv_flags, (struct qsockaddr *)&(conn->addr), &socklen);
              if(num_recv > 0)
              {
                 osal_lock_mutex(&conn->buf_mutex);
                 conn->used = num_recv;
                 osal_unlock_mutex(&conn->buf_mutex);
                 diagcomm_io_notify_rx(conn);
                 continue;
              }
           }else
           {
              diagcomm_udp_block_timeout();
              continue;
           }

           if(num_recv < 0)
           {
             MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"qrecvfrom returned an error %d",num_recv);
             break;
           }
           else if(0 == num_recv)
           {
              //Signal writers that they can continue writing more.
             diagcomm_io_enable_flow(conn);
           }
        }
      }
    }
  }
}


// This function will read from the socket for the connection object into the conn->buf buffer
int diagcomm_io_udp_read(diagcomm_io_conn_type *conn)
{
   int num_recv = -1;
   qsocklen_t socklen;

   if(NULL == conn->buf) return DIAG_EPARAM;
   socklen = sizeof(conn->addr);
   /* conn->used should be zero since we will not be reading in diag task context */
   /* If used bytes is not zero, something is wrong and processing is not being done before the next read */
   ASSERT(conn->used == 0);

   num_recv = qrecvfrom(conn->sock_handle, conn->buf, conn->buf_length, conn->recv_flags, (struct qsockaddr *)&(conn->addr), &socklen);
   if(num_recv > 0)
   {
      conn->used = num_recv;
   }
   else if(num_recv < 0)
   {
     MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"qrecvfrom returned an error %d",num_recv);
   }
   else if(0 == num_recv)
   {
     //Signal writers that they can continue writing more.
     diagcomm_io_enable_flow(conn);
   }
   return num_recv;
}

#endif

