
#include <stringl/stringl.h>
#include "stdio.h"
#include "comdef.h"

#include "err.h"
#include "string.h"
#include "assert.h"
#include "rdevmap_wrapper.h"
/*=========================================================================*/
/**
@brief
  function to check if RDM is busy to accept new requests

@dependencies

@sideeffects
  None

@return
  None

*/
/*=========================================================================*/

boolean rdm_is_busy(void)
{
  return FALSE;
}
/*=========================================================================*/
/**
@brief
  This function can be used by services to register service parameters with RDM

@param[in] srvc -- Identifier for service.
@param[in] *srvc_params -- Pointer to service parameter structure for
                           registration.

@dependencies
  rdm_init() must have been called.  rdm_init() needs to be called by just one
  task and only once upon power-up.

@sideeffects
  None

@return
  None

@sa
  rdm_service_params_type
*/
/*=========================================================================*/

void rdm_register_service(rdm_service_enum_type srvc,  rdm_service_params_type *srvc_params)
{
  return;
}

/*=========================================================================*/
/**
@brief
  This function can be used by device drivers to register devices with RDM

@param[in] *dev_info -- Pointer to rdm_device_info_type structure.

@dependencies
  rdm_init() must have been called.  rdm_init() needs to be called by just one
  task and only once upon power-up.

@sideeffects
  None

@return
  None

@sa
  rdm_device_info_type
*/
/*=========================================================================*/

void rdm_register_device(rdm_device_info_type *dev_info)
{
  return;
}

/*=========================================================================*/
/**
@brief
  This function can be used by services to set its compatibility with
  a specific device.

  This function makes service compatible with a specific device, so as to allow
  port mapping between service and device.

@param[in] service -- Identifier for Service for which compatibility is set.
@param[in] device -- Identifier for device for which compatibility is set.

@dependencies
  rdm_init() must have been called.
  Service must register with rdm before calling this function.

@return
  None

@sideeffects
  None

@sa
  None
*/
/*=========================================================================*/

void rdm_set_compatibility(rdm_service_enum_type service, rdm_device_enum_type device)
{
  return;
}

/*=========================================================================*/
/**
@brief
    This function can be used by services to clear its compatibility with
  a specific device.

  This function makes service incompatible with a specific device, so as to disallow
  port mapping between service and device.

@param[in] service -- Identifier for Service for which compatibility is cleared.
@param[in] device -- Identifier for device for which compatibility is cleared.

@dependencies
  rdm_init() must have been called.
  Service must register with rdm before calling this function.

@return
  None

@sideeffects
  None

@sa
  None
*/
/*=========================================================================*/

void rdm_clear_compatibility(rdm_service_enum_type service, rdm_device_enum_type device)
{
  return;
}

/*=========================================================================*/
/**
@brief
  RDM issues opens to SRVCs at the power up time based on last mapping
  before power down.

  RDM issues open to services hosted on remote processor at power-up based on the
  previous port mappings retrieved from EFS. Services using the RDM should not
  just open a port when the MSM boots.  RDM will call the registered open
  routines for the SRVC's if they should open a port at boot.

  If EFS is being used, whatever the previous mapping was will be
  reinstated.

@param
  None

@dependencies
  None

@sideeffects
  If EFS is defined, the last valid port mapping prior to reset is
  re-instated

@return
  None

@sa
  None
*/
/*=========================================================================*/

void rdm_issue_opens(void)
{
  return;
}
/*~ FUNCTION rdm_issue_opens
    SRVNAME rdm_issue_remote_opens */

/*=========================================================================*/
/**
@brief
  RDM issues opens to SRVCs at the power up time based on last mapping
  before power down.

  RDM issues open to services hosted on remote processor at power-up based on the
  previous port mappings retrieved from EFS. Services using the RDM should not
  just open a port when the MSM boots.  RDM will call the registered open
  routines for the SRVC's if they should open a port at boot.

  If EFS is being used, whatever the previous mapping was will be
  reinstated.

  NOTE: when tmc calls rdm_issue_opens() from remote processor, rdm server
        will execute rdm_issue_opens_remote() to only issue opens for
        services on remote processor.
@param
  None

@dependencies
  None

@sideeffects
  If EFS is defined, the last valid port mapping prior to reset is
  re-instated

@return
  None

@sa
  rdm_issue_opens
*/
/*=========================================================================*/

