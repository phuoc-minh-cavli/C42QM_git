/**********************************************************************
 * dcvs_npa_qdsp.c
 *
 * This module implements the NPA layer for DCVS algorithm for QDSP
 *
 * EXTERNALIZED FUNCTIONS
 *
 * Copyright (C) 2009-2012 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 **********************************************************************/
/*=======================================================================
                        Edit History
$Header: //components/rel/core.tx/6.0/power/dcvs/src/common/dcvs_npa_qdsp.c#1 $ 
$DateTime: 2020/01/30 22:49:35 $

when       who     what, where, why
--------   ----    -------------------------------------------------------------------- 
08/24/12   sg      Log events on QDSS tracer.
03/29/12   nj      Added header macros that include target specific include files 
03/23/12   nj      Changed include file to be compatible with 8974 
02/03/12   nj      Boosted current priority inside ClkChangePriority function to avoid 
                   the callback function to be delayed in execution
09/30/11   ss      Removing the maximum MBps for the bus requests as 1024 MBps  .
09/25/11   sg      Moved requestStateVector definition from dcvs_npa.h file;
                   removed hard-coded startup node dependency.
09/12/11   ss      Correcting the bug while aggregating vector requests.
09/12/11   ss      Removing the MAX_THREADS division before making the state request from "/core/cpu"
                   resource.
09/12/11   ss      Restricting the maximum MBps for the bus requests as 1024 MBps  
04/26/11   ss      Fixing Q6 compiler warnings.
04/14/11   ss      Changing the initialization sequence to enable the clients to use the 
                   /core/cpu resource as soon as it is constructed rather than waiting 
                   for external system dependencies. 
03/25/11   ss      Adding the QDSP specific functionality to the file.
02/18/11   ss      Adding context based processing for DCVS.
01/21/11   ss      Initializing resource handle for each of the cores.  
10/22/10   ss      Using the DCVSCoreClockPlanIndexGet API to obtain index of clock plan table.
10/08/10   ss      Making the cpu frequency and bus frequency requests from the lookup 
                   table of clock plan frequencies of DCVS. 
09/14/10   ss      Removing the "Statement not reachable" and "Last line of the 
                   file ends without a newline" warnings.
08/12/10   ss      Renaming the /core/cpu client from "CPU Dynamics Timer"
                   to "CPU_Dynamics_Mips_Changed_or_Timer". 
07/08/10   ss      Requesting the maximum mips of the target at the
                   core_cpu_driver function . 
07/01/10   ss      Changing the type of pendingRequestMips and 
                   activeRequestMips to be of unsigned ints.
07/01/10   ss      Changing the requests made by the init client of clock 
                   and bus resources during start up.
06/17/10   ss      Removing unused NPA clients from the code.  
06/15/10   ss      Resetting the state of /core/cpu resource on entering
                   TCXO shutdown on 7x30 Modem.
04/08/10   ss      Adding a startup client to issue request for max. mips
                   before the actual DCVS algorithm starts.
03/30/10   ss      Adding the DCVS NPA layer event notification interfaces 
                   used to register / deregister for notifications.
03/30/10   ss      Adding real_dcvs_init to initialize the core cpu node 
                   and the remaining portion of the DCVS separately. 
02/21/10   ss      Invoking CPU monitor when the /clk/cpu resource
                   changes state. 
01/20/10   bjs     Cleanup
08/25/09   ss      Initial check in.
===========================================================================================*/
#include "BuildFeatures.h"
#ifndef DEFEATURE_DCVS_NPA_QDSP

#include "dcvs_kernel.h"
#include "dcvs_clkstub.h"
#include "dcvs_trace.h"
#include "dcvs_npa.h"
#include "dcvs_init.h"
#include "CoreString.h"
#include "CoreVerify.h"
#include "dcvs_target.h"
#include "CoreCpu.h"
#include "max_stub.h"

#include DCVS_OS_HEADER1_H

int32 cpuCount = MAX_THREADS;

/* Global variable having the vector requests of a client
   to the /core/cpu resource */
uint32* requestStateVector = NULL;

/*=======================================================================

                 DCVS_NPA EVENT NOTIFICATION function definitions

========================================================================*/

