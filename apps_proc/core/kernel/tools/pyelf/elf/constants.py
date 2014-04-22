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

#We don't really care about having too few public methods in the classes
#here, so we disable this message.
#pylint: disable-msg=R0903
# We want constants to match the ELF documentation. So we decide to follow
# that naming convention over the regular python conventions.
#pylint: disable-msg=C0103
"""
This module defines a set of constant integers defined by
the various ELF specification and used by the ELF library.
"""

__revision__ = 1.0

from elf.util import IntString

class ElfFormatError(Exception):
    """ElfFormatError is raised when an ELF file is found to be corrupted."""

# ELF class  (Ref: 1-7)
class ElfClass(IntString):
    """IntString for ELF class"""
    _show = {}

ELFCLASSNONE = ElfClass(0, "Invalid Class field")
ELFCLASS32 = ElfClass(1, "ELF32")
ELFCLASS64 = ElfClass(2, "ELF64")

# ELF class  (Ref: 1-7)
class ElfData(IntString):
    """IntString for ELF data encoding field"""
    _show = {}

ELFDATANONE = ElfData(0, "Invalid data encoding")
ELFDATA2LSB = ElfData(1, "2's complement, little endian")
ELFDATA2MSB = ElfData(2, "2's complement, big endian")

class ElfOsabi(IntString):
    """IntString for ELF OS/ABI field"""
    _show = {}

ELFOSABI_NONE = ElfOsabi(0, "No extensions or unspecified")

ELFOSABI_NONE = ElfOsabi(0, "UNIX - System V")
ELFOSABI_HPUX = ElfOsabi(1, "UNIX - HP-UX")
ELFOSABI_NETBSD = ElfOsabi(2, "UNIX - NetBSD")
ELFOSABI_LINUX = ElfOsabi(3, "UNIX - Linux")
ELFOSABI_HURD = ElfOsabi(4, "GNU/HURD")
ELFOSABI_SOLARIS = ElfOsabi(6, "UNIX - Solaris")
ELFOSABI_AIX = ElfOsabi(7, "UNIX - AIX")
ELFOSABI_IRIX = ElfOsabi(8, "UNIX - IRIX")
ELFOSABI_FREEBSD = ElfOsabi(9, "UNIX - FreeBSD")
ELFOSABI_TRUU64 = ElfOsabi(10, "UNIX - TRU64")
ELFOSABI_MODESTO = ElfOsabi(11, "Novell - Modesto")
ELFOSABI_OPENBSD = ElfOsabi(12, "UNIX - OpenBSD")
ELFOSABI_OPENVMS = ElfOsabi(13, "VMS - OpenVMS")
ELFOSABI_NSK = ElfOsabi(14, "HP - Non-stop Kernel")
ELFOSABI_AROS = ElfOsabi(15, "Amiga Research OS")
ELFOSABI_ARM_AEABI = ElfOsabi(64, "ARM AEABI")
ELFOSABI_ARM = ElfOsabi(97, "ARM")
ELFOSABI_STANDALONE = ElfOsabi(255, "Standalone App")

# Object file types (Ref: 1-4)
class ElfType(IntString):
    """IntString for Elf file type"""
    _show = {}

ET_NONE = ElfType(0, "NONE (None)")
ET_REL = ElfType(1, "REL (Relocatable file)")
ET_EXEC = ElfType(2, "EXEC (Executable file)")
ET_DYN = ElfType(3, "DYN (Shared object file)")
ET_CORE = ElfType(4, "CORE (Core file)")
ET_LOOS = 0xfe00
ET_HIOS = 0xfeff
ET_LOPROC = 0xff00
ET_HIPROC = 0xffff

# Machine types (Ref: 1-5)
class ElfMachine(IntString):
    """IntString for ELF machine field"""
    _show = {}
    _default_string = "<unknown>: %x"

