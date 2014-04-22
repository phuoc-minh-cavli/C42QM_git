 /*==============================================================================
 
    High Speed Configuration Selector RDM APIs
         I N T E R N A L  H E A D E R   F I L E
   
   GENERAL DESCRIPTION
     This file defines APIs for RDM and compostion management for HS USB.
 
   EXTERNALIZED FUNCTIONS
     hsu_get_rdm_map
     hsu_set_compositon
 
   INITALIZATION AND SEQUENCING REQUIREMENTS
     None.
     
   Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 ==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_conf_sel_rdm.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  4/22/09   tk      created 
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "customer.h" 
#include "hsu_common_int.h"
#include "hsu_conf_sel_stack_utils_int.h"
#include "jtypes_int.h"
#include "jos_int.h"
#include "jslave_init.h"
#include "hsu_conf_sel_comp_utils.h"
#include "rdevmap.h"
#include "hsu_conf_sel_types_int.h"
#include "hsu_conf_sel_i.h"
#include "hsu_conf_sel_rdm.h"
#include "hsu_al_task.h"


/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/
typedef struct {
  rdm_assign_status_type status;
  rdm_service_enum_type service;
  rdm_device_enum_type device;
} hsu_rdm_close_device_cb_par_type;

typedef struct {
  rdm_assign_status_type status;
  rdm_service_enum_type service;
  rdm_device_enum_type device;
} hsu_rdm_assign_port_cb_par_type;

typedef struct {
  boolean status;
} hsu_open_rdm_cb_par_type;

typedef struct {
  void *arg;
  jresult_t status;
} hsu_change_comp_reconfig_cb_par_type;

typedef struct {
  boolean status;
} hsu_close_rdm_cb_par_type;

typedef enum {
  HSU_SET_COMP_STATE_IDLE,
  HSU_SET_COMP_STATE_IN_PROGRESS

} hsu_set_comp_state_type;
typedef struct {
  hsu_set_comp_state_type state;
  boolean rdm_status_done[HSU_NUM_OF_RDM_USB_PORTS];
  hsu_conf_sel_comp_info_type *curr_comp;
  int curr_device_idx;
  hsu_conf_sel_composition_id_enum composition_id;
  hsu_set_comp_cb_type complete_cb;
  hsu_set_comp_cb_type rdm_close_cb;
  hsu_set_comp_cb_type rdm_open_cb;
  hsu_rdm_dev_svc_pare_type 
                          new_rdm_map[HSU_NUM_OF_RDM_USB_PORTS];
  juint8_t new_rdm_map_size;
  hsu_rdm_dev_svc_pare_type 
                          curr_rdm_map[HSU_NUM_OF_RDM_USB_PORTS];
  juint8_t curr_rdm_map_size;
  juint8_t curr_rdm_map_index;
} hsu_change_comp_type;
  

/*==============================================================================
  Variables
==============================================================================*/

static hsu_change_comp_type hsu_change_comp;

/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/
static void hsu_rdm_close_device_cb_qed(void *arg);
static void hsu_rdm_assign_port_cb_qed(void *arg);
static void hsu_open_rdm_cb_qed(void * arg);
static void hsu_change_comp_reconfig_cb_qed(void * arg);
static void hsu_close_rdm_cb_qed(void *arg);


