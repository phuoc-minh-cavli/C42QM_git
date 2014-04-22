#ifndef DIAGCOMM_SMD_H 
#define DIAGCOMM_SMD_H
/*==========================================================================

              Diagnostic Communications Layer Header

General Description
  API declarations for the diagnostic protocol transport layer.
  
Copyright (c) 2000-2014,2016, 2019 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                           Edit History
                           
 $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diagcomm_smd.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/10/19   kdey    Porting of Qsockets
07/20/16   nk      Changes to support ThreadX
02/26/14   sa      Added support for compression in buffering mode.
10/01/13   xy      Removed code under FEATURE_WINCE 
02/27/13   sr      Added support to HDLC encode the fwd channel traffic 
                   on APPS 
05/15/12   is      Allow "build time" mask queries from Master Diag
11/04/11   is      Support for buffered threshold mode
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
03/04/11   is      Support for Diag over SMD-Lite
09/14/10   is      New control channel and central routing features 
08/10/10   sg      Close SMD Port when the SIO port on apps is closed
05/13/10   JV      Merge functionality in diagcomm_smd.c and diagcomm_smd2.c
                   in a single file
10/01/09   JV      Removed inclusion of diagcomm.h
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/31/09   JV      Merged Q6 diag code back to mainline
07/17/09   mad     Featurized includion of customer.h and target.h for WM.
05/07/09   vk      added internal header files includes for CMI cleanup
04/10/03    wx     Move qcdmg and to_data_mode functions from RDM to here.
10/24/01   jal     Added ability to flush transmit channel
04/06/01   lad     Introduces types for callback function pointers.
                   Moved ASYNC char definitions from diagi.h.
02/23/01   lad     Created file.

===========================================================================*/

#include "comdef.h"

#include "customer.h"
#include "target.h"


#include "diagcomm_v.h"
#include "diagcomm_io.h"


/* ------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------ */

#ifdef __cplusplus
	extern "C" {
#endif

/* Generic - SMD specific */
void diagcomm_smd_init( diagcomm_enum_port_type port_num );
boolean diagcomm_smd_open( diagcomm_enum_port_type port_num );
void diagcomm_smd_close( diagcomm_enum_port_type port_num );
boolean diagcomm_smd_status( diagcomm_enum_port_type port_num );
void diagcomm_smd_close_done_cb( void );

/* SIO/SMD specific */
#ifdef DIAG_SIO_SUPPORT
void diagcomm_smd_open_init( diagcomm_io_conn_type * conn );
#endif /* DIAG_SIO_SUPPORT */

boolean diag_buffered_mode( void );
#if defined(DIAG_BUFFERING_SUPPORT)
void diagcomm_smd_tx_mode_init( void );
extern void diag_update_mode_info (uint8 mode);
#endif

void diag_fwd_notify( void );
void diagcomm_enqueue_dsm (dsm_item_type ** item_ptr);

void diagcomm_enqueue_dsm_dci (dsm_item_type ** item_ptr);


extern uint32 diagcomm_smd_get_rx_wmq_cnt( diagcomm_enum_port_type port_num );
extern void diagcompressedbuf_drain(void);
extern uint32 diagcomm_smd_process_slave_tx( diagcomm_enum_port_type port_num );


#ifdef __cplusplus
	}
#endif

        /*DSM item app_field values*/
#define DIAG_FWD_HDLC_ON    0x00000001

#endif /* DIAGCOMM_SMD_H  */
