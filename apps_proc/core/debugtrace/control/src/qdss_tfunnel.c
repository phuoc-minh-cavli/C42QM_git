/*=============================================================================

FILE:         qdss_tfunnel.c

DESCRIPTION:

================================================================================
              Copyright (c) 2013, 2019 QUALCOMM Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
$ Header: $
  when       who      what, where, why
--------   -------    ----------------------------------------------------------
02/23/19     kdey     Changes for IMEM address and SNOC BIMC clocks
==============================================================================*/
#include "qdss_tfunnel.h"
#include "qdss_control_priv.h"
#include "qdss_control.h"
#include "ClockDefs.h"
#include "qdss_osal.h"

#define QDSS_SNOC_NOMINAL_MODE_SPEED_KHZ     (200000)
#define QDSS_BIMC_NOMINAL_MODE_SPEED_KHZ     (400000)

//supplemental npa resources to be voted on for specific trace sources
typedef struct _qdss_plus_resource
{
   const char *resource_name;
   npa_resource_state state;
   npa_client_handle npa_client;
} qdss_plus_resource;



typedef struct _qdss_plus 
{
   const char *trace_source;
   qdss_plus_resource *resources;
}qdss_plus;


static qdss_plus_resource snoc_bimc_nominal[] =
      {
         {"/clk/bimc",QDSS_BIMC_NOMINAL_MODE_SPEED_KHZ,NULL},
         {"/clk/snoc",QDSS_SNOC_NOMINAL_MODE_SPEED_KHZ,NULL},
         {NULL,0,NULL} //should always end with this
      };


/*For ETM trace source additional SNOC BIMC clock vote. The entries for funnel ports are removed to get ETM traces.
  Without the changes ETM traces are not received.*/
static qdss_plus qdss_plus_tbl[] =
   {
      {NULL,NULL}//should always end with this
   };


/*Votes on a specific resource */
int qdss_plus_vote_resource(qdss_plus_resource *resource)
{
   int nErr= DAL_ERROR;
   if (NULL==resource->npa_client) {
      resource->npa_client = npa_create_sync_client(resource->resource_name,
                                                    "qdss_tfunnel",
                                                    NPA_CLIENT_SUPPRESSIBLE);
      THROW_IF(nErr,(NULL ==  resource->npa_client),DAL_ERROR);

      npa_issue_suppressible_request(resource->npa_client,
                                     resource->state);
   }
   nErr = DAL_SUCCESS;
   CATCH(nErr){}
   return nErr;
}

/*Find supplemental resource list for a trace source if it exists */
qdss_plus  *qdss_plus_find_node(const char *trace_source)
{
   qdss_plus *plus_node; //addons
   int len = strlen(trace_source);

   plus_node = qdss_plus_tbl;
   while (plus_node->trace_source!=NULL) {
      int this_len = strlen(plus_node->trace_source);
      if ( (this_len == len) &&
           (0==strncmp(plus_node->trace_source,trace_source,len) )) {
         return plus_node;
      }
      plus_node++;
   }
   return NULL;
}

/*Vote on supplemental resources for a trace source */
int qdss_plus_vote(const char *trace_source)
{
   int nErr;
   qdss_plus  *plus_node = qdss_plus_find_node(trace_source);
   qdss_plus_resource *resource_node;

   if (NULL==plus_node) {
      //No additional resources to vote on for this trace source
      return DAL_SUCCESS;
   }

   resource_node=plus_node->resources; 
   while (NULL!=resource_node->resource_name) {
      TRY(nErr,qdss_plus_vote_resource(resource_node));
      resource_node++;
   }

   nErr=DAL_SUCCESS;
   CATCH(nErr) {}
   return nErr;
}


static int qdss_ensure_funnel_ready(void);

/*-------------------------------------------------------------------------*/

/**
  @brief Disable all trace funnel ports

  @return 0 if successful, error code otherwise
 */
int qdss_tfunnel_disable_all_ports(void)
{
   int nErr;

   if (FALSE == qdss.ctrlMutex_enable) {
      return QDSS_CONTROL_BAD_STATE;
   }

   qdss_osal_mutex_lock(&qdss.ctrlMutex);
   TRY(nErr,qdss_ensure_funnel_ready());

   THROW_IF(nErr,DAL_SUCCESS !=
            DalTFunnel_DisableAll(qdss.hFunnel),
            QDSS_CONTROL_ERROR);

   CATCH(nErr){}
   qdss_osal_mutex_unlock(&qdss.ctrlMutex);
   return nErr;
}


