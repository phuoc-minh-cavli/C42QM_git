/*=============================================================================

                  S U B S Y S T E M   M O D U L E

GENERAL DESCRIPTION
  Implements Subsys framework for booting/shutting down various subsystems.


REGIONAL FUNCTIONS 

  subsys_init()
    Initializes the subsys module.
    
  subsys_print_debug_logs()
    Prints debug logs of subsys module. Debug purpose.
    
  subsys_load_segment(n)
    Loads nth modem segment at its phys addr. Debug purpose.

EXTERNALIZED FUNCTIONS

  subsys_start(subsys name)
    Starts requested subsystem.

INITIALIZATION AND SEQUENCING REQUIREMENTS

    subsys_init() must be called once before any other subsys functions.

Copyright (c) 2016, 2020 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/subsys/src/subsys.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

08/08/16   ab      File created.

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <stringl/stringl.h>
#include "rcinit.h"
#include "subsys.h"
#include "subsys_v.h"
#include "pil.h"
#include "pil_rmb.h"
#include "pil_utils.h"
#include "pil_os.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/
pil_mutex_obj_type subsys_glb_mutex;
pil_sig_obj_type subsys_sig_obj;

uint32 pil_seg_num = 0;
static pil_elf_struct_type *modem_handle = NULL;

/*-----------------------------------------------------------------------------
  Modem subsys global variables
-----------------------------------------------------------------------------*/
/* Can move all to a context structure when more subsystems handling comesup */
uint32 subsys_modem_vote_count = 0;
subsys_status_type subsys_modem_status = SUBSYS_IS_DOWN;
subsys_start_cb_fn subsys_modem_start_cbs[SUBSYS_CBS_MAX] = {0};
void * subsys_modem_start_cbs_data[SUBSYS_CBS_MAX] = {NULL};
uint32 subsys_modem_start_cbs_cnt = 0;
subsys_stop_cb_fn subsys_modem_stop_cbs[SUBSYS_CBS_MAX] = {0};
void * subsys_modem_stop_cbs_data[SUBSYS_CBS_MAX] = {NULL};
uint32 subsys_modem_stop_cbs_cnt = 0;
boolean subsys_modem_shutdown_done = FALSE;
boolean subsys_modem_hibernation_boot = FALSE;

pil_error_type pil_modem_boot_error;
pil_error_type pil_modem_shutdown_error;

//debug variables
uint32 subsys_boot_time=0;

/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

FUNCTION 
  pil_to_subsys_start_error

DESCRIPTION
  This function converts Pil error to Subsys Start error.

DEPENDENCIES
  None.

RETURN VALUE
  Subsys Start Error value

SIDE EFFECTS
  None.

=============================================================================*/
subsys_start_status_type pil_to_subsys_start_error(pil_error_type err)
{
  switch(err)
  {
    case PIL_SUCCESS: return SUBSYS_START_SUCCESS;
    case PIL_FAILURE: return SUBSYS_START_OTHER_FAILURE;
    case PIL_INVALID_SUBSYS: return SUBSYS_START_INVALID_SUBSYS;
    case PIL_INVALID_PARAMETERS: return SUBSYS_START_OTHER_FAILURE;
    case PIL_PARTITION_OPEN_FAILURE: return SUBSYS_START_FLASH_READ_ERROR;
    case PIL_TIMEOUT_ERROR: return SUBSYS_START_TIMEOUT_ERROR;
    case PIL_MALLOC_FAILURE: return SUBSYS_START_OTHER_FAILURE;
    case PIL_FLASH_READ_FAILED: return SUBSYS_START_FLASH_READ_ERROR;
    case PIL_MAPPING_FAILED: return SUBSYS_START_MMU_ERROR;
    case PIL_UNMAPPING_FAILED: return SUBSYS_START_MMU_ERROR;
    case PIL_ELF_HANDLE_NOT_INITED: return SUBSYS_START_FLASH_READ_ERROR;
    case PIL_ELF_HEADER_INVALID: return SUBSYS_START_INVALID_ELF_ERROR;
    case PIL_ELF_INVALID_SEGMENT_INDEX: return SUBSYS_START_INVALID_ELF_ERROR;
    case PIL_NON_LOADABLE_SEGMENT: return SUBSYS_START_INVALID_ELF_ERROR;
    default: return SUBSYS_START_OTHER_FAILURE;
  }
} /* pil_to_subsys_start_error */

