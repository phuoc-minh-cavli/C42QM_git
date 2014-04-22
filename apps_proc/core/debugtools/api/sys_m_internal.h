#ifndef SYS_M_INTERNAL_H
#define SYS_M_INTERNAL_H
/*===========================================================================

           SYS_M_INTERNAL.H

DESCRIPTION

Copyright (c) 2015-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/debugtools/api/sys_m_internal.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/19/16   nk      Sym_changes to support PIL on ThreadX

===========================================================================*/

#include "qmi_csi.h"
#include "msg.h"
#include "sys_m.h"

#define PROC_ID "APSS:"

typedef enum SYS_M_SPECIAL_MSG_T
{
    SYS_M_NONSPECIAL_MSG = 0,
    SYS_M_SHUTDOWN_MSG   = 1,
    SYS_M_POWEROFF_MSG   = 2,
    SYS_M_SFR_MSG        = 3,
    SYS_M_UNKNOWN_MSG    = 4
} SYS_M_SPECIAL_MSG_TYPE;

//enum to indicate the type of shutdown
typedef enum{
	SYS_M_SHUTDOWN_DEFAULT,
  SYS_M_SHUTDOWN_NOTIFY_ONLY,	
}shutdown_type;

typedef struct 
{
    uint32 MDM2AP_STATUS_GPIO;
    uint32 MDM2AP_ERR_FATAL_GPIO;
    uint32 AP2MDM_STATUS_GPIO;
    uint32 AP2MDM_ERR_FATAL_GPIO;
} sys_m_gpio_map_t;


// complete reset function
void sys_m_complete_reset(void);

// complete hibernation function
void sys_m_complete_hibernation(void);

//spin function, for apps break
void sys_m_spin(void);

//notify peripherals of an error fatal
void sys_m_err_notify_peripherals(void);

//wait for peripherals to finish error handling
void sys_m_wait_peripherals(void);

// * Callback for MPSS error fatal
void sys_m_mpss_errfatal_cb(void);

// * Callback for adsp error fatal
void sys_m_adsp_errfatal_cb(void);

//internal QMI init function
qmi_csi_service_handle ssctl_v02_qmi_ser_init(qmi_csi_os_params *os_params);

/**
  Api to indicate modem to do shutdown process before
  apps shutsdown the modem.

  @param[in] NONE
  
  @return
   NONE

  @dependencies
  None.
*/

void sys_m_notify_modem_before_shutdown(void);


#define SYS_M_AP_NOTIFY_RCV           0x00000002
#define SYS_M_TASKS_COMPLETE          0x00000004
#define SYS_M_INITIATE_HIBERNATION    0x00000010
#define SYS_M_COMPLETE_HIBERNATION    0x00000020
#define SYS_M_BLOCKING_HIBERNATION    0x00000040
#define SYS_M_COMPLETE_RESET          0x00000100
#define SYS_M_INITIATE_RESET          0x00001000
#define SYS_M_QMI_v02_SIG             0x00100000

#define SYS_M_SFR_LENGTH        90
#define sys_m_smd_port          "sys_mon"

void *sys_m_init_sfr_buffer(void);

#endif  /* SYS_M_INTERNAL_H */
