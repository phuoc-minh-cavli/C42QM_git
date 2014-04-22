/*============================================================================
  FILE:         HALmpmIPC.c
 
  OVERVIEW:     This file contains the function implementation to send an IPC
                to RPM whenever vMPM configuration is changed.
 
  DEPENDENCIES: None
 
                Copyright (c) 2012 QUALCOMM Technologies.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

  $Header: //components/rel/core.tx/6.0/power/mpm/src/hal/HALmpmintIPC.c#1 $
============================================================================*/

#include "DDIIPCInt.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "CoreVerify.h"

static DalDeviceHandle *mpm_dal_ipc_handle = NULL;

/**
 * HAL_mpmint_SetupIPC
 */
void HAL_mpmint_SetupIPC(void)
{
  /* Obtaining a DAL handle for IPC interrupt */
  CORE_DAL_VERIFY( DAL_DeviceAttach( DALDEVICEID_IPCINT, 
                                     &mpm_dal_ipc_handle ) );
}

void vmpm_send_interrupt(void)
{
  /* Sending an IPC interrupt to RPM */
  if( NULL != mpm_dal_ipc_handle )
  {
    DalIPCInt_Trigger( mpm_dal_ipc_handle, /* DAL handle for IPC int */
                       DALIPCINT_PROC_RPM, /* Processor to trigger interrupt on */
                       DALIPCINT_GP_MED ); /* Interrupt Id*/
  }
  else
  {
    CORE_VERIFY(0);
  }
}