void rdm_issue_remote_opens(void)
{
  return;
}

/*=========================================================================*/
/**
@brief
  This function initiates the RDEVMAP state machine to change the current
  serial port configuration to give the specified serial port to the
  specified Service task. This function will perform checks to ensure that
  the specified configuration is proper.

  The actual port change may or may not be completed prior to this function
  return, depending upon the priorities of the tasks involved.

  If the device is RDM_NULL_DEV, this function tries to close the first
  device in the rdm_device_enum_type which the service is using.
  Please use rdm_close_device if you want to specify which port to close.

  Since RDM hasn't its own task, this function run's in caller's context,
  Don't call it in ISR.

  Portmapping is recorded in EFS file.

  Several items determine the action of the RDM:
  --  The validity of the mapping is checked.  If it is not a valid request,
      the RDM returns.
  --  If the port mapping is allowed and the RDM is not in the middle of another
      port mapping assignment, the RDM moves to the Busy state.  Otherwise, the
      RDM was already in the Busy state and returns.
  --  If the service can only hold one port at any given time, RDM will
      check if the service is holding a device.  If so, it will direct the
      service to close its current port first.
  --  The RDM will determine if the destination device is in use.  If so, it
      calls the close routine for the service currently using the device.
      When the close routine returns, the RDM will return.
      If the device is not currently in use, the RDM calls the open routine for the
      requested service.  When the open routine returns, the RDM will return.

@param[in] service -- Service task being assigned a new port
@param[in] device  -- Device port to give to the service task
@param[in] *callback_ptr -- optional callback function; pass in NULL if not used


@dependencies
   The requested service and device must be listed in rdm_service_enum_type
   and rdm_device_enum_type respectively.

@sideeffects
  This function may cause a task context switch prior to returning.

@return
    TRUE -- if the mapping is valid and will be attempted.
           cbptr is called immediately before the function returns
           if the port mapping is already set as requested.
           cbptr will be called later to inform the mapping status
           otherwise.
    FALSE -- if the requested mapping is not valid, RDM is busy, or
           the function is call during an ISR.
           cbptr is called immediately to reject the request before
           the function returns.


@sa
  None
*/
/*=========================================================================*/

boolean rdm_assign_port
(
  /* Service Task desiring a device                     */
  rdm_service_enum_type         service,

  /* Device to give to service task                     */
  rdm_device_enum_type          device,

  /* Optional callback function to notify the status of
     the assign action                                  */
  rdm_assign_port_cb_type       cbptr
)
{
  return TRUE;
}

/*~ FUNCTION rdm_assign_port */

