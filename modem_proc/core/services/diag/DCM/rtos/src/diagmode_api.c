/*==========================================================================

              Diag API support for buffering and streaming modes

General Description

Copyright (c) 2015, 2017-2018 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                            Edit History

$Header: //components/rel/core.mpss/3.10/services/diag/DCM/rtos/src/diagmode_api.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/30/17   ph      Add support for mode change notification.
08/31/15   ph      File created to add buffering mode APIs

===========================================================================*/
#include "diag.h"
#include "diagdiag_v.h"
#include "diagi_v.h"      /* For diag_tx_mode_type */
#include "diag_mode_v.h"

/*diag_buffer_api_lock_state indicates the state of buffering APIs */
uint8 diag_buffer_api_lock_state=DIAG_BUFFER_API_LOCKED;

/*==========================================================================

FUNCTION DIAG_SET_TX_MODE

DESCRIPTION
  This API sets the TX mode to one of the following :
    DIAG_TX_MODE_STREAMING
    DIAG_TX_MODE_BUFFERED_THRESH
    DIAG_TX_MODE_BUFFERED_CIR

PARAMETERS
    mode: Mode which user wants to set to.

DEPENDENCIES
  This API sets the mode only if the unlock command is sent.

RETURN VALUE
  Returns the mode that DIAG has set.
  DIAG_TX_MODE_INVALID if DIAG is unable to set the mode.
  DIAG_TX_MODE_LOCKED if API is locked.

===========================================================================*/
uint8 diag_set_tx_mode(uint8 mode)
{
    uint8 set_mode = DIAG_TX_MODE_LOCKED;

    if(diag_buffer_api_lock_state == DIAG_BUFFER_API_LOCKED)
    {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_tx_mode: Error in setting the mode since it is locked");
        return set_mode;
    }

    if(mode >DIAG_TX_MODE_LAST)
    {
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Attempting to set invalid mode %d", mode);
        set_mode=DIAG_TX_MODE_INVALID;
        return set_mode;
    }

    /*Currently Clients can set the mode for stream 1 only*/
    set_mode = diag_set_tx_mode_stream(mode, DIAG_STREAM_1 );

    return set_mode;
}

/*==========================================================================

FUNCTION DIAG_GET_TX_MODE

DESCRIPTION
  This API gets the TX mode that DIAG is operating.

PARAMETERS
    None

DEPENDENCIES
  None.

RETURN VALUE
  Returns the mode that DIAG is currently in.
  DIAG_TX_MODE_INVALID if DIAG is unable to get the mode.

===========================================================================*/
uint8 diag_get_tx_mode()
{
    uint8 mode= DIAG_TX_MODE_INVALID;

    mode = diag_get_tx_mode_stream(DIAG_STREAM_1);

    return mode;
}
/*==========================================================================

FUNCTION DIAG_DRAIN_TX_BUFFER

DESCRIPTION
  This API flushes the existing packets in the buffer.

PARAMETERS
    None

DEPENDENCIES
  This API drains the buffer only if the unlock command is sent.

RETURN VALUE
  None

===========================================================================*/
void diag_drain_tx_buffer()
{
    if(diag_buffer_api_lock_state == DIAG_BUFFER_API_LOCKED)
    {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_drain_tx_buffer: API locked, unable to drain");
        return;
    }

    diag_drain_tx_buffer_stream(DIAG_STREAM_1);
}

