#include "comdef.h"    /* Common definitions such as byte, uint16, etc. */
//#include "wwan_stubs.h"
//#include "trm.h"
#include "rfm_mode_types.h"
#include "rfm_device_types.h"
#include "npa.h"
#include "ftm.h"
#include "rfm_device_config_type.h"
#include "rex.h"
ftm_mode_type ftm_mode = AMSS_MODE;
rex_tcb_type     ftm_tcb;

typedef struct
{
  rfm_mode_enum_type    mode;
  rfm_path_state        path_state;
  rfm_device_enum_type  device_num;
} rfm_tech_info;

typedef struct
{
  rfm_tech_info       tech_A;
  rfm_tech_info       tech_B;
  uint8               npa_id;
  npa_client_handle   npa_handle;
} rfm_resource_info;

void rfm_get_awake_resource_request(rfm_resource_info *params);

void rfm_get_awake_resource_request(rfm_resource_info *params)
{
return;
}

boolean
rfm_get_devices_configuration
(
  rfm_devices_configuration_type *dev_cfg
);

boolean
rfm_get_devices_configuration
(
  rfm_devices_configuration_type *dev_cfg
)
{
return TRUE;
}



ftm_mode_type  ftm_get_mode( void);

ftm_mode_type  ftm_get_mode( void)
{
  return ftm_mode;
}


rex_sigs_type ftm_sig_rex_wait(  rex_sigs_type   mask        /* Mask of signals to wait for */);

rex_sigs_type ftm_sig_rex_wait
(
  rex_sigs_type   mask        /* Mask of signals to wait for */
)
{
  rex_sigs_type   sigs = 0;   /* Signals returned by rex_get_sigs call */

  return(sigs);
} /* End ftm_sig_rex_wait */