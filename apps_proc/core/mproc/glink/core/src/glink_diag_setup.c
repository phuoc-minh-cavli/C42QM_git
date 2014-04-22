/*===========================================================================

                 GLINK internal diag commands support


 Copyright (c) 2015,2018 by Qualcomm Technologies, Inc.
 All Rights Reserved.
===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/03/15   db      Changed struct/union type from anonymous to named type.
06/19/15   pp      Initial version (Support for logging configuration) 
===========================================================================*/
/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "diagdiag.h"
#include "glink_internal.h"

/*===========================================================================
                       MACRO DEFINITION
===========================================================================*/

/* Commands supported */
#define GLINK_DIAG_OPTION_XPORT_FILTERING   0
#define GLINK_DIAG_OPTION_CH_NAME_FILTERING 1
#define GLINK_DIAG_OPTION_CH_STATS          2

/* Diag command to configure logging (2048 is being used by smdl_profile.c)*/
#if defined(MPROC_LPASS_PROC) || defined(MPROC_ADSP_PROC)
#define GLINK_LOGGING_DIAG_CMD_CODE 2049
#define GLINK_DIAG_PROC DIAG_QDSP6_PROC
  
#elif defined(MPROC_MODEM_PROC)
#define GLINK_LOGGING_DIAG_CMD_CODE 2050
#define GLINK_DIAG_PROC DIAG_MODEM_PROC
  
#elif defined(MPROC_SLPI_PROC)
#define GLINK_LOGGING_DIAG_CMD_CODE 2051
#define GLINK_DIAG_PROC DIAG_SLPI_PROC
  
#elif defined(MPROC_APPS_PROC)
#define GLINK_LOGGING_DIAG_CMD_CODE 2052
#define GLINK_DIAG_PROC DIAG_APP_PROC

#elif defined(MPROC_CDSP_PROC)
#define GLINK_LOGGING_DIAG_CMD_CODE 2053
#define GLINK_DIAG_PROC DIAG_CDSP_PROC

#endif

#if !(defined(GLINK_OS_DEFINED_LOGGING) || defined(GLINK_MEMORY_LOGGING))
#error "Logging is disabled - No need to add this file"
#endif
/*===========================================================================
                          GLOBAL DATA DECLARATIONS
===========================================================================*/
/* Forward declaration of callback routine */
PACK(void *) glinki_logging_diag_cb (PACK(void *) req_pkt, uint16 pkt_len);

static const diagpkt_user_table_entry_type glink_logging_rsp_tbl[] =
{
  {GLINK_LOGGING_DIAG_CMD_CODE, 
   GLINK_LOGGING_DIAG_CMD_CODE, 
   glinki_logging_diag_cb}
};

/*===========================================================================
                           TYPE DEFINITIONS
===========================================================================*/
/* Payload structure for channel based filtering */
PACKED struct channel_name_filter {
    uint8 remote_host;
    char channel_name[GLINK_CH_NAME_LEN];
};

/* Structure to interpret the byte stream received from QXDM commands to 
   configure logging */
typedef PACKED struct _glink_logging_diag_req {
  diagpkt_subsys_header_type header;
  uint8 req_type;
  PACKED union {
    uint8 xport_logging_mask;
    struct channel_name_filter ch_fltr;
  }filter;
}glink_logging_diag_req;


