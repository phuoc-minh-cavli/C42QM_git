/*===========================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

===========================================================================*/

#ifndef __QMI_TS_SENSOR_H__
#define __QMI_TS_SENSOR_H__

#include "sensors_manager_internal.h"

int modem_ts_qmi_init(void);
int modem_qmi_ts_comm_release(void);
int modem_ts_temp_request(const char *sensor_id, int send_current_temp_report,
                          struct thresholds_req_t *thresh);

int  qmi_ts_setup(struct sensor_info *sensor);
void qmi_ts_shutdown(struct sensor_info *sensor);
int  qmi_ts_get_temperature(struct sensor_info *sensor);
void qmi_ts_interrupt_wait(struct sensor_info *sensor);
void qmi_ts_update_thresholds(struct sensor_info *sensor,
                              struct thresholds_req_t *thresh);
void qmi_ts_update_temperature(const char *sensor, int temperature);

#endif  /* __QMI_TS_SENSOR_H__ */
