/**
 * @file tracer_packet.c
 *
 * This is used in order to measure IPC transmit performance
 */

/*==============================================================================
     Copyright (c) 2015,2018 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/31/15   an      Add getters to packet tracer
07/01/15   bc      Remove unnecessary header
02/04/15   bc      Initial version for tracer packet
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink_tracer.h"
#include "glink_tracer_internal.h"
#include "glink_os_utils.h"
#include "smem_list.h"

/*===========================================================================
                        MACRO DEFINITIONS
===========================================================================*/
/* Version of this tracer packet */
#define TRACER_PACKET_VERSION  1

/* Size of tracer packet header in number of words */
#define TRACER_PACKET_HEADER_SIZE ( sizeof( tracer_pkt_header_type ) )

#define TRACER_PACKET_EVENT_SIZE  ( sizeof( tracer_pkt_event_type ) )

/* Size for one trace event */
#define TRACER_PACKET_EVENT_WORD_SIZE ( TRACER_PACKET_EVENT_SIZE / 4 )

/* Minimum size of tracer packet user can choose.
 * This will only allow to trace 1 event 
 */
#define TRACER_PKT_MIN_SIZE    ( TRACER_PACKET_HEADER_SIZE + TRACER_PACKET_EVENT_SIZE )

/* Bit mask for LSB 32 bit timestamp */
#define LSB_32_BIT_MASK        ( 0x00000000ffffffff )

/* Bit mask for MSB 32 bit timestamp */
#define MSB_32_BIT_MASK        ( 0xffffffff00000000 )

/* 32 bit shift for timestamp */
#define TIMESTAMP_32_BIT_SHIFT ( 32 )

/* Tracer Packet word to byte*/
#define TRACER_PKT_WORD_TO_BYTE( word ) ( (word) * 4 )

/* Tracer Packet byte to word */
#define TRACER_PKT_BYTE_TO_WORD( byte ) ( (byte) / 4 )

/* Max packet private size (client cookie size) */
#define TRACER_PKT_MAX_PKT_PRIV_LEN  ( 12 )

/* Number of GLINK event bits */
#define TRACER_PKT_GLINK_EVENT_BITS  ( 31 )

/* Number of client event bits */
#define TRACER_PKT_CLIENT_EVENT_BITS ( 15 )

/* Max Event ID for G-Link */
#define TRACER_PKT_MAX_GLINK_EVENT_ID ( 255 )

/* Min valid Event ID */
#define TRACER_PKT_MIN_EVENT_ID ( 1 )

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/
typedef struct tracer_pkt_event
{
  uint16 event_id;
  uint16 reserved;
  uint32 timestamp;
  
}tracer_pkt_event_type;

typedef struct tracer_pkt_header
{
  uint16 version      : 4;
  uint16 reserved     : 8;
 
  /* Indicates whether subsystem ID or transport ID exists */
  uint16 id_present   : 1;
  
  /* Indicates whether QDSS tracing is enabled */
  uint16 qdss_enabled : 1;
  
  /* Length of client cookie */
  uint16 pkt_priv_len : 2;
  
  /* Length of packet to be traced (including header size) */
  uint16 packet_length;
  
  /* Offset of next event to be logged */
  uint16 event_offset;
  
  /* client event configuration bit mask */
  uint16 client_event_cfg_mask;
  
  /* IPC event configuration bit mask. For ex. G-Link.
   * currently only G-Link supports tracer packet */
  uint32 glink_event_cfg_mask;
  
  /* base timestamp 64 bit */
  uint32 base_timestamp[2];
 
  /* Client's private data */
  uint32 pkt_priv[3];
  
  /* Subsystem ID or Transport ID only valid when id_present bit is set */
  uint32 subsys_xport_id;
}tracer_pkt_header_type;

typedef struct glink_tracer_cached_timetick
{
  struct  glink_tracer_cached_timetick *next;
  
  uint32  channel_id;
  uint32  pkt_id;
  uint32  timetick;
  uint32  event_id;
}glink_tracer_cached_timetick_type;

