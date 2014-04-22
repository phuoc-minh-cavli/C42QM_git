/*==============================================================================

Dynamic Loading Support Libs

--------------------------------------------------------------------------------
 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 when               who         what, where, why
 --------           ---         -----------------------------------------------
 1/22/15          amitkulk      created 
==============================================================================*/
/*
 * Australian Public Licence B (OZPLB)
 *
 * Version 1-0
 *
 * Copyright (c) 2006-2010, Open Kernel Labs, Inc.
 *
 * All rights reserved.
 *
 * Developed by: Embedded, Real-time and Operating Systems Program (ERTOS)
 *               National ICT Australia
 *               http://www.ertos.nicta.com.au
 *
 * Permission is granted by Open Kernel Labs, Inc., free of charge, to
 * any person obtaining a copy of this software and any associated
 * documentation files (the "Software") to deal with the Software without
 * restriction, including (without limitation) the rights to use, copy,
 * modify, adapt, merge, publish, distribute, communicate to the public,
 * sublicense, and/or sell, lend or rent out copies of the Software, and
 * to permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimers.
 *
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimers in the documentation and/or other materials provided
 *       with the distribution.
 *
 *     * Neither the name of Open Kernel Labs, Inc., nor the names of its
 *       contributors, may be used to endorse or promote products derived
 *       from this Software without specific prior written permission.
 *
 * EXCEPT AS EXPRESSLY STATED IN THIS LICENCE AND TO THE FULL EXTENT
 * PERMITTED BY APPLICABLE LAW, THE SOFTWARE IS PROVIDED "AS-IS", AND
 * NATIONAL ICT AUSTRALIA AND ITS CONTRIBUTORS MAKE NO REPRESENTATIONS,
 * WARRANTIES OR CONDITIONS OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS
 * REGARDING THE CONTENTS OR ACCURACY OF THE SOFTWARE, OR OF TITLE,
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT,
 * THE ABSENCE OF LATENT OR OTHER DEFECTS, OR THE PRESENCE OR ABSENCE OF
 * ERRORS, WHETHER OR NOT DISCOVERABLE.
 *
 * TO THE FULL EXTENT PERMITTED BY APPLICABLE LAW, IN NO EVENT SHALL
 * NATIONAL ICT AUSTRALIA OR ITS CONTRIBUTORS BE LIABLE ON ANY LEGAL
 * THEORY (INCLUDING, WITHOUT LIMITATION, IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHERWISE) FOR ANY CLAIM, LOSS, DAMAGES OR OTHER
 * LIABILITY, INCLUDING (WITHOUT LIMITATION) LOSS OF PRODUCTION OR
 * OPERATION TIME, LOSS, DAMAGE OR CORRUPTION OF DATA OR RECORDS; OR LOSS
 * OF ANTICIPATED SAVINGS, OPPORTUNITY, REVENUE, PROFIT OR GOODWILL, OR
 * OTHER ECONOMIC LOSS; OR ANY SPECIAL, INCIDENTAL, INDIRECT,
 * CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES, ARISING OUT OF OR IN
 * CONNECTION WITH THIS LICENCE, THE SOFTWARE OR THE USE OF OR OTHER
 * DEALINGS WITH THE SOFTWARE, EVEN IF NATIONAL ICT AUSTRALIA OR ITS
 * CONTRIBUTORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH CLAIM, LOSS,
 * DAMAGES OR OTHER LIABILITY.
 *
 * If applicable legislation implies representations, warranties, or
 * conditions, or imposes obligations or liability on Open Kernel Labs, Inc.
 * or one of its contributors in respect of the Software that
 * cannot be wholly or partly excluded, restricted or modified, the
 * liability of Open Kernel Labs, Inc. or the contributor is limited, to
 * the full extent permitted by the applicable legislation, at its
 * option, to:
 * a.  in the case of goods, any one or more of the following:
 * i.  the replacement of the goods or the supply of equivalent goods;
 * ii.  the repair of the goods;
 * iii. the payment of the cost of replacing the goods or of acquiring
 *  equivalent goods;
 * iv.  the payment of the cost of having the goods repaired; or
 * b.  in the case of services:
 * i.  the supplying of the services again; or
 * ii.  the payment of the cost of having the services supplied again.
 *
 * The construction, validity and performance of this licence is governed
 * by the laws in force in New South Wales, Australia.
 */

#ifndef _UNISTD_H_
#define _UNISTD_H_

/*
 * PSE-51 Minimal Realtime compliance symbol
 * As specified in POSIX 1003.13, Section 6.1.2
 * This is defined by the build system for any projects
 * built in the pse51_env environment
 */

#if !defined(THREAD_SAFE)
extern char **environ;
#else
#include <threadstate.h>
#define environ (*(thread_state_get_environ()))
#endif

#include <sys/types.h>

#if !defined(_SSIZE_T)
#define _SSIZE_T
typedef int ssize_t;
#endif

#if !defined(_OFF_T)
#define _OFF_T
typedef long off_t;
#endif


/*
 * Creates a channel for interprocess communication using Unix pipe semantics.
 *
 * @param fildes            buffer in which the new file descriptors are
 *                          returned
 *
 * @return 0 on success, or -1 and set errno on failure
 */
int
pipe(int fildes[2]);

/*
 * Reads from a file descriptor.
 *
 * @param fildes        file descriptor to read from
 * @param buf           buffer to read data into
 * @param nbyte         number of bytes to read
 *
 * @return the number of bytes actually read, or -1 and set errno if an error
 * occurs. A return value of 0 signifies end-of-file
 */
ssize_t
read(int fildes, void *buf, size_t nbyte);

/*
 * Writes to a file descriptor. Currently only works with pipes, as the file
 * system is static.
 *
 * @param fildes        file descriptor to write to
 * @param buf           buffer where data to be written is stored
 * @param nbyte         number of bytes to write
 *
 * @return the number of bytes actually written, or -1 and set errno if an error
 * occurs.
 */
ssize_t
write(int fildes, const void *buf, size_t nbyte);

/*
 * Closes a file descriptor
 *
 * @param fildes        file descriptor to close
 *
 * @return 0 on success, or -1 and set errno on failure
 */
int
close(int fildes);

/*
 * Move the read/write file offset
 *
 * @param fildes        file descriptor to operate on
 * @param offset        amount to modify the offset by
 * @param whence        seek mode
 *
 * @return the new offset from the beginning of the file, or -1 and set errno on
 * error
 */
off_t
lseek(int fildes, off_t offset, int whence);

/*
 * Write changes to the backing store. Since we don't have a backing store this
 * doesn't do anything.
 *
 * @param fildes        file descriptor to operate on
 *
 * @return 0 if successful, or -1 and set errno on error
 */
int
fsync(int fildes);
#endif /* _UNISTD_H_ */
