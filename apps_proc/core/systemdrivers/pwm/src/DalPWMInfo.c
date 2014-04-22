#include "DALReg.h"
#include "DALDeviceId.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult PWM_DalPWM_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

const DALREG_DriverInfo
DALPWM_DriverInfo = {PWM_DalPWM_Attach, 0, NULL};
