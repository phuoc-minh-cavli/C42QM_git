/*=============================================================================

        P E R I P H E R A L   I M A G E   L O A D E R    M O D U L E
          M O D E M   P I L   H A N D L I N G   F U N C T I O N S 

GENERAL DESCRIPTION
  Implements PIL functions for Modem Subsystem

EXTERNALIZED FUNCTIONS
  pil_modem_start()
    Boots up the Modem by enabling required clocks, loading required elfs
  
  pil_modem_shutdown()
    Shuts down the Modem processor

  pil_modem_register_for_status_bits
    Registers for Modem SMP2P status bits. Resets previous bits' status.
  
  pil_modem_wait_for_start_status_bits
    Waits for SMP2P bits(Clk Ready, Err Ready) after Modem start.
  
REGIONAL FUNCTIONS 
  pil_release_modem_proc()
    Release Modem processor from reset state
    
  pil_modem_load_image_and_metadata()
    Loads Modem metadata and segments to memory
  
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
    1) pil_modem_shutdown() should be called after pil_modem_start()
    2) pil_modem_wait_for_start_status_bits() should be used after pil_modem_start()

Copyright (c) 2016-17, 2020 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/subsys/src/pil_modem.c#4 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

09/15/16   ab      File created.
05/04/17   ab      Added Support for PIL Hibernation

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "pil.h"
#include "pil_modem.h"
#include "pil_elf_loader.h"
#include "pil_utils.h"
#include "pil_os.h"
#include "pil_clock.h"
#include "pil_rmb.h"
#include "timer.h"
#include "msg.h"
#include "pil_hwio.h"
#include <HALhwio.h>
#include "sys_m_smp2p.h"
#include "sys_m_internal.h"
#include "pil_scm.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/
/* This variable decides how segments will be loaded. Whether we load all 
   segments together or one at a time. This is applicable for MBA, for TZ
   we will always load all the segments at once and then authenticate all. */
boolean pil_segmentwise_loading = PIL_LOAD_SEGMENT_WISE_CONFIG;
uint32 pil_modem_smp2p_bits;
uint32 pil_smp2p_wait_timeout_ms = 0;

#ifdef PIL_TZ_FILTER

  #ifdef PIL_DALSYS_AVAILABLE
    extern DALSYSMemHandle MemObj_global;
  #else
    extern qurt_mem_region_t MemObj_global;
  #endif /* PIL_DALSYS_AVAILABLE */

  uint64 modem_image_size; /* Global variable which stores the size of modem image.
                              This gets updated by calling into get_modem_image_size() */
  extern uint64 pil_get_modem_image_size(pil_elf_struct_type *handle);
#endif /* PIL_TZ_FILTER */

uint64 modem_start_pa; /* Global variable which stores the physical start address of modem.
                            This gets updated by calling into get_modem_start_addr() */
extern uint64 modem_start_va;

/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================
FUNCTION 
  pil_modem_status_bits_cb

DESCRIPTION
  This is a callback function registered with System Monitor (SysMon) for any 
  SMP2P bit set updates from Modem. This saves the bit states to a global variable

DEPENDENCIES
  None.

RETURN VALUE
  None
=============================================================================*/
void pil_modem_status_bits_cb(uint32 bits)
{
  PIL_LOG_INFO(1,"got bits 0x%x", bits); 
  
  //Save in global variable
  pil_modem_smp2p_bits |= bits;
  
  return;
} /* pil_modem_status_bits_cb */


/*=============================================================================
FUNCTION 
  pil_modem_register_for_status_bits

DESCRIPTION
  This function registers internal function with SysMon to get SMP2P bit set 
  updates from Modem. This also resets the previous saved states.
  This should be called before Modem is started/stopped.

DEPENDENCIES
  None.

RETURN VALUE
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_modem_register_for_status_bits(void)
{
  boolean ret;
  ret = sys_m_modem_smp2p_notify_register(SYS_M_SUBSYS2AP_SMP2P_ERRFATAL |
                            SYS_M_SUBSYS2AP_SMP2P_ERR_HDL_RDY |
                            SYS_M_SUBSYS2AP_SMP2P_PWR_CLK_RDY |
                            SYS_M_SUBSYS2AP_SMP2P_SHUT_DWN_ACK |
                            SYS_M_SUBSYS2AP_SMP2P_SHUT_DWN |
                            SYS_M_SUBSYS2AP_SMP2P_PWR_OFF |
                            SYS_M_SUBSYS2AP_QMI_REQ_ACK, pil_modem_status_bits_cb);

  PIL_VERIFY(ret == TRUE, "sys_m_modem_smp2p_notify_register returned false", ret, 0, 0);
  
  //Reset the Modem smp2p bits
  pil_modem_smp2p_bits = 0;
  
  return PIL_SUCCESS;
} /* pil_modem_register_for_status_bits */


