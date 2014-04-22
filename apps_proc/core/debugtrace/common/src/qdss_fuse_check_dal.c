/*=========================================================================
                       QDSS e-fuse check

GENERAL DESCRIPTION 
   The interface to check efuses associated with the QDSS for accessing its 
   trace functionality is implemented. The fuses that are checked include 
   DBGEN, SPIDEN, NIDEN and SPNIDEN. The functionality controlled by the fuses 
   are as follows. DBGEN controls non-secure invasive debugging, SPIDEN controls
   secured invasive debugging, NIDEN controls non-secure non-invasive debugging 
   and SPNIDEN controls secure non-invasive debugging.

   According to the ARM Coresight architecture, in all cases, the fuse 
   setting for invasive debugging overrides settings for non-invasive debugging.
    
 
EXTERNALIZED FUNCTIONS
   qdss_fuse_trace_access
 
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
   Requires System FPB clock to access the efuse registers.
   

      Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
==========================================================================*/

/*========================================================================== 
 $Header: //components/rel/core.tx/6.0/debugtrace/common/src/qdss_fuse_check_dal.c#1 $
==========================================================================*/  

#include "qdss_fuse_check.h"
#include "DDISTMCfg.h"
#include "DALStdErr.h"
#include "DALDeviceId.h"
#include "qdss_throw.h"
#include "npa.h"
#include "ClockDefs.h"


/*
Input: None

Description:
   Checks the efuse associated with QDSS and returns a boolean on whether trace
   access is possible. Relies on the STM authentication status get a summary of 
   qdss fuse check.

Return: 
   0 - Trace access not possible
   1 - Trace access is possible
*/

struct _qdss_fuse_check
{
   DALDEVICEHANDLE hSTMCfg;
   npa_client_handle npa_client;
};

struct _qdss_fuse_check qdss_fuse_check;
  

unsigned int qdss_fuse_trace_access(void)
{
   int nErr = DAL_SUCCESS;

   if (NULL==qdss_fuse_check.hSTMCfg) {

      qdss_fuse_check.npa_client = npa_create_sync_client("/clk/qdss", "qdss_fuse_check", NPA_CLIENT_REQUIRED);

      if (NULL== qdss_fuse_check.npa_client) {
         THROW(nErr, DAL_ERROR);
      }
      npa_issue_required_request(qdss_fuse_check.npa_client, CLOCK_QDSS_LEVEL_DEBUG);

      // This will fail if AUTHSTATUS is not set
      nErr = DAL_STMCfgDeviceAttach(DALDEVICEID_STM_CONFIG,
                                    &qdss_fuse_check.hSTMCfg);

      npa_issue_required_request(qdss_fuse_check.npa_client, CLOCK_QDSS_LEVEL_OFF);
   }
   
   CATCH(nErr){}

   if (NULL==qdss_fuse_check.hSTMCfg)  {
      return FALSE;
   }
   //We were able to attach to STM config driver
   return TRUE;
}
