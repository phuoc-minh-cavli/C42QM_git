#ifndef __PIL_UTILS_H__
#define __PIL_UTILS_H__

/*=============================================================================

        P E R I P H E R A L   I M A G E   L O A D E R    M O D U L E
                    U T I L I T I E S   A P I S
              
GENERAL DESCRIPTION
  Implements Utilities Apis from external modules for PIL requirements


REGIONAL FUNCTIONS 
  None
  
EXTERNALIZED FUNCTIONS

  pil_utils_init()
    Does any required initializations required to use external apis

  pil_get_timetick()
    Returns current timetick
    
  pil_busywait(usecs)
    simply wait loops for given number of micro seconds.
    
  pil_strcmp(str1, str2)
    compares str1 with str2
  
  pil_flash_open_partition(partition name, elf handle)
    opens flash partition for given partition name
  
  pil_flash_close_partition(elf handle)
    closes flash partition
  
  pil_flash_read_partition(elf handle, dest_buffer, offset, size)
    reads data from partition at given offset for given size and 
    copies to dest_buffer
  
  pil_map_and_get_virt_addr(phys_addr, size, virt_addr, MemObj)
    Maps given physical address for given size in MMU and returns 
    virtual address
  
  pil_unmap_virt_addr(MemObj)
    Unmaps previous given virtual address at MMU

  pil_query_virt_addr(phys_addr, size, virt_addr, MemObj)
    Queries virtual address for a given physical address
    
  pil_common_clocks_vote()
    votes for common system clocks
    
  pil_common_clocks_devote()
    devotes for common system clocks
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
    None.

Copyright (c) 2016 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/subsys/src/pil_utils.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

09/15/16   ab      File created.

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "pil.h"
#include "pil_elf_loader.h"
#include "err.h"
#include "ULogFront.h"


/*=============================================================================

                         DATA DECLARATIONS

=============================================================================*/
extern ULogHandle pil_log_err_handle;
extern ULogHandle pil_log_info_handle;

/*=============================================================================

                         MACRO DECLARATIONS

=============================================================================*/

/*=============================================================================
MACRO 
  PIL_VERIFY

DESCRIPTION
  Given a boolean expression, this macro will verify the condition is TRUE
  and do nothing if the condition is TRUE, otherwise it will call the
  error fatal.

SIDE EFFECTS
  This macro never returns if the condition is FALSE.

RETURN VALUE
  None
=============================================================================*/
/* The forever loop after error_handler will never be executed, it is added to 
   fix klockwork warning */
#define PIL_VERIFY( xx_exp, msg, arg1, arg2, arg3 ) \
            do { \
               if( !(xx_exp) ) \
               { \
                 ERR_FATAL( msg, arg1, arg2, arg3 ); \
                 while(1) \
                 { \
                 } \
               } \
            } while(0)

/*=============================================================================
MACRO 
  PIL ULOG Macros

DESCRIPTION
  ULOG macros for printing out Debug info and error info

SIDE EFFECTS
  None

RETURN VALUE
  None
=============================================================================*/
#define PIL_LOG_ERR_N(args, fmt, ...) \
  ULogFront_RealTimePrintf(pil_log_err_handle, \
                           args + 2, \
                           "%s:%d: " fmt, __FUNCTION__, __LINE__, __VA_ARGS__)

//Todo: Below macros may not be needed if we pass args as input to PIL_LOG_ERR
//0 number arguments are not allowed for now
//#define PIL_LOG_ERR_0(args, fmt, ...) PIL_LOG_ERR_N(args, fmt, __VA_ARGS__)
#define PIL_LOG_ERR_1(args, fmt, ...) PIL_LOG_ERR_N(args, fmt, __VA_ARGS__)
#define PIL_LOG_ERR_2(args, fmt, ...) PIL_LOG_ERR_N(args, fmt, __VA_ARGS__)
#define PIL_LOG_ERR_3(args, fmt, ...) PIL_LOG_ERR_N(args, fmt, __VA_ARGS__)
#define PIL_LOG_ERR_4(args, fmt, ...) PIL_LOG_ERR_N(args, fmt, __VA_ARGS__)
#define PIL_LOG_ERR_5(args, fmt, ...) PIL_LOG_ERR_N(args, fmt, __VA_ARGS__)
#define PIL_LOG_ERR_6(args, fmt, ...) PIL_LOG_ERR_N(args, fmt, __VA_ARGS__)
#define PIL_LOG_ERR_7(args, fmt, ...) PIL_LOG_ERR_N(args, fmt, __VA_ARGS__)
#define PIL_LOG_ERR_8(args, fmt, ...) PIL_LOG_ERR_N(args, fmt, __VA_ARGS__)
#define PIL_LOG_ERR_9(args, fmt, ...) PIL_LOG_ERR_N(args, fmt, __VA_ARGS__)