/*===========================================================================
                              LOCAL DATA DEFINITIONS
===========================================================================*/
#ifdef GLINK_MEMCPY_PROFILE_ENABLED
#define TRACER_PKT_MAX_TX_TIMETICK_CACHE ( 50 )
#define TRACER_PKT_MAX_RX_TIMETICK_CACHE ( 2 )

static glink_tracer_cached_timetick_type  tx_timetick_list[TRACER_PKT_MAX_TX_TIMETICK_CACHE];
static uint32                             tx_timetick_list_index = 0;

/* For RX only need to cache twice */
static glink_tracer_cached_timetick_type  rx_timetick_list[TRACER_PKT_MAX_RX_TIMETICK_CACHE]; 
static uint32                             rx_timetick_list_index = 0;

static os_cs_type                        *timetick_list_cs = NULL;

static glink_tracer_cache_dir_type tracer_pkt_direction;

#endif

/*===========================================================================
                         LOCAL FUNCTION DEFINITIONS
===========================================================================*/

#ifdef GLINK_MEMCPY_PROFILE_ENABLED
/*===========================================================================
FUNCTION      glink_tracer_init_context
===========================================================================*/
/** 
 * Initialize context for glink_tracer
 * This is only called when to measure glink performance more 
 * ex. memcpy (tx/rx) and IPC time
 *
 * @return         None
 *
 * @sideeffects    None
 */
/*=========================================================================*/
static void glink_tracer_init_context( void )
{
  timetick_list_cs = glink_os_malloc( sizeof( os_cs_type ) );
  glink_os_cs_init( timetick_list_cs );
  tracer_pkt_direction = INVALID_DIR;
}
#endif

/*===========================================================================
                         PUBLIC FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      tracer_packet_init
===========================================================================*/
/** 
 * Initializes tracer packet. 
 *
 * @param[in/out]  tracer_pkt       Packet to trace. User is supposed to allocate
 *                                  the buffer with the size they choose to trace.
 *                                  Allocated buffer will be initialized
 *                                  by this function. The size needs to be greater
 *                                  than TRACER_PKT_MIN_SIZE  which is
 *                                  defined in glink_tracer.c(36 byte) if user
 *                                  plan to use qtimer tracing. 
 * 
 * @param[in]      pkt_len          length of tracer packet
 * 
 * @param[in]      init_config      tracer packet initialization config.
 *
 * @return         Standard tracer packet error code
 *
 * @sideeffects    None
 */
