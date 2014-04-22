/*!
  @file vbatt_efs.h

  @brief
   EFS Item support for VBATT

*/

/*=============================================================================

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved.  By accepting
  this material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.

=============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mcs.mpss/5.1/limitsmgr/vbatt/inc/vbatt_efs.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/31/16   btl     EFS mode and generic records support
05/29/15   tl      Initial Revision

=============================================================================*/

#ifndef VBATT_EFS_H
#define VBATT_EFS_H

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "sys.h"
#include "lmtsmgr_efs.h"

/*=============================================================================
                       CONSTANTS AND DEFINES
=============================================================================*/

/* default EFS version */
#define VBATT_EFS_DEF_VERSION    1

/* generic EFS version */
#define VBATT_EFS_GEN_VERSION    1
#define VBATT_EFS_GEN_SIZE (sizeof(vbatt_stage_efs_record_type)*VBATT_STAGES_MAX)
/* Thresholds for generic solution, from EFS in default solution */
#define VBATT_EFS_GEN_HYSTERESIS 500

/* Define tech band based on tech, band provided */
#define VBATT_EFS_DEFINE_TECH_BAND(tech, sub_tech, band) ((((uint32)((tech) & 0xFF)) << 24) | \
                                                          (((uint32)((sub_tech) & 0xFF)) << 16) | ((band) & 0xFFFF))

/*=============================================================================
                             TYPEDEFS
=============================================================================*/

/* Enumeration used to indicate the current vbatt stage. */
typedef enum
{
  VBATT_STAGE_INVALID   = -2,
  VBATT_STAGE_0         = -1,
  VBATT_STAGE_1         =  0,
  VBATT_STAGE_2         =  1,
  VBATT_STAGES_MAX      =  2,
} vbatt_stage_type;

typedef enum
{
  VBATT_OP_MODE_DEFAULT,
  VBATT_OP_MODE_GENERIC,
  VBATT_OP_MODE_MAX
} vbatt_op_mode_e;

#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */

/* Packed data structures as stored in EFS */
typedef PACK (struct)
{
  int16                         tx_power_limit;
  int16                         voltage_down;
  int16                         voltage_up;
} vbatt_stage_efs_record_type;

typedef PACK (struct)
{
  uint16                        band;
  vbatt_stage_efs_record_type   stages[VBATT_STAGES_MAX];
  uint16                        time_hysteresis;
} vbatt_efs_record_type;

#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */

/* Naturally-aligned data structures for memory */
typedef struct
{
  int16                         tx_power_limit;
  int16                         voltage_down;
  int16                         voltage_up;
} vbatt_stage_record_type;

typedef struct
{
  uint32                        tech_band;
  vbatt_stage_record_type       stages[VBATT_STAGES_MAX];
  uint16                        time_hysteresis;
} vbatt_record_type;

/*=============================================================================
                            FUNCTION DECLARATIONS
=============================================================================*/

/*=============================================================================

  FUNCTION:  vbatt_efs_init

=============================================================================*/
/*!
    @brief
    Initializes this module's internal data structures and reads VBATT records
    from EFS.

    @return
    None
*/
/*===========================================================================*/
void vbatt_efs_init(void);

/*=============================================================================

  FUNCTION:  vbatt_efs_deinit

=============================================================================*/
/*!
    @brief
    Frees all memory allocated by vbatt_efs_init().

    @return
    None
*/
/*===========================================================================*/
void vbatt_efs_deinit(void);

/*=============================================================================

  FUNCTION:  vbatt_lookup

=============================================================================*/
/*!
    @brief
    Looks up a VBATT record for the specified band.

    @param[in]  band    The tech/band to look up

    @return
    A pointer to the VBATT record if the band could be found; NULL otherwise.
*/
/*===========================================================================*/
const vbatt_record_type * vbatt_lookup(cxm_tech_type tech,
                                       cxm_lte_sub_tech_type sub_tech,
                                       sys_band_class_e_type band);

#endif /* VBATT_EFS_H */
