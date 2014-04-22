/*===========================================================================

  Copyright (c) 2013,2017 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

===========================================================================*/

#ifndef __DEVICES_QMI_H__
#define __DEVICES_QMI_H__

int qmi_communication_init(void);
int qmi_communication_release(void);
int modem_request(int level);
int modem_cx_request(int level);
int vdd_restrict_qmi_request(int level);
int cpr_cold_qmi_request(int level);
int modem_proc_request(int level);
int modem_proc_current_request(int level);

#endif /* __DEVICES_QMI_H__ */
