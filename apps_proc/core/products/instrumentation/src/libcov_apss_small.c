/* $Revision: #1 $ $Date: 2020/01/30 $
// Copyright (c) Bullseye Testing Technology
// This source file contains confidential proprietary information.
//
// BullseyeCoverage small footprint run-time porting layer
// http://www.bullseye.com/help/env-embedded.html
//
// Implementations should conform to the Single UNIX Specification
// http://www.opengroup.org/onlinepubs/009695399/
*/

#if _BullseyeCoverage
  #pragma BullseyeCoverage off
#endif

  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include "assert.h"

  #define O_CREAT 0x0100
  #define O_TRUNC 0x0200
  #define O_WRONLY 1
  #define S_IRUSR 0x0100
  #define S_IWUSR 0x0080

  /* Declare a buffer to store our coverage data (I just picked an arbitrary
     size) */
  /* Alternatively we could use malloc to dynamically create a buffer of
     the appropriate size */
#define buffer_size 1000000
  unsigned char buffer[buffer_size]; 

  unsigned char * write_ptr;

  int getpid(void)
  {
    /* If you have multiple instrumented programs running simultaneously,
    // this function should return a different value for each of them.
    // Otherwise, return any value.
    */
    return 1;
  }

  int open(const char* path, int oflag, int mode)
  {
    /* Successful return is > 2 */

    /* reset our write pointer to the start of the buffer */
    write_ptr = buffer;
    return 3;
  }

  int close(int fildes)
  {
    /* Successful return is 0 */

    /* For a simple memory buffer we don't need to do anything special */
    return 0;
  }

  int write(int fildes, const void* buf, unsigned nbyte)
  {
    if (fildes == 2) {
      /* Insert your code here to report the error message in buf.
      // The message is nul-terminated and the length of the message is nbyte.
      // This is critical. Implement this first.
      */
      		ERR_FATAL("flides ==2 ", 0, 0, 0);

      /* Log the error or assert() or something */
    } else {
      /* Write the data to our bufffer */
	    if (((unsigned) write_ptr) - ((unsigned) &buffer) + nbyte > buffer_size)
      		ERR_FATAL("Buffer size reached, current write_ptr at %d", write_ptr, 0, 0);
      memcpy(write_ptr, buf, nbyte);
      write_ptr += nbyte;
      return nbyte;
    }
    /* Successful return is number of bytes written >= 0 */
    return -1;
  }
// #endif

/*---------------------------------------------------------------------------
// BullseyeCoverage run-time code
*/
#include "atomic.h"
#include "libcov.h"
#include "version.h"
#include "libcov-core-small.c"