EM_NONE         = ElfMachine(0)
EM_M32          = ElfMachine(1, "WE32100")
EM_SPARC        = ElfMachine(2, "Sparc")
EM_386          = ElfMachine(3, "Intel 80386")
EM_68K          = ElfMachine(4, "MC68000")
EM_88K          = ElfMachine(5, "MC88000")
EM_486          = ElfMachine(6, "Intel 80486")
EM_860          = ElfMachine(7, "Intel 80860")
EM_MIPS         = ElfMachine(8, "MIPS R3000")
EM_S370         = ElfMachine(9, "IBM System/370")
EM_MIPS_RS4_BE  = ElfMachine(10, "MIPS R4000 big-endian")
EM_MIPS_RS4_BE  = ElfMachine(10, "MIPS R4000 big-endian")
EM_OLD_SPARCV9	= ElfMachine(11, "Sparc v9 (old)")
EM_PARISC	= ElfMachine(15, "HPPA")

EM_VPP550	= ElfMachine(17)
EM_PPC_OLD      = ElfMachine(17, "Power PC (old)") # Note the clash!

EM_SPARC32PLUS	= ElfMachine(18, "Sparc v8+")
EM_960		= ElfMachine(19, "Intel 90860")
EM_PPC		= ElfMachine(20, "PowerPC")
EM_PPC64	= ElfMachine(21, "PowerPC64")
EM_S390		= ElfMachine(22, "IBM S/390")

EM_V800		= ElfMachine(36, "NEC V800")
EM_FR20		= ElfMachine(37, "Fujitsu FR20")
EM_RH32		= ElfMachine(38, "TRW RH32")
EM_MCORE	= ElfMachine(39, "MCORE")
EM_RCE		= ElfMachine(39)
EM_ARM		= ElfMachine(40, "ARM")
EM_OLD_ALPHA	= ElfMachine(41, "Digital Alpha (old)")
EM_SH		= ElfMachine(42, "Renesas / SuperH SH")
EM_SPARCV9	= ElfMachine(43, "Sparc v9")
EM_TRICORE	= ElfMachine(44, "Siemens Tricore")
EM_ARC		= ElfMachine(45, "ARC")
EM_H8_300	= ElfMachine(46, "Renesas H8/300")
EM_H8_300H	= ElfMachine(47, "Renesas H8/300H")
EM_H8S		= ElfMachine(48, "Renesas H8S")
EM_H8_500	= ElfMachine(49, "Renesas H8/500")
EM_IA_64	= ElfMachine(50, "Intel IA-64")
EM_MIPS_X	= ElfMachine(51, "Stanford MIPS-X")
EM_COLDFIRE	= ElfMachine(52, "Motorola Coldfire")
EM_68HC12	= ElfMachine(53, "Motorola M68HC12")
EM_MMA		= ElfMachine(54, "Fujitsu Multimedia Accelerator")
EM_PCP		= ElfMachine(55, "Siemens PCP")
EM_NCPU		= ElfMachine(56, "Sony nCPU embedded RISC processor")
EM_NDR1		= ElfMachine(57, "Denso NDR1 microprocesspr")
EM_STARCORE	= ElfMachine(58, "Motorola Star*Core processor")
EM_ME16		= ElfMachine(59, "Toyota ME16 processor")
EM_ST100	= ElfMachine(60, "STMicroelectronics ST100 processor")
EM_TINYJ	= ElfMachine(61, "Advanced Logic Corp. TinyJ "\
                             "embedded processor")
EM_X86_64	= ElfMachine(62, "Advanced Micro Devices X86-64")