#define PIL_LOG_ERR(args, fmt, ...) PIL_LOG_ERR_##args(args, fmt, __VA_ARGS__)

#define PIL_LOG_N(level, args, fmt, ...) \
  ULogFront_RealTimePrintf(pil_log_info_handle, \
                           args + 2, \
                           level "%s:%d: " fmt, __FUNCTION__, __LINE__, __VA_ARGS__)

//0 number arguments are not allowed for now
//#define PIL_LOG_0(level, args, fmt, ...) PIL_LOG_N(level, args, fmt)
#define PIL_LOG_1(level, args, fmt, ...) PIL_LOG_N(level, args, fmt, __VA_ARGS__)
#define PIL_LOG_2(level, args, fmt, ...) PIL_LOG_N(level, args, fmt, __VA_ARGS__)
#define PIL_LOG_3(level, args, fmt, ...) PIL_LOG_N(level, args, fmt, __VA_ARGS__)
#define PIL_LOG_4(level, args, fmt, ...) PIL_LOG_N(level, args, fmt, __VA_ARGS__)
#define PIL_LOG_5(level, args, fmt, ...) PIL_LOG_N(level, args, fmt, __VA_ARGS__)
#define PIL_LOG_6(level, args, fmt, ...) PIL_LOG_N(level, args, fmt, __VA_ARGS__)
#define PIL_LOG_7(level, args, fmt, ...) PIL_LOG_N(level, args, fmt, __VA_ARGS__)
#define PIL_LOG_8(level, args, fmt, ...) PIL_LOG_N(level, args, fmt, __VA_ARGS__)
#define PIL_LOG_9(level, args, fmt, ...) PIL_LOG_N(level, args, fmt, __VA_ARGS__)

#define PIL_LOG(level, args, fmt, ...) PIL_LOG_##args(level, args, fmt, __VA_ARGS__)

#ifdef FEATURE_PIL_ENABLE_ALL_LOGS
  #define PIL_LOG_LOW(args, fmt, ...) PIL_LOG("LOW:", args, fmt, __VA_ARGS__)
#else
  #define PIL_LOG_LOW(args, fmt, ...)
#endif
#define PIL_LOG_INFO(args, fmt, ...) PIL_LOG("", args, fmt, __VA_ARGS__)
#define PIL_LOG_WARN(args, fmt, ...)  PIL_LOG("WARNING:", args, fmt, __VA_ARGS__)
//#define PIL_LOG_ERR(args, fmt, ...)  PIL_LOG("ERR", args, fmt, __VA_ARGS__)

//Todo: call this from pil_verify or vice versa.. can fix this to 3 args
#define PIL_LOG_FATAL(args, fmt, ...) \
  do { \
    PIL_LOG("FATAL", args, fmt, __VA_ARGS__);\
    ERR_FATAL(fmt, 0, 0, 0); \
  } while(0)

#define PIL_LOG_RAW_ERR(args, fmt, ...) \
  ULogFront_RealTimePrintf(pil_log_err_handle, args, fmt, __VA_ARGS__)

#define PIL_LOG_RAW_INFO(args, fmt, ...) \
  ULogFront_RealTimePrintf(pil_log_info_handle, args, fmt, __VA_ARGS__)

#define PIL_LOG_RAW_WRN(args, fmt, ...) \
  ULogFront_RealTimePrintf(pil_log_info_handle, args, fmt, __VA_ARGS__)

//Todo: call this from pil_verify or vice versa.. can fix this to 3 args
#define PIL_LOG_RAW_FATAL(args, fmt, ...) \
  do { \
    ULogFront_RealTimePrintf(pil_log_info_handle, args, fmt, __VA_ARGS__); \
    ERR_FATAL(fmt, 0, 0, 0); \
  } while(0)
     
