#ifndef MHI_CTRL_H
#define MHI_CTRL_H
/**
  @file mhi_ctrl.h
  @brief
  This file contains the internal data structures, definitions shared by the 
  various MHI driver layers 
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/19/13   nk      Created
===============================================================================
                   Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "mhi_osal.h" 

/* Number of destination ID's */
#define NUM_ID 128

/* Type of control message */ 
typedef enum _mhi_ctrl_msg_id
{
   MHI_CTRL_MSG_SET_LINE_STATE=16,  /**< RS-232 DTR & RTS Signals */ 
   MHI_CTRL_MSG_SET_SERIAL_STATE=17,     /**< RS-232 DCD. DSR. and RI signals */ 
   MHI_CTRL_MSG_ENCAPSULATED_CMD=18,     /**< Protocol dependent message */ 
}mhi_ctrl_msg_id_t; 

/** Control message header */ 
typedef PACKED struct _mhi_ctrl_msg_hdr
{
   uint32 preamble;  /**< Set to ASCII in little endian 'CTRL' */
   uint8  msgid;        /**< Type of message encapsulated by control message */
   uint8  res[3];  /**< Reserved */
   uint8  destid;       /**< Destination MHI Interface ID for this message */ 
   uint8  res2[3];  /**< Reserved */
   uint32 size;         /**< Size of encapsulated message */ 
   uint32 msg_start;    /**< First 4 bytes of message */ 
}mhi_ctrl_msg_hdr_t;

/** Control Line State */ 
typedef enum _mhi_ctrl_line_state
{
   MHI_CTRL_LINE_STATE_NULL=0L,     /**< Null default state */ 
   MHI_CTRL_LINE_STATE_DTR=1<<0,   /**< RS-232 Signal DTR. Signal originates from DTE to DCE */ 
   MHI_CTRL_LINE_STATE_RTS=1<<1,   /**< RS-232 Signal RTS. Signal originates from DTE to DCE */ 
}mhi_ctrl_line_state_t; 

/** Control Serial State */ 
typedef enum _mhi_ctrl_serial_state
{
   MHI_CTRL_SERIAL_STATE_NULL=0L,    /**< Null default state */  
   MHI_CTRL_SERIAL_STATE_DCD=1<<0, /**< RS-232 Signal DCD. Signal originates from DCE to DTE */ 
   MHI_CTRL_SERIAL_STATE_DSR=1<<1, /**< RS-232 Signal DSR. Signal originates from DCE to DTE */ 
   MHI_CTRL_SERIAL_STATE_RI=1<<3,  /**< RS-232 Signal RI.  Signal originates from DCE to DTE */ 
}mhi_ctrl_serial_state_t; 

/** Line/Serial state notification functions */ 
typedef void (*mhi_ctrl_notify_func)(void* userdata, mhi_ctrl_line_state_t msg );

/** Control Context */ 
typedef struct _mhi_ctrl_ctx
{
   mhi_osal_sync_type sync;                         /**< Synchronization for Control context */ 

   mhi_ctrl_line_state_t line_state[NUM_ID];    /**< Control line state copy for messages originating from DTE to DCE */ 
   mhi_ctrl_notify_func notify_func;                /**< Notification function on which to receive notifications for destination id's */
   void *userdata[NUM_ID];                          /**< userdata to be supplied with notification */ 
   uint32 notify_enable[NUM_ID/sizeof(uint32)];     /**< Bit mask of for which destination ID's notifications are enabled */ 
   void *ctrl_pool;                                 /**< Control Message pool used on RX path */ 
}mhi_ctrl_ctx_t;

/** 
 * @brief      mhi_ctrl_init
 *
 *             Initializes MHI control layer
 *              
 * 
 * @param[in]  *ctrl_ctx - Pointer to control context where to store info
 * @param[in]  notify_func - Notification function to be used for control state changes
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_ctrl_init(mhi_ctrl_ctx_t *ctrl_ctx, mhi_ctrl_notify_func notify_func);

/** 
 * @brief      mhi_ctrl_register_notify_cb
 *
 *             Register a control notification callback for a particular destination id
 *              
 * 
 * @param[in]  destid - Destination id with which to register a callback
 * @param[in]  *userdata - userdata to be supplied with callback
 *
 * @dependencies
 *             mhi_ctrl_init must be called first
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_ctrl_register_notify_cb(uint32 destid, void *userdata);

/** 
 * @brief      mhi_ctrl_deregister_notify_cb
 *
 *             De-register a notification callback with a particular destination id
 *              
 * 
 * @param[in]  destid - Destination id 
 *
 * @dependencies
 *             mhi_ctrl_init must be called first
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_ctrl_deregister_notify_cb(uint32 destid);

/** 
 * @brief      mhi_ctrl_send_line_state
 *
 *             Send line state
 *              
 * 
 * @param[in]  destid - Destination id associated with line state
 * @param[in]  msg - Line state
 *
 * @dependencies
 *             mhi_ctrl_init must be called first
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_ctrl_send_line_state(uint32 destid, mhi_ctrl_line_state_t msg);

/** 
 * @brief      mhi_ctrl_send_serial_state
 *
 *             Send control state
 *              
 * 
 * @param[in]  destid - Destination id associated with control state
 * @param[in]  msg - Control state
 *
 * @dependencies
 *             mhi_ctrl_init must be called first
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_ctrl_send_serial_state(uint32 destid, mhi_ctrl_serial_state_t msg);

/** 
 * @brief      mhi_ctrl_send_generic_cmd
 *
 *             Send a generic command
 *              
 * 
 * @param[in]  destid - Destination id 
 * @param[in]  *cmd - Pointer to command
 * @param[in]  size - Command size
 *
 * @dependencies
 *             mhi_ctrl_init must be called first
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_ctrl_send_generic_cmd(uint32 destid, uint8 *cmd, uint32 size);

#endif
