/*==============================================================================
@file core_stubs.c

Implements stub functions.

Copyright (c) 2016 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.tx/6.0/power/stubs/core_stubs.c#1 $
==============================================================================*/

#include "DALStdDef.h"
#include "DALStdErr.h"
#include "mpmint.h"
#include "rpm.h"
#include "kvp.h"
#include "sleep_lpr.h"
#include "CoreObjectQueue.h"
#include "npa_remote_resource.h"
#include "devcfg.h"
#include "VCSDefs.h"

//#define NPA_STUB_ENABLE                

//#define NPA_REMOTE_STUB_ENABLE         

//#define RPM_STUB_ENABLE                

//#define COREOBJECTQUEUE_STUB_ENABLE    	 

//#define MPM_STUB_ENABLE			       

//#define SLEEP_STUB_ENABLE  		  

//#define CPR_ENABLE		  

#ifdef NPA_STUB_ENABLE

#include "npa.h"
#include "npa_resource.h"
#include "npa_transaction.h"

void npa_cancel_request( npa_client_handle client )
{
 
}


npa_client_handle npa_create_sync_client_ex( const char       *resource_name,
                                             const char       *client_name, 
                                             npa_client_type   client_type,
                                             unsigned int      client_value,
                                             void             *client_ref )

{
  npa_client *client;

  
  return client;
}

void npa_issue_scalar_request( npa_client_handle   client, 
                               npa_resource_state  state )
{
  return;
}

void npa_resource_available_cb( const char     *resource_name,
                                npa_callback    callback,
                                npa_user_data   context )
{
  
}
   
void npa_resources_available_cb( unsigned int   num_resources,
                                 const char    *resources[],
                                 npa_callback   callback,
                                 void          *context )
{

}								 
	   
void npa_assign_resource_state( npa_resource       *resource,
                                npa_resource_state  new_state )
{

}

void npa_issue_impulse_request( npa_client_handle client )
{

}

void npa_define_node_cb( npa_node_definition *definition,
                         npa_resource_state   initial_state[],
                         npa_callback         callback,
                         npa_user_data        context )
{

}

void npa_dispatch_custom_events( npa_resource *resource, 
                                 npa_event_type event_type, void *event_data )
{

}


npa_resource_state npa_no_client_update_fcn( npa_resource      *resource,
                                             npa_client_handle  client )
{
  /* This function should never be invoked */
  return 0;
}

/* Resource supports no clients - used for markers */
static void npa_no_client_create_fcn ( npa_client *client )
{

}


const npa_resource_plugin npa_no_client_plugin =
{
  npa_no_client_update_fcn,
  /* Supported client types */
  0,
  npa_no_client_create_fcn,     /* Create client function */
  NULL                          /* Destroy client function */
};

void npa_resource_lock( npa_resource *resource )
{

}

int npa_resource_trylock( npa_resource *resource )
{
  return 1;
}
/**
 * <!-- npa_resource_unlock -->
 *
 * @brief  Unlock the resource that was previously locked by
 * npa_resource_lock.
 *   
 * @param resource : Handle to the resource
 */
void npa_resource_unlock( npa_resource *resource )
{

}

void npa_update_resource_state( npa_resource      *resource,
                                npa_resource_state new_state )
{
  
}

void npa_define_marker( const char *marker_name )
{

}

void npa_alias_resource_cb( const char    *resource_name, 
                            const char    *alias_name,
                            npa_callback   callback,
                            npa_user_data  context )
{

}


npa_resource_state npa_identity_update_fcn( npa_resource      *resource,
                                            npa_client_handle  client )
{  
  return 1; 
}

npa_resource_state npa_max_update_fcn( npa_resource      *resource,
                                       npa_client_handle  client )
{
  return 1;
}
  