/*=============================================================================
FUNCTION 
  pil_modem_wait_for_start_status_bits

DESCRIPTION
  This function should be called after Modem is started.
  This will wait for Err Ready, Clk Ready bits getting set from Modem.

DEPENDENCIES
  pil_modem_register_for_status_bits() should be called before.

RETURN VALUE
  PIL_SUCCESS - In case both Err Ready, Clk Ready bits got set
  PIL_TIMEOUT_ERROR - if timeout happened waiting for bits
  PIL_FAILURE - In case they are not set
=============================================================================*/
pil_error_type pil_modem_wait_for_start_status_bits(void)
{
  pil_wait_obj_type wait_obj;
  boolean timeout = FALSE;
  
  //start Timed Wait Obj
  pil_os_wait_obj_init(&wait_obj);
  pil_os_start_wait_obj(&wait_obj, pil_smp2p_wait_timeout_ms);

  //wait for Err Ready, Clk Ready bits
  while(!((pil_modem_smp2p_bits & SYS_M_SUBSYS2AP_SMP2P_ERR_HDL_RDY) && 
               (pil_modem_smp2p_bits & SYS_M_SUBSYS2AP_SMP2P_PWR_CLK_RDY)))
  {
    //Check if Timeout happened
    if(pil_os_check_wait_obj(&wait_obj) == TRUE)
    {
      timeout = TRUE;
      break;
    }
    
    //Delay before next attempt to check
    timer_sleep(50, T_MSEC, TRUE);
  }
  
  //Deinit wait obj
  pil_os_wait_sig_obj_deinit(&wait_obj);

  if((pil_modem_smp2p_bits & SYS_M_SUBSYS2AP_SMP2P_ERR_HDL_RDY) && 
               (pil_modem_smp2p_bits & SYS_M_SUBSYS2AP_SMP2P_PWR_CLK_RDY))
    return PIL_SUCCESS;
  
  if(timeout == TRUE)
  {
    PIL_LOG_ERR(1,"waiting for SMP2P bits timedout %d", pil_modem_smp2p_bits);
    return PIL_TIMEOUT_ERROR;
  }
                            
  return PIL_FAILURE;
} /* pil_modem_wait_for_start_status_bits */


/*=============================================================================
FUNCTION 
  pil_modem_wait_for_stop_status_bits

DESCRIPTION
  This function should be called after Modem is notified to shutdown.
  This will wait for SYS_M_SUBSYS2AP_QMI_REQ_ACK bit from Modem.

DEPENDENCIES
  pil_modem_register_for_status_bits() should be called before.

RETURN VALUE
  PIL_SUCCESS - In case both Err Ready, Clk Ready bits got set
  PIL_TIMEOUT_ERROR - if timeout happened waiting for bits
  PIL_FAILURE - In case they are not set
=============================================================================*/
pil_error_type pil_modem_wait_for_stop_status_bits(void)
{
  pil_wait_obj_type wait_obj;
  boolean timeout = FALSE;
  
  //start Timed Wait Obj
  pil_os_wait_obj_init(&wait_obj);
  pil_os_start_wait_obj(&wait_obj, pil_smp2p_wait_timeout_ms);

  //wait for Err Ready, Clk Ready bits
  while(!(pil_modem_smp2p_bits & SYS_M_SUBSYS2AP_QMI_REQ_ACK))
  {
    //Check if Timeout happened
    if(pil_os_check_wait_obj(&wait_obj) == TRUE)
    {
      timeout = TRUE;
      break;
    }
    
    //Delay before next attempt to check
    timer_sleep(50, T_MSEC, TRUE);
  }
  
  //Deinit wait obj
  pil_os_wait_sig_obj_deinit(&wait_obj);

  if(pil_modem_smp2p_bits & SYS_M_SUBSYS2AP_QMI_REQ_ACK)
    return PIL_SUCCESS;
  
  if(timeout == TRUE)
  {
    PIL_LOG_ERR(1,"waiting for SMP2P bits timedout %d", pil_modem_smp2p_bits);
    return PIL_TIMEOUT_ERROR;
  }
                            
  return PIL_FAILURE;
} /* pil_modem_wait_for_stop_status_bits */


