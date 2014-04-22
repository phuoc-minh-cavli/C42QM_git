#ifndef DS_MUX_LOGGING_H
#define DS_MUX_LOGGING_H
/*===========================================================================

                                 D S   M U X   L O G G I N G

                                 H E A D E R   F I L E

DESCRIPTION
  This is the Internal header file for the MUX Logging. This file
  contains all the functions, definitions and data types needed for
  logging MUX packets.

  Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/inc/ds_mux_logging.h#1 $
  $DateTime: 2019/11/26 00:18:17 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/16   ad      initial version
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/
#include "ds_mux_defs.h"
#include "ds_mux_phys.h"
#include "ds_mux_io.h"
#include "log.h"
#include "log_codes.h"
#include "ps_byte.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define      DS_MUX_LOGGING_LOG_PKT_VER                 0x0
#define      DS_MUX_LOGGING_MAX_PKT_LEN                 100
#define      DS_MUX_LOGGING_MUX_HEADER_LEN              7
#define      DS_MUX_LOGGING_INTERFACE_NAME_LEN          16
#define      DS_MUX_LOGGING_EVENT_NAME_LEN              16
#define      DS_MUX_LOGGING_PAYLOAD_MAX_LEN             64
#define      DS_MUX_LOGGING_CONN_STATUS_VER_ID          1
#define      DS_MUX_LOGGING_MAX_VERSION_ID_LEN          1
#define      DS_MUX_LOGGING_MAX_DLCI_ID_LEN             1

#define      DS_MUX_LOGGING_MAX_STATE_LOG_LEN   (DS_MUX_LOGGING_MAX_DLCI_ID_LEN +   \
                   DS_MUX_LOGGING_MAX_VERSION_ID_LEN+                  \
                   DS_MUX_LOGGING_INTERFACE_NAME_LEN +         \
                   DS_MUX_LOGGING_EVENT_NAME_LEN +      \
                   DS_MUX_LOGGING_PAYLOAD_MAX_LEN + 1 )


//Below Defination should be removed Once Diag definations is available in log_codes.h file
//#define LOG_DATA_MUX_LOGICAL_CONNECTIONS_STATUS_C     ((0x9C1) + LOG_1X_BASE_C)

/*---------------------------------------------------------------------------
  Size of header added by DIAG subsytem to each log packet
---------------------------------------------------------------------------*/
#define DS_MUX_LOG_HDR_SIZE  sizeof(log_hdr_type)


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

typedef enum
{
  DS_MUX_LOGGING_CNTL_CHANNEL_DLCI0  = 0,
  DS_MUX_LOGGING_QMI_INTERFACE       = 1,
  DS_MUX_LOGGING_MSGR_INTERFACE      = 2,
  DS_MUX_LOGGING_DPM_DATA_INTERFACE  = 3,

  //This should be last
  DS_MUX_LOGGING_MAX_INTERFACE       = 4

}ds_mux_logging_application_type;


typedef enum
{
  DS_MUX_LOGGING_PORT_OPEN_REQUEST  = 0,
  DS_MUX_LOGGING_PORT_CLOSE_REQUEST = 1,
  DS_MUX_LOGGING_SABM_REQ_SENT      = 2,
  DS_MUX_LOGGING_SABM_REQ_RECV      = 3,
  DS_MUX_LOGGING_SABM_CONNECTED     = 4,
  DS_MUX_LOGGING_PN_SENT            = 5,
  DS_MUX_LOGGING_PN_RECVD           = 6,
  DS_MUX_LOGGING_MSC_SENT           = 7,
  DS_MUX_LOGGING_MSC_RECV           = 8,
  DS_MUX_LOGGING_PWR_SAVE_REQ_SENT  = 9,
  DS_MUX_LOGGING_IN_PWR_SAVE        = 10,
  DS_MUX_LOGGING_WAKE_UP_REQ        = 11,
  DS_MUX_LOGGING_WAKE_UP_RES        = 12,
  DS_MUX_LOGGING_WAKE_UP            = 13,
  DS_MUX_LOGGING_FLOW_ENABLE        = 14,
  DS_MUX_LOGGING_FLOW_DISABLE       = 15,
  DS_MUX_LOGGING_PWR_SAVE_RES_RECV  = 16,

  //This should be last
  DS_MUX_LOGGING_STATE_MAX          = 17
 }ds_mux_logging_connection_state_type;