/**
  @brief dcvs_npa_events_initialize

  The function is used to initialize the event notification mechanism using
  the core event notification apis. 

  @param context: The DCVS Data context.
  
  @return : None.

*/
static void dcvs_npa_events_initialize(DCVS_CONTEXT* context)
{
  /* Obtain the client context. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;

  if(resDataContext->eventHandle == NULL) 
  {
    resDataContext->eventHandle = Core_EventInitialize(&context->resDataContext->notifyEvent, 
                                                       DCVS_NPA_EVENT_COUNT);
    CORE_VERIFY_PTR(resDataContext->eventHandle);
  }
  return;
}


/**
  @brief dcvs_npa_events_register

  The function is used to register for the event notification mechanism using
  the core event notification apis. 

  @param  event_id: The id which identifies the type of event .
  @param cb: The callback function which is notified when the registered event occurs.
  @param context: The DCVS Data context.

  @return : None.

*/
void dcvs_npa_events_register(dcvs_npa_event event_id, dcvs_npa_event_cb cb, 
                              DCVS_CONTEXT* context)
{
  /* Obtain the client context. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;  
  /* Make sure we are initialized */
  dcvs_npa_events_initialize(context);

  Core_EventRegister(resDataContext->eventHandle, event_id, (Core_EventCallback)cb, 
                     CORE_EVENT_PRIORITY_HIGHEST, context);

  return;
}

/**
  @brief dcvs_npa_events_deregister

  The function is used to deregister for the event notification mechanism using
  the core event notification apis. 

  @param  event_id: The id which identifies the type of event which is registered.

  @param cb: The callback function which is notified when the registered event occurs.
  @param context: The DCVS Data context.

  @return : None.

*/
void dcvs_npa_events_deregister(dcvs_npa_event event_id, dcvs_npa_event_cb cb, 
                                DCVS_CONTEXT* context)
{
  /* Obtain the client context. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;

  /* Make sure we are initialized */
  dcvs_npa_events_initialize(context);
    
  Core_EventDeRegister(resDataContext->eventHandle, event_id, (Core_EventCallback)cb, 
                       CORE_EVENT_PRIORITY_HIGHEST, context);

  return;
}

/**
  @brief dcvs_npa_events_notify

  The function is used to notify the clients for the registered events of type event_id .

  @param client_data: The DCVS Data context.
  
  @return : None.

*/
void dcvs_npa_events_notify(dcvs_npa_event event_id, DCVS_CONTEXT* context)
{
  /* Obtain the client context. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;

  /* Make sure we are initialized */
  dcvs_npa_events_initialize(context);

  Core_EventNotify(resDataContext->eventHandle, event_id, context);

  return;
}



/*=======================================================================

                  LOCAL FUNCTION DEFINITIONS

========================================================================*/


/**
  @brief DetermineMipsFloor
  
  This function is used to determine the floor mips request from the 
  given input vector .
  // Determine the minimum frequency needed.
  //   1. Determine maximum individual core request MIPS
  //   2. Determine excess floating core MIPS
  //   3. Subtract Excess from Floating Requests MIPS
  //   4. Divide remaining value by N cores, and add to #1
  //   5. Request max of #4 and algorithm calculated mips
  //   1-4 are only required when a new request is made.
  //   Since this state is MIPS, we should probably return expected MHz * cores.
  //   Of course there may be a conversion from MIPS to MHz.
  //requestStateVector[0]
  //  
  @param: The input request vector. 
  @return: The mips floor.

*/

static uint32 DetermineMipsFloor (DCVS_CONTEXT* context, uint32 * requestVector)
{
  int i;
  uint32 maxRequest, floatingRequests, maxMips;

  /* Obtain the resource data context. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;

  // Find the Maximum individual core request
  maxRequest = 0;
  maxMips = (resDataContext->maxMips / cpuCount);
  for(i = 1; i <= cpuCount; i++)
  {
    if (requestVector[i] > maxRequest)
    {
      maxRequest = requestVector[i];
    }
  }
  if (maxRequest >= maxMips)
  {
    return(resDataContext->maxMips);
  }
  // Now we have the max, and we have limited it to a per core request.
  // And we have the total requests.
  // Assume that floating work can be distributed evenly to all 
  // processors.  Distribute the load.
  floatingRequests = requestVector[0];
  for(i = 1; i <= cpuCount; i++)
  {
    if((requestVector[i] < maxRequest)&&(floatingRequests > 0))
    {
      if((maxRequest - requestVector[i]) > floatingRequests)
      {
        floatingRequests = 0;
      }
      else
      {
        floatingRequests -= (maxRequest - requestVector[i]);
      }
    }
  }
  // Now we have the floatingRequests that are not already covered.
  // Distribute those across all cores evenly.  Make sure we round up.
  maxRequest += (floatingRequests + cpuCount - 1) / cpuCount;
  if (maxRequest >= maxMips)
  {
    return(resDataContext->maxMips);
  }

  return(maxRequest * cpuCount);
}


/**
  @brief ResDataMemoryAllocateAndInitialize
  
  This function is used to allocate and initialize memory for the resource data.
  @param context: The DCVS data context.

  @return: None.

*/
static void ResDataMemoryAllocateAndInitialize(DCVS_CONTEXT* context)
{
  
  DCVS_RES_DATA* resDataContext;
  if(context->resDataContext == NULL)
  {
    /* Allocate the memory for storing the client information for the resources */
    if(DALSYS_Malloc(sizeof(DCVS_RES_DATA),  
                     (void**) &resDataContext)
                     != DAL_SUCCESS) 
    {
      /* Error fatal */
      CORE_VERIFY(NULL);   
    }
    /* Initialize the allocated memory. */
    memset(resDataContext, 0, sizeof(DCVS_RES_DATA));
    context->resDataContext = resDataContext;

  }
 
}

/**
  @brief res_core_cpu_update_fcn
  
  This function is invoked by the /core/cpu resource when a client request for new 
  mips is made. It gets the new mips from the algorithm .

  Note on active and pending requests of a client ::

  Each client has an active and pending request ( double buffered ) 

  The clients have 2 entries in the work buffer. One is used for the active request, other for the pending request.
  NPA_ACTIVE_REQUEST always returns the state in the current index of the buffer.
  NPA_PENDING_REQUEST always returns the state in the other index of the buffer.
  The index of the work buffer is shifted in the 
  npa_request_serviced ( from npa_client_complete_request) which is done after issuing request against the resource by the client.
  
  When a request against a resource is issued, then the update function of the node(having the resource) 
  is invoked. Inside the update function the pending state of the client would be the state just requested. 
  The return value of the update function represents the state request ( still pending request ) against the resource. 
  Then the corresponding driver function of the resource is called with the state request from the update function
  and the resource state is updated. 
  
  @param resource: A dynamic system element that work requests can be made against.
  @param client: The handle to the clients registered to the resource.

  @return : None.

*/
static npa_resource_state res_core_cpu_update_fcn( npa_resource *resource,
                                                   npa_client_handle client )
{
  npa_resource_state newMips = 0;

  /* Keep track of the new incoming requests for the client*/
  uint32  pendingRequestMips = 0;

  /* Keep track of the active request for the client*/
  uint32  activeRequestMips = 0;

  /* Obtain the DCSV data context which is stored in the resource. */
  DCVS_CONTEXT* context = (DCVS_CONTEXT*)resource->definition->data;

  /* Obtain the client context from the resource. */
  DCVS_RES_DATA* resDataContext= context->resDataContext;
  
  /* Ensure that the client is not NULL */
  CORE_VERIFY(client);


  switch (client->type)
  {
    case NPA_CLIENT_REQUIRED:
      /* Required client request*/

      /* Get the pending new request  from the client */
      pendingRequestMips = NPA_PENDING_REQUEST(client).state;

      // ToDo: Use resource max.
      if (pendingRequestMips > resDataContext->maxMips)
      {
        /* The requested mips is more than that supported by the target 
           Set the requested mips to the max mips on the target */
        pendingRequestMips = resDataContext->maxMips;
      }

      /* Get the previous active request of the client */
      activeRequestMips = NPA_ACTIVE_REQUEST(client).state;

      if (activeRequestMips > resDataContext->maxMips)
      {
        /* The previous active mips is more than that supported by the target 
         Set the  mips to the max mips on the target */
        activeRequestMips = resDataContext->maxMips;
      }

      resDataContext->clientDelta = pendingRequestMips - activeRequestMips;
      requestStateVector[0] = requestStateVector[0] + resDataContext->clientDelta;
      resDataContext->activeFloor = DetermineMipsFloor(context, requestStateVector);
      break;

    case NPA_CLIENT_VECTOR:
      {
        int i;
        int32 * clientData = (int32 *)client->resource_data;
        int32 * commandVector = &clientData[1];
        int32 * activeRequest = &clientData[clientData[0]+1];
        int32 * pendingRequest = (int32*)NPA_PENDING_REQUEST(client).pointer.vector;
        for(i = 0; i < clientData[0]; i++)
        {
          if (pendingRequest[i] > (int32)resDataContext->maxMips)
          {
            // The request is above our max mips.  Limit it to max.
            pendingRequest[i] = resDataContext->maxMips;
          }

          requestStateVector[commandVector[i]] += pendingRequest[i] - activeRequest[i];
          // Save the new request
          activeRequest[i] = pendingRequest[i];
        }
      }
      resDataContext->activeFloor = DetermineMipsFloor(context, &requestStateVector[0]);
      break;

    case NPA_CLIENT_IMPULSE:
      /* Impulse client request */
      resDataContext->activeImpulse = TRUE;
      break;

    case NPA_CLIENT_ISOCHRONOUS:
      /* Isochronous client request */
      if (NPA_PENDING_REQUEST(client).state == 0)
      {
        resDataContext->activeIsochronous--;
      }
      else
      {
        resDataContext->activeIsochronous++;
      }
      break;

    case NPA_CLIENT_INTERNAL:
      /* Internal client request */

      if (resDataContext->clkChangePriorityInit == 0)
      {
        // Only do this once
        resDataContext->clkChangePriorityInit = 1;

        // Read the current thread priority (from which the timer callback was
        // invoked) and store it for use as our clock change event priority.
        // We will use this priority (we're assuming it is fairly high)
        // instead of using the caller's priority when a clock change callback
        // is called - that way we ensure that we'll switch the clock at a
        // high priority even if the caller of clock change callback is of a
        // lower priority (CR 264711).
        resDataContext->clkChangePriority = DALSYS_SetThreadPriority(0);
      }

      /* DCVS Timer has kicked us.  Request new MIPS. */
      resDataContext->algoMipsChangedOrTimerFired = 1;
      break;

    default:
      break;
  } /* End of switch */

  if ( TRUE == resDataContext->dcvsInit )
  {
    // Determine the minimum frequency needed.
    //   1. Determine maximum individual core request MIPS
    //   2. Determine excess floating core MIPS
    //   3. Subtract Excess from Floating Requests MIPS
    //   4. Divide remaining value by N cores, and add to #1
    //   5. Request max of #4 and algorithm calculated mips
    //   1-4 are only required when a new request is made.
    //   Since this state is MIPS, we should probably return expected MHz * cores.
    //   Of course there may be a conversion from MIPS to MHz.
    //requestStateVector[0]
    /* DCVS subsystem is initialized . Get the new mips */
    newMips = DCVSCoreMipsGet(context);
  }
  else
  {
    /* Return the max mips for now till DCVS is initialized */
    newMips = resDataContext->maxMips*MAX_THREADS;
  }

  return newMips;
}


/**
  @brief node_core_cpu_driver_fcn

  This is the driver function for DCVS node. This function issues state requests
  to the clocks and bus. 

  @param resource: A dynamic system element that work requests can be made against.
  @param client: The handle to the clients registered to the resource.
  @param state: Work request type.

  @return : None.

*/
static npa_resource_state node_core_cpu_driver_fcn ( npa_resource *resource,
                                                     npa_client *client,
                                                     npa_resource_state state )
{
  uint32 khz, mb_s, mips_core;

  /* Obtain the core context from the resource */
  DCVS_CONTEXT* context = (DCVS_CONTEXT*)resource->definition->data;

  DCVS_RES_DATA* resDataContext = NULL;

  if(client->type == NPA_CLIENT_INITIALIZE)
  {
    /* Allocate the memory for the resource data if not already done so. */
    ResDataMemoryAllocateAndInitialize(context);

    resDataContext = context->resDataContext;

    resDataContext->maxMips = DCVSCoreTargetMaxFreqGet(context);

    /* Obtain the pointer to the clock plan of the target. */
    resDataContext->clockPlan = DCVSCoreClockPlanGet(context);

    /* This is the initialize client to the bus and clock resources */
    /* Initialize the resource max and active max for the /core/cpu reource. */
    resource->active_max = resDataContext->maxMips;
    resource->definition->max = resDataContext->maxMips;
    /* Initially request for max mips */
    state = resDataContext->maxMips;
  }

  if(resDataContext == NULL)
  {
    resDataContext = context->resDataContext;
  }

  // BJS, this will let initialization work properly.
  // it is not the final solution.
  // I should probably (for non /core/cpu resources) get the mips from the core, but
  // I still need to work through that portion of the 
  // startup sequence.
  if (resource == NPA_RESOURCE( resource, 0))
  {
    uint32 index;

    mips_core = state;
    
    index = DCVSCoreClockPlanIndexGet(mips_core, context); 
    
    /* Lookup the CPU frequency in KHz from the clock plan table. */
    khz = resDataContext->clockPlan->clockPlanTable[index].cpuFreq;
    
    mb_s = resDataContext->clockPlan->clockPlanTable[index].busThroughput;

    // Log this on QDSS
    DcvsTraceCpuDynamics(khz, mb_s);
    
    /* Request the state into the clock resource */
    npa_issue_required_request( NPA_DEPENDENCY(resource, 0), 
                                khz );

    /* Request the state into the bus resource */
    npa_issue_required_request( NPA_DEPENDENCY(resource, 1), 
                                mb_s );
  }

  return state;
}


/**
  @brief clk_cpu_change_cb

  This function is the callback which gets invoked everytime the
  state of /clk/cpu resource changes.

  @param user_data: The optional user data passed in this callback.
  @param cb_event_type:  NPA event type
  @param cb_data: NPA event data structure passed to this callback.
  @param size: size of the NPA event data structure.

  @return : None.

*/

static void *clk_cpu_change_cb( void *user_data, 
                                uint32 cb_event_type,
                                void *cb_data, 
                                uint32 size )
{
  /* Obtain the DCVS data context from the resource */
  DCVS_CONTEXT* context = (DCVS_CONTEXT*)user_data;

  /* Obtain the client context from the resource. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;
  
  if ( TRUE == resDataContext->dcvsInit )
  {
     uint32 previousPriority;

    /* Notify all the registered clients who requested for the callbacks 
	      from this event */
    previousPriority = DALSYS_SetThreadPriority(resDataContext->clkChangePriority);
    dcvs_npa_events_notify(DCVS_NPA_EVENT_CLK_CHANGE, context);
    DALSYS_SetThreadPriority(previousPriority);
  }
  return(NULL);
}



/**
  @brief DCVSInitializationCb

  This function is the callback after the file system and other DCVS system
  dependencies become available. After the /node/core/cpu is created  
  it waits for the system dependencies to be available during  
  system initialization and this call back is invoked.

  When invoked, the callback arguments will be
  (context, 0, *resource_name, strlen( resource_name ) );

  @param  context: The DCVS data context.
  @param  event_type: The type of the event.
  @param  data: The name of the node being created.
  @param  data_size: The length of the node name.

  @return : None.

*/
static void DCVSInitializationCb(void         *context,
                                 unsigned int event_type, 
                                 void         *data,       
                                 unsigned int data_size)
{
  DCVS_CONTEXT *dcvsContext = (DCVS_CONTEXT*)context;
  DCVS_RES_DATA  *resDataContext = dcvsContext->resDataContext;

  /* Initialize the kernel portion of DCVS */
  DCVSKernelInitialize(dcvsContext);
  /* Initialize the core data portion of the DCVS */
  DCVSCoreInitialize(dcvsContext);
  /* Mark the DCVS subsystem as initialized. */
  resDataContext->dcvsInit = TRUE;

}

/**
  @brief NodeCoreCpuCreationCb

  This function is the callback from the single resource node 
  created while initializing DCVS.  
  The callback will be invoked after the node is fully
  constructed.  The callback can be NULL if creation notification is
  unneeded. If invoked, the callback arguments are (context, 0,
  node->name, sizeof(const char *)).
  
  @param  context: The DCVS data context.
  @param  event_type: The type of the event.
  @param  data: The name of the node being created.
  @param  data_size: The length of the node name.

  @return : None.

*/
static void NodeCoreCpuCreationCb(  void         *context,
                                    unsigned int event_type, 
                                    void         *data,       
                                    unsigned int data_size )
{

  DCVS_CONTEXT *dcvsContext = (DCVS_CONTEXT*)context;
  /* Obtain the resource context */
  npa_resource_definition *resourceContext = (npa_resource_definition*)dcvsContext->resourceContext;
  DCVS_RES_DATA  *resDataContext;

  /* Query handle to NPA resource */
  npa_query_handle queryHandle;
  
  /* Allocate the memory for the resource data if not already done so. */
  ResDataMemoryAllocateAndInitialize(dcvsContext);

  resDataContext = dcvsContext->resDataContext;

   resDataContext->handleClient = npa_create_async_client(resourceContext->name,
                                                         "CPU_Dynamics_Mips_Changed",
                                                         NPA_CLIENT_INTERNAL,
                                                         NULL);
  CORE_VERIFY(resDataContext->handleClient);

  /* Create a startup only client for /core/cpu resource */
  resDataContext->startupClient = npa_create_async_client(resourceContext->name, 
                                                          "DCVS_Startup_Client",
                                                          NPA_CLIENT_REQUIRED,
                                                          NULL);
  CORE_VERIFY(resDataContext->startupClient);

  CORE_VERIFY_PTR( queryHandle = npa_create_query_handle(resourceContext->name) );
  CORE_VERIFY_PTR( resDataContext->resHandle = npa_query_get_resource( queryHandle ) );


  /* Set up the change event for the CPU monitor */
  /* Create a callback event for state change of /clk/cpu resource */
  CORE_DAL_VERIFY( DALSYS_EventCreate( DALSYS_EVENT_ATTR_CALLBACK_EVENT,
                                       &resDataContext->clockChangeEvent,
                                       NULL ) );

  /* Set up a callback function to be notified when coreCpuChangeEvent occurs */
  resDataContext->clkChangePriority = 237;
  CORE_DAL_VERIFY( DALSYS_SetupCallbackEvent( resDataContext->clockChangeEvent, 
                                               clk_cpu_change_cb, context ) );

  CORE_VERIFY( npa_create_change_event( dcvsContext->clockDependency, 
                                        "Clock_Change_Event", 
                                        resDataContext->clockChangeEvent) );

  /* Signal the succesful creation of node and the clients to the node */
  resDataContext->nodeCreateComplete = TRUE;

  /* Issue a request for maximum mips before the actual DCVS algorithm starts */
  npa_issue_required_request(resDataContext->startupClient, resDataContext->maxMips);

  #ifdef WINSIM
  /* Notify the test framework that the /node/core/cpu creation is complete */
  //  dcvs_npa_events_notify(DCVS_NPA_CORE_CPU_CREATE_COMPLETE, NULL);
  #endif
  

  /* Register and wait for the DCVS external system dependencies to be available */
  npa_resources_available_cb(dcvsContext->numSysDeps,
                             dcvsContext->sysDeps,
                             DCVSInitializationCb,
                             dcvsContext);
}


/**
  @brief dcvs_destroy_client_fcn

  This function is empty for now.

  @param  : None.

  @return : None.

*/
void dcvs_destroy_client_fcn( npa_client * client)
{
  if (client->resource_data != NULL)
  {
    DALSYS_Free(client->resource_data);
    client->resource_data = NULL;
  }
}

/**
  @brief dcvs_create_client_ex_fcn

  This function handles the various client data input. 
  If the client is a vector client,
  The vector is of the form of vector length, control vector and request vector.
  The vector length is the number of elements in the vector.
   
  Command vector is an array of ids of each HW thread [ 0 to 
  MAX_THREADS-1 / -1 represents floating requests. ] 
   
  Request vector is an array of requests on each corresponding 
  HW thread mentioned in the Control vector. Request Vector is 
  passed in during the issuing of requests and not during 
  creation of vector clients.  

  @param  client : The NPA client for the "/core/cpu" resource. 
  @param  vectorLength :  The length of the vector.
  @param  vector : The actual command vector passed in by the 
                 user .
  

  @return : None.

*/
unsigned int dcvs_create_client_ex_fcn( npa_client * client, unsigned int vectorLength, void * vector)
{
  uint32 *resourceData;
  int32 *cmdVector;
  uint32 i;
 

  // Handle vector client creation
  // If vectorLength is 0 and vector is NULL, then this is not a vector client.
  // 

  if (client->type == NPA_CLIENT_VECTOR)
  {
    /*
     When the clients create a vector we pass int he vector length and the command vector only.
     When issuing the request we pass in the request vector .
     Ex:: the length of vector is 3 and the actual vector could be [-1, 0, 1]
 
     The total length of the resourceData array is (num_entries*2 + 1)
     num_entries = vectorLength in this case
     resourceData is an array having the first index value as (num_entries) of vector.
     The first (num_entries) have the commands { ids of each HW thread [ 0 to MAX_THREADS-1 / -1 represents floating requests. ] }
     The remaining (num_entries) have the actual requests for each HW thread. This is not filled int his function. It is filled during usage time.
 
     Ex:: for the vector of length  3 and the actual vector  [-1, 0, 1]
     resourceData[0] = 3
     resourceData[1] = 0
     resourceData[2] = 1
     resourceData[3] = 2
     ....remaining empty...
     resourceData[4] = 0 
     resourceData[5] = 0
     resourceData[6] = 0
 
 
     */
    if ((vectorLength == 0) || (vector == NULL))
    {
      // How do I log to the NPA log?  This is a client creation error!
      // I can return a code, and the client creation will fail.  But,
      // how can I return a string to be logged?
    }
    CORE_VERIFY((vectorLength != 0) && (vector != NULL));

    // Allocate memory for storage of vector length, control vector,
    // and request vector.
    DALSYS_Malloc(((vectorLength * 2)+1)*sizeof(uint32), (void**)&resourceData);
    // Zero out the memory
    memset(resourceData, 0, ((vectorLength * 2)+1)*sizeof(uint32));

    CORE_VERIFY(resourceData != NULL);

    resourceData[0] = vectorLength;
    // Copy the command vector to the data storage area.  Modify it as necessary.
    cmdVector = vector;
    for(i = 0; i < vectorLength; i++)
    {
      if (cmdVector[i] >= cpuCount)
      {
        cmdVector[i] = -1;
      }
      resourceData[i+1] = cmdVector[i]+1;
    }

    // Assign the vector data to the appropriate field in the client.
    client->resource_data = resourceData;
  }

  return(0);  // Success
}


/* The plugin definition for /core/cpu resource */
const npa_resource_plugin res_core_cpu_plugin = 
{
  res_core_cpu_update_fcn,
  NPA_CLIENT_REQUIRED | 
  NPA_CLIENT_ISOCHRONOUS |
  NPA_CLIENT_IMPULSE |
  NPA_CLIENT_INTERNAL |
  NPA_CLIENT_VECTOR ,           /* Supported client types */
  NULL,                         /* Create client function */
  dcvs_destroy_client_fcn,      /* Destroy client function */
  dcvs_create_client_ex_fcn     /* Extended client create function */
};



/*=======================================================================

                 PUBLIC FUNCTION DEFINITIONS

========================================================================*/


void CoreCpuInitialize( void )
{

  // Allocate sufficient memory for the request vector
  DALSYS_Malloc((cpuCount+1)*sizeof(uint32), (void**)&requestStateVector);
  memset(requestStateVector, 0, (cpuCount+1)*sizeof(uint32));

  blast_profile_enable(1);
  blast_profile_reset_idle_pcycles();

  /* Initialize each core in the multicore environment of DCVS */
  DCVSTargetSpecificInit();

}


void DCVSTargetTimerFire(DCVS_CONTEXT* context)
{
  /* Obtain the client The DCVS data context. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;

  /* Proceed only if the /node/core/cpu creation is complete */
  if ( resDataContext->nodeCreateComplete == TRUE )
  {
    npa_issue_internal_request(resDataContext->handleClient);
  }
}


/**
  @brief DCVSInitialize

  This function is empty for now.

  @param  : None.

  @return : None.

*/
void DCVSInitialize(void)
{
  npa_define_marker("/init/dcvs/startup");
}

void DCVSStartupClientDestroy(DCVS_CONTEXT* context)
{
  /* Obtain the client The DCVS data context. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;

  npa_destroy_client(resDataContext->startupClient);
}

void DCVSZeroStateRequestIssue(DALBOOL sleepEntered, DCVS_CONTEXT* context)
{
  /* Obtain the client The DCVS data context. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;

  NPA_PENDING_REQUEST(resDataContext->zeroRequestClient).state = sleepEntered;
  npa_issue_internal_request(resDataContext->zeroRequestClient);

}
void CoreCpuResStateChange(uint32 state, DCVS_CONTEXT* context)
{
  /* Obtain the client The DCVS data context. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;

  NPA_PENDING_REQUEST(resDataContext->stateChangeClient).state = state;
  npa_issue_internal_request(resDataContext->stateChangeClient);

}



void NodeCoreCpuCreate (npa_node_dependency* targetNodeDeps, uint32 numNodeDeps, const char** targetSysDeps, uint32 numSysDeps, uint32 coreId, DCVS_CONTEXT* context)
{
  npa_node_definition *nodePtr = NULL; 
  /* Initial state of the resource */
  npa_resource_state initialState[] = 
  {
    0
  };

  /****** Create the resource memory **********/
  if(DALSYS_Malloc(sizeof(npa_resource_definition),  
                   (void**) &context->resourceContext)
                   != DAL_SUCCESS) 
  {
    /* Error fatal */
    CORE_VERIFY(NULL);   
  }
  memset(context->resourceContext, 0, sizeof(npa_resource_definition)); 

  /* Note that the target node dependencies information as well as system dependencies information need to be persistent till the 
     end . We just use pointers to those structures. The structures need to remain valid till the end. */

  /* Fill in the target system dependency information. Note that we are copying the pointer to the memory area where the system 
     dependency information is present . This meory area has to be persistent till the end. */
  context->sysDeps = targetSysDeps;
  context->numSysDeps = numSysDeps;


  /* Fill up the resource information. */  
  ((npa_resource_definition *)context->resourceContext)->name = "/core/cpu";                 /* Name of resource */
  ((npa_resource_definition *)context->resourceContext)->units = "MIPS";                      /* Units for this resource */
  ((npa_resource_definition *)context->resourceContext)->max = 0;                             /* Maximum state */
  ((npa_resource_definition *)context->resourceContext)->plugin = &res_core_cpu_plugin;     /* Plugin */
  ((npa_resource_definition *)context->resourceContext)->attributes = NPA_RESOURCE_DEFAULT;   /* Resource Attributes */
  ((npa_resource_definition *)context->resourceContext)->data = context;          /* User data field  */
  ((npa_resource_definition *)context->resourceContext)->query_fcn = NULL;                    /* Query fcn */

  
  /****** Create the memory for single resource node **********/
  if(DALSYS_Malloc(sizeof(npa_node_definition),  
                   (void**) &nodePtr)
                   != DAL_SUCCESS) 
  {
    /* Error fatal */
    CORE_VERIFY(NULL);   
  }  
  memset(nodePtr, 0, sizeof(npa_node_definition));
 
  nodePtr->name = "/node/core/cpu";                /* Node name */
  nodePtr->driver_fcn = node_core_cpu_driver_fcn;  /* Driver function used to actually update resource */
  nodePtr->attributes = NPA_NODE_DEFAULT;  /* Node Attributes */
  nodePtr->data = NULL;                    /* User supplied data */
  nodePtr->resources = ((npa_resource_definition *)context->resourceContext); /* The resources for this node. */
  nodePtr->resource_count = 1;             /* The number of resources */
  nodePtr->dependencies = targetNodeDeps;        /* The information about the dependencies */
  nodePtr->dependency_count = numNodeDeps;       /* The number of dependencies */

  /* The clock dependency name for the particular clock resource */
  context->clockDependency = targetNodeDeps[0].name;
    
  npa_define_node_cb(nodePtr, initialState, NodeCoreCpuCreationCb, context);
  
}

#endif //DEFEATURE_DCVS_NPA_QDSP

