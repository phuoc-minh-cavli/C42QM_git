#ifndef __ECM_FAME_H__
#define __ECM_FAME_H__

//assumption : little endian.
enum ecm_peripheral_id
{
	S1_PS = 0x15,
	S2_PS = 0x18,
	S3_PS = 0x1b,
	S4_PS = 0x1e,
	S5_PS = 0x21,
	LDO1 = 0x40,
	LDO2 = 0x41,
	LDO3 = 0x42,
	LDO4 = 0x43,
	LDO5 = 0x44,
	LDO6 = 0x45,
	LDO8 = 0x47,
	LDO9 = 0x48,
	MISC_QPOET = 0x09
};
enum ecm_frame_header_ts
{
	VADC_DATA_000  = 0xF0,
	VADC_DATA_VPH  = 0xF1,
	VADC_DATA_TEMP = 0xF2
};

struct ecm_frame_hdr
{
	uint8 header;       /* \see enum ecm_frame_header_ts */
	uint8 data[2];      /* contains temperature or voltage depending on header type */
	uint8 timestamp[3]; /* RTC time , resolution is 1ms */
	uint8 gpio_status;  /* GPIO6 status */
	uint8 gpc_data[2];  /* GPC data */
};

struct ecm_buck_data
{
	uint8 peripheral_id;
	uint8 ecm_status;           /*<-- */
	uint8 ecm_data[2];          /*<-- ecm data for bucks */
	uint8 buck_voltage_setting[2];
	uint8 buck_mode;
	uint8 buck_reg_status[2];
};

struct ecm_ldo_data
{
	uint8 peripheral_id;
	uint8 ecm_status;
	uint8 ecm_data[2];
	uint8 ldo_voltage_settings[2];
	uint8 ldo_mode;
	uint8 ldo_reg_status;
};

struct ecm_qpoet_data
{
	uint8 peripheral_id;
	uint8 qpoet_ecm_status;
	uint8 qpoet_ecm_data[2];
};
#endif
