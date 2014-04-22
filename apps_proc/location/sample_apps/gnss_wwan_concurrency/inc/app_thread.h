/*****************************************************************************
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#ifndef __APP_THREAD_H__
#define __APP_THREAD_H__

#include <stdint.h>
#include "tx_api.h"
#include "tx_thread.h"

typedef void (*threadEntry)(unsigned long args);
typedef void (*msgHandler)(void *args, size_t size);

typedef enum
{
    APP_THREAD_ID_MIN,
	APP_THREAD_ID_CONTROLLER,
	APP_THREAD_ID_GNSS_WORKER,
	APP_THREAD_ID_MAX
} app_thread_id;

typedef struct {
	TX_THREAD *handle;
	const void *msgQ;
	uint8_t bValid;
} AppThreadContext;

typedef struct {
	void *msg;
	size_t msgSize;
	msgHandler handler;
	int msgId;
} AppMsg;

void app_thread_module_init();
AppThreadContext app_get_context(app_thread_id threadId);
int app_thread_create(const char *threadName, threadEntry entryFunction, const app_thread_id threadId);
int app_thread_destroy(app_thread_id threadId);
int app_thread_send_msg(app_thread_id dstThreadId, int msgId, void *msg, size_t msgSize);
int app_thread_rcv_msg(app_thread_id threadId, void **payload);

#endif /* __APP_THREAD_H__ */
