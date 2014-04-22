#ifndef PS_SOCK_MEM_POOL_H
#define PS_SOCK_MEM_POOL_H
/*===========================================================================

          S O C K E T S  M E M O R Y  P O O L  H E A D E R  F I L E

DESCRIPTION
 The Protocol Services Socket Memory Pool Header File. Contains declarations
 of functions specifice to sockets memory initialization.

Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"



#ifdef FEATURE_DATA_PS
/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_SOCK_MEM_POOL_INIT()

DESCRIPTION
  Initializes dssocki module

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_sock_mem_pool_init
(
  void
);

#endif /* FEATURE_DATA_PS */
#endif /* PS_SOCK_MEM_POOL_H */
