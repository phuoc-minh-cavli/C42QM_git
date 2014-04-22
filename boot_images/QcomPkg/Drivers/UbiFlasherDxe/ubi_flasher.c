/***********************************************************************
 * ubi_flasher.c
 *
 * UBI Flasher module.
 * Copyright (C) 2017 QUALCOMM Technologies, Inc.
 *
 * Flashes an UBI or an UBIFS images onto nand flash.
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

#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include "ubi_flasher.h"
#include "ubi_nand.h"
#include "ubi_util.h"
#include "ubi_crc.h"
#include "ubi_media.h"

#include <string.h>


#ifndef UBI_FLASHER_MAX_HANDLES
#define UBI_FLASHER_MAX_HANDLES  (2)
#endif

PACKED struct ubi_block_info_type {
	unsigned int is_bad_block : 1;
	unsigned int is_erased_block : 1;
	unsigned int is_ec_hdr_valid : 1;
	unsigned int is_vid_hdr_valid : 1;
	uint32 block_no;
	uint64 ec;
	uint32 vol_id;
}PACKED_POST;

struct ubi_flasher_handle_type {

	int is_in_use;

	const char *partition_name;
	int is_it_volume;
	uint32 vol_id;

	uint32 frame_no;
	uint8 *buf;
	uint32 buf_size;

	int last_frame_rcvd;
	uint32 prev_write_block_no;
	uint32 cur_write_block_no;
	uint32 image_block_count;

	ubi_nand_handle nand_hdl;

	uint32 page_size;
	uint32 block_size;
	uint32 block_size_bytes;
	uint32 block_count;

	uint8 *block_buf;

	uint32 bad_block_count;
	uint32 used_block_count; /* blocks with vid-hdr */
	uint32 free_block_count; /* both erased and non-vid-hdr blocks */

	uint64 ec_sum;
	uint64 mean_ec;

	struct ubi_block_info_type *block_info;

	uint32 vtbl_block_nos[UBI_MAX_VOLUME_TABLES];
	int vtbl_count;

	uint32 vid_hdr_offset;
	uint32 data_offset;
	uint32 image_seq;
	uint32 incoming_img_seq;

	uint32 leb_size;
	uint32 leb_start_page; /* LEB start-page inside one block */
	uint32 leb_block_size; /* Number of LEB pages in one block */

	uint32 leb_no;
};
static struct ubi_flasher_handle_type ubi_flasher_handles[UBI_FLASHER_MAX_HANDLES];

static struct ubi_flasher_handle_type* ubi_flasher_handle_alloc(void)
{
	uint32 i;
	struct ubi_flasher_handle_type *hdl;

	for (i = 0; i < UBI_FLASHER_MAX_HANDLES; ++i) {
		hdl = &ubi_flasher_handles[i];
		if (!hdl->is_in_use) {
			ubi_util_memset(hdl, 0, sizeof(*hdl));
			hdl->is_in_use = 1;
			return hdl;
		}
	}

	return NULL;
}

static int ubi_flasher_handle_free(struct ubi_flasher_handle_type *hdl)
{
	ASSERT(hdl != NULL);
	ASSERT(hdl->is_in_use = 1);

	hdl->is_in_use = 0;
	return 0;
}

static int ubi_open_partition(struct ubi_flasher_handle_type *hdl,
	const char *partition_name)
{
	int result;

	ASSERT(hdl != NULL);
	ASSERT(partition_name != NULL);

	result = ubi_nand_open_partition(partition_name, &hdl->nand_hdl);
	if (result != 0) {
		DEBUG((EFI_D_ERROR, "[%d] partition open failed\n", result));
		return result;
	}

	result = ubi_nand_get_page_size(hdl->nand_hdl, &hdl->page_size);
	if (result != 0) {
		DEBUG((EFI_D_ERROR, "[%d] get page-size failed\n", result));
		return result;
	}
	ASSERT(hdl->page_size > 0);

	result = ubi_nand_get_block_size(hdl->nand_hdl, &hdl->block_size);
	if (result != 0) {
		DEBUG((EFI_D_ERROR, "[%d] get block-size failed\n", result));
		return result;
	}
	ASSERT(hdl->block_size > 0);

	hdl->block_size_bytes = hdl->block_size * hdl->page_size;

	result = ubi_nand_get_block_count(hdl->nand_hdl, &hdl->block_count);
	if (result != 0) {
		DEBUG((EFI_D_ERROR, "[%d] get block-count failed\n", result));
		return result;
	}
	ASSERT(hdl->block_count > 0);

	return 0;
}

static int ubi_open_volume(struct ubi_flasher_handle_type *hdl,
	const char *volume_name)
{
	int result;

	ASSERT(hdl != NULL);
	ASSERT(volume_name != NULL);

	result = ubi_open_partition(hdl, UBI_FLASHER_SYSTEM_PARTITION_NAME);
	if (result != 0)
		return result;

	hdl->is_it_volume = 1;

	return 0;
}


static int ubi_close_partition(struct ubi_flasher_handle_type *hdl)
{
	int result;

	ASSERT(hdl != NULL);

	result = ubi_nand_close_partition(hdl->nand_hdl);
	if (result != 0) {
		DEBUG((EFI_D_ERROR, "[%d] partition close failed\n", result));
		return result;
	}

	return 0;
}

static int ubi_do_mallocs(struct ubi_flasher_handle_type *hdl)
{
	uint32 size;

	ASSERT(hdl != NULL);

	hdl->block_buf = ubi_util_malloc(hdl->block_size_bytes);
	if (hdl->block_buf == NULL) {
		DEBUG((EFI_D_ERROR, "[%d pg-malloc failed\n", hdl->page_size));
		return -1;
	}

	size = hdl->block_count * sizeof(struct ubi_block_info_type);
	hdl->block_info = ubi_util_malloc(size);
	if (hdl->block_info == NULL) {
		DEBUG((EFI_D_ERROR, "[%d pebs-malloc failed\n", size));
		return -1;
	}

	return 0;
}