EM_PDP10	= ElfMachine(64)
EM_PDP11	= ElfMachine(65)
EM_FX66		= ElfMachine(66, "Siemens FX66 microcontroller")
EM_ST9PLUS	= ElfMachine(67, "STMicroelectronics ST9+ 8/16 bit microcontroller")
EM_ST7		= ElfMachine(68, "STMicroelectronics ST7 8-bit microcontroller")
EM_68HC16	= ElfMachine(69, "Motorola MC68HC16 Microcontroller")
EM_68HC11	= ElfMachine(70, "Motorola MC68HC11 Microcontroller")
EM_68HC08	= ElfMachine(71, "Motorola MC68HC08 Microcontroller")
EM_68HC05	= ElfMachine(72, "Motorola MC68HC05 Microcontroller")
EM_SVX		= ElfMachine(73, "Silicon Graphics SVx")
EM_ST19		= ElfMachine(74, "STMicroelectronics ST19 8-bit " \
                             "microcontroller")
EM_VAX		= ElfMachine(75, "Digital VAX")
EM_CRIS		= ElfMachine(76, "Axis Communications 32-bit " \
                             "embedded processor")
EM_JAVELIN	= ElfMachine(77, "Infineon Technologies 32-bit embedded cpu")
EM_FIREPATH	= ElfMachine(78, "Element 14 64-bit DSP processor")
EM_ZSP		= ElfMachine(79, "LSI Logic's 16-bit DSP processor")
EM_MMIX		= ElfMachine(80, "Donald Knuth's educational 64-bit processor")
EM_HUANY	= ElfMachine(81, "Harvard Universitys's machine-independent "\
                             "object format")
EM_PRISM	= ElfMachine(82, "Vitesse Prism")
EM_AVR		= ElfMachine(83, "Atmel AVR 8-bit microcontroller")
EM_FR30		= ElfMachine(84, "Fujitsu FR30")
EM_D10V		= ElfMachine(85, "d10v")
EM_D30V		= ElfMachine(86, "d30v")
EM_V850		= ElfMachine(87, "NEC v850")
EM_M32R		= ElfMachine(88, "Renesas M32R (formerly Mitsubishi M32r)")
EM_MN10300	= ElfMachine(89, "mn10300")
EM_MN10200	= ElfMachine(90, "mn10200")
EM_PJ		= ElfMachine(91, "picoJava")
EM_OPENRISC	= ElfMachine(92, "OpenRISC")
EM_ARC_A5	= ElfMachine(93)
EM_XTENSA	= ElfMachine(94, "Tensilica Xtensa Processor")
EM_PJ_OLD       = ElfMachine(99, "picoJava")
EM_IP2K		= ElfMachine(101, "Ubicom IP2xxx 8-bit microcontrollers")
EM_CR		= ElfMachine(103)
EM_MSP430	= ElfMachine(105)
EM_BLACKFIN	= ElfMachine(106, "Analog Devices Blackfin")
EM_ALTERA_NIOS2	= ElfMachine(113, "Altera Nios II")
EM_CRX		= ElfMachine(114, "National Semiconductor CRX microprocessor")

class ElfFlags(IntString):
    """IntString for ELF Flags field"""
    _show = {}

class ArmFlags(IntString):
    """IntString for Arm Flags field"""
    _show = {}

class MipsFlags(IntString):
    """IntString for Mips Flags field"""
    _show = {}

EF_MIPS_NOREORDER   = MipsFlags(1, "noreorder")
EF_MIPS_PIC         = MipsFlags(2, "pic")
EF_MIPS_CPIC        = MipsFlags(4, "cpic")
EF_MIPS_XGOT        = MipsFlags(8, "xgot")
EF_MIPS_N32         = MipsFlags(0x20, "n32")
EF_MIPS_ABI_O32     = MipsFlags(0x1000, "o32")
EF_MIPS_ABI_O64     = MipsFlags(0x2000, "o64")
EF_MIPS_ARCH        = MipsFlags(0xf0000000L, "")
EF_MIPS_ARCH_1      = MipsFlags(0x00000000L, "mips1")
EF_MIPS_ARCH_2      = MipsFlags(0x10000000L, "mips2")
EF_MIPS_ARCH_3      = MipsFlags(0x20000000L, "mips3")
EF_MIPS_ARCH_4      = MipsFlags(0x30000000L, "mips4")
EF_MIPS_ARCH_5      = MipsFlags(0x40000000L, "mips5")
EF_MIPS_ARCH_32     = MipsFlags(0x50000000L, "mips32")
EF_MIPS_ARCH_64     = MipsFlags(0x70000000L, "mips64")

