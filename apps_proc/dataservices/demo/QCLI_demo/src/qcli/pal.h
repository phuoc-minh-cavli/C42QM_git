/*!
  @file
  pal.c

  @brief
  Console functionality for QCLI
*/
/*===========================================================================
  Copyright (c) 2017-2019  by Qualcomm Technologies, Inc.  All Rights Reserved.
 
  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.
 ======================================================================*/


/*
 * Copyright (c) 2015 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#ifndef __PAL_H__
#define __PAL_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_types.h"
#include "qurt_signal.h"
#include "sio.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/


#define PAL_RECIEVE_BUFFER_SIZE                         (128)
#define PAL_RECIEVE_BUFFER_COUNT                        (1)

#define PAL_RECEIVE_EVENT_MASK                          (0x00000001)
#define START_THREAD_EVENT_MASK                          (0x00000001)


#define PAL_ENTER_CRITICAL()                            do { __asm("cpsid i"); } while(0)
#define PAL_EXIT_CRITICAL()                             do { __asm("cpsie i"); } while(0)


/**
   This definition indicates the character that is inpretted as an end of 
   line for inputs from the console.
*/
#define PAL_INPUT_END_OF_LINE_CHARACTER                     '\r'

/**
   This definition represents the string that is used as the end of line 
   for outputs to the console.
*/
#define PAL_OUTPUT_END_OF_LINE_STRING                       "\r\n"


#define DSM_QCLI_SIO_TX_ITEM_SIZ                 512
#ifndef MINI_QCLI
#define DSM_QCLI_SIO_TX_ITEM_CNT                 512
#else
#define DSM_QCLI_SIO_TX_ITEM_CNT                100
#endif

#define DSM_QCLI_SIO_TX_ITEM_ARRAY_SIZ (DSM_QCLI_SIO_TX_ITEM_CNT * DSM_QCLI_SIO_TX_ITEM_SIZ)




/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

typedef struct PAL_Context_s
{
   uint8_t             Rx_Buffer[PAL_RECIEVE_BUFFER_COUNT][PAL_RECIEVE_BUFFER_SIZE];
   uint8_t             Rx_Buffer_Length[PAL_RECIEVE_BUFFER_COUNT];
   uint8_t             Rx_In_Index;
   uint8_t             Rx_Out_Index;
   volatile uint32_t   Rx_Buffers_Free;

   qurt_signal_t       Rx_Event;
   sio_stream_id_type  stream_id;
   dsm_watermark_type   tx_wm_queue;
   q_type                tx_q;

} PAL_Context_t;


/*-------------------------------------------------------------------------
 * Static & global Variable Declarations
 *-----------------------------------------------------------------------*/

extern qurt_signal_t         Start_Thread_Event;


/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief This function is used to write a buffer to the console. Note
          that when this function returns, all data from the buffer will
          be written to the console or buffered locally. 

   @param Length is the length of the data to be written.
   @param Buffer is a pointer to the buffer to be written to the console.
*/
void PAL_Console_Write(uint32_t Length, const uint8_t *Buffer);

/**
   @brief This function is indicates to the PAL layer that the application 
          should exit. For embedded applications this is typically a reset.
*/
void PAL_Exit(void);

#endif
