/* Autogenerated file.  Manual modification is pointless. */

#include "sns_rc.h"
#include "sns_register.h"
#include "sns_types.h"

sns_rc sns_register_suid_sensor(sns_register_cb const *register_api);
sns_rc sns_register_cm(sns_register_cb const *register_api);
sns_rc sns_register_diag_sensor(sns_register_cb const *register_api);
sns_rc sns_register_dynamic_sensor(sns_register_cb const *register_api);
sns_rc sns_register_power_sensor(sns_register_cb const *register_api);
sns_rc sns_accel_cal_register(sns_register_cb const *register_api);
sns_rc sns_amd_register(sns_register_cb const *register_api);
sns_rc sns_async_com_port_register(sns_register_cb const *register_api);
sns_rc sns_da_test_register(sns_register_cb const *register_api);
sns_rc sns_ext_svc_register(sns_register_cb const *register_api);
sns_rc sns_register_interrupt(sns_register_cb const *register_api);
sns_rc sns_register_lps22hx(sns_register_cb const *register_api);
sns_rc sns_register_lsm6dsm(sns_register_cb const *register_api);
sns_rc sns_pedometer_register(sns_register_cb const *register_api);
sns_rc sns_remote_proc_state_register(sns_register_cb const *register_api);
sns_rc sns_resampler_register(sns_register_cb const *register_api);
sns_rc sns_register_shtw2(sns_register_cb const *register_api);
sns_rc sns_signal_register(sns_register_cb const *register_api);
sns_rc sns_smd_register(sns_register_cb const *register_api);
sns_rc sns_test_register(sns_register_cb const *register_api);
sns_rc sns_timer_register(sns_register_cb const *register_api);
sns_rc sns_register_tmd2725(sns_register_cb const *register_api);

const sns_register_entry sns_register_sensor_list[] =
{
 { sns_register_suid_sensor, 1},
 { sns_register_cm, 1},
 { sns_register_diag_sensor, 1},
 { sns_register_dynamic_sensor, 1},
 { sns_register_power_sensor, 1},
 { sns_accel_cal_register, 1},
 { sns_amd_register, 1},
 { sns_async_com_port_register, 1},
 { sns_da_test_register, 1},
 { sns_ext_svc_register, 1},
 { sns_register_interrupt, 1},
 { sns_register_lps22hx, 1},
 { sns_register_lsm6dsm, 1},
 { sns_pedometer_register, 1},
 { sns_remote_proc_state_register, 1},
 { sns_resampler_register, 1},
 { sns_register_shtw2, 1},
 { sns_signal_register, 1},
 { sns_smd_register, 1},
 { sns_test_register, 1},
 { sns_timer_register, 1},
 { sns_register_tmd2725, 1},
};

const uint32_t sns_register_sensor_list_len = ARR_SIZE(sns_register_sensor_list);