/*=============================================================================
FUNCTION 
  pil_enable_modem_hw_jtag

DESCRIPTION
  Check the certificate to see if JTAG over-ride register needs to be 
  programmed to enable access via JTAG.
  
  Note: Currently not executing as it needs to be done in secure mode only
        which is not required for now.

RETURN VALUE
  None
=============================================================================*/
void pil_enable_modem_hw_jtag( void )
{
#if 0
  uint32 override4_val=0;
  
  secboot_verified_info_type *sbl_auth_verified_info;
  uint32 entry_point;
  
  BL_VERIFY((bl_shared_data != NULL), BL_ERR_NULL_PTR_PASSED );
  
  entry_point = boot_sbl_qsee_interface_get_entry_point(&(bl_shared_data->sbl_qsee_interface),
                                            SECBOOT_MBA_SW_TYPE);
  
  /* If MBA image is loaded then update the value of the MPSS over-ride */
  if (entry_point)
  {
    sbl_auth_verified_info = 
      (secboot_verified_info_type *) boot_get_sbl_auth_verified_info
      (bl_shared_data);
	  
    BL_VERIFY((sbl_auth_verified_info != NULL), BL_ERR_NULL_PTR_PASSED );

    if(SECBOOT_DEBUG_ENABLE == sbl_auth_verified_info->enable_debug)
    {
      override4_val = 1 << (HWIO_SHFT(OVERRIDE_4,
                                 OVRID_MSS_DBGEN_DISABLE));
      override4_val |= 1 << (HWIO_SHFT(OVERRIDE_4,
                                 OVRID_MSS_NIDEN_DISABLE));                                 
    }
  }

  /* Write the override value to register */    
  HWIO_OUT(OVERRIDE_4, override4_val);

  // Target getting reset while trying to write to below register.. 
  // Seems we need to do this only if SECBOOT is enabled.. 
  // no need for now
  override4_val = 1 << (HWIO_SHFT(OVERRIDE_4,
                                 OVRID_MSS_DBGEN_DISABLE));
  override4_val |= 1 << (HWIO_SHFT(OVERRIDE_4,
                                 OVRID_MSS_NIDEN_DISABLE));
  /* Write the override value to register */
  HWIO_OUT(OVERRIDE_4, override4_val);
#endif /* #if 0 */
} /* pil_enable_modem_hw_jtag */


/*=============================================================================
FUNCTION 
  pil_release_modem_proc

DESCRIPTION
  This function brings modem proc out of reset
  
DEPENDENCIES
  MBA Image should be copied to PIL_MBA_RAM_BASE address before calling this

RETURN VALUE
  PIL_SUCCESS
  PIL_FAILURE

SIDE EFFECTS
  None.

=============================================================================*/
#ifdef PIL_MBA_FILTER
pil_error_type pil_release_modem_proc(void)
{
  pil_error_type status;
  boolean err;
  
  {
    /* Setup Clocks for the Q6 processor */
    err = Clock_SetupProcessor(CLOCK_PROCESSOR_MODEM);
    if(err == FALSE)
    {
      PIL_LOG_ERR(1,"Clock_SetupProcessor failed", 0);
      return PIL_FAILURE;
    }
    
    //Since Modem proc is setup, enable RMB registers access
    pil_rmb_modify_reg_access(TRUE);
    
    /* Communicate MBA image start address to modem pbl via reg RMB_MBA_IMAGE */
    pil_rmb_set_mba_image_addr((uint8 *)PIL_MBA_RAM_BASE);
    
    //Todo: Try without this instruction and see if it works
    /* Set modem pbl start address*/
    HWIO_OUT(MSS_QDSP6SS_RST_EVB, (uint32)MODEM_PBL_START_ADDR >> 4);
       
    /* Bring Q6 proc out of reset. Modem pbl executes and then jumps to MBA
       image */
    err = Clock_EnableProcessor(CLOCK_PROCESSOR_MODEM);
    if(err == FALSE)
    {
      PIL_LOG_ERR(1,"Clock_EnableProcessor failed", 0);    
      return PIL_FAILURE;
    }
    
    //Verify MBA is up and unlocked
    status = pil_rmb_check_if_mba_unlocked();
    if(status != PIL_SUCCESS)
    {
      PIL_LOG_ERR(1,"pil_rmb_check_if_mba_unlocked failed with err %d MBA didnt get unlock.. returning", status);
      return status;
    }
    
    //Enable jtag if allowed
    /* We donot need to do this as this needs to be done from secboot and using some specific procedure..
       */
    //pil_enable_modem_hw_jtag();
  }
  
  return PIL_SUCCESS;
} /* pil_release_modem_proc */
#endif /* PIL_MBA_FILTER */


