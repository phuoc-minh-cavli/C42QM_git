#include <stdio.h>
#include <stdlib.h>
#include "qurt_qdi_driver.h"
#include "qurt.h"

/*
 * Singleton states:
 * 0 ->QURT_QDI_SINGLETON_TYPE_TRUE
 * 1 ->QURT_QDI_SINGLETON_TYPE_FALSE
 * 2 ->QURT_QDI_SINGLETON_TYPE_PER_PROCESS
 * By default singelton state is QURT_QDI_SINGLETON_TYPE_PER_PROCESS
 */

 /*
  * Island states:
  * 0 ->Island mode False
  * 1 ->Island mode True
  * By default island mode is False
  */

extern int {0} (int client_id, const char *name, qurt_qdi_version_t version, qurt_qdi_man_obj_t **qdi_obj);
extern int {1} (QDI_EXT_INVOKE_ARGS);
extern void {2} (qurt_qdi_man_obj_t *qdi_obj);

qurt_qdi_man_device {3}_qdi_device= {{ {{qurt_qdi_ext_opener, QDI_REFCNT_PERM, 0}}, //QDI Opener object
										"{3}", 										//Driver name
										{5},										//Island state
										{6}, 										//Singleton state
										{0}, 										//Create function
										{1}, 										//Invication function
										{2},										//Destroy function
										QURT_MUTEX_INIT,							//Mutex initialization
										NULL										//List head initialization
									}};

void qurt_qdi_{3}_set_singleton(unsigned char singleton)
{{
	 {3}_qdi_device.singleton = singleton;
}}

void {4} (void)
{{
    /* Register the device */
    qurt_qdi_devname_register ({3}_qdi_device.name, &{3}_qdi_device.opener_obj);
    
}}

