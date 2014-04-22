/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diag I/O Configuration

General Description

Copyright (c) 2014, 2016, 2019 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.tx/6.0/services/diag/platform/bmp/src/diagcomm_cfg.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------  
01/10/19   kdey    Porting of Qsockets
07/20/16   nk      Changes to support ThreadX
01/13/16   is      Send feature mask before processing commands
11/25/14   ph      Mainlined the feature DIAG_CONSUMER_API.
02/21/14   is      Command/response separation 
01/07/14   xy      Added fusion DCI feature    

===========================================================================*/

#include "diagcomm_io.h"
#include "stdio.h"               /* For snprintf */

#include "diagcomm_dci.h"
#include "assert.h"
#include "qsocket_ids.h"
#include "diagi_v.h"
/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */


char diag_ctrl_read_buf[DIAG_READ_BUF_SIZE]; //8k
char diag_cmd_read_buf[DIAG_READ_BUF_SIZE*2]; //16k
char diag_data_read_buf[DIAG_READ_BUF_SIZE*2]; //16k

#ifdef DIAG_CONSUMER_API
  char diag_dci_cmd_read_buf[DIAG_READ_BUF_SIZE];
  char diag_dci_data_read_buf[DIAG_READ_BUF_SIZE];
#endif /* DIAG_CONSUMER_API */


diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; 

#ifdef DIAG_CONSUMER_API
  extern diagcomm_io_conn_type diagcomm_io_dci_conn[NUM_SMD_PORTS];
  extern diagcomm_io_conn_type diagcomm_io_dci_cmd_conn[NUM_SMD_PORTS];
#endif /* #ifdef DIAG_CONSUMER_API */

#ifndef QSOCKET_DIAG_ID
#define QSOCKET_DIAG_ID 0x1001
#endif

/*===========================================================================

FUNCTION DIAGCOMM_IO_INIT

DESCRIPTION
  Initilizes Diag variables for interprocess communication and I/O
  
PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_io_init( void )
{
#if defined(DIAG_MP) || defined(DIAG_SOCKET_SUPPORT)
  int ret;
#endif

  uint8 i, j = 0;
  
  for( i=0; i < NUM_SMD_PORT_TYPES; i++ )
  {
    for( j=0; j < NUM_SMD_PORTS; j++ )
    {
      (void)memset( &diagcomm_io_conn[i][j], 0, sizeof(diagcomm_io_conn_type) );
      (void)memset( &diagcomm_io_ctrl_conn[i][j], 0, sizeof(diagcomm_io_conn_type) );
      (void)memset( &diagcomm_io_cmd_conn[i][j], 0, sizeof(diagcomm_io_conn_type) );
      #if defined(DIAG_MP_MASTER)
    #ifdef DIAG_CONSUMER_API
      if (i== DIAGCOMM_PORT_SMD ) 
      {
         (void)memset( &diagcomm_io_dci_conn[j], 0, sizeof(diagcomm_io_conn_type) );
         (void)memset( &diagcomm_io_dci_cmd_conn[j], 0, sizeof(diagcomm_io_conn_type) );
      }
    #endif /* #ifdef DIAG_CONSUMER_API */
      #endif	  
      diagcomm_io_conn[i][j].port_num = (diagcomm_enum_port_type)j;
      diagcomm_io_conn[i][j].connected = FALSE;
      diagcomm_io_conn[i][j].channel_connected = FALSE;
      diagcomm_io_conn[i][j].feature_mask_sent = FALSE;
      diagcomm_io_conn[i][j].data_pending = 0;
      diagcomm_io_conn[i][j].io_type = DIAGCOMM_IO_TYPE_INVALID;
      diagcomm_io_conn[i][j].channel_type = DIAGCOMM_IO_DATA;
      diagcomm_io_conn[DIAGCOMM_PORT_SMD][j].port_type = DIAGCOMM_PORT_SMD;
      diagcomm_io_conn[DIAGCOMM_PORT_SIO][j].port_type = DIAGCOMM_PORT_SIO;
      diagcomm_io_conn[DIAGCOMM_PORT_SMD][j].allow_flow = TRUE;
      
	  diagcomm_io_ctrl_conn[i][j].port_num = (diagcomm_enum_port_type)j;
      diagcomm_io_ctrl_conn[i][j].connected = FALSE;
      diagcomm_io_ctrl_conn[i][j].channel_connected = FALSE;
      diagcomm_io_ctrl_conn[i][j].feature_mask_sent = FALSE;
      diagcomm_io_ctrl_conn[i][j].data_pending = 0;
      diagcomm_io_ctrl_conn[i][j].io_type = DIAGCOMM_IO_TYPE_INVALID;
	  diagcomm_io_ctrl_conn[i][j].channel_type = DIAGCOMM_IO_CTRL;
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][j].port_type = DIAGCOMM_PORT_SMD;
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][j].port_type = DIAGCOMM_PORT_SIO;
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][j].allow_flow = TRUE;

	  diagcomm_io_cmd_conn[i][j].port_num = (diagcomm_enum_port_type)j;
	  diagcomm_io_cmd_conn[i][j].connected = FALSE;
      diagcomm_io_cmd_conn[i][j].channel_connected = FALSE;
      diagcomm_io_cmd_conn[i][j].feature_mask_sent = FALSE;
      diagcomm_io_cmd_conn[i][j].data_pending = 0;
      diagcomm_io_cmd_conn[i][j].io_type = DIAGCOMM_IO_TYPE_INVALID;
	  diagcomm_io_cmd_conn[i][j].channel_type = DIAGCOMM_IO_CMD;
      diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][j].port_type = DIAGCOMM_PORT_SMD;
      diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][j].port_type = DIAGCOMM_PORT_SIO;
      diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][j].allow_flow = TRUE;
