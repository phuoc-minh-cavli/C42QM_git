#ifdef PIL_MBA_FILTER
#ifndef __PIL_RMB_H__
#define __PIL_RMB_H__

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

$Header: //components/rel/core.tx/6.0/debugtools/subsys/src/pil_rmb.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

09/15/16   ab      File created.

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/



/*=============================================================================

                            DATA DECLARATIONS
                            
=============================================================================*/

#define SUBSYS_PBL_SUCCESS 0x1

#define CMD_PMI_META_DATA_READY         0x1
#define CMD_PMI_SEGMENTED_LOAD_READY    0x2
#define CMD_PILFAIL_NFY_MBA		          0xffffdead

/* MBA status register values */
#define RMB_REG_UNINITIALIZED                       0x0
#define MBA_XPU_UNLOCKED                            0x1
#define MBA_XPU_UNLOCKED_MEM_SCRIBBLED              0x2
#define MBA_PMI_META_DATA_AUTHENTICATION_SUCCESS    0x3
#define MBA_ELF_PROGRAM_SEGMENTS_COMPLETE           0x4
#define MBA_ELF_EXECUTION_COMPLETE                  0x5
#define MBA_MBA_UNLOCKED                            0x6

/*   RMB Register Defines */
#define RMB_MBA_IMAGE                  MSS_RELAY_MSG_BUFFER_00
#define RMB_PBL_STATUS                 MSS_RELAY_MSG_BUFFER_01
#define RMB_MBA_COMMAND                MSS_RELAY_MSG_BUFFER_02
#define RMB_MBA_STATUS                 MSS_RELAY_MSG_BUFFER_03
#define RMB_PMI_META_DATA              MSS_RELAY_MSG_BUFFER_04
#define RMB_PMI_CODE_START             MSS_RELAY_MSG_BUFFER_05
#define RMB_PMI_CODE_LENGTH            MSS_RELAY_MSG_BUFFER_06
#define RMB_MBA_VERSION                MSS_RELAY_MSG_BUFFER_07
#define RMB_MBA_STATUS_2               MSS_RELAY_MSG_BUFFER_08
#define RMB_AVS_ENTRY                  MSS_RELAY_MSG_BUFFER_09

//Note: If there is change in RMB Registers count modify this too
#define RMB_REGISTERS_COUNT            10


/*=============================================================================

                            FUNCTION DECLARATIONS
                            
=============================================================================*/
//Note: Please refer to pil_rmb.c for each function's description

pil_error_type pil_rmb_check_if_mba_unlocked ( void );

void pil_rmb_populate_meta_info ( uint8 *metadata_addr );

pil_error_type pil_rmb_check_metadata_auth_status ( void );

void pil_rmb_set_pmi_code_start_addr ( unsigned int  pmi_code_start_addr );

void pil_rmb_set_pmi_total_code_length ( uint32 pmi_code_length );

void pil_rmb_set_pmi_elf_load_ready ( void );

pil_error_type pil_rmb_verify_blob(uint64 size);

pil_error_type pil_rmb_check_subsys_status( void );

void pil_rmb_modify_reg_access(boolean is_allow);

void pil_rmb_reg_log(void);

void pil_rmb_set_mba_image_addr ( uint8 *mba_image_addr );

pil_error_type pil_rmb_deinit_mba(void);

#endif /*__PIL_RMB_H__*/
#endif /* PIL_MBA_FILTER */

