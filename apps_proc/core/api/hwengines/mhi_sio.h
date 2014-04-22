
/**
  @file mhi_sio.h
  @brief
  This file defines the SIO registration functions used by SIO to register the MHI device driver 
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/15/13   nk      Created
===============================================================================
                   Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/


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

uint32 mhi_sio_register_driver(sio_register_device_driver_cb_type register_driver_fn);