/*===========================================================================
FUNCTION              HSU_CHANGE_COMP_RECONFIG_CB

DESCRIPTION
  wrapper function for hsu_rdm_close_device_cb to run on HSU AL task.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void hsu_rdm_close_device_cb_enq
(
  rdm_assign_status_type status,
  rdm_service_enum_type service,
  rdm_device_enum_type device
)
{
  hsu_rdm_close_device_cb_par_type *par = 
    jmalloc(sizeof(hsu_rdm_close_device_cb_par_type),M_ZERO);

  if(par == NULL)
  {
    HSU_ASSERT(par != NULL);
    return;
  }

  par->status = status;
  par->service = service;
  par->device = device;

  hsu_al_task_enqueue_cmd(
    hsu_rdm_close_device_cb_qed,
    par);
}

/*===========================================================================
FUNCTION              HSU_RDM_CLOSE_DEVICE_CB

DESCRIPTION
  The callback function to be called when rdm_close_device is completed. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void hsu_rdm_close_device_cb
(
  rdm_assign_status_type status,
   
  rdm_service_enum_type service,
  
  rdm_device_enum_type device
)
{
  hsu_change_comp_type *ctx = (hsu_change_comp_type *)&hsu_change_comp;
  rdm_service_enum_type rdm_service;
  rdm_device_enum_type rdm_device;

  if(status == RDM_DONE_S)
  {
    for(;ctx->curr_rdm_map_index < ctx->curr_rdm_map_size;
         ctx->curr_rdm_map_index++ )
    {
      rdm_device = ctx->curr_rdm_map[ctx->curr_rdm_map_index].rdm_device;
      rdm_service = rdm_get_service(rdm_device);
      if((rdm_device != RDM_NULL_DEV)&&(rdm_service != RDM_NULL_SRVC))
      {
        ctx->curr_rdm_map_index++; /* it is required becauase function returns
                                    * witout increase the index 
                                    */
        rdm_close_device(
          rdm_service, 
          rdm_device, 
          hsu_rdm_close_device_cb_enq);
        return;
      }
    }
  }

  if(ctx->rdm_close_cb) ctx->rdm_close_cb(TRUE);
}

/*===========================================================================
FUNCTION              HSU_RDM_CLOSE_DEVICE_CB_QED

DESCRIPTION
  wrapper function for hsu_rdm_close_device_cb to run on HSU AL task.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void hsu_rdm_close_device_cb_qed(void *arg)
{
  hsu_rdm_close_device_cb_par_type *par = 
    (hsu_rdm_close_device_cb_par_type *)arg;

  if(par == NULL)
  {
    HSU_ASSERT(par != NULL);
    return;
  }

  hsu_rdm_close_device_cb(
    par->status,
    par->service,
    par->device);
  jfree(par);
}

/*===========================================================================
FUNCTION              HSU_CLOSE_RDM

DESCRIPTION
  This function closes RDM ports in rdm_map. 

  rdm_map :  list of device and service pairs.
  size : the size of the list.
  cb_ptr : callback function called when the all of the ports are closed.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void hsu_close_rdm(
  hsu_rdm_dev_svc_pare_type *rdm_map,
  juint8_t  size,
  hsu_set_comp_cb_type cb_ptr)
{
    
  hsu_change_comp_type *ctx = (hsu_change_comp_type *)&hsu_change_comp;
  rdm_service_enum_type rdm_service;
  rdm_device_enum_type rdm_device;
  int i;

  /* if size == 0, call the callback function and finish closing ports */
  if(size == 0)
  {
    if(cb_ptr) cb_ptr(TRUE);
    return;
  }

  /* save the rdm_map into a static */
  ctx->curr_rdm_map_size = size;

  for(i=0; i<size; i++)
  {
    ctx->curr_rdm_map[i].rdm_device = rdm_map[i].rdm_device;
    ctx->curr_rdm_map[i].rdm_service= rdm_map[i].rdm_service;
  }

  /* start close the RDM port from index = 0 */
  for(ctx->curr_rdm_map_index = 0; 
         ctx->curr_rdm_map_index < 
         ctx->curr_rdm_map_size;
         ctx->curr_rdm_map_index++ )
  {
    rdm_device = ctx->curr_rdm_map[ctx->curr_rdm_map_index].rdm_device;
    rdm_service = rdm_get_service(rdm_device);
    if((rdm_device != RDM_NULL_DEV)&&(rdm_service != RDM_NULL_SRVC))
    {
      ctx->curr_rdm_map_index++; /* it is required becauase function returns
                                  * witout increase the index 
                                  */
      ctx->rdm_close_cb = cb_ptr;
      rdm_close_device(
        rdm_service, 
        rdm_device, 
        hsu_rdm_close_device_cb_enq);
      return;
    }
  }

  /* if reach here, it mean there is no RDM port to be closed. 
   * Finish the closing. */
  if(cb_ptr) cb_ptr(TRUE);

}

