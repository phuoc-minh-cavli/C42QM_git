#ifdef PIL_MBA_FILTER

/*=============================================================================

        P E R I P H E R A L   I M A G E   L O A D E R    M O D U L E
                P I L   M B A   R M B   I n t e r f a c e

GENERAL DESCRIPTION
  Implements Interface functions to access MBA RMB Registers

EXTERNALIZED FUNCTIONS
  pil_rmb_check_if_mba_unlocked()
  
  pil_rmb_populate_meta_info()
  
  pil_rmb_check_metadata_auth_status()
  
  pil_rmb_set_pmi_code_start_addr()
  
  pil_rmb_set_pmi_elf_load_ready()
  
  pil_rmb_set_pmi_total_code_length()
  
  pil_rmb_verify_blob()
  
  pil_rmb_check_subsys_status()
  
  pil_rmb_reg_log()
  
  pil_rmb_set_mba_image_addr()
  
  pil_rmb_deinit_mba()
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2016, 2020 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/subsys/src/pil_rmb.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

09/15/16   ab      File created.

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "pil.h"
#include "pil_rmb.h"
#include "pil_hwio.h"
#include "pil_utils.h"
#include "pil_os.h"
#include "msg.h"
#include "timer.h"
#include <HALhwio.h>

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/
#ifdef BOOT_PRE_SILICON
/*Skip all MBA image handshake status checks on RUMI before MBA is ready, turn it off when MBA is ready */
static boolean boot_rmb_status_chk_skip = TRUE;
#else
static boolean boot_rmb_status_chk_skip = FALSE;
#endif

boolean pil_rmb_reg_access_allowed = FALSE;
uint32 pil_rmb_wait_timeout_ms=0;
uint32 pil_rmb_reg_log_arr[RMB_REGISTERS_COUNT]={0};

/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

FUNCTION 
  pil_rmb_check_if_mba_unlocked

DESCRIPTION
  This function checks if MBA has unlocked XPUs etc from MBA Status register

DEPENDENCIES
  Need to update MBA in DDR and bring MBA out of reset before this.

RETURN VALUE
  PIL_TIMEOUT_ERROR - If timeout happens waiting for MBA unlock
  PIL_FAILURE - If MBA didnt unlocked or on some error
  PIL_SUCCESS - If MBA has unlocked

SIDE EFFECTS
  None.

=============================================================================*/
pil_error_type pil_rmb_check_if_mba_unlocked ( void )
{
  int32 rmb_status = 0x0;
  pil_wait_obj_type wait_obj;
  boolean timeout = FALSE;
  
  /* ensure elf copying is completed before indicating to MBA */
  pil_os_mem_barrier();
  
  PIL_LOG_LOW(1,"reading rmb_status with timeout %d", pil_rmb_wait_timeout_ms);

  pil_os_wait_obj_init(&wait_obj);
  pil_os_start_wait_obj(&wait_obj, pil_rmb_wait_timeout_ms);
  /* Wait for MBA image to unlock XPU. Do not proceed meta info population
    till MBA is ready
  */
  rmb_status =(int32)HWIO_IN(RMB_MBA_STATUS);
  while(!(rmb_status == MBA_XPU_UNLOCKED || rmb_status == MBA_XPU_UNLOCKED_MEM_SCRIBBLED || rmb_status < 0) && !boot_rmb_status_chk_skip)
  {
    if(pil_os_check_wait_obj(&wait_obj) == TRUE)
    {
      timeout = TRUE;
      break;
    }
    timer_sleep(5, T_MSEC, TRUE);
    rmb_status =(int32)HWIO_IN(RMB_MBA_STATUS);
  }
  pil_os_wait_sig_obj_deinit(&wait_obj);
  
  if(timeout == TRUE)
  {
    PIL_LOG_ERR(1,"MBA unlocking timedout %d", rmb_status);
    return PIL_TIMEOUT_ERROR;
  }
  
  PIL_LOG_INFO(1,"rmb_status %d", rmb_status);
  
  /* TO-DO: ensure mba sets these status's.*/
  if(!((MBA_XPU_UNLOCKED == rmb_status) ||
             (MBA_XPU_UNLOCKED_MEM_SCRIBBLED == rmb_status) ||
             (boot_rmb_status_chk_skip)))
     return PIL_FAILURE;
  
  return PIL_SUCCESS;
} /* pil_rmb_check_if_mba_unlocked */


