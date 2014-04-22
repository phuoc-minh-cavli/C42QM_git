#ifndef __PIL_MODEM_H__
#define __PIL_MODEM_H__

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
    Registers for Modem SMSM status bits. Resets previous bits' status.
  
  pil_modem_wait_for_start_status_bits
    Waits for SMSM bits(Clk Ready, Err Ready) after Modem start.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
    1) pil_modem_shutdown() should be called after pil_modem_start()
    2) pil_modem_wait_for_start_status_bits() should be used after pil_modem_start()

Copyright (c) 2016-17,2020 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/subsys/src/pil_modem.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

09/15/16   ab      File created.
05/04/17   ab      Added Support for PIL Hibernation

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "pil_elf_loader.h"


/*=============================================================================

                            DATA DECLARATIONS
                            
=============================================================================*/
typedef struct
{
  pil_elf_struct_type *elf_handle;
}pil_mba_priv_type;

typedef struct
{
  pil_elf_struct_type *elf_handle;
}pil_modem_priv_type;


/*=============================================================================

                            FUNCTION DECLARATIONS
                            
=============================================================================*/
#ifdef PIL_MBA_FILTER
  pil_error_type pil_modem_start_mba(pil_boot_options_type *options);
  pil_error_type pil_modem_shutdown(void);
#endif /* PIL_MBA_FILTER */
pil_error_type pil_modem_start_tz(pil_boot_options_type *options);
pil_error_type pil_modem_register_for_status_bits(void);
pil_error_type pil_modem_wait_for_start_status_bits(void);

#endif /* __PIL_MODEM_H__ */