EF_ARM_RELEXEC        = ArmFlags(0x01, "relocatable executable")
EF_ARM_HASENTRY       = ArmFlags(0x02, "has entry point")

# The remaining ARM flags are defined differently by the ARM and GNU EABIs.
# ARM EABI flags:
EF_ARM_SYMSARESORTED  = ArmFlags(0x04, "symbols are sorted")
EF_ARM_DYNSYMSUSESEGIDX = ArmFlags(0x08, "dynamic symbols use segment index")
EF_ARM_MAPSYMSFIRST   = ArmFlags(0x10, "mapping symbols are first")

# GNU ABI flags:
EF_ARM_INTERWORK      = ArmFlags(0x04, "interworking enabled")
EF_ARM_APCS_26        = ArmFlags(0x08, "uses APCS/26")
EF_ARM_APCS_FLOAT     = ArmFlags(0x10, "uses APCS/float")
EF_ARM_PIC            = ArmFlags(0x20, "position independent")
EF_ARM_ALIGN8         = ArmFlags(0x40, "8 bit structure alignment")
EF_ARM_NEW_ABI        = ArmFlags(0x80, "uses new ABI")
EF_ARM_OLD_ABI        = ArmFlags(0x100, "uses old ABI")
EF_ARM_SOFT_FLOAT     = ArmFlags(0x200, "software FP")
EF_ARM_VFP_FLOAT      = ArmFlags(0x400, "VFP")
EF_ARM_MFP_FLOAT      = ArmFlags(0x800, "MFP")

EF_ARM_BE8          = ArmFlags(0x00800000, "be-8")
EF_ARM_LE8          = ArmFlags(0x00400000, "le-8")

EF_ARM_EABIMASK     = ArmFlags(0xFF000000, "eabimask")
EF_ARM_EABI_UNKNOWN = ArmFlags(0x00000000, "GNU EABI")
EF_ARM_EABI_VER1    = ArmFlags(0x01000000, "Version1 EABI")
EF_ARM_EABI_VER2    = ArmFlags(0x02000000, "Version2 EABI")
EF_ARM_EABI_VER3    = ArmFlags(0x03000000, "Version3 EABI")
EF_ARM_EABI_VER4    = ArmFlags(0x04000000, "Version4 EABI")
EF_ARM_EABI_VER5    = ArmFlags(0x05000000, "Version5 EABI")

EF_IA_64_ABI64      = ElfFlags(0x10, "64-bit")

# Version (Ref: 1-5)
# ELF class  (Ref: 1-7)
class ElfVersion(IntString):
    """IntString for ELF version field"""
    _show = {}

EV_NONE = ElfVersion(0, "Invalid version")
EV_CURRENT = ElfVersion(1, "1 (current)")


class ElfPhType(IntString):
    """IntString for ELF program header types"""
    _show = {}

PT_NULL = ElfPhType(0, "NULL")
PT_LOAD = ElfPhType(1, "LOAD")
PT_DYNAMIC = ElfPhType(2, "DYNAMIC")
PT_INTERP = ElfPhType(3, "INTERP")
PT_NOTE = ElfPhType(4, "NOTE")
PT_SHLIB = ElfPhType(5, "SHLIB")
PT_PHDR = ElfPhType(6, "PHDR")
PT_TLS = ElfPhType(7, "TLS")
PT_LOOS = 0x60000000L
PT_HIOS = 0x6fffffffL
PT_LOPROC = 0x70000000L     # Processor specific types
PT_HIPROC = 0x7fffffffL

# MIPS types
PT_MIPS_REGINFO = ElfPhType(PT_LOPROC + 0, "MIPS_REGINFO")


