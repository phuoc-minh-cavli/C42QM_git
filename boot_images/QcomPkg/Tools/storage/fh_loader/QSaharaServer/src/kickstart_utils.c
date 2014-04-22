/*===========================================================================
 * Copyright (c) 2010,2019-2021 Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 *  All data and information contained in or disclosed by this document is
 *  confidential and proprietary information of Qualcomm Technologies, Inc. and all
 *  rights therein are expressly reserved.  By accepting this material the
 *  recipient agrees that this material and the information contained therein
 *  is held in confidence and in trust and will not be used, copied, reproduced
 *  in whole or in part, nor its contents revealed in any manner to others
 *  without the express written permission of Qualcomm Technologies, Inc.
 *
 *  FILE:
 *  kickstart_utils.c
 *
 *  DESCRIPTION:
 *  The module provides wrappers for file io operations and linked list operations
 *  for storing image id and file mapping
 *
 *  kickstart_utils.c : The module provides wrappers for file io operations and linked list operations
 *  for storing image id and file mapping
 *
 *  =========================================================================== 
 *  Edit History:
 *  YYYY-MM-DD		who			why
 *  -----------------------------------------------------------------------------
  * 2021-05-25   	rrwt 	    modified crc_16_calc for bytes 
 *  2019-07-01   	c_vkuppa 	Compile Qsahara on linux
 *==========================================================================================
 */

/* ----------------------------------------------------------------------------- */
/* Include Files */
/* ----------------------------------------------------------------------------- */
#include "common_protocol_defs.h"
#include "kickstart_utils.h"
#include "kickstart_log.h"
#if defined(WINDOWSPC) || defined(WIN32) 
#include "windows_utils.h"
#endif

#define CRC_TAB_SIZE    256             /* 2^CRC_TAB_BITS      */

/* CRC table for 16 bit CRC, with generator polynomial 0x1021,
** calculated 8 bits at a time, MSB first.
*/
const uint16_t crc16_table[CRC_TAB_SIZE] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

static const char* search_paths[MAX_SEARCH_PATHS] = {0};
static int num_search_paths = 0;
static char full_filename_with_path[MAX_FILE_NAME_SIZE];

bool add_search_path(const char* path) {
	if ('/' != path[strlen(path) - 1] && '\\' != path[strlen(path) - 1]) {
		dbg (LOG_ERROR, "Search paths must terminate with a slash");
		return false;
	}

	if (MAX_SEARCH_PATHS == num_search_paths) {
		dbg (LOG_ERROR, "Cannot accept more than %d search paths", MAX_SEARCH_PATHS);
		return false;
	}
	else {
		search_paths[num_search_paths++] = path;
		return true;
	}
}

bool init_search_path_list() 
{
#if defined(WINDOWSPC) || defined(WIN32) 
	add_search_path("C:\\builds\\Perforce_main\\core\\storage\\tools\\kickstart\\common\\kickstart\\kickstart\\");
#endif
	return add_search_path("/firmware/images/");
}

void show_search_paths(void)
{
	int i;
	printf("Search paths:\n");
	for(i = 0; i < num_search_paths; i++) {
		printf("'%s'\n", search_paths[i]);
	}
}

const char* find_file(const char *filename)
{
	int i = 0;
	struct stat status_buf;

	for (i = 0; i < num_search_paths; i++) {
		dbg(LOG_INFO, "search path %d: %s", i, search_paths[i]);
		if (strlcpy(full_filename_with_path, search_paths[i], sizeof(full_filename_with_path)) >= sizeof(full_filename_with_path)) {
			dbg(LOG_ERROR, "String was truncated while copying");
			return NULL;
		}
		if (strlcat(full_filename_with_path, filename, sizeof(full_filename_with_path)) >= sizeof(full_filename_with_path)) {
			dbg(LOG_ERROR, "String was truncated while copying");
			return NULL;
		}
		if (stat(full_filename_with_path, &status_buf) == 0) {
			dbg(LOG_INFO, "Found '%s'", full_filename_with_path);
			return full_filename_with_path;
		}
	}
	if (stat(filename, &status_buf) == 0) {
		dbg(LOG_INFO, "Found '%s' in local directory", filename);
		return filename;
	}

	dbg(LOG_ERROR, "Could not find the file '%s', returning NULL",filename);
	return NULL;
}

