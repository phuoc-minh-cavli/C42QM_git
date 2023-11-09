/*****************************************************************************
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "qca4004.h"
#include "qca4004_utils.h"
#include "qca4004_api.h"
#include "qca4004_internal.h"
#include "qca4004_uart.h"
#include "qca4004_gpio.h"
#include "qca4004_fota.h"
/*-------------------------------------------------------------------------
 * Macros
 *-----------------------------------------------------------------------*/
#define QCA4004_STACK_SIZE 4096


/*-------------------------------------------------------------------------
 * Static & global Variable Declarations
 *-----------------------------------------------------------------------*/
qca4004_Callback_Table_t *gQca4004CallbackTable = NULL;
qurt_timer_t gTimer = (qurt_timer_t)0;
	
static uint32_t power_on_timeout = 5000;
static uint32_t power_off_timeout = 20;

extern uint32_t	gRequestId;
extern QCA4004_Power_State_t	gPowerState;
extern QCA4004_Scan_State_t	gScanState;
extern QCA4004_UART_Ctx_t 	QCA4004_UART_Ctx_D;

/*-------------------------------------------------------------------------
 * Local Function Definitions
 *-----------------------------------------------------------------------*/

/* Timer callback functions for power state change */
void timer_power_on_cb(void *cbdata)
{
	QCA4004_DEBUG_LOG("timer power on , ");

    if(gPowerState != QCA4004_POWER_ON)
    {
    	gPowerState = QCA4004_POWER_OFF;
		qca4004_set_gpio_all(0);
    	if(gQca4004CallbackTable && gQca4004CallbackTable->powerStateCb)
    	{
    		gQca4004CallbackTable->powerStateCb(
						gRequestId, 
						QCA4004_ERROR_TIMEOUT, 
						gPowerState, 
						(uint8_t *)0);
    	}	
    }
	stop_qca4004_timer(&(gTimer));
}

/* Timer callback functions for power state change */
void timer_power_off_cb(void *cbdata)
{
	QCA4004_DEBUG_LOG("timer power off , ");

	gPowerState = QCA4004_POWER_OFF;
    if(gQca4004CallbackTable && gQca4004CallbackTable->powerStateCb)
    {
    	gQca4004CallbackTable->powerStateCb(
						gRequestId, 
						QCA4004_ERROR_SUCCESS, 
						gPowerState, 
						(uint8_t *)0);
    }
	
	stop_qca4004_timer(&(gTimer));
}

/* Timer callback functions for scan */
void timer_scan_cb(void *cbdata)
{
	QCA4004_DEBUG_LOG("timer_scan_cb , ");
	
    if(gScanState == QCA4004_SCAN_PENDING)
    {
    	if(gQca4004CallbackTable && gQca4004CallbackTable->scanCb)
    	{
    		gQca4004CallbackTable->scanCb(
						gRequestId,
						QCA4004_ERROR_TIMEOUT,
						(uint8_t *)0,
						0,
						NULL);
    	}
		gScanState = QCA4004_SCAN_IDLE;
    }
	stop_qca4004_timer(&(gTimer));
}


/*-------------------------------------------------------------------------
 * Public Function Definitions
 *-----------------------------------------------------------------------*/

uint8_t start_power_change_timer(uint8_t value)
{
	if(gTimer)
		return 1;
	if(value == 0)
	{
		gTimer = start_qca4004_timer(power_off_timeout, &timer_power_off_cb, (void *)&value);
	}
	else
	{
		gTimer = start_qca4004_timer(power_on_timeout, &timer_power_on_cb, (void *)&value);
	}
	if(gTimer)
		return 0;
	else
		return 2;
}

uint8_t start_scan_timer(uint32_t timeout)
{
	if(gTimer)
		return 1;
	gTimer = start_qca4004_timer(timeout, &timer_scan_cb,(void *)1);
	if(gTimer)
		return 0;
	else
		return 2;
}

#if 0
void set_callback_table(qca4004_Callback_Table_t *pCallbackTable)
{
	gQca4004CallbackTable = pCallbackTable;
}
#endif

static int scan_after_poweron = 0;
uint32_t fw_version = 0;
uint32_t power_on_cnt = 0;
extern QCA4004_FOTA_Ctx_t *qca4004_fota_ctxt_p;
void process_mac(uint8_t* rsp, uint16_t rsp_len)
{
	uint8_t mac[QCA4004_MAC_ADDR_LEN];
	
	if(rsp_len == (QCA4004_MAC_ADDR_LEN+sizeof(uint32_t)))
	{
		memcpy(&fw_version, rsp+QCA4004_MAC_ADDR_LEN, sizeof(uint32_t));
	}
	
	if(qca4004_fota_ctxt_p != NULL && qca4004_fota_ctxt_p->fota_state == QCA4004_FOTA_STATE_H2T_COMPLETE 
		&& (qca4004_fota_ctxt_p->flag &QCA4004_FOTA_AUTO_RESET))
	{
		qurt_signal_set(&(qca4004_fota_ctxt_p->signalc), 0x01);
	}
	
	if(rsp_len == QCA4004_MAC_ADDR_LEN || rsp_len == (QCA4004_MAC_ADDR_LEN+sizeof(uint32_t)))
	{
		power_on_cnt++;
		if(qca4004_send_scan_req(0xffffffff) == 0)
		{
			scan_after_poweron = 1;
		}
	}
	
	return ;
}

