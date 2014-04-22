/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D S   M U X    L O G G I N G

GENERAL DESCRIPTION
  This file contains all the functions, definitions and data types needed
  for MUX logging.



  Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/src/ds_mux_logging.c#1 $
  $DateTime: 2019/11/26 00:18:17 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/16   ad     Initial version
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#include "ds_mux_logging.h"

/*===========================================================================
                     Global Defination
===========================================================================*/

uint8    ds_mux_interface_name[DS_MUX_LOGGING_MAX_INTERFACE][DS_MUX_LOGGING_INTERFACE_NAME_LEN] =
{
   "DLCI0 CHANNEL ",
   "QMI INTERNAL ",
   "MSGR ",
   "DPM DATA PATH ",
};


uint8    ds_mux_logging_event_name[DS_MUX_LOGGING_STATE_MAX][DS_MUX_LOGGING_EVENT_NAME_LEN] =
{
"PORT OPEN REQ", /*Index : DS_MUX_LOGGING_PORT_OPEN_REQUEST*/
"PORT CLOSE REQ",/*Index : DS_MUX_LOGGING_PORT_CLOSE_REQUEST*/
"CONN REQ SEND", /*Index : DS_MUX_LOGGING_SABM_REQ_SENT*/
"CONN REQ RECV",/*Index : DS_MUX_LOGGING_SABM_REQ_RECV*/
"CONNECTED",/*Index : DS_MUX_LOGGING_SABM_CONNECTED*/
"PN REQ SEND",/*Index : DS_MUX_LOGGING_PN_SENT*/
"PN REQ RECV",/*Index : DS_MUX_LOGGING_PN_RECVD*/
"MSC REQ SENT",/*Index : DS_MUX_LOGGING_MSC_SENT*/
"MSC REQ RECV",/*Index : DS_MUX_LOGGING_MSC_RECV*/
"PWR SAVEREQ SENT",/*Index : DS_MUX_LOGGING_PWR_SAVE_REQ_SENT*/
"PWR SAVE STATE",/*Index : DS_MUX_LOGGING_IN_PWR_SAVE*/
"WAKEUP REQUEST",/*Index : DS_MUX_LOGGING_WAKE_UP_REQ*/
"WAKEUP RESPOND",/*Index : DS_MUX_LOGGING_WAKE_UP_RES*/
"WOKEUP STATE",/*Index : DS_MUX_LOGGING_WAKE_UP*/
"FLOW ENABLE",/*Index : DS_MUX_LOGGING_FLOW_ENABLE*/
"FLOW DISABLE",/*Index : DS_MUX_LOGGING_FLOW_DISABLE*/
"PWR SAVE RSP",/*Index : DS_MUX_LOGGING_PWR_SAVE_RES_RECV*/

};


/*===========================================================================
                     Function Defination
===========================================================================*/
void ds_mux_logging_log_raw_packet
(
  dsm_item_type* item_ptr ,
  uint16         log_code
)
{
  uint8 *                log_ptr    = NULL;
  uint16                 len        = 0;
  uint16                 offset     = 0;
  uint8 *                base_log_ptr  = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Invalid DSM item.
  -----------------------------------------------------------------------*/
  if( NULL == item_ptr || 0 == dsm_length_packet(item_ptr))
  {
    return;
  }
  do
  {
    len = MIN(DS_MUX_LOGGING_MAX_PKT_LEN,(uint16) dsm_length_packet(item_ptr));
    //MAX data packet + Log header + version byte (1)
    log_ptr = (uint8*)log_alloc(log_code, len + DS_MUX_LOG_HDR_SIZE + 1);

    if (log_ptr == NULL)
    {
      return;
    }
    //Assign Base pointer
    base_log_ptr = log_ptr;

    log_ptr += DS_MUX_LOG_HDR_SIZE;
    log_ptr = put8(log_ptr, DS_MUX_LOGGING_LOG_PKT_VER);

    /*-----------------------------------------------------------------------
      Extract data from item_ptr into log buffer(log_ptr).
    -----------------------------------------------------------------------*/
    if (dsm_seek_extract(&item_ptr,
          &offset,
          (void *)log_ptr  ,
          &len) == FALSE)
    {
      break;
    }
    /*-----------------------------------------------------------------------
      Invalid copy
    -----------------------------------------------------------------------*/
    if( 0 == len || len > DS_MUX_LOGGING_MAX_PKT_LEN)
    {
      break;
    }
    log_ptr = log_ptr + len;

    log_shorten(base_log_ptr, (uint32) (log_ptr - base_log_ptr));

    log_commit(base_log_ptr);

    return;
  }while(0);

  log_free(base_log_ptr);

  return;
}/* ds_mux_logging_log_raw_packet */


