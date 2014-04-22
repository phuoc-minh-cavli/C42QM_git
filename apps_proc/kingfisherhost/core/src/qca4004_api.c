/*****************************************************************************
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "qca4004_api.h"
#include "qca4004.h"
#include "qca4004_internal.h"
#include "qca4004_gpio.h"
#include "qca4004_uart.h"

extern qca4004_Callback_Table_t *gQca4004CallbackTable;
/*-------------------------------------------------------------------------
 * Static & global Variable Declarations
 *-----------------------------------------------------------------------*/
uint8_t qca4004_initilized = 0;

uint32_t	gRequestId = 0;
QCA4004_Power_State_t	gPowerState = QCA4004_POWER_OFF;
QCA4004_Scan_State_t	gScanState = QCA4004_SCAN_IDLE;

static qurt_mutex_t	gMutex;


/*-------------------------------------------------------------------------
 * Local Function Definitions
 *-----------------------------------------------------------------------*/

static QCA4004_Status_t qca4004_hal_init(void)
{
	if(qca4004_get_gpio_all() != 0)
		return QCA4004_ERR_GPIO;

	if(qca4004_uart_open() != 0)
		return QCA4004_ERR_UART;

	gRequestId = 0;
	gPowerState = QCA4004_POWER_OFF;
	gScanState = QCA4004_SCAN_IDLE;
	qurt_mutex_init(&(gMutex));

	return QCA4004_OK;
}

static QCA4004_Status_t qca4004_hal_deinit(void)
{
	if(qca4004_release_gpio_all() != 0)
		return QCA4004_ERR_GPIO;

	if(qca4004_uart_close() != 0)
		return QCA4004_ERR_UART;
	
	qurt_mutex_destroy(&(gMutex));
	
	return QCA4004_OK;
}


/*-------------------------------------------------------------------------
 * Public Function Definitions
 *-----------------------------------------------------------------------*/

QCA4004_Status_t qca4004_init(qca4004_Callback_Table_t *pCallbackTable)
{
	QCA4004_Status_t status = QCA4004_OK;

	if(qca4004_initilized)
		return status;
	
	status = qca4004_hal_init();
	if(status != QCA4004_OK)
		return status;
	
	if(qca4004_daemon_start() != 0)
		return QCA4004_ERR_THREAD;

	//set_callback_table(pCallbackTable);
	gQca4004CallbackTable = pCallbackTable;

	qca4004_initilized = 1;
	return status;
}

QCA4004_Status_t qca4004_deinit(void)
{
	QCA4004_Status_t status = QCA4004_OK;

	if(qca4004_initilized == 0)
		return status;
	
	status = qca4004_hal_deinit();
	if(status != QCA4004_OK)
		return status;

	gQca4004CallbackTable = NULL;
	
	qca4004_initilized = 0;
	return status;
}

QCA4004_Status_t qca4004_power_state_change(
				uint32_t requestId, const QCA4004_Power_State_t state)
{
	QCA4004_Status_t status = QCA4004_OK;
	uint8_t value = 0;

	QCA4004_DEBUG_LOG("qca4004_power_state_change , ");
	if(qca4004_initilized != 1)
	{
		return QCA4004_ERR_NOT_INIT;
	}
	
	if(state == gPowerState)
	{
		return status;
	}

	if(state != QCA4004_POWER_ON && state != QCA4004_POWER_OFF)
	{
		return QCA4004_ERR_INVALID_PARAM;
	}

	if(gPowerState == QCA4004_POWER_SUSPEND)
	{
		return QCA4004_ERR_BUSY;
	}
	
	if(QCA4004_LOCK(gMutex))
	{
		value = (state == QCA4004_POWER_ON?1:0);
		if(qca4004_set_gpio_all(value) == 0)
		{
			if(start_power_change_timer(value) == 0)
			{
				gRequestId = requestId;
				gPowerState = QCA4004_POWER_SUSPEND;
			}
			else
			{
				qca4004_set_gpio_all(1-value);
				status = QCA4004_ERR_TIMER;
			}
		}
		else
		{
			status = QCA4004_ERR_GPIO;
		}
		QCA4004_UNLOCK(gMutex);
	}
	else
	{
		status = QCA4004_ERR_LOCK;
	}

	return status;

}

QCA4004_Status_t qca4004_get_AP_list(uint32_t requestId, uint32_t timeout)
{
	QCA4004_Status_t status = QCA4004_OK; 
	//char cmd[] = "SCAN,";
	char cmd_str[20];
	uint32_t cmd_len = 0;
	char *cmd_ptr = &cmd_str[0];

	QCA4004_DEBUG_LOG("AP List, ");
	if(qca4004_initilized != 1)
	{
		return QCA4004_ERR_NOT_INIT;
	}
	
	if(gPowerState != QCA4004_POWER_ON)
	{
		return QCA4004_ERR_POWER_STATE;
	}

	if(gScanState == QCA4004_SCAN_PENDING)
	{
		return QCA4004_ERR_BUSY;
	}

	if(QCA4004_LOCK(gMutex))
	{
		memset(&cmd_str[0], 0 , 20);

		memcpy(cmd_ptr, COMMAND_SCAN_STRING, strlen(COMMAND_SCAN_STRING));
		cmd_ptr += strlen(COMMAND_SCAN_STRING);

		*cmd_ptr++ = ',';
		
		qca4004_uxtoa(requestId, cmd_ptr);
		cmd_ptr += 8;
		
		*cmd_ptr++ = ',';

		cmd_len = cmd_ptr - &cmd_str[0];
		QCA4004_DEBUG_LOG("cmd len=%d, ",cmd_len);
		
		status = qca4004_send_command(cmd_len, &cmd_str[0]); 
		//status = qca4004_send_command(strlen(cmd), &cmd[0]); 
		QCA4004_DEBUG_LOG("qca4004_write status=%d, ",status);
		while(cmd_len--)
		{
			QCA4004_DEBUG_LOG("cmd=%c, ",cmd_str[cmd_len]);
		}
		
		if(status == QCA4004_OK)
		{
			if(start_scan_timer(timeout) == 0)
			{
				gScanState = QCA4004_SCAN_PENDING;
			}
			else
			{
				status = QCA4004_ERR_TIMER;
			}
		}
		QCA4004_UNLOCK(gMutex);
	}
	else
	{
		status = QCA4004_ERR_LOCK;
	}

	return status;

}


