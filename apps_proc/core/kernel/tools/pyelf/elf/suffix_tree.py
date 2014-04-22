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
#pylint: disable-msg=W0212
# we have a recursive, private method so disable warning about accessing a
# _method

"""
This module implements a SuffixTree class. This data structure is an efficient
way of keeping track of a set of strings where some are the suffix of others.

In particular, this could be useful for representing an ELF string table, in
which the efficient packing of data can be achieved by taking advantage of
suffix relations.
"""

class SuffixTree(object):
    """
    Suffix trees efficiently store sets of words. Futhermore, they can
    efficiently produce a list of strings such that every word in the set
    if the suffix of one of the words in the list (along with index information
    (index, offset) to identify the location of each word in the list.
    """

    def __init__(self):
        """Create an empty SuffixTree node."""
        self.children = {}
        self.start = False

    def add(self, string):
        """ Add a string to the suffix tree."""
        last_char = string[-1]
        if last_char not in self.children:
            self.children[last_char] = SuffixTree()
        if len(string) == 1:
            self.children[last_char].start = True
        else:
            self.children[last_char].add(string[:-1])

    def remove(self, string):
        """ Remove a string from the suffix tree."""
        if string:
            char = string[-1]
            child = self.children[char]
            child.remove(string[:-1])
            if not child.start and not child.children:
                del self.children[char]
        else:
            self.start = False

    def __contains__(self, string):
        """ Test whether a given string is in the suffix tree."""
        if string:
            char = string[-1]
            if char in self.children:
                return string[:-1] in self.children[char]
            else:
                return False
        else:
            return self.start

    def to_strings(self):
        """
        Return a list of strings as well as a dictionary
        of string -> (index, index)
        """
        strings, indices, suffix = [], {}, ""
        return self._to_strings(strings, indices, suffix)

    def _to_strings(self, strings, indices, suffix):
        """
        Recursively find the values required for to_strings().
        """
        if self.children:
            for child, subtree in self.children.items():
                strings, indices = subtree._to_strings(strings,
                                                       indices,
                                                       child + suffix)
        else:
            assert self.start
            strings.append(suffix)
        if self.start:
            indices[suffix] = len(strings) - 1, len(strings[-1]) - len(suffix)
        return strings, indices

    def to_nulled_strings(self):
        """
        Return a null separated string and the associated "symbol table"
        representation (string -> offset) for this tree.
        """
        strings, indices = self.to_strings()
        cum_indices = []
        cum_index = 0
        for string in strings:
            cum_indices.append(cum_index)
            cum_index += len(string) + 1

        pos_dict = dict(enumerate(cum_indices))
        for k in indices:
            index, offset = indices[k]
            indices[k] = pos_dict[index] + offset
        return "\0".join(strings), indices


def _main():
    """
    Rudimentary test code.
    """
    tree = SuffixTree()
    tree.add("foo")
    tree.add("poo")
    tree.add("bar")
    tree.add("asjskfhlksdjfghsldkfjgsldkfjg")
    assert "foo" in tree
    assert "poo" in tree
    assert "bar" in tree
    assert "food" not in tree
    assert "oo" not in tree
    tree.add("oo")
    tree.add("asd")
    tree.add("asdasd")
    assert "oo" in tree
    assert "asjskfhlksdjfghsldkfjgsldkfjg" in tree
    print tree.to_strings()

    tree.remove("oo")
    assert "oo" not in tree
    assert "foo" in tree
    tree.remove("asdasd")
    assert "asdasd" not in tree
    assert "asd" in tree

    print tree.to_strings()

    nulled_string, symbol_table = tree.to_nulled_strings()
    for symbol, offset in symbol_table.items():
        assert symbol == nulled_string[offset:].split("\0")[0]

if __name__ == '__main__':
    _main()
