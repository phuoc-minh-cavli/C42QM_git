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
Classes for sharing caps between domains.
"""

#
# The cap design below is a little confusing, but there is method in
# the madness.
#
# The problem to be solved is how to give one domain certain access
# rights to an object in another domain, and ensure that the first
# domain has no other rights and that those rights cannot be given (or
# stolen) to another domain.
#
# At the XML, the solution looks like this:
#
# <cell name="src" >
#     <program name="prog1" >
#         <thread name="helper" />
#         <memsection name="heap_seg" size="4K" />
#     </program>
# </cell>
# <cell name="dest" >
#     <environment>
#         <entry key="HELPER_THREAD" cap="/src/prog1/helper" />
#         <entry key="HEAP_SEG" cap="/src/prog1/heap_seg/useasheap" />
#     </environment>
# <cell>
#
# This gives programs in the "dest" cell the right to send messages to
# prog1's helper thread (but not its main thread) and to use the heap_seg
# memsection as a kernel heap.
#
# Every object that can be shared must appear as a bottom-level
# directory in the elfweaver namespace (a POSIX-like namespace where
# the cells are in the top level directory and programs, etc are the
# intermediate directories).  The 'files' within this directory are
# the rights objects found below.
#
# Each Rights object gives a name to a 'rights mask', which is a
# bitmap of possible operations that a client could perform on the
# object.  The values in the mask are specific to the object
# associated with it.  It is not possible to combine rights masks at
# the XML, so a Rights object must be created for each supported mask
# combination.  A rights mask is made by passing a list of strings to
# the Rights object's constructor.  The list of valid strings can be
# seen further down the file.
#
# Each class has the add_standard_rights() class method which can be
# used to populate the namespace directory with supported rights
# masks.  It is also possible to add Rights objects to the namespace
# directly.
#
# To make the user's life easier, the namespace has the concept of the
# 'default' name, which will be used whenever the directory name is
# the last item in a namespace path.  The default Right should be the
# one that the user wants to use.  For example the 'HELPER_THREAD'
# environment entry above refers to the helper thread's default Rights,
# which allows IPCs to be sent to the thread.  The BUFFER entry refers
# to a Right object directly.  For memsections, the default right
# allows the memsection's physical segment to be mapped into the
# client's address space.
#
# Each Right object is an abstract concept.  Until the right is
# referred to in an environment, and a cap created for it, it does not
# consume any memory in the target image.
#
# A cap is created by the destination environment calling copy_to() on
# the right and passing its own domain.  The default implementation
# calls domain.register_cap(), which eventually causes the cap_copy()
# syscall to be called.  The function returns an object that has the
# method get_cap_value(), which returns, once the kernel heap has been
# written out, the cap value as a word.  This value is placed in the
# environment for the client to use.
#
# Note that the cap value doesn't have to be a real cap.  For example,
# the PhysAddrRights class below returns the physical address of a
# segment, and other values are possible as well.
#
# The design is complex because of the following factors:
#
# a) The environment entry element may contain a forward reference to
#    an object that not yet been seen by elfweaver.
#
# b) ELfweaver should know all of the valid rights masks during the
#    XML process stage so that meaningful error messages can be
#    generated if an invalid right is requested.
#
# c) The destination domain, not the source domain, is responsible for
#    generating the cap value.
#
# d) The cap value is not known until the heap is initialised, but the
#    structure of the environment (but not the final contents) must be
#    known before that.
#

from pyokl4.autogen import Autogen
from weaver import MergeError

def _get_rights_constant(name):
    """
    Get the Autogen rights constant 'name', or return 0 if the constant does
    not exist.
    """
    value = Autogen.rights.__dict__.get(name)
    if value is None:
        return 0
    return value

class RightsCopyData(object):
    """
    Class for holding the parameters passed to the copy_to() method.

    A class is used, rather than a parameter list, because it simplifies the
    construction of the parameter list from different cell implementations.
    """

    def __init__(self, domain, image_group):
        self.domain = domain
        self.image_group = image_group

class Rights(object):
    """
    Generic class for cap rights in the elfweaver namespace.
    """

    def _get_rights(self):
        return {}

    def __init__(self, name, obj, rights=None):
        self.name = name
        self.object = obj
        self.rights_mask = 0
        self.env_entries = {}
        self.rights_list = rights

        if rights is not None:
            for right in rights:
                self.add_right(right)

    def get_name(self):
        """Return the name of the cap."""
        return self.name

    def get_rights_mask(self):
        """Return the rights bit mask."""
        return self.rights_mask

    def get_str(self, type_str):
        if self.rights_list is None:
            return "%s cap" % type_str
        else:
            return '%s cap with rights: "%s"' % \
                   (type_str, '", "'.join(sorted(self.rights_list)))

    def add_right(self, right):
        """Add a right value to the rights bitmask."""
        right_val = self._get_rights().get(right, None)

        if right_val is None:
            raise MergeError, "'%s' not a supported right." % right

        self.rights_mask |= right_val

    def copy_to(self, cpdata):
        """
        Copy this right to the object into the given domain.

        Return a cap object.
        """
        return cpdata.domain.register_cap(self.object, self.rights_mask)

    def store_env_entries(self, env_entries):
        """
        Certain rights implicitly create new environment entries for the
        owner. We store these entries so if other cells require the right,
        they also get a copy of the associated environment entries.
        """
        for name, entry in env_entries:
            self.env_entries[name] = entry


class ThreadRights(Rights):
    """Cap rights for threads."""

    def _get_rights(self):
        return {
            'master'    : _get_rights_constant("RIGHTS_THREAD_MASTER"),
            'notify'    : _get_rights_constant("RIGHTS_THREAD_NOTIFY"),
            'ipc'       : _get_rights_constant("RIGHTS_THREAD_IPC"),
            'registers' : _get_rights_constant("RIGHTS_THREAD_REGISTERS"),
            'abort'     : _get_rights_constant("RIGHTS_THREAD_ABORT"),
            'interrupt' : _get_rights_constant("RIGHTS_THREAD_INTERRUPT")
            }

    def __init__(self, name, obj, rights=None):
        Rights.__init__(self, name, obj, rights)

    def __str__(self):
        """
        Return a human readable string describing the cap rights.
        """
        return self.get_str("Thread")

    @classmethod
    def add_standard_rights(cls, obj, namespace):
        """
        Install into the weaver namespace the standard rights for this
        object and set the default name to most commonly used right.
        """

        right = cls('master', obj, ['master'])
        namespace.add(right.get_name(), right)

        right = cls('ipc_only', obj, ['ipc'])
        namespace.add(right.get_name(), right)

        right = cls('ipc', obj, ['ipc', 'notify'])
        namespace.add(right.get_name(), right)

        namespace.set_default_name('ipc')

class DomainRights(Rights):
    """Cap rights for domains."""

    def _get_rights(self):
        return {
            'master' : _get_rights_constant("RIGHTS_DOMAIN_MASTER"),
            'capmgmt' : _get_rights_constant("RIGHTS_DOMAIN_CAPMGMT")
            }

    def __init__(self, name, obj, rights=None):
        Rights.__init__(self, name, obj, rights)

    def __str__(self):
        """
        Return a human readable string describing the cap rights.
        """
        return self.get_str("Domain")

    @classmethod
    def add_standard_rights(cls, obj, namespace):
        """
        Install into the weaver namespace the standard rights for this
        object and set the default name to most commonly used right.
        """

        right = cls('master', obj, ['master'])
        namespace.add(right.get_name(), right)

        right = cls('capmgmt', obj, ['capmgmt'])
        namespace.add(right.get_name(), right)

        # Note: Domains aren't meant to be shared, but we'll give a
        # default for consistency.
        namespace.set_default_name('capmgmt')

class MMUContextRights(Rights):
    """Cap rights for MMU contexts."""

    def _get_rights(self):
        return {
            'master' : _get_rights_constant("RIGHTS_MMU_CONTEXT_MASTER"),
            'attach' : _get_rights_constant("RIGHTS_MMU_CONTEXT_ATTACH"),
            'share'  : _get_rights_constant("RIGHTS_MMU_CONTEXT_SHARE"),
            'manage' : _get_rights_constant("RIGHTS_MMU_CONTEXT_MANAGE"),
            'flush'  : _get_rights_constant("RIGHTS_MMU_CONTEXT_FLUSH"),
            }

    def __init__(self, name, obj, rights=None):
        Rights.__init__(self, name, obj, rights)

    def __str__(self):
        """
        Return a human readable string describing the cap rights.
        """
        return self.get_str("MMUContext")

    @classmethod
    def add_standard_rights(cls, obj, namespace):
        """
        Install into the weaver namespace the standard rights for this
        object and set the default name to most commonly used right.
        """

        right = cls('master', obj, ['master'])
        namespace.add(right.get_name(), right)

        # Note: MMU contexts aren't meant to be shared, but we'll give
        # a default for consistency.
        namespace.set_default_name('master')

class SegmentRights(Rights):
    """Cap rights for segments."""

    def _get_rights(self):
        return {
            'master'    : _get_rights_constant("RIGHTS_SEGMENT_MASTER"),
            'attach'    : _get_rights_constant("RIGHTS_SEGMENT_ATTACH"),
            'useasheap' : _get_rights_constant("RIGHTS_SEGMENT_USEASHEAP"),
            }

    def __init__(self, name, obj, rights=None):
        Rights.__init__(self, name, obj, rights)

    def __str__(self):
        """
        Return a human readable string describing the cap rights.
        """
        return self.get_str("Segment")

    def copy_to(self, cpdata):
        """
        Copy this right to the object into the given domain.

        Return a cap object.
        """
        image_group = cpdata.image_group
        domain = cpdata.domain

        # Add the object to an image group in target cell so that a virtual
        # address can be allocated for the object in that cell.
        # FIXME: Problem with segments: different virtual addresses in each sas!
        attrs = self.object.attrs
        image_group.add_object(attrs)

        image_group.sas.mark_virtual(attrs.abs_name(),
                                     attrs.virt_addr_sas(image_group.sas),
                                     attrs.size, attrs.cache_attr)


        # Get the copied cap in the given domain for the source segment.
        shared_cap = domain.register_cap(self.object, self.rights_mask)

        # Create a pseudo-segment in the given domain that can be used in
        # places (such as the segment table) where segment details are
        # needed.
        shared_segment = domain.register_shared_segment(self.object,
                                                        shared_cap)

        # If desired, map the segment into the main mmu_context of this
        # domain, otherwise just add attach it and add it to the segment
        # table.
        if domain.map_in_external_objects:
            domain.mmu_contexts[0].register_mapping(shared_segment,
                                                image_group.sas_attrs(attrs))
        else:
            domain.mmu_contexts[0].register_attachment(shared_segment)

        return shared_cap


    @classmethod
    def add_standard_rights(cls, obj, namespace, env_entries=None):
        """
        Install into the weaver namespace the standard rights for this
        object and set the default name to most commonly used right.
        Optionally take a list of name, environment entry tuples potentially
        associated with each right and store them in the internal dictionary.
        """

        if env_entries is None:
            env_entries = []

        right = cls('master', obj, ['master'])
        namespace.add(right.get_name(), right)

        right = cls('attach', obj, ['attach'])
        right.store_env_entries(env_entries)
        namespace.add(right.get_name(), right)

        right = cls('useasheap', obj, ['useasheap'])
        right.store_env_entries(env_entries)
        namespace.add(right.get_name(), right)

        namespace.set_default_name('attach')

class MutexRights(Rights):
    """Cap rights for mutexes."""

    def _get_rights(self):
        return {
            'master' : _get_rights_constant("RIGHTS_MUTEX_MASTER"),
            'access' : _get_rights_constant("RIGHTS_MUTEX_ACCESS"),
            }

    def __init__(self, name, obj, rights=None):
        Rights.__init__(self, name, obj, rights)

    def __str__(self):
        """
        Return a human readable string describing the cap rights.
        """
        return self.get_str("Mutex")

    @classmethod
    def add_standard_rights(cls, obj, namespace):
        """
        Install into the weaver namespace the standard rights for this
        object and set the default name to most commonly used right.
        """

        right = cls('master', obj, ['master'])
        namespace.add(right.get_name(), right)

        right = cls('access', obj, ['access'])
        namespace.add(right.get_name(), right)

        namespace.set_default_name('access')


class HybridMutexRights(Rights):
    """Cap rights for mutexes."""

    def _get_rights(self):
        return {
            'master' : _get_rights_constant("RIGHTS_HYBRID_MUTEX_MASTER"),
            'access' : _get_rights_constant("RIGHTS_HYBRID_MUTEX_ACCESS"),
            }

    def __init__(self, name, obj, rights=None):
        Rights.__init__(self, name, obj, rights)

    def __str__(self):
        """
        Return a human readable string describing the cap rights.
        """
        return self.get_str("Hybrid Mutex")

    @classmethod
    def add_standard_rights(cls, obj, namespace):
        """
        Install into the weaver namespace the standard rights for this
        object and set the default name to most commonly used right.
        """

        right = cls('master', obj, ['master'])
        namespace.add(right.get_name(), right)

        right = cls('access', obj, ['access'])
        namespace.add(right.get_name(), right)

        namespace.set_default_name('access')


class TraceBufferControlRights(Rights):
    """Cap rights for tracebuffers."""

    def _get_rights(self):
        return {
            'master' : _get_rights_constant("RIGHTS_MASTER"),
            }

    def __init__(self, name, rights=None):
        Rights.__init__(self, name, None, rights)

    def __str__(self):
        """
        Return a human readable string describing the cap rights.
        """
        return self.get_str("Tracebuffer Control")

    class TraceBufferControlPseudoCap(object):
        def __init__(self, domain):
            self.domain = domain

        def get_cap_value(self):
            if self.domain.okl4_tracebuffer_control_object is not None:
                return self.domain.okl4_tracebuffer_control_object._cap_id
            else:
                return None

    def copy_to(self, cpdata):
        return self.TraceBufferControlPseudoCap(cpdata.domain)

class InterruptRights(Rights):
    """Cap rights for interrupts."""

    def _get_rights(self):
        return {
            'master'   : _get_rights_constant("RIGHTS_INTERRUPT_MASTER"),
            'register' : _get_rights_constant("RIGHTS_INTERRUPT_REGISTER"),
            'raise'    : _get_rights_constant("RIGHTS_INTERRUPT_RAISE")
            }

    def __init__(self, name, obj, rights=None):
        Rights.__init__(self, name, obj, rights)

    def __str__(self):
        """
        Return a human readable string describing the cap rights.
        """
        return self.get_str("Interrupt")

    @classmethod
    def add_standard_rights(cls, obj, namespace):
        """
        Install into the weaver namespace the standard rights for this
        object and set the default name to most commonly used right.
        """

        right = cls('master', obj, ['master'])
        namespace.add(right.get_name(), right)

        right = cls('register', obj, ['register'])
        namespace.add(right.get_name(), right)

        right = cls('raise', obj, ['raise'])
        namespace.add(right.get_name(), right)

        namespace.set_default_name('master')

class PlatformControlRights(Rights):
    """Cap rights for Platform Control."""

    all_rights = {}

    def __str__(self):
        """
        Return a human readable string describing the cap rights.
        """
        return self.get_str("Platform Control")

    @classmethod
    def dynamic_cap(cls, name, obj):
        try:
            mask = int(name, 0)
        except ValueError:
            return None

        cls.all_rights[name] = mask
        right = cls(name, obj, [name])
        return right

    def _get_rights(self):
        return self.__class__.all_rights

    def __init__(self, name, obj, rights=None):
        # FIXME: Jira ticket OLD-1680 - adamc.
        self.__class__.all_rights['master'] = ~0 #_get_rights_constant("RIGHTS_PLATFORM_CONTROL_MASTER")
        Rights.__init__(self, name, obj, rights)

    class PlatformControlPseudoCap(object):
        def __init__(self, domain):
            self.domain = domain

        def get_cap_value(self):
            if self.domain.plat_control.okl4_object is not None:
                return self.domain.plat_control.okl4_object._cap_id
            else:
                return None

    def copy_to(self, cpdata):
        if self.rights_mask == ~0:
            return self.PlatformControlPseudoCap(cpdata.domain)
        else:
            return cpdata.domain.register_cap(self.object, self.rights_mask)

    @classmethod
    def add_standard_rights(cls, obj, namespace):
        """
        Install into the weaver namespace the standard rights for this
        object and set the default name to most commonly used right.
        """

        right = cls('master', obj, ['master'])
        namespace.add(right.get_name(), right)

        namespace.add('__dynamic', (cls.dynamic_cap, obj))

class ProcessorControlRights(Rights):
    """Cap rights for processor control."""

    all_rights = {}

    def __str__(self):
        """
        Return a human readable string describing the cap rights.
        """
        return self.get_str("Processor Control")

    @classmethod
    def dynamic_cap(cls, name, obj):
        try:
            mask = int(name, 0)
        except ValueError:
            return None

        cls.all_rights[name] = mask
        right = cls(name, obj, [name])
        return right

    def _get_rights(self):
        return self.__class__.all_rights

    def __init__(self, name, obj, rights=None):
        # FIXME: Jira ticket OLD-1680 - adamc.
        self.__class__.all_rights['master'] = ~0 #_get_rights_constant("RIGHTS_PROCESSOR_CONTROL_MASTER")
        Rights.__init__(self, name, obj, rights)

    class ProcessorControlPseudoCap(object):
        def __init__(self, domain):
            self.domain = domain

        def get_cap_value(self):
            if self.domain.processor_control.okl4_object is not None:
                return self.domain.processor_control.okl4_object._cap_id
            else:
                return None

    def copy_to(self, cpdata):
        if self.rights_mask == ~0:
            return self.ProcessorControlPseudoCap(cpdata.domain)
        else:
            return cpdata.domain.register_cap(self.object, self.rights_mask)

    @classmethod
    def add_standard_rights(cls, obj, namespace):
        """
        Install into the weaver namespace the standard rights for this
        object and set the default name to most commonly used right.
        """

        right = cls('master', obj, ['master'])
        namespace.add(right.get_name(), right)

        namespace.add('__dynamic', (cls.dynamic_cap, obj))

class ProfileControlRights(Rights):
    """Cap rights for profile control."""

    all_rights = {}

    @classmethod
    def dynamic_cap(cls, name, obj):
        try:
            mask = int(name, 0)
        except ValueError:
            return None

        cls.all_rights[name] = mask
        right = cls(name, obj, [name])
        return right

    def _get_rights(self):
        return self.__class__.all_rights

    def __init__(self, name, obj, rights=None):
        # FIXME: Jira ticket OLD-1680 - adamc.
        self.__class__.all_rights['master'] = ~0 #_get_rights_constant("RIGHTS_PROFILE_CONTROL_MASTER")
        Rights.__init__(self, name, obj, rights)

    class ProfileControlPseudoCap(object):
        def __init__(self, domain):
            self.domain = domain

        def get_cap_value(self):
            if self.domain.profile_control.okl4_object is not None:
                return self.domain.profile_control.okl4_object._cap_id
            else:
                return None

    def copy_to(self, cpdata):
        if self.rights_mask == ~0:
            return self.ProfileControlPseudoCap(cpdata.domain)
        else:
            return cpdata.domain.register_cap(self.object, self.rights_mask)

    @classmethod
    def add_standard_rights(cls, obj, namespace):
        """
        Install into the weaver namespace the standard rights for this
        object and set the default name to most commonly used right.
        """

        right = cls('master', obj, ['master'])
        namespace.add(right.get_name(), right)

        namespace.add('__dynamic', (cls.dynamic_cap, obj))

class IntlockControlRights(Rights):
    """Cap rights for intlock_control."""

    all_rights = {}

    @classmethod
    def dynamic_cap(cls, name, obj):
        try:
            mask = int(name, 0)
        except ValueError:
            return None

        cls.all_rights[name] = mask
        right = cls(name, obj, [name])
        return right

    def _get_rights(self):
        return self.__class__.all_rights

    def __init__(self, name, obj, rights=None):
        # FIXME: Jira ticket OLD-1680 - adamc.
        self.__class__.all_rights['master'] = ~0 #_get_rights_constant("RIGHTS_INTLOCK_CONTROL_MASTER")
        Rights.__init__(self, name, obj, rights)

    class IntlockControlPseudoCap(object):
        def __init__(self, domain):
            self.domain = domain

        def get_cap_value(self):
            if self.domain.intlock_control.okl4_object is not None:
                return self.domain.intlock_control.okl4_object._cap_id
            else:
                return None

    def copy_to(self, cpdata):
        if self.rights_mask == ~0:
            return self.IntlockControlPseudoCap(cpdata.domain)
        else:
            return cpdata.domain.register_cap(self.object, self.rights_mask)

    @classmethod
    def add_standard_rights(cls, obj, namespace):
        """
        Install into the weaver namespace the standard rights for this
        object and set the default name to most commonly used right.
        """

        right = cls('master', obj, ['master'])
        namespace.add(right.get_name(), right)

        namespace.add('__dynamic', (cls.dynamic_cap, obj))


class PhysAddrRights(Rights):
    """Rights to the physical address of a segment."""

    def _get_rights(self):
        return {
            'master' : _get_rights_constant("RIGHTS_MASTER"),
            }

    def __init__(self, name, obj, rights=None):
        Rights.__init__(self, name, obj, rights)

    def __str__(self):
        return "Physical Address of the Segment"

    def add_right(self, _):
        """Add a right value to the rights bitmask."""
        # Adding rights are meaningless to this object.
        pass

    class PhysAddrPseudoCap(object):
        """
        Pseudo cap for physical addresses.  The physical address is
        used instead of the cap value.
        """

        # Back channel to identify this cap as holding a physical address.
        really_phys_addr = True

        def __init__(self, attrs):
            self.attrs = attrs

        def get_cap_value(self):
            """
            Return the segment number for the shared memory region.
            """
            return self.attrs.phys_addr

    def copy_to(self, cpdata):
        """
        Copy this right to the object into the given domain.

        Return a cap object.
        """
        return self.PhysAddrPseudoCap(self.object.attrs)

    @classmethod
    def add_standard_rights(cls, obj, namespace):
        """
        Install into the weaver namespace the standard rights for this
        object and set the default name to most commonly used right.
        """

        right = cls('physical', obj)
        namespace.add(right.get_name(), right)

        # Don't set default name because these rights are shared with
        # segment rights.

class SizeRights(Rights):
    """Rights to the size of a segment."""

    def _get_rights(self):
        return {
            'master' : _get_rights_constant("RIGHTS_MASTER"),
            }

    def __init__(self, name, obj, rights=None):
        Rights.__init__(self, name, obj, rights)

    def __str__(self):
        return "Size of the Segment"

    def add_right(self, _):
        """Add a right value to the rights bitmask."""
        # Adding rights are meaningless to this object.
        pass

    class SizePseudoCap(object):
        """
        Pseudo cap for sizes.
        """
        def __init__(self, attrs):
            self.attrs = attrs

        def get_cap_value(self):
            return self.attrs.size

    def copy_to(self, cpdata):
        """
        Copy this right to the object into the given domain.

        Return a cap object.
        """
        return self.SizePseudoCap(self.object.attrs)

    @classmethod
    def add_standard_rights(cls, obj, namespace):
        """
        Install into the weaver namespace the standard rights for this
        object and set the default name to most commonly used right.
        """

        right = cls('size', obj)
        namespace.add(right.get_name(), right)

        # Don't set default name because these rights are shared with
        # segment rights.


class ValueRights(Rights):
    """."""

    def _get_rights(self):
        return {
            'master' : _get_rights_constant("RIGHTS_MASTER"),
            }

    def __init__(self, name, value):
        Rights.__init__(self, name, None, None)

        self.value = value

    def __str__(self):
        return "Value" % self.value

    def add_right(self, _):
        """Add a right value to the rights bitmask."""
        # Adding rights are meaningless to this object.
        pass

    class ValuePseudoCap(object):
        """
        Pseudo cap for sizes.
        """
        def __init__(self, value):
            self.value = value

        def get_cap_value(self):
            return self.value

    def copy_to(self, cpdata):
        """
        Copy this right to the object into the given domain.

        Return a cap object.
        """
        return self.ValuePseudoCap(self.value)

    @classmethod
    def add_standard_rights(cls, obj, namespace):
        """
        Install into the weaver namespace the standard rights for this
        object and set the default name to most commonly used right.
        """

        right = cls('size', obj)
        namespace.add(right.get_name(), right)

        # Don't set default name because these rights are shared with
        # segment rights.


class Attachment(Rights):
    """Attachment node"""

    rights = {}

    def __init__(self, name, obj, rights=None):
        Rights.__init__(self, name, obj, rights)

    def add_right(self, _):
        """Add a right value to the rights bitmask."""
        # Adding rights are meaningless to this object.
        pass

    class AttachmentPseudoCap(object):
        """
        Pseudo cap for attachments.  Returns the actual object we are attached
        to / from.
        """
        def __init__(self, obj):
            self.obj = obj

        def get_cap_value(self):
            """
            Return the attached object.
            """
            return self.obj

    def copy_to(self, cpdata):
        """
        Copy this right to the object into the given domain.

        Return a cap object.
        """
        return self.AttachmentPseudoCap(self.object)

    @classmethod
    def add(cls, obj, namespace):
        """
        Install into the weaver namespace the standard rights for this
        object and set the default name to most commonly used right.
        """

        right = cls('attachment', obj)
        namespace.add(right.get_name(), right)

        # Don't set default name because these rights are shared with
        # segment rights.

class TrustZoneRights(Rights):
    """Cap rights for tzmonitor objects."""

    def _get_rights(self):
        return {
            'master'   : _get_rights_constant("RIGHTS_TZMONITOR_MASTER"),
            }

    def __init__(self, name, obj, rights=None):
        Rights.__init__(self, name, obj, rights)

    @classmethod
    def add_standard_rights(cls, obj, namespace):
        """
        Install into the weaver namespace the standard rights for this
        object and set the default name to most commonly used right.
        """

        right = cls('master', obj, ['master'])
        namespace.add(right.get_name(), right)

        namespace.set_default_name('master')

class MessageQueueRights(Rights):
    """Cap rights for message_queue objects."""

    def _get_rights(self):
        return {
            'master'    : _get_rights_constant("RIGHTS_MESSAGE_QUEUE_MASTER"),
            'send'      : _get_rights_constant("RIGHTS_MESSAGE_QUEUE_SEND"),
            'receive'   : _get_rights_constant("RIGHTS_MESSAGE_QUEUE_RECEIVE"),
            'buffer'    : _get_rights_constant("RIGHTS_MESSAGE_QUEUE_BUFFER"),
            }

    def __init__(self, name, obj, rights=None):
        Rights.__init__(self, name, obj, rights)

    def __str__(self):
        """
        Return a human readable string describing the cap rights.
        """
        return self.get_str("Message Queue")

    @classmethod
    def add_standard_rights(cls, obj, namespace):
        """
        Install into the weaver namespace the standard rights for this
        object and set the default name to most commonly used right.
        """

        right = cls('master', obj, ['master'])
        namespace.add(right.get_name(), right)

        right = cls('send', obj, ['send'])
        namespace.add(right.get_name(), right)

        right = cls('receive', obj, ['receive'])
        namespace.add(right.get_name(), right)

        right = cls('buffer', obj, ['buffer'])
        namespace.add(right.get_name(), right)

        right = cls('send-receive', obj, ['send', 'receive'])
        namespace.add(right.get_name(), right)

        right = cls('submaster', obj, ['buffer', 'send', 'receive'])
        namespace.add(right.get_name(), right)

        namespace.set_default_name('send-receive')

class ChannelRights(Rights):
    """Cap rights for channel objects."""

    def _get_rights(self):
        return {
            'master' : _get_rights_constant("RIGHTS_CHANNEL_MASTER"),
            'secondary' : _get_rights_constant("RIGHTS_CHANNEL_SECONDARY"),
            }

    def __init__(self, name, obj, rights=None):
        Rights.__init__(self, name, obj, rights)

    def __str__(self):
        """
        Return a human readable string describing the cap rights.
        """
        return self.get_str("Channel")

    @classmethod
    def add_standard_rights(cls, obj, namespace):
        """
        Install into the weaver namespace the standard rights for this
        object and set the default name to most commonly used right.
        """

        right = cls('master', obj, ['master'])
        namespace.add(right.get_name(), right)

        right = cls('secondary', obj, ['secondary'])
        namespace.add(right.get_name(), right)

        namespace.set_default_name('secondary')

#
#---------------------------------------------------------------------------
#
#
# VIRQ source objects.
#

class VIRQSource(object):
    """Base class for objects representing VIRQ sources."""
    def __init__(self, domain):
        self.domain = domain

    def connect_source(self, virq):
        """Connect the kernel virq object to this source."""
        raise NotImplementedError

class VInterruptLineSource(VIRQSource):
    """
    Class for VInterruptLine sources.

    A VInterruptLine source isn't a real object, instead it is just the right
    for a domain to raise interrupts on the virq.
    """

    def __init__(self, virq_line, domain):
        VIRQSource.__init__(self, domain)
        self.virq_line = virq_line

    def __str__(self):
        return "Virtual Interrupt Line"

    def connect_source(self, virq):
        """
        Connect the virq to this interrupt line by giving the source's domain
        the right to raise interrupts on the virq.
        """
        cap = self.domain.register_cap(virq,
                                       Autogen.rights.RIGHTS_INTERRUPT_RAISE)
        self.virq_line.cap_object = cap

    @classmethod
    def add_standard_sources(cls, name, virq_line, domain, namespace):
        """
        Add the standard source objects for a given source.
        """
        # Only one object is needed for the source.
        source = cls(virq_line, domain)
        namespace.add(name, source)

class ChannelVIRQSource(VIRQSource):
    """
    Class for channel VIRQ sources.

    A channel has four transition VIRQ sources that indicate whether or not
    the inbound and outbound directions are full or empty.  Clients use these
    to know if this it save to send or receive a message.
    """
    def __init__(self, domain, channel, transition):
        VIRQSource.__init__(self, domain)

        self.channel = channel
        self.transition = transition

    def __str__(self):
        return "Channel Signal Source"

    def connect_source(self, virq):
        """
        Connect the virq to the channel transition source described by this
        object.
        """
        self.channel.virqs[self.transition] = virq

    @classmethod
    def add_standard_sources(cls, domain, channel, namespace):
        """
        Add to the namespace objects representing the four transition VIRQ
        sources of the channel.
        """
        trigger = Autogen.types.OKL4_channel_transition

        source = cls(domain, channel,
                     trigger.CHANNEL_TRANSITION_NOTFULL_OUTBOUND)
        namespace.add("out_not_full", source)

        source = cls(domain, channel,
                     trigger.CHANNEL_TRANSITION_NOTFULL_INBOUND)
        namespace.add("in_not_full", source)

        source = cls(domain, channel,
                     trigger.CHANNEL_TRANSITION_NOTEMPTY_OUTBOUND)
        namespace.add("out_not_empty", source)

        source = cls(domain, channel,
                     trigger.CHANNEL_TRANSITION_NOTEMPTY_INBOUND)
        namespace.add("in_not_empty", source)

#
#---------------------------------------------------------------------------
#
# Value objects.
#

class Value(object):
    """
    Class for finding constant integers in the namespace.

    The object is not initialised with the value, rather the value can be setg
    at a later date.  This allows the object to be found in the namespace even
    if the value has not yet been calculated (for instance, once cell needs to
    know that the value *exists* so that it can calculate the size of its
    environment, but a cell that's processed later calculates the value.
    """
    def __init__(self):
        self._value = None

    def _get_value(self):
        """Get the object's value."""
        return self._value

    def _set_value(self, value):
        """Set the object's value."""
        self._value = value

    value = property(_get_value, _set_value)

    def __str__(self):
        if self.value is None:
            return "Value: <Not Set>"
        else:
            return "Value: %d" % self.value

    @classmethod
    def add_value(cls, name, namespace):
        """
        Add the standard value objects for a given name.
        """
        # Only one object is needed for the source.
        val_obj = Value()

        namespace.add(name, val_obj)

        return val_obj
