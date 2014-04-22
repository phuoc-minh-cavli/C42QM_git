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
12/01/14   nk      Add MHI DCI 
01/15/13   nk      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "mhi_sio_i.h"

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
                                     (DEVICE_IFACE).forward_port_id=(FWD_PORT_ID); \
                                     (DEVICE_IFACE).driver_available=(DRV_AVAIL); 

#define MHI_ASSERT(x)  

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

uint32 mhi_sio_register_driver(sio_register_device_driver_cb_type register_driver_fn)
{
  sio_driver_interface driver_iface;
  sio_device_interafce device_iface;
  uint32 ret;

  (void)ret; 
  MHI_SIO_INIT_DRV_IFACE(driver_iface , mhi_sio_ser_open, mhi_sio_ser_close, mhi_sio_ser_ioctl,
        mhi_sio_ser_transmit, mhi_sio_ser_flush_tx, NULL, mhi_sio_ser_ctrl_open, mhi_sio_ser_ctrl_close, 
        mhi_sio_ser_ctrl_ioctl, mhi_sio_ser_ctrl_transmit);

#ifndef FEATURE_HAS_NO_MHI_CORE
  MHI_SIO_INIT_DEV_IFACE(device_iface,&driver_iface,DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_DIAG), 
                           FALSE /*fwd port */,SIO_PORT_NULL,TRUE /* driver avail */);
#else
  MHI_SIO_INIT_DEV_IFACE(device_iface,&driver_iface,DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI), 
                           TRUE /*fwd port */,SIO_PORT_NULL,FALSE /* driver avail */);
#endif
 
 
 ret = register_driver_fn(&device_iface);
 MHI_ASSERT(ret); 
  
  /* MHI DIAG */
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_DIAG);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret); 

  /* MHI_SAHARA */
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_SAHARA);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret); 

  /* MHI_SSR */
 device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_SSR);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);
  /* MHI_EFS */
 device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_EFS);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret); 
  /* MHI_QMI */
 device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_QMI);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret); 
  /* MHI_IPSW_0 */
 device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_IPSW_0);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret); 
  /* MHI_IPSW_1 */
 device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_IPSW_1);
  ret = register_driver_fn(&device_iface)
  MHI_ASSERT(ret); 
  /* MHI_IPSW_2 */
 device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_IPSW_2);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret); 
  /* MHI_IPSW_3 */
 device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_IPSW_3);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);
  /* MHI_QDSS */
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_QDSS);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);
  /* MHI_SMCT */
 device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_SMCT);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret); 
  /* MHI_CSVT */
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_CSVT);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);
  /* MHI_DUN */
 device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_DUN);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

  /* MHI_QMI_2 */
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_QMI_2);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

  /* MHI_DCI */
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, SIO_MINOR_MHI_DCI);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);


 /* 5 Loopback channels for test */
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, 20);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, 21);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, 22);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);
  
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, 23);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI, 24);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

  /* for now we will use same MHI major number for all, forget about a different one for ECM */ 
#if 0

  /* ECM Driver Interface, used for RMNET with control path */
  MHI_SIO_INIT_DRV_IFACE(driver_iface , mhi_sio_ecm_open, mhi_sio_ecm_close, mhi_sio_ecm_ioctl,
        mhi_sio_ecm_transmit, mhi_sio_ecm_flush_tx, NULL, mhi_sio_ecm_ctrl_open, mhi_sio_ecm_ctrl_close, 
        mhi_sio_ecm_ctrl_ioctl, mhi_sio_ecm_ctrl_transmit);

#ifndef FEATURE_HAS_NO_MHI_CORE
  MHI_SIO_INIT_DEV_IFACE(device_iface,&driver_iface,DEVICE_ID(SIO_MAJOR_MHI_ECM, SIO_MINOR_MHI_ECM_RMNET0), 
        FALSE /*fwd port */,SIO_PORT_NULL,TRUE /* driver avail */);
#else
  MHI_SIO_INIT_DEV_IFACE(device_iface,&driver_iface,DEVICE_ID(SIO_MAJOR_MHI_ECM, SIO_MINOR_MHI_ECM_RMNET0), 
        TRUE /*fwd port */,SIO_PORT_NULL,FALSE /* driver avail */);
#endif

  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

  /* RMNET 1 */
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI_ECM, SIO_MINOR_MHI_ECM_RMNET2);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

  /* RMNET 2 */
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI_ECM, SIO_MINOR_MHI_ECM_RMNET2);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

  /* RMNET 3 */
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI_ECM, SIO_MINOR_MHI_ECM_RMNET3);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

  /* RMNET 4 */
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI_ECM, SIO_MINOR_MHI_ECM_RMNET4);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);
  
  /* RMNET 5 */
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI_ECM, SIO_MINOR_MHI_ECM_RMNET5);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

  /* RMNET 6*/
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI_ECM, SIO_MINOR_MHI_ECM_RMNET6);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

  /* RMNET 7 */
  device_iface.device_id=DEVICE_ID(SIO_MAJOR_MHI_ECM, SIO_MINOR_MHI_ECM_RMNET7);
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

  /* register primary MHI serial device with SIO */
  ret = register_driver_fn(&device_iface);
  MHI_ASSERT(ret);

#endif

  return 1;
}







