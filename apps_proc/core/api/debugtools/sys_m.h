#ifndef SYS_M_H
#define SYS_M_H
/*===========================================================================

           S Y S _ M . H

DESCRIPTION

Copyright (c) 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION : system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/api/debugtools/sys_m.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/03/17   sm      Initial version

===========================================================================*/

/** sys_m_shutdown
 *
 * This is used for system shutdown.
 *
 */
void sys_m_shutdown(void);

/** sys_m_reset
 *
 * This is used for system reset.
 *
 */
void sys_m_reset(void);

/** sys_m_hibernation
 *
 * This is used for system hibernation. This is additional sleep state
 * in which we will retain the DDR in self-refresh and power collapse the MDM,
 * and while booting, avoiding copy of RO part from NAND and also ZI portion
 * to be taken care by DMA instead of using the host processor.
 *
 * @param h_time_ms Total time for hibernation (in ms)
 *
 * @return SYS_M_HIBERNATION_ERR_INVALID_H_TIME if hibernation time is
 *         less or equal to max processing time of hibernation else 
 *         SYS_M_HIBERNATION_ERR_NONE.
 */
#define SYS_M_HIBERNATION_ERR_NONE  0
#define SYS_M_HIBERNATION_ERR_INVALID_H_TIME 1
unsigned int sys_m_hibernation(unsigned int h_time_ms);

#endif  /* SYS_M_H */