/*=========================================================================*/
/**
@brief
  This function initiates the RDEVMAP state machine to change the current
  serial port configuration to give the specified serial port to the
  specified Service task. This function will perform checks to ensure that
  the specified configuration is proper.

  The actual port change may or may not be completed prior to this function
  return, depending upon the priorities of the tasks involved.

  If the device is RDM_NULL_DEV, this function tries to close the first
  device in the rdm_device_enum_type which the service is using.
  Please use rdm_close_device if you want to specify which port to close.

  Since RDM hasn't its own task, this function run's in caller's context,
  Don't call it in ISR.

  Portmapping is NOT recorded in EFS file.

  Several items determine the action of the RDM:
  --  The validity of the mapping is checked.  If it is not a valid request,
      the RDM returns.
  --  If the port mapping is allowed and the RDM is not in the middle of another
      port mapping assignment, the RDM moves to the Busy state.  Otherwise, the
      RDM was already in the Busy state and returns.
  --  If the service can only hold one port at any given time, RDM will
      check if the service is holding a device.  If so, it will direct the
      service to close its current port first.
  --  The RDM will determine if the destination device is in use.  If so, it
      calls the close routine for the service currently using the device.
      When the close routine returns, the RDM will return.
      If the device is not currently in use, the RDM calls the open routine for the
      requested service.  When the open routine returns, the RDM will return.

@param[in] service -- Service task being assigned a new port
@param[in] device  -- Device port to give to the service task
@param[in] *callback_ptr -- optional callback function; pass in NULL if not used


@dependencies
   The requested service and device must be listed in rdm_service_enum_type
   and rdm_device_enum_type respectively.

@sideeffects
  This function may cause a task context switch prior to returning.

@return
    TRUE -- if the mapping is valid and will be attempted.
           cbptr is called immediately before the function returns
           if the port mapping is already set as requested.
           cbptr will be called later to inform the mapping status
           otherwise.
    FALSE --  if the requested mapping is not valid, RDM is busy, or
           the function is call during an ISR.
           cbptr is called immediately to reject the request before
           the function returns.
@sa
  rdm_assign_port_tmp
*/
/*=========================================================================*/

boolean rdm_assign_port_tmp
(
  /* Service Task desiring a device                     */
  rdm_service_enum_type         service,

  /* Device to give to service task                     */
  rdm_device_enum_type          device,

  /* Optional callback function to notify the status of
     the assign action                                  */
  rdm_assign_port_cb_type       cbptr
)
{
  return TRUE;
}

/*~ FUNCTION rdm_assign_port_tmp */

/*=========================================================================*/
/**
@brief
  This function will close a device used by a service and map the device
  to NULL service.

  This routine allows one service to direct another service, which may hold
  multiple devices, to close a specified device. And the caller service will
  get notified when the close action is finished via the optional cbptr.
  Comparing to rdm_assign_port( service, NULL,), which closes the device for
  the services and can hold only one device, rdm_close_device() closes a
  specified device for services which may hold multiple devices.

@param[in] service -- Service task to close a device
@param[in] device  -- Device to be closed by service
@param[in] cbptr   -- Optional callback function to inform caller the
                      result of the closing action

@dependencies
   The requested service and device must be listed in rdm_service_enum_type
   and rdm_device_enum_type respectively.

@sideeffects
  This function may cause a task context switch prior to returning.

@return
    TRUE --  the closing action is made
    FALSE -- If the device or service is not valid or
           If the service is not using the device or
           If the function is call in an ISR

@sa
  rdm_assign_port
*/
/*=========================================================================*/

boolean rdm_close_device
(
  /* Service Task request closing of the device         */
  rdm_service_enum_type         service,

  /* Device/port to be closed                           */
  rdm_device_enum_type          device,

  /* Optional callback function to inform the result of
     the closing action                                 */
  rdm_assign_port_cb_type       cbptr
)
{
  return TRUE;
}
/*~ FUNCTION rdm_close_device */

/*=========================================================================*/
/**
@brief
  This function will notify to the DEVMAP state machine the specified result
  of a requested serial port change. This function performs a portion of the
  DEVMAP state machine.

  This function informs the RDM of the status of the last open/close call
  directed by the RDM.  This function can be re-entrant

@param[in] srvc -- Service to which this call is referring
@param[in] status -- Status of the request

@dependencies
  None

@sideeffects
  None

@return
  None

@sa
  None
*/
/*=========================================================================*/

void rdm_notify
(
  rdm_service_enum_type  service,    /* Service Task desiring a new port */
  rdm_assign_status_type status      /* status of requested task change */
)
{
  return;
}
/*~ FUNCTION rdm_notify */

/*=========================================================================*/
/**
@brief
  Query the Runtime Device Mapper for the port/device currently being used
  by the specified application/service.

@param[in] service -- Service task to query about what device (if any)
                      it is using

@dependencies
  The specified service must be defined in rdm_service_enum_type.

@sideeffects
  None

@return
  rdm_device_enum_type that corresponds to the specified rdm_service_enum_type
  If specified service is not currently using any device, RDM_NULL_DEV is returned

@sa
  None
*/
/*=========================================================================*/

