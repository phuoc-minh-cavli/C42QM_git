#ifndef __CLADE2FOTA_H__
#define __CLADE2FOTA_H__

#ifdef DEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif

// Block size for compression/decompression
// Cachelines are 64B, but operating at that size will affect performance
// However, we don't need to allocate the entire compressed size
#define BLK_SIZE (1 << 14)

#define BYTES_PER_LINE 64
#define FL_BLK_SZ 64
#define FL_BLK_DIM 23

#define CLADE_COMP_OFF 0
#define CLADE_COMP_SZ 1
#define CLADE_EX_LO_LG_OFF 2
#define CLADE_EX_LO_LG_SZ 3
#define CLADE_EX_LO_SM_OFF 4
#define CLADE_EX_LO_SM_SZ 5
#define CLADE_EX_HI_OFF 6
#define CLADE_EX_HI_SZ 7
#define CLADE_DICT_OFF 8
#define CLADE_DICT_SZ 9
#define CLADE_META_OFF 10
#define CLADE_META_SZ 11
#define CLADE2_COMP_OFF 12
#define CLADE2_COMP_SZ 13
#define CLADE2_META_OFF 14
#define CLADE2_META_SZ 15
#define CLADE2_FL16_OFF 16
#define CLADE2_FL16_SZ 17
#define CLADE2_FL32_OFF 18
#define CLADE2_FL32_SZ 19
#define CLADE2_FL48_OFF 20
#define CLADE2_FL48_SZ 21
#define CLADE2_FL64_OFF 22
#define CLADE2_FL64_SZ 23
#define CLADE2_REGS_OFF 24
#define CLADE2_REGS_SZ 25
#define CLADE_HIGH_SZ 26
#define CLADE_LOW_SZ 27
#define CLADE2_UNCOMP_SZ 28
#define CLADE2_ANCHOR1 29
#define CLADE2_ANCHOR2 30
#define CLADE2_ANCHOR3 31
#define CLADE2_ANCHOR4 32
#define CLADE2_FL16_OFFSET 33
#define CLADE2_FL32_OFFSET 34
#define CLADE2_FL48_OFFSET 35
#define CLADE2_FL64_OFFSET 36
#define CLADE2_COMP_ENTRIES16 37
#define CLADE2_COMP_ENTRIES32 38
#define CLADE2_COMP_ENTRIES48 39
#define CLADE2_COMP_ENTRIES64 40
#define CLADE2_FREELIST_ENTRIES16 41
#define CLADE2_FREELIST_ENTRIES32 42
#define CLADE2_FREELIST_ENTRIES48 43
#define CLADE2_FREELIST_ENTRIES64 44
#define NUM_METADATA 45

typedef struct {
	uint32_t anchors[4];
	uint32_t entries[4];
	uint32_t freelist_entries[4];
	uint32_t *freelist[4];
	uint32_t *comp_data;
	uint32_t *metadata;
	uint32_t *regs;
	uint32_t comp_data_len;
	uint32_t metadata_len;
	uint32_t input_len;
	int errorcode;
} clade2_data_t;

typedef void * (*oem_mem_malloc)(int buf_size);
typedef void (*oem_mem_free)(void * buf);
typedef void * (*oem_def_assert)(int exp);
typedef void (*oem_def_printf)(void * msg);

// define custom malloc/free/assert/print code
// Curently only malloc required
void clade2_function_init(oem_mem_malloc oem_malloc, oem_mem_free oem_free, oem_def_assert oem_assert, oem_def_printf oem_printf);

// Initialize clade2 compression here. Fill out the clade2_data_t structure members:
// config.anchors
// config.entries
// config.freelist_entries
// config.input_len
int clade2_compress_init(clade2_data_t *s);

// Initialize clade2 decompression here. Fill out the clade2_data_t structure members:
// config.anchors
// config.input_len
// config.comp_data_len
// config.comp_data (allocate)
// config.metadata (allocate)
int clade2_decompress_init(clade2_data_t *s);

// Compress data to CLADE2 format. Everything must happen in sequence here...
uint32_t clade2_compress_blk(clade2_data_t *s, uint32_t *uncomp_buf, uint32_t uncomp_len);

// After compression, we run cleanup to finalize freelists and registers
void clade2_compress_cleanup(clade2_data_t *s);

// Decompression interface. We specify on cacheline basis which lines to decompress.
// Need to loop through 0-N cachelines in CLADE2 to get final uncompressed data
void clade2_decompress_lines(clade2_data_t *s, uint8_t *buf, uint32_t start, uint32_t end);

#endif