/*===========================================================================
MACRO   DS_MUX_LOG_PHY_TX_PACKET

DESCRIPTION
  Log a full dsm item which is transmitted over physical interface

PARAMETERS
  tx_packet       : packet to be logged

RETURN VALUE
   None
NOTE:
   DS MUX will expect only one MUX packet per DSM item and will log first 100 bytes only
===========================================================================*/
#define DS_MUX_LOG_PHY_TX_PACKET(tx_packet)                      \
  ds_mux_logging_log_raw_packet (tx_packet, LOG_DATA_MUX_TX_RAW_PACKET_C )


/*===========================================================================
MACRO   DS_MUX_LOG_PHY_RX_PACKET

DESCRIPTION
  Log a full dsm item which is received over physical interface

PARAMETERS
  tx_packet       : packet to be logged

RETURN VALUE
   None

 NOTE:
   DS MUX will expect only one MUX packet per DSM item and will log first 100 bytes only
===========================================================================*/
#define DS_MUX_LOG_PHY_RX_PACKET(tx_packet)                      \
  ds_mux_logging_log_raw_packet (tx_packet, LOG_DATA_MUX_RX_RAW_PACKET_C )


/*===========================================================================
MACRO   DS_MUX_LOG_APP_TX_PACKET

DESCRIPTION
  Log a full dsm item which is transmitted by logical application

PARAMETERS
  tx_packet       : packet to be logged

RETURN VALUE
   None
===========================================================================*/
#define DS_MUX_LOG_APP_TX_PACKET(dlci_id, tx_packet)                      \
  ds_mux_logging_log_app_packet (dlci_id, tx_packet, LOG_DATA_MUX_TX_FRAME_PACKET_C )


/*===========================================================================
MACRO   DS_MUX_LOG_APP_RX_PACKET

DESCRIPTION
   Log a full dsm item which  is delivered to logical application

PARAMETERS
  tx_packet       : packet to be logged

RETURN VALUE
   None
===========================================================================*/
#define DS_MUX_LOG_APP_RX_PACKET( dlci_id, tx_packet)                      \
  ds_mux_logging_log_app_packet (dlci_id, tx_packet, LOG_DATA_MUX_RX_FRAME_PACKET_C )

/*===========================================================================
MACRO   DS_MUX_LOG_STATE_INFO1_0

DESCRIPTION
  Log DLCI state with no argument

PARAMETERS
  DLCI ID  - DLCI ID
  EVENT ID - event ID
  fmtString - Format String
RETURN VALUE
   None
===========================================================================*/

#define DS_MUX_LOG_STATE_INFO1_0( dlci_id, event_id)                                              \
{                                                                                                  \
/*-------------------------------------------------------------------------------------*/\
  ds_mux_logging_log_connection_status( dlci_id, event_id, NULL);                                  \
}


/*===========================================================================
MACRO   DS_MUX_LOG_STATE_INFO1_1

DESCRIPTION
  Log DLCI state with one argument

PARAMETERS
  DLCI ID  - DLCI ID
  EVENT ID - event ID
  fmtString - Format String
RETURN VALUE
   None
===========================================================================*/

#define DS_MUX_LOG_STATE_INFO1_1( dlci_id, event_id, fmtString, a)                                \
{                                                                                                  \
  uint8            event_payload[DS_MUX_LOGGING_PAYLOAD_MAX_LEN];                                  \
/*-------------------------------------------------------------------------------------*/\
  memset(&event_payload, 0x0, DS_MUX_LOGGING_PAYLOAD_MAX_LEN);                                     \
  snprintf((char *)&event_payload,DS_MUX_LOGGING_PAYLOAD_MAX_LEN, (const char*)fmtString, a);                   \
  ds_mux_logging_log_connection_status( dlci_id, event_id, (uint8 *)&event_payload[0]);                        \
}
/*===========================================================================
MACRO   DS_MUX_LOG_STATE_INFO1_2

DESCRIPTION
  Log DLCI state with two argument

PARAMETERS
  DLCI ID  - DLCI ID
  EVENT ID - event ID
  fmtString - Format String
RETURN VALUE
   None
===========================================================================*/

#define DS_MUX_LOG_STATE_INFO1_2( dlci_id, event_id, fmtString, a,b)                                \
{                                                                                                  \
  uint8            event_payload[DS_MUX_LOGGING_PAYLOAD_MAX_LEN];                                  \
/*-------------------------------------------------------------------------------------*/\
  memset(&event_payload, 0x0, DS_MUX_LOGGING_PAYLOAD_MAX_LEN);                                     \
  snprintf((char *)&event_payload,DS_MUX_LOGGING_PAYLOAD_MAX_LEN, fmtString, a, b);                \
  ds_mux_logging_log_connection_status( dlci_id, event_id, (uint8 *)&event_payload);                        \
}
/*===========================================================================
MACRO   DS_MUX_LOG_STATE_INFO1_3

DESCRIPTION
  Log DLCI state with three argument

PARAMETERS
  DLCI ID  - DLCI ID
  EVENT ID - event ID
  fmtString - Format String
RETURN VALUE
   None
===========================================================================*/