rdm_device_enum_type rdm_get_device
(
  rdm_service_enum_type service
                         /* What Service to determine what  */
                         /* Device (if any) it has open */
)
{
  return (rdm_device_enum_type) 0;
}
/*~ FUNCTION rdm_get_device */

/*=========================================================================*/
/**
@brief
  Query the Runtime Device Mapper for the port/devices currently being used
  by the specified application/service. It returns a device mask for the
  devices the service holds. The caller can test if the service is using
  a dev (for example, RDM_USB_DIAG_DEV) by if
  (dev_mask & (1 << RDM_USB_DIAG_DEV)).

  This routine allows a service task to determine which device, if any,
  another service task may have open.  This API must be used for a service
  that may hold multiple devices at the same time.  It can also be used for
  a service that can hold one port at any given time.  Comparing to
  rdm_get_device(), which is designed for services that can hold only one
  device at any time, this function can get multiple devices a service may
  currently hold.
  NOTE:
  It can only be used to query first 32 device. Not compatible in RDM3.0
  for devices greater than 32.

@param[in] service -- Service task to query what devices it currently holds

@dependencies
  None

@return
  device mask -- This function returns a uint32 device mask with
  each bits position corresponding to a devices index in
  rdm_device_enum_type.  The caller will know that the service is using a
  device if that devices bit location contains a 1

@sideeffects
  None

@sa
  None
*/
/*=========================================================================*/

uint32 rdm_get_multi_dev
(
  /* Service to query what devices it may be using, if any */
  rdm_service_enum_type service
)
{
  return 0;
}
/*~ FUNCTION rdm_get_multi_dev */

/*=========================================================================*/
/**
@brief
  Query the Runtime Device Mapper for the application/service that is
  currently holding to the specified port/device (if any).

  This routine allows a service task to determine what service, if any,
  a device may be opened by currently. This routine is the inverse of
  rdm_get_device().

@param[in] device -- Device to query about what service task (if any)
                     is using it

@dependencies
  The specified device must be defined in rdm_device_enum_type.

@sideeffects
  None

@return
   rdm_service_enum_type that corresponds to the specified rdm_device_enum_type
   If specified device is not currently mapped to any service, RDM_NULL_SRVC
   is returned

@sa
  None
*/
/*=========================================================================*/

rdm_service_enum_type rdm_get_service
(
  /* Device to see what service is using it, if any */
  rdm_device_enum_type device
)
{
  return RDM_FIRST_SRVC;
}
/*~ FUNCTION rdm_get_service */

/*=========================================================================*/
/**
@brief
  Query the RDM for the list of valid ports for the specified service.

  This routine allows the user to obtain all of the valid
  rdm_device_enum_type values for the specified rdm_service_enum_type.  The
  user needs to pass in the storage location of an array of type
  rdm_service_display_type of size RDM_MAX_DEV.  The RDM will fill out each
  item in devices[] with the appropriate information relative to the
  specified service.

@param[in] service -- Service task to query what devices are valid port
                      mapping options
@param[in,out] devices[] -- Location to write the valid device information

@dependencies
  The caller must have allocated space of RDM_DEV_MAX to the
  dev_displays array.
  The specified service must be defined in rdm_service_enum_type.

@sideeffects
  None

@return
  TRUE -- if the service is valid.
  FALSE -- Otherwise

@sa
  None
*/
/*=========================================================================*/

boolean rdm_get_valid_ports
(
  /* The service that getting the valid ports           */
  rdm_service_enum_type    service,

  /* array of rdm_service_display of length RDM_DEV_MAX */
  rdm_service_display_type devices[]
)
{
  return TRUE;
}
/*~ FUNCTION rdm_get_valid_ports */
/*~ PARAM INOUT devices VARRAY LENGTH RDM_DEV_MAX */

