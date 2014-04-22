/*=========================================================================
                       QDSS INIT

GENERAL DESCRIPTION
   Provides APIs to initialize QDSS driver components and to also flush trace
   data from QDSS trace buffers.

EXTERNALIZED FUNCTIONS
   QDSSInit
   QDSSisEnabled
   QDSSFlush

INITIALIZATION AND SEQUENCING REQUIREMENTS
   QDSSInit is the first function that needs to be invoked in order to
   initialized QDSS driver components. QDSSInit assumes that the DAL component
   has been initialized prior to executing QDSSInit.

      Copyright (c) 2011-2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
==========================================================================*/

/*==========================================================================
 $Header: //components/rel/core.tx/6.0/debugtrace/common/src/qdss_init.c#1 $
==========================================================================*/

#include "comdef.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DDISTMCfg.h"
#include "DDITFunnel.h"
#include "DDISTMTrace.h"
#include "DDITMC.h"
#include "DDIHWEvent.h"
#if ((!defined(QDSS_STANDALONE_MODE)) && (!defined(TRACER_VENUS_BUILD)))
#include "npa.h"
#endif
#ifndef TRACER_VENUS_BUILD
#include <err.h>
#endif
#include "qdss.h"
#include "qdss_fuse_check.h"


/**
 * TODO: Until we get an updated "ClockDefs.h"
 * Clock QDSS levels
 */
typedef enum
{
  CLOCK_QDSS_LEVEL_OFF,      /* clock set to CXO, and off */
  CLOCK_QDSS_LEVEL_DEBUG,    /* clock rate follows VDD_CX */
  CLOCK_QDSS_LEVEL_HSDEBUG,  /* clock set to high speed */
} ClockQDSSLevelType;



typedef enum{
   QDSS_OFF=0,
   QDSS_READY,
   QDSS_IN_USE,
   QDSS_ENUM_LAST=0x7FFFFFFF
}QDSSState;

typedef struct {
   QDSSState eQDSSState;
   DALDEVICEHANDLE hTrace;
   DALDEVICEHANDLE hHWEvent;
   uint32 flush_port;
#ifdef QDSS_STANDALONE_MODE
   DALDEVICEHANDLE hSTMCfg,hFunnel,hTMC;
#endif
} QDSSStatus;

static QDSSStatus stQDSSStatus={QDSS_OFF};

#if ((!defined(QDSS_STANDALONE_MODE)) && (!defined(TRACER_VENUS_BUILD)))
static npa_client_handle qdss_npa_client=NULL;
#endif

#ifndef QDSS_DISABLED
static int32 QDSSAttachDrivers(void)
{
#ifdef QDSS_STANDALONE_MODE
   if(DAL_ERROR == DAL_TMCDeviceAttach("DALDEVICEID_TMC",&stQDSSStatus.hTMC))
   {
      return -1;
   }
   if(DAL_ERROR == DalTMC_SetMode(stQDSSStatus.hTMC,TMC_TRACESINK_ETB))
   {
      return -1;
   }
   if(DAL_ERROR == DalTMC_EnableTrace(stQDSSStatus.hTMC))
   {
     return -1;
   }

   if(DAL_ERROR == DAL_TFunnelDeviceAttach(DALDEVICEID_TFUNNEL_0,&stQDSSStatus.hFunnel))
   {
      return -1;
   }

   if(DAL_ERROR == DAL_STMCfgDeviceAttach(DALDEVICEID_STM_CONFIG,&stQDSSStatus.hSTMCfg))
   {
      return -1;
   }
   if (DAL_SUCCESS != DalSTMCfg_STMControl(qdss.hSTMCfg,STM_EN,1)) 
   {
      return -1;
   }

#endif /*QDSS_STANDALONE_MODE*/

   if(DAL_ERROR == DAL_STMTraceDeviceAttach(DALDEVICEID_STM_TRACE,&stQDSSStatus.hTrace))
   {
      return -1;
   }
   if( NULL == stQDSSStatus.hTrace)
   {
      return -1;
   }
   if(DAL_ERROR==DalSTMTrace_NewPort(stQDSSStatus.hTrace,&stQDSSStatus.flush_port))
   {
      return -1;
   }
   
   return 0;
}
#endif /*QDSS_DISABLED*/