void ds_mux_logging_log_app_packet
(
  uint8          dlci_id,
  dsm_item_type* item_ptr,
  uint16         log_code
)
{
  uint8 *                log_ptr       = NULL;
  uint8 *                base_log_ptr  = NULL;
  uint16                 len           = 0;
  uint16                 offset        = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Invalid DSM item.
  -----------------------------------------------------------------------*/
  if( NULL == item_ptr || 0 == dsm_length_packet(item_ptr))
  {
    return;
  }

  do
  {
    len = MIN(DS_MUX_LOGGING_MAX_PKT_LEN,(uint16) dsm_length_packet(item_ptr));

    //Max Data packet + Log header + MUX header(7 byte)
    log_ptr = (uint8*)log_alloc(log_code, (len + DS_MUX_LOG_HDR_SIZE + DS_MUX_LOGGING_MUX_HEADER_LEN) );

    if (log_ptr == NULL)
    {
      return;
    }
    //Assign Base pointer
    base_log_ptr = log_ptr;

    log_ptr += DS_MUX_LOG_HDR_SIZE;
    log_ptr = put8(log_ptr, DS_MUX_LOGGING_LOG_PKT_VER);
    //Advance the log pointer to point DLCI ID index
    log_ptr++;
    log_ptr = put8(log_ptr, dlci_id);
    log_ptr = put8(log_ptr, DS_MUX_FRAME_TYPE_UTIL_UIH);

    log_ptr = base_log_ptr + DS_MUX_LOG_HDR_SIZE + DS_MUX_LOGGING_MUX_HEADER_LEN;
    /*-----------------------------------------------------------------------
      Extract data from item_ptr into log buffer(log_ptr).
    -----------------------------------------------------------------------*/
    if (dsm_seek_extract(&item_ptr,
          &offset,
          (void *)(log_ptr ) ,
          &len) == FALSE)
    {
      break;
    }
    /*-----------------------------------------------------------------------
      Invalid copy
    -----------------------------------------------------------------------*/
    if( 0 == len || len > DS_MUX_LOGGING_MAX_PKT_LEN)
    {
      break;
    }
    log_ptr = log_ptr + len;

    log_shorten(base_log_ptr, (uint32) (log_ptr - base_log_ptr));

    log_commit(base_log_ptr);

    return;
  }while(0);

  log_free(base_log_ptr);

  return;
}/* ds_mux_logging_log_raw_packet */


void ds_mux_logging_log_connection_status
(
  uint8          dlci_id,
  uint8          event_id,
  uint8          *payload
)
{
  uint8 *                log_ptr       = NULL;
  uint8 *                base_log_ptr  = NULL;
  uint16                 len           = DS_MUX_LOGGING_MAX_STATE_LOG_LEN;
  uint8                  copy_len       = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( dlci_id >= DS_MUX_LOGGING_MAX_INTERFACE ||
      event_id >= DS_MUX_LOGGING_STATE_MAX )
  {
    return;
  }

  do
  {
    //Max Data packet + Log header + MUX header(7 byte)
    log_ptr = (uint8*)log_alloc(LOG_DATA_MUX_LOGICAL_CONNECTIONS_STATUS_C, DS_MUX_LOG_HDR_SIZE + len );

    if (log_ptr == NULL)
    {
      return;
    }
    //Assign Base pointer
    base_log_ptr = log_ptr;

    log_ptr += DS_MUX_LOG_HDR_SIZE;

    memset(log_ptr, 0x0, len);

    log_ptr = put8(log_ptr, DS_MUX_LOGGING_CONN_STATUS_VER_ID);
    log_ptr = put8(log_ptr, dlci_id);

    //Copy Interface description
    copy_len = memscpy( log_ptr,
                        DS_MUX_LOGGING_INTERFACE_NAME_LEN,
                        ds_mux_interface_name[dlci_id],
                        strnlen((const char*)ds_mux_interface_name[dlci_id], DS_MUX_LOGGING_INTERFACE_NAME_LEN));

    log_ptr = log_ptr + DS_MUX_LOGGING_INTERFACE_NAME_LEN;

    //Copy Event Name
    copy_len = memscpy( log_ptr,
                        DS_MUX_LOGGING_EVENT_NAME_LEN,
                        ds_mux_logging_event_name[event_id],
                        strnlen((const char*)ds_mux_logging_event_name[event_id], DS_MUX_LOGGING_EVENT_NAME_LEN));

    log_ptr = log_ptr + DS_MUX_LOGGING_EVENT_NAME_LEN;

    //Copy event payload if available
    if( NULL != payload)
    {
      copy_len = memscpy( log_ptr,
                          DS_MUX_LOGGING_PAYLOAD_MAX_LEN,
                          payload,
                          strnlen((const char*)payload, DS_MUX_LOGGING_PAYLOAD_MAX_LEN));

      log_ptr = log_ptr + copy_len;
    }
    //Validate log copy
    if( ( log_ptr - base_log_ptr) <= (DS_MUX_LOGGING_MAX_STATE_LOG_LEN + DS_MUX_LOG_HDR_SIZE))
    {
      log_shorten(base_log_ptr, (uint32) (log_ptr - base_log_ptr));

      log_commit(base_log_ptr);
    }
    else
    {
        DS_MUX_LOG_MSG_ERROR_2( " ds_mux_logging_log_connection_status invalid copy "
                                " copied len %d  allowed len %d",
                                ( log_ptr - base_log_ptr),
                                DS_MUX_LOGGING_MAX_STATE_LOG_LEN + DS_MUX_LOG_HDR_SIZE);

      log_free(base_log_ptr);
    }
    log_free(base_log_ptr);
    return;
  }while(0);

}/* ds_mux_logging_log_connection_status */