/*=========================================================================*/
/**
@brief
  Function for Service Tasks to register their open function. This open
  function is used by RDM to instruct a Service to open a specified port.

  The function is used in conjunction with rdm_register_close_func.

  The service can only hold one device at a time if it registers
  its open function by this function. If the service holds dev1 and requests
  RDM to map dev2 to it, RDM will take dev1 from the service and then assign
  dev2 to the service.

  Please use rdm_register_func_multi_dev if you want multi-dev support.

@param[in] service -- Service for which open function is being registered
@param[in] open_func -- Open_function callback for the service

@dependencies
  None

@sideeffects
  None

@return
  None

@sa
  None
*/
/*=========================================================================*/

void rdm_register_open_func
(
  rdm_service_enum_type service,       /* Service registering the function */
  rdm_service_open_func_ptr_type open_func     /* the routine to be called */
)
{
  return;
}
/*~ FUNCTION rdm_register_open_func
    SRVNAME rdm_register_remote_open_func */

/*=========================================================================*/
/**
@brief
  Function for Service Tasks to register their open function. This open
  function is what RDM calls to instruct a Service to open a specified port.

  The function is used in conjunction with rdm_register_remote_close_func.

  The service can only hold one device at a time if it registers
  its open function by this function. If the service holds dev1 and requests
  RDM to map dev2 to it, RDM will take dev1 from the service and then assign
  dev2 to the service.

  Please use rdm_register_func_multi_dev if you want multi-dev support.

  NOTE: when service on remote processor calls rdm_register_open_func(),
        RDM server executes rdm_register_remote_open_func() on local proc.

@param[in] service -- Service for which open function is being registered
@param[in] open_func -- Open_function callback for the service

@dependencies
  None

@sideeffects
  None

@return
  None

@sa
  rdm_register_open_func
*/
/*=========================================================================*/

void rdm_register_remote_open_func
(
  rdm_service_enum_type service,       /* Service registering the function */
  rdm_service_open_func_ptr_type open_func     /* the routine to be called */
)
{
  return;
}

/*=========================================================================*/
/**
@brief
  Function for Service Tasks to register their close function. This close
  function is what RDM calls to instruct a Service to close it's open port.

  The function is used in conjunction with rdm_register_open_func.

  By using this function, a service can have only one port open, hence
  no sio_port_id_type argument in the close function.

@param[in] service -- Service for which close function is being registered
@param[in] close_func -- Close_function callback for the service

@dependencies
  None

@sideeffects
  None

@return
  None

@sa
  None
*/
/*=========================================================================*/

void rdm_register_close_func
(
  rdm_service_enum_type service,       /* Service registering the function */
  rdm_service_close_func_ptr_type  close_func  /* the routine to be called */
)
{
  return;
}
/*~ FUNCTION rdm_register_close_func
    SRVNAME rdm_register_remote_close_func */

/*=========================================================================*/
/**
@brief
  Function for Service Tasks to register their close function. This close
  function is what RDM calls to instruct a Service to close it's open port.

  The function is used in conjunction with rdm_register_open_func.

  By using this function, a service can have only one port open, hence
  no sio_port_id_type argument in the close function.

  NOTE: when service on remote processor calls rdm_register_close_func(),
        RDM server executes rdm_register_remote_close_func() on local proc.

@param[in] service -- Service for which close function is being registered
@param[in] close_func -- Close_function callback for the service

@dependencies
  None

@return
  None

@sideeffects
  None

@sa
  rdm_register_close_func
*/
/*=========================================================================*/

void rdm_register_remote_close_func
(
  rdm_service_enum_type service,       /* Service registering the function */
  rdm_service_close_func_ptr_type  close_func  /* the routine to be called */
)
{
  return;
}