/*=============================================================================
FUNCTION 
  pil_modem_load_image_and_metadata_tz

DESCRIPTION
  This function will populate a buffer with the meta data for the current
  elf image being processed. Metadata includes elf headers, program headers
  and the hash segment.After loading the metadata, all the segments of modem
  will be loaded and a TZ call will be made for authenticating the segments.

RETURN VALUE
  PIL_SUCCESS
  PIL_FAILURE

SIDE EFFECTS
  None.

=============================================================================*/
#ifdef PIL_TZ_FILTER
pil_error_type pil_modem_load_image_and_metadata_tz(pil_elf_struct_type *modem_handle)
{
  pil_error_type status;
  uint32 seg_count=0, i, result=0;
  uint64 size =0, meta_data_size =0;
  unsigned int ehdr_entry = pil_elf_get_edhr_entry(modem_handle);
  
  PIL_LOG_INFO(1,"pil_elf_get_edhr_entry returned entry address as 0x%x", ehdr_entry);
  
  /* Get meta information for the elf to be loaded. send it to subsystem
     image expecting it ( TZ )
  */

  /* pil_get_meta_data_size will return the meta data size, which is
     elf hdr size + (num of prg hdr * prg hdr size) + size of hash seg. */
  meta_data_size = pil_get_meta_data_size(modem_handle);
  if(meta_data_size == 0)
  {
    status = PIL_FAILURE;
    PIL_LOG_ERR(1,"pil_get_meta_data_size returned %d", 0);
    return status;
  }

  /* Populate the meta info to meta buffer */
  status = pil_elf_populate_meta_info_tz(modem_handle, (uint8 *)(unsigned long)modem_start_pa, meta_data_size);
  
  if(status != PIL_SUCCESS)
  {
    PIL_LOG_ERR(1,"pil_elf_populate_meta_info_tz or pil_elf_populate_meta_info_mba returned error %d", status);
    return status;
  }

  {
    /* Load all segments in one shot */
    /* Get the size of modem image */
    modem_image_size = pil_get_modem_image_size(modem_handle);
    if(modem_image_size == 0)
    {
      PIL_LOG_ERR(1,"pil_get_modem_image_size returned 0",0);
      return PIL_FAILURE;
    }
    /* Load all data segments into memory for segmented authentication */ 
    status = pil_elf_load_loadable_segments(modem_handle);
    if(status != PIL_SUCCESS)
    {
      PIL_LOG_ERR(1,"pil_elf_load_loadable_segments returned error %d", status);
      return status;
    }

    result = pil_validate_all_segments((uint8*)(unsigned long)modem_start_va,modem_image_size);
	
	
    if(result != PIL_SUCCESS)
    {
      PIL_LOG_ERR(1,"Failed to validate the modem segments and scm call returned error %d", result);
      return PIL_FAILURE;
    }

    /* Set total length of loaded segments. MBA will xpu lock
       segments as soon as length is updated.
    */
  }
  
  return PIL_SUCCESS;
} /* pil_modem_load_image_and_metadata_tz */
#endif /* PIL_TZ_FILTER */

