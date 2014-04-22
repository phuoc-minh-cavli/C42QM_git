#ifndef GLINK_URPM_H
#define GLINK_URPM_H

/**
 * @file glink_urpm.h
 *
 * Public API for the GLink-RPM in uImage
 */

/** \defgroup glink GLink
 * \ingroup SMD
 *
 * GLink reliable, in-order, datagram-based interprocessor communication
 * over a set of supported transport (Shared Memory, UART, BAM, HSIC)
 *
 * All ports preserve message boundaries across the interprocessor channel; one
 * write into the port exactly matches one read from the port.
 */
/*@{*/

/*==============================================================================
     Copyright (c) 2015,2018 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.10/api/mproc/glink_urpm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/02/15   an      Initial release of GLink uRPM API
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

/*================= RESTRICTED API ==========================*/
/** Note that these API are only supported for RPM client. No other
 * client may use these as a generic API as the underlying implementation
 * works only for RPM ports */

 
/** 
 * This API allows the RPM client to write data into Tx FIFO in uImage mode.
 *
 * @param[in]   buffer      pointer to the client buffer
 * @param[in]   size        size of the client buffer
 *
 * @return      number of bytes sent or 0 if FIFO had no enough space
 *
 * @sideeffects  None
 */
int glink_rpm_write(void* buffer, int size);

/** 
 * This API allows the RPM client to read data from Rx FIFO in uImage mode.
 *
 * @param[in]   buffer      pointer to the client buffer
 * @param[in]   size        size of the client buffer
 *
 * @return      positive value equal to the size of the data copied into 
                client buffer, 0 if there is no incoming data, 
                negative value equal to -size of incoming packet 
                if the client buffer is too small
 *
 * @sideeffects  None
 */
int glink_rpm_read(void* buffer, int size);


#ifdef __cplusplus
}
#endif

#endif //GLINK_URPM_H