/*=========================================================================*/
/**
@brief
  Function for Service Tasks to register their open/close functions.  The
  functions are what RDM calls to instruct a Service to open/close device.

  This function supports multiple ports functionality, ie a service can
  open more than one device at a time.

  It is newer than rdm_register_open_func and rdm_register_close_func,
  which register the opening/closing function when RDM only supports one
  port for one service. rdm_register_open_func and rdm_register_close_func
  are maintained for back compatibility.

@param[in] service -- Service for which close function is being registered
@param[in] open_func -- Open_function callback for the service
@param[in] close_func -- Close_function callback for the service
@param[in] *data -- User data, the pointer will be passed back to the service
                   (Optional)

@dependencies
  None

@sideeffects
  None

@return
    TRUE -- the registration succeeds
    FALSE -- the registration fails. The following reasons could cause failure.
           1) open or close function is NULL
           2) service is out of range

@sa
  None
*/
/*=========================================================================*/

boolean rdm_register_func_multi_dev
(
  /* the service who registers the open and close function      */
  rdm_service_enum_type                 service,

  /* function for opening device                                */
  rdm_srv_func_ptr_multi_dev_type       open_func,

  /* function for closing device                                */
  rdm_srv_func_ptr_multi_dev_type       close_func,

  /* user data, the pointer will be passed back to the service
     It can be set to NULL.                                     */
  void *                                data
)
{
  return TRUE;
}
/*~ FUNCTION rdm_register_func_multi_dev
    SRVNAME rdm_register_remote_func_multi_dev */

/*=========================================================================*/
/**
@brief
  Function for Service Tasks to register their open/close functions.  The
  functions are what RDM calls to instruct a Service to open/close device.

  This function supports multiple ports functionality, ie a service can
  open more than one device at a time.

  It is newer than rdm_register_open_func and rdm_register_close_func,
  which register the opening/closing function when RDM only supports one
  port for one service. rdm_register_open_func and rdm_register_close_func
  are maintained for back compatibility.

 NOTE: when service on remote processor calls rdm_register_func_multi_dev(),
        RDM server executes rdm_register_remote_func_multi_dev() on local proc.


@param[in] service -- Service for which close function is being registered
@param[in] open_func -- Open_function callback for the service
@param[in] close_func -- Close_function callback for the service
@param[in] *data -- User data, the pointer will be passed back to the service
                   (Optional)

@dependencies
  None

@sideeffects
  None

@return
    TRUE -- the registration succeeds
    FALSE -- the registration fails. The following reasons could cause failure.
           1) open or close function is NULL
           2) service is out of range

@sa
  rdm_register_func_multi_dev
*/
/*=========================================================================*/

boolean rdm_register_remote_func_multi_dev
(
  /* the service who registers the open and close function      */
  rdm_service_enum_type                 service,

  /* function for opening device                                */
  rdm_srv_func_ptr_multi_dev_type       open_func,

  /* function for closing device                                */
  rdm_srv_func_ptr_multi_dev_type       close_func,

  /* user data, the pointer will be passed back to the service
     It can be set to NULL.                                     */
  void *                                data
)
{
  return TRUE;
}


/*=========================================================================*/
/**
@brief
  Query the RDM for the ASCII string that corresponds to the
  rdm_device_enum_type specified.

  Note:This is used by the UI, and only 13 bytes of storage will be
  available in *str. One byte is for the NULL.

@param[in] device -- Identifier for device for which string is requested
@param[in,out] *str -- Place holder for device string

@dependencies
  The device must be in rdm_device_enum_type. The str has pre-allocated
  space of at least 13 characters including null-terminator.

@sideeffects
  The contents of pointer str will be written to. If the specified device
  is not valid, str is set to NULL.

@return
  TRUE -- if the device is valid
  FALSE -- Otherwise

@sa
  None
*/
/*===========================================================================*/

boolean rdm_get_device_str
(
  rdm_device_enum_type  device,            /* device to look up            */
  char                  *str               /* where to write the string    */
)
{
  return TRUE;
}
/*~ FUNCTION rdm_get_device_str */
/*~ PARAM OUT str STRING 13 */

