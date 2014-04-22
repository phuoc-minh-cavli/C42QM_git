#include "DALPropDef.h"
#include "DALPropDef.h"
#include "DALPropDef.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "dalconfig.h"
#include "dalconfig.h"
#include "dalconfig.h"
#include "dalconfig.h"
#include "dalconfig.h"
#include "dalconfig.h"
#include "pm_dal_prop_ids.h"
#include "DALStdDef.h" 
#include "DALSysTypes.h" 
#include "dalconfig.h" 

#ifndef DAL_CONFIG_IMAGE_RPM 
#define DAL_CONFIG_IMAGE_RPM 
#endif 
extern void * icb_rpm_system;
extern void * glink_ssr_devcfg_config;
extern void * xport_rpm_devcfg_proc_info;
extern void * xport_rpm_devcfg_remote_procs;
extern void * xport_rpm_devcfg_toc;
extern void * SourceConfig;
extern void * RPMClockConfig;
extern void * PCNOClockConfig;
extern void * BIMCClockConfig;
extern void * APSSAXIClockConfig;
extern void * QDSSATClockConfig;
extern void * QDSSTraceClockConfig;
extern void * QDSSSTMClockConfig;
extern void * QDSSTSCTRClockConfig;
extern void * QDSSTRIGClockConfig;
extern void * RBCPRClockConfig;
extern void * SPMIAHBClockConfig;
extern void * SPMISERClockConfig;
extern void * APSSAHBClockConfig;
extern void * QPICClockConfig;
extern void * QPICIOClockConfig;
extern void * CRYPTOClockConfig;
extern void * ClockLogDefaultConfig;
extern void * ClockVregRailMapConfig;
extern void * clk_reg;
extern void * smps_reg;
extern void * ldo_reg;
extern void * fts_volt;
extern void * fts2p5_volt;
extern void * hfs_volt;
extern void * boost_volt;
extern void * boost_byp_volt;
extern void * nmos_volt;
extern void * n600_volt;
extern void * n1200_volt;
extern void * pmos_volt;
extern void * ln_volt;
extern void * fts_range;
extern void * fts2p5_range;
extern void * hfs_range;
extern void * boost_range;
extern void * boost_byp_range;
extern void * nmos_range;
extern void * n600_range;
extern void * n1200_range;
extern void * pmos_range;
extern void * ln_range;
extern void * ult_nmos_volt;
extern void * ult_pmos_volt;
extern void * ult_nmos_range;
extern void * ult_pmos_range;
extern void * ult_buck_range_1;
extern void * ult_buck_volt_1;
extern void * ult_buck_range_2;
extern void * ult_buck_volt_2;
extern void * ult_ldo_settling_time;
extern void * ult_smps_settling_time;
extern void * ldo_settling_time;
extern void * smps_settling_time;
extern void * vs_settling_time;
extern void * boost_settling_time;
extern void * clk_xo_reg;
extern void * clk_common;
extern void * num_of_smps;
extern void * num_of_ldo;
extern void * ldo_rail;
extern void * smps_rail;
extern void * clk_info;
extern void * smps_dep;
extern void * ldo_dep;
extern void * clk_dep;
extern void * pm_mpm_active_cfg;
extern void * pm_mpm_sleep_cfg;
extern void * pm_mpm_cmd_index;
extern void * pm_mpm_rails_info;
extern void * sleep_enter_info;
extern void * sleep_exit_info;

const void * DALPROP_StructPtrs[78] =  {
	&icb_rpm_system,
	&glink_ssr_devcfg_config,
	&xport_rpm_devcfg_proc_info,
	&xport_rpm_devcfg_remote_procs,
	&xport_rpm_devcfg_toc,
	&SourceConfig,
	&RPMClockConfig,
	&PCNOClockConfig,
	&BIMCClockConfig,
	&APSSAXIClockConfig,
	&QDSSATClockConfig,
	&QDSSTraceClockConfig,
	&QDSSSTMClockConfig,
	&QDSSTSCTRClockConfig,
	&QDSSTRIGClockConfig,
	&RBCPRClockConfig,
	&SPMIAHBClockConfig,
	&SPMISERClockConfig,
	&APSSAHBClockConfig,
	&QPICClockConfig,
	&QPICIOClockConfig,
	&CRYPTOClockConfig,
	&ClockLogDefaultConfig,
	&ClockVregRailMapConfig,
	&clk_reg,
	&smps_reg,
	&ldo_reg,
	&fts_volt,
	&fts2p5_volt,
	&hfs_volt,
	&boost_volt,
	&boost_byp_volt,
	&nmos_volt,
	&n600_volt,
	&n1200_volt,
	&pmos_volt,
	&ln_volt,
	&fts_range,
	&fts2p5_range,
	&hfs_range,
	&boost_range,
	&boost_byp_range,
	&nmos_range,
	&n600_range,
	&n1200_range,
	&pmos_range,
	&ln_range,
	&ult_nmos_volt,
	&ult_pmos_volt,
	&ult_nmos_range,
	&ult_pmos_range,
	&ult_buck_range_1,
	&ult_buck_volt_1,
	&ult_buck_range_2,
	&ult_buck_volt_2,
	&ult_ldo_settling_time,
	&ult_smps_settling_time,
	&ldo_settling_time,
	&smps_settling_time,
	&vs_settling_time,
	&boost_settling_time,
	&clk_xo_reg,
	&clk_common,
	&num_of_smps,
	&num_of_ldo,
	&ldo_rail,
	&smps_rail,
	&clk_info,
	&smps_dep,
	&ldo_dep,
	&clk_dep,
	&pm_mpm_active_cfg,
	&pm_mpm_sleep_cfg,
	&pm_mpm_cmd_index,
	&pm_mpm_rails_info,
	&sleep_enter_info,
	&sleep_exit_info,
	 NULL }; 

