/*
 * Copyright (C) 2012, 2018-20 Qualcomm Technologies, Inc. All rights reserved.
 *
 * All data and information contained in or disclosed by this document is
 * confidential and proprietary information of Qualcomm Technologies, Inc. and all
 * rights therein are expressly reserved.  By accepting this material the
 * recipient agrees that this material and the information contained therein
 * is held in confidence and in trust and will not be used, copied, reproduced
 * in whole or in part, nor its contents revealed in any manner to others
 * without the express written permission of Qualcomm Technologies, Inc.
 *
 *
 *  comm.c : Handles com port connection and data transmission
 * ==========================================================================================
 *   $Header: //components/rel/boot.xf/0.2/QcomPkg/Tools/storage/fh_loader/QSaharaServer/src/uart_comm_win.c#3 $
 *   $DateTime: 2021/03/08 04:36:02 $
 *   $Author: pwbldsvc $
 *
 *  Edit History:
 *  YYYY-MM-DD		who		why
 *  -----------------------------------------------------------------------------
 *  2019-11-12       rrawat  Separated functions to uart_comm and usb_comm
 *  2010-09-28       ng      Added command mode support
 *
 *  Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 *==========================================================================================
 */

#include "common_protocol_defs.h"
#include "comm.h"
#include "kickstart_log.h"
#include "kickstart_utils.h"
#include "transport_protocol.h"
#include "windows_utils.h"
#include "winioctl.h"
#include "ntddser.h"



SERIAL_STATUS comm_stat;

void uart_comm_populate_medium_interface(function_table *Medium)
{
	Medium->initialize_medium = uart_transport_init;
	Medium->port_connect      = uart_port_connect;
	Medium->port_disconnect   = uart_port_disconnect;
	Medium->boot_transport_tx = boot_transport_protocol_pkt_tx;
	Medium->boot_transport_rx = boot_transport_protocol_pkt_rx;
}

void uart_port_disconnect()
{
	if (INVALID_PORT_HANDLE_VALUE == com_port.port_fd)
		return;

	dbg(LOG_INFO, "Disconnecting from com port");
	if (com_port.port_fd != INVALID_PORT_HANDLE_VALUE)
	{

		PORT_CLOSE(com_port.port_fd);
		com_port.port_fd = INVALID_PORT_HANDLE_VALUE;
	}
	/* free up allocated space during initialization*/
	boot_transport_layer_deinit();
}

bool uart_port_connect(char* port_name)
{

	COMMTIMEOUTS timeouts;
	DCB dcb;
	BOOL success;


	if (NULL == port_name || port_name[0] == '\0') {
		dbg(LOG_ERROR, "NULL port name");
		dbg(LOG_ERROR, "Should be a COM port on windows (\\.\\COMx)\n"
			"or a device file on Linux (/dev/ttyUSB0)");
		return false;
	}

	com_port.port_name = port_name;

	/* Close any existing open port */
	if (com_port.port_fd != INVALID_PORT_HANDLE_VALUE)
		uart_port_disconnect();

	com_port.port_fd = CreateFileA(com_port.port_name,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_ALWAYS,
		FILE_FLAG_OVERLAPPED,
		NULL);
	if (com_port.port_fd == INVALID_PORT_HANDLE_VALUE) {
		dbg(LOG_ERROR, "Failed to open com port handle");
		return false;
	}

	SecureZeroMemory(&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);
	success = GetCommState(com_port.port_fd, &dcb);
	if (!success)
	{
		dbg(LOG_ERROR, "GetCommState failed with error %d.\n", GetLastError());
		return FALSE;
	}

	// Setting the baud rate to 3000000
	dcb.BaudRate = 3000000;
	dcb.fOutX = 0;
	dcb.fInX = 0;
	dcb.fOutxCtsFlow = 0;
	dcb.fOutxDsrFlow = 0;
	dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
	dcb.ByteSize = 8;

	success = SetCommState(com_port.port_fd, &dcb);
	if (!success)
	{
		dbg(LOG_ERROR, "SetCommState failed with error %d.\n", GetLastError());
		return FALSE;
	}
	printf("\n COM Port properties\nBaudRate = %d, ByteSize = %d, Parity = %d, StopBits = %d\n",
		dcb.BaudRate, dcb.ByteSize, dcb.Parity, dcb.StopBits);

	success = PurgeComm(com_port.port_fd, PURGE_TXCLEAR);
	if (!success)
	{
		dbg(LOG_ERROR, "PurgeComm for tx clear failed with error %d.\n", GetLastError());
		return FALSE;
	}
	success = PurgeComm(com_port.port_fd, PURGE_RXCLEAR);
	if (!success)
	{
		dbg(LOG_ERROR, "PurgeComm for rx clear failed with error %d.\n", GetLastError());
		return FALSE;
	}

	timeouts.ReadIntervalTimeout = 100;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 100; // 100 /* Actual timeout in ms */
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 2000;

	if (!SetCommTimeouts(com_port.port_fd, &timeouts)) // Error setting time-outs
	{
		dbg(LOG_WARN, "Error setting com port timeouts");
	}

	return true;
}

