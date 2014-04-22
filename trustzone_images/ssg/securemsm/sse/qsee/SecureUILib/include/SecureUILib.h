/** @file qsee/SecureUILib/include/SecureUILib.h
 * @brief
 * This file contains the interfaces to use the Secure UI Library
 */

/*===========================================================================
 * Copyright(c) 2013-2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#pragma once
#include <stdbool.h>
#include <TouchApiDefs.h>
#include <SecureUILibDefs.h>

/** @addtogroup qsee_ui_library
@{ */

/**
  Application callback called upon touch events.

  @datatypes
  #sec_touch_err_t \n
  #tsFingerData

  @param[in]   err      Error code from the operation executed in the HLOS.
  @param[in]   fingers  Pointer to the fingers (touch data) related to the
                        event.
  @param[out]  timeout  Pointer to the number of milliseconds to wait for the
                        next touch. A value of -1 indicates an infinite wait.

  @return
  Command for the next touch operation. @newpage
*/
typedef sec_touch_cmd_t (*sec_touch_callback_t)(sec_touch_err_t err, struct tsFingerData *fingers, int32_t *timeout);

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  Determines whether a command is intended for Secure UI, and if so, processes
  it. If this function returns TRUE, no further processing is required by the
  calling application.

  @param[in]   req      Pointer to the request from the QSApp client.
  @param[in]   cmdlen   Length of the request buffer in bytes.
  @param[out]  rsp      Pointer to the response buffer.
  @param[in]   rsplen   Length of the response buffer in bytes.

  @return
  TRUE  -- The command was intended for secure UI and has been processed. \n
  FALSE -- The secure UI library system failed to process the command. For
           example, the command was not intended for it or an internal error
           occurred.

  @dependencies
  None.
*/
int secUiProcessCmd(void* req, uint32_t cmdlen, void* rsp, uint32_t rsplen);

/**
  Determines whether a command is intended for Secure UI.

  @param[in]   req      Pointer to request from the QSApp client.
  @param[in]   cmdlen   Length of the request buffer in bytes.

  @return
  TRUE  -- The command is intended for the secure UI library subsystem. \n
  FALSE -- Otherwise.

  @dependencies
  None. @newpage
 */
bool secUiCmd(void* req, uint32_t cmdlen);

/**
  Registers the Secure Touch callback.

  @datatypes
  #sec_touch_callback_t

  @param[in]   callback  Callback function; NULL to unregister.

  @return
  None.

  @dependencies
  None.
*/
void secUiRegisterTouchCallback(sec_touch_callback_t callback);

/**
  Queries whether Secure Touch is active.

  @return
  TRUE if active, FALSE otherwise.

  @dependencies
  None.
*/
bool secUiTouchIsActive(void);

/**
  Stops Secure Touch.

  @return
  TRUE if the touch is stopped after this call terminates, i.e., the resources
  locked for Secure Touch in the TrustZone software have been released. \n
  FALSE otherwise.

  @dependencies
  None.
*/
bool secUiTouchStop(void);

/**
  Sets the touch panel size.

  @param[in] width    Panel width in pixels.
  @param[in] height   Panel height in pixels.

  @return
  None.

  @dependencies
  None.
*/
void secUiSetPanelSize(uint32_t width, uint32_t height);

/** @} */ /* end_addtogroup qsee_ui_library */
