#ifndef _CSD_INTF_H_
#define _CSD_INTF_H_
/*! \file csd_intf.h
    \brief CSD module common definition

    Copyright (C) 2010 - 2012 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Proprietary and Confidential.
*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd/common/inc/csd_intf.h#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $
$Change: 17929171 $
$Revision: #1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/10   zj      initial release

============================================================================*/
#include "mmdefs.h"
#include "csd.h"


/*! \struct csd vtable */
typedef uint32_t (*csd_open_fn)(enum csd_open_code code, void* open_struct, uint32_t len);
typedef int32_t (*csd_close_fn)(uint32_t handle);
typedef int32_t (*csd_write_fn)(uint32_t handle, void* payload, uint32_t len);
typedef int32_t (*csd_read_fn)(uint32_t handle, void* payload, uint32_t len);
typedef int32_t (*csd_ioctl_fn)(uint32_t handle, uint32_t cmd, void* payload, uint32_t len);
struct csd_vtable
{
    csd_open_fn     open_fn;        /**< Open function */
	csd_close_fn	close_fn;		/**< Close function */
	csd_write_fn	write_fn;		/**< Write function */
	csd_read_fn	    read_fn;		/**< Read function */
	csd_ioctl_fn	ioctl_fn;       /**< IOCTL function */
};



/**-------------------------------------------------------------------------*/
/*! \brief	csd components initialization functions                         */
/**-------------------------------------------------------------------------*/

/*! \brief  csd audio stream initialization */
int32_t csd_as_init(struct csd_vtable ** vtable);
int32_t csd_as_dinit(void);
int32_t csd_as_debug_init(struct csd_vtable ** vtable);
int32_t csd_as_debug_dinit(void);

/*! \brief  csd audio context initialization */
int32_t csd_ac_init(struct csd_vtable ** vtable);
int32_t csd_ac_dinit(void);
int32_t csd_ac_debug_init(struct csd_vtable ** vtable);
int32_t csd_ac_debug_dinit(void);

/*! \brief  csd dev initialization */
int32_t csd_dev_init(struct csd_vtable ** vtable);
int32_t csd_dev_dinit(void);
int32_t csd_dev_debug_init(struct csd_vtable ** vtable);
int32_t csd_dev_debug_dinit(void);


/*! \brief  csd ATL initialization */
int32_t csd_atl_init(void);
int32_t csd_atl_dinit(void);

/*! \brief  csd acdb initialization */
int32_t csd_acdb_init(void);
int32_t csd_acdb_dinit(void);

/*! \brief  csd handle initialization */
int32_t csd_handle_init(uint32_t num);
int32_t csd_handle_dinit(void);

/*! \brief  csd handle initialization */
int32_t csd_pmem_init(void);
int32_t csd_pmem_deinit(void);

/*! \brief  csd vs initialization */
int32_t csd_vs_init(struct csd_vtable ** vtable);
int32_t csd_vs_deinit(void);

/*! \brief  csd vs initialization */
int32_t csd_vc_init(struct csd_vtable ** vtable);
int32_t csd_vc_deinit(void);

/*! \brief  csd vm initialization */
int32_t csd_vm_init(struct csd_vtable ** vtable);
int32_t csd_vm_deinit(void);

/*! \brief  csd vm initialization */
int32_t csd_voice_debug_init(struct csd_vtable ** vtable);
int32_t csd_voice_debug_deinit(void);

/*! \brief  csd rm initialization */
int32_t csd_rm_init(uint32_t num);
int32_t csd_rm_deinit(void);

/*! \brief  csd ssr initialization */
int32_t csd_ssr_init( void );
int32_t csd_ssr_deinit( void );

/*! \brief  csd alm initialization */
int32_t csd_alm_init(struct csd_vtable ** vtable);
int32_t csd_alm_deinit(void);

/*! \brief  csd ocmem initialization */
int32_t csd_ocmem_init(void);
int32_t csd_ocmem_dinit(void);

/*! \brief  csd core query adsp version */
int32_t csd_core_query_adsp_version(void);

#endif /* _CSD_INTF_H_ */