#if defined(DIAG_MP_MASTER)
#ifdef DIAG_CONSUMER_API

      if (i == DIAGCOMM_PORT_SMD ) 
      {

         diagcomm_io_dci_conn[j].port_num = (diagcomm_enum_port_type)j;
         diagcomm_io_dci_conn[j].data_pending = 0;
         diagcomm_io_dci_conn[j].io_type = DIAGCOMM_IO_TYPE_SIO;
         diagcomm_io_dci_conn[j].channel_type = DIAGCOMM_IO_DCI;
         diagcomm_io_dci_conn[j].port_type = DIAGCOMM_PORT_SMD;
         diagcomm_io_dci_conn[j].connected = FALSE;
         diagcomm_io_dci_conn[j].channel_connected = FALSE;
         diagcomm_io_dci_conn[j].feature_mask_sent = FALSE;
         diagcomm_io_dci_conn[j].allow_flow = TRUE;
		 
         diagcomm_io_dci_cmd_conn[j].port_num = (diagcomm_enum_port_type)j;
         diagcomm_io_dci_cmd_conn[j].data_pending = 0;
         diagcomm_io_dci_cmd_conn[j].io_type = DIAGCOMM_IO_TYPE_SIO;
         diagcomm_io_dci_cmd_conn[j].channel_type = DIAGCOMM_IO_DCI;
         diagcomm_io_dci_cmd_conn[j].port_type = DIAGCOMM_PORT_SMD;
         diagcomm_io_dci_cmd_conn[j].connected = FALSE;
         diagcomm_io_dci_cmd_conn[j].channel_connected = FALSE;
         diagcomm_io_dci_cmd_conn[j].feature_mask_sent = FALSE;
         diagcomm_io_dci_cmd_conn[j].allow_flow = TRUE;
      }
#endif /* #ifdef DIAG_CONSUMER_API */
#endif /* DIAG_MP_MASTER */
    }
  }
 
  
 #if defined(DIAG_MP_MASTER)&& defined (DIAG_MP)
  //Use SMD/SIO on Data & Ctrl path from Apps to Modem.
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].io_type = DIAGCOMM_IO_TYPE_SIO;
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].handle.sio_handle = SIO_NO_STREAM_ID;
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].open_params.sio_params.params.port_id = SIO_PORT_NULL;
  
  #ifdef DIAG_CONSUMER_API
    diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_FUSION_DCI].io_type = DIAGCOMM_IO_TYPE_SIO;
    diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_FUSION_DCI].handle.sio_handle = SIO_NO_STREAM_ID;
    diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_FUSION_DCI].open_params.sio_params.params.port_id = SIO_PORT_NULL;
  #endif /* #ifdef DIAG_CONSUMER_API */
  //Use SMD/SIO on Data & Ctrl path from Apps to Modem.
 
 

  
  #if defined(DIAG_SOCKET_SUPPORT)
  #ifdef DIAG_CONSUMER_API
    diagcomm_io_dci_conn[DIAGCOMM_PORT_1].isServer = TRUE;
    diagcomm_io_dci_conn[DIAGCOMM_PORT_1].addr.sa_family = AF_IPC_ROUTER;
    diagcomm_io_dci_conn[DIAGCOMM_PORT_1].addr.address.addrtype = IPCR_ADDR_NAME;
    diagcomm_io_dci_conn[DIAGCOMM_PORT_1].addr.address.addr.port_name.instance= DIAG_SOCKET_INST_ID_DCI_DATA;
    diagcomm_io_dci_conn[DIAGCOMM_PORT_1].addr.address.addr.port_name.service = QSOCKET_DIAG_ID;
    diagcomm_io_dci_conn[DIAGCOMM_PORT_1].send_flags = QMSG_DONTWAIT;

    diagcomm_io_dci_conn[DIAGCOMM_PORT_1].buf = &diag_dci_data_read_buf;
    diagcomm_io_dci_conn[DIAGCOMM_PORT_1].buf_length = sizeof(diag_dci_data_read_buf);
    diagcomm_io_dci_conn[DIAGCOMM_PORT_1].buf_mutex.name = "MUTEX_DIAG_DCI_DATA_BUF";
    ret = osal_init_mutex(&diagcomm_io_dci_conn[DIAGCOMM_PORT_1].buf_mutex);
    ASSERT(OSAL_SUCCESS == ret);
  #endif /* DIAG_CONSUMER_API */

  
  diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].isServer = TRUE;
  diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].addr.sa_family = AF_IPC_ROUTER;
  diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].addr.address.addrtype = IPCR_ADDR_NAME;
  diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].addr.address.addr.port_name.instance= DIAG_SOCKET_INST_ID_DATA;
  diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].addr.address.addr.port_name.service = QSOCKET_DIAG_ID;
  diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].send_flags = QMSG_DONTWAIT;
  
  diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].buf = &diag_data_read_buf;
  diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].buf_length = sizeof(diag_data_read_buf);
  diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].buf_mutex.name = "MUTEX_DIAG_DATA_BUF";
  ret = osal_init_mutex(&diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].buf_mutex);
  ASSERT(OSAL_SUCCESS == ret);

