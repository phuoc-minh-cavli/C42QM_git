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
A namespace for objects declared in the XML config file and used in
the object environment.

Namespaces are hierarchical and have POSIX-like semantics.
"""

from weaver import MergeError

class NameSpaceObject(object):
    """
    An optional base class for objects in the namespace.  These objects know
    their name and can generate their absolute pathname.
    """

    def __init__(self, parent, name = None, representing = None):
        self.parent = parent
        self.name = name
        self.representing = representing

    def __str__(self):
        if self.representing is not None:
            return str(self.representing)
        else:
            return "Namespace"

    def abs_name(self):
        """Return the absolute name of the object."""
        assert self.parent is not None
        assert self.name is not None

        return self.parent.abs_path(self.name)

class ObjectNameSpace(NameSpaceObject):
    """
    A namespace for objects declared in the XML config file and used in
    the object environment.

    Namespaces are hierarchical and have POSIX-like semantics.
    """

    SEPARATOR = '/'

    def __init__(self, parent, name, representing = None):
        NameSpaceObject.__init__(self, parent, name,
                                 representing = representing)

        # If parent is None, then this namespace is the top of the tree.
        if parent is None:
            self.parent = self
            root = self
        else:
            root = parent.root

        self.root = root
        self.symbols = {}
        self.default = 'master'

    def set_default_name(self, default):
        """
        Set the name of the default object that should be returned
        if the namespace itself is referred to.
        """
        # Entry must exist.
        assert self.symbols.get(self.default, None) is not None

        self.default = default

    def get_namespace(self, name):
        """
        Return the named child namespace.  Search is non-recursive.

        If the name doesn't refer to a namespace, return None.
        """
        obj = self.symbols.get(name)

        if isinstance(obj, ObjectNameSpace):
            return obj

        return None

    def add(self, name, the_object):
        """
        Add an object to the namespace under the given name if the
        name isn't already in use.
        """
        assert name is not None

        if name in self.symbols:
            raise MergeError, \
                  'Namespace %s: name "%s" already in use.' % \
                  (self.abs_name(), name)

        self.symbols[name] = the_object

    def add_namespace(self, name, rep_obj = None):
        """Add a new sub-namespace to this namespace."""
        assert name is not None

        new_namespace = ObjectNameSpace(parent = self, name = name,
                                        representing = rep_obj)
        self.add(name, new_namespace)

        return new_namespace

    def namei(self, parts):
        """
        Recursively move the list of names, looking for the  target.

        Historical Note: This function is named after a similar
        function in the UNIX v6 kernel.
        """

        if len(parts) == 0:
            # If the name refers to a namespace, return the default
            # right if present.
            return self.symbols.get(self.default)
        elif parts[0] == '.':
            return self.namei(parts[1:])
        elif parts[0] == '..':
            return self.parent.namei(parts[1:])
        elif parts[0] in self.symbols:
            obj = self.symbols[parts[0]]

            if isinstance(obj, ObjectNameSpace):
                return obj.namei(parts[1:])
            elif len(parts) == 1:
                return obj
            else:
                return None
        else:
            if len(parts) == 1 and '__dynamic' in self.symbols:
                (dyn_func,obj) =  self.symbols['__dynamic']

                return dyn_func(parts[0], obj)

            return None

    def lookup(self, name):
        """
        Lookup a name in the namespace.  If found the object will be
        returned, otherwise None.
        """
        # Convert the string into a list of parts.  Absolute paths
        # have an empty string as the first element.
        parts = name.split(ObjectNameSpace.SEPARATOR)

        # If an absolute path, then search from the root of the
        # namespace, otherwise search from the current namespace.
        if parts[0] == '':
            start = self.root
            parts = parts[1:]
        else:
            start = self

        return start.namei(parts)

    def abs_path(self, name):
        """
        Return the absolute path of a name in the current namespace.
        Returns None if the name does not exist.
        """
        if name != '.':
            if name not in self.symbols:
                return None
            path = [name]
        else:
            path = []

        node = self
        parent = self.parent

        while parent is not node:
            for (dir_name, obj) in parent.symbols.items():
                if obj is node:
                    path = [dir_name] + path
                    break

            node = parent
            parent = node.parent

        return self.SEPARATOR + self.SEPARATOR.join(path)

    def dump(self):
        """
        Return the contents of the namespace, and sub-namespaces, as a list of
        humam readable strings.  The contents are returned in alphabetical
        order.
        """
        txt = []

        for (name, sym) in sorted(self.symbols.items()):
            # Mark the namespace's default name
            if name is self.default:
                default_str = '(*)'
            else:
                default_str = ''

            txt.append("%s%s: %s" % (self.abs_path(name), default_str, sym))

            # Recurse if the name points to a namespace.
            if isinstance(sym, ObjectNameSpace):
                txt.extend(sym.dump())

        return txt
