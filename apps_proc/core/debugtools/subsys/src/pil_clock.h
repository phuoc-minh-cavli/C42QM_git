#ifndef __PIL_CLOCK_H__
#define __PIL_CLOCK_H__

/*=============================================================================

        P E R I P H E R A L   I M A G E   L O A D E R    M O D U L E
              C L O C K S   H A N D L I N G   M O D U L E 

GENERAL DESCRIPTION
  Implements clocks handling apis for PIL Framework

REGIONAL FUNCTIONS 
  None
  
EXTERNALIZED FUNCTIONS

  Clock_SetupProcessor(processor type)
    Does Initial setup for Given processor to bring out of reset.

  Clock_EnableProcessor(processor type)
    Starts given processor by setting required clock config
    
  Clock_ShutdownProcessor(processor type)
    Shutdowns given processor by setting required clock config
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
    None.

Copyright (c) 2016 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/subsys/src/pil_clock.h#1 $

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
/**
 * Processor type.
 *
 * This type lists core processors running on different subsystems.
 */
typedef enum
{
  CLOCK_PROCESSOR_LPASS     = 0,  /**< LPASS Processor.  */
  CLOCK_PROCESSOR_MODEM     = 1,  /**< Modem Processor   */
  CLOCK_PROCESSOR_WCN       = 2,  /**< WCN Processor.    */
  CLOCK_PROCESSOR_RPM       = 3   /**< RPM Processor.    */
} ClockProcessorType;



/*=============================================================================

                            FUNCTION DECLARATIONS
                            
=============================================================================*/

/*=============================================================================

FUNCTION CLOCK_SETUPPROCESSOR

DESCRIPTION
  This function Setsup clocks to bring processor out of reset

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If successful
  FALSE - On Failure

SIDE EFFECTS
  None.

=============================================================================*/
boolean Clock_SetupProcessor(ClockProcessorType eProcessorType);

/*=============================================================================

FUNCTION CLOCK_ENABLEPROCESSOR

DESCRIPTION
  This function triggers given processor to come out of reset

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If successful
  FALSE - On Failure

SIDE EFFECTS
  None.

=============================================================================*/
boolean Clock_EnableProcessor(ClockProcessorType eProcessorType);

/*=============================================================================

FUNCTION CLOCK_SHUTDOWNPROCESSOR

DESCRIPTION
  This function Setsup clocks to bring processor down

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If successful
  FALSE - On Failure

SIDE EFFECTS
  None.

=============================================================================*/
boolean Clock_ShutdownProcessor(ClockProcessorType eProcessorType);

#endif /* __PIL_CLOCK_H__ */

