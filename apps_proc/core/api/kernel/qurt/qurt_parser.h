#ifndef QURT_FILE_H
#define QURT_FILE_H

/**
  @file qurt_parser.h
  @brief Structures for elf parsing operations
  
  - Contains the structures for ELF headers, program headers and other structures and macros for module integrity check. 

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2019-2021 by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/

#include "tx_api.h"
#include "qurt_types.h"
#include "tx_port.h"
#include "fs_public.h"
#include "qurt_memory.h"

/*======================================================================
                          CONSTANTS AND MACROS
=======================================================================*/

/** first two bytes of elf header contains the magic number */
#define ELF_ID_SIZE					16	

/** ELF header magic number and format indices */
#define ELF_MAGIC_0_INDEX			0
#define ELF_MAGIC_1_INDEX			1
#define ELF_MAGIC_2_INDEX			2
#define ELF_MAGIC_3_INDEX			3
#define ELF_FILE_FORMAT_INDEX		4

/** Magic values */
#define ELF_MAGIC_0					0x7F
#define ELF_MAGIC_1					'E'
#define ELF_MAGIC_2					'L'
#define ELF_MAGIC_3					'F'
#define BIN_MAGIC_0					0x55
#define BIN_MAGIC_1					0x44
#define BIN_MAGIC_2					0x4F
#define BIN_MAGIC_3					0x4D

/** file parsing related return values */
#define TX_FILE_TYPE_ERROR				((UINT) 0x23)
#define TX_READ_FAIL					((UINT) 0x24)
#define TX_FILE_SEEK_FAIL				((UINT) 0x25)
#define TX_MDT_ERROR					((UINT) 0x26)

/** Program Header types */
#define PT_NULL				0
#define PT_LOAD				1

#define ELF_FLAG_SEGMENT_TYPE_MASK		0x7000000
#define ELF_FLAG_SEGMENT_TYPE_SHIFT		0x18
#define ELF_HASH_SEGMENT        		0x2

#define ELF_SEGMENT_TYPE(flag) \
         ( ((flag) & ELF_FLAG_SEGMENT_TYPE_MASK) >> \
            ELF_FLAG_SEGMENT_TYPE_SHIFT )
			
/** 4K alignment */
#define size_round_up(size,alignment) (((((UINT)size) + ((alignment)-1)) & (~((alignment)-1))))


/**file permissions */
#define RO    	0x0     /** open for read only */
#define WO    	0x1     /** open for write only */
#define RW      0x2     /** open for read/write */

/** re-purposing unused variable for different purpose and still keeps direct association with Module handle. */
#define GET_MODULE_IMAGE_HANDLE(m)		(module_img_t *)m->txm_module_instance_image_handle
#define SET_MODULE_IMAGE_HANDLE(m,val)		m->txm_module_instance_image_handle = (ULONG)val

/*=====================================================================
                          Typedefs
 ======================================================================*/

/** 32-bit ELF header format */
typedef struct{
    UCHAR elf_id[ELF_ID_SIZE];				/**< File type identification */
    USHORT elf_file_type;   				/**< object file type */
    USHORT elf_ISA;         				/**< instruction set architecture */
    UINT elf_obj_version;       			/**< object file version */
    UINT elf_img_start_va;         		 	/**< starting Virtual Address */
    UINT elf_ph_offset;             		/**< offset of the program header */
    UINT elf_sh_offset;             		/**< offset of the section header */
    UINT elf_flags;							/**< architecture specific flags */
    USHORT elf_header_size;     			/**< contains elf header size */
    USHORT elf_ph_size;       				/**< contains program header entry size */
    USHORT elf_ph_num;       				/**< contains number of entries in program header table */
    USHORT elf_sh_size;       				/**< contains size of section header table entry */
    USHORT elf_sh_num;       				/**< Number of entries in Section header table */
    USHORT elf_sh_index;       				/**< Contains index of the section header table entry that contains the section names. */
}elf_Header_32_t;

/** 32-bit Program Header format */
typedef struct{
    UINT progHeader_type;           		/**< identifies the type of program header */
    UINT progHeader_offset;          		/**< offset of the segment in the file image */
    UINT progHeader_va;             		/**< virtual address of the segment in memory */
    UINT progHeader_pa;             		/**< physical address of the segment in memory */
    UINT progHeader_file_size;      		/**< size in bytes of the segment in the file image */
    UINT progHeader_mem_size;       		/**< size in bytes of the segment in memory */
    UINT progHeader_flags;          		/**< segment dependent flags */
    UINT progHeader_align;          		/**< alignment */
}elf_progHeader_32_t;