#define DS_MUX_LOG_STATE_INFO1_3( dlci_id, event_id, fmtString, a,b, c)                            \
{                                                                                                  \
  uint8            event_payload[DS_MUX_LOGGING_PAYLOAD_MAX_LEN];                                  \
/*-------------------------------------------------------------------------------------*/\
  memset(&event_payload, 0x0, DS_MUX_LOGGING_PAYLOAD_MAX_LEN);                                     \
  snprintf((char *)&event_payload,DS_MUX_LOGGING_PAYLOAD_MAX_LEN, fmtString, a, b, c);             \
  ds_mux_logging_log_connection_status( dlci_id, event_id, (uint8 *)&event_payload);                        \
}

/*===========================================================================
MACRO   DS_MUX_LOG_STATE_INFO1_4

DESCRIPTION
  Log DLCI state with four argument

PARAMETERS
  DLCI ID  - DLCI ID
  EVENT ID - event ID
  fmtString - Format String
RETURN VALUE
   None
===========================================================================*/

#define DS_MUX_LOG_STATE_INFO1_4( dlci_id, event_id, fmtString, a,b, c, d)                         \
{                                                                                                  \
  uint8            event_payload[DS_MUX_LOGGING_PAYLOAD_MAX_LEN];                                  \
/*-------------------------------------------------------------------------------------*/\
  memset(&event_payload, 0x0, DS_MUX_LOGGING_PAYLOAD_MAX_LEN);                                     \
  snprintf((char *)&event_payload,DS_MUX_LOGGING_PAYLOAD_MAX_LEN, fmtString, a, b, c, d);          \
  ds_mux_logging_log_connection_status( dlci_id, event_id, (uint8 *)&event_payload);                        \
}
/*===========================================================================
MACRO   DS_MUX_LOG_STATE_INFO1_5

DESCRIPTION
  Log DLCI state with five argument

PARAMETERS
  DLCI ID  - DLCI ID
  EVENT ID - event ID
  fmtString - Format String
RETURN VALUE
   None
===========================================================================*/
#define DS_MUX_LOG_STATE_INFO1_5( dlci_id, event_id, fmtString, a,b, c, d, e)                      \
{                                                                                                  \
  uint8            event_payload[DS_MUX_LOGGING_PAYLOAD_MAX_LEN];                                  \
/*-------------------------------------------------------------------------------------*/\
  memset(&event_payload, 0x0, DS_MUX_LOGGING_PAYLOAD_MAX_LEN);                                     \
  snprintf((char *)&event_payload,DS_MUX_LOGGING_PAYLOAD_MAX_LEN, fmtString, a, b, c, d, e);       \
  ds_mux_logging_log_connection_status( dlci_id, event_id, (uint8 *)&event_payload);                        \
}
/*===========================================================================
MACRO   DS_MUX_LOG_STATE_INFO1_6

DESCRIPTION
  Log DLCI state with six argument

PARAMETERS
  DLCI ID  - DLCI ID
  EVENT ID - event ID
  fmtString - Format String
RETURN VALUE
   None
===========================================================================*/
#define DS_MUX_LOG_STATE_INFO1_6( dlci_id, event_id, fmtString, a,b, c, d, e, f)                   \
{                                                                                                  \
  uint8            event_payload[DS_MUX_LOGGING_PAYLOAD_MAX_LEN];                                  \
/*-------------------------------------------------------------------------------------*/\
  memset(&event_payload, 0x0, DS_MUX_LOGGING_PAYLOAD_MAX_LEN);                                     \
  snprintf((char *)&event_payload,DS_MUX_LOGGING_PAYLOAD_MAX_LEN, fmtString, a, b, c, d, e, f);    \
  ds_mux_logging_log_connection_status( dlci_id, event_id, (uint8 *)&event_payload);                        \
}

/**
  @brief Log all packets recived/transmit over physical interface
*/
void ds_mux_logging_log_raw_packet
(
  dsm_item_type* item_ptr ,
  uint16         log_code
);

/**
  @brief Log all packets recived/transmit over logical interface
*/
void ds_mux_logging_log_app_packet
(
  uint8          dlci_id,
  dsm_item_type* item_ptr,
  uint16         log_code
);

/**
  @brief Log all events and state of logical connection
*/
void ds_mux_logging_log_connection_status
(
  uint8          dlci_id,
  uint8          event_id,
  uint8          *payload
);

#endif /* DS_MUX_LOGGING_H */
