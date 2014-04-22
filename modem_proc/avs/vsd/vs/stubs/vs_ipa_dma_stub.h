/*!
  @file
  ipa_dma.h

  @brief
  This file contain the API of Q6 IPA driver for the DMA feature.

*/
/*===========================================================================

  Copyright (c) 2018 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/
/*===========================================================================*/

#ifndef VS_IPA_DMA_STUB_H
#define VS_IPA_DMA_STUB_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include "vs_ipa_stub.h"

/*==============================================================================

                           EXTERNAL TYPE DEFINITIONS

==============================================================================*/

/*! @brief IPA DMA mechanism state */
typedef enum {
  IPA_DMA_STATE_UNINITIALIZED = 0x0,

  /* IPA driver DMA state where clients may start calling open channel API.
  to open a new channel */
  IPA_DMA_STATE_OPEN_READY = 0x1,
  
  /* IPA driver DMA state, where clients may start to queue up transfers */
  IPA_DMA_STATE_XFR_READY = 0x2,
  
  IPA_DMA_STATE_MAX
} ipa_dma_state_e;

/*! @brief DMA client type */
typedef enum {
  IPA_DMA_CLIENT_TYPE_AUDIO_UL = 0x0,
  IPA_DMA_CLIENT_TYPE_AUDIO_DL = 0x1,
  IPA_DMA_CLIENT_TYPE_MHI_DATA = 0x2,
  IPA_DMA_CLIENT_TYPE_MHI_CTRL = 0x3,
  IPA_DMA_CLIENT_TYPE_MAX
} ipa_dma_client_type_e;

/*! @brief DMA client type */
typedef enum {
  IPA_DMA_CH_IRQ_MODE_TYPE_IRQ = 0x0,
  IPA_DMA_CH_IRQ_MODE_TYPE_POLLING = 0x1,
  IPA_DMA_CH_IRQ_MODE_TYPE_POLLING_IRQ = 0x2,
  IPA_DMA_CH_IRQ_MODE_TYPE_MAX
} ipa_dma_ch_irq_mode_type_e;

/*! @brief For each DMA client, pre-define max number of outstanding transfers */
#define IPA_DMA_MAX_OUTSTANDING_XFR_AUDIO_UL (2)
#define IPA_DMA_MAX_OUTSTANDING_XFR_AUDIO_DL (2)
#define IPA_DMA_MAX_OUTSTANDING_XFR_ASYNC_MHI (2)
#define IPA_DMA_MAX_OUTSTANDING_XFR_SYNC_MHI (1)

/*!
* @brief User callback function to be called for ASYNC operations
* On success, status is always IPA_SUCCESS.
* Note: Client implementation for callback should be slim and
*       without any sleep functionality such as mutex.
*/
typedef void(*ipa_dma_cb_func_t)(void *user_data, int32 status);

/*!
* @brief User callback function to be called for IPA DMA state change.
* status: On success, status is always IPA_SUCCESS.
*         No other status is expected to be returned.
* Note: Client implementation is from Interrupt service routine context.
*       Callback implementation must be slim and
*       without any sleep functionality such as mutex.
*/
typedef void(*ipa_dma_state_cb_func_t)(ipa_dma_state_e state, int32 status);

/*! @brief DMA register parameters */
typedef struct {
  ipa_dma_client_type_e client_type; /* */
  boolean is_async; /* TRUE for ASYNC channel, FALSE for sync channel */
  ipa_dma_ch_irq_mode_type_e ch_irq_mode_type; /* Set as IRQ, As only IRQ is supported for now. */
  uint32 size; /* Should be 0. for backward compatibility. Length of the FIFO, number of descriptors */
  ipa_dma_cb_func_t cb; /* For AYNC channel. Callback used to notify on transfer completion */
  ipa_dma_state_cb_func_t cb_state;  /* if non a NULL, this callback will be called to notify on state changes */
  uint32 options; /* 0 - no flags (for future growth) */
  uint32 ch_handle; /* out param */
} ipa_dma_ch_param_t;

