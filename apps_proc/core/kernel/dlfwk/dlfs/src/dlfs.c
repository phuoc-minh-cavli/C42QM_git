/*==============================================================================

Dynamic Loading Support Libs

Copyright (c) 2014 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

--------------------------------------------------------------------------------
 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 when               who         what, where, why
 --------           ---         -----------------------------------------------
 1/22/15          amitkulk      created 
==============================================================================*/
#include "dlfs.h"
#include "dlfs_std_io.h"
#include "dlfs_version_info.h"
#include "comdef.h"
#include "elf_metadata.h"

#include <stdlib.h>
#include <stdio.h>
#include "sys_mman.h"
#include "types.h"

#define MAX_DIGEST_SIZE 32  //MAX digest size is 32

//dlfs file handle, that we pass as opaque handle to dynamic loader
typedef struct dlfs_file {
	unsigned int			used;		/*Marker to detect if spot is free ot in use*/
	const char* 			name;
	dlfs_std_FILE			fd;
	Elf32_Ehdr				*Ehdr;
	Elf32_Phdr				*Phdr;
	dl_qc_auth_seg			*Auth_hdr;
	char*					hash_segment_buffer;
	uint32					hash_seg_size;
	int						nDigestSize;
} dlfs_file;

//Global declaration
static dlfs_file dlfs_handle_q[30] ={0}; /*Static array of dlfs file handles*/

//returns index of a free spot in static array of handles
//If more than 30 files are opened, returns -1 as error
static int dlfs_add(void){

	int index=0;
	for(index=0; index<30; index++)
	{
		if(dlfs_handle_q[index].used == 0)
		{
			//ToDo atomic add to mark used
			dlfs_handle_q[index].used = 1;
			return index;
		}
	}
	return -1;
}

//Frees a spot in static array of handles
static void dlfs_remove(int dlfs_queue_index){

	dlfs_handle_q[dlfs_queue_index].fd.fd = -1;
	dlfs_handle_q[dlfs_queue_index].fd.filesystem = -1;

	if(dlfs_handle_q[dlfs_queue_index].Ehdr != NULL) free(dlfs_handle_q[dlfs_queue_index].Ehdr);
	dlfs_handle_q[dlfs_queue_index].Ehdr = NULL;

	if(dlfs_handle_q[dlfs_queue_index].Phdr != NULL) free(dlfs_handle_q[dlfs_queue_index].Phdr);
	dlfs_handle_q[dlfs_queue_index].Phdr = NULL;

	dlfs_handle_q[dlfs_queue_index].hash_seg_size= 0;
	dlfs_handle_q[dlfs_queue_index].nDigestSize =  0;

	//make used zero in the very last end for thread safety
	dlfs_handle_q[dlfs_queue_index].used = 0;
}



