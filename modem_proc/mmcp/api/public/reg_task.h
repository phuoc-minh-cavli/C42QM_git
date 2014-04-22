#ifndef REG_TASK_H
#define REG_TASK_H
/*==============================================================================


                   R E G  -  T A S K   H E A D E R   F I L E


                                  DESCRIPTION

  This module defines the types and functions necessary to send a command
  to the REG task.

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_task.h_v   1.1   06 Jun 2002 10:45:00   kabraham  $
$Header: //components/rel/mmcp.mpss/6.1.10/api/public/reg_task.h#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -------------------------------------------------------------

==============================================================================*/


/*==============================================================================

FUNCTION NAME

  reg_main

DESCRIPTION

  Main routine for the REG task.

RETURN VALUE

  None

==============================================================================*/

extern void reg_main
(
  dword dummy
);

extern boolean reg_mode_is_first_round_scan_after_oos_poweroff();
#endif
