##############################################################################z
# Australian Public Licence B (OZPLB)
# 
# Version 1-0
# 
# Copyright (c) 2006-2010, Open Kernel Labs, Inc.
# 
# All rights reserved.
# 
# Developed by: Embedded, Real-time and Operating Systems Program (ERTOS)
#               National ICT Australia
#               http://www.ertos.nicta.com.au
# 
# Permission is granted by Open Kernel Labs, Inc., free of charge, to
# any person obtaining a copy of this software and any associated
# documentation files (the "Software") to deal with the Software without
# restriction, including (without limitation) the rights to use, copy,
# modify, adapt, merge, publish, distribute, communicate to the public,
# sublicense, and/or sell, lend or rent out copies of the Software, and
# to permit persons to whom the Software is furnished to do so, subject
# to the following conditions:
# 
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimers.
# 
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimers in the documentation and/or other materials provided
#       with the distribution.
# 
#     * Neither the name of Open Kernel Labs, Inc., nor the names of its
#       contributors, may be used to endorse or promote products derived
#       from this Software without specific prior written permission.
# 
# EXCEPT AS EXPRESSLY STATED IN THIS LICENCE AND TO THE FULL EXTENT
# PERMITTED BY APPLICABLE LAW, THE SOFTWARE IS PROVIDED "AS-IS", AND
# NATIONAL ICT AUSTRALIA AND ITS CONTRIBUTORS MAKE NO REPRESENTATIONS,
# WARRANTIES OR CONDITIONS OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
# BUT NOT LIMITED TO ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS
# REGARDING THE CONTENTS OR ACCURACY OF THE SOFTWARE, OR OF TITLE,
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT,
# THE ABSENCE OF LATENT OR OTHER DEFECTS, OR THE PRESENCE OR ABSENCE OF
# ERRORS, WHETHER OR NOT DISCOVERABLE.
# 
# TO THE FULL EXTENT PERMITTED BY APPLICABLE LAW, IN NO EVENT SHALL
# NATIONAL ICT AUSTRALIA OR ITS CONTRIBUTORS BE LIABLE ON ANY LEGAL
# THEORY (INCLUDING, WITHOUT LIMITATION, IN AN ACTION OF CONTRACT,
# NEGLIGENCE OR OTHERWISE) FOR ANY CLAIM, LOSS, DAMAGES OR OTHER
# LIABILITY, INCLUDING (WITHOUT LIMITATION) LOSS OF PRODUCTION OR
# OPERATION TIME, LOSS, DAMAGE OR CORRUPTION OF DATA OR RECORDS; OR LOSS
# OF ANTICIPATED SAVINGS, OPPORTUNITY, REVENUE, PROFIT OR GOODWILL, OR
# OTHER ECONOMIC LOSS; OR ANY SPECIAL, INCIDENTAL, INDIRECT,
# CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES, ARISING OUT OF OR IN
# CONNECTION WITH THIS LICENCE, THE SOFTWARE OR THE USE OF OR OTHER
# DEALINGS WITH THE SOFTWARE, EVEN IF NATIONAL ICT AUSTRALIA OR ITS
# CONTRIBUTORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH CLAIM, LOSS,
# DAMAGES OR OTHER LIABILITY.
# 
# If applicable legislation implies representations, warranties, or
# conditions, or imposes obligations or liability on Open Kernel Labs, Inc.
# or one of its contributors in respect of the Software that
# cannot be wholly or partly excluded, restricted or modified, the
# liability of Open Kernel Labs, Inc. or the contributor is limited, to
# the full extent permitted by the applicable legislation, at its
# option, to:
# a.  in the case of goods, any one or more of the following:
# i.  the replacement of the goods or the supply of equivalent goods;
# ii.  the repair of the goods;
# iii. the payment of the cost of replacing the goods or of acquiring
#  equivalent goods;
# iv.  the payment of the cost of having the goods repaired; or
# b.  in the case of services:
# i.  the supplying of the services again; or
# ii.  the payment of the cost of having the services supplied again.
# 
# The construction, validity and performance of this licence is governed
# by the laws in force in New South Wales, Australia.