/*!
* @ brief Max size of a single transfer is 8K
* IPA driver shall enforce this limitation.
* Related to ipa_dma_trans_t.size
*/
#define IPA_DMA_MAX_PER_XFR_SIZE (0x2000)

/*! @brief DMA transfer request parameters */
typedef struct {
  uint64 host_addr; /* host physical address */
  uint64 device_addr; /* device physical address */
  uint32 size; /* number of bytes to copy. ASSERT(size != 0 && size <= IPA_DMA_MAX_PER_XFR_SIZE) */
  uint32 options; /* 0 - no flags (for future growth) */
  
  /*! @brief 32b transfer descriptor (cookie) to uniquely identify a transfer.
  IPA driver uses this as an IN parameter calling client CB, which was set by client when calling ipa_dma_open.
  ASSERT(user_data != NULL) */
  void *user_data;
} ipa_dma_req_t;

/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS

==============================================================================*/
/*=============================================================================

  FUNCTION:  ipa_dma_state_get

=============================================================================*/
/*!
    @brief
    This function returns IPA DMA state.

    @return
    @retval IPA_SUCCESS        On success
    @retval IPA_ERR_FAIL       On unrecoverable error

    @dependencies
    None.
*/
int32 ipa_dma_state_get(ipa_dma_state_e *state);

/*=============================================================================

  FUNCTION:  ipa_dma_open

=============================================================================*/
/*!
    @brief
    This function is used to open a DMA channel.
 
    @param ch_param - request to register. See ipa_dma_ch_param_t for details.

    @return
    @retval IPA_SUCCESS        On success
    @retval IPA_ERR_FAIL       On unrecoverable error

    @dependencies
    This may be called only after IPA SW init is completed.
*/
int32 ipa_dma_open(ipa_dma_ch_param_t *ch_param);


/*=============================================================================

  FUNCTION:  ipa_dma_close

=============================================================================*/
/*!
    @brief
    This function is used to close DMA channel.

    @param handle - DMA channel handle received from ipa_dma_open function.

    @return
    @retval IPA_SUCCESS        On success
    @retval IPA_ERR_FAIL       On unrecoverable error

    @dependencies
    This may be called only after IPA SW init is completed.
*/
int32 ipa_dma_close(uint32 ch_handle);

/*=============================================================================

  FUNCTION:  ipa_dma_host_to_device

=============================================================================*/
/*!
    @brief
    This function is used to copy data from host space buffer to
    device space buffer. Transfer size limitations: IPA_DMA_MAX_PER_XFR_SIZE

    @param handle - DMA channel handle received from ipa_dma_open function.
    @param req - request to transfer. See ipa_dma_req_t for details.

    @return
    @retval IPA_SUCCESS            On successful queuing request
    @retval IPA_ERR_FAIL           On unrecoverable error
    @retval IPA_EDESC_FULL         On channel full

    @dependencies
    This may be called only after IPA is clocked, calling ipa_vote().
    Caller should ensure that this precondition is met.
*/
int32 ipa_dma_host_to_device(uint32 ch_handle, ipa_dma_req_t *req);

/*=============================================================================

  FUNCTION:  ipa_dma_device_to_host

=============================================================================*/
/*!
    @brief
    This function is used to copy data from device space buffer
    to host space buffer. Transfer size limitations: IPA_DMA_MAX_PER_XFR_SIZE

    @param handle - DMA channel handle received from ipa_dma_open function.
    @param req - request to transfer. See ipa_dma_req_t for details.

    @return
    @retval IPA_SUCCESS            On successful queuing request
    @retval IPA_ERR_FAIL           On unrecoverable error
    @retval IPA_EDESC_FULL         On channel full

    @dependencies
    This may be called only after IPA is clocked, calling ipa_vote().
    Caller should ensure that this precondition is met.
*/
int32 ipa_dma_device_to_host(uint32 ch_handle, ipa_dma_req_t *req);

#endif
