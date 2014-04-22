#ifndef UIM_LOGGING_H
#define UIM_LOGGING_H
/*==============================================================================
  FILE:         uim_logging.h

  OVERVIEW:     FIle conatins the inclusions and definitions needed for
                logging related functions.

  DEPENDENCIES: N/A

                Copyright (c) 2014-2015 QUALCOMM Technologies, Inc.
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
==============================================================================*/

/*=============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/uim_logging.h#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

when        who        what, where, why
------      ----       ---------------------------------------------------------
06/07/16    sam        Reduce Timeout_info_logging buffer size
06/07/16    sam        Stub out GPIO Info logging
05/19/16    sam        Remove EFS logging feature
04/27/16    na         Restricting EFS APDU Logging with Feature
03/30/16    sam        Remove the logic to store timeout info in EFS
12/22/14    na         Making APDU Logging file instance based
04/29/14    nmb        Correct LOG_UIM_MAX_CHARS to pass buffer storage checks
01/23/14    yk         The first revision
==============================================================================*/


/* Number of UIM GPIOs as inputs upon timeout w/recovery disabled*/
#define UIM_NUM_TIMEOUT_INFO                                    2

/* Max number of bytes can be logged */
#define LOG_UIM_MAX_CHARS   246

/* Time stamp size */
#define LOG_UIM_TSTAMP_SIZE  8

/* Define data structure */
LOG_RECORD_DEFINE(LOG_UIM_DATA_C)
  byte  length;  /* Actual data length */
  byte  data[LOG_UIM_MAX_CHARS + LOG_UIM_TSTAMP_SIZE];
LOG_RECORD_END

typedef LOG_UIM_DATA_C_type uim_log_data_type;

/* Attribute of followed data */
typedef enum {
  UIM_LOG_ATTRIB_INIT  = 0x00, /* Init value */
  UIM_LOG_TSTAMP       = 0x01, /* Data is time stamp */
  UIM_LOG_TX_DATA      = 0x10, /* Data is ME to UIM */
  UIM_LOG_TX_REPEAT    = 0x30, /* Re-sent data from ME to UIM */
  UIM_LOG_RX_DATA      = 0x80, /* Data is UIM to ME */
  UIM_LOG_RX_REPEAT    = 0xC0  /* Re-sent data from UIM to ME */
} uim_log_attrib_type;

/* Type for logging character used in logging */
typedef struct
{
  uim_log_attrib_type     attrib;
  uim_instance_enum_type  slot_id;
  char                    the_char;
} uim_log_char_type;

/* Type for log mask used for apdu logging*/
typedef enum
{
  INVALID_LOGGING_MASK = 0,
  SINGLE_SLOT_LOGGING_MASK_1098,
  MULTI_SLOT_LOGGING_MASK_14CE
}uimLogMaskType;

typedef struct
{
  uimLogMaskType  log_mask;
}uim_apdu_log_mask_type;

/**
 * Put a byte to APDU log buf
 *
 * @param attr Attribute of logging data
 * @param ch Data to be logged.
 * @param uim_ptr Pointer to the global data common.
 */
void uim_log_put_byte(uim_log_attrib_type attr,
                      char ch,
                      uim_instance_global_type *uim_ptr);

/**
 * Put a time stamp then logging all data in APDU log bug.
 *
 * @param uim_ptr
 */
void uim_log_put_last_byte(uim_instance_global_type *uim_ptr);

#endif /* UIM_LOGGING_H */
