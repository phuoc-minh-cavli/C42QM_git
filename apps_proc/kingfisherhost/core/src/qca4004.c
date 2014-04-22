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

/*-------------------------------------------------------------------------
 * Macros
 *-----------------------------------------------------------------------*/
#define QCA4004_STACK_SIZE 4096


/*-------------------------------------------------------------------------
 * Static & global Variable Declarations
 *-----------------------------------------------------------------------*/
qca4004_Callback_Table_t *gQca4004CallbackTable = NULL;
static qurt_timer_t gTimer = (qurt_timer_t)0;
	
static uint32_t power_on_timeout = 3000;
static uint32_t power_off_timeout = 20;

extern uint32_t	gRequestId;
extern QCA4004_Power_State_t	gPowerState;
extern QCA4004_Scan_State_t	gScanState;


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

void process_mac(uint8_t* rsp, uint16_t rsp_len)
{
	uint8_t mac[QCA4004_MAC_ADDR_LEN];
	
	if(rsp_len == QCA4004_MAC_ADDR_LEN)
	{
		stop_qca4004_timer(&(gTimer));
		gPowerState = QCA4004_POWER_ON;
		
		memcpy(&(mac[0]), rsp, QCA4004_MAC_ADDR_LEN);
		
		if(gQca4004CallbackTable && gQca4004CallbackTable->powerStateCb)
    	{
			gQca4004CallbackTable->powerStateCb(
				gRequestId, QCA4004_ERROR_SUCCESS, gPowerState, &(mac[0]));
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
uint8_t qca4004_daemon_start(void)
{
	qurt_thread_attr_t Thread_Attribte;
    qurt_thread_t      Thread_Handle;
		
	/*Sleep for 20 ms*/
	qurt_thread_sleep_ext(2*QURT_TIMER_NEXT_EXPIRY);

	/* Start the daemon thread. */
    qurt_thread_attr_init(&Thread_Attribte);
    qurt_thread_attr_set_name(&Thread_Attribte, "QCA4004 Thread");
    qurt_thread_attr_set_priority(&Thread_Attribte, 152);
    qurt_thread_attr_set_stack_size(&Thread_Attribte, QCA4004_STACK_SIZE);
    if(QURT_EOK != qurt_thread_create(&Thread_Handle, &Thread_Attribte, qca4004_thread, NULL))
    {
    	return 1;//TO DO
    }
	
	return 0;
}