const uint32 DALPROP_PropBin[] = {

			0x00000690, 0x00000028, 0x00000218, 0x00000218, 0x00000218, 
			0x00000002, 0x02000145, 0x000002c0, 0x0200009b, 0x00000668, 
			0x74737973, 0x70006d65, 0x5f63696d, 0x5f627261, 0x65736162, 
			0x6464615f, 0x776f0072, 0x0072656e, 0x65746e69, 0x70757272, 
			0x6f630074, 0x6769666e, 0x6f727000, 0x6e695f63, 0x72006f66, 
			0x746f6d65, 0x72705f65, 0x0073636f, 0x00636f74, 0x5f6d696c, 
			0x636d6962, 0x6d696c00, 0x6e63705f, 0x6c00636f, 0x715f6d69, 
			0x00636970, 0x5f6d696c, 0x00757063, 0x5f6b6c63, 0x746c6f76, 
			0x726f635f, 0x6c43006e, 0x536b636f, 0x6372756f, 0x67007365, 
			0x725f6363, 0x705f6d70, 0x5f636f72, 0x6b6c6368, 0x63636700, 
			0x6e63705f, 0x615f636f, 0x635f6268, 0x67006b6c, 0x625f6363, 
			0x5f636d69, 0x006b6c63, 0x5f636367, 0x73737061, 0x6978615f, 
			0x6b6c635f, 0x63636700, 0x7364715f, 0x74615f73, 0x6b6c635f, 
			0x63636700, 0x7364715f, 0x72745f73, 0x63656361, 0x6e696b6c, 
			0x6b6c635f, 0x63636700, 0x7364715f, 0x74735f73, 0x6c635f6d, 
			0x6367006b, 0x64715f63, 0x745f7373, 0x72746373, 0x6b6c635f, 
			0x63636700, 0x7364715f, 0x72745f73, 0x635f6769, 0x67006b6c, 
			0x725f6363, 0x72706362, 0x6b6c635f, 0x63636700, 0x6d70735f, 
			0x68615f69, 0x6c635f62, 0x6367006b, 0x70735f63, 0x735f696d, 
			0x635f7265, 0x67006b6c, 0x615f6363, 0x5f737370, 0x5f626861, 
			0x006b6c63, 0x5f636367, 0x63697071, 0x6b6c635f, 0x63636700, 
			0x6970715f, 0x6f695f63, 0x63616d5f, 0x635f6f72, 0x67006b6c, 
			0x635f6363, 0x74707972, 0x6c635f6f, 0x6c43006b, 0x4c6b636f, 
			0x6544676f, 0x6c756166, 0x43007374, 0x6b636f6c, 0x67657256, 
			0x6c696152, 0x0070614d, 0x41464544, 0x5f544c55, 0x51455246, 
			0x434e4555, 0x54510059, 0x52454d49, 0x5f43415f, 0x45534142, 
			0x49545100, 0x5f52454d, 0x45534142, 0x00000000, 0x00000001, 
			0xffffffff, 0xffffffff, 0x00000001, 0xffffffff, 0xffffffff, 
			0x00000001, 0xffffffff, 0xffffffff, 0x00000001, 0xffffffff, 
			0xffffffff, 0x00000000, 0xffffffff, 0x00000012, 0x80000000, 
			0x00000000, 0xff00ff00, 0x00000002, 0x80000007, 0x02000000, 
			0x00000002, 0x8000001a, 0x00000004, 0x00000002, 0x80000020, 
			0x00000030, 0xff00ff00, 0x00000012, 0x8000002a, 0x00000001, 
			0xff00ff00, 0x00000012, 0x80000031, 0x00000002, 0x00000012, 
			0x8000003b, 0x00000003, 0x00000012, 0x80000048, 0x00000004, 
			0xff00ff00, 0x00000014, 0x8000004c, 0x00000000, 0x00000014, 
			0x80000055, 0x0000000c, 0x00000014, 0x8000005f, 0x00000018, 
			0x00000014, 0x80000068, 0x00000024, 0x00000014, 0x80000070, 
			0x00000030, 0x00000012, 0x8000007e, 0x00000005, 0x00000012, 
			0x8000008b, 0x00000006, 0x00000012, 0x8000009d, 0x00000007, 
			0x00000012, 0x800000af, 0x00000008, 0x00000012, 0x800000bc, 
			0x00000009, 0x00000012, 0x800000cd, 0x0000000a, 0x00000012, 
			0x800000dd, 0x0000000b, 0x00000012, 0x800000f5, 0x0000000c, 
			0x00000012, 0x80000106, 0x0000000d, 0x00000012, 0x80000119, 
			0x0000000e, 0x00000012, 0x8000012b, 0x0000000f, 0x00000012, 
			0x80000139, 0x00000010, 0x00000012, 0x8000014a, 0x00000011, 
			0x00000012, 0x8000015b, 0x00000012, 0x00000012, 0x8000016c, 
			0x00000013, 0x00000012, 0x80000179, 0x00000014, 0x00000012, 
			0x8000018f, 0x00000015, 0x00000012, 0x8000019e, 0x00000016, 
			0x00000012, 0x800001af, 0x00000017, 0xff00ff00, 0x00000012, 
			0x00000001, 0x00000018, 0x00000012, 0x00000002, 0x00000019, 
			0x00000012, 0x00000003, 0x0000001a, 0x00000012, 0x00000006, 
			0x0000001b, 0x00000012, 0x0000001e, 0x0000001c, 0x00000012, 
			0x00000007, 0x0000001d, 0x00000012, 0x00000008, 0x0000001e, 
			0x00000012, 0x0000002a, 0x0000001f, 0x00000012, 0x00000009, 
			0x00000020, 0x00000012, 0x0000000a, 0x00000021, 0x00000012, 
			0x0000000b, 0x00000022, 0x00000012, 0x0000000c, 0x00000023, 
			0x00000012, 0x0000000d, 0x00000024, 0x00000012, 0x0000000e, 
			0x00000025, 0x00000012, 0x0000001f, 0x00000026, 0x00000012, 
			0x0000000f, 0x00000027, 0x00000012, 0x00000010, 0x00000028, 
			0x00000012, 0x00000028, 0x00000029, 0x00000012, 0x00000011, 
			0x0000002a, 0x00000012, 0x00000012, 0x0000002b, 0x00000012, 
			0x00000013, 0x0000002c, 0x00000012, 0x00000014, 0x0000002d, 
			0x00000012, 0x00000015, 0x0000002e, 0x00000012, 0x00000027, 
			0x0000002f, 0x00000012, 0x00000025, 0x00000030, 0x00000012, 
			0x00000026, 0x00000031, 0x00000012, 0x00000024, 0x00000032, 
			0x00000012, 0x00000020, 0x00000033, 0x00000012, 0x00000021, 
			0x00000034, 0x00000012, 0x00000022, 0x00000035, 0x00000012, 
			0x00000023, 0x00000036, 0x00000012, 0x0000002b, 0x00000037, 
			0x00000012, 0x0000002c, 0x00000038, 0x00000012, 0x00000016, 
			0x00000039, 0x00000012, 0x00000017, 0x0000003a, 0x00000012, 
			0x00000018, 0x0000003b, 0x00000012, 0x00000019, 0x0000003c, 
			0x00000012, 0x0000001b, 0x0000003d, 0x00000012, 0x0000001c, 
			0x0000003e, 0xff00ff00, 0x00000012, 0x00000065, 0x0000003f, 
			0x00000012, 0x00000067, 0x00000040, 0x00000012, 0x00000086, 
			0x00000041, 0x00000012, 0x00000087, 0x00000042, 0x00000012, 
			0x00000074, 0x00000043, 0x00000012, 0x0000006e, 0x00000044, 
			0x00000012, 0x0000006d, 0x00000045, 0x00000012, 0x00000071, 
			0x00000046, 0x00000012, 0x0000012d, 0x00000047, 0x00000012, 
			0x0000012e, 0x00000048, 0x00000012, 0x0000012f, 0x00000049, 
			0x00000012, 0x00000130, 0x0000004a, 0x00000012, 0x00000075, 
			0x0000004b, 0x00000012, 0x00000076, 0x0000004c, 0xff00ff00, 
			0x00000002, 0x800001c0, 0x0124f800, 0x00000002, 0x800001d2, 
			0x00082000, 0x00000002, 0x800001e1, 0x00084000, 0xff00ff00 };



 const StringDevice driver_list[] = {
			{"/dev/icb/rpm",2545934574u, 592, NULL, 0, NULL },
			{"DALDEVICEID_SPMI_DEVICE",3290583706u, 608, NULL, 0, NULL },
			{"/dev/core/mproc/glink/ssr",944539910u, 648, NULL, 0, NULL },
			{"/dev/core/mproc/glink/xport_rpm",2797335449u, 664, NULL, 0, NULL },
			{"/rpm/pmic/common",2887753651u, 996, NULL, 0, NULL },
			{"/rpm/pmic/target",3536625265u, 1468, NULL, 0, NULL }
};


 const DALProps DALPROP_PropsInfo = {(const byte*)DALPROP_PropBin, DALPROP_StructPtrs , 6, driver_list};