/*=============================================================================

FUNCTION 
  pil_rmb_populate_meta_info

DESCRIPTION
  This function signals subsys authenticator image (eg MBA image)
  the location of elf meta information buffer.

DEPENDENCIES
  The buffer should be filled with Modem Metadata

RETURN VALUE
 None

SIDE EFFECTS
  None.

=============================================================================*/
void pil_rmb_populate_meta_info ( uint8 *metadata_addr )
{
  /* ensure elf copying is completed before indicating to MBA */
  pil_os_mem_barrier();

  /* Flush cache  */
  //Note: Cache flush may not be necessary since we will be using non-cached memories while copying data
  //pil_os_cache_flush();

  /* Set RMB_PMI_META_DATA with the physical address of the meta-data */
  HWIO_OUT(RMB_PMI_META_DATA, (uint32)metadata_addr);

  /* Set the RMB_MBA_COMMAND to 0x1 to start the Meta Data
     authentication/validation.
  */
  HWIO_OUT(RMB_MBA_COMMAND, (uint32)CMD_PMI_META_DATA_READY);

} /* pil_rmb_populate_meta_info */


/*=============================================================================

FUNCTION 
  pil_rmb_check_metadata_auth_status

DESCRIPTION
  This function checks for metadata authentication status

DEPENDENCIES
  Need to provide Metadata buffer to MBA before this

RETURN VALUE
  PIL_TIMEOUT_ERROR - If timeout happens waiting for authentication status
  PIL_FAILURE - If Metadata is not authenticated successfully
  PIL_SUCCESS - If Metadata is authenticated successfully

SIDE EFFECTS
  None.

=============================================================================*/
pil_error_type pil_rmb_check_metadata_auth_status ( void )
{
  int32 rmb_status = 0x0;
  pil_wait_obj_type wait_obj;
  boolean timeout = FALSE;
  
  /* ensure elf copying is completed before indicating to MBA */
  pil_os_mem_barrier();
  
  PIL_LOG_LOW(1,"reading rmb_status", 0);

  pil_os_wait_obj_init(&wait_obj);
  pil_os_start_wait_obj(&wait_obj, pil_rmb_wait_timeout_ms);
  
  /* Wait for MBA image to unlock XPU. Do not proceed meta info population
    till MBA is ready
  */
  rmb_status =(int32)HWIO_IN(RMB_MBA_STATUS);
  while(!(rmb_status == MBA_PMI_META_DATA_AUTHENTICATION_SUCCESS || rmb_status < 0) && !boot_rmb_status_chk_skip)
  {
    if(pil_os_check_wait_obj(&wait_obj) == TRUE)
    {
      timeout = TRUE;
      break;
    }
    timer_sleep(5, T_MSEC, TRUE);
    rmb_status =(int32)HWIO_IN(RMB_MBA_STATUS);
  }
  pil_os_wait_sig_obj_deinit(&wait_obj);
  
  PIL_LOG_INFO(1,"rmb_status %d", rmb_status);
  
  if(timeout == TRUE)
  {
    PIL_LOG_ERR(1,"checking mba status timedout %d", rmb_status);
    return PIL_TIMEOUT_ERROR;
  }
  
  /* TO-DO: ensure mba sets these status's.*/
  if(!((MBA_PMI_META_DATA_AUTHENTICATION_SUCCESS == rmb_status) ||
             (boot_rmb_status_chk_skip)))
     return PIL_FAILURE;
  
  return PIL_SUCCESS;
} /* pil_rmb_check_metadata_auth_status */


/*=============================================================================

FUNCTION 
  pil_rmb_set_pmi_code_start_addr

DESCRIPTION
  This function signals subsys authenticator image (eg MBA image)
  the start of first elf segment.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None.

=============================================================================*/
void pil_rmb_set_pmi_code_start_addr ( unsigned int  pmi_code_start_addr )
{
  /* ensure elf copying is completed before indicating to MBA */
  pil_os_mem_barrier();
  
  /* Flush cache  */
  //Cache flush may not be necessary since we will be using non-cached memories while copying data
  //pil_os_cache_flush();

  /*
    Program the RMB_PMI_CODE_START to the start address of the first
    loadable segment.
  */
  HWIO_OUT(RMB_PMI_CODE_START, pmi_code_start_addr);
} /* pil_rmb_set_pmi_code_start_addr */


