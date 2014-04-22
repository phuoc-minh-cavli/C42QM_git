#ifndef __PIL_H__
#define __PIL_H__

/*=============================================================================

        P E R I P H E R A L   I M A G E   L O A D E R    M O D U L E

GENERAL DESCRIPTION
  Implements PIL framework for booting/shutting down various subsystems.


REGIONAL FUNCTIONS 
  None
  
EXTERNALIZED FUNCTIONS

  pil_boot(subsys name)
    pil boots given subsystem.
    
  pil_shutdown(subsys name)
    Shutdowns given subsystem.

INITIALIZATION AND SEQUENCING REQUIREMENTS
    None.

Copyright (c) 2016-17 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/subsys/src/pil.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

09/15/16   ab      File created.
05/04/17   ab      Added Support for PIL Hibernation

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "subsys.h"
#include "comdef.h"
#include "subsys_v.h"
#include "pil_config.h"  //Todo: Need to remove this once moved to Dalcfg

/*=============================================================================

                            DATA DECLARATIONS
                            
=============================================================================*/

/* PIL error return types */
typedef enum
{
  PIL_SUCCESS,
  PIL_FAILURE,
  PIL_INVALID_SUBSYS,
  PIL_INVALID_PARAMETERS,
  PIL_PARTITION_OPEN_FAILURE,
  PIL_TIMEOUT_ERROR,
  PIL_MALLOC_FAILURE,
  PIL_FLASH_READ_FAILED,
  PIL_MAPPING_FAILED,
  PIL_UNMAPPING_FAILED,
  PIL_ELF_HANDLE_NOT_INITED,
  PIL_ELF_HEADER_INVALID,
  PIL_ELF_PROG_TABLE_INVALID,
  PIL_ELF_INVALID_SEGMENT_INDEX,
  PIL_NON_LOADABLE_SEGMENT,
  PIL_SCM_FAILURE,
}pil_error_type;


typedef struct
{
  boolean hiber_boot;         /* Boot from hibernation */
}pil_boot_options_type;


/* Todo: Changing PIL to framework based calls.. Need to change pil_boot() to 
use these apis when possible so that it can be applied to various subsystems */
/* PIL descriptor type */
typedef struct pil_desc_type pil_desc;

/* PIL subsys Operations structure */
typedef struct 
{
  pil_error_type (*init_image)(pil_desc *pil);
  pil_error_type (*load_image)(pil_desc *pil);
  pil_error_type (*auth_and_reset)(pil_desc *pil);
  pil_error_type (*check_status)(pil_desc *pil);
  pil_error_type (*deinit_image)(pil_desc *pil);
  pil_error_type (*shutdown)(pil_desc *pil);
  pil_error_type (*status_wait)(pil_desc *pil);    // Api to wait for status from remote proc.. say through SMSM bits like err ready, clk ready
}pil_subsys_ops;

/* PIL descriptor type */
typedef struct pil_desc_type
{
  char subsys[MAX_SUBSYS_NAME_SIZE];
  void *priv;
  pil_subsys_ops  ops;
}pil_desc;


/*=============================================================================

                            FUNCTION DECLARATIONS
                            
=============================================================================*/
#define pil_boot_options_init(options) \
{\
   options.hiber_boot = FALSE; \
}

/*=============================================================================

FUNCTION 
  PIL_BOOT

DESCRIPTION
  This function starts given subsystem.

DEPENDENCIES
  None.

RETURN VALUE
  Pil Error value indicating subsystem start is success/failure

SIDE EFFECTS
  None.

=============================================================================*/
pil_error_type pil_boot(char *subsys, pil_boot_options_type *options);

/*=============================================================================

FUNCTION 
  PIL_SHUTDOWN

DESCRIPTION
  This function Shutdowns given subsystem.

DEPENDENCIES
  Subsystem start should have happened before Shutdown.

RETURN VALUE
  Pil Error value indicating subsystem shutdown is success/failure

SIDE EFFECTS
  None.

=============================================================================*/
pil_error_type pil_shutdown(char *subsys);

/*=============================================================================

FUNCTION 
  PIL_SUBSYS_OPS_INIT

DESCRIPTION
  This function initializes pil_subsys_ops operations structure.
  Note: Required when changing PIL to framework based calls

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/
void pil_subsys_ops_init(pil_subsys_ops *ops);

/*=============================================================================

FUNCTION 
  PIL_REGISTER_SUBSYS_OPS

DESCRIPTION
  This function registers OPS structure from each subsys module. This is to 
  make PIL module dynamic and PIL will call into OPS callback functions for
  any operations required. 
  
  Note1: ops structure will be copied, priv ptr will be saved to pil_desc
  Note2: This is not yet completed and planned for later

DEPENDENCIES
  Individual proc file should implement callback functions in pil_subsys_ops

RETURN VALUE
  Pil Error value indicating registration is success/failure

SIDE EFFECTS
  None.

=============================================================================*/
pil_error_type pil_register_subsys_ops(char *subsys, pil_subsys_ops *ops, void *priv);

              
#endif /* __PIL_H__ */