/*=========================================================================*/
tracer_pkt_result_type tracer_packet_init
(
  void                        *tracer_pkt,
  uint32                       packet_len,
  tracer_pkt_init_config_type *init_config
)
{
  tracer_pkt_header_type *header;
  unsigned long long      base_timestamp;

#ifdef GLINK_MEMCPY_PROFILE_ENABLED
  if( !timetick_list_cs )
  {
    glink_tracer_init_context();
  }
#endif
  
  if( !tracer_pkt )
  {
    return TRACER_PKT_NULL_PKT;
  }
  
  if( packet_len < TRACER_PKT_MIN_SIZE )
  {
    return TRACER_PKT_INVALID_PKT_SIZE;
  }
  
  if( TRACER_PKT_WORD_TO_BYTE( init_config->pkt_priv_len ) > 
      TRACER_PKT_MAX_PKT_PRIV_LEN )
  {
    return TRACER_PKT_STATUS_FAILURE;
  }
  
  memset( tracer_pkt, 0, packet_len );
  
  header = (tracer_pkt_header_type*)tracer_pkt;
  
  header->version         = TRACER_PACKET_VERSION;
  header->reserved        = 0;
  header->subsys_xport_id = 0; /* For now it's just 0 */
  
  header->qdss_enabled    = init_config->qdss_trace_enable;
  
  /* event is logged right after header */
  header->event_offset    = TRACER_PKT_BYTE_TO_WORD(TRACER_PACKET_HEADER_SIZE);
  header->packet_length   = (uint16)TRACER_PKT_BYTE_TO_WORD(packet_len);
  
  header->pkt_priv_len    = TRACER_PKT_BYTE_TO_WORD(TRACER_PKT_MAX_PKT_PRIV_LEN);
  
  glink_os_copy_mem( header->pkt_priv,
                     init_config->pkt_priv,
                     TRACER_PKT_MAX_PKT_PRIV_LEN );
  
  base_timestamp            = glink_os_get_current_timetick();
  header->base_timestamp[0] = ( base_timestamp >> TIMESTAMP_32_BIT_SHIFT );
  header->base_timestamp[1] = ( base_timestamp & LSB_32_BIT_MASK );

  header->client_event_cfg_mask = init_config->client_event_cfg;
  header->glink_event_cfg_mask    = init_config->glink_event_cfg;
  
  return TRACER_PKT_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      tracer_packet_set_event_cfg
===========================================================================*/
/**
 * Sets the trace event configuration in case user wants to trace assymetrical
 * events. 
 * For example, user may want to trace n number of events for transmission yet
 * m number of events for receive.
 * 
 * @param[in/out]  tracer_pkt       Packet to trace. 
 *                                  Packet must be initialized. 
 * 
 * @param[in]      client_event_cfg configuration for client defined event. 
 *                                  0 if not needed
 * 
 * @param[in]      glink_event_cfg  configuration for IPC event. 
 *                                  For ex. glink 
 *                                  (currently only glink supports tracer pkt)
 *                                  0 if not needed
 *
 * @return         Standard tracer packet error code
 *
 * @sideeffects    None
 */
/*=========================================================================*/
tracer_pkt_result_type tracer_packet_set_event_cfg
(
  void    *tracer_pkt,
  uint16   client_event_cfg,
  uint32   glink_event_cfg
)
{
  tracer_pkt_header_type *header;
  
  if( !tracer_pkt )
  {
    return TRACER_PKT_NULL_PKT;
  }
  
  header = (tracer_pkt_header_type *)tracer_pkt;
  
  if( header->version != TRACER_PACKET_VERSION )
  {
    return TRACER_PKT_STATUS_FAILURE;
  }
  
  header->client_event_cfg_mask = client_event_cfg;
  header->glink_event_cfg_mask  = glink_event_cfg;
  
  return TRACER_PKT_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      tracer_packet_log_event
===========================================================================*/
/**
 * Log the time stamp for the event given
 * 
 * @param[in/out]  tracer_pkt  Packet to trace. 
 *                             Packet must be initialized. 
 
 * @param[in]      event_id    event id to trace
 *
 * @return         Standard tracer packet error code
 *
 * @sideeffects    None
 */
/*=========================================================================*/
tracer_pkt_result_type tracer_packet_log_event
(
  void    *tracer_pkt,
  uint16   event_id
)
{
  tracer_pkt_header_type *header;
  tracer_pkt_event_type  *event_to_log;
  unsigned long long      timestamp;
  uint32                  event_cfg = 0;
  
  if( !tracer_pkt )
  {
    return TRACER_PKT_NULL_PKT;
  }
  
  header = (tracer_pkt_header_type *)tracer_pkt;
  
  if( header->version != TRACER_PACKET_VERSION )
  {
    return TRACER_PKT_STATUS_FAILURE;
  }
  
  if( header->event_offset + TRACER_PACKET_EVENT_WORD_SIZE > header->packet_length )
  {
    return TRACER_PKT_OUT_OF_EVENT_SPACE;
  }
  
  if( event_id < TRACER_PKT_MIN_EVENT_ID )
  {
    return TRACER_PKT_STATUS_FAILURE;
  }
  
  if( event_id <= TRACER_PKT_MAX_GLINK_EVENT_ID )
  {
    event_cfg = header->glink_event_cfg_mask & 
                ( 1 << ( ( event_id - 1 ) % TRACER_PKT_GLINK_EVENT_BITS ) ); 
  }
  else
  {
    event_cfg = header->client_event_cfg_mask & 
                ( 1 << ( ( event_id - 1 ) % TRACER_PKT_CLIENT_EVENT_BITS ) ); 
  }
  
  if( !event_cfg )
  {
    return TRACER_PKT_STATUS_SUCCESS;
  }
  
  event_to_log 
    = (tracer_pkt_event_type *)( (uint32 *)header + header->event_offset );

  timestamp               = glink_os_get_current_timetick();
  event_to_log->timestamp = timestamp & LSB_32_BIT_MASK;
  event_to_log->event_id  = event_id;
  
  header->event_offset += TRACER_PACKET_EVENT_WORD_SIZE;
  
  return TRACER_PKT_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      tracer_packet_calc_hex_dump_size
===========================================================================*/
/**
 * Calculate the size of traced packet in hexadecimal format
 * User can use return value to allocate buffer to save this dump
 * 
 * @param[in/out]  tracer_pkt  Packet traced
 
 * @param[in]      pkt_len     length of packet
 *
 * @return         size of tracer packet in hex format
 *
 * @sideeffects    None
 */
/*=========================================================================*/
uint32 tracer_packet_calc_hex_dump_size
(
  void    *tracer_pkt,
  uint32   pkt_len
)
{
  tracer_pkt_header_type *header;
  
  if( !tracer_pkt )
  {
    return TRACER_PKT_NULL_PKT;
  }

  header = (tracer_pkt_header_type *)tracer_pkt;
  
  if( (uint32)TRACER_PKT_WORD_TO_BYTE(header->packet_length) != pkt_len )
  { /* Currently exact dump format is undecided. just make sure given length
     * is same as the one used during initialization */
    ASSERT(0);
  }
  
  return TRACER_PKT_WORD_TO_BYTE(header->packet_length);
}

/*===========================================================================
FUNCTION      tracer_packet_calc_hex_dump_size
===========================================================================*/
/**
 * Dump the traced packet into given buffer. The buffer will be in hex format
 *
 * User should have called tracer_packet_calc_hex_dump_size and allocated
 * buffer based on return value before calling this
 * 
 * @param[in/out]  dest_buf        Buffer to dump to
 * 
 * @param[in]      dest_buf_len    length of dest_buf
 * 
 * @param[in/out]  traced_pkt      Packet traced
 * 
 * @param[in]      traced_pkt_len  length of traced packet
 * 
 * @return         Standard tracer packet error code
 *
 * @sideeffects    None
 */
/*=========================================================================*/
tracer_pkt_result_type tracer_packet_hex_dump
(
  void    *dest_buf,
  uint32   dest_buf_len,
  void    *traced_pkt,
  uint32   traced_pkt_len
)
{
  uint32 size = ( dest_buf_len <= traced_pkt_len ) ? dest_buf_len : traced_pkt_len;
  
  if( !dest_buf || !traced_pkt )
  {
    return TRACER_PKT_NULL_PKT;
  }
  
  glink_os_copy_mem( dest_buf, traced_pkt, size );
  return TRACER_PKT_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      tracer_packet_get_timestamp
===========================================================================*/
/**
 * This function extracts timestamp for the given event
 * 
 * @param[in]      traced_pkt      Packet traced
 * @param[in]      traced_pkt_len  length of traced packet
 * @param[in]      event_id        Event id
 * @param[out]     timestamp       Pointer to timestamp memory
 *
 * @return         Standard tracer packet error code
 *
 * @sideeffects    None
 */
/*=========================================================================*/
tracer_pkt_result_type tracer_packet_get_timestamp
(
  void    *traced_pkt,
  uint32   traced_pkt_len,
  uint16   event_id,
  uint64   *timestamp
)
{
  tracer_pkt_header_type *header;
  tracer_pkt_event_type  *pevent;
  
  if (!timestamp || !traced_pkt)
  {
    OS_LOG_ERROR( 2, "[tracer_packet_get_timestamp] Invalid parameters: %x, %x.", timestamp, traced_pkt );
    return TRACER_PKT_NULL_PKT;
  }
  
  header = (tracer_pkt_header_type *)traced_pkt;
  
  if (header->version != TRACER_PACKET_VERSION)
  {
    OS_LOG_ERROR( 2, "[tracer_packet_get_timestamp] Unsupported version: %d, %d.", header->version, TRACER_PACKET_VERSION );
    return TRACER_PKT_STATUS_FAILURE;
  }
  
  if (event_id < TRACER_PKT_MIN_EVENT_ID)
  {
    return TRACER_PKT_STATUS_FAILURE;
  }
  
  for (pevent = (tracer_pkt_event_type*)(header + 1);
       pevent < (tracer_pkt_event_type *)((uint32 *)header + header->event_offset);
       pevent++)
  {
    if (pevent->event_id == event_id)
    {
      *timestamp = ((uint64)header->base_timestamp[0]) << TIMESTAMP_32_BIT_SHIFT;
      *timestamp += (pevent->timestamp < header->base_timestamp[1] ? (1ULL << TIMESTAMP_32_BIT_SHIFT) : 0) + 
                    pevent->timestamp;
      return TRACER_PKT_STATUS_SUCCESS;
    }
  }

  OS_LOG_ERROR( 1, "[tracer_packet_get_timestamp] Event not found: %d.", event_id );
  return TRACER_PKT_STATUS_FAILURE;
}

/*===========================================================================
FUNCTION      tracer_packet_get_pkt_priv
===========================================================================*/
/**
 * This function extracts private data the given tracer packet
 * 
 * @param[in]      traced_pkt      Packet traced
 * @param[in]      traced_pkt_len  length of traced packet
 * @param[in]      index           Index of pkt_priv (must be less than 3)
 * @param[out]     value           Pointer to data memory
 *
 * @return         Standard tracer packet error code
 *
 * @sideeffects    None
 */
/*=========================================================================*/
tracer_pkt_result_type tracer_packet_get_pkt_priv
(
  void    *traced_pkt,
  uint32   traced_pkt_len,
  uint32   index,
  uint32   *value
)
{
  tracer_pkt_header_type *header;
  
  if (!value || !traced_pkt || index >= 3)
  {
    OS_LOG_ERROR( 2, "[tracer_packet_get_pkt_priv] Invalid parameters: %x, %x.", value, traced_pkt );
    return TRACER_PKT_NULL_PKT;
  }
  
  header = (tracer_pkt_header_type *)traced_pkt;
  
  if (header->version != TRACER_PACKET_VERSION)
  {
    OS_LOG_ERROR( 2, "[tracer_packet_get_pkt_priv] Unsupported version: %d, %d.", header->version, TRACER_PACKET_VERSION );
    return TRACER_PKT_STATUS_FAILURE;
  }

  *value = header->pkt_priv[index];
 
  return TRACER_PKT_STATUS_SUCCESS;
}


/*===========================================================================
                   GLINK PROTECTED FUNCTION DEFINITIONS
===========================================================================*/

#ifdef GLINK_MEMCPY_PROFILE_ENABLED
/*===========================================================================
FUNCTION      tracer_packet_cache_rx_timetick
===========================================================================*/
/** 
 * Cache timetick in RX side. This must be only called in rx side
 * Cached timetick will be used to capture exact rx complete time
 *
 * @param[in]  event_id     event_id to cache
 * 
 * @return         None
 *
 * @sideeffects    None
 */
/*=========================================================================*/
void tracer_packet_cache_rx_timetick
(
  uint32   event_id
)
{
  glink_tracer_cached_timetick_type *cached_timetick;
  
  if( !timetick_list_cs )
  {
    glink_tracer_init_context();
  }
  
  if( tracer_pkt_direction == TX )
  {
    return;
  }
  
  glink_os_cs_acquire( timetick_list_cs );
  
  tracer_pkt_direction = RX;
  
  if( rx_timetick_list_index == TRACER_PKT_MAX_TX_TIMETICK_CACHE )
  {
    OS_LOG_MSG(1, "GLINK TRACER: out of space for caching tx timetick");
    return;
  }
  
  cached_timetick = &rx_timetick_list[rx_timetick_list_index];
  cached_timetick->event_id = event_id;
  cached_timetick->timetick = glink_os_get_current_timetick();
  ++rx_timetick_list_index;

  glink_os_cs_release( timetick_list_cs );
}

/*===========================================================================
FUNCTION      tracer_packet_cache_tx_timetick
===========================================================================*/
/** 
 * Cache timetick in TX side. This must be only called in tx side
 * Cached timetick will be used to capture exact tx complete time
 *
 * @param[in]  tracer_pkt   tracer packet. this is used to get packet priv info
 * @param[in]  event_id     event_id to cache
 * 
 * @return         None
 *
 * @sideeffects    None
 */
/*=========================================================================*/
void tracer_packet_cache_tx_timetick
(
  void    *tracer_pkt,
  uint32   event_id
)
{
  glink_tracer_cached_timetick_type *cached_timetick;
  tracer_pkt_header_type            *header;
  
  if( tracer_pkt_direction == RX )
  {
    return;
  }
  
  cached_timetick = tx_timetick_list;

  header = (tracer_pkt_header_type *)tracer_pkt;

  glink_os_cs_acquire( timetick_list_cs );
  
  tracer_pkt_direction = TX;
  
  if( tx_timetick_list_index == TRACER_PKT_MAX_TX_TIMETICK_CACHE )
  {
    OS_LOG_MSG(1, "GLINK TRACER: out of space for caching tx timetick");
    return;
  }
  
  cached_timetick[tx_timetick_list_index].event_id = event_id;
  cached_timetick[tx_timetick_list_index].timetick = glink_os_get_current_timetick();
  
  cached_timetick[tx_timetick_list_index].pkt_id
    = header->pkt_priv[GLINK_PROFILE_TRACER_PKT_ID];
  
  cached_timetick[tx_timetick_list_index].channel_id
    = header->pkt_priv[GLINK_PROFILE_TRACER_PKT_CH_ID];
  ++tx_timetick_list_index;
  
  glink_os_cs_release( timetick_list_cs );
}


/*===========================================================================
FUNCTION      tracer_packet_get_final_pkt
===========================================================================*/
/** 
 * Get tracer packet that is finally merged with cached packet.
 * Cached packet contains accurate timetick for GLINK_XPRT_TX
 * 
 * @param[in/out]  tracer_pkt   tracer_pkt
 * @param[in]      cache_dir    timetick cache direction 
 * 
 * @return         None
 *
 * @sideeffects    None
 */
/*=========================================================================*/
void tracer_packet_get_final_pkt
(
  void                        *tracer_pkt,
  glink_tracer_cache_dir_type  cache_dir
)
{
  tracer_pkt_event_type  *event;
  uint32                  offset;
  uint32                  i;

  tracer_pkt_header_type *header;
  
  glink_tracer_cached_timetick_type *cached_timetick;
  uint32                             timetick_list_index;
  
  if( cache_dir == TX )
  {
    cached_timetick     = tx_timetick_list;
    timetick_list_index = tx_timetick_list_index;
  }
  else if( cache_dir == RX )
  {
    if( tracer_pkt_direction == TX )
    {
      /* this internal functions are to measure one way direction only 
       * Since this is sender side you don't measure RX packets */
      return;
    }
    
    cached_timetick     = rx_timetick_list;
    timetick_list_index = rx_timetick_list_index;
  }
  else
  {
    ASSERT(0);
  }
  
  header = (tracer_pkt_header_type*)tracer_pkt;
  
  glink_os_cs_acquire( timetick_list_cs );

  for( i = 0; i < timetick_list_index; ++i )
  {
    if( cache_dir == TX &&
        (( cached_timetick[i].channel_id != 
          header->pkt_priv[GLINK_PROFILE_TRACER_PKT_CH_ID] ) ||
        ( cached_timetick[i].pkt_id !=
          header->pkt_priv[GLINK_PROFILE_TRACER_PKT_ID] ) ) )
    {
      continue;
    }
    
    event = (tracer_pkt_event_type *)( (char *)header + TRACER_PACKET_HEADER_SIZE );
    offset = TRACER_PKT_BYTE_TO_WORD(TRACER_PACKET_HEADER_SIZE);

    while ( event && ( offset < header->event_offset ) )
    {
      if( event->event_id == cached_timetick[i].event_id )
      {
        event->timestamp = cached_timetick[i].timetick;
        break;
      }
      
      ++event;
      offset += TRACER_PACKET_EVENT_WORD_SIZE;
    }
    
    if( offset == header->event_offset )
    {
      OS_LOG_MSG( 2,
                  "GLINK TRACER: timetick cached event not found[event id: %d]",
                  cached_timetick[i].event_id );
    }
  }
  
  if( cache_dir == RX )
  {
    /* Reset rx timetick list index. Reset only necessary for RX */
    rx_timetick_list_index = 0;
  }

  glink_os_cs_release( timetick_list_cs );
}

/*===========================================================================
FUNCTION      tracer_packet_log_timestamp
===========================================================================*/
/** 
 * Log tracer packet event to ulog
 * 
 * @param[in]  tracer_pkt   tracer_pkt 
 * 
 * @return         None
 *
 * @sideeffects    None
 */
/*=========================================================================*/
void tracer_packet_log_timestamp
(
  void     *tracer_pkt
)
{
  static os_cs_type      *log_timestamp_cs = NULL;
  static uint32           pkt_id = 0;
  
  tracer_pkt_header_type *header;
  tracer_pkt_event_type  *event_to_log;
  uint32                  offset;

  if( !log_timestamp_cs )
  {
    log_timestamp_cs = glink_os_calloc( sizeof( os_cs_type ) );
    glink_os_cs_init( log_timestamp_cs );
  }
  
  if( !tracer_pkt )
  {
    OS_LOG_MSG(1, "GLINK TRACER: log timestamp packet null");
    return;
  }
  
  header = (tracer_pkt_header_type *)tracer_pkt;
  
  if( header->version != TRACER_PACKET_VERSION )
  {
    OS_LOG_MSG(2, "GLINK TRACER: log timestamp version mismatch[header: %d]", header->version);
    return;
  }

  offset = TRACER_PKT_BYTE_TO_WORD(TRACER_PACKET_HEADER_SIZE);
  
  event_to_log 
    = (tracer_pkt_event_type *)( (char *)tracer_pkt + TRACER_PACKET_HEADER_SIZE );

  glink_os_cs_acquire( log_timestamp_cs );
  
  while( event_to_log && ( offset < header->event_offset ) )
  {
    OS_LOG_MSG(5, 
               "GLINK TRACER LOG: "
               "[tracer_pkt_id: %d][pkt_size: %d][event_id: %d][timestamp: 0x%x]",
               pkt_id, 
               TRACER_PKT_WORD_TO_BYTE(header->packet_length), 
               event_to_log->event_id, 
               event_to_log->timestamp ); 
 
    offset += TRACER_PACKET_EVENT_WORD_SIZE;
    ++event_to_log;
  }
  ++pkt_id;
  OS_LOG_MSG( 1, "GLINK TRACER: =============================================");
  glink_os_cs_release( log_timestamp_cs );
}
#endif