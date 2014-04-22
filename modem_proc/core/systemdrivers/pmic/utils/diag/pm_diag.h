#ifndef __PM_DIAG_H__
#define __PM_DIAG_H__

/*! \file pm_diag.h
*
*  \brief Internal header file for the PMIC DIAG functionality.
*  \n
*  &copy; Copyright 2014-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/utils/diag/pm_diag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/28/14   kt      Created
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES

===========================================================================*/

#include "stringl.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "DALSys.h"

#include "pm_qc_pmic.h"
#include "pm_version.h"
#include "pm_resource_manager.h"
#include "DDITimetick.h"
#include "pm_target_information.h"
#include "device_info.h"

/*===========================================================================

                     MACRO DEFINITIONS

===========================================================================*/

#define PM_DIAG_PWR_RAILS_LOG_CMD_ID     10  /* PMIC Power rails Subsystem Command ID */
#define PM_DIAG_CLOCKS_LOG_CMD_ID        11  /* PMIC Clocks Subsystem Command ID */
#define PM_DIAG_GPIOS_LOG_CMD_ID         12  /* PMIC GPIOs Subsystem Command ID */
#define PM_DIAG_MPPS_LOG_CMD_ID          13  /* PMIC MPPs Subsystem Command ID */
#define PM_DIAG_PEEK_POKE_LOG_CMD_ID     14  /* PMIC PEEK-POKE Subsystem Command ID */
#define PM_DIAG_PMIC_INFO_LOG_CMD_ID     15  /* PMIC Info Subsystem Command ID */
#define PM_DIAG_PBS_INFO_LOG_CMD_ID      16  /* PBS Info Subsystem Command ID */

/*
 * # of ANSI characters(bytes) allocated for a PMIC name
 */
#define PM_DIAG_PMIC_NAME_LEN      16

/*===========================================================================

                     TYPE DEFINITIONS

===========================================================================*/

/*
 * Result used in QXDM/Diag communications.
 */
typedef enum
{
  PM_DIAG_SUCCESS,
  PM_DIAG_ERROR
} pm_diag_result_type;

/*
 * Command identifier used in QXDM/Diag communications.
 */
typedef enum
{
  PM_DIAG_LOGGING_START,
  PM_DIAG_LOGGING_STOP
} pm_diag_command_type;

/*
 * This structure is used to describe the Power rails diag 
 * log packet sent back to the test tool. This structure 
 * is appended * to Diag response packet structure in 
 * memory prior to submitting to Diag.
 */
typedef PACK(struct)
{
  uint64       timestamp;                          /* Power rail log time stamp */
  uint32       volt_level;                         /* Power rail Voltage Level */
  char         pmic_name[PM_DIAG_PMIC_NAME_LEN];   /* PMIC Model name to which the Power rail belongs */
  uint8        pmic_model_type;                    /* PMIC Model type to which the Power rail belongs */
  uint8        rail_type;                          /* Power rail type (SMPS,BOOST,LDO,VS) */
  uint8        periph_index;                       /* Power rail peripheral index */
  uint8        enable_status;                      /* Power rail Enable Status */
  uint8        sw_status;                          /* Power rail SW Enable Status */
  uint8        pin_ctrl_status;                    /* Power rail Pin Ctrl Status */
  uint8        mode_type;                          /* Power rail Mode Type (LPM,NPM,AUTO,BYPASS) */
} pm_pwr_rails_diag_log_item_type;

/*
 * This structure is used to describe the Clocks diag 
 * log packet sent back to the test tool. This structure 
 * is appended to Diag response packet structure in 
 * memory prior to submitting to Diag.
 */
typedef PACK(struct)
{
  uint64       timestamp;                          /* Clock log time stamp */
  char         pmic_name[PM_DIAG_PMIC_NAME_LEN];   /* PMIC Model name to which the Clock belongs */
  uint8        clk_type;                           /* Clock type */
  uint8        periph_index;                       /* Clock peripheral index */
  uint8        clk_ok_status;                      /* Clock OK Status */
  uint8        sw_enable_status;                   /* Clock SW Enable Status */
  uint8        pin_ctrl_status;                    /* Clock Pin Ctrl Status */
  uint8        drv_strength;                       /* Clock Drive Strength */
  uint8        pull_down_status;                   /* Clock Pull Down */
  uint8        status;                             /* Clock read status */
} pm_clocks_diag_log_item_type;