/*=============================================================================

FUNCTION 
  pil_to_subsys_stop_error

DESCRIPTION
  This function converts Pil error to Subsys stop error.

DEPENDENCIES
  None.

RETURN VALUE
  Subsys Start Error value

SIDE EFFECTS
  None.

=============================================================================*/
subsys_stop_status_type pil_to_subsys_stop_error(pil_error_type err)
{
  switch(err)
  {
    case PIL_SUCCESS: return SUBSYS_STOP_SUCCESS;
    case PIL_FAILURE: return SUBSYS_STOP_FAILURE;
    //case PIL_INVALID_SUBSYS: return SUBSYS_INVALID_SUBSYS;
    //case PIL_INVALID_PARAMETERS: return SUBSYS_INVALID_PARAMETERS;
    //case PIL_PARTITION_OPEN_FAILURE: return SUBSYS_PARTITION_OPEN_FAILURE;
    //case PIL_TIMEOUT_ERROR: return SUBSYS_TIMEOUT_ERROR;
    //case PIL_MALLOC_FAILURE: return SUBSYS_MALLOC_FAILURE;
    //case PIL_FLASH_READ_FAILED: return SUBSYS_FLASH_READ_FAILED;
    //case PIL_MAPPING_FAILED: return SUBSYS_MAPPING_FAILED;
    //case PIL_UNMAPPING_FAILED: return SUBSYS_UNMAPPING_FAILED;
    //case PIL_ELF_HANDLE_NOT_INITED: return SUBSYS_ELF_HANDLE_NOT_INITED;
    //case PIL_ELF_HEADER_INVALID: return SUBSYS_ELF_HEADER_INVALID;
    //case PIL_ELF_INVALID_SEGMENT_INDEX: return SUBSYS_ELF_INVALID_SEGMENT_INDEX;
    //case PIL_NON_LOADABLE_SEGMENT: return SUBSYS_NON_LOADABLE_SEGMENT;
    default: return SUBSYS_STOP_FAILURE;
  }
} /* pil_to_subsys_stop_error */