/*=============================================================================

FUNCTION 
  pil_rmb_set_pmi_elf_load_ready

DESCRIPTION
  This function signals subsys authenticator image (eg MBA image)
  to start the elf load

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None.

=============================================================================*/
void pil_rmb_set_pmi_elf_load_ready ( void )
{
  /* ensure elf copying is completed before indicating to MBA */
  pil_os_mem_barrier();
  
  /* Flush cache  */
  //Cache flush may not be necessary since we will be using non-cached memories while copying data
  //pil_os_cache_flush();

  /* Set the RMB_MBA_COMMAND to 0x2 to start the PMI
    authentication/validation.
  */
  HWIO_OUT(RMB_MBA_COMMAND, (uint32)CMD_PMI_SEGMENTED_LOAD_READY);

  /*
    RMB_MBA_STATUS of MBA_EXECUTE_SUCCESS indicates MBA executed successfully
    and was able to jump to modem image. This status can be checked at end
    of boot. 
  */
} /* pil_rmb_set_pmi_elf_load_ready */


/*=============================================================================

FUNCTION 
  pil_rmb_set_pmi_total_code_length

DESCRIPTION
  This function signals subsys authenticator image (eg MBA image)
  the total loaded elf length.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None.

=============================================================================*/
void pil_rmb_set_pmi_total_code_length ( uint32 pmi_code_length )
{
  /* ensure elf copying is completed before indicating to MBA */
  pil_os_mem_barrier();
  
  /* Flush cache before Updating pmi length */
  //Cache flush may not be necessary since we will be using non-cached memories while copying data
  //pil_os_cache_flush();
  
  PIL_LOG_INFO(1,"length passed: %d", pmi_code_length);

  /* Set RMB_PMI_CODE_LENGTH to whole modem image length  */
  HWIO_OUT(RMB_PMI_CODE_LENGTH, (uint32)pmi_code_length);
}  /* pil_rmb_set_pmi_total_code_length */


/*=============================================================================

FUNCTION 
  pil_rmb_verify_blob

DESCRIPTION
  This function increments length to RMB length register from previous value 
  and checks status for any error. This is called after each modem segment is 
  loaded to memory.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None.

=============================================================================*/
pil_error_type pil_rmb_verify_blob(uint64 size)
{
  uint32 len;
  int32 mba_status = 0;
  
  /* ensure elf copying is completed before indicating to MBA */
  pil_os_mem_barrier();

  /* Flush cache  */
  //Cache flush may not be necessary since we will be using non-cached memories while copying data
  //pil_os_cache_flush();
  
  PIL_LOG_INFO(1,"length passed: %d", (uint32)size);
  
  len = HWIO_IN(RMB_PMI_CODE_LENGTH);
  len += size;
  HWIO_OUT(RMB_PMI_CODE_LENGTH, (uint32)len);

  /* ensure reg update is completed before checking status */
  pil_os_mem_barrier();
  
  mba_status = (int32)HWIO_IN(RMB_MBA_STATUS);
  if((mba_status <0) && !boot_rmb_status_chk_skip)
  {
     PIL_LOG_ERR(1,"mba status: %d", mba_status);
     return PIL_FAILURE;
  }
  
  return PIL_SUCCESS;
} /* pil_rmb_verify_blob */