/*===========================================================================
FUNCTION              HSU_RDM_ASSIGN_PORT_CB_ENQ

DESCRIPTION
  wrapper function for hsu_rdm_assign_port_cb to run on HSU AL task.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void hsu_rdm_assign_port_cb_enq
(
  rdm_assign_status_type status,
  rdm_service_enum_type service,
  rdm_device_enum_type device
)
{
  hsu_rdm_assign_port_cb_par_type *par =
    jmalloc(sizeof(hsu_rdm_assign_port_cb_par_type),M_ZERO);

  if(par == NULL)
  {
    HSU_ASSERT(par != NULL);
    return;
  }

  par->status = status;
  par->service = service;
  par->device = device;

  hsu_al_task_enqueue_cmd(
    hsu_rdm_assign_port_cb_qed, 
    par);
}

/*===========================================================================

FUNCTION              HSU_OPEN_RDM

DESCRIPTION
  None
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hsu_rdm_assign_port_cb
(
  rdm_assign_status_type status,
   
  rdm_service_enum_type service,
  
  rdm_device_enum_type device
)
{
  hsu_change_comp_type *ctx = (hsu_change_comp_type *)&hsu_change_comp;
  rdm_service_enum_type rdm_service;
  rdm_device_enum_type rdm_device;

  if(status == RDM_DONE_S)
  {
    for(;ctx->curr_rdm_map_index < ctx->curr_rdm_map_size;
         ctx->curr_rdm_map_index++ )
    {
      rdm_device = ctx->curr_rdm_map[ctx->curr_rdm_map_index].rdm_device;
      rdm_service = ctx->curr_rdm_map[ctx->curr_rdm_map_index].rdm_service;
      if((rdm_device != RDM_NULL_DEV)&&(rdm_service != RDM_NULL_SRVC))
      {
        ctx->curr_rdm_map_index++; /* it is required becauase function returns
                                    * witout increase the index 
                                    */
        rdm_assign_port(
          rdm_service, 
          rdm_device, 
          hsu_rdm_assign_port_cb_enq);
        return;
      }
    }
  }

  if(ctx->rdm_open_cb) ctx->rdm_open_cb(TRUE);
}

