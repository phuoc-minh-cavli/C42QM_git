##############################################################################
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
# We disable the warning about a class having too many public methods
# because for this class we are sure it is a good thing.
#pylint: disable-msg=R0904
"""
This module provides the File class.
"""

__revision__ = 0.1

import stat
import os
from elf.ByteArray import ByteArray

class File(file):
    """
    File extends the in-built python "file" type to give it some
    useful extra functions. In particular is support a convenient way
    of getting stat-like data out of it. E.g: File.size()
    """

    def stat(self):
        """Return stat tuple."""
        return os.fstat(self.fileno())

    def inode_mode(self):
        """Inode protection mode."""
        return self.stat()[stat.ST_MODE]

    def inode(self):
        """Inode number."""
        return self.stat()[stat.ST_INO]

    def device(self):
        """Device inode resides on"""
        return self.stat()[stat.ST_DEV]

    def num_links(self):
        """Number of links to the inode"""
        return self.stat()[stat.ST_NLINK]

    def uid(self):
        """User id of the owner"""
        return self.stat()[stat.ST_UID]

    def gid(self):
        """Group id of the owner"""
        return self.stat()[stat.ST_GID]

    def size(self):
        """
        Size in bytes of a plain file; amount of data
        waiting on some special files.
        """
        return self.stat()[stat.ST_SIZE]

    def atime(self):
        """Time of last access."""
        return self.stat()[stat.ST_ATIME]

    def mtime(self):
        """Time of last modification."""
        return self.stat()[stat.ST_MTIME]

    def ctime(self):
        """The ``ctime'' as reported by the operating system."""
        return self.stat()[stat.ST_CTIME]

    def is_dir(self):
        """Return true if the file is a directory."""
        return stat.S_ISDIR(self.inode_mode())

    def is_character(self):
        """Return true if the file is a character device."""
        return stat.S_ISDIR(self.inode_mode())

    def is_block(self):
        """Return true if the file is a block device."""
        return stat.S_ISDIR(self.inode_mode())

    def is_device(self):
        """
        Return true if the file is a device node.
        (Either chararacter, or block).
        """
        return self.is_block() or self.is_character()

    def is_regular(self):
        """Return true if the file is a regular file."""
        return stat.S_ISREG(self.inode_mode())

    def is_fifo(self):
        """Return true if the file is a FIFO."""
        return stat.S_ISFIFO(self.inode_mode())

    def is_symlink(self):
        """Return true if the file is a symbolic link."""
        return stat.S_ISLNK(self.inode_mode())

    def is_socket(self):
        """Return true if the file is a socket."""
        return stat.S_ISSOCK(self.inode_mode())

    def get_data(self, base, size):
        """
        Get some size bytes of data starting at base.
        base must be > 0.
        """
        assert 'b' in self.mode
        self.seek(base)
        data = ByteArray()
        data.fromfile(self, size)
        return data
