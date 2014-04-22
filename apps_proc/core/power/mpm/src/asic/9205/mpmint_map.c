/**
@file mpmint_map.c
 
@brief MPM HAL <--> QGIC mapping info.

*/

/*-----------------------------------------------------------------------------
             Copyright © 2013-2018, 2020 QUALCOMM Technologies, Incorporated.
                 All Rights Reserved.
             QUALCOMM Confidential and Proprietary.
-----------------------------------------------------------------------------*/

/*=============================================================================

$Header: //components/rel/core.tx/6.0/power/mpm/src/asic/9205/mpmint_map.c#2 $

=============================================================================*/


/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "mpmint_target.h"
#include "mpminti.h"


/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

/**
 * Table mapping the external enumeration's representation of interrupts to the
 * HAL's internal enumeration values.
 * IMPORTANT: This list must be in the same order as mpmint_isr_type.
 */
mpmint_irq_data_type mpmint_isr_tbl[] =
{
  /* 0 */
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_00,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_01,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_02,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_03,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO5,                          MPMINT_NULL_IRQ),

  /* 5 */
  MPMINT_DEFINE_IRQ_MAPPING(GPIO0,                          MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO2,                          MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_07,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_08,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO9,                          MPMINT_NULL_IRQ),
  
  /* 10 */
  MPMINT_DEFINE_IRQ_MAPPING(GPIO13,                         MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO1,                          MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO20,                         MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO21,                         MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO22,                         MPMINT_NULL_IRQ),

  /* 15 */
  MPMINT_DEFINE_IRQ_MAPPING(GPIO50,                         MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO52,                         MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO28,                         MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO51,                         MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO26,                         MPMINT_NULL_IRQ),

  /* 20 */
  MPMINT_DEFINE_IRQ_MAPPING(GPIO4,                          MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO47,                         MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO6,                          MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO8,                          MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO14,                         MPMINT_NULL_IRQ),

  /* 25 */
  MPMINT_DEFINE_IRQ_MAPPING(GPIO19,                         MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO27,                         MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO30,                         MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO36,                         MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(GPIO46,                         MPMINT_NULL_IRQ),

  /* 30 */
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_30,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_31,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_32,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_33,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_34,                     MPMINT_NULL_IRQ),

  /* 35 */
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_35,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_36,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_37,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_38,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_39,                     MPMINT_NULL_IRQ),

  /* 40 */
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_40,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(TSENS_UPPER_LOWER,              216            ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_INT,                        214            ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_43,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_44,                     MPMINT_NULL_IRQ),

  /* 45 */
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_45,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(SRST_N,                         MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(USB2_PHY_SVLD,                  168            ),
  MPMINT_DEFINE_IRQ_MAPPING(USB2_PHY_ID,                    MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(USB2_PHY_DPSE,                  MPMINT_NULL_IRQ),

  /* 50 */
  MPMINT_DEFINE_IRQ_MAPPING(GPIO49,                         MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(USB20_POWER_EVENT,              74             ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_52,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_53,                     MPMINT_NULL_IRQ            ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_54,                     MPMINT_NULL_IRQ),

  /* 55 */
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_55,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_56,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_57,                     MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(USB2_PHY_DMSE,                  168            ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_59,                     MPMINT_NULL_IRQ),

  /* 60 */
  MPMINT_DEFINE_IRQ_MAPPING(UIM_CONTROLLER_CARD_EVENT,      MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(UIM_CONTROLLER_BATT,            MPMINT_NULL_IRQ),
  MPMINT_DEFINE_IRQ_MAPPING(MPM_SPMI_WAKE,                  222            ),

  /* 63 */
  MPMINT_DEFINE_IRQ_MAPPING(MPM_BIT_63,                     MPMINT_NULL_IRQ),
};


/* Number of entries in the mapping table. */
const uint32 mpmint_isr_tbl_size = sizeof(mpmint_isr_tbl) /
                                   sizeof(mpmint_irq_data_type);

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

boolean mpmint_get_isr_tbl_index
(
  mpmint_isr_type in_id,
  uint32          *out_id
)
{
  if ( in_id < mpmint_isr_tbl_size )
  {
    /* 1:1 mapping */
    *out_id = in_id;
    return TRUE;
  }
  return FALSE;
}

