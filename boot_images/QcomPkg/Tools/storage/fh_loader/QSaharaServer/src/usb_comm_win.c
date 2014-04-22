/*
 * Copyright (C) 2012, 2018, 2020 Qualcomm Technologies, Inc. All rights reserved.
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
 *   $Header: //components/rel/boot.xf/0.2/QcomPkg/Tools/storage/fh_loader/QSaharaServer/src/usb_comm_win.c#3 $
 *   $DateTime: 2021/03/08 04:36:02 $
 *   $Author: pwbldsvc $
 *
 *  Edit History:
 *  YYYY-MM-DD		who		why
 *  -----------------------------------------------------------------------------
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


com_port_t com_port = {
	"",                   // port_name
	INVALID_PORT_HANDLE_VALUE, // port_fd
	0,                    // total_bytes_sent
	0,                    // total_bytes_recd
	USING_USB,            // transport_medium
	SAHARA_PROTOCOL,      // protocol
	10000,                    // rx_timeout
	5000,                 // sleep_btwn_reads_usec
	1048576,              // MAX_TO_READ
	1048576,              // MAX_TO_WRITE
	24                    // BYTES_TO_PRINT
};


SERIAL_STATUS comm_stat;

void usb_comm_populate_medium_interface(function_table* Medium)
{
	Medium->initialize_medium = usb_transport_init;
	Medium->port_connect      = usb_port_connect;
	Medium->port_disconnect   = usb_port_disconnect;
	Medium->boot_transport_tx = boot_usb_tx_data;
	Medium->boot_transport_rx = boot_usb_rx_data;
}

void usb_port_disconnect()
{
	if (INVALID_PORT_HANDLE_VALUE == com_port.port_fd)
		return;

	dbg(LOG_INFO, "Disconnecting from com port");
	if (com_port.port_fd != INVALID_PORT_HANDLE_VALUE)
	{
		PORT_CLOSE(com_port.port_fd);
		com_port.port_fd = INVALID_PORT_HANDLE_VALUE;
	}
}


bool usb_port_connect(char* port_name)
{

	COMMTIMEOUTS timeouts;

	if (NULL == port_name || port_name[0] == '\0') {
		dbg(LOG_ERROR, "NULL port name");
		dbg(LOG_ERROR, "Should be a COM port on windows (\\.\\COMx)\n"
			"or a device file on Linux (/dev/ttyUSB0)");
		return false;
	}
	com_port.port_name = port_name;

	/* Close any existing open port */
	if (com_port.port_fd != INVALID_PORT_HANDLE_VALUE)
		usb_port_disconnect();

	com_port.port_fd = CreateFileA(com_port.port_name,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,              // FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
		NULL);
	if (com_port.port_fd == INVALID_PORT_HANDLE_VALUE) {
		dbg(LOG_ERROR, "Failed to open com port handle");
		return false;
	}

	if (com_port.protocol == SAHARA_PROTOCOL) {
		if (com_port.rx_timeout > 0) {
			timeouts.ReadIntervalTimeout = com_port.rx_timeout;
			timeouts.ReadTotalTimeoutMultiplier = com_port.rx_timeout;
			timeouts.ReadTotalTimeoutConstant = 10000;
		}
		else {
			timeouts.ReadIntervalTimeout = 0;
			timeouts.ReadTotalTimeoutMultiplier = 0;
			timeouts.ReadTotalTimeoutConstant = 0;
		}
		timeouts.WriteTotalTimeoutMultiplier = com_port.rx_timeout;
		timeouts.WriteTotalTimeoutConstant = 10000;
	}
	else if (com_port.protocol == DLOAD_PROTOCOL) {
		timeouts.ReadIntervalTimeout = 10;
		timeouts.ReadTotalTimeoutMultiplier = 1;
		timeouts.ReadTotalTimeoutConstant = 1;
		timeouts.WriteTotalTimeoutMultiplier = 1;
		timeouts.WriteTotalTimeoutConstant = 10;
	}
	if (!SetCommTimeouts(com_port.port_fd, &timeouts)) // Error setting time-outs
	{
		dbg(LOG_WARN, "Error setting com port timeouts");
	}

	return true;
}

bool boot_usb_tx_data(byte* buffer, size_t bytes_to_send) {
	int temp_bytes_sent;
	size_t bytes_sent = 0;

	dbg(LOG_INFO, "Transmitting %d bytes", bytes_to_send);

	if (kickstart_options.verbose > 1) {
		print_buffer(buffer, bytes_to_send, com_port.BYTES_TO_PRINT);
	}

	while (bytes_sent < bytes_to_send) {

		if (!WriteFile(com_port.port_fd, buffer + bytes_sent, MIN(bytes_to_send - bytes_sent, com_port.MAX_TO_WRITE), (LPDWORD)& temp_bytes_sent, NULL)) {
			dbg(LOG_ERROR, "Error occurred while writing to COM port");
			return false;
		}
		else {
			bytes_sent += temp_bytes_sent;
		}
	}

	com_port.total_bytes_sent += bytes_sent;
	dbg(LOG_INFO, "Total bytes sent so far: %d", com_port.total_bytes_sent);

	return true;
}

bool boot_usb_rx_data(byte* buffer, size_t bytes_to_read, size_t* bytes_read)
{
	DWORD retval;

	if (!ReadFile(com_port.port_fd, buffer, MIN(bytes_to_read, com_port.MAX_TO_READ), &retval, NULL)) {
		dbg(LOG_ERROR, "Error occurred while reading from COM port");
		return false;
	}

	if (NULL != bytes_read)
		* bytes_read = retval;

	dbg(LOG_INFO, "Received %d bytes", retval);
	com_port.total_bytes_recd += retval;
	// dbg (INFO, "Total bytes received so far: %d", com_port.total_bytes_recd);

	if (kickstart_options.verbose > 1) {
		print_buffer(buffer, (size_t)retval, com_port.BYTES_TO_PRINT);
	}

	return true;
}

void usb_transport_init()
{
	/*empty method*/
	return;
}
