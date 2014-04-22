/*!
  @file
  qflog.h

  @brief
  QFLOG Processing functionality
*/
/*===========================================================================
  Copyright (c) 2018- 2021 by Qualcomm Technologies, Inc.  All Rights Reserved.
 
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


#ifndef __QFLOG_H__   // [
#define __QFLOG_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_types.h"
#include "qapi_socket.h"
#include "ds_util.h"
#include "qapi_ssl.h"
#include "qflog_utils.h"

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/
#define MAX_QFLOG_APP_NAME_SIZE 256


/*-------------------------------------------------------------------------
 * Static & global Variable Declarations
 *-----------------------------------------------------------------------*/

extern qbool_t        QFLOG_Start_Thread_Check;

typedef struct qflog_handle_info_s
{
  struct qflog_handle_info_s *next;        /* Application handle list */
  uint32_t id;                             /* External handle identifier */
  char app_name[MAX_QFLOG_APP_NAME_SIZE];
  uint16_t log_cnt;
  uint16_t max_log_cnt;
  uint16_t max_log_file_size;
  qurt_mutex_t log_mutex;
  char *log_pkts;
} qflog_handle_info_t;


/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief This function is used to initialize the QFLOG module.

   This function must be called before any other QFLOG functions.

   @return
    - true if QFLOG was initialized successfully.
    - false if initialization failed.
*/
qbool_t QFLOG_Initialize(void);

/**
   @brief This function passes characters input from the command line to 
          the QFLOG module for processing.

   @param Length is the number of bytes in the provided buffer.
   @param Buffer is a pointer to the buffer containing the inputted data.

   @return
    - true if QFLOG was initialized successfully.
    - false if initialization failed.
*/
void QFLOG_Process_Input_Data(uint32_t Length, uint8_t *Buffer);

int qflog_abort_handler(void);

void QFLOG_Printf(const char *format, ...);

int32_t QFLOG_Convert_Store_Cert_util(qapi_Net_SSL_Cert_Type_t cert_type, const char* cert_path, const char* key_path, const char* ssl_cert_name);

int32_t QFLOG_Read_Cert_and_Key(const char* cert_name, qapi_Net_SSL_Cert_Type_t cert_type);

int QFLOG_Cert_Convert_and_store(const char* cert_name, qapi_Net_SSL_Cert_Info_t* cert_info);

void Qflog_Delete_File_util(void * Buffer, int Length , char *file_dir , int file_dir_len);

void Qflog_Push_File_util(void * Buffer, int Length , char *file_dir , int file_dir_len);

void QFLOG_Delete_Cmd(void *Buffer,int Length);

void QFLOG_Ok_Cmd(void * Buffer, int Length);

void QFLOG_Push_Cmd(void * Buffer, int Length);

void QFLOG_Error_Cmd(void * Buffer, int Length);

void QFLOG_Hello_Cmd(void * Buffer, int Length);

void QFLOG_Printf_v2(qflog_App_Handler_t handle, const char *format, ...);

void QFLOG_Push_FTL_Cmd(void *Buffer, int Length);

void Qflog_Push_Ftl_util(void * Buffer, int Length);



#endif // ] #ifndef __QFLOG_H__

