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
Support for the IA64 ABI.
"""

import elf.abi
from elf.abi import ElfRelocType, ElfAbi
from elf.constants import EM_IA_64

class IA64ElfAbi(ElfAbi):
    pass

class IA64RelocType(ElfRelocType):
    """ElfRelocType for IA-64 processors. """
    _show = {}

R_IA_64_NONE = IA64RelocType(0, "NONE")
R_IA_64_IMM14 = IA64RelocType(0x21, "IMM14")
R_IA_64_IMM22 = IA64RelocType(0x22, "IMM22")
R_IA_64_IMM64 = IA64RelocType(0x23, "IMM64")
R_IA_64_DIR32MSB = IA64RelocType(0x24, "DIR32MSB")
R_IA_64_DIR32LSB = IA64RelocType(0x25, "DIR32LSB")
R_IA_64_DIR64MSB = IA64RelocType(0x26, "DIR64MSB")
R_IA_64_DIR64LSB = IA64RelocType(0x27, "DIR64LSB")
R_IA_64_GPREL22 = IA64RelocType(0x2a, "GPREL22")
R_IA_64_GPREL64I = IA64RelocType(0x2b, "GPREL64I")
R_IA_64_GPREL32MSB = IA64RelocType(0x2c, "GPREL32MSB")
R_IA_64_GPREL32LSB = IA64RelocType(0x2d, "GPREL32LSB")
R_IA_64_GPREL64MSB = IA64RelocType(0x2e, "GPREL64MSB")
R_IA_64_GPREL64LSB = IA64RelocType(0x2f, "GPREL64LSB")
R_IA_64_LTOFF22 = IA64RelocType(0x32, "LTOFF22")
R_IA_64_LTOFF64I = IA64RelocType(0x33, "LTOFF64I")
R_IA_64_PLTOFF22 = IA64RelocType(0x3a, "PLTOFF22")
R_IA_64_PLTOFF64I = IA64RelocType(0x3b, "PLTOFF64I")
R_IA_64_PLTOFF64MSB = IA64RelocType(0x3e, "PLTOFF64MSB")
R_IA_64_PLTOFF64LSB = IA64RelocType(0x3f, "PLTOFF64LSB")
R_IA_64_FPTR64I = IA64RelocType(0x43, "FPTR64I")
R_IA_64_FPTR32MSB = IA64RelocType(0x44, "FPTR32MSB")
R_IA_64_FPTR32LSB = IA64RelocType(0x45, "FPTR32LSB")
R_IA_64_FPTR64MSB = IA64RelocType(0x46, "FPTR64MSB")
R_IA_64_FPTR64LSB = IA64RelocType(0x47, "FPTR64LSB")
R_IA_64_PCREL60B = IA64RelocType(0x48, "PCREL60B")
R_IA_64_PCREL21B = IA64RelocType(0x49, "PCREL21B")
R_IA_64_PCREL21M = IA64RelocType(0x4a, "PCREL21M")
R_IA_64_PCREL21F = IA64RelocType(0x4b, "PCREL21F")
R_IA_64_PCREL32MSB = IA64RelocType(0x4c, "PCREL32MSB")
R_IA_64_PCREL32LSB = IA64RelocType(0x4d, "PCREL32LSB")
R_IA_64_PCREL64MSB = IA64RelocType(0x4e, "PCREL64MSB")
R_IA_64_PCREL64LSB = IA64RelocType(0x4f, "PCREL64LSB")
R_IA_64_LTOFF_FPTR22 = IA64RelocType(0x52, "LTOFF_FPTR22")
R_IA_64_LTOFF_FPTR64I = IA64RelocType(0x53, "LTOFF_FPTR64I")
R_IA_64_LTOFF_FPTR32MSB = IA64RelocType(0x54, "LTOFF_FPTR32MSB")
R_IA_64_LTOFF_FPTR32LSB = IA64RelocType(0x55, "LTOFF_FPTR32LSB")
R_IA_64_LTOFF_FPTR64MSB = IA64RelocType(0x56, "LTOFF_FPTR64MSB")
R_IA_64_LTOFF_FPTR64LSB = IA64RelocType(0x57, "LTOFF_FPTR64LSB")
R_IA_64_SEGREL32MSB = IA64RelocType(0x5c, "SEGREL32MSB")
R_IA_64_SEGREL32LSB = IA64RelocType(0x5d, "SEGREL32LSB")
R_IA_64_SEGREL64MSB = IA64RelocType(0x5e, "SEGREL64MSB")
R_IA_64_SEGREL64LSB = IA64RelocType(0x5f, "SEGREL64LSB")
R_IA_64_SECREL32MSB = IA64RelocType(0x64, "SECREL32MSB")
R_IA_64_SECREL32LSB = IA64RelocType(0x65, "SECREL32LSB")
R_IA_64_SECREL64MSB = IA64RelocType(0x66, "SECREL64MSB")
R_IA_64_SECREL64LSB = IA64RelocType(0x67, "SECREL64LSB")
R_IA_64_REL32MSB = IA64RelocType(0x6c, "REL32MSB")
R_IA_64_REL32LSB = IA64RelocType(0x6d, "REL32LSB")
R_IA_64_REL64MSB = IA64RelocType(0x6e, "REL64MSB")
R_IA_64_REL64LSB = IA64RelocType(0x6f, "REL64LSB")
R_IA_64_LTV32MSB = IA64RelocType(0x74, "LTV32MSB")
R_IA_64_LTV32LSB = IA64RelocType(0x75, "LTV32LSB")
R_IA_64_LTV64MSB = IA64RelocType(0x76, "LTV64MSB")
R_IA_64_LTV64LSB = IA64RelocType(0x77, "LTV64LSB")
R_IA_64_PCREL21BIa = IA64RelocType(0x79, "PCREL21BIa")
R_IA_64_PCREL22 = IA64RelocType(0x7A, "PCREL22")
R_IA_64_PCREL64I = IA64RelocType(0x7B, "PCREL64I")
R_IA_64_IPLTMSB = IA64RelocType(0x80, "IPLTMSB")
R_IA_64_IPLTLSB = IA64RelocType(0x81, "IPLTLSB")
R_IA_64_SUB = IA64RelocType(0x85, "SUB")
R_IA_64_LTOFF22X = IA64RelocType(0x86, "LTOFF22X")
R_IA_64_LDXMOV = IA64RelocType(0x87, "LDXMOV")
R_IA_64_TPREL14 = IA64RelocType(0x91, "TPREL14")
R_IA_64_TPREL22 = IA64RelocType(0x92, "TPREL22")
R_IA_64_TPREL64I = IA64RelocType(0x93, "TPREL64I")
R_IA_64_TPREL64MSB = IA64RelocType(0x96, "TPREL64MSB")
R_IA_64_TPREL64LSB = IA64RelocType(0x97, "TPREL64LSB")
R_IA_64_LTOFF_TPREL22 = IA64RelocType(0x9A, "LTOFF_TPREL22")
R_IA_64_DTPMOD64MSB = IA64RelocType(0xA6, "DTPMOD64MSB")
R_IA_64_DTPMOD64LSB = IA64RelocType(0xA7, "DTPMOD64LSB")
R_IA_64_LTOFF_DTPMOD22 = IA64RelocType(0xAA, "LTOFF_DTPMOD22")
R_IA_64_DTPREL14 = IA64RelocType(0xB1, "DTPREL14")
R_IA_64_DTPREL22 = IA64RelocType(0xB2, "DTPREL22")
R_IA_64_DTPREL64I = IA64RelocType(0xB3, "DTPREL64I")
R_IA_64_DTPREL32MSB = IA64RelocType(0xB4, "DTPREL32MSB")
R_IA_64_DTPREL32LSB = IA64RelocType(0xB5, "DTPREL32LSB")
R_IA_64_DTPREL64MSB = IA64RelocType(0xB6, "DTPREL64MSB")
R_IA_64_DTPREL64LSB = IA64RelocType(0xB7, "DTPREL64LSB")
R_IA_64_LTOFF_DTPREL22 = IA64RelocType(0xBA, "LTOFF_DTPREL22")

elf.abi.ABI_REGISTRATION[EM_IA_64] = (IA64ElfAbi, IA64RelocType)
