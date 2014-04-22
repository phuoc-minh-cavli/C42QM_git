#ifndef CE_RSA_MEM_H
#define CE_RSA_MEM_H

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

   This file contains a secure memset implementation.
  ===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  $Header: //components/rel/ssg.tz/1.1.3/securemsm/uclib/usr/legacy/secrsa/shared/src/secrsa_mem.h#1 $
  $DateTime: 2019/05/23 05:32:51 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  04/11/11   qxu     initial version
=============================================================================*/

#undef memset

#define memset secrsa_memset

void * secrsa_memset(void *buf, int i, size_t size);



#endif /* CE_RSA_MEM_H */
