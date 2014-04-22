/*===========================================================================

  Copyright (C) 2018-2019 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/4.0/datalink/apr_qsocket/src/apr_qsocket.c#2 $
  $Author: svutukur $

=============================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "mmstd.h"
#include "apr_dbg_msg.h"
#include "apr_errcodes.h"
#include "apr_list.h"
#include "qsocket_ipcr.h"
#include "qurt_thread.h"
#include "apr_thread.h"
#include "aprv2_ipc_config.h"
//#include "ipc_router_os.h"
#include "aprv2_api.h"
#include "apr_comdef.h"
#include "aprv2_api_inline.h"
#include "apr_qsocket.h"

/*===========================================================================
                  CONSTANT / MACRO DACLARATIONS
===========================================================================*/
#define APR_QSOCKET_RCVE_BUFFER_SIZE_IN_BYTES (1*1024)
#define APR_QSOCKET_WAIT_FOR_SVC_TIMEOUT_IN_MS (-1)  /* Infinite timeout */
#define APR_QSOCKET_BROADCAST_PACKET_SIZE_IN_BYTES (100)
#define APR_QSOCKET_TASK_PRIORITY ( 181 ) //Need to check on how to fetch this as mdsp will not have elite thread priorites support
#define APR_QSOCKET_TASK_STACK_SIZE_IN_BYTES (8*1024)

qurt_thread_attr_t apr_qsocket_thread_attrs[APR_QSOCKET_MAX_PORTS];
qurt_thread_t apr_qsocket_thread_ids[APR_QSOCKET_MAX_PORTS];


/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/
int32_t apr_qsocket_tx(uint32_t port_index, void *buf, uint32_t length);
struct qsockaddr_ipcr apr_qsocket_client_addrs[APRV2_PL_MAX_DOMAIN_ID_V];

typedef struct apr_qsocket_port_t
{
   ipcr_name_t socket_name; //stores the socket name
   int32_t is_service; //indicates service/client
   int32_t port_index; 
   int32_t data_fd; //stores the data socket 
   int32_t ctrl_fd; //stores control socket information
   int32_t is_connected; //indicates connection established between end points
   qurt_thread_t thread_id; //thread information
   void* thread_stack; //thread stack
}apr_qsocket_port_t;

/*===========================================================================
                        GLOBAL VARIABLES
===========================================================================*/

apr_qsocket_port_t* apr_qsocket_port_info[ 2 ];
struct qsockaddr_ipcr sender_addr;
void *apr_qsocket_temp_rcve_buffer;
/*===========================================================================
                        LOCAL FUNCTIONS
===========================================================================*/

/* Function to receive the data from given client or service name.*/
static int32_t apr_qsocket_dispatch_fn(aprv2_packet_t *packet, void *dispatch_data)
{
    return APR_EOK;
}


