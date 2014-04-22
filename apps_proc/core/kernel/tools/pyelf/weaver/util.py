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

import os
import sys
import re

from weaver import MergeError
from weaver.allocator import Allocator

def _0(n):
    """
    Return 0 if n is None, otherwise n.

    This function can be called before image.layout() and
    therefore addresses and counts can contain Nones.  In
    those circumstances the value doesn't matter (it won't be
    written to a file), so use 0 instead.
    """
    if n is None or n is Allocator.IGNORE:
        return 0
    else:
        return n

def get_symbol(elf, symbol, may_not_exist = False):
    sym = elf.find_symbol(symbol)

    if not sym:
        if may_not_exist:
            return None
        else:
            raise MergeError, "Symbol %s not found" % symbol

    return sym

def get_symbol_addr(elf, symbol, may_not_exist = False):
    sym = elf.find_symbol(symbol)

    if not sym:
        if may_not_exist:
            return None
        else:
            raise MergeError, "Symbol %s not found" % symbol

    address = sym.value
    bytes = sym.size

    if bytes == 0:
        bytes = elf.wordsize / 8

    return (address, bytes)

def get_symbol_value(elf, symbol):
    """Given a symbol name, return it's contents."""
    sym = elf.find_symbol(symbol)

    size = [elf.wordsize/8, sym.size][sym.size != 0]
    if sym:
        return elf.get_value(sym.value, size)
    else:
        return None

def next_power_of_2(n):
    """Return the next power of two greater than of equal to 'n'."""
    result = 1
    while (result <= n):
        result *= 2
    return result

def log2(n):
    """Return a value 'm' such that 2^m >= 'n'."""
    m = 0
    size = 1
    while (n > size):
        size *= 2
        m += 1
    return m

def arch_import(lookup_table, name, cpu):
    """
    Given an architecture lookup table, lookup the given cpu
    and import "weaver.<name>_<arch>"
    """

    if cpu not in lookup_table:
        raise MergeError, \
              "Unable to determine architecture of cpu: %s" % cpu
    arch = lookup_table[cpu]
    try:
        return getattr(__import__("weaver.%s_%s" % (name, arch)),
                       ("%s_%s" % (name, arch)))
    except ImportError:
        return __import__("%s_%s" % (name, arch))

def import_custom(path, package, ending):
    """
    Import all files with a given ending from a package at a given path.
    """
    if not path:
        return
    old_path = sys.path

    sys.path = [path]
    for _, _, files in os.walk(os.path.join(path, package)):
        mods = [mod[:-3] for mod in files if mod.endswith(ending)]

        for mod in mods:
            __import__(package + "." + mod)

    sys.path = old_path

def replace_illegal_chars(name):
    """Replace all illegal identifiers with underscores."""
    valid = re.compile(r'^[a-zA-Z0-9_]$')
    return "".join(map(lambda x: [x, '_'][valid.match(x) is None], name))

def check_file_exists(file, relpath=None):
    """
    Check if the file 'filename' exists or not.

    This function checks the existence of the file in the following order.
    -> if filename exists relative to relpath
    -> if filename is absolute path to a file
    -> if filename exists relative to current working directory
    -> if filename exists relative to the OKL4_SDK directory
    -> in the current working directory

    Raises merge error if the file does not exist or is not a regular file.
    In the case where the file does not exist, a warining is issued
    if the OKL4_SDK environment variable is not set.

    Returns the path to the file.
    """
    filename = None
    error_string = "File %s is missing, does not exist:\n" % file

    #search relative to relpath
    if relpath is not None:
        if os.path.exists(os.path.join(relpath, file)):
            filename = os.path.join(relpath, file)
        else:
            error_string += "  under relative path %s\n" % relpath

    #search absolutely
    if not filename and os.path.exists(file):
        filename = file
    else:
        error_string += "  as an absolute path\n"

    sdk_base = os.environ.get("OKL4_SDK")

    if sdk_base is None:
        error_string +="  and OKL4_SDK is not set\n"
    else:
        #search in the SDK base
        if not filename and os.path.exists(os.path.join(sdk_base, file)):
            filename = os.path.join(sdk_base, file)
        else:
            error_string +="  under OKL4_SDK %s\n"% sdk_base

        #search relative to SDK base
        if relpath is not None:
            if not filename and os.path.exists(os.path.join(sdk_base, relpath, file)):
                filename = os.path.join(sdk_base, relpath, file)
            else:
                error_string +="  relative OKL4_SDK %s\n" % os.path.join(sdk_base, relpath)

    if filename is None:
        raise MergeError(error_string)

    if not os.path.isfile(filename):
        raise MergeError("File %s is not a regular file" % filename)

    return filename