//Given a file name, parse an MBN file 
//Recover Ehdr, Phdr table, Auth header
static int dlfs_ParseElf(const char* name, unsigned int mode){

	//File
	dlfs_std_FILE               fd;
	
	int                         hash_seg_size = 0,
                                dlfs_handle_q_index=-1;
	
   //elf structures
	Elf32_Ehdr                  *Ehdr = NULL;
	Elf32_Phdr                  *Phdr = NULL;
	char                        *hash_segment_buffer = NULL;
   
	//Open
	if(_fopen(name, mode, &fd) == -1) {
		return -1;
	}

	//Malloc, read Ehdr
	if((Ehdr = (Elf32_Ehdr*)malloc(sizeof(Elf32_Ehdr))) == NULL)
	{
		goto BAD;
	}
   
	if((_fread(&fd, (char*)Ehdr, sizeof(Elf32_Ehdr))) !=  sizeof(Elf32_Ehdr))
	{
		goto BAD;
	}

	//Verify sanity
	if(		!(Ehdr->e_ident[ELFINFO_MAG0_INDEX] == ELFINFO_MAG0 && 
			Ehdr->e_ident[ELFINFO_MAG1_INDEX] == ELFINFO_MAG1 &&
			Ehdr->e_ident[ELFINFO_MAG2_INDEX] == ELFINFO_MAG2 &&
			Ehdr->e_ident[ELFINFO_MAG3_INDEX] == ELFINFO_MAG3 &&
			Ehdr->e_ident[ELFINFO_CLASS_INDEX] == ELF_CLASS_32 &&
			Ehdr->e_ident[ELFINFO_VERSION_INDEX] == ELF_VERSION_CURRENT &&
			Ehdr->e_ehsize == sizeof(Elf32_Ehdr) &&
			Ehdr->e_phentsize == sizeof(Elf32_Phdr)))
	{
		goto BAD;
	}
   
	//Malloc, read Phdr
	if((Phdr = (Elf32_Phdr*)malloc(sizeof(Elf32_Phdr)* (Ehdr->e_phnum))) == NULL)
	{
		goto BAD;
	}
   
	if((_fseek(&fd, Ehdr->e_phoff, DLFS_STD_IO_SEEK_SET)) != 0)
	{
		goto BAD;
	}

	if((_fread(&fd, (char*)Phdr, sizeof(Elf32_Phdr) * Ehdr->e_phnum)) != (sizeof(Elf32_Phdr) * Ehdr->e_phnum))
	{
		goto BAD;
	}    

	if(Ehdr != NULL && Phdr != NULL)
	{
		dlfs_handle_q_index = dlfs_add();

		if(dlfs_handle_q_index != -1)
		{    
			dlfs_handle_q[dlfs_handle_q_index].fd.fd = fd.fd;
			dlfs_handle_q[dlfs_handle_q_index].fd.filesystem = fd.filesystem;
			dlfs_handle_q[dlfs_handle_q_index].name = name;
			dlfs_handle_q[dlfs_handle_q_index].Ehdr = Ehdr;
			dlfs_handle_q[dlfs_handle_q_index].Phdr = Phdr;
			dlfs_handle_q[dlfs_handle_q_index].Auth_hdr = NULL;
			dlfs_handle_q[dlfs_handle_q_index].hash_segment_buffer = NULL;
			dlfs_handle_q[dlfs_handle_q_index].hash_seg_size = 0;
			dlfs_handle_q[dlfs_handle_q_index].nDigestSize = 0; 
			return dlfs_handle_q_index;
		}
		else //failed to get spot in the static array of dlfs handles 
		{
			goto BAD;
		}  
	}
	
	//If we fail at any point, release all the buffers, close file and bail out
	BAD:
		if(Ehdr != NULL) free(Ehdr);
		if(Phdr != NULL) free(Phdr);
		if(hash_segment_buffer != NULL) sys_munmap(hash_segment_buffer, hash_seg_size);
		_fclose(&fd);
		return -1;

}


int dlfs_open(const char* name, unsigned int mode)
{
	int				dlfs_handle_q_index=-1;
   
	//Parse ELF to get authentication details
	if((dlfs_handle_q_index = dlfs_ParseElf(name, mode)) == -1)
	{
		return -1;
	}
    
	//set back the file pointer to begining
	_fseek(&(dlfs_handle_q[dlfs_handle_q_index].fd), 0, DLFS_STD_IO_SEEK_SET);
    
	return dlfs_handle_q_index;  
   
}

int dlfs_close(int fd)
{
	int ret_val;

	//close file
	ret_val = _fclose(&(dlfs_handle_q[fd].fd));

	//Call destroy dlfs handle element here
	dlfs_remove(fd);

	return ret_val;
}

int dlfs_flen(int fd)
{
	return _flen(&(dlfs_handle_q[fd].fd)); 
}

int dlfs_fstat(int fd, void* buffer)
{ 
	struct dlfs_stat* pstat = (struct dlfs_stat*)buffer;
	pstat->st_dev = 0;
	pstat->st_ino = 0;
	pstat->st_size = dlfs_flen(fd);
	return 0;
}

int dlfs_ftell(int fd)
{
	return _ftell(&(dlfs_handle_q[fd].fd));
}


int dlfs_read(int fd, char* buffer, int count)
{
	return _fread(&(dlfs_handle_q[fd].fd), (char*)buffer, count);

}

int dlfs_seek(int fd, int offset)
{
	return _fseek(&(dlfs_handle_q[fd].fd), offset, DLFS_STD_IO_SEEK_SET);
}

void* dlfs_mmap(void* addr, size_t length, int prot, int flags, int fd,
                off_t offset)
{
	return sys_mmap(addr, length, prot, flags, (int)(&(dlfs_handle_q[fd].fd)), offset);
}


int dlfs_munmap(void* addr, size_t length)
{
	return sys_munmap(addr, length);
}