//CTRL Channel
  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].isServer = TRUE;
  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].addr.sa_family = AF_IPC_ROUTER;
  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].addr.address.addrtype = IPCR_ADDR_NAME;
  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].addr.address.addr.port_name.instance= DIAG_SOCKET_INST_ID_CTRL;
  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].addr.address.addr.port_name.service = QSOCKET_DIAG_ID;
  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].send_flags = QMSG_DONTWAIT;

  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].buf = &diag_ctrl_read_buf;
  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].buf_length = sizeof(diag_ctrl_read_buf);
  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].buf_mutex.name = "MUTEX_DIAG_CTRL_BUF";
  ret = osal_init_mutex(&diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].buf_mutex);
  ASSERT(OSAL_SUCCESS == ret);

#endif /* DIAG_SOCKET_SUPPORT */
 
  //Always use SMDL on cmd/rsp path

  #if defined(DIAG_SOCKET_SUPPORT)
  //Settings to be used when using sockets
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].isServer = FALSE;
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].addr.sa_family = AF_IPC_ROUTER;
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].addr.address.addrtype = IPCR_ADDR_NAME;
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].addr.address.addr.port_name.instance= DIAG_SOCKET_INST_ID_CMD;
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].addr.address.addr.port_name.service = QSOCKET_DIAG_ID;
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].send_flags = QMSG_DONTWAIT;
#endif

  diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].buf = &diag_cmd_read_buf;
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].buf_length = sizeof(diag_cmd_read_buf);
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].buf_mutex.name = "MUTEX_DIAG_CMD_BUF";
  ret = osal_init_mutex(&diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].buf_mutex);
  ASSERT(OSAL_SUCCESS == ret);

  #if defined(DIAG_SOCKET_SUPPORT)
  #ifdef DIAG_CONSUMER_API
    //Settings to be used when using sockets
    diagcomm_io_dci_cmd_conn[DIAGCOMM_PORT_1].isServer = FALSE;
    diagcomm_io_dci_cmd_conn[DIAGCOMM_PORT_1].addr.sa_family = AF_IPC_ROUTER;
    diagcomm_io_dci_cmd_conn[DIAGCOMM_PORT_1].addr.address.addrtype = IPCR_ADDR_NAME;
    diagcomm_io_dci_cmd_conn[DIAGCOMM_PORT_1].addr.address.addr.port_name.instance= DIAG_SOCKET_INST_ID_DCI_CMD;
    diagcomm_io_dci_cmd_conn[DIAGCOMM_PORT_1].addr.address.addr.port_name.service = QSOCKET_DIAG_ID;
    diagcomm_io_dci_cmd_conn[DIAGCOMM_PORT_1].send_flags = QMSG_DONTWAIT;
  #endif /* DIAG_CONSUMER_API */
  #endif /* DIAG_SOCKET_SUPPORT */

  #ifdef DIAG_CONSUMER_API
    diagcomm_io_dci_cmd_conn[DIAGCOMM_PORT_1].buf = &diag_dci_data_read_buf;
    diagcomm_io_dci_cmd_conn[DIAGCOMM_PORT_1].buf_length = sizeof(diag_dci_data_read_buf);
    diagcomm_io_dci_cmd_conn[DIAGCOMM_PORT_1].buf_mutex.name = "MUTEX_DIAG_DCI_DATA_BUF";
    ret = osal_init_mutex(&diagcomm_io_dci_cmd_conn[DIAGCOMM_PORT_1].buf_mutex);
    ASSERT(OSAL_SUCCESS == ret);
  #endif /* DIAG_CONSUMER_API */
#else
  //Always use SIO to USB/UART
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].io_type = DIAGCOMM_IO_TYPE_SIO;
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].handle.sio_handle = SIO_NO_STREAM_ID;
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].open_params.sio_params.params.port_id = SIO_PORT_NULL;
#endif
} /* diagcomm_io_init */
  
/*===========================================================================
  
FUNCTION DIAGCOMM_UPDATE_IO_TYPE
  
DESCRIPTION
  Changes the connection type to the one specified in change_to_type.

PARAMETERS
  None
  
RETURN VALUE
  None

===========================================================================*/
void diagcomm_update_io_type(void)
{
   diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].io_type = diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].change_to_type;
   diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].io_type = diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].change_to_type;
   diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].io_type = diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].change_to_type;
}/* diagcomm_update_io_type */

