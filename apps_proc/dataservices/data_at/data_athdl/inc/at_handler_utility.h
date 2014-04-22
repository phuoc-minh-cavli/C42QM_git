#ifndef AT_HANDLER_UTILITY_H
#define AT_HANDLER_UTILITY_H
/*===========================================================================
                         AT_HANDLER_UTILITY.H

DESCRIPTION
  This header file contains definitions used for uitility by the AT HANDLER
  Module.

  Copyright (c)2018,2021 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/
//#include "data_atfwd.h"
#include "qapi_diag.h"
#include <stdio.h>
#include "ds_log.h"


/*------------------------------------------------------------------------------
  Log Message Macros
------------------------------------------------------------------------------*/
void data_ril_format_log_msg
(
  const char* file_name, uint32_t line_number, uint32_t mask,
  char *fmt, 
  ...
);


#define ATFWD_LOG_MSG_ERROR(fmtstr, arg1, arg2, arg3)        ds_iot_log_err3(fmtstr, arg1, arg2, arg3)
#define ATFWD_LOG_MSG_ERROR_SPRINT(fmtstr, arg1, arg2, arg3) ds_iot_log_strf_err(fmtstr, arg1, arg2, arg3)

#if defined(AT_LOG_LEVEL_HIGH) || defined(AT_LOG_LEVEL_MED) || defined(AT_LOG_LEVEL_LOW)
#define ATFWD_LOG_MSG_HIGH(fmtstr, arg1, arg2, arg3)         ds_iot_log_high3(fmtstr, arg1, arg2, arg3)
#define ATFWD_LOG_MSG_HIGH_SPRINT(fmtstr, arg1, arg2, arg3)  ds_iot_log_strf_high(fmtstr, arg1, arg2, arg3)
#else
#define ATFWD_LOG_MSG_HIGH(fmtstr, arg1, arg2, arg3)
#define ATFWD_LOG_MSG_HIGH_SPRINT(fmtstr, arg1, arg2, arg3)
#endif

#if defined(AT_LOG_LEVEL_HIGH) || defined(AT_LOG_LEVEL_MED)
#define ATFWD_LOG_MSG_MED(fmtstr, arg1, arg2, arg3)          ds_iot_log_med3(fmtstr, arg1, arg2, arg3)
#define ATFWD_LOG_MSG_MED_SPRINT(fmtstr, arg1, arg2, arg3)   ds_iot_log_strf_med(fmtstr, arg1, arg2, arg3)
#else
#define ATFWD_LOG_MSG_MED(fmtstr, arg1, arg2, arg3)
#define ATFWD_LOG_MSG_MED_SPRINT(fmtstr, arg1, arg2, arg3)
#endif

#if defined(AT_LOG_LEVEL_HIGH)
#define ATFWD_LOG_MSG_LOW(fmtstr, arg1, arg2, arg3)          ds_iot_log_low3(fmtstr, arg1, arg2, arg3)
#define ATFWD_LOG_MSG_LOW_SPRINT(fmtstr, arg1, arg2, arg3)   ds_iot_log_strf_low(fmtstr, arg1, arg2, arg3)
#else
#define ATFWD_LOG_MSG_LOW(fmtstr, arg1, arg2, arg3)
#define ATFWD_LOG_MSG_LOW_SPRINT(fmtstr, arg1, arg2, arg3)
#endif

