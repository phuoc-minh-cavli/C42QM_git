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
#include <stdlib.h>
#include <stdio.h>
#include "string.h"

#include "rfs_api.h"
#include "dlfs_std_io.h"

#define DLFS_RFS_FMT "/readwrite%s"
#define DLFS_EFS_FMT  "/sd%s"

static int _rfs_fopen(const char* filename, unsigned int  fileMode) {
   int fd = -1, len =-1, err =-1;
   char* name_tagged = NULL;
   //const char* open_mode = NULL;
   
   len = strlen(DLFS_RFS_FMT) + strlen(filename) + 1;
   
   name_tagged = malloc(len * sizeof(char));
   do
   {
      if(name_tagged)
      {
         //do actual copy with required formatting
         err = snprintf(name_tagged, len, DLFS_RFS_FMT, filename);
         if( err>=len || err <0)
            break;
         
         fd = (int)rfs_open(name_tagged, RFS_O_RDONLY, 0);
         
      }
   }while(0);
   
   free(name_tagged);
   if(fd >= 0) return fd; 
   else return -1;
}


int _fopen(const char *filename, unsigned int  fileMode, dlfs_std_FILE *psout)
{ 
   if(!psout)
      return -1;
   
   psout->fd = _rfs_fopen(filename, fileMode);
   if(psout->fd == 0)
   {
      psout->filesystem = DLFS_STD_IO_RFS; // RFS
   } else
   {
      psout->fd  = -1;
      psout->filesystem = -1;
      return -1;
    }
	return 1;
}


static long _rfs_flen(int fd)
{
   long rlen = 0, curr_pos = 0;
   curr_pos = rfs_seek((int)fd, 0, RFS_SEEK_CUR); //store current position
   
   //Seek to end
   if(0 <= rfs_seek(fd, 0, RFS_SEEK_END)) 
   {
      //Get current offset, EOF offset is returned
      rlen = rfs_seek(fd, 0, RFS_SEEK_CUR);
      if(0 <= rlen)
      {
         //go back to original offset
         if(0 <= rfs_seek(fd, curr_pos, RFS_SEEK_SET)) 
         {
            return rlen;
         }
      }
   }

   return -1;
}


int _flen(dlfs_std_FILE *psin)
{
   if(!psin)
      return -1;
      
   if(DLFS_STD_IO_RFS == psin->filesystem)
   {
      return (int)_rfs_flen(psin->fd);
   }  
   else
   {
      return -1;
   }
}

int _fread(dlfs_std_FILE *psin, char *buf, int bufLen)
{
   int nBytesRead = 0;   
   if(!psin)
      return -1;
      
   if(DLFS_STD_IO_RFS == psin->filesystem)
   {
      nBytesRead = rfs_read(psin->fd, (char *)buf, bufLen);
   }  
   else
   {
      return -1;
   }

   if(nBytesRead != bufLen)
   {
      return -1;
   }

   return nBytesRead;
}

static int rfs_fseek(int fd, int offset, dlfs_std_IO_SEEK whence)
{
   int type = 0, err =-1;

   switch(whence)
   {
      case DLFS_STD_IO_SEEK_SET:
         type = RFS_SEEK_SET;
         break;
      case DLFS_STD_IO_SEEK_CUR:
         type = RFS_SEEK_CUR;
         break;
      default:
         return -1;
   }
   
   err =  rfs_seek(fd, offset, type);

   if (err != offset) return -1; //RFS seek returns the current file offset from begining
   else return 0;
}

int _fseek(dlfs_std_FILE *psin, int offset, dlfs_std_IO_SEEK whence)
{
   if(!psin)
      return -1;
      
   if(DLFS_STD_IO_RFS == psin->filesystem)
   {
      return rfs_fseek(psin->fd, offset, whence);
   } 
   else
   {
      return -1;
   }

}

int _ftell(dlfs_std_FILE *psin)
{
   int err =-1;
   if(!psin)
      return -1; 
     
   if(DLFS_STD_IO_RFS == psin->filesystem)
   {
      err = rfs_seek((int)(psin->fd), 0, RFS_SEEK_CUR);
      if(err < 0) return -1;
      else return err;
   } 
   else
   {
      return -1;
   }
   
}

int _fclose(dlfs_std_FILE *psin)
{

   if(DLFS_STD_IO_RFS == psin->filesystem)
   {
      return rfs_close(psin->fd);
   } 
   else
   {
      return -1;
   }
   
}
