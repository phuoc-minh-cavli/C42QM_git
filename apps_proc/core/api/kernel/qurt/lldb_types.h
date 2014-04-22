#ifndef LLDB_TYPES_H
#define LLDB_TYPES_H
#endif

/*======================================================================
                        lldb_types.h 

GENERAL DESCRIPTION
  LLDB data types and structures.

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2018  by Qualcomm Technologies, Inc.  All Rights Reserved.
 ======================================================================*/
/*======================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 03/12/2018   AJ      Initial Version 
 ======================================================================*/

#include "lldb_port.h"


typedef void* lldb_rspcom_handle;

/** Packet type received from RSP */
typedef enum {
  /* Command packet, which start with '$'  */
  LLDB_RX_PKT_RESPONSE     = 0,
  /* Notification packet, which start with '%'  */
  LLDB_RX_PKT_NOTIFICATION = 1,
  
  /* TODO: Do we need to consider binary data */
  
} lldb_rx_pkt_type;


/** Packet type sent to RSP */
typedef enum {
  /* Response packet, which start with '$'  */
  LLDB_TX_PKT_RESPONSE     = 0,
  /* Notification packet, which start with '%'  */
  LLDB_TX_PKT_NOTIFICATION = 1,
  
  /* TODO: Do we need to consider binary data */
  
} lldb_tx_pkt_type;


/** LLDB server status/return codes */
typedef enum {
  /* Success */
  LLDB_EOK	= 0,
  /* Invalid parameter is passed to LLDB functions */
  LLDB_INVALID_PARAM = -1,
  /*Some error */
  LLDB_FAILED = 1
  
} lldb_err_type;


/** LLDB server connection state */
typedef enum {
  /* Debugger client is disconnected from rsp */
  LLDB_CONN_STATE_DISCONNECTED = 0,
  
  /* Debugger client is connected to rsp */
  LLDB_CONN_STATE_CONNECTED    = 1,
} lldb_conn_state_type;

/* LLDB states */
typedef enum {
	TX_LLDB_NOT_STARTED = 0,
	TX_LLDB_INIT    ,
	TX_LLDB_DEBUG   ,
	TX_LLDB_RUNNING 
} lldb_debug_state_t;

/* MODULE states */
typedef enum {
	TX_LLDB_MODULE_NOT_STARTED = 0,
	TX_LLDB_MODULE_STARTED_BREAK    ,
	TX_LLDB_MODULE_DEBUG   ,
	TX_LLDB_MODULE_RUNNING 
} lldb_module_debug_state_t;


/* packet received from RSP */
typedef struct{
	lldb_rx_pkt_type  rx_pkt_type;  
	size_t			  rx_pkt_size;
	char 			  rx_pkt_buf[RX_BUFF_MAX_SIZE];
	
}lldb_cmd_packet_t;

typedef struct{
	lldb_tx_pkt_type  tx_pkt_type;  
	char 			  tx_pkt_buf[TX_BUFF_MAX_SIZE];
	size_t			  tx_pkt_size;
}lldb_resp_packet_t;

/* 
	Signal on which LLDB server will be waiting forever 
	Callback from RSP (when there's a data to be sent to LLDB serer) will set this signal.
*/
typedef struct {
	lldb_signal_t rsp_recv_sig;
	unsigned int  bkpt_set;
	unsigned int  bkpt_cont;
} lldb_sig_t ;

/* 
	LLDB session created for each DAM module
	Each module to be debugged will have its unique session config struct.
*/
typedef struct{
	lldb_conn_state_type lldb_conn_state;
	lldb_cmd_packet_t 	rsp_packet;
	lldb_resp_packet_t	lldb_resp_pkt;
	uint32 				asid;
	LLDB_MODULE_INSTANCE *module_instance;
	lldb_rspcom_handle	rspcom_handle;
	lldb_mutex_t		rx_mutex;
	lldb_mutex_t		tx_mutex;
}lldb_session_config_t;

struct _LLDB_Scratch_Area {
    TX_THREAD * lldb_bp_thread_id;
    struct QURT_local_regs lldb_bp_thread_regs;
	uint32 svc_stack_ptr;
};

struct registers
{
    uint32_t gpr[NUM_GPR_REGS];
};


/*=============================================================================
  FUNCTION  lldb_server_open_rspcom
=============================================================================*/
/**
 * Called at module loading time to open rspcom session for debug session(if any).
 *
 * @param[in]  module_instance pointer   
 */
/*===========================================================================*/

lldb_err_type lldb_server_open_rspcom(LLDB_MODULE_INSTANCE *);

/*=============================================================================
  FUNCTION  lldb_server_close_rspcom
=============================================================================*/
/**
 * close rspcom session for any further debugging.
 *
 * @param[in]  lldb_rspcom_handle       The handle used to interact with RSPCOM.
 */
/*===========================================================================*/

lldb_err_type lldb_server_close_rspcom(lldb_rspcom_handle *);



