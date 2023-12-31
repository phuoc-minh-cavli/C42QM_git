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

"""Binary representations of linux atag environment entries."""

from pyokl4.autogen import Autogen
from weaver.util import _0
import weaver.weavertypes

# FIXME: Jira ticket OLD-1768 - jamesp.
# Atags identifers should be autogenerated
class ATag:
    def __init__(self, tag, size=None):
        # Note: size is in words
        self.tag = tag
        self.size = size

        # for convenience
        self.word_size = Autogen.types.OKL4_word_t._type_size
        self.endianess = weaver.weavertypes._ELFWEAVER_TYPES_ENDIANESS

    def write_tag(self, section):
        section.write_word(self.size)
        section.write_word(self.tag)

    def get_size(self):
        return self.size * self.word_size

class ATagNone(ATag):
    def __init__(self):
        ATag.__init__(self, 0x00000000, 0)

    def get_size(self):
        # ATAG_NONE is special: size field is 0 but actual size is 2 words
        return 2 * self.word_size

class ATagCore(ATag):
    def __init__(self, flags, pagesize, rootdev):
        ATag.__init__(self, 0x54410001, 5)
        self.flags = flags
        self.pagesize = pagesize
        self.rootdev = rootdev

    def write_tag(self, section):
        ATag.write_tag(self, section)
        section.write_word(self.flags)
        section.write_word(self.pagesize)
        section.write_word(self.rootdev)

class ATagMem(ATag):
    def __init__(self, phys_size, phys_start):
        ATag.__init__(self, 0x54410002, 4)
        self.phys_size = phys_size
        self.phys_start = phys_start

    def write_tag(self, section):
        ATag.write_tag(self, section)
        section.write_word(self.phys_size)
        section.write_word(self.phys_start)

class ATagCmdLine(ATag):
    def __init__(self, cmdline):
        ATag.__init__(self, 0x54410009)
        self.cmdline = cmdline
        sz = len(cmdline + "\0")
        self.padding = 0
        while (sz % self.word_size != 0):
            sz += 1
            self.padding += 1
        self.size = 2 + (sz / self.word_size)

    def write_tag(self, section):
        ATag.write_tag(self, section)
        section.write_strings([self.cmdline])
        section.write_bytes(0, self.padding)

class ATagChannel(ATag):
    def __init__(self, name, segment=None, attrs=None, channel=None,
			entry=None):
        ATag.__init__(self, 0x99990000)
        self.name = name
        self.segment = segment
        self.attrs = attrs
        self.channel = channel
        self.entry = entry

        sz = len(self.name + "\0")
        self.padding = 0
        while (sz % self.word_size != 0):
            sz += 1
            self.padding += 1
        self.size = 7 + (sz / self.word_size)

    def write_tag(self, section):
        ATag.write_tag(self, section)

        if self.segment is not None:
            assert self.channel is not None

            if self.segment.okl4_object is not None:
                section.write_word(self.attrs.phys_addr)
            else:
                section.write_word(0)

            section.write_word(self.attrs.size)

            if self.channel.okl4_object is not None:
                section.write_word(self.channel.okl4_object._cap_id)
                section.write_word(self.channel.message_size)
                section.write_word(self.channel.max_messages)
            else:
                section.write_word(Autogen.types.KCAP_INVALID)
                section.write_word(0)
                section.write_word(0)
        else:
            assert self.entry is not None

            section.write_word(0)
            section.write_word(0)

            cap = self.entry.dest_cap.get_cap_value()
            if cap is not None:
                section.write_word(cap)
                section.write_word(self.entry.right.object.message_size)
                section.write_word(self.entry.right.object.max_messages)
            else:
                section.write_word(Autogen.types.KCAP_INVALID)
                section.write_word(0)
                section.write_word(0)

        section.write_strings([self.name])
        section.write_bytes(0, self.padding)

