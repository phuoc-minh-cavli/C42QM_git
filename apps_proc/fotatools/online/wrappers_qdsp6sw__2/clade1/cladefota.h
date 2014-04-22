
#ifndef _CLADE_FOTA_H
#define _CLADE_FOTA_H

#ifdef DEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif

#define DICT_WORDS          2048
#define BYTES_PER_WORD      4
#define WORDS_PER_LINE      16
#define BYTES_PER_LINE      (WORDS_PER_LINE * 4)
#define BLOCK_SZ (BYTES_PER_LINE * 128)
#define DBL_BLOCK_SZ (2*BLOCK_SZ)
#define BYTES_PER_2LINES (2*BYTES_PER_LINE)

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
//#define CLADE_HIGH_SZ 26 jsumit change
#define CLADE_HIGH_SZ 12
// #define CLADE_LOW_SZ 27 jsumit change
#define CLADE_LOW_SZ 13
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
// #define NUM_METADATA 45 jsumit changed 
#define NUM_METADATA 14

#ifdef __cplusplus
extern "C" {
#endif

// Initialize High priority decompression
// Supply dictionary & hi prio exception data, as well as length of uncompressed data
extern void init_HP_decompression(char *exc_hi, uint32_t **dict, unsigned int hi_len);

// Initialize Low priority decompression
// supply dictionary and low prio exception data. Also need to supply the uncompressed length
// as well as the low exception small size, which is provided in the fota_meta structure.
extern void init_LP_decompression(char *exc_lo, uint32_t len, uint32_t **dict, uint32_t lo_len);

// Returns a block of uncompressed high priority data
// Run this continuously until no more data left to uncompress
extern unsigned int clade_HP_decompress_next_blk(char *comp, char *uncomp, unsigned blk_size);

// Returns a block of uncompressed low priority data
// Run this continuously until no more data left to uncompress
extern unsigned int clade_LP_decompress_next_blk(char *comp, char *uncomp, unsigned blk_size);

typedef void * (*oem_mem_malloc)(int buf_size);
typedef void (*oem_mem_free)(void * buf);
typedef void * (*oem_def_assert)(int exp);
typedef void (*oem_def_printf)(void * msg);

// define custom malloc/free/assert/print code
// Curently only malloc required
void clade_function_init(oem_mem_malloc oem_malloc, oem_mem_free oem_free, oem_def_assert oem_assert, oem_def_printf oem_printf);

// Initialize CLADE data for compression. Supply the scratch space and dictionaries.
// Sizes are included in the fota meta structure.
int clade_compress_init(size_t region_low_len, size_t low_exc_len, uint32_t** dicts_data);

// supply 2 cachelines of high priority + 1 cacheline of low priority data
// return the compressed output
void clade_unit_compress(uint32_t* high, uint32_t* low, size_t high_len, size_t low_len, uint32_t* out);

// After supplying all uncompressed data, we need to finalize exceptions and fill in metadata bits.
// We do this here and finalize the low prio compression. Same step not necessary for hi prio.
void finalize_low(uint32_t* buf, uint32_t* main_comp);


#ifdef __cplusplus
}
#endif

#endif // _CLADE_FOTA_H