static int ubi_do_frees(struct ubi_flasher_handle_type *hdl)
{
	uint32 size;

	ASSERT(hdl != NULL);

	ubi_util_free(hdl->block_buf, hdl->block_size_bytes);
	hdl->block_buf = NULL;

	size = hdl->block_count * sizeof(struct ubi_block_info_type);
	ubi_util_free(hdl->block_info, size);
	hdl->block_info = NULL;

	return 0;
}

static int ubi_is_ec_hdr_valid(struct ubi_flasher_handle_type *hdl,
				struct ubi_ec_hdr *ec_hdr,
				int is_verbose)
{
	uint32 be32, ec_hdr_crc;
	uint64 be64;

	ASSERT(hdl != NULL);
	ASSERT(ec_hdr != NULL);

	be32 = ubi_util_le_to_be32(ec_hdr->magic);
	if (be32 != UBI_EC_HDR_MAGIC) {
		if (is_verbose) {
			DEBUG((EFI_D_ERROR,
			"[%x] UBI invalid ec-hdr magic\n", be32));
		}
		return 0;
	}

	if (ec_hdr->version != UBI_EC_HDR_VERSION) {
		if (is_verbose) {
			DEBUG((EFI_D_ERROR,
			"[%x] UBI invalid ec-hdr version\n", ec_hdr->version));
		}
		return 0;
	}

	be64 = ubi_util_le_to_be64(ec_hdr->ec);
	if (be64 > UBI_EC_HDR_MAX_ERASE_COUNTER) {
		if (is_verbose) {
			DEBUG((EFI_D_ERROR,
			"[%lx] UBI invalid ec-hdr ec\n", be64));
		}
		return 0;
	}

	ec_hdr_crc = ubi_crc32(UBI_CRC32_INIT, (const uint8 *)ec_hdr,
				UBI_EC_HDR_SIZE_NO_CRC);
	be32 = ubi_util_le_to_be32(ec_hdr_crc);
	if (be32 != ec_hdr->hdr_crc) {
		if (is_verbose) {
			DEBUG((EFI_D_ERROR,
			"[%x vs %x] UBI invalid ec-hdr crc\n",
			be32, ec_hdr->hdr_crc));
		}
		return 0;
	}

	return 1;
}

static int ubi_is_vid_hdr_valid(struct ubi_flasher_handle_type *hdl,
	struct ubi_vid_hdr *vid_hdr, int is_verbose)
{
	uint32 be32, vid_hdr_crc;

	ASSERT(hdl != NULL);
	ASSERT(vid_hdr != NULL);

	be32 = ubi_util_le_to_be32(vid_hdr->magic);
	if (be32 != UBI_VID_HDR_MAGIC) {
		if (is_verbose) {
			DEBUG((EFI_D_ERROR,
			"[%x] UBI invalid vid-hdr magic\n", be32));
		}
		return 0;
	}

	if (vid_hdr->version != UBI_VID_HDR_VERSION) {
		if (is_verbose) {
			DEBUG((EFI_D_ERROR,
			"[%x] UBI invalid vid-hdr version\n",
			vid_hdr->version));
		}
		return 0;
	}

	vid_hdr_crc = ubi_crc32(UBI_CRC32_INIT, (const uint8 *)vid_hdr,
				UBI_VID_HDR_SIZE_NO_CRC);

	be32 = ubi_util_le_to_be32(vid_hdr_crc);
	if (be32 != vid_hdr->hdr_crc) {
		if (is_verbose) {
			DEBUG((EFI_D_ERROR,
			"[%x vs %x] UBI invalid vid-hdr crc\n",
			be32, vid_hdr->hdr_crc));
		}
		return 0;
	}

	return 1;
}

static int ubi_is_vtbl_rec_valid(struct ubi_flasher_handle_type *hdl,
	struct ubi_vtbl_record *vtbl_rec)
{
	uint32 be32, vtbl_rec_crc;

	ASSERT(hdl != NULL);
	ASSERT(vtbl_rec != NULL);

	vtbl_rec_crc = ubi_crc32(UBI_CRC32_INIT, (const uint8 *)vtbl_rec,
				UBI_VTBL_RECORD_SIZE_CRC);

	be32 = ubi_util_le_to_be32(vtbl_rec_crc);
	if (be32 != vtbl_rec->crc) {
		DEBUG((EFI_D_ERROR, "[%x vs %x] UBI invalid vtbl-rec crc\n",
			be32, vtbl_rec->crc));
		return 0;
	}

	return 1;
}


static int ubi_update_ec_hdr(struct ubi_flasher_handle_type *hdl,
		struct ubi_ec_hdr *ec_hdr, uint64 ec, uint32 img_seq_no)
{
	uint32 ec_hdr_crc, be32;

	ASSERT(hdl != NULL);
	ASSERT(ec_hdr != NULL);

	ec_hdr->ec = ubi_util_le_to_be64(ec);
	ec_hdr->image_seq = ubi_util_le_to_be32(img_seq_no);

	ec_hdr_crc = ubi_crc32(UBI_CRC32_INIT, (const uint8 *)ec_hdr,
				UBI_EC_HDR_SIZE_NO_CRC);
	be32 = ubi_util_le_to_be32(ec_hdr_crc);
	ec_hdr->hdr_crc = be32;

	return 0;
}

static int ubi_create_ec_hdr(struct ubi_flasher_handle_type *hdl,
		struct ubi_ec_hdr *ec_hdr, uint64 ec)
{
	uint32 ec_hdr_crc, be32;

