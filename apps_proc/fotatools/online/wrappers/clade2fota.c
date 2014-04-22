
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "clade2fota.h"

#ifdef __cplusplus
extern "C" {
#endif

static long check_filesize(FILE *fp) {
  fseek(fp, 0 , SEEK_END);
  long filesize = ftell(fp);
  return filesize;
}

static FILE* open_file(const char* filename, const char *rw_flag) {
	FILE *fp = fopen(filename, rw_flag);
	if (!fp) {
		printf("Error opening file: %s\n", filename);
		exit(2);		
	}
	dprintf("Opening file: %s\n", filename);
	return fp;
}

static void write_to_file(void *buf, uint32_t size, FILE *fp) {
	unsigned rw_size;
	rw_size = fwrite(buf, 1, size, fp);
	if (rw_size != size) {
		printf("Writeout error\n");
		exit(1);
	}
}

static void read_from_file(void *buf, int off, uint32_t size, FILE *fp) {
	unsigned rw_size;
	if (off != -1) {
		fseek(fp, off, SEEK_SET);
	}
	rw_size = fread(buf, 1, size, fp);
	if(rw_size != size) {
		printf("FileRead error: requested %d B, returned %d B\n", size, rw_size);
		exit(1);
	}
}

void* mymalloc(int size) {
	return malloc(size);
}

static void clade2_decomp(const char *metafile, const char *infile, const char *outfile) {
	FILE* fp;
	uint32_t meta[NUM_METADATA];

	fp = open_file(metafile, "rb");
	read_from_file(meta, 0, sizeof(uint32_t)*NUM_METADATA, fp);
	fclose(fp);

	clade2_data_t config;
	memset(&config, 0, sizeof(clade2_data_t));

	config.anchors[0] = meta[CLADE2_ANCHOR1];
	config.anchors[1] = meta[CLADE2_ANCHOR2];
	config.anchors[2] = meta[CLADE2_ANCHOR3];
	config.anchors[3] = meta[CLADE2_ANCHOR4];
	config.input_len = meta[CLADE2_UNCOMP_SZ];
	config.comp_data_len = meta[CLADE2_COMP_SZ];
	config.comp_data = (uint32_t*)malloc(meta[CLADE2_COMP_SZ]);
	config.metadata = (uint32_t*)malloc(meta[CLADE2_META_SZ]);
	if (clade2_decompress_init(&config)) {
		printf("Error initializing before decompression\n");
		exit(2);
	}

	fp = open_file(infile, "rb");
	long size = check_filesize(fp);

	if (size < meta[CLADE2_COMP_OFF] + meta[CLADE2_COMP_SZ]) {
		printf("Input not large enough, is it mbn image? size of input = %ld", size);
		exit(2);
	}
	read_from_file(config.comp_data, meta[CLADE2_COMP_OFF], meta[CLADE2_COMP_SZ], fp);
	read_from_file(config.metadata, meta[CLADE2_META_OFF], meta[CLADE2_META_SZ], fp);
	fclose(fp);

	// decompress
	uint8_t *buf = (uint8_t*)malloc(BLK_SIZE);
	fp = open_file(outfile, "wb");

	uint32_t cur = 0;
	uint32_t curr_len_out, stride;
	int line = 0;
	while (cur < config.input_len) {
		if ((cur + BLK_SIZE) > config.input_len) {
			curr_len_out = config.input_len - cur;
		} else {
			curr_len_out = BLK_SIZE;
		}
		stride = (curr_len_out + BYTES_PER_LINE - 1) / BYTES_PER_LINE;
		memset(buf, 0, BLK_SIZE);
		clade2_decompress_lines(&config, buf, line, line+stride);
		line += stride;
		cur += BLK_SIZE;
		write_to_file(buf, curr_len_out, fp);
	}
	printf("Wrote out decompressed clade2 data to file: %s\n", outfile);
	fclose(fp);
	free(buf);
	free(config.comp_data);
	free(config.metadata);
	return;
}

static void clade2_compress(const char *metafile, const char *infile, const char *outfile) {
	FILE* fp;
	uint32_t meta[NUM_METADATA];

	fp = open_file(metafile, "rb");
	read_from_file(meta, 0, sizeof(uint32_t)*NUM_METADATA, fp);
	fclose(fp);

	clade2_data_t config;
	memset(&config, 0, sizeof(clade2_data_t));

	config.anchors[0] = meta[CLADE2_ANCHOR1];
	config.anchors[1] = meta[CLADE2_ANCHOR2];
	config.anchors[2] = meta[CLADE2_ANCHOR3];
	config.anchors[3] = meta[CLADE2_ANCHOR4];
	config.entries[0] = meta[CLADE2_COMP_ENTRIES16];
	config.entries[1] = meta[CLADE2_COMP_ENTRIES32];
	config.entries[2] = meta[CLADE2_COMP_ENTRIES48];
	config.entries[3] = meta[CLADE2_COMP_ENTRIES64];
	config.freelist_entries[0] = meta[CLADE2_FREELIST_ENTRIES16];
	config.freelist_entries[1] = meta[CLADE2_FREELIST_ENTRIES32];
	config.freelist_entries[2] = meta[CLADE2_FREELIST_ENTRIES48];
	config.freelist_entries[3] = meta[CLADE2_FREELIST_ENTRIES64];
	config.input_len = meta[CLADE2_UNCOMP_SZ];

	uint32_t *dummy_buf;
	uint32_t dummy_size = meta[CLADE2_META_OFF] - meta[CLADE2_COMP_OFF] - meta[CLADE2_COMP_SZ];

	dummy_buf = (uint32_t *)malloc(dummy_size);
	memset(dummy_buf, 0, dummy_size);
	
	// Set our "custom" malloc
	clade2_function_init(mymalloc, NULL, NULL, NULL);

	// Initialize anchors and listmanager sizes
	if (clade2_compress_init(&config)) {
		exit(2);
	}

	char* data = (char*) malloc(BLK_SIZE);

	// Main compression loop
	uint32_t cur = 0;
	fp = open_file(infile, "rb");

	while(cur < config.input_len) {
		if ((cur + BLK_SIZE) > config.input_len) {
			memset(data, 0, BLK_SIZE);
			read_from_file(data, cur, config.input_len - cur, fp);
			clade2_compress_blk(&config, (uint32_t*)data, config.input_len - cur);
		} else {
			read_from_file(data, cur, BLK_SIZE, fp);
			clade2_compress_blk(&config, (uint32_t*)data, BLK_SIZE);
		}
		cur += BLK_SIZE;
	}
	fclose(fp);
	// End of main compression loop

	// Save registers and freelist
	clade2_compress_cleanup(&config);

	fp = open_file(outfile, "wb");
	write_to_file(config.comp_data, config.comp_data_len, fp);
	write_to_file(dummy_buf, dummy_size, fp);
	write_to_file(config.metadata, config.metadata_len, fp);
	write_to_file(config.freelist[0], ((config.freelist_entries[0] + FL_BLK_DIM - 1) / FL_BLK_DIM * FL_BLK_SZ), fp);
	write_to_file(config.freelist[1], ((config.freelist_entries[1] + FL_BLK_DIM - 1) / FL_BLK_DIM * FL_BLK_SZ), fp);
	write_to_file(config.freelist[2], ((config.freelist_entries[2] + FL_BLK_DIM - 1) / FL_BLK_DIM * FL_BLK_SZ), fp);
	write_to_file(config.freelist[3], ((config.freelist_entries[3] + FL_BLK_DIM - 1) / FL_BLK_DIM * FL_BLK_SZ), fp);
	write_to_file(config.regs, 64, fp);
	printf("Wrote out decompressed clade2 data to file: %s\n", outfile);
	fclose(fp);
	free(data);
}

int main(int argc, const char** argv) {
	if (argc != 4) {
		printf("usage: clade2fota (-compress | -decompress) metafile inputfile outputfile\n");
		printf("     -- compress inputs: metafile <uncomp_data>\n");
		printf("     -- decompress inputs: metafile <mbn_image>\n");
		return 0;
	}
	if (strcmp(argv[1], "-compress") == 0) {
		const char *out = "clade2comp.bin";
		clade2_compress(argv[2], argv[3], out);
	}
	if (strcmp(argv[1], "-decompress") == 0) {
		const char *out = "clade2uncomp.bin";
		clade2_decomp(argv[2], argv[3], out);
	}
	return 0;
}

#ifdef __cplusplus
}
#endif