class ATagSharedBuffer(ATag):
    def __init__(self, name, segment=None, attrs=None, entry=None):
        ATag.__init__(self, 0x99990001)
        self.name = name
        self.segment = segment
        self.attrs = attrs
        self.entry = entry

        sz = len(self.name + "\0")
        self.padding = 0
        while (sz % self.word_size != 0):
            sz += 1
            self.padding += 1
        self.size = 4 + (sz / self.word_size)

    def write_tag(self, section):
        ATag.write_tag(self, section)

        if self.segment is not None:
            if self.attrs.phys_addr is not None:
                section.write_word(self.attrs.phys_addr)
            else:
                section.write_word(0)

            section.write_word(self.attrs.size)

        else:
            if self.entry.right is not None:
                assert len(self.entry.right.env_entries) == 1

                # FIXME: Jira ticket MARVIN-247 - jamesp.
                # Somewhat evil.  We need to get the size of the segment but
                # this data is not actually present in the okl4 segment object
                # (to which this cap entry points).   So, since we are reusing
                # the segment linking code we look into the copied
                # CellEnvSegment object (attached to the cap) which stored the
                # attrs containing the size that the segment was originally
                # created with.  When the need for copying the env entries is
                # removed (see bug) this should be reworked to something less
                # terrible.
                attrs = self.entry.right.env_entries.items()[0][1].attrs
                if attrs.phys_addr is not None:
                    section.write_word(attrs.phys_addr)
                else:
                    section.write_word(0)

                section.write_word(attrs.size)
            else:
                section.write_word(0)
                section.write_word(0)

        section.write_strings([self.name])
        section.write_bytes(0, self.padding)

class ATagVirq(ATag):
    def __init__(self, name, irq_obj):
        ATag.__init__(self, 0x99990002)
        self.irq_obj = irq_obj
        self.name = name

        sz = len(self.name + "\0")
        self.padding = 0
        while (sz % self.word_size != 0):
            sz += 1
            self.padding += 1
        self.size = 3 + (sz / self.word_size)

    def write_tag(self, section):
        ATag.write_tag(self, section)
        section.write_word(_0(self.irq_obj.irq))
        section.write_strings([self.name])
        section.write_bytes(0, self.padding)

class ATagVirqLine(ATag):
    def __init__(self, name, line_obj):
        ATag.__init__(self, 0x99990003)
        self.line_obj = line_obj
        self.name = name

        sz = len(self.name + "\0")
        self.padding = 0
        while (sz % self.word_size != 0):
            sz += 1
            self.padding += 1
        self.size = 3 + (sz / self.word_size)

    def write_tag(self, section):
        ATag.write_tag(self, section)
        section.write_word(_0(self.line_obj.cap_id))
        section.write_strings([self.name])
        section.write_bytes(0, self.padding)

class ATagVClient(ATag):
    def __init__(self, name, channel):
        ATag.__init__(self, 0x99990004)
        self.name = name
        self.channel = channel

        sz = len(self.name + "\0")
        self.padding = 0
        while (sz % self.word_size != 0):
            sz += 1
            self.padding += 1
        self.size = 7 + (sz / self.word_size)

    def write_tag(self, section):
        ATag.write_tag(self, section)

        cap = self.channel.dest_cap.get_cap_value()
        if cap is not None:
            section.write_word(cap)
            section.write_word(self.channel.right.object.message_size)
            section.write_word(self.channel.right.object.max_messages)
        else:
            section.write_word(Autogen.types.KCAP_INVALID)
            section.write_word(0)
            section.write_word(0)

        section.write_word(_0(self.channel.virq_in.irq))
        section.write_word(_0(self.channel.virq_out.irq))

        section.write_strings([self.name])
        section.write_bytes(0, self.padding)

# ATAG_RAMDISK should also be implemented
class ATagInitrd2(ATag):
    def __init__(self, attrs):
        ATag.__init__(self, 0x54420005, 4)

        self.attrs = attrs

    def write_tag(self, section):
        ATag.write_tag(self, section)
        section.write_word(_0(self.attrs.phys_addr))
	section.write_word(self.attrs.size)
