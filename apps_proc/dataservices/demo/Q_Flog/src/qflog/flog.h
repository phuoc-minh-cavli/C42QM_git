/*!
  @file
  flog.h

  @brief
  Console functionality for QFLOG
*/
/*===========================================================================
  Copyright (c) 2018-2019  by Qualcomm Technologies, Inc.  All Rights Reserved.
 
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

#ifndef __FLOG_H__
#define __FLOG_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_types.h"
#include "qurt_signal.h"
#include "sio.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/


#define FLOG_RECIEVE_BUFFER_SIZE                         (512)
#define FLOG_RECIEVE_BUFFER_COUNT                        (10)

#define FLOG_RECEIVE_EVENT_MASK                          (0x00000001)
#define START_THREAD_EVENT_MASK                          (0x00000001)


#define FLOG_ENTER_CRITICAL()                            do { __asm("cpsid i"); } while(0)
#define FLOG_EXIT_CRITICAL()                             do { __asm("cpsie i"); } while(0)


/**
   This definition indicates the character that is inpretted as an end of 
   line for inputs from the console.
*/
#define FLOG_INPUT_END_OF_LINE_CHARACTER                     '\r'

/**
   This definition represents the string that is used as the end of line 
   for outputs to the console.
*/
#define FLOG_OUTPUT_END_OF_LINE_STRING                       "\r\n"


#define DSM_FLOG_SIO_TX_ITEM_SIZ                 512
#ifndef MINI_QCLI
#define DSM_FLOG_SIO_TX_ITEM_CNT                 256
#else
#define DSM_FLOG_SIO_TX_ITEM_CNT                 100
#endif

#define DSM_FLOG_SIO_TX_ITEM_ARRAY_SIZ (DSM_FLOG_SIO_TX_ITEM_CNT * DSM_FLOG_SIO_TX_ITEM_SIZ)

extern void ds_format_log_msg
(
  char *buf_ptr,
  int buf_size,
  char *fmt,
  ...
);

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

typedef struct FLOG_Context_s
{
   uint8_t             Rx_Buffer[FLOG_RECIEVE_BUFFER_COUNT][FLOG_RECIEVE_BUFFER_SIZE];
   uint8_t             Rx_Buffer_Length[FLOG_RECIEVE_BUFFER_COUNT];
   uint8_t             Rx_In_Index;
   uint8_t             Rx_Out_Index;
   volatile uint32_t   Rx_Buffers_Free;

   qurt_signal_t       Rx_Event;
   sio_stream_id_type  stream_id;
   dsm_watermark_type   tx_wm_queue;
   q_type                tx_q;

#if 0
   dsm_watermark_type   rx_wm_queue;
   q_type               rx_q;
     #endif        //rx_q;

} FLOG_Context_t;


typedef struct FLOG_Data_s
{
   uint8_t ctx_id;
   uint8_t cmd_type;
   uint16_t pkt_id;
   uint32_t tot_size;
   uint16_t payload_len;
   uint8_t pad_2[2];
   uint8_t * payload;
  
} FLOG_Data_t;

typedef enum FLOG_cmd_type_t
{

  FLOG_ERROR = -1,
  FLOG_HELLO     ,
  FLOG_OK        ,
  FLOG_PUSH      ,   
  FLOG_PULL      ,
  FLOG_DELETE    ,
  FLOG_LOG       ,
  FLOG_LOG_ENABLE,
  FLOG_LOG_DISABLE,
  LWM2M_CONFIG_PUSH,
  LWM2M_CONFIG_PULL,
  LWM2M_CONFIG_DELETE,
  LWM2M_CERT_PUSH,
  LWM2M_CERT_PULL,
  LWM2M_CERT_DEL,
  APPCONFIG_PUSH,
  APPCONFIG_PULL,
  APPCONFIG_DELETE,
  APPCONFIG_CERT_NET_SSL_CERTIFICATE_PUSH,
  APPCONFIG_CERT_NET_SSL_CERTIFICATE_PULL,
  APPCONFIG_CERT_NET_SSL_CERTIFICATE_DELETE,
  APPCONFIG_CERT_NET_SSL_CA_LIST_PUSH,
  APPCONFIG_CERT_NET_SSL_CA_LIST_PULL,
  APPCONFIG_CERT_NET_SSL_CA_LIST_DELETE,
  APPCONFIG_CERT_NET_SSL_PSK_TABLE_PUSH,
  APPCONFIG_CERT_NET_SSL_PSK_TABLE_PULL,
  APPCONFIG_CERT_NET_SSL_PSK_TABLE_DELETE,
  APPCONFIG_CERT_NET_SSL_DI_CERT_PUSH,
  APPCONFIG_CERT_NET_SSL_DI_CERT_PULL,
  APPCONFIG_CERT_NET_SSL_DI_CERT_DELETE,
  LOG_PULL,
  MODULE_LOAD_START,
  MODULE_STOP_UNLOAD,
  PULL_OK,
  PULL_ERROR,
  LOGS_CONFIG,
  FLOG_PUSH_FTL,
  FLASH_MODULE_LOAD_START,
}FLOG_Cmd_Type;


/*-------------------------------------------------------------------------
 * Static & global Variable Declarations
 *-----------------------------------------------------------------------*/

extern qurt_signal_t         QFLOG_Start_Thread_Event;


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
void FLOG_Console_Write(uint32_t Length, uint8_t *Buffer);


void FLOG_Packet_Write(uint32_t Length, uint8_t *Buffer);


void PAL_Exit(void);

#endif