bool uart_tx_data(byte* buffer, size_t bytes_to_write, size_t* bytes_written)
{
	OVERLAPPED overlapped = { 0 };
	BOOL result;
	DWORD bytes_transferred;

	overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);  // manual reset, nonsignaled

	result = WriteFile(com_port.port_fd, buffer, bytes_to_write, &bytes_transferred, &overlapped);

	if (result == 0 && GetLastError() == ERROR_IO_PENDING)
	{
		result = GetOverlappedResult(com_port.port_fd, &overlapped, &bytes_transferred, TRUE);
		if (result && bytes_written) {
			*bytes_written = bytes_transferred;
		}

	}

	com_port.total_bytes_sent += *bytes_written;
	dbg(LOG_INFO, "Total bytes sent so far: %d", com_port.total_bytes_sent);

	CloseHandle(overlapped.hEvent);
	return result;

}

bool uart_rx_data(byte* buffer, uint32_t bytes_to_read, uint32_t* bytes_read)
{

	OVERLAPPED overlapped = { 0 };
	BOOL result;
	DWORD bytes_transferred = 0;

	//uint8_t buff[1024];
	uint32_t last_err;
	uint32_t bytes_left = bytes_to_read;
	*bytes_read = 0;

	overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);  // manual reset, nonsignaled
	uint32_t num_of_tries = 0;


	while ((*bytes_read < bytes_to_read) && (num_of_tries < UART_MAX_READ_RETRIES))
	{

		result = ReadFile(com_port.port_fd, (uint8_t*)buffer + (*bytes_read), bytes_left, &bytes_transferred, &overlapped);

		*bytes_read += bytes_transferred;
		bytes_left -= bytes_transferred;

		last_err = GetLastError();


		if (result == 0 && GetLastError() == ERROR_IO_PENDING)
		{
			result = GetOverlappedResult(com_port.port_fd, &overlapped, &bytes_transferred, TRUE);
			if (result && bytes_read) {
				*bytes_read += bytes_transferred;
				bytes_left -= bytes_transferred;
			}
			else { printf("Error in reading\n"); }
		}

		if (bytes_transferred == 0)
		{
			num_of_tries += 1;
		}
	}

	dbg(LOG_INFO, "Received %d bytes", *bytes_read);
	CloseHandle(overlapped.hEvent);
	return result;
}

void uart_transport_init()
{
	/*initialize transport layer for uart*/
	bool retval = false;
	do {
		retval = boot_transport_layer_init();
		if (false == retval) {
			dbg(LOG_ERROR, "Transport protocol error");
			break;
		}
		else {
			dbg(LOG_STATUS, "Transport INIT completed");
		}
	} while (false == retval);
}