boolean QDSSisInUse(void)
{
   return (stQDSSStatus.eQDSSState==QDSS_IN_USE);
}

static int32 QDSSClkOn(void)
{
#if ((!defined(QDSS_STANDALONE_MODE)) && (!defined(TRACER_VENUS_BUILD)))
if (qdss_npa_client==NULL) //No mutex. 
{
   qdss_npa_client = npa_create_sync_client("/clk/qdss", "qdss", NPA_CLIENT_REQUIRED);
   if (qdss_npa_client==NULL)
   {
      return -1;
   }
}
   npa_issue_required_request(qdss_npa_client, CLOCK_QDSS_LEVEL_DEBUG);
#endif
   return 0;
}

static int32 QDSSClkOff(void)
{
#if ((!defined(QDSS_STANDALONE_MODE)) && (!defined(TRACER_VENUS_BUILD)))
   if(qdss_npa_client!=NULL)
   {
      npa_issue_required_request(qdss_npa_client, CLOCK_QDSS_LEVEL_OFF);
      return 0;
   }
   else
   {
      return -1;
   }
#else
   return 0;
#endif
}

int32 QDSSOn(void)
{
   switch (stQDSSStatus.eQDSSState)
   {
      case QDSS_READY:
         if(0==QDSSClkOn())
         {
            stQDSSStatus.eQDSSState=QDSS_IN_USE;
            return 0;
         }
         else
         {
            return -1;
         }
      case QDSS_IN_USE:
         return 0;
      default:
         return -1;
   }
}

int32 QDSSOff(void)
{
   switch (stQDSSStatus.eQDSSState)
   {
      case QDSS_IN_USE:
         /*Do NOT turn off QDSS clocks if this flag is defined*/
         #ifndef QDSS_CLK_ALWAYS_ON
            if(QDSSClkOff()==0)
            {
               stQDSSStatus.eQDSSState=QDSS_READY;
               return 0;
            }
            else
            {
               return -1;
            }
         #else
            return 0;
         #endif
      case QDSS_READY:
         return 0;
      default:
         return -1;
   }
}

void QDSSInit(void)
{
   if(stQDSSStatus.eQDSSState == QDSS_OFF)
   {
      #ifndef QDSS_DISABLED
         /*Initialize QDSS drivers only once*/
         if(0==QDSSAttachDrivers())
         {
      /*TBD: in standalone mode skip efuse check */
      /*Register after ERR INIT is complete */
            if(qdss_fuse_trace_access()==1)
            {
               stQDSSStatus.eQDSSState=QDSS_READY;
            }
         }
      #endif
   }

/* QDSS_CLK_ALWAYS_ON - Force QDSS clocks to remain ON */
/**
* QDSS_STANDALONE_MODE - For standalone mode,switch modes without enabling/disabling QDSS clocks.
* The assumption is standalone mode does not have RPM present and hence NPA
* calls will not be required since QDSS clocks will be enabled externally via JTAG or
* by virtue of platform always enabling all clocks in the system by default.
**/

   #if (defined(QDSS_CLK_ALWAYS_ON)||defined(QDSS_STANDALONE_MODE) || \
        defined(TRACER_VENUS_BUILD))
      QDSSOn();
   #endif
   return;
}

void QDSSErrInit(void)
{
   if(stQDSSStatus.eQDSSState == QDSS_READY)
   {
#ifndef TRACER_VENUS_BUILD
      err_crash_cb_register(QDSSFlush);
#endif
   }
}


void QDSSFlush(void)
{
/*pre-requisite for flush is QDSS is in IN_USE state*/
   if((QDSSisInUse()==TRUE) && (stQDSSStatus.flush_port!=NULL) )
   {
      DalSTMTrace_Trigger(stQDSSStatus.hTrace,stQDSSStatus.flush_port,TRACE_TRIG_GUARANTEED|TRACE_TRIG_TIMESTAMPED);
   }
}

