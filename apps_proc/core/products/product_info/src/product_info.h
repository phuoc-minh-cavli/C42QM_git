
#include "diagpkt.h"
#include "diagcmd.h"
#include "osal.h"
#include "pm_version.h"

/*===========================================================================

                     INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
PACK(void *) product_info (PACK(void *), uint16);


#define PRODUCT_INFO_SSID 3448
#define QFPROM_RAW_FEAT_CONFIG_ROW3_MSB 0xc1db8114
#define QFPROM_RAW_JTAG_ID_LSB 0xc1db80a0
#define QFPROM_RAW_JTAG_ID_MSB 0xc1db80a4

/*===========================================================================

                     INTERNAL DATA DECLARATIONS

===========================================================================*/


/* table for diag packet command handler registration */
static const diagpkt_user_table_entry_type product_info_diagpkt_tbl[] =
{
  {PRODUCT_INFO_SSID, PRODUCT_INFO_SSID, product_info}
};

typedef enum
{
  PRODUCT_INFO_CHIPINFO,
  PRODUCT_INFO_PMIC,

  SIZEOF_PRODUCT_INFO_TYPE
} product_info_type;

typedef PACK(struct) 
{
  diagpkt_subsys_header_type hdr;
  uint8 crash_cause;
  uint8 crash_delay;
} product_info_req_type;


typedef PACK(struct)
{
  //pm_pbs_info_type pmic_pbs_info;
  uint32 PmicModel;
  uint32 PmicAllLayerRevision;
  uint32 PmicMetalRevision;
  uint32 pmic_reserve1;
  uint32 pmic_reserve2;
  uint8          PmicLotId[PM_PBS_INFO_NUM_LOT_IDS];  /**< PMIC device lot number */
  uint32         PmicPBSROMVersion;                   /**< PBS ROM Version number */
  uint32         PmicPBSRAMVersion;                   /**< PBS RAM Version number */
}product_info_pmic;

typedef PACK(struct)
{
  uint32 ddr_manufacture;
  uint32 ddr_reserve1;
  uint32 ddr_reserve2;
}product_info_ddr;


typedef PACK(struct) {
  diagpkt_subsys_header_type hdr;
  uint32 jtag_id;
  uint32 serial_msb;
  uint32 serial_lsb;
  product_info_pmic pmic_info;
  product_info_ddr ddr_info;  
  uint32 reserve1;
  uint32 reserve2;
  uint32 reserve3;
  uint32 reserve4;
  uint32 reserve5;
  uint32 reserve6;
  uint32 reserve7;
  uint32 reserve8;
} product_info_rsp_type;



