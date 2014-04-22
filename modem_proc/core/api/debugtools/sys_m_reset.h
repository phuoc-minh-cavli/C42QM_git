#ifndef SYS_M_RESET_H
#define SYS_M_RESET_H
/*===========================================================================

           S Y S _ M _ R E S E T. H

DESCRIPTION

Copyright (c) 2011-15 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.10/api/debugtools/sys_m_reset.h#2 $
  when       who      what, where, why
--------     ---     ------------------------------------------------------------
11/04/14     rks      Added reason code support with SSREQ req
06/26/14     rks      SSREQ client implimentation
===========================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/
/**
   RCECB Exposed Events

   @dependencies
   Requires client to include rcecb.h to use
*/

#define SYS_M_RCECB_FWD_POWEROFF    "SYSM:FWD:POWEROFF"
#define SYS_M_RCECB_FWD_SHUTDOWN    "SYSM:FWD:SHUTDOWN"

/** sys_m status/return codes */
typedef enum {
  SYS_M_STATUS_SUCCESS                   = 0,
  /* General failure */
  SYS_M_STATUS_FAILURE                   = -1,
  /* Invalid Input params */
  SYS_M_STATUS_INVALID_PARAM             = -2,
  /* SYS M Module Not Initialized */
  SYS_M_STATUS_NOT_INIT                  = -3,
  /* Malloc etc system resource related errors */
  SYS_M_STATUS_OUT_OF_RESOURCES          = -4,
  /* Requested Operation not supported */
  SYS_M_STATUS_OP_NOT_SUPPORTED          = -5,
  /* Failures relating to SYS_M operation timeout */
  SYS_M_STATUS_TIMEOUT                   = -6,
  /* OS API returned with some error */
  SYS_M_STATUS_OS_ERROR                  = -7
}sys_m_err_type;


typedef enum sys_m_reset_t{
   SYS_M_SHUTDOWN,
   SYS_M_RESTART,
}sys_m_reset_type;


typedef enum _ssreq_ind_result{
   SSREQ_REQUEST_SERVED,
   SSREQ_REQUEST_NOT_SERVED,
}ssreq_ind_result;

typedef enum _ssreq_qmi_request_res_code_type{
   SSREQ_QMI_RES_NONE=0,
   SSREQ_QMI_RES_MODEM_CONF_CHANGE = 0x10,/*SSREQ_QMI_RES_MODEM_CONF_CHANGE_V01*/
   SSREQ_QMI_RES_SEGMENT_LOADING/*SSREQ_QMI_RES_SEGMENT_LOADING_V01*/
}ssreq_qmi_request_res_code_type;

typedef enum _ssreq_status_t
{
   SSREQ_SUCCESS      =  0, /*request was successful*/
   SSREQ_FAILURE      = -1, /*request failed*/
   SSREQ_IN_PROCESS   = -2, /*already a request is in process and cannot process a
                              new request until existing request completes */
}ssreq_status_t;

/* Init macro for sys_m_reset_config_type structure */
#define SYS_M_RESET_CONFIG_INIT(cfg)  memset(&(cfg), 0, sizeof(cfg))

#define SYS_M_CONFIG_SET_RESET_TYPE(cfg, type) {cfg.reset_type = type;}

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/
/* Indication callback type */
typedef void ssreq_qmi_ind_cb_fn_type(ssreq_ind_result res);

/** 
    Sys_M reset Configuration Structure 
    Note1: This should be initialized with SYS_M_RESET_CONFIG_INIT before 
           updating parameters 
    Note2: use SYS_M_SET_RESET_TYPE(cfg) to set reset_type parameter
*/
typedef struct  sys_m_reset_config_t {
  sys_m_reset_type      reset_type;  /* Type of Reset */
  /* Can add more parameters here as per future needs */
}sys_m_reset_config_type;


/*===========================================================================
                      SYS_M_RESET PUBLIC API
===========================================================================*/

/**
  Initiates reset of the subsystem

  @return
  None.

  @dependencies
  None.
*/
void sys_m_initiate_shutdown(void);


/**
  Performs various kinds of resets depending on input configuration provided

  @return
  Standard Sys_M error codes.

  @dependencies
  Config structure should be initialized with SYS_M_RESET_CONFIG_INIT before 
  updating parameters
*/
sys_m_err_type sys_m_reset(sys_m_reset_config_type *cfg_ptr);


/**
  Initiates full poweroff of the subsystem

  @return
  None.

  @dependencies
  None.
*/
void sys_m_initiate_poweroff(void);

/**
  Initiates restart of the complete system using SSREQ QMI
  And notify about the indication responce status to 
  the user of this API via its callback function (ind_cb_fn)  if provided

  @return
   SSREQ_SUCCESS
   SSREQ_FAILURE
   SSREQ_IN_PROCESS

  @dependencies
  None.
*/
ssreq_status_t sys_m_initiate_restart_ssreq(ssreq_qmi_ind_cb_fn_type *ind_cb_fn);

/**
  Initiates the peripheral restart using SSREQ QMI
  And notify about the indication responce status to 
  the user of this API via its callback function(ind_cb_fn) if provided


  @return
   SSREQ_SUCCESS
   SSREQ_FAILURE
   SSREQ_IN_PROCESS

  @dependencies
  None.
*/
ssreq_status_t sys_m_request_peripheral_restart_ssreq(ssreq_qmi_ind_cb_fn_type *ind_cb_fn);

/**
  Initiates reset of the system using SSREQ QMI
  and notify about the indication responce status to 
  the user of this API via its callback function (ind_cb_fn)  if provided
  In addition provide the reason code(res_code) to the service for SSREQ request initiation

  @return
   SSREQ_SUCCESS
   SSREQ_FAILURE
   SSREQ_IN_PROCESS

  @dependencies
  None.
*/
ssreq_status_t sys_m_init_restart_ssreq_with_res_code(ssreq_qmi_ind_cb_fn_type *ind_cb_fn,
                                       ssreq_qmi_request_res_code_type res_code);
/**
  Initiates the peripheral restart using SSREQ QMI
  and notify about the indication responce status to 
  the user of this API via its callback function(ind_cb_fn) if provided
  In addition provide the reason code(res_code) to the service for SSREQ request initiation

  @return
   SSREQ_SUCCESS
   SSREQ_FAILURE
   SSREQ_IN_PROCESS

  @dependencies
  None.
*/
ssreq_status_t sys_m_req_pp_restart_ssreq_with_res_code(ssreq_qmi_ind_cb_fn_type *ind_cb_fn,
                                          ssreq_qmi_request_res_code_type res_code);
#if defined(__cplusplus)
}
#endif

#endif  /* SYS_M_RESET_H */