import os
from cStringIO import StringIO
import gzip
from struct import pack
import optparse
from weaver import MergeError
from weaver.subcommand import register_subcommand


# From: http://people.freebsd.org/~kientzle/libarchive/man/cpio.5.txt
# The CPIO newc header format is:
#
# struct cpio_newc_header {
#	   char    c_magic[6];
#	   char    c_ino[8];
#	   char    c_mode[8];
#	   char    c_uid[8];
#	   char    c_gid[8];
#	   char    c_nlink[8];
#	   char    c_mtime[8];
#	   char    c_filesize[8];
#	   char    c_devmajor[8];
#	   char    c_devminor[8];
#	   char    c_rdevmajor[8];
#	   char    c_rdevminor[8];
#	   char    c_namesize[8];
#	   char    c_check[8];
#   };
#
# The input file should match the Linux initramfs file format used by
# the gen_init_cpio program.
#
# The magic number is 070701
#
#
# We don't pull this from stat since we may be running on a non-Linux
# machine

MAGIC = "070701"

S_IFSOCK = "0140000"
S_IFLNK = "0120000"
S_IFREG = "0100000"
S_IFBLK = "0060000"
S_IFDIR = "0040000"
S_IFCHR = "0020000"
S_IFIFO = "0010000"

PADDINGS = [
    '',
    pack('ccc', '\0', '\0', '\0'),
    pack('cc', '\0', '\0'),
    pack('c', '\0')
        ]

def split_line(line):
    """
    Split a line into individul parts. Fields are separate by spaces.
    Handle. Spaces in a field are escaped by a back-slash. E.g:
    "foo bar\ baz" => ("foo", "bar baz")
    """
    parts = line.split(" ")
    new_parts = []
    join = False

    for part in parts:
        if part == '':
            continue

        if not join:
            new_parts.append(part)
        else:
            new_parts[-1] += part

        if part.endswith("\\"):
            join = True
            new_parts[-1] = new_parts[-1][:-1] + " "
        else:
            join = False

    return new_parts

class CpioBuilder:
    def __init__(self, list_file):
        self.stream = StringIO()
        self.cstream = gzip.GzipFile(fileobj=self.stream, mode="wb") #gzip compressed stream
        self.file_name = list_file
        self.list_file = open(list_file, "r")
        self.count = 0
        self.ino = 37
        self.mtime = 0
        self.major = 3
        self.minor = 1
        self.size = 0

    def cpio_align(self):
        """
        Align total number of written bytes on a 4 byte boundary, pad with
        NULL bytes if necessary.
        """
        padding = self.size % 4
        if self.size % 4 != 0:
            self.cstream.write(PADDINGS[padding])
            self.cstream.flush()
            self.size += len(PADDINGS[padding])

    def cpio_header_append(self, string):
        """
        Append a string to cpio header and align header + string on 4 byte
        boundary (padded with NULL bytes).
        """

        def align_header(align_len):
            if align_len % 4 != 0:
                padding = align_len % 4
                self.cstream.write(PADDINGS[padding])
                self.cstream.flush()
                self.size += len(PADDINGS[padding])

        self.cstream.write(string)
        self.cstream.write(PADDINGS[3])
        self.cstream.flush()
        self.size += len(string) + 1

        align_header(110 + len(string) + 1) #len(header) + len(string)

    def cpio_header_append_noalign(self, string):
        """
        Append a string to cpio heade, but do not align header + string on
        4 byte boundary.
        """
        self.cstream.write(string)
        self.cstream.write(PADDINGS[3])
        self.cstream.flush()
        self.size += len(string) + 1

    def cpio_header(self, name, filetype, mode, uid, gid, filesize=0,
            major="0", minor="0", nlink = 1):
        mode = int(mode, 8) | int(filetype, 8)
        uid = int(uid, 0)
        gid = int(gid, 0)
        rmajor = int(major, 0)
        rminor = int(minor, 0)
        check = 0
        fmt = MAGIC + ("%08X" * 13)
        self.ino += 1
        data = fmt % (self.ino, mode, uid, gid, nlink, self.mtime, filesize,
                      self.major, self.minor, rmajor, rminor, len(name) + 1,
                      check)

        self.cstream.write(data)
        self.cstream.flush()
        self.size += len(data)

    def cpio_filedata(self, src):
        infile = open(src, "rb")
        self.cstream.write(infile.read())
        self.cstream.flush()
        self.size += os.path.getsize(src)

    def dir(self, name, mode, uid, gid):
        self.cpio_header(name, S_IFDIR, mode, uid, gid, nlink=2)
        self.cpio_header_append(name)

    def slink(self, name, dest, mode, uid, gid):
        self.cpio_header(name, S_IFLNK, mode, uid, gid, len(dest) + 1)
        self.cpio_header_append_noalign(name)
        self.cpio_align()
        self.cpio_header_append_noalign(dest)
        self.cpio_align()

    def file(self, name, src, mode, uid, gid, *links):
        if os.path.exists(src):
            filename = src
            filesize = os.path.getsize(src)
        else:
            filename = os.path.join(os.path.dirname(self.file_name), src)
            if(os.path.exists(filename)):
                filesize = os.path.getsize(filename)
            else:
                raise MergeError("file %s not found- required for initramfs" % filename)

        nlink = len(links) + 1

        self.cpio_header(name, S_IFREG, mode, uid, gid, filesize,
                         nlink = nlink)
        self.cpio_header_append_noalign(name)
        self.cpio_align()
        self.cpio_filedata(filename)
        self.cpio_align()