PT_ARM_EXIDX = ElfPhType(PT_LOPROC + 1, "ARM_EXIDX")
PT_PAX_FLAGS    = ElfPhType(PT_LOOS + 0x5041580L, "PAX_FLAGS")
PT_GNU_EH_FRAME = ElfPhType(PT_LOOS + 0x474e550L, "GNU_EH_FRAME")
PT_GNU_STACK = ElfPhType(PT_LOOS + 0x474e551L, "GNU_STACK")
PT_GNU_RELRO = ElfPhType(PT_LOOS + 0x474e552L, "GNU_RELRO")

PT_IA_64_UNWIND = ElfPhType(PT_LOPROC + 1, "IA_64_UNWIND")


PF_X = (1 << 0)
PF_W = (1 << 1)
PF_R = (1 << 2)
PF_MASKOS = 0x0FF00000L
PF_MASKPROC = 0xF0000000L

class ElfShIndex(IntString):
    """IntString for ELF section indexes"""
    _show = {}
    _default_string = "%3d"

SHN_UNDEF     = ElfShIndex(0, "UND")
SHN_LORESERVE = ElfShIndex(0xff00, "RSV")
SHN_LOPROC    = ElfShIndex(0xff00, "PRC")
SHN_HIPROC    = ElfShIndex(0xff1f, "PRC")
SHN_ABS       = ElfShIndex(0xfff1, "ABS")
SHN_COMMON    = ElfShIndex(0xfff2, "COM")
SHN_HIRESERVE = ElfShIndex(0xffff, "RCV")

class ElfShType(IntString):
    """IntString for ELF section header types"""
    _show = {}

SHT_NULL = ElfShType(0, "NULL")
SHT_PROGBITS = ElfShType(1, "PROGBITS")
SHT_SYMTAB = ElfShType(2, "SYMTAB")
SHT_STRTAB = ElfShType(3, "STRTAB")
SHT_RELA = ElfShType(4, "RELA")
SHT_HASH = ElfShType(5, "HASH")
SHT_DYNAMIC = ElfShType(6, "DYNAMIC")
SHT_NOTE = ElfShType(7, "NOTE")
SHT_NOBITS = ElfShType(8, "NOBITS")
SHT_REL = ElfShType(9, "REL")
SHT_SHLIB = ElfShType(10, "SHLIB")
SHT_DYNSYM = ElfShType(11, "DYNSYM")

SHT_INIT_ARRAY = ElfShType(14, "INIT_ARRAY")
SHT_FINI_ARRAY = ElfShType(15, "FINI_ARRAY")
SHT_GROUP = ElfShType(17, "GROUP")

SHT_LOPROC = 0x70000000L
SHT_HIPROC = 0x7fffffffL
SHT_LOUSER = 0x80000000L
SHT_HIUSER = 0xffffffffL


SHT_ARM_EXIDX = ElfShType(SHT_LOPROC + 1, "ARM_EXIDX")

SHT_ARM_EXIDX = ElfShType(SHT_LOPROC + 1, 'ARM_EXIDX')
SHT_IA_64_UNWIND = ElfShType(SHT_LOPROC + 1, "IA_64_UNWIND")
SHT_MIPS_REGINFO = ElfShType(SHT_LOPROC + 6, "MIPS_REGINFO")

SHT_VERNEED = ElfShType(0x6ffffffe, "VERNEED")
SHT_VERSYM = ElfShType(0x6fffffff, "VERSYM")


SHF_WRITE = 0x1
SHF_ALLOC = 0x2
SHF_EXECINSTR = 0x4

SHF_MERGE = (1 << 4)
SHF_STRINGS = (1 << 5)

SHF_LINK_ORDER = (1 << 7)

SHF_GROUP = (1 << 9)

SHF_MASKOS =   0x0f000000L
SHF_MASKPROC = 0xf0000000L

# OKL4 specfic flag to mark the first section in a segment.
# This is used to help map sections to segments in cases where two
# segments occupy the same virtual address (which occurs in multi-cell
# images because each cell runs in its own SAS).
SHF_OKL4_SEGMENT_START = (1L << 24)
SHF_OKL4_SEGMENT_END = (1L << 25)