/*=============================================================================
FUNCTION 
  pil_modem_load_image_and_metadata_mba

DESCRIPTION
  This function will populate a buffer with the meta data for the current
  elf image being processed. It then loads the elf image and signals mba
  via rmb registers, to process metadata .
  
DEPENDENCIES
  Must be called only after bringing modem out of reset (pil_release_modem_proc)

RETURN VALUE
  PIL_SUCCESS
  PIL_FAILURE

SIDE EFFECTS
  None.

=============================================================================*/
#ifdef PIL_MBA_FILTER
pil_error_type pil_modem_load_image_and_metadata_mba(pil_elf_struct_type *modem_handle)
{
  pil_error_type status;
  uint32 seg_count=0, i, result=0;
  uint64 size =0, meta_data_size =0;
  unsigned int ehdr_entry = pil_elf_get_edhr_entry(modem_handle);
  
  PIL_LOG_INFO(1,"pil_elf_get_edhr_entry returned entry address as 0x%x", ehdr_entry);
  
  /* Get meta information for the elf to be loaded. send it to subsystem
     image expecting it ( MBA )
  */

  /* pil_get_meta_data_size will return the meta data size, which is
     elf hdr size + (num of prg hdr * prg hdr size) + size of hash seg. */
  meta_data_size = pil_get_meta_data_size(modem_handle);
  if(meta_data_size == 0)
  {
    status = PIL_FAILURE;
    PIL_LOG_ERR(1,"pil_get_meta_data_size returned %d", 0);
    return status;
  }

  /* Populate the meta info to meta buffer */
  status = pil_elf_populate_meta_info_mba(modem_handle, (uint8 *)(unsigned long)modem_start_pa, meta_data_size);
  
  if(status != PIL_SUCCESS)
  {
    PIL_LOG_ERR(1,"pil_elf_populate_meta_info_tz or pil_elf_populate_meta_info_mba returned error %d", status);
    return status;
  }

  /*Signal subsystem authenticator image to retrieve meta info buffer */
    pil_rmb_populate_meta_info((uint8 *)(unsigned long int)modem_start_pa);
  
  /* Check Metadata authentication is successful or not */
  status = pil_rmb_check_metadata_auth_status();
  if(status != PIL_SUCCESS)
  {
    PIL_LOG_ERR(1,"pil_rmb_check_metadata_auth_status: Metadata authentication failed with status %d", status);
    return status;
  }
  
  /* Set the image start address. It is assumed the elf segments are in
     increasing order and first segment start address is lowest address
  */
  pil_rmb_set_pmi_code_start_addr(ehdr_entry);
  
  /*Signal MBA image to start processing PMI elf segments and
    jump to elf. */
  pil_rmb_set_pmi_elf_load_ready();
    
  /* Load remaining Modem segments */
  /* Featurizing out the if part since we will load all segments at once and TZ
     will authenticate all the segments at once */
  if(pil_segmentwise_loading == TRUE)
  {
    /* Using segment wise loading and validating procedure */
    
    status = pil_elf_get_segments_count(modem_handle, &seg_count);
    if(status != PIL_SUCCESS)
    {
      PIL_LOG_ERR(1,"pil_elf_get_segments_count returned error %d", status);
      return status;
    }
    
    for(i=0; i<seg_count; i++)
    {
      /* Load nth segment to memory */
      status = pil_elf_load_nth_segment(modem_handle, i);
      if(status != PIL_SUCCESS && status != PIL_NON_LOADABLE_SEGMENT)
      {
        PIL_LOG_ERR(2,"pil_elf_load_nth_segment returned error %d for %d segment", status, i);
        return status;
      }
      
      /* If able to load segment successfully, update size to MBA */
      if(status == PIL_SUCCESS)
      {
        //Get size of recently loaded segment
        status = pil_elf_get_segment_size(modem_handle, i, &size);
        if(status != PIL_SUCCESS)
        {
          PIL_LOG_ERR(2,"pil_elf_get_segment_size returned error %d for %d segment", status, i);
          return status;
        }    
        
        //Update size to MBA and verify the status till here
        status = pil_rmb_verify_blob(size);
        if(status != PIL_SUCCESS)
        {
          PIL_LOG_ERR(2,"pil_rmb_verify_blob returned error %d at %d segment", status, i);
          return status;
        }
      }
    }
  }
  else
  {
    /* Load all segments in one shot */
    uint32 total_elf_segments_size=0;
    
    /* Initialize total segments size of elf to be loaded */
    total_elf_segments_size = pil_get_read_elf_size(modem_handle);
    /* Load all data segments into memory for segmented authentication */ 
    status = pil_elf_load_loadable_segments(modem_handle);
    if(status != PIL_SUCCESS)
    {
      PIL_LOG_ERR(1,"pil_elf_load_loadable_segments returned error %d", status);
      return status;
    }
	
    /* Set total length of loaded segments. MBA will xpu lock
       segments as soon as length is updated.
    */
  pil_rmb_set_pmi_total_code_length(pil_get_read_elf_size(modem_handle) - total_elf_segments_size);
  }
  
  /* Check if all the segments are authenticated successfully and MBA indicating 
     PROGRAM_SEGMENTS_COMPLETE status 
   */
  status = pil_rmb_check_subsys_status();
  if(status != PIL_SUCCESS)
  {
    PIL_LOG_ERR(1,"pil_rmb_check_subsys_status returned error %d", status);
    return status;
  }

  return PIL_SUCCESS;
} /* pil_modem_load_image_and_metadata_mba */
#endif /* PIL_MBA_FILTER */

/*=============================================================================
FUNCTION 
  pil_modem_reset_proc

DESCRIPTION
  This function should  be called if there is some error in boot up sequence and
  if modem is already brought out of reset. This will indicate MBA that we are 
  de-initializing and then shuts down the modem by de-asserting required clocks
  
DEPENDENCIES
  Modem should be already brought out of reset

RETURN VALUE
  PIL_SUCCESS
  PIL_FAILURE

SIDE EFFECTS
  None.

=============================================================================*/
#ifdef PIL_MBA_FILTER
pil_error_type pil_modem_reset_proc(void)
{
  pil_error_type status;
  boolean err;
  
  //Indicate MBA to deinitialize
  status = pil_rmb_deinit_mba();
  if(status != PIL_SUCCESS)
    PIL_LOG_ERR(1,"pil_rmb_deinit_mba returned error %d", status);
  
  //disable RMB access before shutdown
  pil_rmb_modify_reg_access(FALSE);
  
  //Halt the Modem processor by setting required clocks
  err = Clock_ShutdownProcessor(CLOCK_PROCESSOR_MODEM);
  if(err == FALSE)
  {
    PIL_LOG_ERR(1,"pil_shutdown_modem: Clock_ShutdownProcessor failed", 0);
    return PIL_FAILURE;
  }
  
  return PIL_SUCCESS;
} /* pil_modem_reset_proc */