/*
 * format of scan result is:
 * -----------------------------------------
 * | Request ID		| MAC		| NumAP		| AP list		|
 * -----------------------------------------
 * | 8bytes (arg=2)		| 6bytes		| 1byte		| 			|
 * -----------------------------------------
*/
void process_aps(uint8_t* rsp, uint16_t rsp_len)
{
	uint8_t  numAps;
	QCA4004_Bss_Scan_Info *info;
	uint8_t mac[QCA4004_MAC_ADDR_LEN];
	uint32_t currentRequestId = 0;
	uint8_t i;
	uint8_t rssi;
	
	
	#if 0
	if(QCA4004_Ctx_D.scanState != QCA4004_SCAN_IDLE)
		return ;
	#endif

	currentRequestId = qca4004_atoux((char *)rsp);
	rsp += 8;
	rsp_len -= 8;
	
	memcpy(&(mac[0]), rsp, QCA4004_MAC_ADDR_LEN);
	rsp += QCA4004_MAC_ADDR_LEN;
	rsp_len -= QCA4004_MAC_ADDR_LEN;
	
	numAps = rsp[0];
	rsp++;
	rsp_len--;
	
	QCA4004_DEBUG_LOG("process_aps rsp_len=%d,numAps=%d",rsp_len,numAps);

	if(rsp_len == numAps * sizeof(QCA4004_Bss_Scan_Info))
	{
		if(scan_after_poweron == 1)
		{
			stop_qca4004_timer(&(gTimer));
			gPowerState = QCA4004_POWER_ON;
			scan_after_poweron = 0;
			if(gQca4004CallbackTable && gQca4004CallbackTable->powerStateCb)
			{
				gQca4004CallbackTable->powerStateCb(
					gRequestId, QCA4004_ERROR_SUCCESS, gPowerState, &(mac[0]));
			}
			return;
		}
		
		stop_qca4004_timer(&(gTimer));
		
		info = (QCA4004_Bss_Scan_Info *)rsp;
		for(i=0;i<numAps;i++)
		{
			rssi = (uint8_t)(info[i].rssi); 
			info[i].rssi = (int8_t)(rssi - 96);
		}

		if(gQca4004CallbackTable && gQca4004CallbackTable->scanCb)
		{
			gQca4004CallbackTable->scanCb(
						currentRequestId,
						QCA4004_ERROR_SUCCESS,
						&(mac[0]),
						numAps,
						info);
		}
		gScanState = QCA4004_SCAN_IDLE;	
	}
	
	return;
}

/**
   @brief This function is the main entry point for the daemon of qca4004 driver .
*/
qurt_signal_t 	qca4004_daemon_event;
uint8_t qca4004_daemon_status = 0;
uint8_t qca4004_daemon_start(void)
{
	qurt_thread_attr_t Thread_Attribte;
    qurt_thread_t      Thread_Handle;
		
	/*Sleep for 20 ms*/
	qurt_thread_sleep_ext(2*QURT_TIMER_NEXT_EXPIRY);

	/* Start the daemon thread. */
	qurt_signal_init(&qca4004_daemon_event);
	qca4004_daemon_status = 1;
    qurt_thread_attr_init(&Thread_Attribte);
    qurt_thread_attr_set_name(&Thread_Attribte, "QCA4004 Thread");
    qurt_thread_attr_set_priority(&Thread_Attribte, 152);
    qurt_thread_attr_set_stack_size(&Thread_Attribte, QCA4004_STACK_SIZE);
    if(QURT_EOK != qurt_thread_create(&Thread_Handle, &Thread_Attribte, qca4004_thread, NULL))
    {
		qca4004_daemon_status = 0;
		qurt_signal_destroy(&qca4004_daemon_event);
    	return 1;//TO DO
    }
	
	return 0;
}

/**
   @brief This function will destroy the daemon of qca4004 driver .
*/
void qca4004_daemon_destroy(void)
{
	/* Change the driver status and signal the event to qca4004_thread	*/ 
	qca4004_daemon_status = 0;
    qurt_signal_set(&(QCA4004_UART_Ctx_D.event), QCA4004_DAEMON_EVENT_MASK); 
	
	qurt_signal_wait(&qca4004_daemon_event, 0x1, QURT_SIGNAL_ATTR_WAIT_ANY | QURT_SIGNAL_ATTR_CLEAR_MASK); 
	qurt_signal_destroy(&qca4004_daemon_event);
	
	return;
}

uint32_t qca4004_send_scan_req(uint32_t requestId)
{
	uint32_t status = 0;
	char cmd_str[20];
	uint32_t cmd_len = 0;
	char *cmd_ptr = &cmd_str[0];

	memset(&cmd_str[0], 0 , 20);

	memcpy(cmd_ptr, COMMAND_SCAN_STRING, strlen(COMMAND_SCAN_STRING));
	cmd_ptr += strlen(COMMAND_SCAN_STRING);

	*cmd_ptr++ = ',';
		
	qca4004_uxtoa(requestId, cmd_ptr);
	cmd_ptr += 8;
		
	*cmd_ptr++ = ',';

	cmd_len = cmd_ptr - &cmd_str[0];
	QCA4004_DEBUG_LOG("cmd len=%d, ",cmd_len);
		
	status = (uint32_t)qca4004_send_command(cmd_len, &cmd_str[0]); 

	return status;
}
