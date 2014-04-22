/** 
  @file mhi_sio_i.h
  @brief
  This header file contains the common internal definitions of MHI SIO adaptation
  layer 
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/15/13   nk      Created
===============================================================================
                   Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "comdef.h"
#include "sio.h"
/**
 * @brief      mhi_sio_ser_init
 *
 *             Initialize MHI SIO Serial device interface
 *              
 *
 * @dependencies
 *              
 * 
 * @return       
 *              MHI_SUCCESS if operation succeeded  
 * 
 * @sideeffect  None 
*/
uint32 mhi_sio_ser_init(void); 

/**
 * @brief     mhi_sio_ser_open 
 *
 *            Called by wrapper function sio_open. Opens a serial stream 
 *
 * @param[in]   *open_ptr - pointer to sio_open_type structure               
 *
 * @dependencies
 *              
 * 
 * @return       
 *              SIO_DONE_S if operation succeeded  
 * 
 * @sideeffect  None 
*/
sio_status_type mhi_sio_ser_open(sio_open_type *open_ptr);

/**
 * @brief     mhi_sio_ser_close 
 *
 *            Closes an opened serial stream
 *
 * @param[in]     stream_id - id of stream
 * @param[in]     port_id  -  id of port
 * @param[in]     close_done_cb  -  Pointer to callback function to be called when stream is closed
 *
 *
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None 
*/
void mhi_sio_ser_close(sio_stream_id_type stream_id, sio_port_id_type port_id, sio_vv_func_ptr_type close_done_cb);

/**
 * @brief     mhi_sio_ser_ioctl 
 *
 *            Serial stream control
 *              
 * @param[in] stream_id - id of stream
 * @param[in] port_id - id of port
 * @param[in] ioctl_cmd - ioctl command to perform
 * @param[in] *ioctl_param_ptr - pointer to ioctl parameters structure
 *
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None 
*/
void mhi_sio_ser_ioctl(sio_stream_id_type stream_id, sio_port_id_type port_id, sio_ioctl_cmd_type ioctl_cmd, sio_ioctl_param_type* ioctl_param_ptr);

/**
 * @brief     mhi_sio_ser_transmit 
 *
 *            Transmits data over the serial stream
 *
 * @param[in]   stream_id - id of stream
 * @param[in]   portid - id of port  
 * @param[in]   *tx_ptr - DSM item pointer containing data to be transmitted 
 * @param[in]   stream_id - id of stream
 *              
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None
 * 
 * @sideeffect  None 
*/
void mhi_sio_ser_transmit(sio_stream_id_type stream_id, sio_port_id_type port_id, dsm_item_type *tx_ptr);

/**
 * @brief     mhi_sio_ser_flush_tx 
 *
 *            Flushes any queued data to the device. The callback supplied is called when operation is finished.
 *              
 * @param[in]   stream_id - id of stream
 * @param[in]   portid - id of port  
 * @param[in]   flush_tx_done_cb - pointer to callback function 
 *
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None
 * 
 * @sideeffect  None 
*/
void mhi_sio_ser_flush_tx( sio_stream_id_type stream_id, sio_port_id_type port_id, sio_vv_func_ptr_type flush_tx_done_cb);

/**
 * @brief     mhi_sio_ser_ctrl_open 
 * 
 .            Opens a serial control stream
 * 
 * @param[in]   *open_ptr - pointer to sio_open_type structure               
 *
 * @dependencies
 *              
 * 
 * @return       
 *              SIO_DONE_S if operation succeeded  
 * 
 * @sideeffect  None 
*/

sio_status_type mhi_sio_ser_ctrl_open(sio_open_type *open_ptr);

/**
 * @brief     mhi_sio_ser_ctrl_close 
 *
 *            Closes an opened serial stream
 *              
 *
 * @param[in]     stream_id - id of stream
 * @param[in]     port_id  -  id of port
 * @param[in]     close_done_cb  -  Pointer to callback function to be called when stream is closed
 *
 *
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None 
*/
void mhi_sio_ser_ctrl_close(sio_stream_id_type stream_id, sio_port_id_type port_id, sio_vv_func_ptr_type close_done_cb);

/**
 * @brief     mhi_sio_ser_ctrl_ioctl 
 *
 *            Serial control stream control
 *              
 * @param[in] stream_id - id of stream
 * @param[in] port_id - id of port
 * @param[in] ioctl_cmd - ioctl command to perform
 * @param[in] *ioctl_param_ptr - pointer to ioctl parameters structure
 *
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None 
*/
void mhi_sio_ser_ctrl_ioctl(sio_stream_id_type stream_id, sio_port_id_type port_id, sio_ioctl_cmd_type ioctl_cmd, sio_ioctl_param_type *ioctl_param_ptr);

/**
 * @brief     mhi_sio_ser_ctrl_transmit 
 *
 *            Transmits data over the serial control stream
 *
 * @param[in]   stream_id - id of stream
 * @param[in]   portid - id of port  
 * @param[in]   *tx_ptr - DSM item pointer containing data to be transmitted 
 * @param[in]   stream_id - id of stream
 *              
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None
 * 
 * @sideeffect  None 
*/
void mhi_sio_ser_ctrl_transmit(sio_stream_id_type stream_id, sio_port_id_type port_id, dsm_item_type *tx_ptr);

