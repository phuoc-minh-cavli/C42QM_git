#ifdef DIAG_CONSUMER_API

/*==========================================================================

              Diagnostic Communications Layer Header

General Description
  API declarations for the diagnostic protocol transport layer for DCI.


Copyright (c) 2014-2016, 2019 by Qualcomm Technologies, Incorporated. All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                           Edit History



when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/10/19   kdey    Porting of Qsockets
07/20/16   nk      Changes to support ThreadX
10/08/14   xy      Implement DCI discovery and DIAG DCI over PCIe 
01/07/14   xy      Created 
===========================================================================*/
#ifndef DIAGCOMM_DCI_H_
#define DIAGCOMM_DCI_H_

#include "comdef.h"

#if !defined(FEATURE_WINCE)
#include "target.h"
#endif

#include "diagcomm_v.h"
#include "diagcomm_io.h"
#include "diagi_v.h"
/* ------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------ */
#ifdef __cplusplus
	extern "C" {
#endif


uint32 diagcomm_smd_get_dci_rx_wmq_cnt( diagcomm_enum_port_type port_num );

void diag_dci_dtr_enable( diagcomm_io_conn_type * conn );
void diagcomm_smd_dci_init( diagcomm_enum_port_type port_num );
void diagcomm_smd_dci_conn_changed( diagcomm_io_conn_type * conn, boolean connected );
void diagcomm_dci_close ( diagcomm_enum_port_type port_num );
void diag_fwd_notify_dci( void );
void diag_process_rx_fusion_dci (void);
void diagcomm_sio_fusion_dci_inbound_pkt_notify (dsm_item_type ** item_ptr);
void * diagcomm_inbound_fusion (unsigned int *stream_len);
void diag_send_conn_status_fusion_dci(boolean connected);
void diagcomm_smd_dci_close_done_cb(void);
void diagcomm_smd_fusion_process_slave_tx(diagcomm_enum_port_type port_num);
void diagcomm_smd_dci_set_port(diagcomm_io_conn_type * conn);
void diag_send_handshake_fusion_dci(void * rev_pkt, uint16 pkt_len);

PACK(void *) 
diagpkt_err_dci_rsp(diagpkt_cmd_code_type code, PACK(void *) req_pkt, 
                  uint16 req_len);
void diagcomm_smd_dci_send( void *req_pkt, uint16 pkt_len, 
                        diagcomm_enum_port_type port_num );

#if defined (DIAG_FRAMING)&& !defined (FEATURE_DIAG_SWITCH)
static boolean diag_get_rx_pkt_ctrl_dci (diag_rx_pkt_type* rx_pkt);
#endif

#ifdef DIAG_SIO_SUPPORT
void diagcomm_smd_dci_open_init( diagcomm_io_conn_type * conn );
#endif /* DIAG_SIO_SUPPORT */
/*===========================================================================

FUNCTION DIAGCOMM_DCI_OPEN

DESCRIPTION
  Opens the DCI port.

===========================================================================*/
boolean diagcomm_dci_open ( diagcomm_enum_port_type port_num );

/*===========================================================================

FUNCTION DIAGCOMM_DCI_CMD_OPEN

DESCRIPTION
  Opens the DCI CMD port.

===========================================================================*/
boolean diagcomm_dci_cmd_open (diagcomm_enum_port_type port_num);

/*===========================================================================

FUNCTION DIAGPKT_PROCESS_REQUEST_FUSION_DCI

DESCRIPTION

The function process the ctrl packet received from APQ side through HSIC port
===========================================================================*/
void 
diagpkt_process_request_fusion_dci (void *req_pkt, uint16 pkt_len);
#ifdef __cplusplus
	}
#endif

#endif /* DIAGCOMM_DCI_H_ */

#endif /* #ifdef DIAG_CONSUMER_API */

