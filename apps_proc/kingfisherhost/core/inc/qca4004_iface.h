/*****************************************************************************
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/

#ifndef __QCA4004_IFACE_H__
#define __QCA4004_IFACE_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "qapi_types.h"
#include "qurt.h"


/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/
#define START_OF_HLOS_STREAM	2
#define END_OF_HLOS_STREAM		3


/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
 * This structure contains the information for one response enrty.
 */
typedef struct {
    uint8_t*	rsp;
    uint16_t	rsp_len;
    void 	(* process_rsp)(uint8_t* rsp, uint16_t rsp_len); 
} AT_RSP_ENTRY;


/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/
 
/**
   @brief This function is used to process input data from QCA4004

   @param Length is the number of bytes in the provided buffer.
   @param Buffer is a pointer to the buffer containing the inputted data.

   @return
*/
void QCA4004_Process_Input_Data(uint32_t length, uint8_t *buffer);

#endif

