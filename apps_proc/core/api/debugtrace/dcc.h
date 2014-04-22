#ifndef DCC_H
#define DCC_H

/*=========================================================================
                       DCC PUBLIC API

GENERAL DESCRIPTION
   Public interface to DCC HW block providing functionality to Capture or CRC
   compare an address range.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   dcc_init should have been invoked via RCINIT for all other APIs to be
   functionally correct.
               Copyright 2015 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               QUALCOMM Proprietary and Confidential
==========================================================================*/
/*==========================================================================
 $Header: //components/rel/core.tx/6.0/api/debugtrace/dcc.h#1 $
==========================================================================*/

#include "comdef.h"

/* DCC API return status */
typedef enum
{
  DCC_ENOERR, /* No error */
  DCC_EINVAL, /* Invalid Params */
  DCC_ENOMEM, /* No memory for operation */
  DCC_ENOSPC, /* No space on device */
  DCC_EBUSY,  /* Device is busy */
  DCC_EINIT,  /* Driver not initialized */
  DCC_ERANGE, /* Param out of range */
  DCC_ENOCONFIG, /* Congiuration not complete. Yet to set LL, Mode or Sink */
  DCC_ECLOCK,    /* Failed to set up clock */
  DCC_CRC_ERR,   /* CRC check on data failed */
  DCC_EUNKNOWN /* Not expected. Used for fallback when assert is turned-off*/
}dcc_return_type;

/* DCC Modes of operation */
typedef enum
{
  DCC_CONFIG_DATA_CAPTURE,
  DCC_CONFIG_DATA_CRC,
}dcc_config_mode;

/* DCC data sink options */
typedef enum
{
  DCC_CONFIG_SINK_SRAM,
  DCC_CONFIG_SINK_ATB,
}dcc_config_sink;

/**
Description:
   Add an address range to be captured or CRC checked by DCC HW.

Input:
  addr: Physical address that can be accessed by the HW.
  len : The len from addr that needs to be operated upon by the HW. HW has
        limited SRAM to store data.

Return: Check dcc_return_type for possible values.
**/
dcc_return_type dcc_config_add_address_range (uint32 addr, uint32 len);

/**
Description:
   Reset the configuration data. DCC driver has the following configuration
   data: Mode, Sink, Address range. Use this API to reset the current config.
   Addresses programmed are lost and driver goes to a state where it cannot be
   enabled for SW or HW trigger.

Input: None

Return: Check dcc_return_type for possible values.
**/
dcc_return_type dcc_config_reset (void);

/**
Description:
   Set Mode of operation to Capture or CRC.

Input:
  mode: requested mode of operation

Return: Check dcc_return_type for possible values.
**/
dcc_return_type dcc_config_set_mode (dcc_config_mode mode);

/**
Description:
   Set data sink to SRAM or ATB. If the mode is CRC, sink is irrelevant.
   For data capture mode of operation, setting sink to SRAM, will collect data
   into DCC SRAM and ATB will queue data to QDSS.

Input:
  sink: requested sink for data

Return: Check dcc_return_type for possible values.
**/
dcc_return_type dcc_config_set_sink (dcc_config_sink sink);

/**
Description:
   Set data sink to SRAM or ATB. If the mode is CRC, sink is irrelevant.
   For data capture mode of operation, setting sink to SRAM, will collect data
   into DCC SRAM and ATB will queue data to QDSS.

Input:
  sink: requested sink for data

Return: Check dcc_return_type for possible values.
**/
dcc_return_type dcc_enable (void);

/**
Description:
   Set data sink to SRAM or ATB. If the mode is CRC, sink is irrelevant.
   For data capture mode of operation, setting sink to SRAM, will collect data
   into DCC SRAM and ATB will queue data to QDSS.

Input:
  sink: requested sink for data

Return: Check dcc_return_type for possible values.
**/
dcc_return_type dcc_trigger_operation (void);

/**
Description:
   Check the current status of the HW block. API does not verify the current
   driver state to check if an operation had been queued in the past since
   the operation can be HW or SW triggered. It will only query HW for its
   current state. If HW indicates no error it could mean no operation had been
   triggered.

Input: None

Return: Check dcc_return_type for possible values.
**/
dcc_return_type dcc_check_operation_status (void);

/**
Description:
  Get the current CRC check status and CRC value. API does not verify the
  current driver state to check if an operation had been queued in the past
  since the operation can be HW or SW triggered. It will only query HW for its
  current state. If HW indicates no error it could mean no operation had been
  triggered.

Input:
  crc: Buffer to hold current value in the CRC register.

Return: Check dcc_return_type for possible values.
**/
dcc_return_type dcc_get_current_crc (uint32 *crc);

/**
Description:
  Helper function to get current configured mode of operation.

Input:
  mode: Buffer to hold current config state.

Return: Check dcc_return_type for possible values.
**/
dcc_return_type dcc_config_get_mode (dcc_config_mode *mode);

/**
Description:
  Helper function to get current configured sink.

Input:
  sink: Buffer to hold current config state.

Return: Check dcc_return_type for possible values.
**/
dcc_return_type dcc_config_get_sink (dcc_config_sink *sink);

/**
Description:
  Helper function to read the sram data. The read operation is state-less and
  does not have pointer to where the previous read left off. Use offset to
  specify where to read from.

Input:
  buffer : Buffer to hold the data
  offset : offset from base of SRAM to read
  len: length of the supplied buffer.
  read_len : Buffer to specify the actual length of data read. If read less
             than specified len, indicates end of SRAM.

Return: Check dcc_return_type for possible values.
**/
dcc_return_type dcc_read_sram_data (void *buffer, uint32 offset,
                                    uint32 len, uint32 *read_len);

/**
Description:
  Helper function to dump the DCC register space which is needed to interpret
  the DCC SRAM data.

Input:
  buffer : Buffer to hold the data
  offset : offset from register base. Needs to be 32bit aligned.
  len: length of the supplied buffer. Needs to be 32bit aligned.
  read_len : Buffer to specify the actual length of data read. If read less
             than specified len, indicates end of register space.

Return: Check dcc_return_type for possible values.
**/
dcc_return_type dcc_dump_registers (void *buffer, uint32 offset,
                                    uint32 len, uint32 *read_len);

#endif
