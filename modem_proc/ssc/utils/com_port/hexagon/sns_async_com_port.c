/**
 * sns_async_com_port.c
 *
 * The Asynchronous COM Port Utility implementation
 *
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "sns_assert.h"
#include "sns_async_com_port_int.h"
#include "sns_com_port_priv.h"
#include "sns_com_port_types.h"
#include "sns_island.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_sync_com_port_service.h"
#include "sns_async_com_port.pb.h"


/** Vtables for bus specific API.
 */
extern sns_ascp_port_api i2c_async_port_api;
#ifndef SSC_TARGET_HEXAGON_CORE_SPI_NOT_SUPPORTED
extern sns_ascp_port_api spi_async_port_api;
#endif
extern sns_sync_com_port_service_api scp_port_service_api;

/** Interface functions for supported bus ports.
 *
 *  IMPORTANT: This list must match enum definitions in
 *  sns_bus_type.
 */
_Static_assert(SNS_BUS_I2C            == SNS_ASYNC_COM_PORT_BUS_TYPE_I2C,
               "Sync & Async bus type mismatch!");
#ifndef SSC_TARGET_HEXAGON_CORE_SPI_NOT_SUPPORTED
_Static_assert(SNS_BUS_SPI            == SNS_ASYNC_COM_PORT_BUS_TYPE_SPI,
               "Sync & Async bus type mismatch!");
#endif
_Static_assert(SNS_BUS_I3C_SDR        == SNS_ASYNC_COM_PORT_BUS_TYPE_I3C_SDR,
               "Sync & Async bus type mismatch!");
_Static_assert(SNS_BUS_I3C_HDR_DDR    == SNS_ASYNC_COM_PORT_BUS_TYPE_I3C_HDR_DDR,
               "Sync & Async bus type mismatch!");
_Static_assert(SNS_BUS_I3C_I2C_LEGACY == SNS_ASYNC_COM_PORT_BUS_TYPE_I3C_I2C_LEGACY,
               "Sync & Async bus type mismatch!");
static sns_ascp_port_api *ascp_port_apis[] SNS_SECTION(".data.sns") =
{
  &i2c_async_port_api,  // SNS_BUS_I2C
#ifndef SSC_TARGET_HEXAGON_CORE_SPI_NOT_SUPPORTED
  &spi_async_port_api,  // SNS_BUS_SPI
#endif
  NULL,                 //placeholder for SNS_BUS_UART &uart_port,
  &i2c_async_port_api,  // SNS_BUS_I3C_SDR
  &i2c_async_port_api,  // SNS_BUS_I3C_DDR
  &i2c_async_port_api,  // SNS_BUS_I3C_I2C_LEGACY
};


SNS_SECTION(".text.sns") static sns_rc sns_ascp_register_callback(void  *port_handle,
                                      sns_ascp_callback callback,
                                      void* args)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  return ascp_port_apis[bus_info->bus_type]->sns_ascp_register_callback(port_handle, callback, args);
}

SNS_SECTION(".text.sns") static sns_rc sns_async_vectored_rw(void  *port_handle,
                           uint8_t num_vectors,
                           sns_port_vector vectors[],
                           bool save_write_time)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_rc return_code = SNS_RC_FAILED;
  if(bus_info->opened == true && bus_info->power_on == true)
  {
    return_code =  ascp_port_apis[bus_info->bus_type]->
      sns_ascp_register_rw(port_handle, num_vectors, vectors, save_write_time);
  }
  return return_code;
}

sns_ascp_port_api sns_async_port_api SNS_SECTION(".data.sns") =
{
  .struct_len                  = sizeof(sns_ascp_port_api),
  .sync_com_port               = &scp_port_service_api,
  .sns_ascp_register_callback  = &sns_ascp_register_callback,
  .sns_ascp_register_rw        = &sns_async_vectored_rw,
};

