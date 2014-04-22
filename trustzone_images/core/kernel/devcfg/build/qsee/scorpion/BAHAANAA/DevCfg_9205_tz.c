#include "DALPropDef.h"
#include "DALDeviceId.h"
#include "DALPropDef.h"
#include "HALvmidmt.h"
#include "HALvmidmtTarget.h"
#include "HALcomdef.h"
#include "com_dtypes.h"
#include "HALvmidmtInfoImpl.h"
#include "HALhwio.h"
#include "HALvmidmtHWIOTarget.h"
#include "msmhwiobase.h"
#include "ddr_defs.h"
#include "tzbsp_target.h"
#include "customer.h"
#include "custbahaanaaa.h"
#include "targbahaanaaa.h"
#include "custremoteapis.h"
#include "custtarget.h"
#include "custsdcc.h"
#include "custsurf.h"
#include "custdiag.h"
#include "custefs.h"
#include "custpmic.h"
#include "custsio_8660.h"
#include "custsec.h"
#include "custsfs.h"
#include "tzbsp_tz_memmap.h"
#include "HALqgic.h"
#include "timer_defs.h"
#include "dbg_cfg.h"
#include "dbg_cfg_arch.h"
#ifndef DEVCFG_STANDALONE 
#include "DALStdDef.h" 
#include "DALSysTypes.h" 

#else 
#include "stubs.h" 

#include "devcfgTypes.h" 

#endif 
extern void * bam_tgt_config_9205_tz;
extern void * spmiInfo_9205_tz;
extern void * info_9205_tz;
extern void * NOCERR_propdata_9205_tz;
extern void * ABT_propdata_9205_tz;
extern void * BIMC_propdata_9205_tz;
extern void * icbcfg_boot_prop_9205_tz;
extern void * map_ddr_regions_9205_tz;
extern void * bimc_hal_info_9205_tz;
extern void * channel_map_9205_tz;
extern void * safe_reset_seg_9205_tz;