int open_file(const char *file_name, bool for_reading)
{
    static bool created_path_to_save_files = false;
	int fd = -1;
    char full_filename[MAX_FILENAME_LENGTH] = {0};
	int write_flags = O_CREAT | O_WRONLY | O_TRUNC;

#if !defined(WINDOWSPC) && !defined(WIN32) 
#ifndef LINUXPC
	struct stat fileInfo;
#endif
#endif

    if (file_name == 0 || file_name[0] == 0) {
        dbg(LOG_ERROR, "Invalid file name");
        return fd;
    }

    if (for_reading) {
#if defined(WINDOWSPC) || defined(WIN32) 
		_sopen_s(&fd, file_name, O_RDONLY | _O_BINARY, _SH_DENYNO, 0);
#else
        fd = open(file_name, O_RDONLY);
#endif
    }
    else {
        if (strlcpy(full_filename,
                    kickstart_options.path_to_save_files,
                    sizeof(full_filename)) >= sizeof(full_filename)
         || strlcat(full_filename,
                    kickstart_options.saved_file_prefix,
                    sizeof(full_filename)) >= sizeof(full_filename)
	     || strlcat(full_filename,
                    file_name,
                    sizeof(full_filename)) >= sizeof(full_filename)) {
            dbg(LOG_ERROR, "String was truncated while concatenating");
            return fd;
        }

		if(false == created_path_to_save_files && strlen(kickstart_options.path_to_save_files) > 0) 
		{
			if(create_path(kickstart_options.path_to_save_files) != true) 
			{
                dbg(LOG_ERROR, "Could not create directory \"%s\" to save files into", kickstart_options.path_to_save_files);
                return fd;
            }
			created_path_to_save_files = true;
        }
		dbg(LOG_EVENT,"Saving '%s'", full_filename);

#if defined(WINDOWSPC) || defined(WIN32) 
		_sopen_s (&fd, full_filename, write_flags | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
#else
#if !defined(LINUXPC) && !defined(linux)
		if (stat(full_filename, &fileInfo) == 0 && S_ISBLK(fileInfo.st_mode)) {
			// If the file is a block device
			write_flags = write_flags | O_DIRECT;
		}
#endif
		fd = open (full_filename,
				   write_flags,
				   S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
#endif
    }
    if (-1 == fd) {
        dbg(LOG_ERROR, "Unable to open input file %s.  Error %d: %s",
             full_filename,
             errno,
             strerror (errno));
    }
    return fd;
}

unsigned long get_file_size(int fd)
{
    struct stat fileInfo;
    if (fstat (fd, &fileInfo) != 0) {
        dbg(LOG_ERROR, "Unable to get file size for fd: %d.  Error: %s",
             errno,
             strerror (errno));
        return 0;
    }
    return fileInfo.st_size;
}

bool  close_file(int fd)
{
   if(close(fd))
   {
     dbg(LOG_ERROR, "Unable to close file descriptor. Error %s", strerror(errno));
     return false;
   }
   return true;
}

/*===========================================================================
 *  METHOD:
 *  create_path
 *
 *  DESCRIPTION:
 *  Creates a directory path if it does not already exist
 *
 *  PARAMETERS
 *  path         - directory path to check for
 *  mode         - mode to be used for any directory creation
 *
 *  RETURN VALUE:
 *  int          - 0 if path is successfully created
 *  ===========================================================================*/
bool create_path(const char* path) {
	struct stat status_buf;
	int retval;
	int permission_checked;
	int i;
	int path_len;
	char temp_dir[MAX_FILE_NAME_SIZE];

	dbg(LOG_INFO, "Inside create_path");

	permission_checked = 0;
	if (strlcpy(temp_dir, path, sizeof(temp_dir)) >= sizeof(temp_dir)) 
	{
		dbg(LOG_ERROR, "String truncated while copying");
		return false;
	}
	if ('\\' == temp_dir[strlen(temp_dir) - 1] || '/' == temp_dir[strlen(temp_dir) - 1]) {
		// Windows expects the name in stat() to not contain a trailing slash
		temp_dir[strlen(temp_dir) - 1] = '\0';
	}

	dbg(LOG_INFO, "temp_dir is '%s'",temp_dir);

	retval = stat(temp_dir, &status_buf);

	if (retval == 0) 
	{
		if (access(temp_dir, W_OK) == -1) 
		{
			dbg (LOG_ERROR, "Does not have write permissions for %s. Pausing...", temp_dir);
			sleep(25);
		}
		return true;
	}

    dbg (LOG_ERROR, "Path %s does not exist. Attempting to create..", temp_dir);

    path_len = strlen(path);
	dbg (LOG_ERROR, "path = '%s'", path);

    i = 0;
    for(i = 0; i < path_len; i++) 
	{
            if (i > 0 && (path[i] == '/' || i == path_len - 1)) 
			{
                //strlcpy(temp_dir, path, i+2);
                //temp_dir[i+1] = '\0';
				dbg (LOG_ERROR, "temp_dir now = '%s', null added at %d", temp_dir,i+1);

                retval = stat(temp_dir, &status_buf);
                if(retval != 0)
				{
						dbg (LOG_ERROR, "Calling mkdir(%s)", temp_dir);

#if defined(WINDOWSPC) || defined(WIN32) 
						retval = mkdir(temp_dir);
#else
						retval = mkdir(temp_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
                        if (retval != 0) {
                                dbg(LOG_ERROR, "Error creating directory '%s'. %s.\n", temp_dir, strerror(errno));
                                return false;
                        }
                        dbg(LOG_INFO, "Successfully created dir '%s'", temp_dir);
                }
                else {
                    if (permission_checked == 0 && access(temp_dir, W_OK) == -1) {
                        permission_checked = 1;
                        dbg(LOG_ERROR, "Does not have write permissions for %s. Pausing...", temp_dir);
                        sleep(25);
                    }
                }
            }
        }
        return true;
}

bool use_wakelock (wakelock_action action) {

	bool retval = true;

#if !defined(WINDOWSPC) && !defined(WIN32) 
#ifndef LINUXPC
    static bool wakelock_works = true;
    const char wake_lock_name[] = "kickstart";
    const char wake_lock_file[] = "/sys/power/wake_lock";
    const char wake_unlock_file[] = "/sys/power/wake_unlock";
    const char *filename;
    int ret = 0;
    int wake_lock_name_length = strlen(wake_lock_name);
    int fd;

    if (false == wakelock_works)
        return false;

    if (action == WAKELOCK_ACQUIRE) {
        filename = wake_lock_file;
    }
    else {
        filename = wake_unlock_file;
    }
    fd = open(filename, O_WRONLY|O_APPEND);

    if (fd < 0) {
        dbg(LOG_ERROR, "Failed to open wake lock file %s: %s", filename, strerror(errno));
        wakelock_works = false;
        return false;
    }

    ret = write(fd, wake_lock_name, wake_lock_name_length);
    if (ret != wake_lock_name_length) {
        dbg(LOG_ERROR, "Error writing to %s: %s", filename, strerror(errno));
        retval = false;
    }
    else {
        dbg(LOG_STATUS, "Wrote to %s", filename);
        retval = true;
    }

    close(fd);
#endif
#endif
    return retval;
}

int is_regular_file(int fd)
{
    struct stat fileInfo;
    if (fstat(fd, &fileInfo) != 0) {
        dbg(LOG_ERROR, "Unable to stat file. Errno %d. Error: %s",
             errno,
             strerror (errno));
        return -1;
    }
    return S_ISREG(fileInfo.st_mode);
}

void print_buffer(byte* buffer, size_t length, int bytes_to_print) {
	size_t i = 0;
	char temp_output_buffer[LOG_BUFFER_SIZE] = { 0 };
	char output_buffer[LOG_BUFFER_SIZE] = { 0 };

    for (i = 0; i < MIN(length, (size_t) bytes_to_print); ++i) {
        snprintf(temp_output_buffer, sizeof(temp_output_buffer), "%02X ", buffer[i]);
        strlcat(output_buffer, temp_output_buffer, sizeof(output_buffer));
    }
    dbg(LOG_INFO, "First few bytes: %s", output_buffer);
    if (length > (size_t) bytes_to_print) {
        output_buffer[0] = '\0';
        for (i = 0; i < (size_t) bytes_to_print; ++i) {
            snprintf(temp_output_buffer, sizeof(temp_output_buffer), "%02X ", buffer[(length - bytes_to_print) + i]);
            strlcat(output_buffer, temp_output_buffer, sizeof(output_buffer));
        }
        dbg(LOG_INFO, "Last few bytes: %s", output_buffer);
    }
}

uint16_t crc_16_calc
(
	byte *buf_ptr,
	/* Pointer to bytes containing the data to CRC.  The bit stream starts
	** in the MS bit of the first byte.
	*/

	uint16_t len
	/* Number of data bytes to calculate the CRC over */
)
{
	uint16_t data, crc_16;

	if (buf_ptr == NULL)
	{
		return 0;
	}

	/* Generate a CRC-16 by looking up the transformation in a table and
	** XOR-ing it into the CRC, one byte at a time.
	*/
	for (crc_16 = CRC_16_SEED; len > 0; len--, buf_ptr++) {
		crc_16 = (uint16_t)(crc16_table[(crc_16 >> (16 - 8)) ^ *buf_ptr] ^ (crc_16 << 8));
	}
	
	return(~crc_16);            /* return the 1's complement of the CRC */

} /* end of crc_16_calc */