/*=============================================================================

FUNCTION 
  pil_rmb_check_subsys_status

DESCRIPTION
  This function Checks if subsystem PBL and auth(eg MBA) images are able to
  boot successfuly.

DEPENDENCIES
  Should be called after loading all Modem segments and updating length value

RETURN VALUE
 PIL_SUCCESS
 PIL_TIMEOUT_ERROR
 PIL_FAILURE

SIDE EFFECTS
  None.

=============================================================================*/
pil_error_type pil_rmb_check_subsys_status( void )
{
  int32 rmb_status, pbl_status;
  pil_wait_obj_type wait_obj;
  boolean timeout = FALSE;
  
  /* ensure elf copying is completed before indicating to MBA */
  pil_os_mem_barrier();
  
  pil_os_wait_obj_init(&wait_obj);
  pil_os_start_wait_obj(&wait_obj, pil_rmb_wait_timeout_ms);
  
  /* Check if subsys PBL was able to jumb to authenticator image (MBA)
    successfully via rmb regiter 01, ie RMB_PBL_STATUS.
  */
  pbl_status =(int32)HWIO_IN(RMB_PBL_STATUS);
  while((pbl_status == RMB_REG_UNINITIALIZED) && !boot_rmb_status_chk_skip)
  {
    if(pil_os_check_wait_obj(&wait_obj) == TRUE)
    {
      timeout = TRUE;
      break;
    }
    timer_sleep(5, T_MSEC, TRUE);
    pbl_status =(int32)HWIO_IN(RMB_PBL_STATUS);
  }
  pil_os_wait_sig_obj_deinit(&wait_obj);
  
  PIL_LOG_INFO(1,"pbl status: %d", pbl_status);
  
  if(timeout == TRUE)
  {
    PIL_LOG_ERR(1,"checking pbl status timedout %d", pbl_status);
    return PIL_TIMEOUT_ERROR;
  }
  
  /* A negative value indicates error */
  if((pbl_status < 0) || (boot_rmb_status_chk_skip)) 
  {
    PIL_LOG_ERR(1,"wrong pbl status: %d", pbl_status);
    return PIL_FAILURE;
  }

  
  pil_os_wait_obj_init(&wait_obj);
  pil_os_start_wait_obj(&wait_obj, pil_rmb_wait_timeout_ms);
  
  /* Check if MBA image executes successfully via rmb regiter 03,
    ie RMB_MBA_STATUS.
  */
  rmb_status = (int32)HWIO_IN(RMB_MBA_STATUS);
  while(!(rmb_status == MBA_ELF_PROGRAM_SEGMENTS_COMPLETE || rmb_status < 0) && !boot_rmb_status_chk_skip)
  {
    if(pil_os_check_wait_obj(&wait_obj) == TRUE)
    {
      timeout = TRUE;
      break;
    }
    timer_sleep(5, T_MSEC, TRUE);
    rmb_status =(int32)HWIO_IN(RMB_MBA_STATUS);
  }
  pil_os_wait_sig_obj_deinit(&wait_obj);
  
  PIL_LOG_INFO(1,"mba status: %d", rmb_status);  

  if(timeout == TRUE)
  {
    PIL_LOG_ERR(1,"checking mba status timedout %d", rmb_status);
    return PIL_TIMEOUT_ERROR;
  }
  
  /* A negative value indicates error */
  if((rmb_status < 0) || (boot_rmb_status_chk_skip)) 
  {
    PIL_LOG_ERR(1,"wrong mba status: %d", rmb_status);    
    return PIL_FAILURE;
  }

  if(!((MBA_ELF_PROGRAM_SEGMENTS_COMPLETE == rmb_status) ||
             (boot_rmb_status_chk_skip)))
     return PIL_FAILURE;
  
  return PIL_SUCCESS;
} /* pil_rmb_check_subsys_status */


/*=============================================================================

FUNCTION 
  pil_rmb_modify_reg_access

DESCRIPTION
  This function needs to be called with TRUE after modem proc is brought 
  out of reset and should be called with FALSE before shutdown

DEPENDENCIES
  Modem should be up for enabling otherwise NOC error happens while 
  accessing RMB registers

RETURN VALUE
  None
  
SIDE EFFECTS
  None
=============================================================================*/
void pil_rmb_modify_reg_access(boolean is_allow)
{
  pil_rmb_reg_access_allowed = is_allow;
}