//ATFWD internal result code
typedef enum
{
  ATFWD_RET_BASE            = 1,
  ATFWD_RET_OK              = 0,
  ATFWD_RET_ERROR           = -1,
  ATFWD_RET_FAILED          = -20,
    ATFWD_RET_QAPI_FAILED           = ATFWD_RET_FAILED - 1,
    ATFWD_RET_INTERNAL_FAILED       = ATFWD_RET_FAILED - 2,
    ATFWD_RET_QMI_FAILED            = ATFWD_RET_FAILED - 3,
  ATFWD_RET_INVALID         = -30,
    ATFWD_RET_GLOBAL_INVALID        = ATFWD_RET_INVALID - 1,
    ATFWD_RET_GLOBAL_SYNC_LOST      = ATFWD_RET_INVALID - 2,
    ATFWD_RET_EP_INVALID            = ATFWD_RET_INVALID - 3,
    ATFWD_RET_EP_OBJ_INVALID        = ATFWD_RET_INVALID - 4,
    ATFWD_RET_EP_INST_INVALID       = ATFWD_RET_INVALID - 5,
    ATFWD_RET_EP_RES_INVALID        = ATFWD_RET_INVALID - 6,
    ATFWD_RET_EP_RES_INST_INVALID   = ATFWD_RET_INVALID - 7,

  ATFWD_RET_EXIST           = -40,
    ATFWD_RET_EP_OBJ_EXIST          = ATFWD_RET_EXIST - 1,
    ATFWD_RET_EP_INST_EXIST         = ATFWD_RET_EXIST - 2,
    ATFWD_RET_EP_RES_EXIST          = ATFWD_RET_EXIST - 3,
    ATFWD_RET_EP_RES_INST_EXIST     = ATFWD_RET_EXIST - 4,

  ATFWD_RET_PARAM_ERR       = -50,
    ATFWD_RET_PARAM_INPUT_ERR       = ATFWD_RET_PARAM_ERR - 1,
    ATFWD_RET_PARAM_RELATE_ERR      = ATFWD_RET_PARAM_ERR - 2,
    ATFWD_RET_PARAM_DEPEND_ERR      = ATFWD_RET_PARAM_ERR - 3,
    ATFWD_RET_PARAM_COUNT_ERR       = ATFWD_RET_PARAM_ERR - 4,
    ATFWD_RET_PARAM_OBJ_ERR         = ATFWD_RET_PARAM_ERR - 5,
    ATFWD_RET_PARAM_RES_ERR         = ATFWD_RET_PARAM_ERR - 6,

  ATFWD_RET_MEMORY_ERR      = -60,
    ATFWD_RET_EP_OBJ_MEMORY_ERR     = ATFWD_RET_MEMORY_ERR - 1,
    ATFWD_RET_EP_INST_MEMORY_ERR    = ATFWD_RET_MEMORY_ERR - 2,
    ATFWD_RET_EP_RES_MEMORY_ERR     = ATFWD_RET_MEMORY_ERR - 3,
    ATFWD_RET_EP_RES_INST_MEMORY_ERR= ATFWD_RET_MEMORY_ERR - 4,

  ATFWD_RET_NOT_FOUND       = -70,
    ATFWD_RET_EP_NOT_FOUND          = ATFWD_RET_NOT_FOUND - 1,
    ATFWD_RET_EP_OBJ_NOT_FOUND      = ATFWD_RET_NOT_FOUND - 2,
    ATFWD_RET_EP_INST_NOT_FOUND     = ATFWD_RET_NOT_FOUND - 3,
    ATFWD_RET_EP_RES_NOT_FOUND      = ATFWD_RET_NOT_FOUND - 4,
    ATFWD_RET_EP_RES_INST_NOT_FOUND = ATFWD_RET_NOT_FOUND - 5,

  ATFWD_RET_UNSUPPORTIVE    = -80,
    ATFWD_RET_QAPI_UNSUPPORTIVE     = ATFWD_RET_UNSUPPORTIVE - 1,
    ATFWD_RET_LOCAL_UNSUPPORTIVE    = ATFWD_RET_UNSUPPORTIVE - 2,
}atfwd_result_code_t;

/** @addtogroup at_qmi_enums
    @{
  */
typedef enum {
  ATHDL_AT_FWD_RESP_RESULT_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  ATHDL_AT_RESULT_ERROR_V01 = 0x00, /**<  Result ERROR. 
       This is set in case of ERROR or CME ERROR. 
       The response buffer contains the error details.  */
  ATHDL_AT_RESULT_OK_V01 = 0x01, /**<  Result OK.
       This is set if the final response must send an OK
       result code to the terminal. 
       The response buffer must not contain an OK.  */
  ATHDL_AT_RESULT_OTHER_V01 = 0x02, /**<  Indicates a result other than ERROR or OK.
       This is set for all result codes other than 
       ERROR or OK.  */
  ATHDL_AT_FWD_RESP_RESULT_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}athdl_at_fwd_resp_result_enum_v01;
/*----------------------------------------------------------------------------
  Macro define
----------------------------------------------------------------------------*/
#define UTIL_AT_STR_LEN_MAX 512 //Experence value

/*----------------------------------------------------------------------------
  Structure define
----------------------------------------------------------------------------*/
//List structure
typedef struct util_at_list_s
{
  struct util_at_list_s * next;
  uint16_t id;
  void * ptr;
}util_at_list_t;
//List structure
typedef struct util_at_list_u8_s
{
  struct util_at_list_u8_s * next;
  uint8_t id;
  void * ptr;
}util_at_list_u8_t;

/*----------------------------------------------------------------------------
  Function declaration
----------------------------------------------------------------------------*/
extern size_t  memscpy(void *dst, size_t dst_size, const void *src, size_t src_size);

uint16 util_at_rand16(void);

char * util_at_save_str(uint8 * input_ptr);
boolean util_at_string_to_Integer(const uint8 *String_Buffer, int32 *Integer_Value);

util_at_list_t * util_at_list_add(util_at_list_t *head, util_at_list_t *node);
void util_at_list_free(util_at_list_t *head);
int util_at_list_count(util_at_list_t *head);
util_at_list_t * util_at_list_find(util_at_list_t *head, uint16_t id);
void *util_at_malloc(size_t  memsize, int line);
#define athdl_malloc(a) util_at_malloc(a, __LINE__)
void util_at_free(void  **ptr, int line);
#define athdl_free(a) util_at_free((void **)(&a), __LINE__)

#endif