/*===========================================================================
FUNCTION              HSU_RDM_ASSIGN_PORT_CB_QED

DESCRIPTION
  wrapper function for hsu_rdm_assign_port_cb to run on HSU AL task.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void hsu_rdm_assign_port_cb_qed(void *arg)
{
  hsu_rdm_assign_port_cb_par_type *par = 
    (hsu_rdm_assign_port_cb_par_type *)arg;

  if(par == NULL)
  {
    HSU_ASSERT(par != NULL);
    return;
  }

  hsu_rdm_assign_port_cb(
    par->status,
    par->service,
    par->device);

  jfree(par);
}


/*===========================================================================

FUNCTION              HSU_OPEN_RDM

DESCRIPTION
  Open RDM ports passed in rdm_map table. The cb_ptr is called when all of the 
  ports in rdm_map table are opened.

  rdm_map : pointer to the table of RDM service and RDM device pairs. 
  size : the size of the table pointed by rdm_map.
  cb_ptr : callback function to get notification of the processing status.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hsu_open_rdm
(
  hsu_rdm_dev_svc_pare_type *rdm_map,
  juint8_t  size,
  hsu_set_comp_cb_type cb_ptr
)
{
  hsu_change_comp_type *ctx = (hsu_change_comp_type *)&hsu_change_comp;
  rdm_service_enum_type rdm_service;
  rdm_device_enum_type rdm_device;
  int i;

  if((size == 0) || (rdm_map == NULL))
  {
    if(cb_ptr)cb_ptr(TRUE);
    return;
  }
  
  ctx->curr_rdm_map_size = size;

  for(i=0; i<size; i++)
  {
    ctx->curr_rdm_map[i].rdm_device = rdm_map[i].rdm_device;
    ctx->curr_rdm_map[i].rdm_service= rdm_map[i].rdm_service;
  }

  for(ctx->curr_rdm_map_index = 0; 
         ctx->curr_rdm_map_index < 
         ctx->curr_rdm_map_size;
         ctx->curr_rdm_map_index++ )
  {
    rdm_device = ctx->curr_rdm_map[ctx->curr_rdm_map_index].rdm_device;
    rdm_service = ctx->curr_rdm_map[ctx->curr_rdm_map_index].rdm_service;
    if((rdm_device != RDM_NULL_DEV)&&(rdm_service != RDM_NULL_SRVC))
    {
      ctx->curr_rdm_map_index++; /* it is required becauase function returns
                                  * witout increase the index 
                                  */
      ctx->rdm_open_cb = cb_ptr;
      rdm_assign_port(
        rdm_service, 
        rdm_device, 
        hsu_rdm_assign_port_cb_enq);
      return;
    }
  }

  if(cb_ptr) cb_ptr(TRUE);

}

/*===========================================================================
FUNCTION              HSU_CHANGE_COMP_RECONFIG_CB

DESCRIPTION
  The wapper function for hsu_open_rdm_cb to run on HSU AL task.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void hsu_open_rdm_cb_enq(boolean status)
{
  hsu_open_rdm_cb_par_type *par = 
    jmalloc(sizeof(hsu_open_rdm_cb_par_type),M_ZERO);

  if(par == NULL)
  {
    HSU_ASSERT(par != NULL);
    return;
  }
  par->status = status;
  hsu_al_task_enqueue_cmd(hsu_open_rdm_cb_qed, par);
}

/*===========================================================================
FUNCTION              HSU_OPEN_RDM_CB

DESCRIPTION
  The callback function called when hsu_open_rdm process is completed.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hsu_open_rdm_cb(boolean status)
{
  hsu_change_comp_type *ctx = &hsu_change_comp; 
  
  if(status == TRUE)
  {
    /* change the status to idle */
    ctx->state = HSU_SET_COMP_STATE_IDLE;

    /* call the callback function passed in hsu_set_composition */
    if(ctx->complete_cb)ctx->complete_cb(TRUE);
  }
}

/*===========================================================================
FUNCTION              HSU_CHANGE_COMP_RECONFIG_CB

DESCRIPTION
  The wapper function for hsu_open_rdm_cb to run on HSU AL task.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void hsu_open_rdm_cb_qed(void * arg)
{
  hsu_open_rdm_cb_par_type *par = (hsu_open_rdm_cb_par_type *)arg;
  
  if(par == NULL)
  {
    HSU_ASSERT(par != NULL);
    return;
  }

  hsu_open_rdm_cb(par->status);
  jfree(par);
}

/*===========================================================================
FUNCTION              HSU_CHANGE_COMP_RECONFIG_CB_ENQ

DESCRIPTION
  wrapper for hsu_change_comp_reconfig_cb to run on HSU AL task. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hsu_change_comp_reconfig_cb_enq(void *arg, jresult_t status)
{
  hsu_change_comp_reconfig_cb_par_type *par =
    (hsu_change_comp_reconfig_cb_par_type *)jmalloc(
      sizeof(hsu_change_comp_reconfig_cb_par_type),M_ZERO);

  if(par == NULL)
  {
    HSU_ASSERT(par != NULL);
    return;
  }

  par->arg = arg;
  par->status = status;

  hsu_al_task_enqueue_cmd(
    hsu_change_comp_reconfig_cb_qed,
    par);
}

/*===========================================================================
FUNCTION              HSU_CHANGE_COMP_RECONFIG_CB

DESCRIPTION
  The callback function called when hsu_conf_sel_switch_dev_stack process is 
  completed.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hsu_change_comp_reconfig_cb(void *arg, jresult_t status)
{
  hsu_change_comp_type *ctx = &hsu_change_comp; 
  
  /* start opening RDM ports passed in hsu_set_composition */
  HSU_ASSERT(ctx->new_rdm_map_size <= HSU_NUM_OF_RDM_USB_PORTS);
  hsu_open_rdm(ctx->new_rdm_map, ctx->new_rdm_map_size, hsu_open_rdm_cb_enq);
}