/*=============================================================================

FUNCTION 
  pil_rmb_reg_log

DESCRIPTION
  Reads and prints all MBA RMB Register values

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
=============================================================================*/
void pil_rmb_reg_log(void)
{
  if(pil_rmb_reg_access_allowed == TRUE)
  {
    pil_rmb_reg_log_arr[0] = (int32)HWIO_IN(RMB_MBA_IMAGE);
    MSG_3(MSG_SSID_TMS, MSG_LEGACY_ERROR,"pil_rmb_check_subsys_status: 0: RMB_MBA_IMAGE: 0x%x", pil_rmb_reg_log_arr[0], 0, 0);  
    PIL_LOG_INFO(1," 0: RMB_MBA_IMAGE: 0x%x", pil_rmb_reg_log_arr[0]);  

    pil_rmb_reg_log_arr[1] =(int32)HWIO_IN(RMB_PBL_STATUS);
    MSG_3(MSG_SSID_TMS, MSG_LEGACY_ERROR,"pil_rmb_check_subsys_status: 1: pbl status: %d", pil_rmb_reg_log_arr[1], 0, 0);
    PIL_LOG_INFO(1," 1: pbl status: %d", pil_rmb_reg_log_arr[1]);

    pil_rmb_reg_log_arr[2] =(int32)HWIO_IN(RMB_MBA_COMMAND);  
    MSG_3(MSG_SSID_TMS, MSG_LEGACY_ERROR,"pil_rmb_check_subsys_status: 2: RMB_MBA_COMMAND: 0x%x", pil_rmb_reg_log_arr[2], 0, 0);  
    PIL_LOG_INFO(1," 2: RMB_MBA_COMMAND: 0x%x", pil_rmb_reg_log_arr[2]);
    
    pil_rmb_reg_log_arr[3] =(int32)HWIO_IN(RMB_MBA_STATUS);  
    MSG_3(MSG_SSID_TMS, MSG_LEGACY_ERROR,"pil_rmb_check_subsys_status: 3: RMB_MBA_STATUS: %d", pil_rmb_reg_log_arr[3], 0, 0);    
    PIL_LOG_INFO(1," 3: RMB_MBA_STATUS: %d", pil_rmb_reg_log_arr[3]);

    pil_rmb_reg_log_arr[4] = (int32)HWIO_IN(RMB_PMI_META_DATA);
    MSG_3(MSG_SSID_TMS, MSG_LEGACY_ERROR,"pil_rmb_check_subsys_status: 4: RMB_PMI_META_DATA: 0x%x", pil_rmb_reg_log_arr[4], 0, 0); 
    PIL_LOG_INFO(1," 4: RMB_PMI_META_DATA: 0x%x", pil_rmb_reg_log_arr[4]);

    pil_rmb_reg_log_arr[5] = (int32)HWIO_IN(RMB_PMI_CODE_START);
    MSG_3(MSG_SSID_TMS, MSG_LEGACY_ERROR,"pil_rmb_check_subsys_status: 5: RMB_PMI_CODE_START: 0x%x", pil_rmb_reg_log_arr[5], 0, 0); 
    PIL_LOG_INFO(1," 5: RMB_PMI_CODE_START: 0x%x", pil_rmb_reg_log_arr[5]);

    pil_rmb_reg_log_arr[6] = (int32)HWIO_IN(RMB_PMI_CODE_LENGTH);
    MSG_3(MSG_SSID_TMS, MSG_LEGACY_ERROR,"pil_rmb_check_subsys_status: 6: RMB_PMI_CODE_LENGTH: 0x%x", pil_rmb_reg_log_arr[6], 0, 0); 
    PIL_LOG_INFO(1," 6: RMB_PMI_CODE_LENGTH: 0x%x", pil_rmb_reg_log_arr[6]);

    pil_rmb_reg_log_arr[7] = (int32)HWIO_IN(RMB_MBA_VERSION);
    MSG_3(MSG_SSID_TMS, MSG_LEGACY_ERROR,"pil_rmb_check_subsys_status: 7: RMB_MBA_VERSION: %d", pil_rmb_reg_log_arr[7], 0, 0); 
    PIL_LOG_INFO(1," 7: RMB_MBA_VERSION: %d", pil_rmb_reg_log_arr[7]);
    
    pil_rmb_reg_log_arr[8] = (int32)HWIO_IN(RMB_MBA_STATUS_2);
    MSG_3(MSG_SSID_TMS, MSG_LEGACY_ERROR,"pil_rmb_check_subsys_status: 8: RMB_MBA_STATUS_2: 0x%x", pil_rmb_reg_log_arr[8], 0, 0); 
    PIL_LOG_INFO(1," 8: RMB_MBA_STATUS_2: 0x%x", pil_rmb_reg_log_arr[8]);

    pil_rmb_reg_log_arr[9] = (int32)HWIO_IN(RMB_AVS_ENTRY);
    MSG_3(MSG_SSID_TMS, MSG_LEGACY_ERROR,"pil_rmb_check_subsys_status: 9: RMB_AVS_ENTRY: 0x%x", pil_rmb_reg_log_arr[9], 0, 0); 
    PIL_LOG_INFO(1," 9: RMB_AVS_ENTRY: 0x%x", pil_rmb_reg_log_arr[9]);
  }
  else
  {
    PIL_LOG_INFO(1,"RMB reg access is not allowed %d", pil_rmb_reg_access_allowed);
  }
  return;
} /* pil_rmb_reg_log */