	ASSERT(hdl != NULL);
	ASSERT(ec_hdr != NULL);

	ubi_util_memset(ec_hdr, 0xFF, sizeof(*ec_hdr));

	ec_hdr->magic = ubi_util_le_to_be32(UBI_EC_HDR_MAGIC);
	ec_hdr->version = UBI_EC_HDR_VERSION;
	ec_hdr->ec = ubi_util_le_to_be64(ec);
	ec_hdr->vid_hdr_offset = ubi_util_le_to_be32(hdl->vid_hdr_offset);
	ec_hdr->data_offset = ubi_util_le_to_be32(hdl->data_offset);
	ec_hdr->image_seq = ubi_util_le_to_be32(hdl->image_seq);

	ec_hdr_crc = ubi_crc32(UBI_CRC32_INIT, (const uint8 *)ec_hdr,
				UBI_EC_HDR_SIZE_NO_CRC);
	be32 = ubi_util_le_to_be32(ec_hdr_crc);
	ec_hdr->hdr_crc = be32;

	return 0;
}

static int ubi_create_vid_hdr(struct ubi_flasher_handle_type *hdl,
		struct ubi_vid_hdr *vid_hdr, uint32 leb_no)
{
	uint32 vid_hdr_crc, be32;

	ASSERT(hdl != NULL);
	ASSERT(vid_hdr != NULL);

	ubi_util_memset(vid_hdr, 0, sizeof(*vid_hdr));

	vid_hdr->magic = ubi_util_le_to_be32(UBI_VID_HDR_MAGIC);
	vid_hdr->version = UBI_VID_HDR_VERSION;
	vid_hdr->vol_type = UBI_VOLUME_TYPE_DYNAMIC;
	vid_hdr->vol_id = ubi_util_le_to_be32(hdl->vol_id);
	vid_hdr->lnum = ubi_util_le_to_be32(leb_no);
	vid_hdr->sqnum = ubi_util_le_to_be32(hdl->image_seq);


	vid_hdr_crc = ubi_crc32(UBI_CRC32_INIT, (const uint8 *)vid_hdr,
				UBI_VID_HDR_SIZE_NO_CRC);
	be32 = ubi_util_le_to_be32(vid_hdr_crc);
	vid_hdr->hdr_crc = be32;

	return 0;
}



static int ubi_scan_block(struct ubi_flasher_handle_type *hdl,
	uint32 block_no, struct ubi_block_info_type *bi)
{
	uint8 *buf;
	struct ubi_vid_hdr *vid_hdr;
	uint32 page_no, vol_id;
	int result;

	ASSERT(hdl != NULL);
	ASSERT(bi != NULL);

	ASSERT(block_no < hdl->block_count);

	buf = hdl->block_buf + hdl->vid_hdr_offset;

	page_no = block_no * hdl->block_size;
	page_no += (hdl->vid_hdr_offset / hdl->page_size);

	result = ubi_nand_read_pages(hdl->nand_hdl, page_no,
			1, buf, hdl->page_size);
	if (result == NAND_DEVICE_FAIL_PAGE_ERASED) {
		++hdl->free_block_count;
		return 0;
	}
	if (result != 0) {
		DEBUG((EFI_D_ERROR, "[%d,%d] read-page failed\n",
			result, page_no));
		++hdl->free_block_count;
		return 0;
	}

	vid_hdr = (struct ubi_vid_hdr *)(buf);
	result = ubi_is_vid_hdr_valid(hdl, vid_hdr, 0);
	if (result != 1) {
		++hdl->free_block_count;
		return 0;
	}

	++hdl->used_block_count;

	vol_id = ubi_util_le_to_be32(vid_hdr->vol_id);
	bi->vol_id = vol_id;
	bi->is_vid_hdr_valid = 1;

	if (vol_id == UBI_LAYOUT_VOLUME_ID) {
		DEBUG((EFI_D_ERROR, "[%d] Found VTBL in the UBI-image\n",
			block_no));
		hdl->vtbl_block_nos[hdl->vtbl_count++] = block_no;
	}

	return 0;
}

static int ubi_sanity_check_image_in_flash(struct ubi_flasher_handle_type *hdl,
	struct ubi_ec_hdr *ec_hdr, struct ubi_block_info_type *bi)
{
	uint32 vid_offset, data_offset, image_seq;

	ASSERT(hdl != NULL);
	ASSERT(ec_hdr != NULL);
	ASSERT(bi != NULL);


	bi->ec = ubi_util_le_to_be64(ec_hdr->ec);
	vid_offset = ubi_util_le_to_be32(ec_hdr->vid_hdr_offset);
	data_offset = ubi_util_le_to_be32(ec_hdr->data_offset);
	image_seq = ubi_util_le_to_be32(ec_hdr->image_seq);

	if (hdl->vid_hdr_offset == 0) {
		if (vid_offset != hdl->page_size) {
			DEBUG((EFI_D_ERROR,
			"[%d,%d] Bad image in flash. page-size mismatch\n",
			vid_offset, hdl->page_size));
			return -1;
		}

		hdl->vid_hdr_offset = vid_offset;
		hdl->data_offset = data_offset;
		hdl->image_seq = image_seq;
		hdl->leb_size = (hdl->block_size_bytes - data_offset);
		hdl->leb_start_page = (data_offset / hdl->page_size);
		hdl->leb_block_size = (hdl->leb_size / hdl->page_size);
		ASSERT(hdl->vid_hdr_offset <= hdl->page_size);
		ASSERT(hdl->page_size >= UBI_VID_HDR_SIZE);
	} else {
		if (vid_offset != hdl->vid_hdr_offset) {
			DEBUG((EFI_D_ERROR,
			"[%d,%d] Bad image in flash. page-size mismatch\n",
			vid_offset, hdl->vid_hdr_offset));
			return -1;
		}
		if (data_offset != hdl->data_offset) {
			DEBUG((EFI_D_ERROR,
			"[%d,%d] Bad image in flash. data-offset mismatch\n",
			data_offset, hdl->data_offset));
			return -1;
		}
		if (image_seq != hdl->image_seq) {
			DEBUG((EFI_D_ERROR,
			"Bad image in flash. [%d,%d]image-seq mismatch\n",
			image_seq, hdl->image_seq));
			return -1;
		}
	}

