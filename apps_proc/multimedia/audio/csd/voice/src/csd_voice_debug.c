/*-----------------------------------------------------------------------------
     Copyright (c) 2009 - 2011 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
	This section contains comments describing changes made to this module.
	Notice that changes are listed in reverse chronological order.

	$Header: //components/rel/audio.tx/2.0/audio/csd/voice/src/csd_voice_debug.c#1 $
	$DateTime: 2018/12/25 22:39:07 $
	$Author: pwbldsvc $
	$Change: 17929171 $
	$Revision: #1 $

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.
  when          who       what, where, why
  --------     -----     ---------------------------------------------------
  09/01/14      rp       enhanced the csd init debug buffer 
  04/16/14      jk       added support for Voice Session Manager

=============================================================================*/

#include "csd_voice_debug.h"
#include "csd_voice_debug_ioctl.h"
#include "csd_voice_debug_private.h" 
#include "csd_vs_private.h"
#include "csd_vc_private.h"
#include "csd_vm_private.h"
#include "csd_vsm.h"

static uint32_t csd_voice_debug_control_handle = 0; /* CSD voice debug handle */
static struct csd_vtable csd_voice_debug_vtable;    /* CSD Function Table */
/*
 ** Start Internal functions
*/
CSD_INTERNAL int32_t csd_voice_debug_open(enum csd_open_code open_id, void* params, uint32_t size)
{
  int32_t rc = CSD_EOK;
  csd_debug_voice_open_control_t* csd_voice_debug_open = (csd_debug_voice_open_control_t* )params;
  if ( CSD_OPEN_DEBUG_VOICE_CONTROL == open_id) 
  {
    csd_voice_debug_control_handle = csd_handle_malloc(CSD_OPEN_DEBUG_VOICE_CONTROL, 
                                                      &csd_voice_debug_control_handle);
    CSD_MSG(CSD_PRIO_HIGH,"CSD:VOICE DEBUG => csd_voice_debug_open open handle [0x%x]",
    		csd_voice_debug_control_handle);
    if(csd_voice_debug_control_handle == 0)
    {
      rc = NULL;
    }
    else
    {
      *csd_voice_debug_open->ret_status = CSD_EOK;
      rc = csd_voice_debug_control_handle;
    }
  }
  else
  {
    *csd_voice_debug_open->ret_status = CSD_ENORESOURCE; 
    rc = NULL;
  }
  return rc;
}

CSD_INTERNAL int32_t csd_voice_debug_close(uint32_t handle)
{
  int32_t rc = CSD_EOK;
  if(!handle)
  {
    rc =  CSD_EBADPARAM;
  }
  else
  {
    CSD_MSG(CSD_PRIO_HIGH,"CSD:VOICE DEBUG => csd_voice_debug_close close handle [0x%x]", handle);
    csd_handle_free(handle);
    handle = 0;
    rc = CSD_EOK;
  }
  return rc;
}

