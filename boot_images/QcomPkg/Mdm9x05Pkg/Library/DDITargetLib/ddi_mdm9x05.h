/*==================================================================
 *
 * FILE:        ddi_mdm9x05.c
 *
 * DESCRIPTION:
 *
 *
 *        Copyright © 2016 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *               QUALCOMM Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 * YYYY-MM-DD   who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 2019-02-25   ds      DDI framwork re-architecture for Twizy
 * 2016-12-15   zhz     Initial file creation
 */


#include "ddi_firehose.h"
#include "boot_sbl_if.h"
#include "pm_config_sbl.h"

#define DDI_PLATFORM "MDM9205"
#define DDI_VERSION  1

void ddi_entry(boot_pbl_shared_data_type *pbl_shared);
boolean signalines_dotest(void* this_p);
boolean basic_dotest(void* this_p);

boolean ddr_defect_dotest(void* this_p);
boolean ddr_intervalstress_dotest(void* this_p);

boolean dqlineshmoo_init(void *this_p);
boolean dqlineshmoo_dotest(void* this_p);

boolean calineshmoo_init(void *this_p);
boolean calineshmoo_dotest(void* this_p);

boolean ddi_firehose_pre_work(void *this_p);
boolean ddi_firehose_post_work(void *this_p);