/*=============================================================================
FUNCTION 
  pil_modem_shutdown

DESCRIPTION
  This function will shutdown the Modem subsystem
  
DEPENDENCIES
  Modem should be up already

RETURN VALUE
  PIL_SUCCESS
  PIL_FAILURE

SIDE EFFECTS
  None.

=============================================================================*/
pil_error_type pil_modem_shutdown(void)
{
  boolean err;
  
  //Indicate System Monitor to notify Modem about shutdown
  sys_m_notify_modem_before_shutdown();
  
  //wait for ACK from Modem after indicating shutdown notification
  (void)pil_modem_wait_for_stop_status_bits();
  
  /* Todo: will there be any force_stop smp2p bit to indicate to modem in cases of 
  force crash and that seems to return stop_ack.. check modem_shutdown() in pil-q6v5-mss.c */
  
  //disable RMB access before shutdown
  pil_rmb_modify_reg_access(FALSE);
  
  //Halt the Modem processor by setting required clocks
  err = Clock_ShutdownProcessor(CLOCK_PROCESSOR_MODEM);
  if(err == FALSE)
  {
    PIL_LOG_ERR(1,"pil_shutdown_modem: Clock_ShutdownProcessor failed", 0);
    return PIL_FAILURE;
  }
  return PIL_SUCCESS; 
} /* pil_shutdown_modem */
#endif /* PIL_MBA_FILTER */

/*=============================================================================
FUNCTION 
  pil_modem_start

DESCRIPTION
  This function will start the Modem subsystem. It will load MBA, Modem ELFs to
  memory and enables required clocks to start the Modem subsystem. 
  
DEPENDENCIES
  None

RETURN VALUE
  PIL_SUCCESS
  PIL_FAILURE

SIDE EFFECTS
  None.

=============================================================================*/
static pil_elf_struct_type *mba_handle = NULL;
static pil_elf_struct_type *modem_handle = NULL;

#define PIL_MODEM_START_RECONSTRUCT_FAILED      1
#define PIL_MODEM_START_RELEASE_PROC_FAILED     2
#define PIL_MODEM_START_MODEM_ELF_INIT_FAILED   3
#define PIL_MODEM_GET_START_ADDRESS_FAILED      4
#define PIL_MODEM_START_MODEM_IMAGE_LOAD_FAILED 5
#ifdef PIL_TZ_FILTER
  #define PIL_MODEM_RESET_FAILED                  6
  #define PIL_MODEM_UNMAP_FAILED                  7
#endif /* PIL_TZ_FILTER */