	return 0;
}

static int ubi_scan_partition(struct ubi_flasher_handle_type *hdl)
{
	struct ubi_block_info_type *bi;
	struct ubi_ec_hdr *ec_hdr;
	uint32 block_no, page_no, block_count;
	int result;

	ASSERT(hdl != NULL);
	ASSERT(hdl->block_info != NULL);

	hdl->image_seq = UBI_EC_HDR_INITIAL_SEQ_NO;
	hdl->vid_hdr_offset = 0;

	for (block_no = 0; block_no < hdl->block_count; ++block_no) {

		bi = &hdl->block_info[block_no];
		ubi_util_memset(bi, 0, sizeof(*bi));

		bi->block_no = block_no;

		result = ubi_nand_is_block_bad(hdl->nand_hdl, block_no);
		if (result == 1) {
			++hdl->bad_block_count;
			bi->is_bad_block = 1;
			continue;
		}

		page_no = block_no * hdl->block_size;
		result = ubi_nand_read_pages(hdl->nand_hdl, page_no,
				1, hdl->block_buf, hdl->page_size);
		if (result == NAND_DEVICE_FAIL_PAGE_ERASED) {
			++hdl->free_block_count;
			bi->is_erased_block = 1;
			continue;
		}
		if (result != 0) {
			DEBUG((EFI_D_ERROR, "[%d,%d] read-block failed\n",
				result, page_no));
			++hdl->free_block_count;
			continue;
		}

		ec_hdr = (struct ubi_ec_hdr *)hdl->block_buf;
		result = ubi_is_ec_hdr_valid(hdl, ec_hdr, 0);
		if (result != 1) {
			++hdl->free_block_count;
			continue;
		}

		result = ubi_sanity_check_image_in_flash(hdl, ec_hdr, bi);
		if (result != 0) {
			++hdl->free_block_count;
			continue;
		}

		bi->is_ec_hdr_valid = 1;

		result = ubi_scan_block(hdl, block_no, bi);
		if (result != 0)
			return result;

	}

	block_count = (hdl->bad_block_count + hdl->used_block_count +
			hdl->free_block_count);
	if (block_count != hdl->block_count) {
		DEBUG((EFI_D_ERROR,
			"[%d vs %d] block-count not tally\n",
			block_count, hdl->block_count));
		return -1;
	}

	if (hdl->used_block_count == 0)
		goto End;

	/* Compute the mean-erase-counter */
	for (block_no = 0; block_no < hdl->block_count; ++block_no) {
		bi = &hdl->block_info[block_no];
		if (bi->is_ec_hdr_valid) {
			hdl->ec_sum += bi->ec;
		}
	}

	hdl->mean_ec = (hdl->ec_sum / hdl->used_block_count);
End:
	return 0;

}

static int ubi_sanity_check_incoming_image(struct ubi_flasher_handle_type *hdl,
	uint8 *buf, uint32 buf_size, int is_first_frame)
{
	struct ubi_ec_hdr *ec_hdr1, *ec_hdr2;
	uint32 page_size, offset, vid_offset, data_offset, seq_no;
	int result;

	ASSERT(hdl != NULL);
	ASSERT(buf != NULL);
	ASSERT(buf_size >= UBI_EC_HDR_SIZE);

	ec_hdr1 = (struct ubi_ec_hdr *)buf;
	result = ubi_is_ec_hdr_valid(hdl, ec_hdr1, 1);
	if (result != 1) {
		DEBUG((EFI_D_ERROR,
			"%d, Supplied file is not an UBI image\n", result));
		return -1;
	}

	vid_offset = ubi_util_le_to_be32(ec_hdr1->vid_hdr_offset);
	data_offset = ubi_util_le_to_be32(ec_hdr1->data_offset);
	seq_no = ubi_util_le_to_be32(ec_hdr1->image_seq);

	page_size = vid_offset;
	if (page_size != hdl->page_size) {
		DEBUG((EFI_D_ERROR,
			"[%d vs %d] page-size mismatch\n",
			page_size, hdl->page_size));
		return -1;
	}

	if (buf_size > hdl->block_size_bytes)
		buf_size = hdl->block_size_bytes;

	for (offset = hdl->page_size; offset < buf_size;
		offset += hdl->page_size) {

		ec_hdr2 = (struct ubi_ec_hdr *)&buf[offset];
		result = ubi_is_ec_hdr_valid(hdl, ec_hdr2, 0);
		if (result == 1) {
			DEBUG((EFI_D_ERROR, "[%d,%d] block-size mismatch\n",
				offset, hdl->block_size_bytes));
			return -1;
		}
	}

	if (is_first_frame) {
 		hdl->vid_hdr_offset = vid_offset;
		hdl->data_offset = data_offset;
		hdl->incoming_img_seq = seq_no;
		hdl->leb_size = hdl->block_size_bytes - hdl->data_offset;
		hdl->leb_start_page = (hdl->data_offset / hdl->page_size);
		hdl->leb_block_size = (hdl->leb_size / hdl->page_size);
	} else {
		if ((vid_offset != hdl->vid_hdr_offset) ||
		    (data_offset != hdl->data_offset) ||
		    (seq_no != hdl->incoming_img_seq)) {
			DEBUG((EFI_D_ERROR,
				"[%d, %d, %d] UBI-headers mismatch\n",
				ec_hdr1->vid_hdr_offset, ec_hdr1->data_offset,
				ec_hdr1->image_seq));
			return -1;
		}
	}

	return 0;
}

