/*****************************************************************************
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#include "qca4004_api.h"

#include "qcli.h"
#include "qcli_api.h"

#include "qapi_tlmm.h"
#include "qapi_uart.h"

#include "qurt.h"
//#include <stdio.h>
// Handle for QCA4004 Driver Test Command Group
QCLI_Group_Handle_t qcli_qca4004_driver_handle;
static uint32_t gCurrentRequestId = 0;

// LOGGING MACROS
#define QCA4004_LOG_QCLI(fmt, ...) QCLI_Printf(qcli_qca4004_driver_handle, fmt , ##__VA_ARGS__)
#define QCA4004_RECEIVE_EVENT_MASK                          (0x00000001)

void qca4004_power_state_cb(
	uint32_t requestId,
	QCA4004_Error_Core_t errCode,
    QCA4004_Power_State_t state,
	uint8_t *mac
)
{
	int i = 0;
  qurt_time_t current_time = qurt_timer_get_ticks();
  qurt_time_t curr_time = qurt_timer_convert_ticks_to_time(current_time, QURT_TIME_MSEC);
  QCA4004_LOG_QCLI("end time=%ld\n",curr_time);
	if(errCode == QCA4004_ERROR_SUCCESS)
	{
		QCA4004_LOG_QCLI("qca4004 power state change to %d,requestId=%d\n",state,requestId);
		if(state == QCA4004_POWER_ON)
		{
			QCA4004_LOG_QCLI("mac address is:");
			for(;i < QCA4004_MAC_ADDR_LEN;i++)
			{
				QCA4004_LOG_QCLI("%x:",mac[i]);
			}
		}
	}
	else
	{
		QCA4004_LOG_QCLI("qca4004 power state failed,err code is %d\n",errCode);
	}
}

void qca4004_scan_cb(
	uint32_t requestId,
	QCA4004_Error_Core_t errCode,
	uint8_t *mac,
    uint8_t numAps,
	QCA4004_Bss_Scan_Info* pOut
)
{
	int i = 0;
	int j = 0;
	QCA4004_Bss_Scan_Info* scanResults = NULL;
  qurt_time_t current_time = qurt_timer_get_ticks();
  qurt_time_t curr_time = qurt_timer_convert_ticks_to_time(current_time,QURT_TIME_MSEC);
  QCA4004_LOG_QCLI("end time=%ld\n",curr_time);
	if(errCode == QCA4004_ERROR_SUCCESS)
	{
		scanResults = malloc(numAps * sizeof(QCA4004_Bss_Scan_Info));
		memcpy(scanResults, pOut, numAps * sizeof(QCA4004_Bss_Scan_Info));
		QCLI_Printf(qcli_qca4004_driver_handle,"qca4004 scan success,requestId=%d,num of APs=%d; ",requestId,numAps);
		#if 1
		for(i = 0;i < numAps;i++)
		{
			for(j = 0;j < scanResults[i].ssid_len;j++)
			{
				QCLI_Printf(qcli_qca4004_driver_handle,"%c",scanResults[i].ssid[j]);
			}
			QCLI_Printf(qcli_qca4004_driver_handle,";");
		}
		#else
		QCLI_Printf(qcli_qca4004_driver_handle,"mac address is:");
		for(;i < QCA4004_MAC_ADDR_LEN;i++)
		{
			QCLI_Printf(qcli_qca4004_driver_handle,"%02x:",mac[i]);
		}
		QCLI_Printf(qcli_qca4004_driver_handle,";number of AP list is:%d\n",numAps);
		for(i = 0;i < numAps;i++)
		{
			QCLI_Printf(qcli_qca4004_driver_handle,"channel=%d;",scanResults[i].channel);
			QCLI_Printf(qcli_qca4004_driver_handle,"rssi=%d;",scanResults[i].rssi);
			QCLI_Printf(qcli_qca4004_driver_handle,"bssid is:");
			for(j = 0;j < QCA4004_MAC_ADDR_LEN;j++)
			{
				QCLI_Printf(qcli_qca4004_driver_handle,"%02x:",scanResults[i].bssid[j]);
			}
			QCLI_Printf(qcli_qca4004_driver_handle,";ssid len=%d;ssid=",scanResults[i].ssid_len);
			for(j = 0;j < scanResults[i].ssid_len;j++)
			{
				QCLI_Printf(qcli_qca4004_driver_handle,"%c",scanResults[i].ssid[j]);
			}
			QCLI_Printf(qcli_qca4004_driver_handle,"\n");
			qurt_thread_sleep_ext(2 * QURT_TIMER_NEXT_EXPIRY);
		}
		#endif
		free(scanResults);
	}
	else
	{
		QCA4004_LOG_QCLI("qca4004 scan failed,err code is %d\n",errCode);
	}
}

qca4004_Callback_Table_t pCallbackTable_e = {qca4004_power_state_cb,qca4004_scan_cb};

/*****************************************************************************
 *          
 * qca4004 driver init 
 *****************************************************************************/
