#ifndef __UBI_FLASHER_H__
#define __UBI_FLASHER_H__

#include "comdef.h"

#ifndef UBI_FLASHER_SYSTEM_PARTITION_NAME
#define UBI_FLASHER_SYSTEM_PARTITION_NAME "system"
#endif

typedef void* ubi_flasher_handle;

int ubi_flasher_open(const char *partition_name, ubi_flasher_handle *hdl,
	uint32 *page_size_arg, uint32 *block_size_arg);
int ubi_flasher_write(ubi_flasher_handle hdl_arg, uint32 frame_no,
  uint8 *buf, uint32 buf_size);
int ubi_flasher_close(ubi_flasher_handle hdl_arg);

#endif /* __UBI_FLASHER_H__ */
