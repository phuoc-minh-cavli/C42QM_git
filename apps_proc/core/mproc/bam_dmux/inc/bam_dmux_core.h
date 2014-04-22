/*=============================================================================

                            BAM DMUX CORE

 BAM Data Multiplexer core

 Copyright  2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/bam_dmux/inc/bam_dmux_core.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/16   rv      Initial version.
===========================================================================*/

#ifndef BAM_DMUX_CORE_H
#define BAM_DMUX_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"
#include "bam_dmux_power.h"

/*=============================================================================

                         MACRO DECLARATIONS

=============================================================================*/


/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/


/*=============================================================================

                     PUBLIC VARIABLE DECLARATIONS

=============================================================================*/


/*=============================================================================

                    PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================
  FUNCTION  bam_dmux_power_notify
=============================================================================*/
/**
 * This function is called by the Power layer to Core layer to notify the 
 * A2 Power state event.
 *
 * @param[in]  event      Power event.
 *
 */
/*===========================================================================*/
void bam_dmux_power_notify(bam_dmux_power_event_type event);

/*=============================================================================
  FUNCTION  bam_dmux_bam_tx_done
=============================================================================*/
/**
 * This function is called by the BAM interface layer to Core layer when 
 * TX(UL) transfer is completed.
 *
 * @param[in]  buf_pa      Buffer physical address.
 * @param[in]  buf_len     Buffer transfered length.
 * @param[in]  priv        Transfers private context.
 *
 */
/*===========================================================================*/
void bam_dmux_bam_tx_done
(
  uint32 buf_pa,
  uint16 buf_len,
  void   *priv
);

/*=============================================================================
  FUNCTION  bam_dmux_bam_rx_done
=============================================================================*/
/**
 * This function is called by the BAM interface layer to Core layer when 
 * RX(DL) transfer is completed.
 * This function handles the received command or data from A2.
 *
 * @param[in]  buf_pa      Buffer physical address.
 * @param[in]  rx_len      received buffer length.
 * @param[in]  priv        Transfers private context.
 *
 */
/*===========================================================================*/
void bam_dmux_bam_rx_done
(
  uint32 buf_pa,
  uint16 rx_len,
  void   *priv
);

#ifdef __cplusplus
}
#endif

#endif /* BAM_DMUX_CORE_H */
