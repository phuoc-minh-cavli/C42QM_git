#ifndef _CAD_ASAC_INTF_H_
#define _CAD_ASAC_INTF_H_
/*! \file cad_asac_intf.h
	\brief This file defines internal interface between audio stream 
	and audio context
	
	Copyright (C) 2010 - 2012 QUALCOMM Technologies Incorporated.
	All Rights Reserved. 
	Qualcomm Proprietary and Confidential.
*/

/*===========================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd/common/inc/csd_asac_intf.h#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $
$Change: 17929171 $
$Revision: #1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/06/09   jzhou   Initial Creation

============================================================================*/
#include "mmdefs.h"

#define CSD_AC_OBJ_MAX_NUM       8

#define CSD_AC_AS_CMD_DEV_ADD        1 /** AC -> AS. AS-AC atatch */
#define CSD_AC_AS_CMD_DEV_ENABLE     2 /** AC -> AS  AS-AC detach */
#define CSD_AC_AS_CMD_DEV_DISABLE    3 /** AC -> AS  AC enable */
#define CSD_AC_AS_CMD_DEV_REMOVE     4 /** AC -> AS  AC disable*/

//#define CSD_AC_AS_CMD_AC_CLOSE       5 /** AC -> AS  AC Close*/
#define CSD_AC_AS_CMD_AS_CLOSE       6 /** AS -> AC  AS Close*/

/* CSD_AC_AS_CMD_DEV_ADD */
/* CSD_AC_AS_CMD_DEV_REMOVE */
struct csd_ac_as_dev_add_remove
{
	uint32_t ac_handle;     /* csd ac handle */
	uint32_t dev_id;        /* device id */
	uint32_t sample_rate;	/** sample rate */
	uint32_t ac_index;      /* AC index :: 0-(CSD_AC_OBJ_MAX_NUM-1) */
	uint32_t num_as_handles;
	uint32_t* as_handles;
};

/* CSD_AC_AS_CMD_DEV_ENABLE */
/* CSD_AC_AS_CMD_DEV_DISABLE */
struct csd_ac_as_dev_enable_disable
{
	uint32_t ac_handle;      /* csd ac handle */
	uint32_t copp_id;        /* Q6 assigned copp id */ 
	uint32_t ac_category;    /* ac app type */
	uint32_t direction;      /* ADM_MATRIX_ID_AUDIO_RX/ADM_MATRIX_ID_AUDIO_TX */
	uint32_t channel_config; /* 1, 2, 4, 6, 8, ... */
	uint32_t ac_index;       /* AC index :: 0-(CSD_AC_OBJ_MAX_NUM-1) */
	uint32_t num_as;         /* num as-ac pairs */
	uint32_t* as_handles;
};

/* CSD_AC_AS_CMD_AC_CLOSE (AC -> AS  AC Close ) (Currently not used/needed) */
struct csd_ac_as_as_list
{
    uint32_t ac_handle;
    uint32_t num_as;
    uint32_t *as_handles;
};
typedef struct csd_ac_as_as_list csd_ac_as_as_list_t;

/* CSD_AC_AS_CMD_AS_CLOSE   (AS -> AC  AS Close ) */
struct csd_ac_as_ac_list
{
    uint32_t as_handle;
    uint32_t num_ac;
    uint32_t *ac_handles;
};
typedef struct csd_ac_as_ac_list csd_ac_as_ac_list_t;
/*! \brief API functino for ac -> as  (as module implements this function) */
int32_t csd_as_ac_ioctl(uint32_t cmd, void* param, uint32_t len);
/*! \brief API functino for as -> ac  (ac module implements this function) */
int32_t csd_ac_as_ioctl(uint32_t cmd, void* param, uint32_t len);
#endif /* _CAD_ASAC_INTF_H_ */