const npa_resource_plugin npa_identity_plugin = 
{
  npa_identity_update_fcn,
  0xffffffff,                   /* All client types supported */
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};
const npa_resource_plugin npa_max_plugin = 
{
  npa_max_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE | NPA_CLIENT_SUPPRESSIBLE2, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

void npa_issue_vector_request( npa_client_handle   client, 
                               unsigned int        num_elems, 
                               npa_resource_state *vector )
{

}

npa_client_handle npa_pass_request_attributes( npa_client_handle current,
                                               npa_client_handle dependency )
{
  return NULL;
}
npa_query_handle npa_create_query_handle( const char *resource_name )
{
  return NULL;
}
npa_resource *npa_query_get_resource( npa_query_handle query_handle )
{
  return query_handle->resource;
}

npa_status npa_remove_resource_log_by_handle( char *log_name,
                                              npa_resource *resource )
{
  return NPA_SUCCESS;
}

void npa_complete_request( npa_client_handle client )
{
  
}

void npa_set_request_attribute( npa_client_handle     client,
                                npa_request_attribute attr )
{

}

void npa_begin_transaction( npa_transaction_handle transaction )
{
  return;
}

npa_transaction_handle stub_transaction = (npa_transaction_handle) 0xDEADC0DE;

npa_transaction_handle npa_define_transaction( const char *transaction_name,
                                               unsigned int attributes,
                                               unsigned int num_resources, 
                                               const char *resources[] )
{
  return stub_transaction;
}

void npa_destroy_client( npa_client_handle client )
{
}

void npa_end_transaction( npa_transaction_handle transaction )
{
  return;
}
#endif 

#ifdef NPA_REMOTE_STUB_ENABLE

void 
npa_remote_define_resource_cb( npa_remote_resource_definition *definition, 
                               npa_resource_state              initial_state,
                               npa_callback                    define_callback,
                               npa_user_data                   define_context )
{

}

npa_resource_state 
npa_remote_resource_local_aggregation_driver_fcn( 
    npa_resource *resource, npa_client *client, npa_resource_state state )
{
  return state;
}

npa_resource_state 
npa_remote_resource_local_aggregation_no_initial_request_driver_fcn(
    npa_resource *resource, npa_client *client, npa_resource_state state )
{
  return state;
}
#endif

#ifdef RPM_STUB_ENABLE

void rpm_preallocate_resource_kvp(rpm_resource_type resource, unsigned id, unsigned key, unsigned expected_length)
{

}

unsigned rpm_msg_to_misc_resource_to_abort(void)
{
 return 1;
}

kvp_t *kvp_create(unsigned buffer_size)
{
  return NULL;
}

void kvp_put(kvp_t *self, unsigned type, unsigned length, const void *value)
{

}

void kvp_reset(kvp_t *self)
{

}

void kvp_clear(kvp_t *self)
{
}

void rpm_barrier(unsigned message_id)
{

}

void kvp_swapbuffer(kvp_t *self, const char *bytes, unsigned buffer_size)
{
}

void rpm_churn_queue(unsigned message_id)
{

}

unsigned rpm_get_request_size(rpm_set_type set, rpm_resource_type resource, unsigned id)
{
  return 1;
}

unsigned rpm_post_request(rpm_set_type set, rpm_resource_type resource, unsigned id, kvp_t *kvps)
{
  return 1;
}
#endif


#ifdef COREOBJECTQUEUE_STUB_ENABLE

int Core_CreteObjectQueue (Core_Object_List **ObjectQueue_list, CoreObjectProcessFcn PfnProcess)
{
	return 1;
}

int Core_ObjectEnqueue (Core_Object_List *list, void *pdata,unsigned int no_signal)
{
	return 1;
}
#endif


#ifdef MPM_STUB_ENABLE

/*
 * mpmint_config_gpio_wakeup
 */
void mpmint_config_gpio_wakeup
(
  uint32                which_gpio,
  mpmint_detect_type    detection,
  mpmint_polarity_type  polarity,
  mpmint_processor_type processor
)
{

}

void mpmint_disable_gpio_wakeup
(
  uint32 which_gpio
)
{

}

void mpmint_config_int
(
  mpmint_isr_type      int_num,     
  mpmint_detect_type   detection,
  mpmint_polarity_type polarity
)
{

}

void mpmint_config_wakeup
(
  mpmint_isr_type       int_num,
  mpmint_processor_type processor
)
{

}

void mpmint_disable_wakeup(mpmint_isr_type int_num)
{

}

uint32 mpm_get_proc_mpm_base_va( void )
{
   return 1;
}

void mpmint_trigger_interrupts( void )
{

}

void mpm_set_wakeup_timetick(uint64_t wakeup_tick)
{

}
#endif

#ifdef SLEEP_STUB_ENABLE

int sleep_define_lpr_str(const char *name, npa_client_handle client_handle)
{
   return 1;
}
#endif

#ifdef CPR_ENABLE

DALResult CPR_Disable( VCSRailType eRail )
{
  return DAL_SUCCESS;
}

DALResult CPR_Enable( VCSRailType eRail )
{
  return DAL_SUCCESS;
}

DALResult CPR_GetRailVoltageRecommendation( VCSRailType eRail, VCSCornerType eCorner, uint32 *nVoltageUV )
{
  switch (eCorner)
  {
    case VCS_CORNER_LOW:
	{
	  *nVoltageUV = 1050000;
	  break;
	}
	case VCS_CORNER_NOMINAL:
	{
	  *nVoltageUV = 1225000;
	  break;
	}
	case VCS_CORNER_TURBO:
	{
	  *nVoltageUV = 1350000;
	  break;
	}
  }
  return DAL_SUCCESS;
}

DALResult CprGetFloorAndCeiling( VCSRailType rail_id, VCSCornerType mode, uint32* floorVoltageUV, uint32* ceilingVoltageUV )
{
  *floorVoltageUV = 0;
  *ceilingVoltageUV = 0;
  switch (mode)
  {
    case VCS_CORNER_LOW:
	{
	  *floorVoltageUV = 1050000;
	  *ceilingVoltageUV = 1050000;
	  break;
	}
	case VCS_CORNER_NOMINAL:
	{
	  *floorVoltageUV = 1050000;
	  *ceilingVoltageUV = 1225000;
	  break;
	}
	case VCS_CORNER_TURBO:
	{
	  *floorVoltageUV = 1150000;
	  *ceilingVoltageUV = 1350000;
	  break;
	}
  }  
  return DAL_SUCCESS;
}
#endif