/*===========================================================================
FUNCTION      dump_channel_stats_from_xports
===========================================================================*/
/**
   Scan all channels within an xport and dump the current stats on Ulog.
   Callback routine from 
   
   @param[in]   if_ptr  interface pointer (Used)
   @param[in]   param1   unused
   @param[in]   param2   unused
   @param[in]   out      unused
*/
/*=========================================================================*/
static void dump_channel_stats_from_xports
( 
  glink_transport_if_type *if_ptr, 
  void* param1,
  uint32 param2,
  void** out 
)
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_core_xport_ctx_type *xport_ctx = if_ptr->glink_core_priv;
  
  glink_os_cs_acquire( &xport_ctx->channel_q_cs );
  
  open_ch_ctx = smem_list_first(&if_ptr->glink_core_priv->open_list);
  while ( open_ch_ctx != NULL ) 
  {
    glink_channel_stats_type stats = open_ch_ctx->ch_stats;
    OS_LOG_MSG( 7, "%s: TX[Bytes(req=%llu done=%llu) #(req=%u done=%u)]",
               open_ch_ctx->name,
               ULOG64_DATA(stats.tx_request_bytes), ULOG64_DATA(stats.tx_done_bytes),  
               stats.tx_request_count, stats.tx_done_count );
    OS_LOG_MSG( 7, "%s: RX[Bytes(req=%llu done=%llu) #(req=%u done=%u)]", 
               open_ch_ctx->name,
               ULOG64_DATA(stats.rx_notify_bytes),  ULOG64_DATA(stats.rx_done_bytes),
               stats.rx_notify_count,  stats.rx_done_count );
    open_ch_ctx = smem_list_next( open_ch_ctx );
  }
  
  glink_os_cs_release( &xport_ctx->channel_q_cs );
}

/*===========================================================================
FUNCTION      glinki_dump_channel_stats_to_ulog
===========================================================================*/
/**
   Scan all channels and dump the current stats on Ulog.
*/
/*=========================================================================*/
static void glinki_dump_channel_stats_to_ulog()
{
  int rm_host;
  /* for each remote host find all the */
  for ( rm_host = 0 ; rm_host < GLINK_NUM_HOSTS ; rm_host++ )
  {
    glinki_xports_for_each( glink_hosts_supported[rm_host],
                            dump_channel_stats_from_xports,
                            NULL, NULL, NULL );
  }
}


/*===========================================================================
FUNCTION      search_and_update_logging_filter
===========================================================================*/
/**
   Scan all open channels within an xport and update the logging filter with 
   corresponding info. 
   
   @param[in]   if_ptr   interface pointer (Used)
   @param[in]   param1   unused
   @param[in]   param2   unused
   @param[in]   out      unused
*/
/*=========================================================================*/
static void search_and_update_logging_filter
( 
  glink_transport_if_type *if_ptr, 
  void*  param1,
  uint32 param2,
  void** out
) 
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_core_xport_ctx_type *xport_ctx = if_ptr->glink_core_priv;
  
  glink_os_cs_acquire( &xport_ctx->channel_q_cs );
  
  open_ch_ctx = smem_list_first( &if_ptr->glink_core_priv->open_list );
  while (open_ch_ctx != NULL ) 
  {
    glinki_update_logging_filter( open_ch_ctx, FALSE );
    open_ch_ctx = smem_list_next( open_ch_ctx );
    if (log_filter_cfg.ch_ctx != NULL) 
    {
      break;
    }
  }
  glink_os_cs_release(&xport_ctx->channel_q_cs);
}
  
/*===========================================================================
FUNCTION      glinki_update_filter
===========================================================================*/
/**
   Scan all open channels and update the logging filter with corresponding
   info. 
   
   @return 0 when successful and -1 when fails to find the requested channel    
*/
/*=========================================================================*/  
static int32 glinki_update_filter()
{
  int rm_host;
  /* for each edge find all xports*/
  
  for ( rm_host = 0 ; rm_host < GLINK_NUM_HOSTS ; rm_host++ )
  {
    glinki_xports_for_each( glink_hosts_supported[rm_host],
                            search_and_update_logging_filter,
                            NULL, NULL, NULL );
    if (log_filter_cfg.ch_ctx != NULL) 
    {
      return 0;
    }
  }
  return 1;
}