static int ubi_block_erase(struct ubi_flasher_handle_type *hdl,
	uint32 block_no, int *is_bad_block)
{
	struct ubi_block_info_type *bi;
	int result;

	ASSERT(hdl != NULL);
	ASSERT(block_no >= 0 && block_no < hdl->block_count);
	ASSERT(is_bad_block != NULL);

	bi = &hdl->block_info[block_no];
	ASSERT(bi->block_no ==  block_no);

	/* This function should  be called only for good-blocks */
	ASSERT(bi->is_bad_block == 0);

	*is_bad_block = 0;

	if (bi->is_vid_hdr_valid) {
		bi->is_vid_hdr_valid = 0;
		ASSERT(hdl->used_block_count > 0);
		--hdl->used_block_count;
		++hdl->free_block_count;
	}

	result = ubi_nand_erase_blocks(hdl->nand_hdl, block_no, 1);
	if (result == 0) {
		bi->is_erased_block = 1;
		return 0;
	}

	DEBUG((EFI_D_ERROR, "[%d, %d] erase-block failed\n",
		block_no, result));

	*is_bad_block = 1;
	bi->is_bad_block = 1;
	ASSERT(hdl->free_block_count > 0);
	--hdl->free_block_count;
	++hdl->bad_block_count;

	result = ubi_nand_mark_block_bad(hdl->nand_hdl, block_no);
	if (result != 0) {
		DEBUG((EFI_D_ERROR, "[%d, %d] mark bad-block failed\n",
			block_no, result));
	}
	return result;
}

static int ubi_block_write(struct ubi_flasher_handle_type *hdl,
	uint8 *buf, uint32 buf_size, uint32 block_no,
	int is_writing_only_ec_hdr, int *is_bad_block)
{
	struct ubi_block_info_type *bi;
	uint32 page_no, num_pages;
	int result;

	ASSERT(hdl != NULL);
	ASSERT(buf != NULL);
	ASSERT(buf_size <= hdl->block_size_bytes);
	ASSERT(block_no >= 0 && block_no < hdl->block_count);
	ASSERT(is_bad_block != NULL);

	bi = &hdl->block_info[block_no];
	ASSERT(bi->block_no ==  block_no);

	/* This function should  be called only for good-blocks */
	ASSERT(bi->is_bad_block == 0);

	*is_bad_block = 0;
	result = ubi_block_erase(hdl, block_no, is_bad_block);
	if (result != 0) {
		ASSERT(*is_bad_block == 1);
		ASSERT(bi->is_bad_block == 1);
		return result;
	}
	ASSERT(bi->is_bad_block == *is_bad_block);
	if (bi->is_bad_block)
		return 0;

	ASSERT(bi->is_bad_block == 0);
	*is_bad_block = 0;

	page_no = block_no * hdl->block_size;

	if (is_writing_only_ec_hdr) {
		ASSERT(buf_size == hdl->page_size);
		num_pages = 1;
	} else {
		num_pages = ubi_util_calc_data_len(buf, buf_size,
				hdl->page_size);
		ASSERT(num_pages >= 1);
	}

	result = ubi_nand_write_pages(hdl->nand_hdl, page_no,
			num_pages, buf, buf_size);
	if (result == 0) {
		if (!is_writing_only_ec_hdr) {
			ASSERT(hdl->free_block_count > 0);
			--hdl->free_block_count;
			++hdl->used_block_count;
			ASSERT(hdl->used_block_count < hdl->block_count);
		}
		return result;
	}

	(void) ubi_nand_erase_blocks(hdl->nand_hdl, block_no, 1);
	result = ubi_nand_mark_block_bad(hdl->nand_hdl, block_no);
	if (result != 0) {
		DEBUG((EFI_D_ERROR, "[%d, %d] mark bad-block failed\n",
			block_no, result));
	}

	*is_bad_block = 1;
	bi->is_bad_block = 1;

	if (!is_writing_only_ec_hdr) {
		ASSERT(hdl->free_block_count > 0);
		--hdl->free_block_count;
		++hdl->bad_block_count;
	}

	return result;
}

static int ubi_block_erase_helper(struct ubi_flasher_handle_type *hdl,
	uint32 block_no, int *is_bad_block)
{
	struct ubi_block_info_type *bi;
	struct ubi_ec_hdr *ec_hdr;
	uint64 ec;
	int result;

	bi = &hdl->block_info[block_no];
	ASSERT(bi->block_no == block_no);

	if (bi->is_ec_hdr_valid)
		ec = bi->ec + 1;
	else
		ec = hdl->mean_ec;

	ubi_util_memset(hdl->block_buf, 0xFF, hdl->page_size);
	ec_hdr = (struct ubi_ec_hdr *)&hdl->block_buf[0];
	result = ubi_create_ec_hdr(hdl, ec_hdr, ec);
	if (result != 0)
		return result;

	result = ubi_block_write(hdl, &hdl->block_buf[0], hdl->page_size,
			block_no, 1, is_bad_block);
	return result;
}

static int ubi_erase_partition(struct ubi_flasher_handle_type *hdl)
{
	struct ubi_block_info_type *bi;
	uint32 block_no;
	int result, is_bad_block;

	ASSERT(hdl != NULL);

	for (block_no = hdl->cur_write_block_no;
		block_no < hdl->block_count;
		++block_no) {

		bi = &hdl->block_info[block_no];
		ASSERT(bi->block_no == block_no);

		if (bi->is_bad_block)
			continue;

		is_bad_block = 0;
		result = ubi_block_erase_helper(hdl, block_no, &is_bad_block);
		if (result != 0)
			return result;
		(void) is_bad_block;
	}

	return 0;
}