/*-------------------------------------------------------------------------*/

/**
  @brief  Enable or disable a a tfunnel port

  @param            port_name [in ] :  port name
  @param              bEnable [in ] :  enable or disable

  @return 0 if successful, error code otherwise
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_STATE = A handle is not valid.
      QDSS_CONTROL_ERROR = Failed.
 */
static int qdss_tfunnel_toggle_port(const char *port_name,DALBOOL bEnable)
{
   int nErr;
   TFunnelControlParam control_param;

   if (FALSE == qdss.ctrlMutex_enable) {
      //TODO:
      //We were called before we were initialized
      //Should return failure here - but that messes up buspm
      //So silently returning success here for now
      return QDSS_CONTROL_SUCCESS;
   }

   qdss_osal_mutex_lock(&qdss.ctrlMutex);
   TRY(nErr,qdss_ensure_funnel_ready());

   control_param.pPortStr = port_name;
   control_param.Val.bEnable = bEnable;
   THROW_IF(nErr,DAL_SUCCESS !=
            DalTFunnel_Control(qdss.hFunnel,TFUNNEL_PORT_ENABLE,&control_param),
            QDSS_CONTROL_ERROR);

   CATCH(nErr){}
   qdss_osal_mutex_unlock(&qdss.ctrlMutex);
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Enable a trace funnel port

  @param            port_name [in ] :  port name

  @return 0 if successful, error code otherwise
 */
int qdss_tfunnel_enable_port(const char *port_name)
{
   int nErr=DAL_ERROR;

   TRY(nErr,qdss_plus_vote(port_name));
   TRY(nErr,qdss_tfunnel_toggle_port(port_name,TRUE));

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief     Disable a trace funnel port

  @param            port_name [in ] :  port name

  @return 0 if successful, error code otherwise
 */
int qdss_tfunnel_disable_port(const char *port_name)
{
   return qdss_tfunnel_toggle_port(port_name,FALSE);
}

/*-------------------------------------------------------------------------*/

/* Return
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_STATE = A handle is not valid.
*/
static int qdss_ensure_funnel_ready(void)
{
   int nErr;

   THROW_IF(nErr,(NULL == qdss.npa_client),QDSS_CONTROL_BAD_STATE);

   if (NULL == qdss.hFunnel) {
      npa_issue_required_request(qdss.npa_client, CLOCK_QDSS_LEVEL_DEBUG);
      THROW_IF(nErr,DAL_SUCCESS !=
               DAL_TFunnelDeviceAttach(DALDEVICEID_TFUNNEL_0,&qdss.hFunnel),
               QDSS_CONTROL_BAD_STATE);
   }

   nErr = QDSS_CONTROL_SUCCESS;

   CATCH(nErr){}

   return nErr;
}

/*-------------------------------------------------------------------------*/

/* Below are qdss control interface functions. Definitions of these
 * are in qdss_contro.h
 */

/*-------------------------------------------------------------------------*/

/* Return
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_STATE = A handle is not valid.
      QDSS_CONTROL_ERROR = Failed.
*/
int qdss_control_get_funnel(const char *name, int *state)
{
   int nErr = 0;
   TFunnelControlParam control_param;

   if (FALSE == qdss.ctrlMutex_enable) {
      return QDSS_CONTROL_BAD_STATE;
   }

   qdss_osal_mutex_lock(&qdss.ctrlMutex);
   TRY(nErr,qdss_ensure_funnel_ready());

   control_param.pPortStr = name;
   THROW_IF(nErr,DAL_SUCCESS !=
            DalTFunnel_Control(qdss.hFunnel,TFUNNEL_PORT_STATUS,&control_param),
            QDSS_CONTROL_ERROR);

   *state =  (control_param.Val.bEnable) ? 1:0;

   CATCH(nErr){}
   qdss_osal_mutex_unlock(&qdss.ctrlMutex);
   return nErr;
}

/*-------------------------------------------------------------------------*/

int qdss_control_set_funnel(const char *name, int state)
{
   if (state) {
      return qdss_tfunnel_enable_port(name);
   }
   else {
      return qdss_tfunnel_disable_port(name);
   }
}