/* Function for service to receive the packets */
static int32_t apr_qsocket_service_receive_data(int32_t port_index)
{

  int32_t rc=APR_EOK,len;
  uint16_t src_addr;
  uint16_t src_port;
  uint16_t dst_addr;
  uint16_t dst_port;
  uint32_t token;
  uint32_t opcode;
  uint32_t handle;
  uint32_t packet_size;
  uint32_t payload_size;
  uint32_t header_size;
  uint32_t src_domain_id;
  void *payload;
  uint32_t msg_type;
  aprv2_packet_t* temp_buffer;
  apr_qsocket_port_t* port = apr_qsocket_port_info[port_index];
  qsocklen_t sender_addrlen = sizeof(sender_addr);

  //Read the next available packet size (and not the actual packet) from the connected endpoint/client
  rc = qrecv(port->data_fd, NULL, 0, QMSG_PEEK);

  // negative error code indicates failure
  if(rc < 0)
  {
    APR_DBG_1(APR_DBG_ERROR,"APR_QSOC_SERVICE:Packet peeking returned failure: %d\n", rc)
    return APR_EFAILED;
  }

  if (rc > APR_QSOCKET_RCVE_BUFFER_SIZE_IN_BYTES)
  {
   APR_DBG_2(APR_DBG_ERROR,"APR_QSOC_SERVICE:Cannot receive as rcved pkt size:[%d] > expected packet size:[%d]",
             rc, APR_QSOCKET_RCVE_BUFFER_SIZE_IN_BYTES);
  }

  len = (size_t)rc;
  rc = qrecvfrom(port->data_fd, apr_qsocket_temp_rcve_buffer, len, QMSG_DONTWAIT, (struct qsockaddr *)&sender_addr, &sender_addrlen);

  if(rc != (int)len) // Check length copied and peaked length are same, else bailout...
  {
    //ipc_router_os_free(apr_qsocket_rcve_tmp_buffer);
    APR_DBG_2(APR_DBG_ERROR,"APR_QSOC_SERVICE:Received [%d] and Peaked length [%d] are not matching", rc,(int)len);
    return APR_EFAILED;
  }
  else //If packet is peaked successfully, now process it.
  {
    temp_buffer = ((aprv2_packet_t*)apr_qsocket_temp_rcve_buffer);
 
    APR_DBG_1(APR_DBG_HIGH,"APR_QSOCKET_SERVICE: Incoming packet opcode (0x%08x)", temp_buffer->opcode); //dbg purposer remove later..


    //If it's not a broadcast pkt, forward to intended clients..
    if(temp_buffer->opcode != APRV2_CMDID_HACK_INIT_IPC)
    {
      //Send the packet to the intended users/clients.
      handle = ((uint32_t)(((APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, temp_buffer->dst_addr)) << 8)
               | (APR_GET_FIELD(APRV2_PKT_SERVICE_ID, temp_buffer->dst_addr))));

      packet_size = APRV2_PKT_GET_PACKET_BYTE_SIZE(temp_buffer->header);
      opcode = temp_buffer->opcode;
      src_addr = temp_buffer->src_addr;
      src_port = temp_buffer->src_port;
      dst_addr = temp_buffer->dst_addr;
      dst_port = temp_buffer->dst_port;
      token = temp_buffer->token;
      payload = APRV2_PKT_GET_PAYLOAD( void, temp_buffer );
      msg_type = APR_GET_FIELD( APRV2_PKT_MSGTYPE, temp_buffer->header); 
      header_size = APRV2_PKT_GET_HEADER_BYTE_SIZE(temp_buffer->header);
      payload_size = packet_size - header_size;

      //allocate and send to the clients..
      rc = __aprv2_cmd_alloc_send ( handle,msg_type, src_addr, src_port,dst_addr, dst_port, token, opcode,  payload, payload_size);

      if ( rc != APR_EOK)
      {
        APR_DBG_1(APR_DBG_ERROR,"APR_QSOC_SERVICE:Packet send returned failure [%d]", rc);
        //ipc_router_os_free(temp_buffer);
        return APR_EFAILED;
      }

    }
    //If it's a broadcast packet then store the client address
    else
    {
      src_domain_id  = APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, temp_buffer->src_addr );
      APR_DBG_3(APR_DBG_HIGH,"APR_QSOC_SERVICE: broadcast packet [0x%08x] from src_addr [0x%x], src_port [0x%x]", 
                temp_buffer->opcode, temp_buffer->src_addr, temp_buffer->src_port);
      APR_DBG_3(APR_DBG_HIGH,"APR_QSOC_SERVICE:broadcast packet: got [%d] bytes from client [%d]:[%d]", rc, 
      sender_addr.address.addr.port_addr.node_id, 
      sender_addr.address.addr.port_addr.port_id);
      //Store the client address for further processing..
      memcpy( (char*)&apr_qsocket_client_addrs[src_domain_id],(char*) &sender_addr, sender_addrlen );
      port->is_connected = 1;
    }
  }
  
  return APR_EOK;

}