/*===========================================================================
FUNCTION      glinki_logging_diag_cb
===========================================================================*/
/**
   The callback routine called when the user submits a QXDM command to 
configure the logging.

  @param[in]  req_pkt   The byte stream from the send_data command
  @param[in]  pkt_len   Length of the byte stream

  @return     In our case NULL.
*/
/*=========================================================================*/
PACK(void *) glinki_logging_diag_cb 
(
  PACK(void *) req_pkt,
  uint16       pkt_len
)
{
  glink_logging_diag_req *req = (glink_logging_diag_req*)req_pkt;
  void* rsp;
  int i =0 , result=1, payload_len;

  payload_len = pkt_len - (sizeof(diagpkt_subsys_header_type) + sizeof(uint8));
  
  if ( req != NULL && 
      payload_len >= 0 ) 
  {
    switch( req->req_type )
    {
    /* XPORT filtering packet */
    case GLINK_DIAG_OPTION_XPORT_FILTERING:
      if ( payload_len <= 0 ) 
      {
         break;
      }
      log_filter_cfg.xport_filter_mask = (uint32)(req->filter.xport_logging_mask);
      OS_LOG_MSG( 1,"Xport Filter = %x", log_filter_cfg.xport_filter_mask );
      result = 0;
      break;

    /* Channel name based filtering packet */
    case GLINK_DIAG_OPTION_CH_NAME_FILTERING:
      
      if( payload_len <= 0 ) 
      {
        break;
      }
      /* Reset the filter before updating the ch_name in filter*/
      glinki_update_logging_filter( log_filter_cfg.ch_ctx, TRUE );
      
      /* Set the remote host */
      log_filter_cfg.remote_host = (req->filter.ch_fltr).remote_host;
      
      /* Copy the channel name to filter */
      i=0;
      while ( ((req->filter.ch_fltr).channel_name[i] != '\0') && 
	     (i < GLINK_CH_NAME_LEN-1) && 
         (i < (payload_len-1)) ) 
      {
        log_filter_cfg.ch_name[i] = (req->filter.ch_fltr).channel_name[i];
        i++;
      }
      log_filter_cfg.ch_name[i] = '\0';
      log_filter_cfg.ch_filter_status = TRUE; 

      if ( glinki_update_filter() != 0 )
      {
        OS_LOG_MSG( 0, "Channel could not be found - resetting filter");
      }
      else 
      {
        OS_LOG_MSG( 2,"Channel Name Filter = %s ch_ctx=%x", 
                    log_filter_cfg.ch_name, log_filter_cfg.ch_ctx );
      }
      result = 0 ;
      break;

    /* Dump channel stats */
    case GLINK_DIAG_OPTION_CH_STATS:
      glinki_dump_channel_stats_to_ulog();
      result = 0;
      break;

    /* Unknown Command */
    default:
      break;
    }
  }

  if (result == 0)
  {
    rsp = (void *)diagpkt_subsys_alloc( DIAG_SUBSYS_COREBSP, 
                                        GLINK_LOGGING_DIAG_CMD_CODE, 
                                        sizeof(diagpkt_subsys_header_type) );
    diagpkt_commit(rsp);
  }
  else
  {
    /* send error response */
    uint8 *temp = (uint8*)req;
    diagpkt_cmd_code_type err_code = DIAG_BAD_PARM_F;

    OS_LOG_MSG( 2,"Error Parsing diag command - pkt_len=%d, req=%d", pkt_len,
               (uint32)req);
    if (temp != NULL)
    {
      for ( i=0; i< pkt_len ; i++ )
      {
        OS_LOG_MSG( 2, "PKT:%d->%d", i, temp[i]);
      }
    }
    rsp = (void *)diagpkt_err_rsp( err_code, req_pkt, pkt_len );
    diagpkt_commit(rsp);
  }

  return NULL;
}

/*===========================================================================
FUNCTION      glink_diag_init
===========================================================================*/
/**
  Function to register the Diag commands. Invoked by rcinit after glink and 
  diag systems are initialized
*/
/*=========================================================================*/
void glink_diag_init(void)
{
  /* Register diag command so that we can configure logging in runtime */

  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_PROC( GLINK_DIAG_PROC,  
                                           DIAG_SUBSYS_CMD_VER_2_F,
                                           DIAG_SUBSYS_COREBSP, 
                                           glink_logging_rsp_tbl ); 
}