/*=========================================================================*/
/**
@brief
  This function queries the RDM if a specified service is holding a specified
  device.

  This routine allows a service task to determine if another service is
  holding a specified device.

@param[in] service -- Service task to query if it holds the device
@param[in] device -- Device to query if it is held by the service

@dependencies
   The specified service must be defined in rdm_service_enum_type.
   The specified device must be defined in rdm_device_enum_type.

@sideeffects
  None

@return
    TRUE --  if the service is holding the device or
         if the service does not hold any device and the second
            parameter is RDM_NULL_DEV
    FALSE --  if the service is not holding the device or
          if the parameters are not right

@sa
  None
*/
/*=========================================================================*/

boolean rdm_is_srv_holding_dev
(
  rdm_service_enum_type service,        /* service to test      */
  rdm_device_enum_type  device          /* device to test       */
)
{
  return TRUE;
}
/*~ FUNCTION rdm_is_srv_holding_dev */

/*=========================================================================*/
/**
@brief
  Set device compatible mask for the service from
  (RDM_LAST_KNOWN_SRVC + 1) to (RDM_SRVC_MAX - 1). It tells RDM which
  device this service can use.

  It provides an interface to dynamically set the compatibility mask. Don't
  use this function for services of from RDM_DIAG_SRVC to
  RDM_LAST_KNOWN_SRVC (RDM_RPC_SRVC). The device compatibility table
  are set for those service at compile time.

  The caller can set dev_mask as this: if a service can use devices
  RDM_UART1_DEV and RDM_USB_DIAG_DEV.  The dev_mask is
  (1 << RDM_NULL_DEV) | (1 << RDM_UART1_DEV) | (1 << RDM_USB_DIAG_DEV).
  RDM_NULL_DEV is here since RDM can always map NULL DEV to the service,
  meaning closing the device of the service.

@param[in] service --  Service task for which to set a dev_mask
@param[in] dev_mask -- Indicates which devices the service can use

@dependencies
  The specified service must be greater than RDM_LAST_KNOWN_SRVC in
  rdm_service_enum_type.

@sideeffects
  None

@return
    TRUE -- if successfully set
    FALSE -- if the service is incorrect.

@sa
  None
*/
/*=========================================================================*/

boolean rdm_set_srv_compat_mask
(
  rdm_service_enum_type service,        /* service to set       */
  uint32                dev_mask        /* dev mask to set      */
)
{
  return TRUE;
}
/*~ FUNCTION rdm_set_srv_compat_mask */

/*=========================================================================*/
/**
@brief
  This function queries the RDM whether a specified service can use a
  specified device.

@param[in] service -- Service task to be queried
@param[in] device --  Device to be queried

@dependencies
  The specified service must be defined in rdm_service_enum_type.
  The specified device must be defined in rdm_device_enum_type.

@sideeffects
  None

@return
    TRUE  -- if the service and the device is compatible,
    FALSE -- if the service and the device is not compatible or
             if the parameters are not right

@sa
  None
*/
/*===========================================================================*/

boolean rdm_is_srv_dev_compat
(
  rdm_service_enum_type service,        /* service to test      */
  rdm_device_enum_type  device          /* device to test       */
)
{
  return TRUE;
}
/*~ FUNCTION rdm_is_srv_dev_compat */

/*=========================================================================*/
/**
@brief
  Initializes the Runtime Device Mapper Services. This function must
  be called before other functions of this module.

@param
  None

@dependencies
  Should be called just once.

@return
  None

@sideeffects
  None

@sa
  None
*/
/*=========================================================================*/

void rdm_init (void)
{
  return;
}

/*=========================================================================*/
/**
@brief
  Read in the port mappings last know by RDM from EFS.

  At powerup, during initialization this function is called to read in port
  mappings by RDM   from EFS.  It is here that the compile time defaults for
  port mapping may   be overwritten by what is read from EFS.  Thus, port
  mappings prior to reset are restored.

@param
  None

@dependencies
  FEATURE_EFS is defined.

@sideeffects
  Port mappings prior to reset will be restored.

@return
  None

@sa
  None
*/
/*=========================================================================*/

void rdm_get_prev_ports(void)
{
  return;
}