/* Client pkt rcve data function */
static int32_t apr_qsocket_client_receive_data(int32_t port_index)
{

  int32_t rc=APR_EOK,len;
  uint16_t src_addr;
  uint16_t src_port;
  uint16_t dst_addr;
  uint16_t dst_port;
  uint32_t token;
  uint32_t opcode;
  uint32_t handle;
  uint32_t packet_size;
  void *payload;
  uint32_t msg_type;
  aprv2_packet_t* temp_buffer;
  apr_qsocket_port_t* port = apr_qsocket_port_info[port_index];
  
  //Read the next available packet size (and not the actual packet) from the connected endpoint/service
  rc = qrecv(port->data_fd, NULL, 0, QMSG_PEEK);

  if(rc < 0)
  {
    APR_DBG_1(APR_DBG_ERROR,"APR_QSOCKET_CLIENT:Packet peeking returned failure: %d\n", rc)
    return APR_EFAILED;
  }

  // negative error code indicates failure
  if (rc > APR_QSOCKET_RCVE_BUFFER_SIZE_IN_BYTES)
  {
    APR_DBG_2(APR_DBG_ERROR,"APR_QSOCKET_CLIENT:Cannot copy as rcved pkt size:[%d] > expected packet size:[%d]", 
              rc, APR_QSOCKET_RCVE_BUFFER_SIZE_IN_BYTES);
  }

  len = (size_t)rc;
  rc = qrecvfrom(port->data_fd, apr_qsocket_temp_rcve_buffer, (size_t)len,QMSG_DONTWAIT, NULL, NULL);

  if(rc != (int)len)
  {
    //ipc_router_os_free(apr_qsocket_rcve_tmp_buffer);
    APR_DBG_2(APR_DBG_ERROR,"APR_QSOCKET_CLIENT:Recved %d and Peaked length (%d) are not matching", rc,(int)len);
    return APR_EFAILED;
  }
  else //If packet is peaked successfully, now process it.
  {
    temp_buffer = ((aprv2_packet_t*)apr_qsocket_temp_rcve_buffer);

    APR_DBG_1(APR_DBG_HIGH,"APR_QSOCKET_CLIENT: Incoming packet opcode (0x%08x)", temp_buffer->opcode); //dbg purposer remove later..

    {
      //Send the packet to the intended users.
      handle = ((uint32_t)(((APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, temp_buffer->dst_addr)) << 8)
                 | (APR_GET_FIELD(APRV2_PKT_SERVICE_ID, temp_buffer->dst_addr))));

      packet_size = APRV2_PKT_GET_PACKET_BYTE_SIZE(temp_buffer->header);
      opcode = temp_buffer->opcode;
      src_addr = temp_buffer->src_addr;
      src_port = temp_buffer->src_port;
      dst_addr = temp_buffer->dst_addr;
      dst_port = temp_buffer->dst_port;
      token = temp_buffer->token;
      payload = APRV2_PKT_GET_PAYLOAD( void, temp_buffer );
      msg_type = APR_GET_FIELD( APRV2_PKT_MSGTYPE, temp_buffer->header); 
      rc = __aprv2_cmd_alloc_send ( handle,msg_type, src_addr, src_port,dst_addr, dst_port, token, opcode,  payload, packet_size);

      if ( rc != APR_EOK)
      {
        APR_DBG_1(APR_DBG_ERROR,"APR_QSOCKET_CLIENT:Packet send returned failure [%d]", rc);
        //ipc_router_os_free(temp_buffer);
        return APR_EFAILED;
      }
    }
  }
  
  return APR_EOK;

}