/*
 * This structure is used to describe the GPIOs diag 
 * log packet sent back to the test tool. This structure 
 * is appended to Diag response packet structure in 
 * memory prior to submitting to Diag.
 */
typedef PACK(struct)
{
  uint64       timestamp;                          /* GPIO log time stamp */
  char         pmic_name[PM_DIAG_PMIC_NAME_LEN];   /* PMIC Model name to which the GPIO belongs */
  uint8        periph_index;                       /* GPIO peripheral index */
  uint8        gpio_ok_status;                     /* GPIO OK Status */
  uint8        sw_enable_status;                   /* GPIO SW Enable Status */
  uint8        mode_select;                        /* GPIO Mode Status */
  uint8        output_select;                      /* GPIO Output Select */
  uint8        output_select_inverted;             /* GPIO Output Select Inverted */
  uint8        volt_source;                        /* GPIO Voltage Source */
  uint8        pull_type;                          /* GPIO Pull Type */
  uint8        output_drv;                         /* GPIO Output Drive Strength */
  uint8        output_buffer;                      /* GPIO Output Buffer */
  uint8        dtest_buffer;                       /* GPIO DTEST Buffer status */
  uint8        input_value;                        /* GPIO Input Value */
  uint8        status;                             /* GPIO read status */
} pm_gpios_diag_log_item_type;

/*
 * This structure is used to describe the MPPs diag 
 * log packet sent back to the test tool. This structure 
 * is appended to Diag response packet structure in 
 * memory prior to submitting to Diag.
 */
typedef PACK(struct)
{
  uint64       timestamp;                          /* MPP log time stamp */
  char         pmic_name[PM_DIAG_PMIC_NAME_LEN];   /* PMIC Model name to which the MPP belongs */
  uint8        periph_index;                       /* MPP peripheral index */
  uint8        mpp_ok_status;                      /* MPP OK Status */
  uint8        sw_enable_status;                   /* MPP SW Enable Status */
  uint8        mode_select;                        /* MPP Mode status */
  uint8        config_select;                      /* MPP config */
  uint8        level_select;                       /* MPP level */
  uint8        input_value;                        /* MPP Input Value */
  uint8        status;                             /* MPP read status */
} pm_mpps_diag_log_item_type;

/*
 * This structure is used to describe the PEEK-POKE diag 
 * log packet sent back to the test tool. This structure 
 * is appended to Diag response packet structure in 
 * memory prior to submitting to Diag.
 */
typedef PACK(struct)
{
  uint64       timestamp;                          /* PEEK-POKE log time stamp */
  char         pmic_name[PM_DIAG_PMIC_NAME_LEN];   /* PMIC Model name to which the Address belongs */
  uint16       reg_addr;                           /* PEEK-POKE address */
  uint8        pmic_index;                         /* PEEK-POKE PMIC index */
  uint8        sid_index;                          /* PEEK-POKE slave id index */
  uint8        reg_data;                           /* PEEK-POKE data */
  uint8        peek_poke_type;                     /* PEEK-POKE command type */
  uint8        status;                             /* PEEK-POKE status */
} pm_peek_poke_diag_log_item_type;

/*
 * This structure is used to describe the PMIC Info diag 
 * log packet sent back to the test tool. This structure 
 * is appended to Diag response packet structure in 
 * memory prior to submitting to Diag.
 */
typedef PACK(struct)
{
  char         pmic_name[PM_DIAG_PMIC_NAME_LEN];   /* PMIC Model name */
  uint8        pmic_index;                         /* PMIC Index */
  uint8        pmic_model;                         /* PMIC Model type */
  uint8        all_layer_rev;                      /* PMIC All Layer Revision (Analog Major Rev) */
  uint8        metal_rev;                          /* PMIC Metal Revision (Analog Minor Rev) */
  uint8        status;                             /* Info read status */
} pm_pmic_info_diag_log_item_type;