static int ubi_erase_volume(struct ubi_flasher_handle_type *hdl)
{
	struct ubi_block_info_type *bi;
	uint32 block_no, block_count;
	int result, is_bad_block;

	ASSERT(hdl != NULL);

	ASSERT(hdl->vol_id < UBI_MAX_VOLUMES);

	block_count = 0;

	for (block_no = 0; block_no < hdl->block_count; ++block_no) {
		bi = &hdl->block_info[block_no];
		ASSERT(bi->block_no == block_no);

		if (bi->is_bad_block)
			continue;

		if (!bi->is_vid_hdr_valid)
			continue;

		if (bi->vol_id != hdl->vol_id)
			continue;

		++block_count;

		ASSERT(bi->is_ec_hdr_valid == 1);

		is_bad_block = 0;
		result = ubi_block_erase_helper(hdl, block_no, &is_bad_block);
		if (result != 0)
			return result;
		(void) is_bad_block;
	}

	DEBUG((EFI_D_ERROR, "[%d/%d], Erased volume blocks\n",
		block_count, hdl->block_count));

	return 0;
}

static void ubi_remove_F_flag(struct ubi_flasher_handle_type *hdl,
	struct ubi_ec_hdr *ec_hdr)
{
	struct ubi_vid_hdr *vid_hdr;
	struct ubifs_sb_node *ubifs_sb;
	struct ubifs_ch *ubifs_ch;
	uint8 *block_buf;
	uint32 vol_id, vid_hdr_offset, data_offset;
	int result;

	ASSERT(hdl != NULL);
	ASSERT(ec_hdr != NULL);

	vid_hdr_offset = ubi_util_le_to_be32(ec_hdr->vid_hdr_offset);
	data_offset = ubi_util_le_to_be32(ec_hdr->data_offset);

	block_buf = (uint8 *)ec_hdr;
	vid_hdr = (struct ubi_vid_hdr *)(block_buf + vid_hdr_offset);

	result = ubi_is_vid_hdr_valid(hdl, vid_hdr, 0);
	if (result != 1)
		return;


	vol_id = ubi_util_le_to_be32(vid_hdr->vol_id);
	if (vol_id > UBI_MAX_VOLUMES &&
		vol_id != UBI_LAYOUT_VOLUME_ID &&
		vol_id != UBI_FM_SB_VOLUME_ID)
		return;

	ubifs_sb = (struct ubifs_sb_node *)(block_buf + data_offset);
	ubifs_ch = (struct ubifs_ch *)ubifs_sb;

	if (ubifs_ch->node_type != UBIFS_SB_NODE)
		return;

	if (ubifs_sb->flags & UBIFS_SB_FLAG_SPACE_FIXUP) {
		ubifs_sb->flags &= (~UBIFS_SB_FLAG_SPACE_FIXUP);
		ubifs_ch->crc = ubi_crc32(UBIFS_CRC32_INIT,
			(void *)ubifs_sb + 8, sizeof(struct ubifs_sb_node) - 8);
		DEBUG((EFI_D_ERROR,
		"[%d, %d]Remove -F flash from UBIFS\n", vol_id,
		hdl->cur_write_block_no));
	}
}

static int ubi_find_volume(struct ubi_flasher_handle_type *hdl)
{
	struct ubi_block_info_type *bi;
	struct ubi_ec_hdr *ec_hdr;
	struct ubi_vid_hdr *vid_hdr;
	struct ubi_vtbl_record *vtbl_rec;
	uint32 i, block_no, page_no, vtbl_records, be32, len1, len2;
	uint8 *block_buf;
	int result;

	ASSERT(hdl != NULL);

	if (hdl->vtbl_count < 1) {
		DEBUG((EFI_D_ERROR,
		"[%d] Missing VTBL in UBI-image\n", hdl->vtbl_count));
		return -1;
	}

	len1 = strlen(hdl->partition_name);

	for (i = 0; i < hdl->vtbl_count; ++i) {

		block_no = hdl->vtbl_block_nos[i];
		ASSERT(block_no < hdl->block_count);

		page_no = block_no * hdl->block_size;
		result = ubi_nand_read_pages(hdl->nand_hdl, page_no,
				hdl->block_size, hdl->block_buf,
				hdl->block_size_bytes);
		if (result == NAND_DEVICE_FAIL_PAGE_ERASED)
			continue;
		if (result != 0) {
			DEBUG((EFI_D_ERROR,
			"[%d] find-vol : Reading nand-page failed\n", page_no));
			return -1;
		}

		ec_hdr = (struct ubi_ec_hdr *)(hdl->block_buf);
		result = ubi_is_ec_hdr_valid(hdl, ec_hdr, 1);
		if (result != 1) {
			DEBUG((EFI_D_ERROR,
			"[%d] invalid ec-hdr in VTBL block\n", page_no));
			return -1;
		}

		vid_hdr = (struct ubi_vid_hdr *)(hdl->block_buf +
				hdl->vid_hdr_offset);
		result = ubi_is_vid_hdr_valid(hdl, vid_hdr, 1);
		if (result != 1) {
			DEBUG((EFI_D_ERROR,
			"[%d] invalid vid-hdr in VTBL block\n", page_no));
			return -1;
		}

		be32 = ubi_util_le_to_be32(vid_hdr->vol_id);
		ASSERT(be32 == UBI_LAYOUT_VOLUME_ID);

		bi = &hdl->block_info[block_no];
		ASSERT(bi->block_no == block_no);
		ASSERT(bi->vol_id == be32);

		vtbl_records = hdl->leb_size / UBI_VTBL_RECORD_SIZE;
		if (vtbl_records > UBI_MAX_VOLUMES)
			vtbl_records = UBI_MAX_VOLUMES;

		block_buf = hdl->block_buf + hdl->data_offset;

		for (i = 0; i < vtbl_records; ++i) {
			vtbl_rec = (struct ubi_vtbl_record *)block_buf;
			result = ubi_is_vtbl_rec_valid(hdl, vtbl_rec);
			if (result == 1) {
				len2 = strlen((const char *)vtbl_rec->name);
				if (len1 == len2) {
					result = memcmp(vtbl_rec->name,
						hdl->partition_name, len1);
					if (result == 0) {
						DEBUG((EFI_D_ERROR,
						"vtbl-rec found\n"));
						hdl->vol_id = i;
						return 0;
					}
				}
			}

			block_buf += UBI_VTBL_RECORD_SIZE;
		}
	}

	return -1;
}

