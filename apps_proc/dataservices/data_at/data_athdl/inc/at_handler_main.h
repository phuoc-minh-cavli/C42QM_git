#ifndef AT_HANDLER_MAIN_H
#define AT_HANDLER_MAIN_H

/*===========================================================================
                         AT_HANDLER_MAIN.H

DESCRIPTION
  This header file contains definitions of AT HANDLER
  Module.

  Copyright (c)2018 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/
#include "qapi_atfwd.h"
#include "at_handler_utility.h"

/*----------------------------------------------------------------------------
  Common define
----------------------------------------------------------------------------*/
#define ATHDL_MAX_ARG 13
#define ATHDL_URC_LEN_MAX 140
#define ATHDL_RESP_LEN_MAX 100
#define ATHDL_RESP_LEN_MIN 50
#define ATHDL_MAX_CMD_REG  QMI_AT_NUM_CMD_MAX_V01

//AT handler command mask
#define ATHDL_MASK_EMPTY    0   //Denotes a feed back mechanism for AT reg and de-reg
#define ATHDL_MASK_NA_V01   1   //Denotes presence of Name field
#define ATHDL_MASK_EQ_V01   2   //Denotes presence of equal (=) operator
#define ATHDL_MASK_QU_V01   4   //Denotes presence of question mark (?)
#define ATHDL_MASK_AR_V01   8   //Denotes presence of trailing argument operator

//AT parameter token
typedef struct
{
  uint8 *arg[ATHDL_MAX_ARG];   /*AT command arguments */
  uint16 args_found;                      /*Keeps track of the number of AT command arguments found*/
}athdl_tokens_t;

//Function declaration
boolean  athdl_parse_param(uint8 *a_ptr, athdl_tokens_t *tokens_res);
/*----------------------------------------------------------------------------
  Command table related define
----------------------------------------------------------------------------*/
#define ATHDL_MAX_CMDNAME 50

//AT command handle function define
typedef atfwd_result_code_t (*athdl_cmd_func)(char * , athdl_tokens_t * );

//AT command table define
typedef struct athdl_cmd_table_s
{
  const char * at_cmd_name;
  athdl_cmd_func test_cmd;
  athdl_cmd_func set_cmd;
  athdl_cmd_func exec_cmd;
  athdl_cmd_func read_cmd;
}athdl_cmd_table_t;

//AT command list define
typedef struct athdl_cmdtbl_list_s
{
  struct athdl_cmdtbl_list_s * next;
  uint16 count;
  athdl_cmd_table_t * cmd_table;
}athdl_cmdtbl_list_t;


//Function declaration
boolean athdl_cmd_list_extend(athdl_cmdtbl_list_t ** new_ep);
/*----------------------------------------------------------------------------
  Forward callback related define
----------------------------------------------------------------------------*/
//AT Handler command event define
typedef enum
{
  DATA_ATFWD_ATHDL_MIN_CMD_EVT = 5, //DATA_ATFWD_MAX_CMD_EVT
  DATA_ATFWD_ATHDL_AT_EVT,
  DATA_ATFWD_ATHDL_LWM2M_EVT,
  DATA_ATFWD_ATHDL_LWM2M_OBSERVE_EVT,
  DATA_ATFWD_ATHDL_MAX_CMD_EVT
} athdl_cmdevt_id_t;

#define ATHDL_ID_RANGE_CHECK(cmd_id) \
                                     ((cmd_id > DATA_ATFWD_ATHDL_MIN_CMD_EVT) && \
                                     (cmd_id < DATA_ATFWD_ATHDL_MAX_CMD_EVT))

//Forward data struct
typedef struct
{
  boolean is_reg;                        /*  If AT command is registered*/
  char *  atcmd_name;                    /*  Pointer to an AT commands string*/
  uint8*  at_fwd_params;                 /*  Pointer to the callback corresponding to the AT commands*/
  uint8   mask;                          /*  AT commands mask*/
  uint32  at_handle;                     /*  Length of AT commands string*/
} athdl_fwddata_t;

//Function declaration
atfwd_result_code_t athdl_fwd_evt_enq(int evt_id, void * extra_data_ptr);
atfwd_result_code_t athdl_fwd_cb_hdl(boolean is_reg, char *atcmd_name, uint8* at_fwd_params, uint8 mask, uint32 at_handle);
void athdl_start(void);
#endif
