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
 *   $Header: //components/rel/boot.xf/0.2/QcomPkg/Tools/storage/fh_loader/QSaharaServer/src/uart_comm_lnx.c#3 $
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
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

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
		tcflush(com_port.port_fd, TCIOFLUSH);

		PORT_CLOSE(com_port.port_fd);
		com_port.port_fd = INVALID_PORT_HANDLE_VALUE;
	}
	/* free up allocated space during initialization*/
	boot_transport_layer_deinit();
}

bool uart_port_connect(char* port_name)
{
	struct termios tio;
	struct termios settings;
	bool using_tty_device;
	int retval;

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

	using_tty_device = (strspn(port_name, "/dev/tty") == strlen("/dev/tty"));

	// Opening the com port
	// Change for Android port - Opening in O_SYNC mode since tcdrain() is not supported
	com_port.port_fd = open(port_name, O_RDWR | O_SYNC);

	if (INVALID_PORT_HANDLE_VALUE == com_port.port_fd)
		return false;

	if (USING_UART == com_port.transport_medium && true == using_tty_device) {
		dbg(LOG_INFO, "\nUSING UART DETECTED pPort='%s'", port_name);
		dbg(LOG_INFO, "\nConfiguring 3000000, 8n1\n\n");

		memset(&tio, 0, sizeof(tio));
		tio.c_iflag = 0;
		tio.c_oflag = 0;
		tio.c_cflag = CS8 | CREAD | CLOCAL;           // 8n1, see termios.h for more information
		tio.c_lflag = 0;
		tio.c_cc[VMIN] = 1;
		tio.c_cc[VTIME] = 5;
		cfsetospeed(&tio, B3000000);
		cfsetispeed(&tio, B3000000);
		tcsetattr(com_port.port_fd, TCSANOW, &tio);
	}

	// Clear any contents
	// Change for Android port - tcdrain() not supported
	// tcdrain (pComm->port_fd);

	if (using_tty_device)
	{
		// Configure termios settings

		// Get the COM port settings
		retval = tcgetattr(com_port.port_fd, &settings);
		if (-1 == retval) {
			dbg(LOG_ERROR, "termio settings could not be fetched Linux System Error:%s", strerror(errno));
			return false;
		}

		// CREAD to enable receiver and CLOCAL to say that the device is directly connected to the host
		cfmakeraw(&settings);
		settings.c_cflag |= CREAD | CLOCAL;

		// configure the new settings
		if (com_port.protocol != SAHARA_PROTOCOL)
			tcflush(com_port.port_fd, TCIOFLUSH);

		retval = tcsetattr(com_port.port_fd, TCSANOW, &settings);
		if (-1 == retval) {
			dbg(LOG_ERROR, "Device could not be configured: Linux System Errno: %s",
				strerror(errno));
			return false;
		}
	}

	return true;
}

bool uart_tx_data(byte* buffer, size_t bytes_to_write, size_t* bytes_written)
{
	int temp_bytes_sent;
	size_t bytes_sent = 0;

	dbg(LOG_INFO, "Transmitting %d bytes", bytes_to_write);

	if (kickstart_options.verbose > 1) {
		print_buffer(buffer, bytes_to_write, com_port.BYTES_TO_PRINT);
	}

	while (bytes_sent < bytes_to_write) {
		// dbg (INFO, "Calling flush before write");
		// tcflush (pComm->port_fd, TCIOFLUSH);
		temp_bytes_sent = write(com_port.port_fd, buffer + bytes_sent, MIN(bytes_to_write - bytes_sent, com_port.MAX_TO_WRITE));
		if (temp_bytes_sent < 0) {
			dbg(LOG_ERROR, "Write returned failure %d, errno %d, System error code: %s", temp_bytes_sent, errno, strerror(errno));
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

bool uart_rx_data(byte* buffer, uint32_t bytes_to_read, uint32_t* bytes_read)
{
	fd_set rfds;
	struct timeval tv;
	int retval;
	int available_bytes = 0 ;
	int wait_count = 1000 ;
	
	// Init read file descriptor
	FD_ZERO(&rfds);
	FD_SET(com_port.port_fd, &rfds);

	// time out initializtion.
	tv.tv_sec = com_port.rx_timeout >= 0 ? com_port.rx_timeout : 0;
	tv.tv_usec = 0;

	retval = select(com_port.port_fd + 1, &rfds, NULL, NULL, ((com_port.rx_timeout >= 0) ? (&tv) : (NULL)));

	if (0 == retval) {
		dbg(LOG_ERROR, "Timeout Occured, No response or command came from the target!");
		return false;
	}
	if (retval < 0) {
		dbg(LOG_ERROR, "select returned error: %s", strerror(errno));
		return false;
	}
	
	/* sleep until all of the data is received in buffer */
	do
	{
		ioctl(com_port.port_fd, FIONREAD, &available_bytes);
		usleep(5); 
		wait_count-- ;
		if(wait_count==0) break; 
	}while((available_bytes < bytes_to_read) && (bytes_to_read > UART_LARGE_PACKETS_WAIT_THRESHOLD));
		
	retval = read(com_port.port_fd, buffer, MIN(bytes_to_read, com_port.MAX_TO_READ));
	if (0 == retval) {
		dbg(LOG_ERROR, "Zero length packet received or hardware connection went off");
		return false;
	}
	else if (retval < 0) {
		// tcflush (com_port.port_fd, TCIOFLUSH);
		if (EAGAIN == errno) {
			// Sleep before retrying in case we were using
			// non-blocking reads
			usleep(com_port.sleep_btwn_reads_usec);
		}
		else {
			dbg(LOG_ERROR, "Read/Write File descriptor returned error: %s, error code %d", strerror(errno), retval);
			return false;
		}
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