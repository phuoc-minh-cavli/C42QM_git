#ifndef THERM_V_H
#define THERM_V_H

/*============================================================================
  FILE:         therm_v.h

  OVERVIEW:     This file declares the private interface to thermal SW.

                Copyright (c) 2011-2019 QUALCOMM Technologies
                Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.mpss/3.10/power/thermal/inc/therm_v.h#3 $
$DateTime: 2019/02/04 21:51:42 $
$Author: pwbldsvc $
========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "rex.h"
#include "qmi_csi.h"
#include "npa_resource.h"

/*
===============================================================================

                             TYPE DEFINITION(S)

===============================================================================
*/
#define THERM_SAMPLING_LOOP_TIMER_SIG     0x00000001
#define THERM_GCF_SAMPLING_LOOP_TIMER_SIG 0x00000002
#define THERM_QMI_MITIGATION_DEV_SIG      0x00000004
#define THERM_QMI_SENSOR_SIG              0x00000008
#define THERM_QMI_TMD_COLD_RSTR_TIMER_SIG 0x00000010

#define THERM_NV_RD_WR_SIG                0x80000000

#define THERM_TASK_SIG_MASK (THERM_SAMPLING_LOOP_TIMER_SIG | \
                             THERM_GCF_SAMPLING_LOOP_TIMER_SIG | \
                             THERM_QMI_MITIGATION_DEV_SIG | \
                             THERM_QMI_SENSOR_SIG | \
                             THERM_QMI_TMD_COLD_RSTR_TIMER_SIG)

typedef enum
{
  THERM_NPA_HKADC_SENSOR = 0,
  THERM_NPA_TSENS_SENSOR,
  THERM_NPA_LOGICAL_SENSOR,
  THERM_NPA_QFE_SENSOR,
  THERM_NPA_MAX_SENSOR = THERM_NPA_QFE_SENSOR,
} therm_npa_sensor_type;

typedef struct
{
  /* Diag log packet subsystem id */
  unsigned int subsystem_id;
  /* Diag log packet sensor id */
  unsigned int sensor_id;
  /* Units of measurement Eg. Degrees C, .001 Degrees C */
  const char  *units_of_measure;
  /* Number of units per whole integer value */
  unsigned int unit_per_whole;
  /* Sensor input name */
  const char  *sensor_input_str;
  /* NPA node name */
  const char  *npa_node_str;
  /* Therm sensor types */
  therm_npa_sensor_type sensor_type;
  /* Sensor type specific data */
  void *sensor_data;
  /* Export via QMI TS */
  boolean export;

} therm_sensor_info;

/*===========================================================================

                     EXTERNAL VARIABLE DECLARATIONS

===========================================================================*/
extern uint32 gcf_detect_disable;

/*
===============================================================================

                           GLOBAL FUNCTION DECLARATIONS

===============================================================================
*/

/**
  @brief therm_target_init

  Initializes target specific thermal SW.

*/
extern void therm_target_init(void);

/**
  @brief therm_target_after_task_start_init

  Initialize target specific thermal SW after thermal
  task has started.
*/
extern void therm_target_after_task_start_init(void);

/**
  @brief therm_target_process_sigs

  Process rex signals for target specific SW.

  @param rcvdSigs
*/
extern void therm_target_process_sigs(rex_sigs_type rcvdSigs);

/**
  @brief therm_monitor_init

  Initializes thermal monitor.

*/
extern void therm_monitor_init(void);

/**
  @brief therm_monitor_after_task_start_init

  Initialize thermal monitor after thermal task has
  started.

*/
extern void therm_monitor_after_task_start_init(void);

/**
  @brief therm_monitor_process_sigs

  Process rex signals for thermal monitor.

  @param rcvdSigs
*/
extern void therm_monitor_process_sigs(rex_sigs_type rcvdSigs);

/**
  @brief therm_qmi_dev_service_init

  This function initializes the QMI thermal mitigation device
  services handler.

  @param  : os_params: OS parameters.

  @return : qmi_csi_service_handle.
*/
extern qmi_csi_service_handle therm_qmi_dev_service_init(qmi_csi_os_params *os_params);

/**
  @brief therm_qmi_dev_service_process_sigs

  Process rex signals for qmi dev service.

  @param rcvdSigs
*/
extern void therm_qmi_dev_service_process_sigs(rex_sigs_type rcvdSigs);

/**
  @brief therm_qmi_sensor_service_init

  This function initializes the QMI thermal sensors services
  handler.

  @param  : os_params: OS parameters.

  @return : qmi_csi_service_handle.
*/
qmi_csi_service_handle therm_qmi_sensor_service_init(qmi_csi_os_params *os_params);

#ifdef __cplusplus
}
#endif

#endif /* THERM_V_H */