/*=============================================================================

FUNCTION SUBSYS_TASK

DESCRIPTION
  Task in whose context Subsys Start/Shutdown are done.

DEPENDENCIES
  subsys_init() should be called before.

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/
void subsys_task(void)
{
  pil_sigs_type sigs;
  
  //Say Hello to RCinit
  rcinit_handshake_startup();
  
  for(;;)
  {
    sigs = pil_os_sig_wait(&subsys_sig_obj, SUBSYS_MODEM_START |
                                            SUBSYS_MODEM_SHUTDOWN |
                                            SUBSYS_PRINT_DEBUG_LOGS |
                                            SUBSYS_TEST_LOAD_NTH_SEG);
    
    if((sigs & SUBSYS_MODEM_START) == SUBSYS_MODEM_START)
    {
      subsys_start_status_type start_status;
      subsys_start_cb_fn start_cb;
      void *data;
      uint32 i;
      uint64 start_tt, end_tt;
      pil_boot_options_type options;
      
      pil_boot_options_init(options);
      
      pil_os_sig_clr(&subsys_sig_obj, SUBSYS_MODEM_START);
      PIL_LOG_INFO(1,"SUBSYS_MODEM_START: pil_boot modem start", 0); 
      
      pil_os_mutex_lock(&subsys_glb_mutex);
      if(subsys_modem_status == SUBSYS_IS_DOWN)
      {
        subsys_modem_status = SUBSYS_LOADING_STARTED;
        
        if(subsys_modem_hibernation_boot == TRUE)
          options.hiber_boot = TRUE;
        else
          options.hiber_boot = FALSE;
        
        pil_os_mutex_unlock(&subsys_glb_mutex);
        
        start_tt = pil_get_timetick();
        //Pil Boot the modem if its not already up
        pil_modem_boot_error = pil_boot(SUBSYS_MODEM_NAME, &options);
        end_tt = pil_get_timetick();
        
        subsys_boot_time = end_tt - start_tt;

        PIL_LOG_ERR(1, "subsys_boot_time = %d",subsys_boot_time);
        
        start_status = pil_to_subsys_start_error(pil_modem_boot_error);
        PIL_LOG_INFO(3,"SUBSYS_MODEM_START: pil_boot of Modem returned %d, start_status %d, time taken %d ticks", pil_modem_boot_error, start_status, (uint32)(end_tt - start_tt));
        
        pil_os_mutex_lock(&subsys_glb_mutex);
      }
      else
      {
        //if Modem is already up
        start_status = SUBSYS_START_SUCCESS;
        PIL_LOG_INFO(1,"SUBSYS_MODEM_START: Modem is already in up state", 0); 
      }
      
      if(start_status == SUBSYS_SUCCESS)
      {
        subsys_modem_status = SUBSYS_IS_UP;
      }
      else
      {
        subsys_modem_status = SUBSYS_LOADING_FAILED;
        //resetting votes
        subsys_modem_vote_count = 0;
      }
     
      //lock is already taken above
      
      //Notify clients who ever voted for Modem subsys_start
      while(subsys_modem_start_cbs_cnt != 0)
      {
        for(i=0, start_cb = NULL; i<SUBSYS_CBS_MAX; i++)
        {
          if(subsys_modem_start_cbs[i] != NULL)
          {
            start_cb = subsys_modem_start_cbs[i];
            data = subsys_modem_start_cbs_data[i];
            subsys_modem_start_cbs[i] = NULL;
            subsys_modem_start_cbs_cnt--;
            break;
          }
        }
        if(start_cb != NULL)
        {
          pil_os_mutex_unlock(&subsys_glb_mutex);
          
          //call client callback outside mutex
          start_cb(data, start_status);
          
          pil_os_mutex_lock(&subsys_glb_mutex);
        }
      }
      pil_os_mutex_unlock(&subsys_glb_mutex);
      PIL_LOG_LOW(3,"SUBSYS_MODEM_START: pil_boot modem end with err %d", pil_modem_boot_error, 0, 0); 
    }

   	if((sigs & SUBSYS_MODEM_SHUTDOWN) == SUBSYS_MODEM_SHUTDOWN)
    {
      subsys_stop_status_type stop_status;
      subsys_stop_cb_fn stop_cb;
      void *data;
      uint32 i;
      
      pil_os_sig_clr(&subsys_sig_obj, SUBSYS_MODEM_SHUTDOWN);
      PIL_LOG_INFO(1,"SUBSYS_MODEM_SHUTDOWN: Shutting down modem start", 0); 
      
      pil_os_mutex_lock(&subsys_glb_mutex);
      if(subsys_modem_status == SUBSYS_IS_UP)
      {
        subsys_modem_status = SUBSYS_SHUTTING_DOWN_STARTED;
        subsys_modem_shutdown_done = TRUE;  //Note that we have called shutdown after subsys is started once

        pil_os_mutex_unlock(&subsys_glb_mutex);

        pil_modem_shutdown_error = pil_shutdown(SUBSYS_MODEM_NAME);

        stop_status = pil_to_subsys_stop_error(pil_modem_shutdown_error);
        PIL_LOG_INFO(3,"SUBSYS_MODEM_SHUTDOWN: pil_shutdown of Modem returned pil err: %d, subsys err: %d", pil_modem_shutdown_error, stop_status, 0);
        
        pil_os_mutex_lock(&subsys_glb_mutex);
      }
      else
      {
        stop_status = SUBSYS_STOP_SUCCESS;
        PIL_LOG_INFO(1,"SUBSYS_MODEM_SHUTDOWN: Modem is already in shutdown state", pil_modem_shutdown_error); 
      }
      
      //Considering even failure case as DOWN state as modem may not be fully up 
      //after shutdown is initiated and can retry to start when requested again??
      //Todo: Check above assumption
      subsys_modem_status = SUBSYS_IS_DOWN;
      
      //lock is already taken above
      
      //Notify clients who ever registered for Modem subsys_stop
      while(subsys_modem_stop_cbs_cnt != 0)
      {
        for(i=0, stop_cb = NULL; i<SUBSYS_CBS_MAX; i++)
        {
          if(subsys_modem_stop_cbs[i] != NULL)
          {
            stop_cb = subsys_modem_stop_cbs[i];
            data = subsys_modem_stop_cbs_data[i];
            subsys_modem_stop_cbs[i] = NULL;
            subsys_modem_stop_cbs_cnt--;
            break;
          }
        }
        if(stop_cb != NULL)
        {
          pil_os_mutex_unlock(&subsys_glb_mutex);
          
          //call client callback outside mutex
          stop_cb(data, stop_status);
          
          pil_os_mutex_lock(&subsys_glb_mutex);
        }
      }
      pil_os_mutex_unlock(&subsys_glb_mutex);
      
 	    PIL_LOG_LOW(3,"SUBSYS_MODEM_SHUTDOWN: Shutting down modem end with status %d", pil_modem_shutdown_error, 0, 0); 

    }
    
    if((sigs & SUBSYS_PRINT_DEBUG_LOGS) == SUBSYS_PRINT_DEBUG_LOGS)
    {
      pil_os_sig_clr(&subsys_sig_obj, SUBSYS_PRINT_DEBUG_LOGS);
      MSG_3(MSG_SSID_TMS, MSG_LEGACY_ERROR,"SUBSYS_PRINT_DEBUG_LOGS: Printing Subsys Debug logs", 0, 0, 0);
      MSG_3(MSG_SSID_TMS, MSG_LEGACY_ERROR,"SUBSYS_PRINT_DEBUG_LOGS: Modem boot time: 0x%x, boot status: %d, shutdown status; %d", subsys_boot_time, pil_modem_boot_error, pil_modem_shutdown_error); 

      
      //Todo: can print more details like global variables etc things from each files.. Need to add later
      #ifdef PIL_MBA_FILTER
        pil_rmb_reg_log();
      #endif
    }
    
	  if((sigs & SUBSYS_TEST_LOAD_NTH_SEG) == SUBSYS_TEST_LOAD_NTH_SEG)
    {
      pil_error_type err;
      pil_os_sig_clr(&subsys_sig_obj, SUBSYS_TEST_LOAD_NTH_SEG);
	   PIL_LOG_INFO(1,"SUBSYS_TEST_LOAD_NTH_SEG: Modem nth segment loading function called for %d", pil_seg_num); 
	  
      if(modem_handle == NULL)
      {
          err = pil_elf_loader_init("0:QDSP", &modem_handle);
          PIL_LOG_INFO(1,"SUBSYS_TEST_LOAD_NTH_SEG: Modem init status returned 0x%x", err); 
          if(modem_handle == NULL || err != PIL_SUCCESS)
            break;
      }
      
      err = pil_elf_load_nth_segment(modem_handle, pil_seg_num);
	    PIL_LOG_INFO(1,"SUBSYS_TEST_LOAD_NTH_SEG: pil_elf_load_nth_segment returned 0x%x", err); 
    }
  } /* for (;;) */
} /* subsys_task */


