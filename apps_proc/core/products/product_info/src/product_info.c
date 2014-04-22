/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      Product info 
                
GENERAL DESCRIPTION
  Command handler for returning product specific info through diag packets

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Diag must be initialized

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.tx/6.0/products/product_info/src/product_info.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
2/28/13   dk     File created.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "product_info.h"
#include "err.h"
#include "DDIChipInfo.h"


/*===========================================================================

                              Function definitions

===========================================================================*/

//gives the shared mem base address
const uint32 g_products_smem_base_addr = SCL_SHARED_RAM_BASE;

/*===========================================================================

FUNCTION product_info_init

DESCRIPTION
  Registers diag command for the product info API

DEPENDENCIES
  diag must be available to register command handler

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void product_info_init (void)
{

  DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_APP_PROC, DIAG_SUBSYS_COREBSP, product_info_diagpkt_tbl);
                                         
}


/*===========================================================================

FUNCTION product_info

DESCRIPTION
  Return product info

DEPENDENCIES
  product_info_init must be called prior

RETURN VALUE
  Pointer to the response packet of the req_ptr
  Note: should never return

SIDE EFFECTS

===========================================================================*/
PACK(void *)
product_info (PACK(void *) req_ptr, uint16 pkt_len)
{
  //product_info_req_type *req;
  product_info_rsp_type *rsp = NULL;
  pm_err_flag_type PM_ERR_FLAG__STATUS;
  pm_device_info_type pmic_info_raw;
  pm_pbs_info_type pbs_info_raw;
  uint8 i;
  uint32* raw_ptr = NULL;
    DalDeviceHandle      *phChipInfo = NULL;
    DalChipInfoVersionType dload_addr_ChipInfoVersion = 0;


  /* send response immediately, so tool won't timeout */
  rsp = (product_info_rsp_type *) diagpkt_subsys_alloc(
     DIAG_SUBSYS_COREBSP,                       
     PRODUCT_INFO_SSID,
     sizeof( product_info_rsp_type ));

  if (NULL == rsp)
	  ERR_FATAL("Response packet is NULL ", 0, 0, 0);

    if (DAL_SUCCESS == DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo))
    {
        if (DAL_SUCCESS == DalChipInfo_GetChipVersion(phChipInfo, &dload_addr_ChipInfoVersion))
        {
		rsp->jtag_id = dload_addr_ChipInfoVersion;
	}
    }
  /* Serial INFO */
  raw_ptr = (uint32*)(QFPROM_RAW_JTAG_ID_LSB);
  rsp->serial_lsb = *raw_ptr ;
  raw_ptr = (uint32*)(QFPROM_RAW_JTAG_ID_MSB);
  rsp->serial_msb = *raw_ptr ;

  /* PMIC INFO */

  PM_ERR_FLAG__STATUS = pm_get_pmic_info(0, &pmic_info_raw);
  if (PM_ERR_FLAG__STATUS != PM_ERR_FLAG__SUCCESS)
  {
	  ERR_FATAL("PMIC INFO API failed to return ", 0, 0, 0);
  }
  rsp->pmic_info.PmicModel = pmic_info_raw.ePmicModel;
  rsp->pmic_info.PmicAllLayerRevision = pmic_info_raw.nPmicAllLayerRevision;
  rsp->pmic_info.PmicMetalRevision = pmic_info_raw.nPmicMetalRevision;

  /* PBS INFO */
  PM_ERR_FLAG__STATUS = pm_get_pbs_info(0, &pbs_info_raw);
  if (PM_ERR_FLAG__STATUS != PM_ERR_FLAG__SUCCESS)
  {
	  ERR_FATAL("PBS INFO API failed to return ", 0, 0, 0);
  }
  for (i = 0; i < PM_PBS_INFO_NUM_LOT_IDS; i++)
	  rsp->pmic_info.PmicLotId[i] = pbs_info_raw.nLotId[i];
  rsp->pmic_info.PmicPBSROMVersion = pbs_info_raw.nPBSROMVersion;
  rsp->pmic_info.PmicPBSRAMVersion = pbs_info_raw.nPBSRAMVersion;

  /* DDR INFO */
  raw_ptr = (uint32*)(QFPROM_RAW_FEAT_CONFIG_ROW3_MSB);
  rsp->ddr_info.ddr_manufacture = *raw_ptr & 0x1f;

  diagpkt_commit(rsp);


  return (rsp);
}