#ifdef PIL_MBA_FILTER
pil_error_type pil_modem_start_mba(pil_boot_options_type *options)
{
  pil_error_type err;
  uint32 err_pos = 0;

  PIL_LOG_INFO(1,"pil_modem_start start", 0);
  
  do{

    /* ELF Init MBA partition */
    err = pil_elf_loader_init(MBA_PARTITION_NAME, &mba_handle);
    if(err != PIL_SUCCESS)
    {
      PIL_LOG_ERR(1,"MBA elf init err status 0x%x", err);
      return err;
    }
    
    /* MBA is not checked for hibernation due to its special case that
    whole image needs to be copied to DDR and also in previous boot
    MBA might have used the MBA dynamic memory for other operations */
    //if(options->hiber_boot == TRUE)
      //pil_elf_set_hiber_boot(mba_handle, TRUE);
    
    PIL_LOG_LOW(1,"Before reconstruct MBA segment", 0);
    /* Reconstruct MBA image in memory by reading all MBA segments */
    err = pil_reconstruct_mba_elf_image(mba_handle, (uint8 *)PIL_MBA_RAM_BASE, PIL_MBA_RAM_SIZE);

    PIL_LOG_LOW(1,"After reconstruct MBA segment with status: %d", err);
    if(err != PIL_SUCCESS)
    {
      PIL_LOG_ERR(1,"reconstruct MBA failed with status: %d", err);
      err_pos = PIL_MODEM_START_RECONSTRUCT_FAILED;
      break;
    }
    
    PIL_LOG_LOW(1,"calling pil_release_modem_proc", 0); 
    /* Release Modem processor out of Reset */
    err = pil_release_modem_proc();

    if(err != PIL_SUCCESS)
    {
      PIL_LOG_ERR(1,"pil_release_modem_proc failed with err %d", err); 
      err_pos = PIL_MODEM_START_RELEASE_PROC_FAILED;
      break;
    }
    PIL_LOG_LOW(1,"pil_release_modem_proc complete with status %d", err);
  
    /* Close MBA partition as reading MBA data is done */
    pil_elf_loader_deinit(&mba_handle);
    
    /* Open Modem partition */
    err = pil_elf_loader_init(MODEM_PARTITION_NAME, &modem_handle);

    if(modem_handle == NULL || err != PIL_SUCCESS)
    {
      PIL_LOG_ERR(1,"Modem init failed with 0x%x", err); 
      err_pos = PIL_MODEM_START_MODEM_ELF_INIT_FAILED;
      break;
    }
    
    //Check if requested boot is from hibernation
    if(options->hiber_boot == TRUE)
      pil_elf_set_hiber_boot(modem_handle, TRUE);
    
    PIL_LOG_LOW(1,"Starting Modem image loading", 0);
	
    /* Getting the physical start address of modem */
    modem_start_pa = pil_get_modem_start_addr(modem_handle);
    if(modem_start_pa == 0)
    {
      PIL_LOG_ERR(1,"Modem init failed with 0x%x", err);
	  err_pos = PIL_MODEM_GET_START_ADDRESS_FAILED;
      break;
    }
	    
    /* Load Modem Metadata which includes elf header, program headers and hash segment.*/
    err = pil_modem_load_image_and_metadata_mba(modem_handle);
	
    if(err != PIL_SUCCESS)
    {
      PIL_LOG_INFO(1,"Modem image loading failed with err %d", err);
      err_pos = PIL_MODEM_START_MODEM_IMAGE_LOAD_FAILED;
      break;
    }

     /* Close Modem partition */
    pil_elf_loader_deinit(&modem_handle);
    
  }while(0);
  
  /* handle the error cases */
  switch(err_pos)
  {
    case PIL_MODEM_START_RECONSTRUCT_FAILED:  
		pil_elf_loader_deinit(&mba_handle);
	    break;
	case PIL_MODEM_START_RELEASE_PROC_FAILED: 
		/* nothing to do */
	    break;
	case PIL_MODEM_START_MODEM_ELF_INIT_FAILED: 
          pil_modem_reset_proc(); /* Featurized this, since this is applicable only if mba validates modem.
                                     In case TZ authenticates modem, only TZ can access the TCSR registers 
                                     and PIL cannot access these registers. */
          break;
	case PIL_MODEM_GET_START_ADDRESS_FAILED: 
		pil_elf_loader_deinit(&modem_handle);
		break;
    case PIL_MODEM_START_MODEM_IMAGE_LOAD_FAILED: 
		pil_elf_loader_deinit(&modem_handle);
		break;
    default: /* do nothing */ break;
  }
  
  return err;  
} /* pil_modem_start_mba */
#endif /* PIL_MBA_FILTER */