#         for link in links:
#             self.ino -= 1
#             self.cpio_header(link, S_IFREG, mode, uid, gid, filesize,
#                              nlink = nlink)
#             self.cpio_header_append_noalign(name)
#             self.cpio_align()
#             self.cpio_filedata(src)
#             self.cpio_align()

    def nod(self, name, mode, uid, gid, dev_type, major, minor):
        dev_type = {'c' : S_IFCHR, 'b' : S_IFBLK}[dev_type]
        self.cpio_header(name, dev_type, mode, uid, gid, 0, major, minor)
        self.cpio_header_append(name)

    def pipe(self, name, mode, uid, gid):
        self.cpio_header(name, S_IFIFO, mode, uid, gid)
        self.cpio_header_append(name)

    def sock(self, name, mode, uid, gid):
        self.cpio_header(name, S_IFSOCK, mode, uid, gid)
        self.cpio_header_append(name)

    def cpio_trailer(self):
        self.cpio_header("TRAILER!!!", "0", "0", "0", "0")
        self.cpio_header_append("TRAILER!!!")
        self.cpio_align()

    def cpio_command(self, command, *args):
        handlers = {
            "slink" : self.slink,
            "dir" : self.dir,
            "file" : self.file,
            "nod" : self.nod,
            "pipe" : self.pipe,
            "sock" : self.sock
            }

        #print command, args
        handlers[command](*args)

    def cpio_build(self):
        for line in self.list_file:
            line = line.strip()

            # Skip empty lines of
            if line == "" or line.startswith("#"):
                continue

            parts = split_line(line)
            command = parts[0]
            args = parts[1:]
            self.cpio_command(command, *args)

        self.cpio_trailer()

        self.cstream.flush()
        self.cstream.close()

    def cpio_archive_data(self):
        return self.stream.getvalue()

    def cpio_finalize(self):
        self.stream.close()

def cpio_cmd(args):
    """
    Build a cpio file from a linux-style config file.
    """
    parser = optparse.OptionParser("%prog cpio [options] <input> <output>",
                                   add_help_option=0)
    parser.add_option("-H", "--help", action="help")


    (_, args) = parser.parse_args(args)

    if len(args) != 2:
        parser.error("Invalid number of arguments")

    cpio_builder = CpioBuilder(args[0])
    cpio_file = open(args[1], "wb")

    cpio_builder.cpio_build()
    cpio_file.write(cpio_builder.cpio_archive_data())
    cpio_file.close()
    cpio_builder.cpio_finalize()

register_subcommand('cpio', cpio_cmd,
                    usage = "Build a cpio file from a linux-style config file",
                    is_hidden = True)
