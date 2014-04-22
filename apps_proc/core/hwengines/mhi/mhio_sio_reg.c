/**
  @file mhi_sio_reg.c
  @brief
  This file contains the implementation of the MHI SIO adaptation layer 
  registration logic  

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

/** MHI_SIO_INIT_DRV_IFACE 
 * Init driver iface help macro 
 *
 */
#define MHI_SIO_INIT_DRV_IFACE(DRIVER_IFACE,OPEN, CLOSE,IOCTL,TRANSMIT, \
                                 FLUSH_TX,DISABLE,CTRL_OPEN,CTRL_CLOSE, \
                                 CTRL_IOCTL,CTRL_TRANSMIT)  \
                                 (DRIVER_IFACE).HW_open=(OPEN); \
                                 (DRIVER_IFACE).HW_close=(CLOSE); \
                                 (DRIVER_IFACE).HW_ioctl=(IOCTL); \
                                 (DRIVER_IFACE).HW_transmit=(TRANSMIT);  \
                                 (DRIVER_IFACE).HW_flush_tx=(FLUSH_TX);      \
                                 (DRIVER_IFACE).HW_disable_device =(DISABLE);        \
                                 (DRIVER_IFACE).HW_control_open=(CTRL_OPEN); \
                                 (DRIVER_IFACE).HW_control_close=(CTRL_CLOSE); \
                                 (DRIVER_IFACE).HW_control_ioctl=(CTRL_IOCTL); \
                                 (DRIVER_IFACE).HW_control_transmit=(CTRL_TRANSMIT);

/** MHI_SIO_INIT_DEV_IFACE 
 * Init device iface help macro 
 *
 */
#define MHI_SIO_INIT_DEV_IFACE(DEVICE_IFACE,DRV_IFACE,DEV_ID,FWD_PORT,FWD_PORT_ID,DRV_AVAIL) \
                                     (DEVICE_IFACE).driver_interface=(DRV_IFACE); \
                                     (DEVICE_IFACE).device_id=(DEV_ID); \
                                     (DEVICE_IFACE).forward_port=(FWD_PORT); \
                                     (DEVICE_IFACE).forwarD_port_id=(FWD_PORT_ID); \
                                     (DEVICE_IFACE).driver_available=(DRV_AVAIL); 

//TODO: move this assert to a mhi private header
#define MHI_ASSERT()  


/**
 * @brief      mhi_sio_register_driver
 *
 * Register MHI Driver with SIO   
 *              
 * @param[in]   register_driver_fn  Callback to be used by this function 
 *                                  to register driver with sio 
 *
 * @dependencies
 *              sio_init() must have been first called before this function  
 * 
 * @return       
 *              None
 * 
 * @sideeffect  None 
*/

void mhi_sio_register_driver(sio_register_device_driver_cb_type register_driver_fn)
{
  sio_driver_interface driver_iface;
  sio_device_interface device_iface;
  uint32 ret;

  MHI_SIO_INIT_DRV_IFACE(driver_iface , mhi_sio_ser_open, mhi_sio_ser_close, mhio_sio_ser_ioctl,
    mhio_sio_ser_transmit, mhi_sio_ser_flush_tx, NULL, mhi_sio_ser_ctrl_open, mhi_sio_ser_ctrl_close, 
    mhi_sio_ser_ctrl_ioctl, mhi_sio_ser_ctrl_transmit);

#ifndef FEATURE_HAS_NO_MHI_CORE
  MHI_SIO_INIT_DEV_IFACE(device_iface,&driver_iface,DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI), 
                           FALSE /*fwd port */,SIO_PORT_NULL,TRUE /* driver avail */);
#else
  MHI_SIO_INIT_DEV_IFACE(device_iface,&driver_iface,DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI), 
                           TRUE /*fwd port */,SIO_PORT_NULL,FALSE /* driver avail */);
#endif
 
 /* register primary MHI serial device with SIO */
 ret = register_driver_fn(device_iface);
 MHI_ASSERT(ret); 
  
/* MHI_SER1/DIAG */
#if defined (SIO_USES_MHI) && (defined (FEATURE_MHI_DIAG) 
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_SER1);
  ret = register_driver_fn(device_iface);
  MHI_ASSERT(ret); 
#endif 

 /* MHI_SER2 */
#if defined (FEATURE_MHI_DIAG_NMEA) 
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_SER2);
  ret = register_driver_fn(device_iface);
  MHI_ASSERT(ret); 
#endif 

 /* MHI_SER3 */
#if defined(SIO_HAS_MHI_SER3) 
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_SER3);
  ret = register_driver_fn(device_iface);
  MHI_ASSERT(ret);
#endif

/* MHI_SER4 */
#ifdef FEATURE_MHI_SER4_PORT
#if defined(SIO_HAS_MHI_SER4) 
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_SER4);
  ret = register_driver_fn(device_iface);
  MHI_ASSERT(ret);
#endif 
#endif 
     
/* RM_NET */
#ifdef FEATURE_MHI_ECM
  MHI_SIO_INIT_DRV_IFACE(driver_iface , mhi_sio_ecm_open, mhi_sio_ecm_close, mhio_sio_ecm_ioctl,
        mhio_sio_ecm_transmit, mhi_sio_ecm_flush_tx, NULL /* disable device */, mhi_sio_ecm_ctrl_open, 
        mhi_sio_reg_ecm_ctrl_close, mhi_sio_ecm_ctrl_ioctl, mhi_sio_reg_ecm_ctrl_transmit);

#ifndef FEATURE_HAS_NO_MHI_CORE
  MHI_SIO_INIT_DEV_IFACE(device_iface,&driver_iface,DEVICE_ID(SIO_MAJOR_MHI_ECM, SIO_MINOR_MHI_ECM_NET_WWAN), 
                           FALSE /*fwd port */,SIO_PORT_NULL,TRUE /* driver avail */);
#else
  MHI_SIO_INIT_DEV_IFACE(device_iface,&driver_iface,DEVICE_ID(SIO_MAJOR_MHI_ECM, SIO_MINOR_MHI_ECM_NET_WWAN), 
                           TRUE /*fwd port */,SIO_PORT_NULL,FALSE /* driver avail */);
#endif

  device_iface.BUFSIZ
  ret = register_driver_fn(device_iface);
  MHI_ASSERT(ret);
#endif /* FEATURE_MHI_ECM */

}