/*==========================================================================*/
/*                         SERIAL DEVICE MAPPER                             */
/*==========================================================================*/

/** Structure for UI baud item */
#if 0
typedef struct
{
  char              *item_string;  /**< String used for UI. Must be exactly 12 characters long.   */
  sio_bitrate_type   baud_id;      /**< Baudrate to be used. */
} sdm_ui_baud_item_type;


#define SDM_MAX_UI_ITEMS     10  /**< This is a limit from the UI code
                                   in any event must be less that.         */

#define SDM_UNKNOWN_INDEX  0xFF  /**< The value of SDM_UNKNOWN_INDEX
                                    Flag value to indicate that we don't
                                    know the current default.               */


/*--------------------------------------------------------------------------*/
/** Structure definitions for DS and DIAG baud rate UI MENU. */
/*--------------------------------------------------------------------------*/

typedef struct
{
  char            *heading;       /**< String used for UI. Must be exactly 12 characters long
                                   as an example: " 115,200 ".         */

  uint8            current_index; /**< index of current selection/value (0-0xFF)
                                     "0xFF" indicates current_index is
                                   unknown. REAL item indexes range
                                   from zero to (SDM_MAX_UI_ITEMS - 1).  */

  uint8            num_items;     /**< MAX of SDM_MAX_UI_ITEMS.        */

  sdm_ui_baud_item_type *items[ SDM_MAX_UI_ITEMS ]; /**< UI Buad item type. */

} sdm_ui_baud_menu_type;

#endif

/*=========================================================================*/
/**
@brief
  Returns the DIAG baud selection menu. If NV contains a valid baud rate
  and it is in our list of baud rate choices, we indicate that as the
  current baud rate - UI will mark this with a "checkmark".

@param[in] nv_result_stat -- identifier for NV result status
@param[in] *nv_data_buf   -- pointer to nv_item_type, only valid
                             if nv_result_stat == NV_DONE_S

@dependencies
  None

@sideeffects
  None

@return
  sdm_ui_baud_menu_type for Diag buad

@sa
  None
*/
/*=========================================================================*/

sdm_ui_baud_menu_type * sdm_ui_get_table_diag_baud
(
  nv_stat_enum_type nv_result_stat,
  void *nv_data_buf      /* only valid if
                                      * nv_result_stat == NV_DONE_S
                                      */
)
{
  return (sdm_ui_baud_menu_type*) 0;
}

/*=========================================================================*/
/**
@brief
  Returns the DS baud selection menu. If NV contains a valid baud rate
  and it is in our list of baud rate choices, we indicate that as the
  current baud rate - UI will mark this with a "checkmark".

@param[in] nv_result_stat -- identifier for NV result status
@param[in] *nv_data_buf   -- pointer to nv_item_type, only valid
                             if nv_result_stat == NV_DONE_S

@dependencies
  None

@sideeffects
  None

@return
  sdm_ui_baud_menu_type for ds baud

@sa
  None
*/
/*=========================================================================*/

sdm_ui_baud_menu_type * sdm_ui_get_table_ds_baud
(
  nv_stat_enum_type nv_result_stat,
  void *nv_data_buf      /* only valid if
                                      * nv_result_stat == NV_DONE_S
                                      */
)
{
  return (sdm_ui_baud_menu_type *)0;
}


/*=========================================================================*/
/**
@brief
  Forwards the new DIAG baud request to DIAG.

@param[in] new_baud_index -- Index to new baud rate

@dependencies
  None

@return
  None

@sideeffects
  None

@sa
  None
*/
/*=========================================================================*/

void sdm_ui_set_diag_baud
(
  uint8 new_baud_index
)
{
  return;
}

/*=========================================================================*/
/**
@brief
  Forwards the new DS baud request to DS.

@param[in] new_baud_index -- Index to new baud rate

@dependencies
  None

@sideeffects
  None

@return
  None

@sa
  None
*/
/*=========================================================================*/

void sdm_ui_set_ds_baud
(
  uint8 new_baud_index
)
{
  return;
}