/*=============================================================================

FUNCTION 
  pil_rmb_set_mba_image_addr

DESCRIPTION
  This function signals subsys PBL image, the MBA image start address.

DEPENDENCIES
  MBA image should be loaded at provided address

RETURN VALUE
  None
  
SIDE EFFECTS
  None
=============================================================================*/
void pil_rmb_set_mba_image_addr ( uint8 *mba_image_addr )
{
  /* ensure elf copying is completed before indicating to MBA */
  pil_os_mem_barrier();

  /* Flush cache  */
  //Cache flush may not be necessary since we will be using non-cached memories while copying data
  //pil_os_cache_flush();

  /* Communicate MBA image start address to modem pbl via RMB register 0,
    ie RMB_MBA_IMAGE
  */
   HWIO_OUT(RMB_MBA_IMAGE, (uint32)mba_image_addr);
} /* pil_rmb_set_mba_image_addr */


/*=============================================================================

FUNCTION 
  pil_rmb_deinit_mba

DESCRIPTION
  Deinits MBA indicating there is some error and expects MBA to deinitialize.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
=============================================================================*/
pil_error_type pil_rmb_deinit_mba(void)
{

  int32 rmb_status = 0x0;
  pil_wait_obj_type wait_obj;
  boolean timeout = FALSE;
  
  /* ensure elf copying is completed before indicating to MBA */
  pil_os_mem_barrier();
  
  /* Set the RMB_MBA_COMMAND to 0x1 to start the Meta Data
     authentication/validation.
  */
  HWIO_OUT(RMB_MBA_COMMAND, (uint32)CMD_PILFAIL_NFY_MBA);
  
  //boot_log_message("MBA unlock XPU, Start");
  //boot_log_start_timer();
  
  PIL_LOG_LOW(1,"reading rmb_status with timeout %d", pil_rmb_wait_timeout_ms);

  pil_os_wait_obj_init(&wait_obj);
  pil_os_start_wait_obj(&wait_obj, pil_rmb_wait_timeout_ms);
  /* Wait for MBA image to unlock XPU. Do not proceed meta info population
    till MBA is ready
  */
  rmb_status =(int32)HWIO_IN(RMB_MBA_STATUS);
  while(!(rmb_status == MBA_MBA_UNLOCKED || rmb_status < 0) && !boot_rmb_status_chk_skip)
  {
    if(pil_os_check_wait_obj(&wait_obj) == TRUE)
    {
      timeout = TRUE;
      break;
    }
    timer_sleep(5, T_MSEC, TRUE);
    rmb_status =(int32)HWIO_IN(RMB_MBA_STATUS);
  }
  pil_os_wait_sig_obj_deinit(&wait_obj);
  
  if(timeout == TRUE)
  {
    PIL_LOG_ERR(1,"MBA unlocking timedout %d", rmb_status);
    return PIL_TIMEOUT_ERROR;
  }
  
  PIL_LOG_INFO(1,"rmb_status %d", rmb_status);
  
  /* TO-DO: ensure mba sets these status's.*/
  if(!((MBA_MBA_UNLOCKED == rmb_status) ||
             (boot_rmb_status_chk_skip)))
  {
     //This always happens and we dont bother currently
     PIL_LOG_ERR(1,"error rmb_status %d", rmb_status);
     return PIL_FAILURE;
  }
  
  //boot_log_stop_timer("MBA unlock XPU, Delta");

  //return (uint8 *)MBA_META_DATA_BUFF_ADDR ;
  return PIL_SUCCESS;
} /* pil_rmb_deinit_mba */

#endif /* PIL_MBA_FILTER */

