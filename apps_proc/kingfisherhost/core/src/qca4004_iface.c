/*****************************************************************************
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "qca4004_iface.h"
#include "qca4004.h"
#include "qca4004_utils.h"
#include "qca4004_internal.h"
#include "qca4004_fota.h"

/*-------------------------------------------------------------------------
 * Static & global Variable Declarations
 *-----------------------------------------------------------------------*/
 
extern void process_ota(uint8_t* rsp, uint16_t rsp_len);
/* response Engine */
AT_RSP_ENTRY at_rsps[] = {
    /* RSP              	LEN                      	 process_rsp*/   
    { "MAC=",           sizeof("MAC=")-1,        process_mac},
	{ "APS=",           sizeof("APS=")-1,        process_aps},
	{ "OTA=",           sizeof("OTA=")-1,        process_ota},
	{ NULL,             0,                       NULL}
};


/*-------------------------------------------------------------------------
 * Local Function Definitions
 *-----------------------------------------------------------------------*/

int8_t find_rsp_idx(uint8_t* rsp, uint16_t len)
{
	int   idx = 0;

	while((at_rsps[idx].rsp) != NULL)
	{
		if(qca4004_strncmpi((char *)rsp, (char *)at_rsps[idx].rsp, at_rsps[idx].rsp_len) == 0)
		{
			return idx;
		}
		idx++;
	}
	
	return -1;
}


/*-------------------------------------------------------------------------
 * Public Function Definitions
 *-----------------------------------------------------------------------*/

/*
 * the maximum size of response is 2048 (MAXIMUM_IFACE_BUFFER_LENGTH)
 * the format of response:
 *----------------------------------------------------------------------
 * | START	| RESPONSE TYPE	| DATA LENGTH	| DATA		| END 	|
 *----------------------------------------------------------------------
 * | 1byte	| 4bytes			| 2bytes		| data length	| 1byte	|
 *----------------------------------------------------------------------
 * | 0x02		| APS=			| 			|			| 0x03	|
 *----------------------------------------------------------------------
 */

extern QCA4004_FOTA_Ctx_t *qca4004_fota_ctxt_p;

void QCA4004_Process_Input_Data(uint32_t length, uint8_t *buffer)
{
	uint16_t data_len = 0;
	int8_t rsp_idx = -1;
	uint8_t *buf;
	
	if(qca4004_fota_ctxt_p != NULL && qca4004_fota_ctxt_p->fota_state == QCA4004_FOTA_STATE_H2T_DATA)
	{
		uint32_t i = 0;
		for(;i<length;i++)
		{
			if(buffer[i] == QCA4004_FOTA_ACK)
			{
				qca4004_fota_ctxt_p->retValue = 1;
				qurt_signal_set(&(qca4004_fota_ctxt_p->signal), 0x01);
			}
			else if(buffer[i] == QCA4004_FOTA_NAK)
			{
				qca4004_fota_ctxt_p->retValue = 0;
				qurt_signal_set(&(qca4004_fota_ctxt_p->signal), 0x01);
			}
		}
		return;
	}
	#if 0
	uint32_t i = 0;
	for(;i<length;i++)
		QCA4004_DEBUG_LOG("data:%x,",buffer[i]);
	#endif
	QCA4004_DEBUG_LOG("process input: length=%d,",length);
	
	while((length) && (buffer))
	{
		if(buffer[0] == START_OF_HLOS_STREAM)
		{
			break;
		}
		buffer++;
		length--;
	}

	buf = buffer+1;
	rsp_idx = find_rsp_idx(buf, 4);
	if(rsp_idx == -1)
	{
		return ;
	}
	buf += 4;

	data_len = buf[1]*256 + buf[0];
	if(data_len == 0 || data_len > (length - 8))
	{
		return ;
	}
	buf += 2;

	if(buf[data_len] != END_OF_HLOS_STREAM)
	{
		return ;
	}

	at_rsps[rsp_idx].process_rsp(buf, data_len);
	return ;
}


