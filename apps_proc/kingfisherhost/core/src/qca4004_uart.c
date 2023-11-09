/*****************************************************************************
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "qca4004_uart.h"
#include "qca4004_internal.h"
#include "qca4004_iface.h"

#include "qapi_status.h"


extern uint8_t qca4004_initilized;
extern qurt_signal_t 	qca4004_daemon_event;
extern uint8_t qca4004_daemon_status;
/*-------------------------------------------------------------------------
 * Static & global Variable Declarations
 *-----------------------------------------------------------------------*/
static qapi_UART_Port_Id_e g_port_id = QAPI_UART_PORT_002_E;

QCA4004_UART_Ctx_t 	QCA4004_UART_Ctx_D;
static char send_buf[QCA4004_TX_BUFFER_SIZE];


/*-------------------------------------------------------------------------
 * Local Function Definitions
 *-----------------------------------------------------------------------*/

static void qca4004_rx_cb(uint32_t num_bytes, void *cb_data)
{
	int length = num_bytes;  

	QCA4004_DEBUG_LOG("qca4004_rx_cb num_bytes=%d,", num_bytes);
	if(QCA4004_UART_Ctx_D.bufferFree != 0)
	{
		/* See how much data can be read.									 */
		if(length > QCA4004_UART_Ctx_D.bufferFree)
		{
		   length = QCA4004_UART_Ctx_D.bufferFree;
		}
			
		QCA4004_ENTER_CRITICAL();
		QCA4004_UART_Ctx_D.bufferFree -= length;
		QCA4004_EXIT_CRITICAL();
	
		if(length > 0)
		{
		   /* Signal the event that data was received.						 */ 
           qurt_signal_set(&(QCA4004_UART_Ctx_D.event), QCA4004_RECEIVE_EVENT_MASK); 
		} 	
	}

}

static void qca4004_tx_cb(uint32_t num_bytes, void *cb_data)
{
	/* This is a dummy callback as now UART QAPI's has made it mandatory to have a TX callback
	 */
	#if 0
	uint8_t *tmp = (uint8_t *)cb_data;
	QCA4004_DEBUG_LOG("tx num bytes %d, ",num_bytes);
	while(num_bytes--)
	{
		QCA4004_DEBUG_LOG("tx=%x,",*tmp);
		tmp++;
	}
	#endif
}


/*-------------------------------------------------------------------------
 * Public Function Definitions
 *-----------------------------------------------------------------------*/

uint8_t qca4004_uart_open(void)
{
	uint8_t result = 0;
	qapi_UART_Open_Config_t open_properties;
	qapi_Status_t q_status = QAPI_OK;

	memset(&QCA4004_UART_Ctx_D, 0, sizeof(QCA4004_UART_Ctx_D));
	QCA4004_UART_Ctx_D.bufferFree = QCA4004_RX_BUFFER_SIZE;
	/* Create a receive event. */ 
	qurt_signal_init(&(QCA4004_UART_Ctx_D.event));
	
	memset(&open_properties, 0, sizeof (open_properties));
	
	open_properties.parity_Mode = QAPI_UART_NO_PARITY_E;
	open_properties.num_Stop_Bits= QAPI_UART_1_0_STOP_BITS_E;
	open_properties.baud_Rate   = 115200;	
	open_properties.bits_Per_Char= QAPI_UART_8_BITS_PER_CHAR_E;
	
	open_properties.rx_CB_ISR = qca4004_rx_cb;
	open_properties.tx_CB_ISR = qca4004_tx_cb;
	
	open_properties.enable_Flow_Ctrl = false;
	open_properties.enable_Loopback = false;
	open_properties.user_mode_client = false;
	
	q_status = qapi_UART_Open(&QCA4004_UART_Ctx_D.uartHandle, g_port_id, &open_properties);
	QCA4004_DEBUG_LOG("qapi_UART_Open status=%d, ", q_status);
	if(q_status == QAPI_OK)
	{
		q_status = qapi_UART_Receive(QCA4004_UART_Ctx_D.uartHandle, 
						(char *)&(QCA4004_UART_Ctx_D.buffer[0]), 
						QCA4004_RX_BUFFER_SIZE, 
						(void*)&(QCA4004_UART_Ctx_D.buffer[0]));  

		if(q_status != QAPI_OK)
			result = 1;

		QCA4004_DEBUG_LOG("qapi_UART_Receive status=%d, ", q_status);
	}
	else
	{
		result = 1;
	}
	
	return (result);
}