static HAL_vmidmt_InfoType	devcfg_11={((0x0008c000+0x00000000)+0x00000000),{4,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_12={((0x00090000+0x00000000)+0x00000000),{4,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_13={((0x00094000+0x00000000)+0x00000000),{128,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_14={((0x00096000+0x00000000)+0x00000000),{128,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_15={((0x00098000+0x00000000)+0x00000000),{128,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_16={((0x0009a000+0x00000000)+0x00000000),{128,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_17={((0x00200000+0x00088000)+0x00000000),{1,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_18={((0x004b0000+0x00001000)+0x00000000),{1,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_19={((0x00700000+0x00000000)+0x00000000),{17,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_20={((0x06000000+0x00088000)+0x00000000),{2,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_21={((0x06000000+0x00049000)+0x00000000),{2,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_22={((0x07880000+0x00000000)+0x00000000),{17,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_23={((0x07980000+0x00000000)+0x00000000),{10,0,0,0,0,0}};


static ddr_config_data_t	devcfg_24={4,(0x80000000+0x01EC0000),0x0140000,};


static HAL_qgic_BaseAddressType	devcfg_25={0xB000000,0xB002000,0xB001000,0xB004000,0x0,};


static timer_reg_data_t	devcfg_26={1,7,29,41,0x4000,{0x0B020000},};


static dbg_cfg_data_t	devcfg_27={0x004AA000,0x0B01700C,0x0B008044,0x01814000,213,18,269,206,2500,};

const DALSYSPropStructTblType DALPROP_StructPtrs_9205_tz[29] =  {
	 {sizeof(void *), &bam_tgt_config_9205_tz},
	 {sizeof(void *), &spmiInfo_9205_tz},
	 {sizeof(void *), &info_9205_tz},
	 {sizeof(void *), &NOCERR_propdata_9205_tz},
	 {sizeof(void *), &ABT_propdata_9205_tz},
	 {sizeof(void *), &BIMC_propdata_9205_tz},
	 {sizeof(void *), &icbcfg_boot_prop_9205_tz},
	 {sizeof(void *), &map_ddr_regions_9205_tz},
	 {sizeof(void *), &bimc_hal_info_9205_tz},
	 {sizeof(void *), &channel_map_9205_tz},
	 {sizeof(void *), &safe_reset_seg_9205_tz},
	 {sizeof(devcfg_11), &devcfg_11},
	 {sizeof(devcfg_12), &devcfg_12},
	 {sizeof(devcfg_13), &devcfg_13},
	 {sizeof(devcfg_14), &devcfg_14},
	 {sizeof(devcfg_15), &devcfg_15},
	 {sizeof(devcfg_16), &devcfg_16},
	 {sizeof(devcfg_17), &devcfg_17},
	 {sizeof(devcfg_18), &devcfg_18},
	 {sizeof(devcfg_19), &devcfg_19},
	 {sizeof(devcfg_20), &devcfg_20},
	 {sizeof(devcfg_21), &devcfg_21},
	 {sizeof(devcfg_22), &devcfg_22},
	 {sizeof(devcfg_23), &devcfg_23},
	 {sizeof(devcfg_24), &devcfg_24},
	 {sizeof(devcfg_25), &devcfg_25},
	 {sizeof(devcfg_26), &devcfg_26},
	 {sizeof(devcfg_27), &devcfg_27},
	{0, 0 } 
 };
const uint32 DALPROP_PropBin_9205_tz[] = {

			0x00000350, 0x00000020, 0x0000017c, 0x0000018c, 0x0000018c, 
			0x00000001, 0x00000000, 0x000001e8, 0x5f6d6162, 0x5f746774, 
			0x666e6f63, 0x6f006769, 0x72656e77, 0x6d707300, 0x65645f69, 
			0x65636976, 0x61440073, 0x766e456c, 0x72615400, 0x43746567, 
			0x47006766, 0x7443626c, 0x57487478, 0x6574754d, 0x6d754e78, 
			0x00726562, 0x72736374, 0x7361625f, 0x616e5f65, 0x7400656d, 
			0x5f727363, 0x65736162, 0x74756d00, 0x6f5f7865, 0x65736666, 
			0x77007374, 0x65636e6f, 0x66666f5f, 0x73746573, 0x666e6900, 
			0x4f4e006f, 0x72724543, 0x505f726f, 0x64706f72, 0x00617461, 
			0x5f544241, 0x706f7250, 0x61746164, 0x4d494200, 0x72724543, 
			0x505f726f, 0x64706f72, 0x00617461, 0x5f626369, 0x666e6f63, 
			0x645f6769, 0x00617461, 0x5f6d756e, 0x6e616863, 0x736c656e, 
			0x6d756e00, 0x6765735f, 0x746e656d, 0x616d0073, 0x64645f70, 
			0x65725f72, 0x6e6f6967, 0x756f635f, 0x6d00746e, 0x645f7061, 
			0x725f7264, 0x6f696765, 0x6200736e, 0x5f636d69, 0x5f6c6168, 
			0x6f666e69, 0x61686300, 0x6c656e6e, 0x70616d5f, 0x66617300, 
			0x65725f65, 0x5f746573, 0x00676573, 0x6f6c6c61, 0x72655f77, 
			0x5f657361, 0x735f6e69, 0x72756365, 0x6f6d5f65, 0x62006564, 
			0x73617079, 0x6f625f73, 0x725f746f, 0x72747365, 0x00746369, 
			0x45524f43, 0x504f545f, 0x5253435f, 0x00000000, 0x00000001, 
			0x00000000, 0x00002070, 0x00000007, 0x00005000, 0x00006000, 
			0x00007000, 0x00008000, 0x00009000, 0x0000a000, 0x0000b000, 
			0x0000c000, 0x00000001, 0x0003d000, 0x0003d004, 0x12800000, 
			0x00000000, 0xff00ff00, 0x0280000f, 0x00000001, 0x12800015, 
			0x00000001, 0xff00ff00, 0x02800022, 0x10000001, 0x14800029, 
			0x00000000, 0x02800033, 0x00000009, 0xff00ff00, 0x11800048, 
			0x00000000, 0x02800057, 0x01900000, 0x14800061, 0x0000000c, 
			0x1480006f, 0x00000030, 0xff00ff00, 0x1280007d, 0x00000002, 
			0xff00ff00, 0x12800082, 0x00000003, 0xff00ff00, 0x12800094, 
			0x00000004, 0xff00ff00, 0x128000a1, 0x00000005, 0xff00ff00, 
			0x128000b4, 0x00000006, 0x028000c4, 0x00000001, 0x028000d1, 
			0x00000003, 0x028000de, 0x00000001, 0x128000f3, 0x00000007, 
			0x12800103, 0x00000008, 0x12800111, 0x00000009, 0x1280011d, 
			0x0000000a, 0xff00ff00, 0x0280012c, 0x00000000, 0xff00ff00, 
			0x12000015, 0x0000000b, 0x12000016, 0x0000000c, 0x12000043, 
			0x0000000d, 0x12000044, 0x0000000e, 0x12000045, 0x0000000f, 
			0x12000046, 0x00000010, 0x12000023, 0x00000011, 0x12000004, 
			0x00000012, 0x12000002, 0x00000013, 0x12000021, 0x00000014, 
			0x12000022, 0x00000015, 0x12000018, 0x00000016, 0x12000027, 
			0x00000017, 0xff00ff00, 0x12000001, 0x00000018, 0xff00ff00, 
			0x12000001, 0x00000019, 0xff00ff00, 0x12000001, 0x0000001a, 
			0xff00ff00, 0x12000001, 0x0000001b, 0xff00ff00, 0x02800147, 
			0x00000000, 0xff00ff00 };



const StringDevice driver_list_9205_tz[] = {
			{"/core/hwengines/bam",1285428979u, 456, NULL, 0, NULL },
			{"DALDEVICEID_SPMI_DEVICE",3290583706u, 468, NULL, 0, NULL },
			{"/dev/core/mproc/smem",1012060316u, 516, NULL, 0, NULL },
			{"/icb/uarb",3907582491u, 552, NULL, 0, NULL },
			{"/dev/NOCError",1518077100u, 564, NULL, 0, NULL },
			{"/dev/ABTimeout",814297740u, 576, NULL, 0, NULL },
			{"/dev/BIMCError",2760514887u, 588, NULL, 0, NULL },
			{"/dev/icbcfg/boot",2382255043u, 600, NULL, 0, NULL },
			{"/storage/rpmb",2005825481u, 668, NULL, 0, NULL },
			{"/dev/vmidmt",337776915u, 680, NULL, 0, NULL },
			{"ddr",193489311u, 788, NULL, 0, NULL },
			{"int_controller",2851810835u, 800, NULL, 0, NULL },
			{"timer",275614598u, 812, NULL, 0, NULL },
			{"dbg_cfg",26643713u, 824, NULL, 0, NULL },
			{"boot_restrict",2479631688u, 836, NULL, 0, NULL }
};