/*=============================================================================

FUNCTION PIL_CHECK_IMEM_COOKIE

DESCRIPTION
  Reads Imem cookie and checks for PIL_IMEM_COOKIE value.
  If the value is present in Imem, will make timeouts to 0.

DEPENDENCIES

RETURN VALUE
  Value in imem address

SIDE EFFECTS

=============================================================================*/
extern uint32 pil_smp2p_wait_timeout_ms;
extern uint32 pil_rmb_wait_timeout_ms;

#ifdef PIL_MBA_FILTER
uint32 pil_check_imem_cookie(void)
{
  uint64 virt_addr = PIL_IMEM_ADDRESS;
  uint64 chunk_size = PIL_IMEM_SIZE;
  uint32 val;
  pil_error_type status = PIL_FAILURE;
  #ifdef PIL_DALSYS_AVAILABLE
  DALSYSMemHandle MemObj = 0;
  #else
  qurt_mem_region_t MemObj;
  #endif

  /* Creating a mapping for the IMEM address range */
  status = pil_map_and_get_virt_addr(PIL_IMEM_ADDRESS, chunk_size, &virt_addr, &MemObj);
  if(status != PIL_SUCCESS)
  {
    PIL_LOG_ERR(1,"pil_map_and_get_virt_addr returned error %d", status);
    return 0;
  }


  val = *(uint32 *)(unsigned int)virt_addr;
  
  PIL_LOG_INFO(3,"Imem Cookie value :0x%x", val);
  
  if(val == PIL_IMEM_COOKIE)
  {
    PIL_LOG_INFO(1,"found expected cookie, making all timeouts to 0", 0);
    pil_rmb_wait_timeout_ms = 0;
    pil_smp2p_wait_timeout_ms = 0;
  }
  else
  {
    PIL_LOG_INFO(2,"Didn't find expected cookie, making rmb timeout as %d, smp2p timeout as %d", PIL_RMB_WAIT_TIMEOUT_MS, PIL_SMP2P_WAIT_TIMEOUT_MS);
    pil_rmb_wait_timeout_ms = PIL_RMB_WAIT_TIMEOUT_MS;
    pil_smp2p_wait_timeout_ms = PIL_SMP2P_WAIT_TIMEOUT_MS;
  }

  /* Unmapping the entry as it is no longer required after this */
  pil_unmap_virt_addr(&MemObj);
  return val;
} /* pil_check_imem_cookie */
#endif /* PIL_MBA_FILTER */