QCLI_Command_Status_t driverInit(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  result = (QCLI_Command_Status_t) qca4004_init(&pCallbackTable_e);
  QCA4004_LOG_QCLI("driver init ret=%d\n",result);
  
  return result;
}

/*****************************************************************************
 *          
 * power state change
 *****************************************************************************/
QCLI_Command_Status_t powerStateChange(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  /* default state is QCA4004_POWER_ON if no parameter. */
  QCA4004_Power_State_t state  =  QCA4004_POWER_ON;
  qurt_time_t current_time = qurt_timer_get_ticks();
  qurt_time_t curr_time = qurt_timer_convert_ticks_to_time(current_time,QURT_TIME_MSEC);
  QCA4004_LOG_QCLI("start time=%ld\n",curr_time);
  if(Parameter_Count == 1 && Parameter_List[0].Integer_Is_Valid && Parameter_List[0].Integer_Value == 0)
  {
	state = QCA4004_POWER_OFF;
  }  

  result = (QCLI_Command_Status_t) qca4004_power_state_change(gCurrentRequestId++,state);
  QCA4004_LOG_QCLI("powerStateChange requestId=%d, state=%d, ret=%d\n",gCurrentRequestId, state, result);

  return result;
}

/*****************************************************************************
 *          
 * get AP list
 *****************************************************************************/
QCLI_Command_Status_t getAPList(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  /* default timeout is 5000ms if no parameter. */
  int32_t timeout = 5000;
  qurt_time_t current_time = qurt_timer_get_ticks();
  qurt_time_t curr_time = qurt_timer_convert_ticks_to_time(current_time,QURT_TIME_MSEC);
  QCA4004_LOG_QCLI("start time=%ld\n",curr_time);
  if(Parameter_Count == 1 && Parameter_List[0].Integer_Is_Valid)
  {  
  	timeout = Parameter_List[0].Integer_Value;
  }

  result = (QCLI_Command_Status_t) qca4004_get_AP_list(gCurrentRequestId++,(uint32_t)timeout);
  
  QCA4004_LOG_QCLI("getAPList requestId=%d, timeout=%d, ret=%d\n",gCurrentRequestId, timeout, result);
  return result;
}

/*****************************************************************************
 *          
 * power on/off
 *****************************************************************************/
extern uint8_t qca4004_get_gpio_all(void);
extern uint8_t qca4004_release_gpio_all(void);
extern uint8_t qca4004_set_gpio_all(uint8_t value);
static QCLI_Command_Status_t powerOnOff(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  uint32_t value = 1;
  if(Parameter_Count == 1 && Parameter_List[0].Integer_Is_Valid)
  {
    value = (uint32_t)Parameter_List[0].Integer_Value;
    if(value != 0)
	{
		return result;
	}
  } 
  if(value == 1)
    qca4004_get_gpio_all();
  qca4004_set_gpio_all((uint8_t)value);
  if(value == 0)
    qca4004_release_gpio_all();
  
  return result;
}

/*****************************************************************************
 *          
 * qca4004 driver de-init test
 *****************************************************************************/
QCLI_Command_Status_t driverDeInit(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  result = (QCLI_Command_Status_t) qca4004_deinit();
  QCA4004_LOG_QCLI("driver de-init ret=%d\n",result);
  
  return result;
}

extern uint8_t qca4004_send_at_command(uint32_t length, const char *buffer);
QCLI_Command_Status_t testAtCmd(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  uint32_t length;
  char *atcmd;
  if(Parameter_Count < 1)
	return result;
  atcmd = (char*)Parameter_List[0].String_Value;
  result = (QCLI_Command_Status_t) qca4004_send_at_command(strlen(atcmd),atcmd);

  QCA4004_LOG_QCLI("test at command ret=%d\n",result);
  
  return result;
}
/*****************************************************************************
 *
 * qca4004 fota test
 *****************************************************************************/
