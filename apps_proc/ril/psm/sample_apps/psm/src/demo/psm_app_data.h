/**************************************************************************************************************
 * Copyright (C) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * @file  psm_app.c
 * @author  Qualcomm Technologies, Inc.
 * @brief File contains the implementation for the Power Saving Mode demo application
 *
 **************************************************************************************************************
 *
 *
 *   Power Saving Modem Sample Application Configuration File
 *
 *       INSTRUCTIONS
 *
 * Place // at the start of a line to comment it out.
 * Defaults are populated here. Make changes as necessary
 *
 **************************************************************************************************************/


// IP to send the Ping to. Should be connected to same Network/Callbox
#define IP "8.8.8.8"

// APN to be used when performing Data call. Comment out if NULL
#define APN "airtel.com"

//Value of Active time voted by Application
#define ACTIVE_TIMER 4

//Value of PSM Time voted by Application
#define TIME_IN_PSM 900

//Data event should be performed for every Nth cycle, then place N here
#define DATA_EVENT_FREQUENCY 1

//If Modem has to be loaded in next cycles
#define MODEM_REQUIRED 1

//If Ping has to be succesful before entering PSM. Recommended 0
#define DATA_SUCCESS_REQUIRED 0

//Number of Bytes to send in PING
#define PING_SIZE 1000