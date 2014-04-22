#ifndef DS_MUX_STUB_H
#define DS_MUX_STUB_H
/*===========================================================================

                        D S _ M U X _ S T U B . H

DESCRIPTION

  This is the external header file for the MUX Task. This file
  contains all the functions, STUB definitions and data types needed for MUX 
  task to interface to SIO for physical port.
  

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/inc/ds_mux_stub.h#1 $ 
  $DateTime: 2019/11/26 00:18:17 $ $Author: pwbldsvc $

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


#ifdef __cplusplus
extern "C"
{
#endif

#include "ds_mux_defs.h"
#include "sio.h"


#define   DS_MUX_STACK_SIZ             4096
#define   DS_MUX_MAX_SERV              16
#define   DS_MUX_PHY_PORT              0x1234
#define   DS_MUX_PHY_STREAM_ID         0x1234
#define   DS_MUX_SERVICE               RDM_DATA_MUX_SRVC
#define   DS_MUX_PHY_DEV               RDM_MUX_PHYSICAL_DEV
#define   DS_MUX_TEST_LOGICAL_SERVICE  RDM_DATA_SRVC

#define   DS_MUX_TEST_PHYS_WM_DEFAULT_LO   175000  //TBD
#define   DS_MUX_TEST_PHYS_WM_DEFAULT_HI   350000  //TBD
#define   DS_MUX_TEST_PHYS_WM_DEFAULT_DNE  400000  //TBD

#define  DS_MUX_TEST_FIRST_LOG_PORT_OPEN   0x1
#define  DS_MUX_TEST_SECOND_LOG_PORT_OPEN  0x2
#define  DS_MUX_TEST_THIRD_LOG_PORT_OPEN   0x3
#define  DS_MUX_TEST_VERIFY_UA_RECEIVED    0x4
#define  DS_MUX_TEST_VERIFY_DATA_RECEIVED  0x5


typedef struct
{
  uint8                 conn_count;
  sio_port_id_type      sio_port[DS_MUX_MAX_LOGICAL_CONNECTION];
  sio_stream_id_type    stream_id[DS_MUX_MAX_LOGICAL_CONNECTION];
} ds_mux_stub_logical_conn_info;


void ds_mux_stub_init
(
  boolean                 is_static_init,
  sio_driver_interface    *test_phy_driver_iface
);


boolean ds_mux_stub_phy_transmit_to_mux
(
  dsm_item_type *tx_ptr
);


void  ds_mux_stub_reg_physical_sio
(
  sio_device_interafce * device_iface_ptr
);


sio_stream_id_type ds_mux_stub_sio_open
( 
  sio_open_type *open_ptr                      /* Configurations of stream */
);


void ds_mux_stub_sio_ioctl
(
  sio_stream_id_type    stream_id,
  sio_ioctl_cmd_type    cmd,
  sio_ioctl_param_type *param
);


void ds_mux_rdm_register_service
(
  rdm_service_enum_type srvc,
  rdm_service_params_type *srvc_params
);


 void ds_mux_stub_register_service_with_rdm
(
  void (*open_func_ptr_type) (sio_stream_id_type),
  void (*close_func_ptr_type)(sio_stream_id_type),
  void (*dtr_func_ptr_type)  (sio_stream_id_type, boolean),
  void (*non_emp_wm_func_ptr_type)(sio_stream_id_type, dsm_watermark_type  *)
);


 void ds_mux_stub_send_data_over_logical
(
  uint8                     dlci_Id,
  dsm_item_type           **tx_ptr
);

void ds_mux_stub_send_ioctl_over_logical
(
  uint8                     dlci_Id,
  sio_ioctl_cmd_type        cmd,
  sio_ioctl_param_type     *param

);

#ifdef __cplusplus
}
#endif


#endif /* STUB */