#ifdef PIL_TZ_FILTER
pil_error_type pil_modem_start_tz(pil_boot_options_type *options)
{
  pil_error_type err;
  uint32 err_pos = 0;
  uint32 result = 0;
 
  PIL_LOG_INFO(1,"pil_modem_start start", 0);
  
  do{

    /* Open Modem partition */
    err = pil_elf_loader_init(MODEM_PARTITION_NAME, &modem_handle);

    if(modem_handle == NULL || err != PIL_SUCCESS)
    {
      PIL_LOG_ERR(1,"Modem init failed with 0x%x", err); 
      err_pos = PIL_MODEM_START_MODEM_ELF_INIT_FAILED;
      break;
    }
    
    //Check if requested boot is from hibernation
    if(options->hiber_boot == TRUE)
      pil_elf_set_hiber_boot(modem_handle, TRUE);
    
    PIL_LOG_LOW(1,"Starting Modem image loading", 0);
	
    /* Getting the physical start address of modem */
    modem_start_pa = pil_get_modem_start_addr(modem_handle);
    if(modem_start_pa == 0)
    {
      PIL_LOG_ERR(1,"Modem init failed with 0x%x", err);
	  err_pos = PIL_MODEM_GET_START_ADDRESS_FAILED;
      break;
    }
	    
    /* Load Modem Metadata which includes elf header, program headers and hash segment.*/
    err = pil_modem_load_image_and_metadata_tz(modem_handle);
	
    if(err != PIL_SUCCESS)
    {
      PIL_LOG_INFO(1,"Modem image loading failed with err %d", err);
      err_pos = PIL_MODEM_START_MODEM_IMAGE_LOAD_FAILED;
      break;
    }

    /* Trigger auth and reset of modem by making scm call internally */
    result = pil_auth_reset();

	
    if(result != PIL_SUCCESS)
    {
      PIL_LOG_INFO(1,"pil_auth_reset failed and scm call returned error %d", result);
      err_pos = PIL_MODEM_RESET_FAILED;
      break;
    }

    /* After modem is reset, we are unmapping the mapping we created
       for loading modem. This will make sure that no one from apps
       can tamper with the region in which modem is loaded. */
	err = pil_unmap_virt_addr(&MemObj_global);
	if(err != PIL_SUCCESS)
    {
      PIL_LOG_INFO(1,"Unmapping modem region in DDR failed with err %d", err);
      err_pos = PIL_MODEM_UNMAP_FAILED;
      break;
    } 
 
    /* Close Modem partition */
    pil_elf_loader_deinit(&modem_handle);
    
  }while(0);
  
  /* handle the error cases */
  switch(err_pos)
  {
	case PIL_MODEM_START_RELEASE_PROC_FAILED: 
		/* nothing to do */
	    break;
	case PIL_MODEM_START_MODEM_ELF_INIT_FAILED: 
	    sys_m_reset();         /*  Rebooting the system in case of error. 
	                               We cannot reset modem from pil directly.*/
	    break;
	case PIL_MODEM_GET_START_ADDRESS_FAILED: 
		pil_elf_loader_deinit(&modem_handle);
		break;
    case PIL_MODEM_START_MODEM_IMAGE_LOAD_FAILED: 
		pil_elf_loader_deinit(&modem_handle);
		break;
    case PIL_MODEM_RESET_FAILED:
	  sys_m_reset(); /* Try rebooting again if reset had failed */
	  err = PIL_FAILURE;
	  break;
	case PIL_MODEM_UNMAP_FAILED:
	  pil_elf_loader_deinit(&modem_handle); /* De initilize elf structures if unmap fails */
	  break;

    default: /* do nothing */ break;
  }
  
  return err;  
} /* pil_modem_start_tz */
#endif /* PIL_TZ_FILTER */



/*=============================================================================
FUNCTION 
  pil_modem_init

DESCRIPTION
  This function will do required initialization for Modem subsystem. It will 
  register with pil framework by providing the pil operation callbacks.
  
  Note: Not yet completed. For now not required.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

=============================================================================*/
//Todo: Not complete now, but when possible, need to update below functions and 
// change the pil_boot() using function pointer calls
void pil_modem_init(void)
{
  pil_subsys_ops ops;
  pil_mba_priv_type *mba_priv;
  pil_modem_priv_type *modem_priv;

  mba_priv = calloc(1, sizeof(pil_mba_priv_type));
  PIL_VERIFY(mba_priv != NULL, "calloc couldnt allocate memory for mba_priv", sizeof(pil_mba_priv_type), 0, 0);
  
  pil_subsys_ops_init(&ops);
  ops.init_image     = NULL; //pil_mba_init;
  ops.load_image     = NULL; //pil_mba_load_image;
  ops.auth_and_reset = NULL; //pil_mba_auth_and_reset;
  ops.check_status   = NULL; //pil_mba_check_status;
  ops.deinit_image   = NULL; //pil_mba_deinit_image;
  ops.shutdown       = NULL; //pil_mba_shutdown;
  
  pil_register_subsys_ops(MBA_SUBSYS_STR, &ops, mba_priv);

  modem_priv = calloc(1, sizeof(pil_modem_priv_type));
  PIL_VERIFY(mba_priv != NULL, "calloc couldnt allocate memory for modem_priv", sizeof(pil_modem_priv_type), 0, 0);
  
  pil_subsys_ops_init(&ops);
  ops.init_image     = NULL; //pil_modem_init;
  ops.load_image     = NULL; //pil_modem_load_image;
  ops.auth_and_reset = NULL; //pil_modem_auth_and_reset;
  ops.check_status   = NULL; //pil_modem_check_status;
  ops.deinit_image   = NULL; //pil_modem_deinit_image;
  ops.shutdown       = NULL; //pil_modem_shutdown;
  
  pil_register_subsys_ops(SUBSYS_MODEM_NAME, &ops, modem_priv);
  
  return;
} /* pil_modem_init */