/**
 * @brief      mhi_sio_ecm_init
 *
 *             Initialize MHI SIO ECM device interface
 *              
 *
 * @dependencies
 *              
 * 
 * @return       
 *              MHI_SUCCESS if operation succeeded  
 * 
 * @sideeffect  None 
*/
uint32 mhi_sio_ecm_init(void);

/**
 * @brief     mhi_sio_ecm_open 
 *
 *            Opens a serial stream
 *
 * @param[in]   *open_ptr - pointer to sio_open_type structure               
 *
 * @dependencies
 *              
 * 
 * @return       
 *              SIO_DONE_S if operation succeeded  
 * 
 * @sideeffect  None 
*/
sio_status_type mhi_sio_ecm_open(sio_open_type *open_ptr);

/**
 * @brief     mhi_sio_ecm_close 
 *
 *            Closes an opened serial stream
 *
 * @param[in]     stream_id - id of stream
 * @param[in]     port_id  -  id of port
 * @param[in]     close_done_cb  -  Pointer to callback function to be called when stream is closed
 *
 *
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None 
*/
void mhi_sio_ecm_close(sio_stream_id_type stream_id, sio_port_id_type port_id, sio_vv_func_ptr_type close_done_cb);

/**
 * @brief     mhi_sio_ecm_ioctl 
 *
 *            Serial stream control
 *              
 * @param[in] stream_id - id of stream
 * @param[in] port_id - id of port
 * @param[in] ioctl_cmd - ioctl command to perform
 * @param[in] *ioctl_param_ptr - pointer to ioctl parameters structure
 *
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None 
*/
void mhi_sio_ecm_ioctl(sio_stream_id_type stream_id, sio_port_id_type port_id, sio_ioctl_cmd_type ioctl_cmd, sio_ioctl_param_type* ioctl_param_ptr);

/**
 * @brief     mhi_sio_ecm_transmit 
 *
 *            Transmits data over the serial stream
 *
 * @param[in]   stream_id - id of stream
 * @param[in]   portid - id of port  
 * @param[in]   *tx_ptr - dsm item pointer containing data to be transmitted 
 * @param[in]   stream_id - id of stream
 *              
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None
 * 
 * @sideeffect  None 
*/
void mhi_sio_ecm_transmit(sio_stream_id_type stream_id, sio_port_id_type port_id, dsm_item_type *tx_ptr);

/**
 * @brief     mhi_sio_ecm_flush_tx 
 *
 *            Flushes any queued data to the device. The callback supplied is called when operation is finished.
 *              
 * @param[in]   stream_id - id of stream
 * @param[in]   portid - id of port  
 * @param[in]   flush_tx_done_cb - pointer to callback function 
 *
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None
 * 
 * @sideeffect  None 
*/
void mhi_sio_ecm_flush_tx( sio_stream_id_type stream_id, sio_port_id_type port_id, sio_vv_func_ptr_type flush_tx_done_cb);

/**
 * @brief     mhi_sio_ecm_ctrl_open 
 * 
 .            Opens a serial control stream
 * 
 * @param[in]   *open_ptr - pointer to sio_open_type structure               
 *
 * @dependencies
 *              
 * 
 * @return       
 *              SIO_DONE_S if operation succeeded  
 * 
 * @sideeffect  None 
*/
sio_status_type mhi_sio_ecm_ctrl_open(sio_open_type *open_ptr);

/**
 * @brief     mhi_sio_ecm_ctrl_close 
 *
 *            Closes an opened serial stream
 *              
 *
 * @param[in]     stream_id - id of stream
 * @param[in]     port_id  -  id of port
 * @param[in]     close_done_cb  -  Pointer to callback function to be called when stream is closed
 *
 *
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None 
*/
void mhi_sio_ecm_ctrl_close(sio_stream_id_type stream_id, sio_port_id_type port_id, sio_vv_func_ptr_type close_done_cb);

/**
 * @brief     mhi_sio_ecm_ctrl_ioctl 
 *
 *            Serial control stream control
 *              
 * @param[in] stream_id - id of stream
 * @param[in] port_id - id of port
 * @param[in] ioctl_cmd - ioctl command to perform
 * @param[in] *ioctl_param_ptr - pointer to ioctl parameters structure
 *
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None 
*/
void mhi_sio_ecm_ctrl_ioctl(sio_stream_id_type stream_id, sio_port_id_type port_id, sio_ioctl_cmd_type ioctl_cmd, sio_ioctl_param_type *ioctl_param_ptr);

/**
 * @brief     mhi_sio_ecm_ctrl_transmit 
 *
 *            Transmits data over the serial control stream
 *
 * @param[in]   stream_id - id of stream
 * @param[in]   portid - id of port  
 * @param[in]   *tx_ptr - dsm item pointer containing data to be transmitted 
 * @param[in]   stream_id - id of stream
 *              
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None
 * 
 * @sideeffect  None 
*/
void mhi_sio_ecm_ctrl_transmit(sio_stream_id_type stream_id, sio_port_id_type port_id, dsm_item_type *tx_ptr);



