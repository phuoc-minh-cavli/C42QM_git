/*===========================================================================
				leak_detector.h

DESCRIPTION
	This header file contains definitions/declarations used by the leak
	Module.

	Copyright (c) 2021-22 by Qualcomm Technologies INCORPORATED.
	All Rights Reserved.
	Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
#ifndef	LEAK_DETECTOR_H
#define	LEAK_DETECTOR_H

#undef tx_byte_allocate
#undef tx_byte_release

#define	FILE_NAME_LENGTH		256
#define OUTPUT_FILE			"leak_info.txt"
#define malloc(size)			leak_kmalloc (size, __FILE__, __LINE__)
#define calloc(elements, size)		leak_kcalloc (elements, size, __FILE__, __LINE__)
#define realloc(mem_ref, size)      	leak_krealloc (mem_ref, size, __FILE__, __LINE__)
#define free(mem_ref)			leak_kfree(mem_ref)

#define tx_byte_allocate(pool_ptr, memory_ptr, memory_size, wait_option) \
	leak_tx_byte_allocate(pool_ptr, memory_ptr, memory_size, wait_option, __FILE__, __LINE__)
#define tx_byte_release(memory_ptr) leak_tx_byte_release(memory_ptr)

void * leak_kmalloc(unsigned int size, const char *file, unsigned int line);
void * leak_kcalloc(unsigned int elements, unsigned int size, const char *file, unsigned int line);
void * leak_krealloc(void *mem_ref, unsigned int size, const char *file, unsigned int line);
void leak_kfree(void *mem_ref);

unsigned int leak_tx_byte_allocate(void *pool_ptr, void **memory_ptr, 
	unsigned long memory_size, unsigned long wait_option, const char *file, unsigned int line);
unsigned int leak_tx_byte_release(void *memory_ptr);

void clear_all_meminfo(void);
void report_memleak(void);
#endif