int ubi_flasher_open(const char *partition_name, ubi_flasher_handle *hdl_arg,
	uint32 *page_size_arg, uint32 *block_size_arg)
{
	struct ubi_flasher_handle_type *hdl;
	int result;

	ASSERT(partition_name != NULL);
	ASSERT(hdl_arg != NULL);
	ASSERT(page_size_arg != NULL);
	ASSERT(block_size_arg != NULL);

	*hdl_arg = NULL;

	hdl = ubi_flasher_handle_alloc();
	if (hdl == NULL) {
		return -1;
	}

	hdl->partition_name = partition_name;

	result = ubi_open_partition(hdl, hdl->partition_name);
	if (result != 0) {
		result = ubi_open_volume(hdl, hdl->partition_name);
		if (result != 0) {
			(void) ubi_flasher_handle_free(hdl);
			return result;
		}
	}

	result = ubi_do_mallocs(hdl);
	if (result != 0) {
		(void) ubi_close_partition(hdl);
		(void) ubi_flasher_handle_free(hdl);
		return result;
	}

	result = ubi_scan_partition(hdl);
	if (result != 0)
		goto Error;

	if (hdl->is_it_volume) {
		result = ubi_find_volume(hdl);
		if (result != 0)
			goto Error;

		result = ubi_erase_volume(hdl);
		if (result != 0)
			goto Error;
	}

	*hdl_arg = hdl;
	*page_size_arg = hdl->page_size;
	*block_size_arg = hdl->block_size_bytes;
	return 0;
Error:
	(void) ubi_do_frees(hdl);
	(void) ubi_close_partition(hdl);
	(void) ubi_flasher_handle_free(hdl);
	return result;
}

static int ubi_flasher_ubi_write(struct ubi_flasher_handle_type *hdl)
{
	struct ubi_ec_hdr *ec_hdr;
	struct ubi_block_info_type *bi;
	uint32 offset, buf_size, data_len_size;
	uint64 ec;
	int result = -1, is_bad_block;

	ASSERT(hdl != NULL);
	ASSERT(hdl->block_info != NULL);
	ASSERT(hdl->last_frame_rcvd == 0);

	if (hdl->frame_no == 1) {
		result = ubi_sanity_check_incoming_image(hdl,
				hdl->buf, hdl->buf_size, 1);
		if (result != 0)
			goto End;

		++hdl->image_seq;
		DEBUG((EFI_D_ERROR, "Will use image-seq : [%d]\n",
			hdl->image_seq));
	}

	buf_size = hdl->buf_size;
	offset = 0;

	hdl->prev_write_block_no = hdl->cur_write_block_no;

	for (;	hdl->cur_write_block_no < hdl->block_count;
		++hdl->cur_write_block_no) {

		if (buf_size == 0) {
			ASSERT(offset == hdl->buf_size);
			result = 0;
			goto End;
		}

		bi = &hdl->block_info[hdl->cur_write_block_no];
		ASSERT(bi->block_no == hdl->cur_write_block_no);
		if (bi->is_bad_block)
			continue;

		if (bi->is_ec_hdr_valid)
			ec = bi->ec + 1;
		else
			ec = hdl->mean_ec;

		if (buf_size < hdl->block_size_bytes) {
			data_len_size = buf_size;
			hdl->last_frame_rcvd = 1;
		} else {
			data_len_size = hdl->block_size_bytes;
		}

 		result = ubi_sanity_check_incoming_image(hdl,
				&hdl->buf[offset], data_len_size, 0);
		if (result != 0)
			goto End;

		ec_hdr = (struct ubi_ec_hdr *)&hdl->buf[offset];

		ubi_remove_F_flag(hdl, ec_hdr);

		result = ubi_update_ec_hdr(hdl, ec_hdr, ec, hdl->image_seq);
		if (result != 0)
			goto End;

		is_bad_block = 0;
		result = ubi_block_write(hdl, &hdl->buf[offset],
				data_len_size, hdl->cur_write_block_no,
				0, &is_bad_block);
		if (result != 0)
			return result;
		if (is_bad_block) {
			result = ubi_update_ec_hdr(hdl, ec_hdr, ec,
					hdl->incoming_img_seq);
			if (result != 0)
				goto End;
			continue;
		}

		buf_size -= data_len_size;
		offset += data_len_size;
	}

	DEBUG((EFI_D_ERROR, "[%d, %d] partition too small for image\n",
		hdl->buf_size, hdl->block_count));
	ASSERT(buf_size >= 1);
	ASSERT(offset < hdl->buf_size);
	result = -1;

End:
	return result;
}

