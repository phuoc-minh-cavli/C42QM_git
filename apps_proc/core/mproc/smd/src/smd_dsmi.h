#ifndef SMD_DSMI_H
#define SMD_DSMI_H

/*===========================================================================

        Shared Memory Driver Watermark Internal Header File


 Copyright (c) 2005-2013 by Qualcomm Technologies, Incorporated.  All Rights
 Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_dsmi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/30/15   db      Added SMD tx/rx count for debug purpose
09/25/13   bt      Change SMD_WM_GET_INFO macro to smd_wm_get_info with checks.
07/24/13   bt      Make SMD use secure SMEM partitions.
11/07/12   bt      Remove confusing unused SMD_WM_PACKET_HEADER_SIZE define.
09/04/12   rs      Removed fields for data mover.
05/29/12   pa      Add packet_header_prev to smd_wm_info_type.
06/15/11   bt      Added required header includes exposed by loopback usage.
02/11/11   bt      Removed old unused SMD profiling code.
01/06/11   bt      Removed SMD Block and Data Mover protocols.
07/21/10   tl      Implemented SMD API for SMD priority.
06/30/10   tl      Remove unused functions left over from earlier rewrite
11/11/09   hwu     Added smd_wm_memcpy_open_sz API.
10/12/09   bfc/rs  Removed control port muxing.
09/29/09   bfc/rs  Added variable sized FIFO API support.
06/10/09   bfc     CMI updates.
05/05/08   jlk     added wait flag to wm_info_type
10/18/07   hwu     Added channel_type smd_wm_info.
10/15/07   jlk     added smd profile item to smd_wm_info_type
07/23/07   ptm     Code optimizations.
04/10/07   ptm     Make packet header 5 words, unconditionally.
02/07/07   ptm     Add code to support DSM priority field.
01/15/07   bfc     Added a define for packet mode
01/09/07   bfc     Added SMD using DM
01/04/07   bfc     initial creation - derived from smd_wm.c.

===========================================================================*/

#include "smd_v.h"
#include "dsm.h"
#include "smd_dsm.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                     CONSTANT AND VARIABLE DECLARATIONS
===========================================================================*/
/* There are 5 words in every SMD packet header = 20 bytes. */
#define SMD_WM_PACKET_HEADER_BYTE_SIZE          (5 * sizeof(uint32))

#define SMD_WM_DEFAULT_MAX_PACKET_SIZE          (200 * 1024)
#define SMD_LOCAL_MEM_VECTOR_SZ                 30

typedef struct
{
  smd_port_id_type port_id;     /* Associated SMD port */

  /* DSM queue which contains the buffers to be transmitted */
  dsm_watermark_type *tx_queue;

  /* DSM item which contains the current buffer to be transmitted */
  dsm_item_type *tx_item_ptr;

  /* Remainder of current DSM chain to transmit */
  dsm_item_type *tx_item_ptr_next;

  /* Buffer for sending packet headers -
     see accessor macros above to see how the entries are used */
  uint32 *tx_packet_header;

  /* DSM queue which is given buffers that are received */
  dsm_watermark_type *rx_queue;

  /* Head of the receive chain or in case of streaming transfer the only
     DSM item to be returned to the client */
  dsm_item_type *rx_head_ptr;

  /* End of the receive chain */
  dsm_item_type *rx_tail_ptr;

  /* Buffer for receiving packet headers -
     see accessor macros above to see how the entries are used */
  uint32 *rx_packet_header;

  /* Buffer for previously received packet headers.
     For debug use. */
  uint32 *rx_packet_header_prev;

  /* TRUE when rx_packet_header is valid */
  boolean rx_header_valid;

  /* TRUE when the rx packet is being skipped */
  boolean rx_skip_packet;

  /* The size of the current dmov read operation */
  uint32 rx_read_pending_size;

  /* Number of bytes remaining in the current packet -
     not used for streaming transfer */
  uint32 bytes_remaining;

  /* Read callback function and data...may be NULL */
  smd_wm_read_callback_fn read_cb;
  uint32                  read_cb_data;

  /* Stream mode: just memcpy for now */
  smd_dataxfr_mode_type dataxfr_mode;

  /* Transfer flow type whether streaming or packet */
  smd_xfrflow_type xfrflow_type;

  /* Maximum packet size (if in packet mode) */
  uint32 max_packet_size;

  /* DSM pool id associated with this stream */
  dsm_mempool_id_enum_type dsm_pool_id;

  /* Close callback */
  smd_close_callback_fn close_cb;
  void                  *close_cb_data;

  /* Flush callback (for ioctl only) */
  smd_vv_func_ptr_type  flush_cb;

  /* DTR changed callback function pointer */
  smd_vv_func_ptr_type   dtr_callback_fn;

  /* extended DTR changed callback function pointer and data */
  smd_vu_func_ptr_type   dtr_callback_ext_fn;
  void                   *dtr_callback_ext_data;

  /* whether to wait to send packets */
  boolean wait;
  
  /* debug code to check number of tx/rx packets */
  uint32 smd_wm_debug_tx_pkt_cnt;
  uint32 smd_wm_debug_rx_pkt_cnt;
} smd_wm_info_type;

/* An imitation of SMD_SIO_NUM_PROCS for the SMD-DSM layer, to limit the 
 * necessary size of the 2D smd_wm_port_to_info pointer array.  Conveniently, 
 * the only remote_hosts that need to support SMD-DSM anymore are Apps, Modem, 
 * and ADSP, which happen to be smem_host_type{0:2}. */
#define SMD_DSM_NUM_PROCS 3

extern smd_wm_info_type *smd_wm_port_to_info[SMD_DSM_NUM_PROCS][SMD_NUM_CHANNELS];

/*===========================================================================

                  SMD Internal FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      smd_wm_memcpy_open_3

DESCRIPTION   This function opens a SMD port for memcpy mode

ARGUMENTS     port_name - The name of the port to be opened
              channel_type - The connection type
              cb_data - A pointer to the client callback data.
              param - SMD memcpy open parameters
              
RETURN VALUE  Returns the port id.
===========================================================================*/
smd_port_id_type smd_wm_memcpy_open_3( const char                *port_name,
                                       smd_channel_type           channel_type,
                                       void                      *cb_data,
                                       const smd_open_param_type *param);

/*===========================================================================
FUNCTION      smd_dsm_notify_write

DESCRIPTION   notifies the SMD task that TX data is pending

ARGUMENTS     port_id - the port id

RETURN VALUE

SIDE EFFECTS  Signal will not be set if TX data aggregation is
              active.  Called in client context.
===========================================================================*/
void smd_dsm_notify_write( smd_port_id_type port_id );

/*===========================================================================
FUNCTION      smd_wm_get_info

DESCRIPTION   Returns the smd_wm_port_to_info pointer for the port.

ARGUMENTS     port_id - SMD port to be used by this stream.

RETURN VALUE  Pointer to the associated smd_wm_info_type.

SIDE EFFECTS  Called in client and SMD context.  May Assert if port_id is
              invalid.
===========================================================================*/
smd_wm_info_type *smd_wm_get_info( smd_port_id_type port_id );

#ifdef __cplusplus
}
#endif

#endif /* SMD_DSMI_H */