CSD_INTERNAL int32_t csd_voice_debug_ioctl(uint32_t handle, uint32_t code, void* params, uint32_t size)
{
  int32_t rc = CSD_EOK;
  csd_vsm_handle_t handle_type;

  switch(code){
  case CSD_DEBUG_VOICE_IOCTL_GET_STREAM_LIST:
    {
      handle_type.assoc_mode = FALSE;
      handle_type.handle_type = CSD_VSM_VS_HANDLE;
      rc = csd_vsm_get_handles( handle_type, CSD_VSM_SESSION_STATE_RUNNING,  params );
      CSD_MSG(CSD_PRIO_HIGH,"CSD:VOICE DEBUG => csd_voice_debug_ioctl CSD_DEBUG_VOICE_IOCTL_GET_STREAM_LIST "
    		  "voice debug handle=0x%x, rc=0x%x",
              handle, rc);
    }
    break;
  case CSD_DEBUG_VOICE_IOCTL_GET_CONTEXT_LIST:
    {
      //rc = csd_vm_ioctl ( NULL, CSD_VM_IOCTL_GET_CONTEXT_LIST, params, size );
      handle_type.assoc_mode = TRUE;
      handle_type.handle_type = CSD_VSM_VC_HANDLE;
       rc = csd_vsm_get_handles(handle_type, CSD_VSM_SESSION_STATE_RUNNING, params );
      CSD_MSG(CSD_PRIO_HIGH,"CSD:VOICE DEBUG => csd_voice_debug_ioctl CSD_DEBUG_VOICE_IOCTL_GET_CONTEXT_LIST "
    		  "voice debug handle=0x%x, rc=0x%x",
              handle, rc);
    }
    break;
  case CSD_DEBUG_VOICE_IOCTL_GET_DEVICE_IDS:
    {
      rc = csd_vc_ioctl ( NULL, CSD_VC_IOCTL_GET_DEVICE_IDS, params, size );
      CSD_MSG(CSD_PRIO_HIGH,"CSD:VOICE DEBUG => csd_voice_debug_ioctl CSD_DEBUG_VOICE_IOCTL_GET_DEVICE_IDS "
    		  "voice debug handle=0x%x, rc=0x%x",
              handle, rc);
    }
    break;
  case CSD_DEBUG_VOICE_IOCTL_GET_CVS_HANDLE:
    {
      csd_debug_voice_ioctl_get_cvs_handle_t *voice_dbg_cvs_get_handle;
      csd_vs_ioctl_get_cvs_handle_t csd_vs_cvs_get_handle;

      voice_dbg_cvs_get_handle = params;

      csd_vs_cvs_get_handle.ret_cvs_handle = voice_dbg_cvs_get_handle->ret_cvs_handle;

      rc = csd_ioctl ( voice_dbg_cvs_get_handle->csd_vs_handle, CSD_VS_IOCTL_GET_CVS_HANDLE, &csd_vs_cvs_get_handle, sizeof ( csd_vs_cvs_get_handle ) );
      CSD_MSG(CSD_PRIO_HIGH,"CSD:VOICE DEBUG => csd_voice_debug_ioctl CSD_DEBUG_VOICE_IOCTL_GET_CVS_HANDLE "
    		  "voice debug handle=0x%x, csd_vs_handle=0x%x, ret_cvs_handle=0x%x rc=0x%x",
              handle, voice_dbg_cvs_get_handle->csd_vs_handle, *voice_dbg_cvs_get_handle->ret_cvs_handle, rc);
    }
    break;
  case CSD_DEBUG_VOICE_IOCTL_GET_CVP_HANDLE:
    {
      csd_debug_voice_ioctl_get_cvp_handle_t *voice_dbg_cvp_get_handle;
      csd_vc_ioctl_get_cvp_handle_t csd_vc_cvp_get_handle;

      voice_dbg_cvp_get_handle = params;

      csd_vc_cvp_get_handle.ret_cvp_handle = voice_dbg_cvp_get_handle->ret_cvp_handle;

      rc = csd_ioctl ( voice_dbg_cvp_get_handle->csd_vc_handle, CSD_VC_IOCTL_GET_CVP_HANDLE, &csd_vc_cvp_get_handle, sizeof ( csd_vc_cvp_get_handle ) );
      CSD_MSG(CSD_PRIO_HIGH,"CSD:VOICE DEBUG => csd_voice_debug_ioctl CSD_DEBUG_VOICE_IOCTL_GET_CVP_HANDLE "
    		  "voice debug handle=0x%x, csd_vc_handle=0x%x, ret_cvp_handle=0x%x, rc=0x%x",
              handle, voice_dbg_cvp_get_handle->csd_vc_handle, *voice_dbg_cvp_get_handle->ret_cvp_handle, rc);
    }
    break;
      
    case CSD_DEBUG_VOICE_IOCTL_GET_AFE_PORT:
      {
        csd_debug_voice_ioctl_get_afe_port_t *voice_debug_get_afe_port = params;
        csd_vc_ioctl_get_afe_port_t vc_get_afe_port;
        
        vc_get_afe_port.ret_rx_afe_port = voice_debug_get_afe_port->ret_rx_afe_port;
        vc_get_afe_port.ret_tx_afe_port = voice_debug_get_afe_port->ret_tx_afe_port;
        vc_get_afe_port.ret_ec_afe_port = voice_debug_get_afe_port->ret_ec_afe_port;
        
        rc = csd_ioctl( voice_debug_get_afe_port->csd_vc_handle, CSD_VC_IOCTL_GET_AFE_PORT, &vc_get_afe_port, sizeof(csd_vc_ioctl_get_afe_port_t) );
        CSD_MSG( CSD_PRIO_MEDIUM, "CSD_DEBUG_VOICE_IOCTL_GET_AFE_PORT returned with rc[0x%x]", rc );        
      }
      break;
      
  default:
    rc = CSD_EUNSUPPORTED;
    break;
  }

  return rc;
}

CSD_INTERNAL int32_t csd_voice_debug_init(struct csd_vtable **ret_vtable)
{
  csd_voice_debug_vtable.open_fn = ( csd_open_fn ) csd_voice_debug_open;
  csd_voice_debug_vtable.close_fn = ( csd_close_fn )csd_voice_debug_close;
  csd_voice_debug_vtable.ioctl_fn = ( csd_ioctl_fn )csd_voice_debug_ioctl;
  csd_voice_debug_vtable.read_fn = ( csd_read_fn )csd_voice_debug_read;
  csd_voice_debug_vtable.write_fn = ( csd_write_fn ) csd_voice_debug_write;

  *ret_vtable = &csd_voice_debug_vtable;
   return CSD_EOK;
}

CSD_INTERNAL int32_t csd_voice_debug_deinit(void)
{
  return CSD_EOK;
}

CSD_INTERNAL int32_t csd_voice_debug_read (
  uint32_t handle,
  void* params,
  uint32_t size
)
{
  return CSD_EFAILED;
}

CSD_INTERNAL int32_t csd_voice_debug_write (
  uint32_t handle,
  void* params,
  uint32_t size
)
{
  return CSD_EFAILED;
}
/*
 ** End Internal functions
*/