/*===========================================================================
FUNCTION              HSU_CHANGE_COMP_RECONFIG_CB_QED

DESCRIPTION
  wrapper for hsu_change_comp_reconfig_cb to run on HSU AL task. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void hsu_change_comp_reconfig_cb_qed(void * arg)
{
  hsu_change_comp_reconfig_cb_par_type *par = 
    (hsu_change_comp_reconfig_cb_par_type *)arg;

  if(par == NULL)
  {
    HSU_ASSERT(par != NULL);
    return;
  }

  hsu_change_comp_reconfig_cb(
    par->arg,
    par->status);

  jfree(par);  
}
/*===========================================================================
FUNCTION              hsu_close_rdm_cb_enq

DESCRIPTION
  wrapper for hsu_close_rdm_cb to run on HSU AL task. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void hsu_close_rdm_cb_enq(boolean status)
{
  hsu_close_rdm_cb_par_type *par = 
    jmalloc(sizeof(hsu_close_rdm_cb_par_type),M_ZERO);

  if(par == NULL)
  {
    HSU_ASSERT(par != NULL);
    return;
  }

  par->status = status;
  hsu_al_task_enqueue_cmd(hsu_close_rdm_cb_qed, par);
}

/*===========================================================================
FUNCTION              HSU_CLOSE_RDM_CB

DESCRIPTION
  callback function called when hsu_close_rdm function is completed.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void hsu_close_rdm_cb(boolean status)
{
  hsu_change_comp_type *ctx = (hsu_change_comp_type *)&hsu_change_comp;
  const hsu_conf_sel_comp_info_type * curr_comp = 
    hsu_conf_sel_comp_utils_get_comp_details_by_hsu_id(ctx->composition_id);

  HSU_ASSERT(curr_comp != NULL);
  
#ifndef T_UNDP /* UNDP does not support composition switching */
  if(status == TRUE)
  {
    /* do composition switching with new composition ID */
    hsu_conf_sel_switch_dev_stack(
            curr_comp->hsu_comp_id, 
            hsu_change_comp_reconfig_cb_enq,
            NULL);
  }
  else
  {
    /* call the callback function from hsu_set_compositon */
    ctx->state = HSU_SET_COMP_STATE_IDLE;
    if(ctx->complete_cb)ctx->complete_cb(FALSE);
  }
#else
  HSU_USE_PARAM(status);
#endif /* !T_UNDP */
}