static int ubi_flasher_vol_write(struct ubi_flasher_handle_type *hdl)
{
	struct ubi_ec_hdr *ec_hdr;
	struct ubi_vid_hdr *vid_hdr;
	struct ubi_block_info_type *bi;
	uint32 offset, buf_size, data_len_size, block_no;
	uint8 *leb_buf;
	uint64 ec;
	int result = -1, is_bad_block;

	ASSERT(hdl != NULL);
	ASSERT(hdl->block_info != NULL);
	ASSERT(hdl->last_frame_rcvd == 0);

	if (hdl->frame_no == 1) {
		for (block_no = 0; block_no < hdl->block_count; ++block_no) {
			bi = &hdl->block_info[block_no];
			ASSERT(bi->block_no == block_no);
			if (bi->is_bad_block)
				continue;
			if (!bi->is_vid_hdr_valid)
				continue;
			ASSERT(bi->vol_id != hdl->vol_id);
		}

		DEBUG((EFI_D_ERROR, "Will use image-seq : [%d]\n",
			hdl->image_seq));
	}

	buf_size = hdl->buf_size;
	offset = 0;

	hdl->prev_write_block_no = hdl->cur_write_block_no;

	for (;	hdl->cur_write_block_no < hdl->block_count;
		++hdl->cur_write_block_no) {

		if (buf_size == 0) {
			ASSERT(offset == hdl->buf_size);
			result = 0;
			goto End;
		}

		bi = &hdl->block_info[hdl->cur_write_block_no];
		ASSERT(bi->block_no == hdl->cur_write_block_no);
		if (bi->is_bad_block)
			continue;
		if (bi->is_vid_hdr_valid)
			continue;

		if (bi->is_ec_hdr_valid)
			ec = bi->ec + 1;
		else
			ec = hdl->mean_ec;

		if (buf_size < hdl->leb_size) {
			data_len_size = buf_size;
			hdl->last_frame_rcvd = 1;
		} else {
			data_len_size = hdl->leb_size;
		}

		ubi_util_memset(hdl->block_buf, 0xFF, hdl->block_size_bytes);
		ec_hdr = (struct ubi_ec_hdr *)&hdl->block_buf[0];
		result = ubi_create_ec_hdr(hdl, ec_hdr, ec);
		if (result != 0)
			goto End;

		vid_hdr = (struct ubi_vid_hdr *)
				&hdl->block_buf[hdl->vid_hdr_offset];
		result = ubi_create_vid_hdr(hdl, vid_hdr, hdl->leb_no);
		if (result != 0)
			goto End;

		leb_buf = &hdl->block_buf[hdl->data_offset];

		ubi_util_memscpy(leb_buf, hdl->leb_size,
				&hdl->buf[offset], data_len_size);

		ubi_remove_F_flag(hdl, ec_hdr);

		is_bad_block = 0;
		data_len_size += hdl->data_offset;
		result = ubi_block_write(hdl, &hdl->block_buf[0],
				data_len_size, hdl->cur_write_block_no,
				0, &is_bad_block);
		if (result != 0)
			return result;
		if (is_bad_block)
			continue;

		++hdl->leb_no;
		buf_size -= data_len_size;
		offset += data_len_size;
	}

	DEBUG((EFI_D_ERROR, "[%d, %d] partition too small for image\n",
		hdl->buf_size, hdl->block_count));
	ASSERT(buf_size >= 1);
	ASSERT(offset < hdl->buf_size);
	result = -1;

End:
	return result;
}



int ubi_flasher_write(ubi_flasher_handle hdl_arg, uint32 frame_no,
	uint8 *buf, uint32 buf_size)
{
	struct ubi_flasher_handle_type *hdl;
	int result;

	ASSERT(hdl_arg != NULL);
	ASSERT(buf != NULL);
	ASSERT(buf_size >= UBI_EC_HDR_SIZE);

	hdl = (struct ubi_flasher_handle_type *)hdl_arg;
	ASSERT(hdl->is_in_use == 1);

	if (hdl->last_frame_rcvd) {
		DEBUG((EFI_D_ERROR,
			"[%d %d] buffer size not aligned to nand-block-size\n",
			hdl->buf_size, buf_size));
		return -1;
	}

	if ((hdl->frame_no + 1) != frame_no) {
		DEBUG((EFI_D_ERROR, "[%d %d] frame sequence wrong\n",
			frame_no, buf_size));
		return -1;
	}

	hdl->buf = buf;
	hdl->buf_size = buf_size;

	if (hdl->frame_no == 0) {
		ASSERT(frame_no == 1);
		ASSERT(hdl->prev_write_block_no == 0);
		ASSERT(hdl->cur_write_block_no == 0);
	} else {
		ASSERT((hdl->frame_no + 1) == frame_no);
		ASSERT(hdl->cur_write_block_no != hdl->prev_write_block_no);
	}
	hdl->frame_no = frame_no;

	if (hdl->is_it_volume) {
		result = ubi_flasher_vol_write(hdl);
		if (result != 0)
			goto End;
	} else {
		result = ubi_flasher_ubi_write(hdl);
		if (result != 0)
			goto End;
	}

	ASSERT(hdl->cur_write_block_no != hdl->prev_write_block_no);
	ASSERT(hdl->block_count == (hdl->bad_block_count +
		hdl->used_block_count + hdl->free_block_count));
	result = 0;
End:
	return result;
}


int ubi_flasher_close(ubi_flasher_handle hdl_arg)
{
	struct ubi_flasher_handle_type *hdl;

	ASSERT(hdl_arg != NULL);

	hdl = (struct ubi_flasher_handle_type *)hdl_arg;
	ASSERT(hdl->is_in_use == 1);

	if (!hdl->is_it_volume) {
		ubi_erase_partition(hdl);
	}

	DEBUG((EFI_D_ERROR,
		"[%d %d %d] UBI flashing done\n",
		hdl->block_count, hdl->cur_write_block_no,
		hdl->bad_block_count));

	(void) ubi_do_frees(hdl);
	(void) ubi_close_partition(hdl);
	(void) ubi_flasher_handle_free(hdl);

	return 0;
}