/*=============================================================================

FUNCTION SUBSYS_START

DESCRIPTION
  Starts specified subsystem from subsys task context. Book keeps how many times
  start request came for given subsystem. Returns success/failure immediately which
  indicates only that request is taken. Actual subsys start status is returned in
  given callback.

DEPENDENCIES

RETURN VALUE
  SUBSYS_SUCCESS
  SUBSYS_INVALID_SUBSYS
  SUBSYS_NOT_ALLOWED

SIDE EFFECTS

=============================================================================*/
subsys_error_type subsys_start(char *str, subsys_start_options_type *options, subsys_start_cb_fn start_cb, void * cb_data)
{
  uint32 i;
  
  if(options == NULL)
    return SUBSYS_INVALID_PARAMETERS;
  
  if(pil_strcmp(str, SUBSYS_MODEM_NAME) == 0)
  {
    pil_os_mutex_lock(&subsys_glb_mutex);
    
    if(subsys_modem_shutdown_done == TRUE)
    {
      pil_os_mutex_unlock(&subsys_glb_mutex);
      //Not allowing subsys start after shutdown as that will trigger
      //SSR scenario and many modules on TN/TX doesnt handle SSR cases
      //When that is handled can remove this check
      PIL_LOG_ERR(1,"subsys_start: start after shutdown is not allowed", 0); 
      return SUBSYS_NOT_ALLOWED;
    }
    
    subsys_modem_vote_count++;
    
    PIL_VERIFY(subsys_modem_start_cbs_cnt < SUBSYS_CBS_MAX, "subsys registered callbacks max reached (%d >= %d)", subsys_modem_start_cbs_cnt, SUBSYS_CBS_MAX, 0);
    
    for(i=0; i<SUBSYS_CBS_MAX; i++)
    {
      if(subsys_modem_start_cbs[i] == NULL)
      {
        subsys_modem_start_cbs[i] = start_cb;
        subsys_modem_start_cbs_data[i] = cb_data;
        break;
      }
    }
    subsys_modem_start_cbs_cnt++;
    
    //Check if client wants Hibernation boot
    if(options->hibernate_boot == TRUE)
    {
      PIL_LOG_INFO(1,"hibernation boot is selected", 0); 
      subsys_modem_hibernation_boot = TRUE;
    }
    else
    {
      PIL_LOG_INFO(1,"normal boot is selected", 0); 
      subsys_modem_hibernation_boot = FALSE;    
    }
    
    pil_os_mutex_unlock(&subsys_glb_mutex);
    
    //Trigger modem start
    pil_os_sig_set(&subsys_sig_obj, SUBSYS_MODEM_START);
    
    return SUBSYS_SUCCESS;
  } /* if(SUBSYS_MODEM_NAME) */
  
  return SUBSYS_INVALID_SUBSYS ;
} /* subsys_start */