/*===========================================================================
FUNCTION              HSU_CLOSE_RDM_CB_QED

DESCRIPTION
  wrapper for hsu_close_rdm_cb to run on HSU AL task. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void hsu_close_rdm_cb_qed(void *arg)
{
  hsu_close_rdm_cb_par_type *par = (hsu_close_rdm_cb_par_type *)arg;
  if(par == NULL)
  {
    HSU_ASSERT(par != NULL);
    return;
  }

  hsu_close_rdm_cb(par->status);
  jfree(par);
}

/*===========================================================================
FUNCTION              HSU_SET_COMPOSITON

DESCRIPTION
  The API hsu_set_composition does 3 steps to complete its operation. 
  
  1. Closing RDM port
  For RDM devices  which require closing before the devices are disabled, 
  corresponding RDM services are notified of closing the device. This step can 
  be avoid when the devices which will be disabled in new composition does not 
  require closing device. For example, the RDM_USB_DIAG_DEV doesn't require 
  closing. 
  The call back function for rdm_close_device will queue actual callback handler 
  into HSU AL task, using hsu_al_task_enqueue_cmd . It is to avoid the stack 
  overflow and serialize the close operation for multiple devices. 
  
  2. Switching composition
  When the last closing device operation is completed, switching composition 
  starts.  The callback function for hsu_conf_sel_switch_dev_stack will queue 
  actual callback handler into HSU AL task by hsu_al_task_enqueue_cmd to avoid 
  stack overflow. 
  
  3. Open RDM ports
  When the callback function for hsu_conf_sel_switch_dev_stack is called, 
  opening RDM ports notified by device and service pares of new_rdm_map the 
  starts. If there is no need to open new RDM ports for the new devices, 
  this step can be skipped by setting  new_rdm_map to NULL and 
  new_rdm_map_size = 0. Once openings all of ports are done, the callback 
  function passed in hsu_set_compostion, complete_cb. All of the operation are 
  done in HSU AL task context by serializing with hsu_al_task_enqueue_cmd.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hsu_set_compositon
(
  hsu_conf_sel_composition_id_enum  composition_id,
  hsu_set_comp_cb_type complete_cb,
  hsu_rdm_dev_svc_pare_type *old_rdm_map,
  juint8_t old_rdm_map_size,
  hsu_rdm_dev_svc_pare_type *new_rdm_map,
  juint8_t new_rdm_map_size
)
{
  hsu_change_comp_type *ctx = (hsu_change_comp_type *)&hsu_change_comp;
  int i;

  if(ctx->state != HSU_SET_COMP_STATE_IDLE)
  {
    if(complete_cb) complete_cb(FALSE);
    return;
  }
  
  ctx->state = HSU_SET_COMP_STATE_IN_PROGRESS;
  
  /* keep new RDM map that will be used for opening RDM ports */
  ctx->new_rdm_map_size = new_rdm_map_size;
  
  for(i =0; i<new_rdm_map_size; i++)
  {
    ctx->new_rdm_map[i].rdm_service = new_rdm_map[i].rdm_service;
    ctx->new_rdm_map[i].rdm_device = new_rdm_map[i].rdm_device;
  }

  /* keep rest of paramters for later use */
  ctx->composition_id = composition_id;
  ctx->complete_cb = complete_cb;

  /* close current RDM ports for HS USB */
  if(old_rdm_map != NULL)
  {
    hsu_close_rdm(old_rdm_map, old_rdm_map_size, hsu_close_rdm_cb_enq);
  }
  else
  {
    hsu_close_rdm(NULL, 0, hsu_close_rdm_cb_enq);
  }
}

/*===========================================================================
FUNCTION              HSU_GET_RDM_MAP

DESCRIPTION
  None
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
juint8_t hsu_get_rdm_map
(
  const hsu_conf_sel_comp_info_type * comp,
  hsu_rdm_dev_svc_pare_type *rdm_map,
  juint8_t max_rdm_map_size
)
{
  juint8_t i;
  juint8_t rdm_size;

  HSU_ASSERT(comp != NULL);
  
  rdm_size = comp->num_of_rdm_dev_in_comp;
  
  if(rdm_size > max_rdm_map_size)return 0;
  
  /* store current HS USB RDM map */
  if(rdm_map != NULL)
  {
    for(i=0; i<rdm_size; i++)
    {
      rdm_map[i].rdm_device = comp->rdm_devices[i];
      rdm_map[i].rdm_service= rdm_get_service(rdm_map[i].rdm_device); 
    }
  }

  return rdm_size;
}
