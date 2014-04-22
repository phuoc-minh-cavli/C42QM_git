/*****************************************************************************
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/

#ifndef __QCA4004_H__
#define __QCA4004_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "qapi_types.h"


/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief This function will create a thread which monitor the data received from UART.

   @return
    - 0 if operation successful.
    - 1 if operation failed.
*/
uint8_t qca4004_daemon_start(void);

/**
   @brief This function will start a timer for power state change.

   @param[in] value is the current power state. 

   @return
*/
uint8_t start_power_change_timer(uint8_t value);

/**
   @brief This function will start a timer for scan.

   @param[in] timeout is timer duration for scan. 
   
   @return
*/
uint8_t start_scan_timer(uint32_t timeout);

/**
   @brief This function will process the data for mac address.

   @param[in] rsp is the pointer to data. 
   @param[in] rsp_len is the length of data.
   
   @return
*/
void process_mac(uint8_t* rsp, uint16_t rsp_len);

/**
   @brief This function will process the data for scan result.

   @param[in] rsp is the pointer to data. 
   @param[in] rsp_len is the length of data.
   
   @return
*/
void process_aps(uint8_t* rsp, uint16_t rsp_len);

#endif