uint8_t qca4004_uart_close(void)
{
	uint8_t result = 0;

	if(QAPI_OK == qapi_UART_Close(QCA4004_UART_Ctx_D.uartHandle))
	{
		/* Destroy a receive event. */ 
		qurt_signal_destroy(&(QCA4004_UART_Ctx_D.event));
		memset(&QCA4004_UART_Ctx_D, 0, sizeof(QCA4004_UART_Ctx_D));
	}
	else
	{
		result = 1;
	}
	return (result);
}

void qca4004_thread(void *Param)
{
	//qapi_Status_t	status;
	uint32_t 	length;
 	
	/* Loop waiting for received data. */
	while(true)
	{
		/* Wait for data to be received. */
		qurt_signal_wait(&(QCA4004_UART_Ctx_D.event), QCA4004_RECEIVE_EVENT_MASK | QCA4004_DAEMON_EVENT_MASK, QURT_SIGNAL_ATTR_WAIT_ANY | QURT_SIGNAL_ATTR_CLEAR_MASK);
		//qurt_signal_clear(&(QCA4004_UART_Ctx_D.event), QCA4004_RECEIVE_EVENT_MASK); 
		
		if(qca4004_daemon_status == 0)
			break;
			
		if((length = QCA4004_RX_BUFFER_SIZE - QCA4004_UART_Ctx_D.bufferFree) == 0)
			continue;
	   	   
		/* Send the next buffer's data to QCLI for processing. */
		QCA4004_Process_Input_Data(length, &(QCA4004_UART_Ctx_D.buffer[0]));
	
		qapi_UART_Receive(QCA4004_UART_Ctx_D.uartHandle, 
	   					(char *)&(QCA4004_UART_Ctx_D.buffer[0]), 
	   					QCA4004_RX_BUFFER_SIZE, 
	   					(void*)1);
	   
		QCA4004_ENTER_CRITICAL();
		QCA4004_UART_Ctx_D.bufferFree += length;
		QCA4004_EXIT_CRITICAL();
	}
	/* notify function qca4004_daemon_destroy that this thread finished */
	qurt_signal_set(&qca4004_daemon_event,0x1);
	qurt_thread_stop();
}

uint8_t qca4004_write(uint32_t length, const uint8_t *buffer)
{
	uint32_t len = QCA4004_TX_BUFFER_SIZE;
	qapi_Status_t q_status = QAPI_OK;
	while((length) && (buffer))
	{
		if(length < QCA4004_TX_BUFFER_SIZE)
			len = length;
		memset(send_buf, 0, sizeof (send_buf));
		memcpy(send_buf, (char*)buffer, len);
		q_status = qapi_UART_Transmit(QCA4004_UART_Ctx_D.uartHandle,send_buf, len, (void*)send_buf); 
		if(q_status != QAPI_OK)
			return 1;

		length -= len;
		buffer += len;
		qurt_thread_sleep_ext(2 * QURT_TIMER_NEXT_EXPIRY);
	}
	return 0;
}

uint8_t qca4004_send_command(uint32_t length, const char *buffer)
{
	/* \nATM=()\n*/
	char *buf = &send_buf[0];
	if(length > QCA4004_TX_BUFFER_SIZE - 6)
	{
		return 1;
	}

	memset(buf, 0, QCA4004_TX_BUFFER_SIZE);
	*buf++ = DELIMITER_COMMAND_CHARACTER;
	
	memcpy(buf, START_OF_COMMAND_STRING, sizeof(START_OF_COMMAND_STRING)-1);
	buf += (sizeof(START_OF_COMMAND_STRING) - 1);

	memcpy(buf, buffer, length);
	buf += length;

	*buf++ = DELIMITER_COMMAND_CHARACTER;

	length += 6;
	if(QAPI_OK != qapi_UART_Transmit(QCA4004_UART_Ctx_D.uartHandle, send_buf, length, (void*)send_buf))
		return 1;

	return 0;
}


uint8_t qca4004_send_at_command(uint32_t length, const char *buffer)
{
	/* \nATM=()\n*/
	char *buf = &send_buf[0];
	if(length > QCA4004_TX_BUFFER_SIZE - 6)
	{
		return 1;
	}

	memset(buf, 0, QCA4004_TX_BUFFER_SIZE);
	*buf++ = DELIMITER_COMMAND_CHARACTER;

	memcpy(buf, buffer, length);
	buf += length;

	*buf++ = DELIMITER_COMMAND_CHARACTER;

	length += 2;
	if(QAPI_OK != qapi_UART_Transmit(QCA4004_UART_Ctx_D.uartHandle, send_buf, length, (void*)send_buf))
		return 1;

	return 0;
}

uint8_t qca4004_send_buf_direct(uint32_t length, char *buffer)
{
	if(QAPI_OK != qapi_UART_Transmit(QCA4004_UART_Ctx_D.uartHandle, buffer, length, (void*)buffer))
		return 1;

	return 0;
}