/*
 * This structure is used to describe the PBS Info diag 
 * log packet sent back to the test tool. This structure 
 * is appended to Diag response packet structure in 
 * memory prior to submitting to Diag.
 */
typedef PACK(struct)
{
  char         pmic_name[PM_DIAG_PMIC_NAME_LEN]; /* PMIC Model name */
  uint8        lot_id[PM_PBS_INFO_NUM_LOT_IDS];  /* PBS Lot Id */
  uint8        otp_rev_id;                       /* PBS OTP Rev id */
  uint8        otp_branch_id;                    /* PBS OTP Branch id */
  uint8        ram_rev_id;                       /* PBS RAM Rev id */
  uint8        ram_branch_id;                    /* PBS RAM Branch id */
  uint8        fab_id;                           /* PBS Fab Id */
  uint8        wafer_id;                         /* PBS Wafer Id */
  uint8        x_coord;                          /* PBS X Coord */
  uint8        y_coord;                          /* PBS Y Coord */
  uint8        test_pgm_rev;                     /* PBS Test PGM Rev */
  uint8        mfg_id_major;                     /* PBS MGF ID Major */
  uint8        mfg_id_minor;                     /* PBS MGF ID Minor */
  uint8        mfg_id_shrink;                    /* PBS MGF ID Shrink */
  uint8        pmic_index;                       /* PMIC Index */
  uint8        pmic_model;                       /* PMIC Model type */
  uint8        status;                           /* Info read status */
} pm_pbs_info_diag_log_item_type;

/*===========================================================================

                     FUNCTION PROTOTYPES

===========================================================================*/

/* =========================================================================
**  Function : pm_diag_model_lookup
** =========================================================================*/
/**
  Lookup function to return the PMIC model string name based on
  PMIC Model type.

  @param model_type   [in]  - PMIC Model type.

  @return
  const char * -- PMIC Model name.

  @dependencies
  None.
*/
const char * pm_diag_model_lookup(pm_model_type model_type);

/* =========================================================================
**  Function : pm_diag_init
** =========================================================================*/
/**
  Initialization function for the PMIC Diag functionality.

  @param
  None.

  @return
  None.

  @dependencies
  None.
*/
void pm_diag_init (void);

/* =========================================================================
**  Function : pm_diag_pwr_rails_init
** =========================================================================*/
/**
  Initialization function for the PMIC Power rails Diag functionality.

  @param
  None.

  @return
  None.

  @dependencies
  None.
*/
void pm_diag_pwr_rails_init (void);

/* =========================================================================
**  Function : pm_diag_clocks_init
** =========================================================================*/
/**
  Initialization function for the PMIC Clocks Diag functionality.

  @param
  None.

  @return
  None.

  @dependencies
  None.
*/
void pm_diag_clocks_init (void);

/* =========================================================================
**  Function : pm_diag_gpios_init
** =========================================================================*/
/**
  Initialization function for the PMIC GPIOs Diag functionality.

  @param
  None.

  @return
  None.

  @dependencies
  None.
*/
void pm_diag_gpios_init (void);

/* =========================================================================
**  Function : pm_diag_mpps_init
** =========================================================================*/
/**
  Initialization function for the PMIC MPPs Diag functionality.

  @param
  None.

  @return
  None.

  @dependencies
  None.
*/
void pm_diag_mpps_init (void);

/* =========================================================================
**  Function : pm_diag_peek_poke_init
** =========================================================================*/
/**
  Initialization function for the PMIC Peek-Poke Diag functionality.

  @param
  None.

  @return
  None.

  @dependencies
  None.
*/
void pm_diag_peek_poke_init (void);

/* =========================================================================
**  Function : pm_diag_pmic_info_init
** =========================================================================*/
/**
  Initialization function for the PMIC Info Diag functionality.

  @param
  None.

  @return
  None.

  @dependencies
  None.
*/
void pm_diag_pmic_info_init (void);

/* =========================================================================
**  Function : pm_diag_pbs_info_init
** =========================================================================*/
/**
  Initialization function for the PBS Info Diag functionality.

  @param
  None.

  @return
  None.

  @dependencies
  None.
*/
void pm_diag_pbs_info_init (void);

#endif /* __PM_DIAG_H__ */

