/***********************************************************************
 * ubi_media.h
 *
 * UBI Data structures.
 * Copyright (C) 2017 QUALCOMM Technologies, Inc.
 *
 * UBI Data structures stored in the nand flash.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-09-08   rp    Create

===========================================================================*/
#ifndef __UBI_MEDIA_H__
#define __UBI_MEDIA_H__

#include "comdef.h"

/* Erase counter header magic number (ASCII "UBI#") */
#define UBI_EC_HDR_MAGIC   (0x55424923)
#define UBI_EC_HDR_VERSION (1)
#define UBI_EC_HDR_MAX_ERASE_COUNTER (0x7FFFFFFF)

/* Volume header magic number (ASCII "UBI!") */
#define UBI_VID_HDR_MAGIC   (0x55424921)
#define UBI_VID_HDR_VERSION (1)

#define UBI_CRC32_INIT (0xFFFFFFFFU)

#ifndef UBI_EC_HDR_INITIAL_SEQ_NO
#define UBI_EC_HDR_INITIAL_SEQ_NO  (0)
#endif

PACKED struct ubi_ec_hdr {
	uint32  magic;
	uint8   version;
	uint8   padding1[3];
	uint64  ec; /* Warning: the current limit is 31-bit anyway! */
	uint32  vid_hdr_offset;
	uint32  data_offset;
	uint32  image_seq;
	uint8   padding2[32];
	uint32  hdr_crc;
} PACKED_POST;

#define UBI_EC_HDR_SIZE        (sizeof(struct ubi_ec_hdr))
#define UBI_EC_HDR_SIZE_NO_CRC (sizeof(struct ubi_ec_hdr) - sizeof(uint32))


#define UBI_MAX_VOLUME_TABLES   (2)
#define UBI_MAX_VOLUMES         (128)
#define UBI_VOLUME_TYPE_DYNAMIC (1)
#define UBI_INTERNAL_VOL_START  (0x7FFFFFFF - 4096)
#define UBI_LAYOUT_VOLUME_ID    (UBI_INTERNAL_VOL_START)
#define UBI_FM_SB_VOLUME_ID     (UBI_INTERNAL_VOL_START + 1)

PACKED struct ubi_vid_hdr {
	uint32  magic;
	uint8   version;
	uint8   vol_type;
	uint8   copy_flag;
	uint8   compat;
	uint32  vol_id;
	uint32  lnum;
	uint32  leb_ver;
	uint32  data_size;
	uint32  used_ebs;
	uint32  data_pad;
	uint32  data_crc;
	uint8   padding2[4];
	uint64  sqnum;
	uint8   padding3[12];
	uint32  hdr_crc;
} PACKED_POST;

#define UBI_VID_HDR_SIZE        (sizeof(struct ubi_vid_hdr))
#define UBI_VID_HDR_SIZE_NO_CRC (sizeof(struct ubi_vid_hdr) - sizeof(uint32))


/* A record in the UBI volume table. */
#define UBI_VOL_NAME_MAX 127
PACKED struct ubi_vtbl_record {
	uint32  reserved_pebs;
	uint32  alignment;
	uint32  data_pad;
	uint8   vol_type;
	uint8   upd_marker;
	uint16  name_len;
	uint8   name[UBI_VOL_NAME_MAX+1];
	uint8   flags;
	uint8   padding[23];
	uint32  crc;
} PACKED_POST;

#define UBI_VTBL_RECORD_SIZE sizeof(struct ubi_vtbl_record)
#define UBI_VTBL_RECORD_SIZE_CRC (UBI_VTBL_RECORD_SIZE - sizeof(uint32))

PACKED struct ubifs_ch {
	uint32  magic;
	uint32  crc;
	uint64  sqnum;
	uint32  len;
	uint8   node_type;
	uint8   group_type;
	uint8   padding[2];
} PACKED_POST;

PACKED struct ubifs_sb_node {
	struct  ubifs_ch ch;
	uint8   padding[2];
	uint8   key_hash;
	uint8   key_fmt;
	uint32  flags;
	uint32  min_io_size;
	uint32  leb_size;
	uint32  leb_cnt;
	uint32  max_leb_cnt;
	uint64  max_bud_bytes;
	uint32  log_lebs;
	uint32  lpt_lebs;
	uint32  orph_lebs;
	uint32  jhead_cnt;
	uint32  fanout;
	uint32  lsave_cnt;
	uint32  fmt_version;
	uint16  default_compr;
	uint8   padding1[2];
	uint32  rp_uid;
	uint32  rp_gid;
	uint64  rp_size;
	uint32  time_gran;
	uint8   uuid[16];
	uint8   padding2[3972];
} PACKED_POST;

#define UBIFS_SB_FLAG_SPACE_FIXUP (0x04)
#define UBIFS_CRC32_INIT (0xFFFFFFFFU)

enum {
	UBIFS_INO_NODE,
	UBIFS_DATA_NODE,
	UBIFS_DENT_NODE,
	UBIFS_XENT_NODE,
	UBIFS_TRUN_NODE,
	UBIFS_PAD_NODE,
	UBIFS_SB_NODE,
	UBIFS_MST_NODE,
	UBIFS_REF_NODE,
	UBIFS_IDX_NODE,
	UBIFS_CS_NODE,
	UBIFS_ORPH_NODE,
	UBIFS_NODE_TYPES_CNT,
};

#endif /* __UBI_MEDIA_H__ */
