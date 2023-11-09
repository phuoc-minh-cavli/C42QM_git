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
#include "qca4004_utils.h"
#include "qca4004_fota.h"

extern qca4004_Callback_Table_t *gQca4004CallbackTable;
extern qurt_timer_t gTimer;
extern qca4004_Callback_Table_t ota_cb;
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

	return QCA4004_OK;
}

static QCA4004_Status_t qca4004_hal_deinit(void)
{
	if(qca4004_uart_close() != 0)
		return QCA4004_ERR_UART;
		
	if(qca4004_release_gpio_all() != 0)
		return QCA4004_ERR_GPIO;

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
		
	qurt_mutex_init(&(gMutex));
	if(QCA4004_LOCK(gMutex))
	{
		status = qca4004_hal_init();
		if(status != QCA4004_OK)
			goto err;

		if(qca4004_daemon_start() != 0)
		{
			status = QCA4004_ERR_THREAD;
			goto err;
		}

		//set_callback_table(pCallbackTable);
		gQca4004CallbackTable = pCallbackTable;

		qca4004_initilized = 1;
err:
		QCA4004_UNLOCK(gMutex);
	}
	else
	{
		status = QCA4004_ERR_LOCK;
	}

	return status;
}

QCA4004_Status_t qca4004_deinit(void)
{
	QCA4004_Status_t status = QCA4004_OK;

	if(gQca4004CallbackTable == &ota_cb)
		return QCA4004_ERR_BUSY;
		
	if(qca4004_initilized == 0)
		return status;
		
	if(QCA4004_LOCK(gMutex))
	{
		qca4004_daemon_destroy();
		
		status = qca4004_hal_deinit();
		if(status != QCA4004_OK)
			goto err;

		gQca4004CallbackTable = NULL;
	
		qca4004_initilized = 0;
		gRequestId = 0;
		gPowerState = QCA4004_POWER_OFF;
		gScanState = QCA4004_SCAN_IDLE;
		gQca4004CallbackTable = NULL;
		if(gTimer)
			stop_qca4004_timer(&(gTimer));
err:
		QCA4004_UNLOCK(gMutex);
	}
	else
	{
		status = QCA4004_ERR_LOCK;
	}
	
	qurt_mutex_destroy(&(gMutex));
	return status;
}

QCA4004_Status_t qca4004_power_state_change(
				uint32_t requestId, const QCA4004_Power_State_t state)
{
	QCA4004_Status_t status = QCA4004_OK;
	uint8_t value = 0;

	if(state == QCA4004_POWER_OFF && gQca4004CallbackTable == &ota_cb)
		return QCA4004_ERR_BUSY;
		
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

	if(gQca4004CallbackTable == &ota_cb)
		return QCA4004_ERR_BUSY;
		
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

extern uint32_t fw_version;
extern uint32_t power_on_cnt;
QCA4004_Status_t qca4004_get_fw_version(uint32_t *ver)
{
	if(ver == NULL)
		return QCA4004_ERR_INVALID_PARAM;
		
	if(power_on_cnt == 0)
		return QCA4004_ERR_GENERAL;
	
	*ver = fw_version;
	return QCA4004_OK;
}

QCA4004_Status_t qca4004_fota_start(char *filePath, int32_t flag, uint32_t *version)
{
	QCA4004_Status_t ret = QCA4004_OK;
	uint8_t init = qca4004_initilized;
	QCA4004_Power_State_t power_state = gPowerState;
	qca4004_Callback_Table_t *cb = gQca4004CallbackTable;
	int i = 0;
	
	if(gPowerState == QCA4004_POWER_SUSPEND || gScanState == QCA4004_SCAN_PENDING)
		return QCA4004_ERR_BUSY;
		
	if(init == 0) {
		ret = qca4004_init(&ota_cb);
		if(ret != QCA4004_OK)
			return ret;
	}
	else
		gQca4004CallbackTable = &ota_cb;
		
	
	if(power_state == QCA4004_POWER_OFF) {
		qca4004_power_state_change(0, QCA4004_POWER_ON);
		for(i = 0;i < 500;i++) 
		{
			qurt_thread_sleep_ext(QURT_TIMER_NEXT_EXPIRY);
			if(gPowerState == QCA4004_POWER_ON) 
				break;
		}
		if(i >= 500) {
			ret = QCA4004_ERR_GENERAL;
			goto error;
		}
	}
		
	if(fw_version == 0) {
		ret = QCA4004_ERR_GENERAL;
		goto error;
	}
	
	ret = qca4004_fota_start_internal(filePath, flag, version);
	
	
error:
	gQca4004CallbackTable = cb;
	if(power_state == QCA4004_POWER_OFF)
		qca4004_power_state_change(0, QCA4004_POWER_OFF);
	qurt_thread_sleep_ext(2*QURT_TIMER_NEXT_EXPIRY);
	
	if(init == 0)
		qca4004_deinit();
	return ret;
}