STN_UNDEF = 0

class ElfSymbolBinding(IntString):
    """IntString for the ELF Symbol Table Binding."""
    _show = {}
    _default_string = "<unknown>: %d"

STB_LOCAL  = ElfSymbolBinding(0, "LOCAL")
STB_GLOBAL = ElfSymbolBinding(1, "GLOBAL")
STB_WEAK   = ElfSymbolBinding(2, "WEAK")
STB_LOPROC = ElfSymbolBinding(13, "processor specific")
STB_HIPROC = ElfSymbolBinding(15, "processor specific")

class ElfSymbolType(IntString):
    """IntString for the ELF Symbol Table Type."""
    _show = {}
    _default_string = "<unknown>: %d"

STT_NOTYPE  = ElfSymbolType(0, "NOTYPE")
STT_OBJECT  = ElfSymbolType(1, "OBJECT")
STT_FUNC    = ElfSymbolType(2, "FUNC")
STT_SECTION = ElfSymbolType(3, "SECTION")
STT_FILE    = ElfSymbolType(4, "FILE")
STT_LOPROC  = ElfSymbolType(13, "processor specific")
STT_HIPROC  = ElfSymbolType(15, "processor specific")

class ElfSymbolVisibility(IntString):
    """IntString for the Elf Symbol Table Visibility."""
    _show = {}
    _default_string = "<unknown>: %"

STV_DEFAULT   = ElfSymbolVisibility(0, "DEFAULT")
STV_INTERNAL  = ElfSymbolVisibility(1, "INTERNAL")
STV_HIDDEN    = ElfSymbolVisibility(2, "HIDDEN")
STV_PROTECTED = ElfSymbolVisibility(3, "PROTECTED")

class ElfDynamicTag(IntString):
    """IntString for a tag in the Elf dynamic section."""
    _show = {}
    _default_string = "<unknown>: %"

DT_NULL     = ElfDynamicTag(0, "NULL")
DT_NEEDED   = ElfDynamicTag(1, "NEEDED")
DT_PLTRELSZ = ElfDynamicTag(2, "PLTRELSZ")
DT_PLTGOT   = ElfDynamicTag(3, "PLTGOT")
DT_HASH     = ElfDynamicTag(4, "HASH")
DT_STRTAB   = ElfDynamicTag(5, "STRTAB")
DT_SYMTAB   = ElfDynamicTag(6, "SYMTAB")
DT_RELA     = ElfDynamicTag(7, "RELA")
DT_RELASZ   = ElfDynamicTag(8, "RELASZ")
DT_RELAENT  = ElfDynamicTag(9, "RELAENT")
DT_STRSZ    = ElfDynamicTag(10, "STRSZ")
DT_SYMENT   = ElfDynamicTag(11, "SYMENT")
DT_INIT     = ElfDynamicTag(12, "INIT")
DT_FINI     = ElfDynamicTag(13, "FINI")
DT_SONAME   = ElfDynamicTag(14, "SONAME")
DT_RPATH    = ElfDynamicTag(15, "RPATH")
DT_SYMBOLIC = ElfDynamicTag(16, "SYMBOLIC")
DT_REL      = ElfDynamicTag(17, "REL")
DT_RELSZ    = ElfDynamicTag(18, "RELSZ")
DT_RELENT   = ElfDynamicTag(19, "RELENT")
DT_PLTREL   = ElfDynamicTag(20, "PLTREL")
DT_DEBUG    = ElfDynamicTag(21, "DEBUG")
DT_TEXTREL  = ElfDynamicTag(22, "TEXTREL")
DT_JMPREL   = ElfDynamicTag(23, "JMPREL")
DT_BIND_NOW = ElfDynamicTag(24, "BIND_NOW")

DT_LOPROC   = 0x70000000L
DT_HIPROC   = 0x7fffffffL