//extern int qca4004_fota_start(char *filePath, int32_t flag, uint32_t *version);
QCLI_Command_Status_t Fota_UintTest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
	char *filePath = NULL;
	int32_t flag = 0;
	uint32_t ver = 0;
	uint32_t flash_size = 0;
	
	if(Parameter_Count > 0 && Parameter_List[0].Integer_Is_Valid) {
		flash_size = Parameter_List[0].Integer_Value;
		if(flash_size < 4) {
			QCA4004_LOG_QCLI("FOTA only support flash size big than 4 Mbits\n");
			return QCLI_STATUS_ERROR_E;
		}
	}
	else {
		QCA4004_LOG_QCLI("Please set the flash size (Mbits)\n");
		return QCLI_STATUS_ERROR_E;
	}
	
	if(Parameter_Count > 1 && Parameter_List[1].Integer_Is_Valid)
		flag = Parameter_List[1].Integer_Value;
	if(Parameter_Count > 2 && Parameter_List[2].Integer_Is_Valid)
		ver = Parameter_List[2].Integer_Value;
	if(Parameter_Count > 3) 
		filePath = (char*)Parameter_List[3].String_Value;
    result = (QCLI_Command_Status_t)qca4004_fota_start(filePath, flag, &ver);
	
	QCA4004_LOG_QCLI("Fota UintTest ret=%d\n",result);
	QCA4004_LOG_QCLI("updated firmware version=%d.%d.%d.%d\n",
			(ver&0xF0000000)>>28,
            (ver&0x0F000000)>>24,
            (ver&0x00FF0000)>>16,
            (ver&0x0000FFFF));

    return result;
}

/*****************************************************************************
 *
 * qca4004 get current fw version which is running in HW
 *****************************************************************************/
QCLI_Command_Status_t get_fw_version(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
	uint32_t ver;
	if(QCA4004_OK == qca4004_get_fw_version(&ver)) {
		QCA4004_LOG_QCLI("qca4004 firmware version=%d.%d.%d.%d\n",
			(ver&0xF0000000)>>28,
            (ver&0x0F000000)>>24,
            (ver&0x00FF0000)>>16,
            (ver&0x0000FFFF));
	
		return QCLI_STATUS_SUCCESS_E;
	}
	else {
		QCA4004_LOG_QCLI("Please confirm you have power on QCA4004 once to get the firmware version\n");
	}
	return QCLI_STATUS_ERROR_E;
}
static QCLI_Command_Status_t QCA4004_Driver_print_cmd_info(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

// List of commands registered with QCLI
const QCLI_Command_t qca4004_driver_cmd_list[] =
{
    /* Command_Function, Start_Thread, Command_String, Usage_String, Description, Abort_function */
	{driverInit, false, "driverInit", "driverInit", "qca4004 driver init", NULL},
    {powerStateChange, false, "powerStateChange", "<state>", "change the power state of QCA4004", NULL},
    {getAPList, false, "getAPList", "<timeout>", "get AP list from QCA4004", NULL},
    {powerOnOff, false, "powerOnOff", "<value>", "powerOnOff", NULL},
	{driverDeInit, false, "driverDeInit", "driverDeInit", "qca4004 driver de-init test", NULL},
	{testAtCmd, false, "testAtCmd", "testAtCmd", "qca4004 at command test", NULL},
	{Fota_UintTest, false, "Fota_UintTest", "<flash size> <flag> <version> <filename>", "fota uint test", NULL},
    {get_fw_version, false, "get_fw_version", "get_fw_version", "get qca4004 firmware version", NULL},
	{QCA4004_Driver_print_cmd_info, false, "qca4004help", "qca4004help",
            "Print info about all qca4004 dirver commands", NULL},
};

static QCLI_Command_Status_t QCA4004_Driver_print_cmd_info(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    int i;
    uint32_t qca4004CmdListSize;
    QCLI_Command_t* cmdPtr;

    // Browse through loc_cmd_list[]
    qca4004CmdListSize = sizeof(qca4004_driver_cmd_list)/sizeof(qca4004_driver_cmd_list[0]);
    QCA4004_LOG_QCLI("QCA4004 Cmd List size: %u\n", qca4004CmdListSize);

    cmdPtr = NULL;
    for(i=0; i<qca4004CmdListSize; i++){

        cmdPtr = (QCLI_Command_t*)&qca4004_driver_cmd_list[i];
        QCA4004_LOG_QCLI("Command: %s", cmdPtr->Command_String);
        QCA4004_LOG_QCLI("Usage: %s", cmdPtr->Usage_String);
        QCA4004_LOG_QCLI("Description: %s\n", cmdPtr->Description);
    }

    return QCLI_STATUS_SUCCESS_E;
}

// Parent group for all QCA4004 Driver Commands
const QCLI_Command_Group_t qca4004_driver_cmd_group =
{
	"qca4004DriverTest",              /* Group_String: will display cmd prompt as "qca4004DriverTest> " */
    sizeof(qca4004_driver_cmd_list)/sizeof(qca4004_driver_cmd_list[0]),   /* Command_Count */
    qca4004_driver_cmd_list        /* Command_List */
};

// Init function, register with QCLI module
void qca4004_driver_qcli_iface_init()
{
    /* Attempt to reqister the Command Groups with the qcli framework.*/
    qcli_qca4004_driver_handle = QCLI_Register_Command_Group(NULL, &qca4004_driver_cmd_group);
    if (qcli_qca4004_driver_handle)
    {
        QCA4004_LOG_QCLI("QCA4004 Driver Test Registered !");
    }
	return ;
}


