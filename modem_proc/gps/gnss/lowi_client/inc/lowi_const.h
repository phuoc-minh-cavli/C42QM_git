#ifndef __LOWI_CONST_H__
#define __LOWI_CONST_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        LOWI Const Interface Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for
  LOWIConst

  Copyright (c) 2012, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

=============================================================================*/
#if APSS
#include <ctype.h>
#include <inttypes.h>
#include <time.h>
#include <new>
#include <stdint.h>
#include <base_util/vector.h>
#include <inc/lowi_defines.h>
#endif
namespace qc_loc_fw
{

#define BSSID_LEN 6
#define SSID_LEN 32
#define MAX_CHAN_INFO_SIZE 16
#define CLIENT_NAME_LEN 16
#define CELL_POWER_NOT_FOUND 0x7F
#define LOWI_COUNTRY_CODE_LEN 2
#define LOWI_LOCALLY_ADMINISTERED_MAC_OCTET_VAL 0x02

const char* const LOWI_CLIENT_SUFFIX = "-LOWIClient";
const char* const SERVER_NAME = "LOWI-SERVER";
const char* const NOT_AVAILABLE = "NA";
const int ASYNC_DISCOVERY_REQ_VALIDITY_PERIOD_SEC_MAX = 3600;
const int MAX_IFNAME_LEN = 32;

#define WIFI_CHANNEL_SPACING 5    // Channels are 5 MHz apart
#define BAND_2G_FREQ_BASE    2407 //Base frequency of 2G Band in MHz
#define BAND_2G_CHAN_BEGIN   1    //First channel for 2G Band
#define BAND_2G_CHAN_END     14   //Last channel for 2G Band
#define BAND_2G_FREQ_LAST    2484 //Last frequency for 2G Band in MHz
#define BAND_5G_FREQ_BASE    5000 //Base frequency of 5G Band in MHz
#define BAND_5G_CHAN_BEGIN   34   //First channel for 5G Band
#define BAND_5G_CHAN_END     196  //Last channel for 5G Band

#define IS_2G_CHANNEL(x) ((x >= BAND_2G_CHAN_BEGIN) && (x <= BAND_2G_CHAN_END))
} // namespace qc_loc_fw
#endif //#ifndef __LOWI_CONST_H__