static int32_t  apr_qsocket_wait_for_service_connect(ipcr_name_t *svc, int timeout, int data_fd, int32_t* ctrl_fd)
{

  int rc = APR_EOK;
  struct qsockaddr_ipcr srv_addr;
  struct qpollfd pfd;

  /* Create a socket */
  *ctrl_fd = qsocket(AF_IPC_ROUTER, QSOCK_DGRAM, 0);
  if(*ctrl_fd < 0)
  {
    APR_DBG_1(APR_DBG_ERROR,"APR_QSOCKET_CLIENT:Socket creation failed. ctrl_fd = %d", *ctrl_fd);
    return -1;
  }

  /* Mark this as a control socket to listen on events 
   * on the name server */
  rc = qsetsockopt(*ctrl_fd, QSOL_IPC_ROUTER, QSO_IPCR_SET_CONTROL_PORT, NULL, 0);
  if(rc)
  {
    APR_DBG_2(APR_DBG_ERROR,"APR_QSOCKET_CLIENT: qsetsockopt(SET_CONTROL_PORT) failed with rc = %d, closing the socket = %d",
              rc, *ctrl_fd);
    qclose(*ctrl_fd);
    return -1;
  }

  /* Specify the service ID "0x1005" that we are intersted in */
  rc = qsetsockopt(*ctrl_fd, QSOL_IPC_ROUTER, QSO_IPCR_SET_SERVICE_PREF, &svc->service, sizeof(svc->service));
  if(rc)
  {
    APR_DBG_2(APR_DBG_ERROR,"APR_QSOCKET_CLIENT: qsetsockopt(SET_SERVICE_PREF) failed with rc= %d, closing the socket = %d",
              rc, *ctrl_fd);
    qclose(*ctrl_fd);
    return -1;
  }

  pfd.fd = *ctrl_fd;
  pfd.events = QPOLLIN;
  pfd.revents = 0;

  while(1)
  {

    unsigned int num_entries = 1;
    ipcr_ctrl_msg msg;
    rc = ipcr_find_name(data_fd, svc, &srv_addr, NULL, &num_entries, 0);

    if(rc > 0)
    {
      APR_DBG_2(APR_DBG_HIGH,"APR_QSOCKET_CLIENT: found port with node_id(%u), port_id(%u) matching service name", 
                srv_addr.address.addr.port_addr.node_id,
                srv_addr.address.addr.port_addr.port_id);

      //Once service name is found, connect the socket "data_fd" to the endpoint/service.
      rc = qconnect(data_fd, (struct qsockaddr *)&srv_addr, sizeof(srv_addr));
      if(rc < 0)
      {
        APR_DBG_3(APR_DBG_ERROR,"APR_QSOCKET_CLIENT: connecting to service with node_id(%u), port_id(%u) failed with result:%d",
                  srv_addr.address.addr.port_addr.node_id, srv_addr.address.addr.port_addr.port_id, rc);
      }
      break;
    }

    /* Wait (infinite timeout) until the service is up */
    rc = qpoll(&pfd, 1, timeout); 
    if(rc < 0)
    {
      APR_DBG_1(APR_DBG_ERROR,"APR_QSOCKET_CLIENT: qpoll for service returned error = %d", rc);
      qclose(data_fd);
      break;
    }
    else if(rc == 0)
    {
      APR_DBG_0(APR_DBG_ERROR,"APR_QSOCKET_CLIENT: qpoll for servicec timedout!");
      rc = -1;
      qclose(data_fd);
      break;
    }
    else
    {
      while(qrecv(*ctrl_fd, &msg, sizeof(msg), QMSG_DONTWAIT) > 0)
      {
      }
    }
  }
  
  return rc;

}


APR_INTERNAL void apr_qrecv_workloop_server(void *thread_params)
{

  int32_t rcve_status = APR_EOK;
  int32_t rc = APR_EOK;
  int32_t return_status =  APR_EOK;
  int32_t port_index;
  struct qsockaddr_ipcr svc_name;
  apr_qsocket_port_t* port;

  port_index = *(int32_t*)thread_params; 
  port = apr_qsocket_port_info[port_index]; /*Pointer to the apr qsocket port*/
  port->thread_id = qurt_thread_get_id(); //Get the thread id for later use

  APR_DBG_1(APR_DBG_ERROR,"Server workloop in progress :%d",*(int32_t*)thread_params);

  //create a socket
  port->data_fd = qsocket(AF_IPC_ROUTER, QSOCK_DGRAM, 0);
  
  if(port->data_fd < 0)
  {
    APR_DBG_1(APR_DBG_ERROR,"APR_QSOCKET_SERVICE: socket creation failed. data_fd = %d",port->data_fd);
  }
   
  /*Make the socket structure filled up and ready for binding in to qsocket */
  svc_name.sa_family = AF_IPC_ROUTER;
  svc_name.address.addrtype = IPCR_ADDR_NAME;
  svc_name.address.addr.port_name = port->socket_name ;
 
  /* Bind the socket/port to the server, so that clients will get notified/connected */
  rc =  qbind(port->data_fd, (struct qsockaddr *)&svc_name, sizeof(svc_name));

  if(rc < 0)
  {
    APR_DBG_2(APR_DBG_ERROR,"APR_QSOCKET_SERVICE:qbind failed with result = %d, closing the socket %d",
              rc, port->data_fd);
    //qclose(port->data_fd);
    return_status = APR_EFAILED;
  }
   
  /* Enter to infinite loop for receiving the packets */
  do
  {
    rcve_status = apr_qsocket_service_receive_data(port_index); 

    if (rcve_status != APR_EOK)
    {
      APR_DBG_1(APR_DBG_ERROR,"APR_QSOCKET_SERVICE: service_receive_data received error [%d]", rcve_status);
      return_status = rcve_status; 
	  //qclose(port->data_fd); //clean up by closing the ports..
      break; //exit on failure..
    }

  } while(return_status == APR_EOK); //If no failure, continue to rcve pkts
  
   qclose(port->data_fd);//clean up by closing the ports before thread close..
}