/*=============================================================================

                            FUNCTION DECLARATIONS
                            
=============================================================================*/

/*=============================================================================
FUNCTION 
  pil_utils_init

DESCRIPTION
  Does any required initialization of external modules that PIL uses. This
  should be called once before using any other pil_utils function.

DEPENDENCIES
  None

RETURN VALUE
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_utils_init(void);

/*=============================================================================
FUNCTION 
  pil_get_timetick

DESCRIPTION
  This function returns current timetick

DEPENDENCIES
  pil_utils_init should be called before.

RETURN VALUE
  64 bit timetick
=============================================================================*/
uint64 pil_get_timetick(void);

/*=============================================================================
FUNCTION 
  pil_busywait

DESCRIPTION
  This function wait loops for given number of usecs and returns

DEPENDENCIES
  pil_utils_init should be called before.

RETURN VALUE
  64 bit timetick
=============================================================================*/
void pil_busywait(uint32 n_usec);

/*=============================================================================
FUNCTION 
  pil_strcmp

DESCRIPTION
  This function compares two strings

DEPENDENCIES
  None

RETURN VALUE
  0 if they are same
  non-zero in case they are different
=============================================================================*/
int pil_strcmp(const char *s1, const char *s2);

/*=============================================================================
FUNCTION 
  pil_flash_open_partition

DESCRIPTION
  Opens flash partition for given name and updates flash handle in elf structure

DEPENDENCIES
  None

RETURN VALUE
  PIL_PARTITION_OPEN_FAILURE
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_flash_open_partition(pil_elf_struct_type *handle, char* partition_name);

/*=============================================================================
FUNCTION 
  pil_flash_close_partition

DESCRIPTION
  Closes previously opened flash partition 

DEPENDENCIES
  None

RETURN VALUE
  PIL_PARTITION_OPEN_FAILURE
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_flash_close_partition(pil_elf_struct_type *handle);


/*=============================================================================
FUNCTION 
  pil_flash_read_partition

DESCRIPTION
  Reads data from flash storage from given offset for given size and copies to 
  given buffer. Assumption is buffer has memory of given size.

DEPENDENCIES
  None

RETURN VALUE
  PIL_FLASH_READ_FAILED
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_flash_read_partition(
   pil_elf_struct_type *handle,    
   void   *dest_addr,              /* Destination address */
   uint32  byte_offset,            /* Logical source byte address */
   uint32  size                    /* Size to copy in bytes */
);

/*=============================================================================
FUNCTION 
  pil_map_and_get_virt_addr

DESCRIPTION
  This function maps given physical address for given size in MMU and returns
  virtual address

DEPENDENCIES
  None

RETURN VALUE
  PIL_INVALID_PARAMETERS
  PIL_MAPPING_FAILED
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_map_and_get_virt_addr(uint64 phys_addr, uint64 size, uint64 *virt_addr, void *MemObj);

/*=============================================================================
FUNCTION 
  pil_unmap_virt_addr

DESCRIPTION
  Unmaps previously provided virtual address in MMU

DEPENDENCIES
  None

RETURN VALUE
  PIL_INVALID_PARAMETERS
  PIL_UNMAPPING_FAILED
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_unmap_virt_addr(void *MemObj);


/*=============================================================================
FUNCTION 
  pil_query_virt_addr

DESCRIPTION
  Queries virtual address for a given physical address from MMU. 
  This is used for Imem memory since that will be already mapped.

DEPENDENCIES
  None

RETURN VALUE
  PIL_INVALID_PARAMETERS
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_query_virt_addr(uint64 phys_addr, uint64 size, uint64 *virt_addr, void *MemObj);


/*=============================================================================
FUNCTION 
  pil_common_clocks_vote

DESCRIPTION
  Votes for common system clocks like BIMC etc

DEPENDENCIES
  NPA handle should be initialized

RETURN VALUE
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_common_clocks_vote(void);


/*=============================================================================
FUNCTION 
  pil_common_clocks_devote

DESCRIPTION
  Devotes for common system clocks like BIMC etc

DEPENDENCIES
  pil_common_clocks_vote should be called before calling this

RETURN VALUE
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_common_clocks_devote(void);

#endif /* __PIL_UTILS_H__ */