/** 32-bit Program Header format */
typedef struct
{
	UINT secHeader_name;					/**< Section name */ 
	UINT secHeader_type;					/**< Section type */ 
	UINT secHeader_flags;					/**< Section flags */ 
	UINT secHeader_addr;					/**< Section addr */ 
	UINT secHeader_offset;					/**< Section offset */ 
	UINT secHeader_size;					/**< Section size */ 
	UINT secHeader_link;					/**< Section link */ 
	UINT secHeader_info;					/**< Section info  */
	UINT secHeader_align;					/**< Section align  */
	UINT secHeader_entsize;	    			/**< Section ent size */ 
}elf_secHeader_32_t;

/** 64-bit ELF header format */
typedef struct
{
	UCHAR elf_id[ELF_ID_SIZE]; 				/**< File type identification */
	USHORT elf_file_type;                   /**< object file type */
	USHORT elf_ISA;                         /**< instruction set architecture */
	UINT elf_obj_version;              		/**< object file version */
	ULONG elf_img_start_va;                 /**< starting Virtual Address */
	ULONG elf_ph_offset;                  	/**< offset of the program header */
	ULONG elf_sh_offset;                  	/**< offset of the section header */
	UINT elf_flags;                  		/**< architecture specific flags */
	USHORT elf_header_size;                	/**< contains elf header size */
	USHORT elf_ph_size;           			/**< contains program header entry size */
	USHORT elf_ph_num;               		/**< contains number of entries in program header table */
	USHORT elf_sh_size;           			/**< contains size of section header table entry */
	USHORT elf_sh_num;                		/**< Number of entries in Section header table */
	USHORT elf_sh_index;            		/**< Contains index of the section header table entry that contains the section names. */
}elf_Header_64_t;

/** 64-bit Program header format */
typedef struct
{
	UINT progHeader_type;                   /**< identifies the type of program header */
	UINT progHeader_flags;                  /**< segment dependent flags */
	ULONG progHeader_offset;                /**< offset of the segment in the file image */
	ULONG progHeader_va;                 	/**< virtual address of the segment in memory */
	ULONG progHeader_pa;                 	/**< physical address of the segment in memory */
	ULONG progHeader_file_size;             /**< size in bytes of the segment in the file image */
	ULONG progHeader_mem_size;              /**< size in bytes of the segment in memory */
	ULONG progHeader_align;                 /**< alignment */
}elf_progHeader_64_t;

/** 64-bit section header format */
typedef struct
{
	UINT secHeader_name;					/**< Section name  */
	UINT secHeader_type;					/**< Section type */ 
	ULONG secHeader_flags;					/**< Section flags  */
	ULONG secHeader_addr;					/**< Section addr */ 
	ULONG secHeader_offset;					/**< Section offset */ 
	ULONG secHeader_size;		       		/**< Section size */ 
	UINT secHeader_link;		       		/**< Section link */ 
	UINT secHeader_info;		       		/**< Section info */ 
	ULONG secHeader_addralign;				/**< Section align */ 
	ULONG secHeader_entsize;	       		/**< Section ent size */ 
}elf_secHeader_64_t;

/** Module file type loaded */
typedef enum {
	IMG_BIN,
	IMG_MBN,
	IMG_UNDEFINED
}img_type_t;

/** ELF format loaded */
typedef enum {
	ELF_FORMAT_32=1,
	ELF_FORMAT_64
}img_format_t;

/** Module specific structure for ELF specific values */
typedef struct {
   UINT file_desc;  						/**< module image file name or pointer to module image */
   img_type_t img_type; 					/**< indicates the file type of the module image file */
   img_format_t img_format;					/**< indaicates the format of the image loaded */
   UINT elf_header_size;					/**< contains the elf header size */
   UINT ph_num;								/**< contains the number of program headers */
   ULONG ph_offset;							/**< indicates the offset of the first program header */
   UINT ph_size;							/**< contains the size of each program header */
   ULONG load_segment_offset;				/**< indicates the offset of the load segment(4K aligned) */
   ULONG hash_segment_offset;				/**< indicates the offset of the hash segment */
   UINT hash_segment_size;					/**< contains the size of the hash segment */
   UINT load_segment_file_size;				/**< contains the size of the load segment in file */
   UINT load_segment_mem_size;				/**< size of the load segment in memory */
   UINT load_segment_aligned_size;			/**< 4K aligned size of load segment */
   char *MDT_buf;							/**< pointer to the MDT buffer */
   char *load_segment;						/**< pointer to the load segment buffer */
} module_img_t;


#endif 	/* QURT_FILE_H */