/*=============================================================================

FUNCTION SUBSYS_SHUTDOWN

DESCRIPTION
  Shutdowns specified subsystem from subsys task context. Book keeps how many times
  stop request came for given subsystem. Returns success/failure immediately which
  indicates only that request is taken. Actual subsys stop status is returned in
  given callback.

DEPENDENCIES
  Subsystem should be started before shutdown
  
RETURN VALUE
  SUBSYS_SUCCESS
  SUBSYS_INVALID_SUBSYS
  SUBSYS_NOT_ALLOWED

SIDE EFFECTS

=============================================================================*/
subsys_error_type subsys_shutdown(char *str, subsys_shutdown_options_type *options, subsys_stop_cb_fn stop_cb, void *cb_data)
{
  uint32 count, i;
  
  if(pil_strcmp(str, SUBSYS_MODEM_NAME) == 0)
  {
    pil_os_mutex_lock(&subsys_glb_mutex);
    
    if(subsys_modem_vote_count == 0)
    {
      pil_os_mutex_unlock(&subsys_glb_mutex);
      PIL_LOG_ERR(1,"subsys_shutdown: Modem is not up to do shutdown", subsys_modem_vote_count); 
      return SUBSYS_NOT_ALLOWED;
    }
    
    count = --subsys_modem_vote_count;
    
    PIL_VERIFY(subsys_modem_stop_cbs_cnt < SUBSYS_CBS_MAX, "subsys stop registered callbacks max reached (%d >= %d)", subsys_modem_stop_cbs_cnt, SUBSYS_CBS_MAX, 0);
    
    for(i=0; i<SUBSYS_CBS_MAX; i++)
    {
      if(subsys_modem_stop_cbs[i] == NULL)
      {
        subsys_modem_stop_cbs[i] = stop_cb;
        subsys_modem_stop_cbs_data[i] = cb_data;
        break;
      }
    }
    subsys_modem_stop_cbs_cnt++;
    
    pil_os_mutex_unlock(&subsys_glb_mutex);
    
    //Trigger Modem Shutdown
    if(count == 0)
      pil_os_sig_set(&subsys_sig_obj, SUBSYS_MODEM_SHUTDOWN);
    
    return SUBSYS_SUCCESS;
  }
  
  return SUBSYS_INVALID_SUBSYS ;
} /* subsys_shutdown */


/*=============================================================================

FUNCTION SUBSYS_PRINT_DEBUG_LOGS

DESCRIPTION
  Prints Subsystem Debug Logs. Used for Debugging purposes

DEPENDENCIES
  
  
RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/
void subsys_print_debug_logs(void)
{
  //Check if we can do this from here itself..
  pil_os_sig_set(&subsys_sig_obj, SUBSYS_PRINT_DEBUG_LOGS);
} /* subsys_print_debug_logs */


/*=============================================================================

FUNCTION SUBSYS_LOAD_SEGMENT

DESCRIPTION
  Loads given nth segment of modem in its physical memory. Used for Debugging 
  purposes

DEPENDENCIES
    
RETURN VALUE
  none

SIDE EFFECTS

=============================================================================*/
void subsys_load_segment(uint32 num)
{
  pil_seg_num = num;
  pil_os_sig_set(&subsys_sig_obj, SUBSYS_TEST_LOAD_NTH_SEG);
} /* subsys_load_segment */


/*=============================================================================

FUNCTION SUBSYS_INIT

DESCRIPTION
  Initializes Subsys Module

DEPENDENCIES
  
  
RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/
void subsys_init(void)
{
  pil_os_mutex_init(&subsys_glb_mutex);

  pil_os_sig_init(&subsys_sig_obj);
  
  pil_utils_init();
  
  //Check imem cookie and if present, reset timeouts
  #ifdef PIL_MBA_FILTER
    pil_check_imem_cookie();
  #endif /* PIL_MBA_FILTER */
  
  return;
} /* subsys_init */

