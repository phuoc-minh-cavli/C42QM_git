#ifndef DEVPRG_HSUART_H
#define DEVPRG_HSUART_H
/**************************************************************************
 * FILE: devprg_hsuart.h
 *
 * This file provides declaration of APIs that directly talk to hsuart driver APIs.
 *
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/DevPrgLib/devprg_hsuart.h#2 $
  $DateTime: 2020/01/28 12:49:31 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
07/01/20     as    Add enhancements in hsuart transport layer
02/25/19     as    Initial Creation.

===========================================================================*/

//#include "LoaderUtils.h"
/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

**  Function :  devprg_hsuart_poll

** ==========================================================================
*/
/*!
 * @brief   This API will poll on hsuart_poll before returning.
 * 
 * @return  boolean result returned by hsuart_poll.
 */
boolean devprg_hsuart_poll(void);

/*===========================================================================

**  Function :  devprg_hsuart_initialize

** ==========================================================================
*/
/*!
 * 
 * @brief   This API initializes calls for hsuart_initialize.
 * 
 * @return  Returns 0 if hsuart_initialize is success or else -1
 */
uint32 devprg_hsuart_initialize(void);

/*===========================================================================

**  Function :  devprg_hsuart_write

** ==========================================================================
*/
/*!
 * 
 * @brief   This API is a wrapper over hsuart_write.
 * 
 *
 * @param   write_buf -  [IN] Pointer to the buffer with data to be sent
 *          length    -  [IN] Number of bytes to be sent
 * @return  returns result returned by hsuart_write.
 */
uint32 devprg_hsuart_write(UINT8* write_buf, uint32 length);   

/*===========================================================================

**  Function :  devprg_hsuart_read

** ==========================================================================
*/
/*!
 * 
 * @brief   This API will keep on calling hsuart_read until requested length 
 *          is recieved.
 *
 * @param   read_buf   -   [OUT] Pointer to the buffer with data to be received
 *          length     -  [IN] Number of bytes to be received
 *          devprg_hsuart_rx_cb - the cb is invoked if the hsuart_read
 *                                reports an error while receiving data
 * 
 * @return  Return total bytes read from hsuart
 */
uint32 devprg_hsuart_read(uint8* read_buf, uint32 length, boolean fixed_length,
                          void (*devprg_hsuart_rx_cb)(void));

/*===========================================================================

**  Function :  devprg_hsuart_close

** ==========================================================================
*/
/*!
 * 
 * @brief   This API isa wrapper over hsuart_close
 */
void devprg_hsuart_close(void);

/*===========================================================================

**  Function :  devprg_hsuart_read_purge_buf

** ==========================================================================
*/
/*!
 * 
 * @brief   This API purges the rx buf of hsuart by reading it completely
 *          and discarding the read data
 *
 * @param   read_buf   -   [OUT] Pointer to the buffer with data to be received
 *          length     -  [IN] Number of bytes to be read
 *
 */
void devprg_hsuart_read_purge_buf(uint8* read_buf, uint32 length);

/*===========================================================================

**  Function :  devprg_hsuart_get_max_packet_size

** ==========================================================================
*/
/*!
 * 
 * @brief   This API is a wrapper over hsuart_get_max_packet_size
 * 
 * @return  returns return vlaue of hsuart_get_max_packet_size.
 */
uint32 devprg_hsuart_get_max_packet_size(void);
#endif /* DEVPRG_HSUART_H */ 