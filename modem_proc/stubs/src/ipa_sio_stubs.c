
/*
	In Modem-lite targets DATAMODEM code will not be compiled, because of which
	compilation will hit the linker error in core\wiredconnectivity\sio.
	
	As per the discussion with CORE team and DATAMODEM team, stubbing out
	ipa_sio_driver_init() for Modem-lite targets. This change will not have any
	side impact on Full modem build as STUBS component will not be compiled.

	Return value for the function is 1, because sio_set_driver_init_list() will
	assert if the return value is 0, thinking it is an error.
*/ 

#include "sio.h"

/*==============================================================================

FUNCTION:  ipa_sio_driver_init

==============================================================================*/
uint32 ipa_sio_driver_init
(
  sio_register_device_driver_cb_type register_driver_fn
)
{
  return 1; /* 1 for success and 0 for failure */
}

