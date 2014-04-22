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

Copyright (c) 2016-17, 2020 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/subsys/src/pil.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

09/15/16   ab      File created.
05/04/17   ab      Added Support for PIL Hibernation

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <stringl/stringl.h>
#include "subsys.h"
#include "pil.h"
#include "pil_rmb.h"
#include "pil_os.h"
#include "pil_utils.h"
#include "msg.h"
#include "pil_modem.h"
#include "sys_m.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/
#define MAX_PIL_DESCRIPTORS 4
pil_desc *pil_desc_arr[MAX_PIL_DESCRIPTORS];
uint32    pil_desc_idx = 0;

/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/

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
pil_error_type pil_boot(char *subsys, pil_boot_options_type *options)
{
  pil_error_type status;
  
  if(pil_strcmp(subsys, SUBSYS_MODEM_NAME) == 0)
  {
    //Vote for system clocks like bimc etc
    pil_common_clocks_vote();
     
    //Register for SMSM bits from modem before booting
    pil_modem_register_for_status_bits();

  #ifdef PIL_MBA_FILTER
    status = pil_modem_start_mba(options);
  #else
    status = pil_modem_start_tz(options);
  #endif /* PIL_MBA_FILTER */
    
    //Log RMB register values after modem boot
    #ifdef PIL_MBA_FILTER
      pil_rmb_reg_log();
    #endif /* PIL_MBA_FILTER */

    if(status == PIL_SUCCESS)
    {
      status = pil_modem_wait_for_start_status_bits();
      if(status != PIL_SUCCESS)
      {
        PIL_LOG_ERR(1,"pil_boot: pil_modem_wait_for_start_status_bits returned err %d, shutting down Modem", status);
        
        //Shutdown Modem
        /* If authentication is done by TZ, then this should not be called from here.
           As the registers cannot be accessed by PIL, and can only be accessed by TZ. 
           Featurized this for MBA. */
        #ifdef PIL_MBA_FILTER
          pil_modem_shutdown();
        #endif 

        /* Restart the system if modem is not up */
        #ifdef PIL_TZ_FILTER
          sys_m_reset();
        #endif
      }
    }

    //Devote for the clocks
    pil_common_clocks_devote();
    
    return status;
  }

  return PIL_INVALID_SUBSYS;
} /* pil_boot */


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
  pil_error_type pil_shutdown(char *subsys)
  {
    #ifdef PIL_MBA_FILTER
      if(pil_strcmp(subsys, SUBSYS_MODEM_NAME)==0)
        return pil_modem_shutdown();
      else
        return PIL_INVALID_SUBSYS;
    #else
      return PIL_INVALID_SUBSYS;
    #endif /* PIL_MBA_FILTER */

  } /* pil_shutdown */

/*=============================================================================

FUNCTION 
  PIL_SUBSYS_OPS_INIT

DESCRIPTION
  This function initializes pil_subsys_ops operations structure.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/
void pil_subsys_ops_init(pil_subsys_ops *ops)
{
  PIL_VERIFY(ops != NULL, "NULL ops passed to pil_subsys_ops_init", 0, 0, 0);
  
  memset(ops, 0, sizeof(pil_subsys_ops));
  
  return;
} /* pil_subsys_ops_init */


/*=============================================================================

FUNCTION 
  PIL_REGISTER_SUBSYS_OPS

DESCRIPTION
  This function registers OPS structure from each subsys module. This is to 
  make PIL module dynamic and PIL will call into OPS callback functions for
  any operations required. 
  
  Note: This is not yet completed and planned for later

DEPENDENCIES
  Individual proc file should implement callback functions in pil_subsys_ops

RETURN VALUE
  Pil Error value indicating registration is success/failure

SIDE EFFECTS
  None.

=============================================================================*/
pil_error_type pil_register_subsys_ops(char *subsys, pil_subsys_ops *ops, void *priv)
{
  PIL_VERIFY(pil_desc_idx<MAX_PIL_DESCRIPTORS, "pil_desc_idx(%d) shouldnt exceed MAX_PIL_DESCRIPTORS(%d)", pil_desc_idx, MAX_PIL_DESCRIPTORS, subsys);
  
  //Todo: Need to add mutex around
  pil_desc_arr[pil_desc_idx] = calloc(1, sizeof(pil_desc));
  PIL_VERIFY(pil_desc_arr[pil_desc_idx] != NULL, "Couldn't allocate pil_desc for subsys 0x%x", subsys, 0, 0);
  
  
  memscpy(pil_desc_arr[pil_desc_idx]->subsys, MAX_SUBSYS_NAME_SIZE, subsys, MAX_SUBSYS_NAME_SIZE);
  memscpy(&pil_desc_arr[pil_desc_idx]->ops, sizeof(pil_subsys_ops), ops, sizeof(pil_subsys_ops));
  pil_desc_arr[pil_desc_idx]->priv = priv;
  
  pil_desc_idx++;
  
  return PIL_SUCCESS;
} /* pil_register_subsys_ops */