/*client workloop */
APR_INTERNAL void apr_qrecv_workloop_client(void *thread_params)
{

  int  rc, rcve_status = 0;
  struct qpollfd pfd[2];
  apr_qsocket_port_t* port;
  void *packet_ptr;
  int32_t port_index;
  struct qsockaddr_ipcr svc_addr;
  int return_status= APR_EOK;
  uint32_t my_handle=0;
  uint16_t my_addrs=0;
  
  port_index = *(int32_t*)thread_params; 
  
  APR_DBG_1(APR_DBG_ERROR,"Client workloop in progress :%d",*(int32_t*)thread_params);
  port = apr_qsocket_port_info[port_index];
  port->thread_id = qurt_thread_get_id(); //Get the thread id for later use
   
  //Put all here if it is needed to be done once..
  aprv2_packet_t* broadcast_packet;
  
  /* Client enters this loop to process below
  1. Open data socket
  2. open control socket
  3. wait for server to come up
  */
  do
  {
    if ( rcve_status != APR_EOK )
    {
      break; //exit...
    }
   
    /* Create a socket */
    port->data_fd = qsocket(AF_IPC_ROUTER, QSOCK_DGRAM, 0);
  
    if(port->data_fd < 0)
    {
      APR_DBG_1(APR_DBG_ERROR,"APR_QSOCKET_CLIENT: socket creation failed. data_fd = %d",port->data_fd);
      break;
    }
  
    //Wait for service to come up...
    if( apr_qsocket_wait_for_service_connect(&port->socket_name, APR_QSOCKET_WAIT_FOR_SVC_TIMEOUT_IN_MS,
                                             port->data_fd, &port->ctrl_fd) < 0)
    {
      APR_DBG_0(APR_DBG_ERROR,"APR_QSOCKET_CLIENT: service did not start within the expected wait time!");
      qclose(port->data_fd);
      break;
    }
    else
    {
      APR_DBG_0(APR_DBG_HIGH,"APR_QSOCKET_CLIENT: service started!");
      port->is_connected = 1;
    }

    //Broadcast client address to service, so that service can send packets to the client..
    /* Multicast the message to all services and wait for one to respond */
    svc_addr.sa_family = AF_IPC_ROUTER;
    svc_addr.address.addrtype = IPCR_ADDR_NAME;
    svc_addr.address.addr.port_name = port->socket_name;
  
    //Prepare the packet for broadcast..
    rc = __aprv2_cmd_alloc (port->thread_id, APRV2_ALLOC_TYPE_COMMAND_RAW, APR_QSOCKET_BROADCAST_PACKET_SIZE_IN_BYTES,&broadcast_packet);
    if (rc != APR_EOK)
    {
      return_status = rc;
      APR_DBG_1(APR_DBG_ERROR,"APR_QSOCKET_CLIENT: APR packet allocation failed for broadcast packet %d", return_status);
	  qclose(port->ctrl_fd);
	  qclose(port->data_fd );
      break;
    }

    //Prepare broadcast packet..Fill in all the headers..
    //Fill up header
    broadcast_packet->header &= ~( APRV2_PKT_MSGTYPE_MASK );
    broadcast_packet->header |= APR_SET_FIELD( APRV2_PKT_MSGTYPE, APRV2_ALLOC_TYPE_COMMAND_RAW );
    rc = __aprv2_cmd_register2(&my_handle, NULL,0, 0, apr_qsocket_dispatch_fn, NULL, &my_addrs);
  
    if(rc != APR_EOK)
    {
      APR_DBG_1(APR_DBG_ERROR,"APR_QSOCKET_CLIENT: apr_cmd_register2 failed :%d",rc);
      return_status = rc;
	  qclose(port->ctrl_fd);
	  qclose(port->data_fd );
      break;
    }
      
    /*Broadcast Packet packing */
    broadcast_packet->src_addr = my_handle;
    broadcast_packet->src_port = APR_NULL_V;
    broadcast_packet->dst_addr = APR_NULL_V;
    broadcast_packet->dst_port = APR_NULL_V;
    broadcast_packet->token = APR_NULL_V;
    broadcast_packet->opcode = APRV2_CMDID_HACK_INIT_IPC; //Opcode to identify broadcast
    packet_ptr = APRV2_PKT_GET_PAYLOAD( void, broadcast_packet );
    
    //send the broadcast packet to service
    rc = qsendto(port->data_fd, broadcast_packet, APR_QSOCKET_BROADCAST_PACKET_SIZE_IN_BYTES, 0, (struct qsockaddr *)&svc_addr, sizeof(svc_addr));
  
    if (rc ==  APR_QSOCKET_BROADCAST_PACKET_SIZE_IN_BYTES)
    {
      APR_DBG_1(APR_DBG_HIGH,"APR_QSOCKET_CLIENT: Broadcast packet to service (address=%d) is successful!",port->socket_name.service); 
    }
    else
    {
      APR_DBG_1(APR_DBG_ERROR,"APR_QSOCKET_CLIENT: Broadcast packet to service (address=%d) is failed!",port->socket_name.service); 
      return_status = APR_EOK;
	  qclose(port->ctrl_fd);
	  qclose(port->data_fd );
      break;
    }
   
    /* This loop will wait on below events
    1. QPOLLHUP --> Remote side has hungup on this port
    2. QPOLLIN  --> when there is an event to receive the packet..
    */
  
    /* Fill up qpoll struct for ctrl events */
    pfd[0].fd = port->ctrl_fd; //service going down.up
    pfd[0].events = QPOLLIN;
    pfd[0].revents = 0;
  
    /* Fill up qpoll struct for data events */
    pfd[1].fd = port->data_fd;  ///nrmal packets
    pfd[1].events = QPOLLIN;
    pfd[1].revents = 0;
   
    while(1)
    {
      rc = qpoll(pfd, 2, -1); //-1 indicates infinite wait..

      if(rc < 0)
      {
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "APR_QSOCKET_CLIENT: qpoll returned %d\n", rc);
        return_status = rc;
        break;
      }
      else if(rc == 0)
      {
        MSG_LOW("APR_QSOCKET_CLIENT: qpoll timedout!\n",0,0,0);
        continue;
      }
      else
      { 
        { /* data socket */
          if(pfd[1].revents & QPOLLIN) //incoming packet.
          {
            //Rcve the packet once a event is notified...
            rcve_status = apr_qsocket_client_receive_data(port_index); 
            if (rcve_status != APR_EOK)
            {
              APR_DBG_0(APR_DBG_ERROR,"APR_QSOCKET_CLIENT: Incoming packet receive from service failed..");
              return_status = rcve_status; 
              break; //exit...
            }
          }
          else if (pfd[1].revents & QPOLLHUP) //Assuming server restarted or hung (ADSP SSR), restart client...
          {
            APR_DBG_1(APR_DBG_ERROR,"APR_QSOCKET_CLIENT: Remote/service disconnected for the client port: %d", port_index);
            APR_DBG_0(APR_DBG_ERROR,"APR_QSOCKET_CLIENT: Restarting client");
            //restart the client connection assuming remote went down..
            //Restarting client..

            break;
          }
        }
        {  /* control socket */
          if (pfd[0].revents & QPOLLIN)  /* any incoming packets on control socket? */
          {
            ipcr_ctrl_msg msg;
            while(qrecv(port->ctrl_fd, &msg, sizeof(msg), QMSG_DONTWAIT) > 0)
            {
              /* just drop the packets*/
              APR_DBG_1(APR_DBG_ERROR,"APR_QSOCKET_CLIENT: packets received on control socket: %d", port_index);
            }
          }
        }
      }

    } // << end while for qpoll..
    qclose(port->data_fd);
    qclose(port->ctrl_fd);
	
  } while ( return_status == APR_EOK);

}


