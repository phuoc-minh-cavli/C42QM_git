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
Support for the AMD64 ABI.
"""

import elf.abi
from elf.abi import ElfRelocType, ElfAbi
from elf.constants import EM_X86_64

class AMD64ElfAbi(ElfAbi):
    pass

class AMD64RelocType(ElfRelocType):
    """ElfRelocType for AMD-64 processors. """
    _show = {}

R_X86_64_NONE = AMD64RelocType(0, "NONE")
R_X86_64_64 = AMD64RelocType(1, "64")
R_X86_64_PC32 = AMD64RelocType(2, "PC32")
R_X86_64_GOT32 = AMD64RelocType(3, "GOT32")
R_X86_64_PLT32 = AMD64RelocType(4, "PLT32")
R_X86_64_COPY = AMD64RelocType(5, "COPY")
R_X86_64_GLOB_DAT = AMD64RelocType(6, "GLOB_DAT")
R_X86_64_JUMP_SLOT = AMD64RelocType(7, "JUMP_SLOT")
R_X86_64_RELATIVE = AMD64RelocType(8, "RELATIVE")
R_X86_64_GOTPCREL = AMD64RelocType(9, "GOTPCREL")
R_X86_64_32 = AMD64RelocType(10, "32")
R_X86_64_32S = AMD64RelocType(11, "32S")
R_X86_64_16 = AMD64RelocType(12, "16")
R_X86_64_PC16 = AMD64RelocType(13, "PC16")
R_X86_64_8 = AMD64RelocType(14, "8")
R_X86_64_PC8 = AMD64RelocType(15, "PC8")
R_X86_64_DTPMOD64 = AMD64RelocType(16, "DTPMOD64")
R_X86_64_DTPOFF64 = AMD64RelocType(17, "DTPOFF64")
R_X86_64_TPOFF64 = AMD64RelocType(18, "TPOFF64")
R_X86_64_TLSGD = AMD64RelocType(19, "TLSGD")
R_X86_64_TLSLD = AMD64RelocType(20, "TLSLD")
R_X86_64_DTPOFF32 = AMD64RelocType(21, "DTPOFF32")
R_X86_64_GOTTPOFF = AMD64RelocType(22, "GOTTPOFF")
R_X86_64_TPOFF32 = AMD64RelocType(23, "TPOFF32")
R_X86_64_PC64 = AMD64RelocType(24, "PC64")
R_X86_64_GOTOFF64 = AMD64RelocType(25, "GOTOFF64")
R_X86_64_GOTPC32 = AMD64RelocType(26, "GOTPC32")
R_X86_64_SIZE32 = AMD64RelocType(32, "SIZE32")
R_X86_64_SIZE64 = AMD64RelocType(33, "SIZE64")
R_X86_64_GOTPC32_TLSDESC = AMD64RelocType(34, "GOTPC32_TLSDESC")
R_X86_64_TLSDESC_CALL = AMD64RelocType(35, "TLSDESC_CALL")
R_X86_64_TLSDESC = AMD64RelocType(36, "TLSDESC")

elf.abi.ABI_REGISTRATION[EM_X86_64] = (AMD64ElfAbi, AMD64RelocType)
