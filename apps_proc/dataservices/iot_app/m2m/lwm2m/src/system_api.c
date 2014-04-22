/******************************************************************************

  @file    system_api.c

  ---------------------------------------------------------------------------

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/
/*******************************************************************************
 *
 * Copyright (c) 2014 Bosch Software Innovations GmbH, Germany.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Bosch Software Innovations GmbH - Please refer to git log
 *
 *******************************************************************************/
/*
 *  event_handler.c
 *
 *  Callback for value changes.
 *
 *  Created on: 21.01.2015
 *  Author: Achim Kraus
 *  Copyright (c) 2014 Bosch Software Innovations GmbH, Germany. All rights reserved.
 */
#include "liblwm2m.h"
#include "lwm2mclient.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static lwm2m_context_t *lwm2mContext;

/**
 * @fn static void prv_value_change()
 *
 * @brief This function is used to get the changes in the resource value
 *
 * @param context handle to context
 * @param uriPath resource uri
 * @param value handle to resource value
 * @param valueLength length of the value handle
 *
 * @return void
 *
 */
void value_change(
    const char* uriPath,
    const char * value,
    size_t valueLength)
{
  lwm2m_uri_t uri;
  if (lwm2mContext == NULL || uriPath == NULL || value == NULL)
  {
    return;
  }
  if (lwm2m_stringToUri(uriPath, strlen(uriPath), &uri))
  {
    handle_value_changed(lwm2mContext, &uri, value, valueLength);
  }
}
/**
 * @fn void init_value_change()
 *
 * @brief This function is used to set the value change handler function
 *
 * @param lwm2m handle to lwm2m context
 *
 * @return void
 *
 */
void init_value_change(lwm2m_context_t * lwm2m)
{
  if (lwm2m == NULL)
    return;
  lwm2mContext = lwm2m;
}

/**
 * @fn void deinit_value_change()
 *
 * @brief This function is used to reset the value change handler function
 *
 * @param in void
 *
 * @return void
 *
 */
void deinit_value_change(void)
{
  lwm2mContext = NULL;
}