/*===========================================================================
FUNCTION apr_qsocket_init
Initalizes the needed qsockets and spawns the worker threads for receiving the packets. 
===========================================================================*/

int32_t apr_qsocket_init (void)
{

  int32_t rc,checkpoint=0;
  int32_t port_index,index=0;
  
  /*memset apr qsocket adress table */
  memset(&apr_qsocket_client_addrs[0], 0, APRV2_PL_MAX_DOMAIN_ID_V*sizeof(struct qsockaddr_ipcr));
 
  /*Create a buffer for copying the packets. All the packets received from qsocket are first copied to this buffer
  and then they are sent to clients using aprv2_cmd_alloc_send */
  apr_qsocket_temp_rcve_buffer = malloc(APR_QSOCKET_RCVE_BUFFER_SIZE_IN_BYTES);
  
  if ( apr_qsocket_temp_rcve_buffer == NULL)
  {
    APR_DBG_0(APR_DBG_ERROR,"apr_qsocket_init: could not allocate memory for rcvng apr qsocket packets");
    return APR_EFAILED;
  }
  
  /* Create the attributes needed for thread creation */
  
  /*Open all the qsockets as per the ipc configuration.*/
  for (index = 0; index < sizeof(apr_qsocket_port_info_table)/sizeof(apr_qsocket_port_info_t); index++)
  {
    port_index = apr_qsocket_port_info_table[index].port_index; //Get the port number from ipc config
  
    APR_DBG_1(APR_DBG_HIGH,"apr_qsocket_init for port_index :%d",port_index);
  
    apr_qsocket_port_info[port_index] = malloc(sizeof(apr_qsocket_port_t));
    if (apr_qsocket_port_info[ port_index] == NULL)
    {  
      APR_DBG_1(APR_DBG_ERROR,"apr_qsocket_init: could not allocate memory for apr qsocket port :%d",port_index);
      rc = APR_ENOMEMORY;
      checkpoint = 1;
      break;     
    }

    /*Copy all the data to the qsocket port info structure, so that globally this structure can be referred */
    apr_qsocket_port_info[port_index]->socket_name.service = apr_qsocket_port_info_table[index].service; //Service address
    apr_qsocket_port_info[port_index]->socket_name.instance = apr_qsocket_port_info_table[index].instance; //Instance address
    apr_qsocket_port_info[port_index]->is_service = apr_qsocket_port_info_table[index].is_server; //service/client
    apr_qsocket_port_info[port_index]->port_index = port_index; //port index used in ipc_config table, not a real need. Place holder 
    apr_qsocket_port_info[port_index]->is_connected = 0;
    
    /*Thread stack creation*/
    apr_qsocket_port_info[port_index]->thread_stack = NULL;
    apr_qsocket_port_info[port_index]->thread_stack = malloc(APR_QSOCKET_TASK_STACK_SIZE_IN_BYTES);
    
    /*Qurt thread creation attributes*/
    qurt_thread_attr_init(&apr_qsocket_thread_attrs[index]);   //Init the params
    qurt_thread_attr_set_name(&apr_qsocket_thread_attrs[index], apr_qsocket_port_info_table[index].thread_name);
    qurt_thread_attr_set_stack_addr(&apr_qsocket_thread_attrs[index], apr_qsocket_port_info[port_index]->thread_stack);
    qurt_thread_attr_set_stack_size(&apr_qsocket_thread_attrs[index], APR_QSOCKET_TASK_STACK_SIZE_IN_BYTES);
    qurt_thread_attr_set_priority(&apr_qsocket_thread_attrs[index], APR_QSOCKET_TASK_PRIORITY);

    if (apr_qsocket_port_info[port_index]->thread_stack == NULL)
    {
      APR_DBG_1(APR_DBG_ERROR,"apr_qsocket_init: %s thread creation failed due to malloc failure", apr_qsocket_port_info_table[index].thread_name);
      rc = APR_ENOMEMORY;
      checkpoint = 2;
      break;
    }

    /*Service/Client thread creation */
    if (apr_qsocket_port_info[port_index]->is_service == 1)
    {
      rc = qurt_thread_create(&apr_qsocket_thread_ids[index], &apr_qsocket_thread_attrs[index], apr_qrecv_workloop_server, (int32_t*)&apr_qsocket_port_info[port_index]->port_index);
    }
    else
    {
      rc = qurt_thread_create(&apr_qsocket_thread_ids[index], &apr_qsocket_thread_attrs[index], apr_qrecv_workloop_client, (int32_t*)&apr_qsocket_port_info[port_index]->port_index);
    }

    if(rc != APR_EOK)
    {
      APR_DBG_2(APR_DBG_HIGH,"apr_qsocket_init: %s thread creation failed with error %d",apr_qsocket_port_info_table[index].thread_name, rc);
      rc = APR_EFAILED;
      checkpoint = 3;
      break;
    }
  }
 
  switch ( checkpoint )
  {
    case 3:
      free(apr_qsocket_port_info[port_index]->thread_stack);
      /*-fallthru */
    case 2:
      free(apr_qsocket_port_info[port_index]);
      /*-fallthru */
    case 1:
      free(apr_qsocket_temp_rcve_buffer);
      /*-fallthru */
    default:
      break;
  }

  return rc;
  
} /*<<end  qsocket init >> */


