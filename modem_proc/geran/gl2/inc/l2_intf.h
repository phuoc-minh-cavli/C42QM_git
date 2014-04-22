#ifndef L2_INTF_H
#define L2_INTF_H

/*!
  @file l2_intf.h

  @brief

  @detail

*/

/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gl2/inc/l2_intf.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
22/11/17   RRP     Initial Revision

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
typedef void (* l2_generate_filler_t)(byte *spare_octets, uint16 num_octets);

/* Interface table to interact with dynamic code */
typedef struct geran_gl2_intf_t
{
  l2_generate_filler_t l2_generate_filler;
} geran_gl2_intf_s;

extern geran_gl2_intf_s l2_vtbl;
#endif /* FEATURE_EXTERNAL_PAGING_GERAN */

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/* Interface function prototypes */
extern void l2_generate_filler_dyn(byte *spare_octets, uint16 num_octets);

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
extern void geran_gl2_init(void);
extern void geran_gl2_deinit(void);
extern void geran_gl2_suspend(void);
extern void geran_gl2_resume(void);
#endif /* FEATURE_EXTERNAL_PAGING_GERAN */

#endif /* L2_INTF_H */
