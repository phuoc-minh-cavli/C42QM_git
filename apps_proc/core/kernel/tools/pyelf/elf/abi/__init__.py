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

"""
Generic interfaces to ABI specific data.
"""

import glob
import os
import sys
import os.path

__all__ = ['amd64', 'arm', 'ia32', 'ia64', 'common', 'mips', 'arm_linker_script']

class ElfAbi:
    """
    A class for ABI-specific hooks that are called while loading, relocating
    or linking an ELF file.
    """

    def __init__(self, elf):
        self.elf = elf
        self._default_abi_information()

    def _default_abi_information(self):
        """
        Set ABI-specific information to useful defaults.
        """
        pass

    def _arch_for_cpu(self, cpu):
        """
        Returns the arch corresponding to a given CPU, to be used as an
        argument to set_arch().
        """
        raise NotImplementedError

    def set_arch(self, arch):
        """
        Configure ABI-specific settings for a specific sub-architecture
        (eg, armv5).
        """
        raise NotImplementedError

    def set_cpu(self, cpu):
        """
        Configure ABI-specific settings for a specific CPU.
        """
        return self.set_arch(self._arch_for_cpu(cpu))

    def fixup_relocs(self):
        """
        Fix up any ABI-specific relocation issues in the elf file before
        copying sections out of it. This is called before reverting
        relocations for a linked ELF binary, and only called for binaries
        that are relocated.
        """
        pass

class ElfRelocType(long):
    """
    A sub-type of integer that allows you to associate a string with a given
    integer, in addition functions can be provided to overwrite class members.
    """
    _show = {}
    _default_string = None
    verbose = False

    def __new__(cls, arg, string=None, calculate_func=None, revert_func=None,
                allocate_func=None, *args, **kwargs):
        if string:
            cls._show[arg] = (string, calculate_func, revert_func,
                              allocate_func, args, kwargs)
        elif arg not in cls._show:
            raise Exception, "Unknown relocation type %d for class %s" % (arg, cls)
        return long.__new__(cls, arg)

    def __str__(self):
        if long(self) in self._show:
            return self._show[long(self)][0]
        elif self._default_string:
            return self._default_string % long(self)
        else:
            return long.__str__(self)

    def calculate(self, elf, sect, symtab, addend, offset, symdx):
        """
        Calculate the value to be written out by this relocation, returns the
        value to be written out or None if we aren't able to handle the
        relocation.
        """
        if long(self) in self._show:
            if self._show[long(self)][1]:
                if self.verbose:
                    print "\t\tCalling calculate function", \
                            self._show[long(self)][1]

                return self._show[long(self)][1](elf, sect, symtab, addend,
                                                 offset, symdx,
                                                 *(self._show[long(self)][4]),
                                                 **(self._show[long(self)][5]))
            else:
                print "No calculate function implemented for relocation type %s" % str(self)
                return None
        else:
            raise Exception, "It should not be possible for this object to exist!"

    def revert(self, elf, sect, symtab, addend, offset, symdx):
        """
        Revert an applied relocation, this function acts in a similar manner
        to calculate but should return what the original contents of the word
        were prior to the relocation being applied in the first place.
        """
        if long(self) in self._show:
            if self._show[long(self)][2]:
                if self.verbose:
                    print "\t\tCalling revert function", \
                            self._show[long(self)][2]

                return self._show[long(self)][2](elf, sect, symtab, addend,
                                                 offset, symdx,
                                                 *(self._show[long(self)][4]),
                                                 **(self._show[long(self)][5]))
            else:
                print "No revert function implemented for relocation type %s" % str(self)
                return None
        else:
            raise Exception, "It should not be possible for this object to exist!"

    def allocate(self, elf, sect, symtab, offset, symdx, relocs):
        """
        Perform any allocations that this relocation might require for e.g.
        entries in the .got table.
        """
        if long(self) in self._show:
            if self._show[long(self)][3]:
                if self.verbose:
                    print "\t\tCalling allocation function", \
                            self._show[long(self)][3]

                return self._show[long(self)][3](elf, sect, symtab, offset,
                                                 symdx, relocs,
                                                 *(self._show[long(self)][4]),
                                                 **(self._show[long(self)][5]))
            elif self.verbose:
                print "\t\tNo allocation function provided"
        else:
            raise Exception, "It should not be possible for this object to exist!"

# Relocation types are architecture specific.

ABI_REGISTRATION = {}
KERNEL_SOC_LINKER_SCRIPT = {}
