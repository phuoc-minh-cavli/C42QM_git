/*==========================================================================
 * FILE:         cladefota.cpp
 *
 * DESCRIPTION:  Wrapper function for CLADE FOTA internal compression/decompression
 *
 * Copyright (c) 2016-2018 Qualcomm Technologies Incorporated.
 * All Rights Reserved. QUALCOMM Proprietary and Confidential.
 ===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cladefota.h"


#ifdef __cplusplus
extern "C" {
#endif

// read = 0, write = 1
static FILE* open_file_POI(const char* filename, const char *rw_flag) {
	FILE *fp = fopen(filename, rw_flag);
	if (!fp) {
		printf("Error opening file: %s\n", filename);
		exit(2);
	}
	return fp;
}

static uint32_t write_to_file_POI(void *buf, uint32_t size, FILE *fp) {
	unsigned rw_size;
	rw_size = fwrite(buf, 1, size, fp);
	if (rw_size != size) {
		printf("Writeout error\n");
		exit(1);
	}
	return rw_size;
}

static uint32_t read_from_file_POI(void *buf, int off, uint32_t size, FILE *fp) {
	unsigned rw_size;
	if (off != -1) {
		fseek(fp, off, SEEK_SET);
	}
	rw_size = fread(buf, 1, size, fp);
	if(rw_size != size) {
		printf("FileRead error: requested %d B, returned %d B\n", size, rw_size);
		exit(1);
	}
	return rw_size;
}

void* mymalloc(int size) {
	return malloc(size);
}

void myfree(void *p) {
	free(p);
}

typedef int (*read_mem_cb)(void *ptr, long int offset, unsigned size);
static FILE *cladehifp = NULL;
static FILE *cladelofp = NULL;

int clade_high_read_cb(void *buf, long int offset, unsigned size) {
  return read_from_file_POI(buf, offset, size, cladehifp);
}

int clade_low_read_cb(void *buf, long int offset, unsigned size) {
  return read_from_file_POI(buf, offset, size, cladelofp);
}

void init_clade_compress_callbacks_POI(const char *high, const char *low) {
	// open high and low input files
	cladehifp = open_file_POI(high, "rb");
	cladelofp = open_file_POI(low, "rb");
}

// For CLADE1 decompression, we can extract high/low priority sections independently
// Each section has an exception area that must be loaded. To save memory, we
// write out blocks at a time to reduce DDR usage. However, we can always save in DDR
// or use a hybrid approach if needed.
static void clade1_decompress_9205_POI(const char *metafile, const char *image, const char *high, const char* low, const char* dict) {
	// read metadata file
	FILE *fdst, *metafp, *imgfp;
    printf("Reading META FILE....\n");
	metafp = open_file_POI(metafile, "rb");
	int *meta = (int*)malloc(NUM_METADATA*sizeof(int));
	read_from_file_POI(meta, 0, NUM_METADATA*sizeof(int), metafp);
	fclose(metafp);
    printf("META FILE Reading Complete....\n");
	// for (int i = 0; i < NUM_METADATA; i++) {
		// printf("Meta %d = %d\n", i, meta[i]);
	// }
    printf("Reading MBN FILE....\n");
	imgfp = open_file_POI(image, "rb");

	// read and setup dictionaries
    printf("\t\tReading CLADE DICT FILE....\n");
	uint32_t *cladedict = (uint32_t*) malloc(meta[CLADE_DICT_SZ]);
	read_from_file_POI(cladedict, meta[CLADE_DICT_OFF], meta[CLADE_DICT_SZ], imgfp);
    printf("\t\tCLADE DICT FILE READING DONE....\n");

	uint32_t *dictionary[3];
	dictionary[0] = cladedict;
	dictionary[1] = (unsigned int*)((char*)dictionary[0] + DICT_WORDS * BYTES_PER_WORD);
	dictionary[2] = (unsigned int*)((char*)dictionary[1] + DICT_WORDS * BYTES_PER_WORD);

	// get high prio exception section
    printf("\t\tReading High Prio Exception Section....\n");
	uint32_t *ex_hi = (uint32_t*) malloc(meta[CLADE_EX_HI_SZ]);
	read_from_file_POI(ex_hi, meta[CLADE_EX_HI_OFF], meta[CLADE_EX_HI_SZ], imgfp);
    printf("\t\tHigh Prio Exception Section Reading Done....\n");
	// decompress high priority section
    printf("\t\tStarting Decompression...\n");
	printf("\t\tStarting hi prioritiy decompression...\n");
	init_HP_decompression((char *) ex_hi, dictionary, meta[CLADE_HIGH_SZ]);
	fdst = open_file_POI(high, "wb");
	printf("\t\tHi Prioritiy Decompression Done...\n");

	char *uncomp_buf, *comp_buf;
	comp_buf = (char*)malloc(BLOCK_SZ);
	uncomp_buf = (char*)malloc(BLOCK_SZ);

	uint32_t ret_size, readsize;
	uint32_t curr = 0;
	// decompress high priority
	// block size should be 64B aligned (except last block)
	while (curr < meta[CLADE_HIGH_SZ]) {
		readsize = (BLOCK_SZ < meta[CLADE_HIGH_SZ] - curr) ? BLOCK_SZ : meta[CLADE_HIGH_SZ] - curr;
        printf("meta[CLADE_HIGH_SZ]: %d  curr: %d meta[CLADE_HIGH_SZ] - curr%d\n ",meta[CLADE_HIGH_SZ],curr,meta[CLADE_HIGH_SZ] - curr);
		memset(comp_buf, 0, BLOCK_SZ);
		memset(uncomp_buf, 0, BLOCK_SZ);
		read_from_file_POI(comp_buf, meta[CLADE_COMP_OFF] + curr, readsize, imgfp);
		ret_size = clade_HP_decompress_next_blk(comp_buf, uncomp_buf, BLOCK_SZ);
		if (ret_size == 0)
			break;
		write_to_file_POI(uncomp_buf, ret_size, fdst);
		curr += BLOCK_SZ;
	}
	fclose(fdst);
	free(ex_hi);
	printf("\t\tFinished hi prioritiy decompression...\n");
	char *ex_lo = (char*) malloc(meta[CLADE_EX_LO_LG_SZ] + meta[CLADE_EX_LO_SM_SZ]);
	char *ex_lo_ptr = (char*) ex_lo + meta[CLADE_EX_LO_LG_SZ];
	read_from_file_POI(ex_lo, meta[CLADE_EX_LO_LG_OFF], meta[CLADE_EX_LO_LG_SZ], imgfp);
	read_from_file_POI(ex_lo_ptr, meta[CLADE_EX_LO_SM_OFF], meta[CLADE_EX_LO_SM_SZ], imgfp);

	// decompress low priority
	dprintf("Starting lo prioritiy decompression...\n");
	init_LP_decompression(ex_lo, meta[CLADE_EX_LO_LG_SZ], dictionary, meta[CLADE_LOW_SZ]);
	fdst = open_file_POI(low, "wb");

	int curr_in = 0, curr_out = 0;
	// note: for low priority, we read twice as much as we write
	while(curr_out < meta[CLADE_LOW_SZ]) {
		readsize = (BLOCK_SZ < meta[CLADE_COMP_SZ] - curr_in) ? 
			BLOCK_SZ : meta[CLADE_COMP_SZ] - curr_in;
		read_from_file_POI(comp_buf, meta[CLADE_COMP_OFF] + curr_in, readsize, imgfp);
		curr_in += readsize;
		ret_size = clade_LP_decompress_next_blk(comp_buf, uncomp_buf, BLOCK_SZ);
		if (ret_size == 0)
			break;
		write_to_file_POI(uncomp_buf, ret_size, fdst);
		curr_out += ret_size;
	}
	fclose(fdst);
	fclose(imgfp);
	dprintf("Finished lo prioritiy decompression...\n");

	free(ex_lo);
	free(comp_buf);
	free(uncomp_buf);

	fdst = open_file_POI(dict, "wb");
	write_to_file_POI(cladedict, meta[CLADE_DICT_SZ], fdst);
	printf("Wroteout following outputs: %s, %s, %s\n", high, low, dict);
	free(meta);
	free(cladedict);
}

// This is the compression core loop that performs the clade compression
// We feed 128B from region_high and 64B from region_low at a time, and
// do it per block to save memory. Data must be sent in order to be compressed
// one unit (line) at a time. We use callbacks to "get the next chunk" of data.
// After all data is consumed by the library, we call finalize to output the final 
// compressed data format
static void clade_compress_core(read_mem_cb read_clade_high, 
								read_mem_cb read_clade_low, 
								uint32_t *out_buf, 
								int *meta, 
								uint32_t *hi_buf, 
								uint32_t hi_blk_sz,
								uint32_t *lo_buf,
								uint32_t lo_blk_sz ) 
{
	uint32_t rw_size;
	uint32_t hi_pos, lo_pos, comp_pos;

	uint32_t hi_bytes_left = (uint32_t) meta[CLADE_HIGH_SZ];
	uint32_t lo_bytes_left = (uint32_t) meta[CLADE_LOW_SZ];
	uint32_t comp_bytes_left = (uint32_t) meta[CLADE_COMP_SZ];

	int line = 0;
	int hi_curr_len, lo_curr_len, bytes_left;
	int hi_offset, lo_offset;
	hi_pos = lo_pos = comp_pos = 0;

	while (comp_bytes_left > 0) {
		// Read new block from high priority
		if((hi_pos % hi_blk_sz) == 0) {
			memset(hi_buf, 0, hi_blk_sz);
			rw_size = hi_bytes_left > hi_blk_sz ? hi_blk_sz : hi_bytes_left;
			read_clade_high(hi_buf, -1, rw_size);
		}
		// Read new block from low priority
		if((lo_pos % lo_blk_sz) == 0) {
			memset(lo_buf, 0, lo_blk_sz);
			rw_size = lo_bytes_left > lo_blk_sz ? lo_blk_sz : lo_bytes_left;
			read_clade_low(lo_buf, -1, rw_size);
		}

		hi_offset = hi_pos % hi_blk_sz;
		lo_offset = lo_pos % lo_blk_sz;

		hi_curr_len = hi_bytes_left >= BYTES_PER_2LINES ? BYTES_PER_2LINES : hi_bytes_left;
		lo_curr_len = lo_bytes_left >= BYTES_PER_LINE ? BYTES_PER_LINE : lo_bytes_left;

		// Compress 2 high lines and 1 low line
		clade_unit_compress(hi_buf + hi_offset/4, lo_buf + lo_offset/4, hi_curr_len, lo_curr_len, out_buf + comp_pos/4);

		bytes_left = comp_bytes_left >= BYTES_PER_2LINES ? BYTES_PER_2LINES : comp_bytes_left;
		comp_bytes_left -= bytes_left;
		comp_pos += bytes_left;

		hi_bytes_left -= hi_curr_len;
		hi_pos += hi_curr_len;

		lo_bytes_left -= lo_curr_len;
		lo_pos += lo_curr_len;

		line++;
	}
}

// This is the overall CLADE1 compression routine. Once we have the metadata and
// dictionary, we can initialize compression. From there we pass our callbacks
// to get a block of high/low data, along with our memory blocks which will be 
// where those data is stored temporarily. The order must be init/compress/finalize.
static void clade1_compress_9205_POI(
					read_mem_cb read_clade_high, 
					read_mem_cb read_clade_low, 
					int *meta, 
					uint32_t *cladedictptr, 
					uint32_t *clade_out_buf, 
					uint32_t *hi_buf, 
					uint32_t hi_blk_sz,
					uint32_t *lo_buf,
					uint32_t lo_blk_sz ) 
{
	uint32_t *dictionary[3];
	dictionary[0] = cladedictptr;
	dictionary[1] = (unsigned int*)((char*)dictionary[0] + DICT_WORDS * BYTES_PER_WORD);
	dictionary[2] = (unsigned int*)((char*)dictionary[1] + DICT_WORDS * BYTES_PER_WORD);

	clade_function_init(mymalloc, myfree, NULL, NULL);

	dprintf("Initializing CLADE compression...\n");
	if (clade_compress_init(meta[CLADE_LOW_SZ], meta[CLADE_EX_LO_SM_SZ] + meta[CLADE_EX_LO_LG_SZ], &dictionary[0]) == 0)
		printf("Failed to initialize CLADE compressor...\n");

	dprintf("Starting CLADE compression...\n");
	// Compress HP and LP but don't fill lo_exc offsets
	clade_compress_core(read_clade_high, read_clade_low, clade_out_buf, 
		meta, hi_buf, hi_blk_sz, lo_buf, lo_blk_sz);

	// finalize exc low and return in buffer
	dprintf("Packing and Finalizing CLADE compression...\n");
	finalize_low(
		(uint32_t*)((char*)clade_out_buf + meta[CLADE_EX_LO_LG_OFF] - meta[CLADE_COMP_OFF]), 
		clade_out_buf);

}

// This wrapper function performance all of the memory allocation and poiner setup
// required to call the library API correctly, while minimizing the memory used.
// Note the callback used for clade1_compress_9205_POI. This can be reimplemented as 
// needed, but is used as FILE I/O for the simplest use case (Linux filesystem)
static void clade1_compress_9205_wrapper_POI(
	const char *metafile, 
	const char *high, 
	const char* low, 
	const char* dict, 
	const char *out)
{
	FILE *dictfp, *metafp, *outfp;

	// read metadata file
	metafp = open_file_POI(metafile, "rb");
	int *meta = (int*)malloc(NUM_METADATA*sizeof(int));
	read_from_file_POI(meta, 0, NUM_METADATA*sizeof(int), metafp);
	fclose(metafp);

	for (int i = 0; i < NUM_METADATA; i++) {
		dprintf("Meta %d = %d\n", i, meta[i]);
	}

	// Init buffer for clade segment
	uint32_t clade_seg_sz = meta[CLADE_META_OFF] + meta[CLADE_META_SZ] - meta[CLADE_COMP_OFF];
	uint32_t *clade_buf = (uint32_t*)malloc(clade_seg_sz);
	memset(clade_buf, 0, meta[CLADE_COMP_SZ]);

	// read dictionaries
	dictfp = open_file_POI(dict, "rb");
	uint32_t *cladedictptr = (uint32_t*)((char*)clade_buf + meta[CLADE_DICT_OFF] - meta[CLADE_COMP_OFF]);
	read_from_file_POI(cladedictptr, 0, meta[CLADE_DICT_SZ], dictfp);
	fclose(dictfp);

	// initialize file IO callbacks
	init_clade_compress_callbacks_POI(high, low);

	// allocate scratchpad memory
	uint32_t *hi_buf = (uint32_t*)malloc(DBL_BLOCK_SZ);
	uint32_t *lo_buf = (uint32_t*)malloc(BLOCK_SZ);

	clade1_compress_9205_POI(clade_high_read_cb, clade_low_read_cb, meta, cladedictptr, 
		clade_buf, hi_buf, DBL_BLOCK_SZ, lo_buf, BLOCK_SZ);

	free(hi_buf);
	free(lo_buf);
	free(meta);
	fclose(cladehifp);
	fclose(cladelofp);

	outfp = open_file_POI(out, "wb");
	write_to_file_POI(clade_buf, clade_seg_sz, outfp);
	printf("Writeout final compressed file: %s...\n", out);
	fclose(outfp);
	free(clade_buf);

}

#ifdef __cplusplus
}
#endif

int main(int argc, const char** argv) {
	if (argc < 4){
		printf("usage: cladefota (-compress | -decompress) metafile inputfile[s]\n");
		printf("\tFor compression, inputs are hi, lo, and dict\n");
		printf("\tFor decompression, input is mbn image\n");
		exit(2);
	}

	if (strcmp(argv[1], "-compress") == 0) {
		const char *out1 = "clade1comp.bin";
        printf("calling the scripts for compression of the image\n");
		clade1_compress_9205_wrapper_POI(argv[2], argv[3], argv[4], argv[5], out1);
	}
	if (strcmp(argv[1],"-decompress") == 0) {
        printf("calling the scripts for De-compression of the image\n");
		const char *hi = "high.bin";
		const char *lo = "low.bin";
		const char *d = "dict.bin";
		clade1_decompress_9205_POI(argv[2], argv[3], hi, lo, d);
	}
	return 0;

}