/*Function to send the packets to remote processors/addresses using qsocket*/
int32_t apr_qsocket_tx(uint32_t port_index, void *buf, uint32_t length)
{

  uint32_t dst_domain_id;
  int32_t rc = APR_EOK;
  aprv2_packet_t *packet;
  packet = ((aprv2_packet_t*)buf);
   
  dst_domain_id  = APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, ((aprv2_packet_t*)buf)->dst_addr );
   
  /*if iam server, then i will lookup my client address in routing table to send to approriate remote client*/
  if  (apr_qsocket_port_info[port_index]->is_service == 1)
  {
    /* if client is registered with service*/
    if (apr_qsocket_client_addrs[dst_domain_id].address.addr.port_addr.port_id != 0 )
    {
      rc = qsendto(apr_qsocket_port_info[port_index]->data_fd, buf, length, 0, 
                  (struct qsockaddr *)&apr_qsocket_client_addrs[dst_domain_id], sizeof(struct qsockaddr_ipcr));
      if (rc < 0)
      {
        APR_DBG_2(APR_DBG_ERROR,"apr_qsocket_tx(): qsendto() from service to client failed. socket descriptor=(%d), dest_domain_id=(%d)",
                  apr_qsocket_port_info[port_index]->data_fd, dst_domain_id);
      }
    }
    else
    {
      APR_DBG_1(APR_DBG_ERROR,"apr_qsocket_tx() failed: client from dest_domain_id(%d) not registered", dst_domain_id);
      return APR_ENOTEXIST; //client does not exists..
    }
  }
  else 
  {
    /* if i am client, i can send to service directly, as i am a connected service */
    rc = qsend(apr_qsocket_port_info[port_index]->data_fd ,  buf, length, 0);    
    if (rc < 0)
  {
      APR_DBG_1(APR_DBG_ERROR,"apr_qsocket_tx(): qsend() from client to service failed. socket descriptor=(%d)", apr_qsocket_port_info[port_index]->data_fd);
    }
  }
      
  /* if the sent bytes is not matching with the returned bytes, then treat as a failure */  
  if ( rc != length)
  {
    if (rc == QEHOSTUNREACH) /* Host is unreachable */ 
    {
      APR_DBG_2(APR_DBG_HIGH,"apr_qsocket_tx() failed: Client is down [%d], sent to [%d] failed", rc, dst_domain_id);
    }
    else
    {
      APR_DBG_2(APR_DBG_HIGH,"apr_qsocket_tx() failed: sent [%d], expected [%d]", rc, length);
    }
    return APR_EFAILED;
  }
  else
  {
    /* Free the packet after the packet is placed in qsocket FIFO */
    ( void ) __aprv2_cmd_free( packet->dst_addr, packet );
    return APR_EOK;
  }
   
}
   
   
   