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

# Auto generated file: Do not modify directly.

# Evil import magic follows.  In pyokl4 code we want to have access to the
# correct auto generated code (objmanager, types, etc.).  We implictly have
# the product and arch info however the python code shouldn't need to worry
# about doing this - nor can we use strings in a import statement
# (except __import__, which has problems with freezing).

# So this helper module imports all possible combinations of auto genned
# modules and then populates an Autogen class with the correct combinat of
# product, arch and cpu modules when called.  Modules then just need to import
# the Autogen class and access its class members to access the correct objects.
# For example after calling import_autogen_modules code can do the following
# to access a type class.

# from autogen import Autogen
# w = Autogen.types.word_t(address)
# thread_array = Autogen.objmanager.object_allocate(....)
# etc.

"""
Helper code to set up the code for the kernel configuration.
"""

# Ignore pylint complaints about too many lines in the file.
#pylint: disable-msg=C0302


import re
from pyokl4.autogen import Autogen

def replace_illegal_chars(name):
    """Replace all illegal identifiers with underscores."""
    valid = re.compile(r'^[a-zA-Z0-9_]$')
    return "".join(map(lambda x: [x, '_'][valid.match(x) is None], name))

def import_autogen_modules(product, arch, cpu_desc, debug):
    """
    Given a product, arch, debug, cpu populate a series of variables with the
    appropiate module objects.  After calling this function one should be
    able to use 'Autogen.types.word_t' or 'Autogen.syscalls.sys_create_thread'.
    """

    # Ignore warnings about too many statements and branches.
    #pylint: disable-msg=R0912,R0915

    # Setup static modules prior to importing other modules.
    import pyokl4.events
    pyokl4.events.init()
    import pyokl4.kernel_data
    pyokl4.kernel_data.init()

    # Temporary fix to strip out illegal identifiers in cpu descriptions
    cpu_desc = replace_illegal_chars(cpu_desc)

    Autogen.cpu_desc = cpu_desc


    if product == 'okl4nano' and arch == 'armv5' and \
            cpu_desc == 'arm926ej_s_r0p3_vfp' and debug == 'production':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'hal',
            'mmu',
            'atomic_ops',
            'cache_control',
            'cell_environment',
            'ElfFile',
            'exception',
            'futex',
            'interrupt',
            'kernel',
            'memory_pools',
            'scheduler',
            'notify',
            'objmanager',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'PD',
            'soc',
            'suspend',
            'tracebuffer',
            'useraccess',
        )

        module_names = (
            'allocator',
            'hal',
            'mmu_static',
            'atomic_ops',
            'cache_control_nano',
            'cell_environment',
            'ElfFile',
            'exception_trivial',
            'futex',
            'interrupt_dynamic',
            'kernel_standard',
            'memory_pools',
            'scheduler_standard',
            'notify_standard',
            'objmanager_trivial',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'single_pd',
            'soc_support',
            'suspend',
            'tracebuffer_null',
            'useraccess_static',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4nano.production.arm926ej_s_r0p3_vfp.types
        Autogen.types = \
            okl4types.okl4nano.production.arm926ej_s_r0p3_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4nano.production.arm926ej_s_r0p3_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4nano.production.arm926ej_s_r0p3_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4nano.production.arm926ej_s_r0p3_vfp.rights
        Autogen.rights = \
            okl4types.okl4nano.production.arm926ej_s_r0p3_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_static
        Autogen.mmu = \
            pyokl4.okl4.mmu_static
        modules.append(Autogen.mmu)

        import pyokl4.okl4.cache_control_nano
        Autogen.cache_control = \
            pyokl4.okl4.cache_control_nano
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.futex
        Autogen.futex = \
            pyokl4.okl4.futex
        modules.append(Autogen.futex)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.objmanager_trivial
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_trivial
        modules.append(Autogen.objmanager)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv5.okl4.mmu_static
        Autogen.mmu_arch = \
            pyokl4.arch.armv5.okl4.mmu_static
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv5.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv5.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4nano.production.arm926ej_s_r0p3_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4nano.production.arm926ej_s_r0p3_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4nano.production.arm926ej_s_r0p3_vfp.objmanager_trivial
        Autogen.objmanager_autogen = \
           okl4types.okl4nano.production.arm926ej_s_r0p3_vfp.objmanager_trivial
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4nano' and arch == 'armv6' and \
            cpu_desc == 'arm1136jf_s_r0p4_vfp' and debug == 'production':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'hal',
            'mmu',
            'atomic_ops',
            'cache_control',
            'cell_environment',
            'ElfFile',
            'exception',
            'futex',
            'interrupt',
            'kernel',
            'memory_pools',
            'scheduler',
            'notify',
            'objmanager',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'PD',
            'soc',
            'suspend',
            'tracebuffer',
            'useraccess',
        )

        module_names = (
            'allocator',
            'hal',
            'mmu_static',
            'atomic_ops',
            'cache_control_nano',
            'cell_environment',
            'ElfFile',
            'exception_trivial',
            'futex',
            'interrupt_dynamic',
            'kernel_standard',
            'memory_pools',
            'scheduler_standard',
            'notify_standard',
            'objmanager_trivial',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'single_pd',
            'soc_support',
            'suspend',
            'tracebuffer_null',
            'useraccess_static',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4nano.production.arm1136jf_s_r0p4_vfp.types
        Autogen.types = \
            okl4types.okl4nano.production.arm1136jf_s_r0p4_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4nano.production.arm1136jf_s_r0p4_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4nano.production.arm1136jf_s_r0p4_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4nano.production.arm1136jf_s_r0p4_vfp.rights
        Autogen.rights = \
            okl4types.okl4nano.production.arm1136jf_s_r0p4_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_static
        Autogen.mmu = \
            pyokl4.okl4.mmu_static
        modules.append(Autogen.mmu)

        import pyokl4.okl4.cache_control_nano
        Autogen.cache_control = \
            pyokl4.okl4.cache_control_nano
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.futex
        Autogen.futex = \
            pyokl4.okl4.futex
        modules.append(Autogen.futex)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.objmanager_trivial
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_trivial
        modules.append(Autogen.objmanager)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv6.okl4.mmu_static
        Autogen.mmu_arch = \
            pyokl4.arch.armv6.okl4.mmu_static
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv6.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv6.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4nano.production.arm1136jf_s_r0p4_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4nano.production.arm1136jf_s_r0p4_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4nano.production.arm1136jf_s_r0p4_vfp.objmanager_trivial
        Autogen.objmanager_autogen = \
           okl4types.okl4nano.production.arm1136jf_s_r0p4_vfp.objmanager_trivial
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4nano' and arch == 'armv6' and \
            cpu_desc == 'arm1136jf_s_r1p5_vfp' and debug == 'production':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'hal',
            'mmu',
            'atomic_ops',
            'cache_control',
            'cell_environment',
            'ElfFile',
            'exception',
            'futex',
            'interrupt',
            'kernel',
            'memory_pools',
            'scheduler',
            'notify',
            'objmanager',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'PD',
            'soc',
            'suspend',
            'tracebuffer',
            'useraccess',
        )

        module_names = (
            'allocator',
            'hal',
            'mmu_static',
            'atomic_ops',
            'cache_control_nano',
            'cell_environment',
            'ElfFile',
            'exception_trivial',
            'futex',
            'interrupt_dynamic',
            'kernel_standard',
            'memory_pools',
            'scheduler_standard',
            'notify_standard',
            'objmanager_trivial',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'single_pd',
            'soc_support',
            'suspend',
            'tracebuffer_null',
            'useraccess_static',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4nano.production.arm1136jf_s_r1p5_vfp.types
        Autogen.types = \
            okl4types.okl4nano.production.arm1136jf_s_r1p5_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4nano.production.arm1136jf_s_r1p5_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4nano.production.arm1136jf_s_r1p5_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4nano.production.arm1136jf_s_r1p5_vfp.rights
        Autogen.rights = \
            okl4types.okl4nano.production.arm1136jf_s_r1p5_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_static
        Autogen.mmu = \
            pyokl4.okl4.mmu_static
        modules.append(Autogen.mmu)

        import pyokl4.okl4.cache_control_nano
        Autogen.cache_control = \
            pyokl4.okl4.cache_control_nano
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.futex
        Autogen.futex = \
            pyokl4.okl4.futex
        modules.append(Autogen.futex)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.objmanager_trivial
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_trivial
        modules.append(Autogen.objmanager)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv6.okl4.mmu_static
        Autogen.mmu_arch = \
            pyokl4.arch.armv6.okl4.mmu_static
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv6.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv6.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4nano.production.arm1136jf_s_r1p5_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4nano.production.arm1136jf_s_r1p5_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4nano.production.arm1136jf_s_r1p5_vfp.objmanager_trivial
        Autogen.objmanager_autogen = \
           okl4types.okl4nano.production.arm1136jf_s_r1p5_vfp.objmanager_trivial
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4nano' and arch == 'armv6' and \
            cpu_desc == 'arm1136jf_r1p4' and debug == 'production':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'hal',
            'mmu',
            'atomic_ops',
            'cache_control',
            'cell_environment',
            'ElfFile',
            'exception',
            'futex',
            'interrupt',
            'kernel',
            'memory_pools',
            'scheduler',
            'notify',
            'objmanager',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'PD',
            'soc',
            'suspend',
            'tracebuffer',
            'useraccess',
        )

        module_names = (
            'allocator',
            'hal',
            'mmu_static',
            'atomic_ops',
            'cache_control_nano',
            'cell_environment',
            'ElfFile',
            'exception_trivial',
            'futex',
            'interrupt_dynamic',
            'kernel_standard',
            'memory_pools',
            'scheduler_standard',
            'notify_standard',
            'objmanager_trivial',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'single_pd',
            'soc_support',
            'suspend',
            'tracebuffer_null',
            'useraccess_static',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4nano.production.arm1136jf_r1p4.types
        Autogen.types = \
            okl4types.okl4nano.production.arm1136jf_r1p4.types
        modules.append(Autogen.types)

        import okl4types.okl4nano.production.arm1136jf_r1p4.machine
        Autogen.machine_info = \
            okl4types.okl4nano.production.arm1136jf_r1p4.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4nano.production.arm1136jf_r1p4.rights
        Autogen.rights = \
            okl4types.okl4nano.production.arm1136jf_r1p4.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_static
        Autogen.mmu = \
            pyokl4.okl4.mmu_static
        modules.append(Autogen.mmu)

        import pyokl4.okl4.cache_control_nano
        Autogen.cache_control = \
            pyokl4.okl4.cache_control_nano
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.futex
        Autogen.futex = \
            pyokl4.okl4.futex
        modules.append(Autogen.futex)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.objmanager_trivial
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_trivial
        modules.append(Autogen.objmanager)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv6.okl4.mmu_static
        Autogen.mmu_arch = \
            pyokl4.arch.armv6.okl4.mmu_static
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv6.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv6.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4nano.production.arm1136jf_r1p4.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4nano.production.arm1136jf_r1p4.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4nano.production.arm1136jf_r1p4.objmanager_trivial
        Autogen.objmanager_autogen = \
           okl4types.okl4nano.production.arm1136jf_r1p4.objmanager_trivial
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4nano' and arch == 'armv7' and \
            cpu_desc == 'cortex_a8_r1p2_vfp' and debug == 'production':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'hal',
            'mmu',
            'atomic_ops',
            'cache_control',
            'cell_environment',
            'ElfFile',
            'exception',
            'futex',
            'interrupt',
            'kernel',
            'memory_pools',
            'scheduler',
            'notify',
            'objmanager',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'PD',
            'soc',
            'suspend',
            'tracebuffer',
            'useraccess',
        )

        module_names = (
            'allocator',
            'hal',
            'mmu_static',
            'atomic_ops',
            'cache_control_nano',
            'cell_environment',
            'ElfFile',
            'exception_trivial',
            'futex',
            'interrupt_dynamic',
            'kernel_standard',
            'memory_pools',
            'scheduler_standard',
            'notify_standard',
            'objmanager_trivial',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'single_pd',
            'soc_support',
            'suspend',
            'tracebuffer_null',
            'useraccess_static',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4nano.production.cortex_a8_r1p2_vfp.types
        Autogen.types = \
            okl4types.okl4nano.production.cortex_a8_r1p2_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4nano.production.cortex_a8_r1p2_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4nano.production.cortex_a8_r1p2_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4nano.production.cortex_a8_r1p2_vfp.rights
        Autogen.rights = \
            okl4types.okl4nano.production.cortex_a8_r1p2_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_static
        Autogen.mmu = \
            pyokl4.okl4.mmu_static
        modules.append(Autogen.mmu)

        import pyokl4.okl4.cache_control_nano
        Autogen.cache_control = \
            pyokl4.okl4.cache_control_nano
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.futex
        Autogen.futex = \
            pyokl4.okl4.futex
        modules.append(Autogen.futex)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.objmanager_trivial
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_trivial
        modules.append(Autogen.objmanager)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv7.okl4.mmu_static
        Autogen.mmu_arch = \
            pyokl4.arch.armv7.okl4.mmu_static
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv7.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv7.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4nano.production.cortex_a8_r1p2_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4nano.production.cortex_a8_r1p2_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4nano.production.cortex_a8_r1p2_vfp.objmanager_trivial
        Autogen.objmanager_autogen = \
           okl4types.okl4nano.production.cortex_a8_r1p2_vfp.objmanager_trivial
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4nano' and arch == 'armv5' and \
            cpu_desc == 'arm926ej_s_r0p3_vfp' and debug == 'debug':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'hal',
            'mmu',
            'atomic_ops',
            'cache_control',
            'cell_environment',
            'ElfFile',
            'exception',
            'futex',
            'interrupt',
            'kdb_interactive',
            'kdb',
            'kernel',
            'memory_pools',
            'scheduler',
            'notify',
            'objmanager',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'PD',
            'soc',
            'suspend',
            'tracebuffer',
            'useraccess',
        )

        module_names = (
            'allocator',
            'hal',
            'mmu_static',
            'atomic_ops',
            'cache_control_nano',
            'cell_environment',
            'ElfFile',
            'exception_trivial',
            'futex',
            'interrupt_dynamic',
            'kdb_interactive',
            'kdb_standard',
            'kernel_standard',
            'memory_pools',
            'scheduler_standard',
            'notify_standard',
            'objmanager_trivial',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'single_pd',
            'soc_support',
            'suspend',
            'tracebuffer_null',
            'useraccess_static',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4nano.debug.arm926ej_s_r0p3_vfp.types
        Autogen.types = \
            okl4types.okl4nano.debug.arm926ej_s_r0p3_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4nano.debug.arm926ej_s_r0p3_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4nano.debug.arm926ej_s_r0p3_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4nano.debug.arm926ej_s_r0p3_vfp.rights
        Autogen.rights = \
            okl4types.okl4nano.debug.arm926ej_s_r0p3_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_static
        Autogen.mmu = \
            pyokl4.okl4.mmu_static
        modules.append(Autogen.mmu)

        import pyokl4.okl4.cache_control_nano
        Autogen.cache_control = \
            pyokl4.okl4.cache_control_nano
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.futex
        Autogen.futex = \
            pyokl4.okl4.futex
        modules.append(Autogen.futex)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.objmanager_trivial
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_trivial
        modules.append(Autogen.objmanager)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv5.okl4.mmu_static
        Autogen.mmu_arch = \
            pyokl4.arch.armv5.okl4.mmu_static
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv5.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv5.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4nano.debug.arm926ej_s_r0p3_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4nano.debug.arm926ej_s_r0p3_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4nano.debug.arm926ej_s_r0p3_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4nano.debug.arm926ej_s_r0p3_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4nano.debug.arm926ej_s_r0p3_vfp.objmanager_trivial
        Autogen.objmanager_autogen = \
           okl4types.okl4nano.debug.arm926ej_s_r0p3_vfp.objmanager_trivial
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4nano' and arch == 'armv6' and \
            cpu_desc == 'arm1136jf_s_r0p4_vfp' and debug == 'debug':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'hal',
            'mmu',
            'atomic_ops',
            'cache_control',
            'cell_environment',
            'ElfFile',
            'exception',
            'futex',
            'interrupt',
            'kdb_interactive',
            'kdb',
            'kernel',
            'memory_pools',
            'scheduler',
            'notify',
            'objmanager',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'PD',
            'soc',
            'suspend',
            'tracebuffer',
            'useraccess',
        )

        module_names = (
            'allocator',
            'hal',
            'mmu_static',
            'atomic_ops',
            'cache_control_nano',
            'cell_environment',
            'ElfFile',
            'exception_trivial',
            'futex',
            'interrupt_dynamic',
            'kdb_interactive',
            'kdb_standard',
            'kernel_standard',
            'memory_pools',
            'scheduler_standard',
            'notify_standard',
            'objmanager_trivial',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'single_pd',
            'soc_support',
            'suspend',
            'tracebuffer_null',
            'useraccess_static',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4nano.debug.arm1136jf_s_r0p4_vfp.types
        Autogen.types = \
            okl4types.okl4nano.debug.arm1136jf_s_r0p4_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4nano.debug.arm1136jf_s_r0p4_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4nano.debug.arm1136jf_s_r0p4_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4nano.debug.arm1136jf_s_r0p4_vfp.rights
        Autogen.rights = \
            okl4types.okl4nano.debug.arm1136jf_s_r0p4_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_static
        Autogen.mmu = \
            pyokl4.okl4.mmu_static
        modules.append(Autogen.mmu)

        import pyokl4.okl4.cache_control_nano
        Autogen.cache_control = \
            pyokl4.okl4.cache_control_nano
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.futex
        Autogen.futex = \
            pyokl4.okl4.futex
        modules.append(Autogen.futex)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.objmanager_trivial
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_trivial
        modules.append(Autogen.objmanager)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv6.okl4.mmu_static
        Autogen.mmu_arch = \
            pyokl4.arch.armv6.okl4.mmu_static
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv6.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv6.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4nano.debug.arm1136jf_s_r0p4_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4nano.debug.arm1136jf_s_r0p4_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4nano.debug.arm1136jf_s_r0p4_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4nano.debug.arm1136jf_s_r0p4_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4nano.debug.arm1136jf_s_r0p4_vfp.objmanager_trivial
        Autogen.objmanager_autogen = \
           okl4types.okl4nano.debug.arm1136jf_s_r0p4_vfp.objmanager_trivial
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4nano' and arch == 'armv6' and \
            cpu_desc == 'arm1136jf_s_r1p5_vfp' and debug == 'debug':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'hal',
            'mmu',
            'atomic_ops',
            'cache_control',
            'cell_environment',
            'ElfFile',
            'exception',
            'futex',
            'interrupt',
            'kdb_interactive',
            'kdb',
            'kernel',
            'memory_pools',
            'scheduler',
            'notify',
            'objmanager',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'PD',
            'soc',
            'suspend',
            'tracebuffer',
            'useraccess',
        )

        module_names = (
            'allocator',
            'hal',
            'mmu_static',
            'atomic_ops',
            'cache_control_nano',
            'cell_environment',
            'ElfFile',
            'exception_trivial',
            'futex',
            'interrupt_dynamic',
            'kdb_interactive',
            'kdb_standard',
            'kernel_standard',
            'memory_pools',
            'scheduler_standard',
            'notify_standard',
            'objmanager_trivial',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'single_pd',
            'soc_support',
            'suspend',
            'tracebuffer_null',
            'useraccess_static',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4nano.debug.arm1136jf_s_r1p5_vfp.types
        Autogen.types = \
            okl4types.okl4nano.debug.arm1136jf_s_r1p5_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4nano.debug.arm1136jf_s_r1p5_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4nano.debug.arm1136jf_s_r1p5_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4nano.debug.arm1136jf_s_r1p5_vfp.rights
        Autogen.rights = \
            okl4types.okl4nano.debug.arm1136jf_s_r1p5_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_static
        Autogen.mmu = \
            pyokl4.okl4.mmu_static
        modules.append(Autogen.mmu)

        import pyokl4.okl4.cache_control_nano
        Autogen.cache_control = \
            pyokl4.okl4.cache_control_nano
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.futex
        Autogen.futex = \
            pyokl4.okl4.futex
        modules.append(Autogen.futex)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.objmanager_trivial
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_trivial
        modules.append(Autogen.objmanager)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv6.okl4.mmu_static
        Autogen.mmu_arch = \
            pyokl4.arch.armv6.okl4.mmu_static
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv6.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv6.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4nano.debug.arm1136jf_s_r1p5_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4nano.debug.arm1136jf_s_r1p5_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4nano.debug.arm1136jf_s_r1p5_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4nano.debug.arm1136jf_s_r1p5_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4nano.debug.arm1136jf_s_r1p5_vfp.objmanager_trivial
        Autogen.objmanager_autogen = \
           okl4types.okl4nano.debug.arm1136jf_s_r1p5_vfp.objmanager_trivial
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4nano' and arch == 'armv6' and \
            cpu_desc == 'arm1136jf_r1p4' and debug == 'debug':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'hal',
            'mmu',
            'atomic_ops',
            'cache_control',
            'cell_environment',
            'ElfFile',
            'exception',
            'futex',
            'interrupt',
            'kdb_interactive',
            'kdb',
            'kernel',
            'memory_pools',
            'scheduler',
            'notify',
            'objmanager',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'PD',
            'soc',
            'suspend',
            'tracebuffer',
            'useraccess',
        )

        module_names = (
            'allocator',
            'hal',
            'mmu_static',
            'atomic_ops',
            'cache_control_nano',
            'cell_environment',
            'ElfFile',
            'exception_trivial',
            'futex',
            'interrupt_dynamic',
            'kdb_interactive',
            'kdb_standard',
            'kernel_standard',
            'memory_pools',
            'scheduler_standard',
            'notify_standard',
            'objmanager_trivial',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'single_pd',
            'soc_support',
            'suspend',
            'tracebuffer_null',
            'useraccess_static',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4nano.debug.arm1136jf_r1p4.types
        Autogen.types = \
            okl4types.okl4nano.debug.arm1136jf_r1p4.types
        modules.append(Autogen.types)

        import okl4types.okl4nano.debug.arm1136jf_r1p4.machine
        Autogen.machine_info = \
            okl4types.okl4nano.debug.arm1136jf_r1p4.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4nano.debug.arm1136jf_r1p4.rights
        Autogen.rights = \
            okl4types.okl4nano.debug.arm1136jf_r1p4.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_static
        Autogen.mmu = \
            pyokl4.okl4.mmu_static
        modules.append(Autogen.mmu)

        import pyokl4.okl4.cache_control_nano
        Autogen.cache_control = \
            pyokl4.okl4.cache_control_nano
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.futex
        Autogen.futex = \
            pyokl4.okl4.futex
        modules.append(Autogen.futex)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.objmanager_trivial
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_trivial
        modules.append(Autogen.objmanager)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv6.okl4.mmu_static
        Autogen.mmu_arch = \
            pyokl4.arch.armv6.okl4.mmu_static
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv6.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv6.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4nano.debug.arm1136jf_r1p4.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4nano.debug.arm1136jf_r1p4.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4nano.debug.arm1136jf_r1p4.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4nano.debug.arm1136jf_r1p4.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4nano.debug.arm1136jf_r1p4.objmanager_trivial
        Autogen.objmanager_autogen = \
           okl4types.okl4nano.debug.arm1136jf_r1p4.objmanager_trivial
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4nano' and arch == 'armv7' and \
            cpu_desc == 'cortex_a8_r1p2_vfp' and debug == 'debug':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'hal',
            'mmu',
            'atomic_ops',
            'cache_control',
            'cell_environment',
            'ElfFile',
            'exception',
            'futex',
            'interrupt',
            'kdb_interactive',
            'kdb',
            'kernel',
            'memory_pools',
            'scheduler',
            'notify',
            'objmanager',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'PD',
            'soc',
            'suspend',
            'tracebuffer',
            'useraccess',
        )

        module_names = (
            'allocator',
            'hal',
            'mmu_static',
            'atomic_ops',
            'cache_control_nano',
            'cell_environment',
            'ElfFile',
            'exception_trivial',
            'futex',
            'interrupt_dynamic',
            'kdb_interactive',
            'kdb_standard',
            'kernel_standard',
            'memory_pools',
            'scheduler_standard',
            'notify_standard',
            'objmanager_trivial',
            'preemption',
            'processor_control',
            'realms',
            'relocation',
            'single_pd',
            'soc_support',
            'suspend',
            'tracebuffer_null',
            'useraccess_static',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4nano.debug.cortex_a8_r1p2_vfp.types
        Autogen.types = \
            okl4types.okl4nano.debug.cortex_a8_r1p2_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4nano.debug.cortex_a8_r1p2_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4nano.debug.cortex_a8_r1p2_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4nano.debug.cortex_a8_r1p2_vfp.rights
        Autogen.rights = \
            okl4types.okl4nano.debug.cortex_a8_r1p2_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_static
        Autogen.mmu = \
            pyokl4.okl4.mmu_static
        modules.append(Autogen.mmu)

        import pyokl4.okl4.cache_control_nano
        Autogen.cache_control = \
            pyokl4.okl4.cache_control_nano
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.futex
        Autogen.futex = \
            pyokl4.okl4.futex
        modules.append(Autogen.futex)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.objmanager_trivial
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_trivial
        modules.append(Autogen.objmanager)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv7.okl4.mmu_static
        Autogen.mmu_arch = \
            pyokl4.arch.armv7.okl4.mmu_static
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv7.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv7.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4nano.debug.cortex_a8_r1p2_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4nano.debug.cortex_a8_r1p2_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4nano.debug.cortex_a8_r1p2_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4nano.debug.cortex_a8_r1p2_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4nano.debug.cortex_a8_r1p2_vfp.objmanager_trivial
        Autogen.objmanager_autogen = \
           okl4types.okl4nano.debug.cortex_a8_r1p2_vfp.objmanager_trivial
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4pico' and arch == 'armv4' and \
            cpu_desc == 'arm7tdmi_r4p3' and debug == 'production':
        modules = []

        # Composition details.
        feature_names = (
            'hal',
            'atomic_ops',
            'cell_environment',
            'interrupt',
            'kernel',
            'memalloc',
            'mmu',
            'mutex',
            'notify',
            'objmanager',
            'filesystem',
            'libokl4_pico',
            'plugin',
            'scheduler',
            'soc',
            'tracebuffer',
        )

        module_names = (
            'hal',
            'atomic_ops',
            'cell_environment',
            'interrupt_pico',
            'kernel_pico',
            'memalloc',
            'mmu_nommu',
            'mutex_pico',
            'notify_pico',
            'objmanager_pico',
            'filesystem_pico',
            'libokl4_pico',
            'plugin_pico',
            'scheduler_pico',
            'soc_support_pico',
            'tracebuffer_null',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4pico.production.arm7tdmi_r4p3.types
        Autogen.types = \
            okl4types.okl4pico.production.arm7tdmi_r4p3.types
        modules.append(Autogen.types)

        import okl4types.okl4pico.production.arm7tdmi_r4p3.machine
        Autogen.machine_info = \
            okl4types.okl4pico.production.arm7tdmi_r4p3.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4pico.production.arm7tdmi_r4p3.rights
        Autogen.rights = \
            okl4types.okl4pico.production.arm7tdmi_r4p3.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.kernel_pico
        Autogen.kernel = \
            pyokl4.okl4.kernel_pico
        modules.append(Autogen.kernel)

        import pyokl4.okl4.mmu_nommu
        Autogen.mmu = \
            pyokl4.okl4.mmu_nommu
        modules.append(Autogen.mmu)

        import pyokl4.okl4.mutex_pico
        Autogen.mutex = \
            pyokl4.okl4.mutex_pico
        modules.append(Autogen.mutex)

        import pyokl4.okl4.objmanager_pico
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_pico
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.scheduler_pico
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_pico
        modules.append(Autogen.scheduler)


        # Import libokl4 modules.

        # Import arch-specific modules.
        import pyokl4.arch.armv4.okl4.kernel_pico
        Autogen.kernel_arch = \
            pyokl4.arch.armv4.okl4.kernel_pico
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4pico.production.arm7tdmi_r4p3.objmanager_pico
        Autogen.objmanager_autogen = \
           okl4types.okl4pico.production.arm7tdmi_r4p3.objmanager_pico
        modules.append(Autogen.objmanager_autogen)

        import okl4types.okl4pico.production.arm7tdmi_r4p3.scheduler_pico
        Autogen.scheduler_autogen = \
           okl4types.okl4pico.production.arm7tdmi_r4p3.scheduler_pico
        modules.append(Autogen.scheduler_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4pico' and arch == 'armv4' and \
            cpu_desc == 'arm7tdmi_r4p3' and debug == 'debug':
        modules = []

        # Composition details.
        feature_names = (
            'hal',
            'atomic_ops',
            'cell_environment',
            'interrupt',
            'kdb',
            'kernel',
            'memalloc',
            'mmu',
            'mutex',
            'notify',
            'objmanager',
            'filesystem',
            'libokl4_pico',
            'plugin',
            'scheduler',
            'soc',
            'tracebuffer',
        )

        module_names = (
            'hal',
            'atomic_ops',
            'cell_environment',
            'interrupt_pico',
            'kdb_pico',
            'kernel_pico',
            'memalloc',
            'mmu_nommu',
            'mutex_pico',
            'notify_pico',
            'objmanager_pico',
            'filesystem_pico',
            'libokl4_pico',
            'plugin_pico',
            'scheduler_pico',
            'soc_support_pico',
            'tracebuffer_null',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4pico.debug.arm7tdmi_r4p3.types
        Autogen.types = \
            okl4types.okl4pico.debug.arm7tdmi_r4p3.types
        modules.append(Autogen.types)

        import okl4types.okl4pico.debug.arm7tdmi_r4p3.machine
        Autogen.machine_info = \
            okl4types.okl4pico.debug.arm7tdmi_r4p3.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4pico.debug.arm7tdmi_r4p3.rights
        Autogen.rights = \
            okl4types.okl4pico.debug.arm7tdmi_r4p3.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.kernel_pico
        Autogen.kernel = \
            pyokl4.okl4.kernel_pico
        modules.append(Autogen.kernel)

        import pyokl4.okl4.mmu_nommu
        Autogen.mmu = \
            pyokl4.okl4.mmu_nommu
        modules.append(Autogen.mmu)

        import pyokl4.okl4.mutex_pico
        Autogen.mutex = \
            pyokl4.okl4.mutex_pico
        modules.append(Autogen.mutex)

        import pyokl4.okl4.objmanager_pico
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_pico
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.scheduler_pico
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_pico
        modules.append(Autogen.scheduler)


        # Import libokl4 modules.

        # Import arch-specific modules.
        import pyokl4.arch.armv4.okl4.kernel_pico
        Autogen.kernel_arch = \
            pyokl4.arch.armv4.okl4.kernel_pico
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4pico.debug.arm7tdmi_r4p3.objmanager_pico
        Autogen.objmanager_autogen = \
           okl4types.okl4pico.debug.arm7tdmi_r4p3.objmanager_pico
        modules.append(Autogen.objmanager_autogen)

        import okl4types.okl4pico.debug.arm7tdmi_r4p3.scheduler_pico
        Autogen.scheduler_autogen = \
           okl4types.okl4pico.debug.arm7tdmi_r4p3.scheduler_pico
        modules.append(Autogen.scheduler_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4pico' and arch == 'armv7' and \
            cpu_desc == 'cortex_a9mp_r0p1' and debug == 'production':
        modules = []

        # Composition details.
        feature_names = (
            'hal',
            'atomic_ops',
            'cell_environment',
            'interrupt',
            'kernel',
            'memalloc',
            'mmu',
            'mutex',
            'notify',
            'objmanager',
            'filesystem',
            'libokl4_pico',
            'plugin',
            'scheduler',
            'soc',
            'tracebuffer',
        )

        module_names = (
            'hal',
            'atomic_ops',
            'cell_environment',
            'interrupt_pico',
            'kernel_pico',
            'memalloc',
            'mmu_nommu',
            'mutex_pico',
            'notify_pico',
            'objmanager_pico',
            'filesystem_pico',
            'libokl4_pico',
            'plugin_pico',
            'scheduler_pico',
            'soc_support_pico',
            'tracebuffer_null',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4pico.production.cortex_a9mp_r0p1.types
        Autogen.types = \
            okl4types.okl4pico.production.cortex_a9mp_r0p1.types
        modules.append(Autogen.types)

        import okl4types.okl4pico.production.cortex_a9mp_r0p1.machine
        Autogen.machine_info = \
            okl4types.okl4pico.production.cortex_a9mp_r0p1.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4pico.production.cortex_a9mp_r0p1.rights
        Autogen.rights = \
            okl4types.okl4pico.production.cortex_a9mp_r0p1.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.kernel_pico
        Autogen.kernel = \
            pyokl4.okl4.kernel_pico
        modules.append(Autogen.kernel)

        import pyokl4.okl4.mmu_nommu
        Autogen.mmu = \
            pyokl4.okl4.mmu_nommu
        modules.append(Autogen.mmu)

        import pyokl4.okl4.mutex_pico
        Autogen.mutex = \
            pyokl4.okl4.mutex_pico
        modules.append(Autogen.mutex)

        import pyokl4.okl4.objmanager_pico
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_pico
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.scheduler_pico
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_pico
        modules.append(Autogen.scheduler)


        # Import libokl4 modules.

        # Import arch-specific modules.
        import pyokl4.arch.armv7.okl4.kernel_pico
        Autogen.kernel_arch = \
            pyokl4.arch.armv7.okl4.kernel_pico
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4pico.production.cortex_a9mp_r0p1.objmanager_pico
        Autogen.objmanager_autogen = \
           okl4types.okl4pico.production.cortex_a9mp_r0p1.objmanager_pico
        modules.append(Autogen.objmanager_autogen)

        import okl4types.okl4pico.production.cortex_a9mp_r0p1.scheduler_pico
        Autogen.scheduler_autogen = \
           okl4types.okl4pico.production.cortex_a9mp_r0p1.scheduler_pico
        modules.append(Autogen.scheduler_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4pico' and arch == 'armv7' and \
            cpu_desc == 'cortex_a9mp_r0p1' and debug == 'debug':
        modules = []

        # Composition details.
        feature_names = (
            'hal',
            'atomic_ops',
            'cell_environment',
            'interrupt',
            'kdb',
            'kernel',
            'memalloc',
            'mmu',
            'mutex',
            'notify',
            'objmanager',
            'filesystem',
            'libokl4_pico',
            'plugin',
            'scheduler',
            'soc',
            'tracebuffer',
        )

        module_names = (
            'hal',
            'atomic_ops',
            'cell_environment',
            'interrupt_pico',
            'kdb_pico',
            'kernel_pico',
            'memalloc',
            'mmu_nommu',
            'mutex_pico',
            'notify_pico',
            'objmanager_pico',
            'filesystem_pico',
            'libokl4_pico',
            'plugin_pico',
            'scheduler_pico',
            'soc_support_pico',
            'tracebuffer_null',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4pico.debug.cortex_a9mp_r0p1.types
        Autogen.types = \
            okl4types.okl4pico.debug.cortex_a9mp_r0p1.types
        modules.append(Autogen.types)

        import okl4types.okl4pico.debug.cortex_a9mp_r0p1.machine
        Autogen.machine_info = \
            okl4types.okl4pico.debug.cortex_a9mp_r0p1.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4pico.debug.cortex_a9mp_r0p1.rights
        Autogen.rights = \
            okl4types.okl4pico.debug.cortex_a9mp_r0p1.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.kernel_pico
        Autogen.kernel = \
            pyokl4.okl4.kernel_pico
        modules.append(Autogen.kernel)

        import pyokl4.okl4.mmu_nommu
        Autogen.mmu = \
            pyokl4.okl4.mmu_nommu
        modules.append(Autogen.mmu)

        import pyokl4.okl4.mutex_pico
        Autogen.mutex = \
            pyokl4.okl4.mutex_pico
        modules.append(Autogen.mutex)

        import pyokl4.okl4.objmanager_pico
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_pico
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.scheduler_pico
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_pico
        modules.append(Autogen.scheduler)


        # Import libokl4 modules.

        # Import arch-specific modules.
        import pyokl4.arch.armv7.okl4.kernel_pico
        Autogen.kernel_arch = \
            pyokl4.arch.armv7.okl4.kernel_pico
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4pico.debug.cortex_a9mp_r0p1.objmanager_pico
        Autogen.objmanager_autogen = \
           okl4types.okl4pico.debug.cortex_a9mp_r0p1.objmanager_pico
        modules.append(Autogen.objmanager_autogen)

        import okl4types.okl4pico.debug.cortex_a9mp_r0p1.scheduler_pico
        Autogen.scheduler_autogen = \
           okl4types.okl4pico.debug.cortex_a9mp_r0p1.scheduler_pico
        modules.append(Autogen.scheduler_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4extras' and arch == 'armv5' and \
            cpu_desc == 'arm926ej_s_r0p3_vfp' and debug == 'production':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'cpuregisters',
            'mmu',
            'arm_shared_domains',
            'hal',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'callback',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception',
            'protected_library',
            'filesystem',
            'mutex_hybrid',
            'scheduler',
            'intlock',
            'interrupt',
            'ipc',
            'kdb',
            'kernel',
            'libokl4-lock',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'PD',
            'mutex',
            'notify',
            'notify_service',
            'objmanager',
            'open_wait',
            'plugin',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'useraccess',
        )

        module_names = (
            'allocator',
            'cpuregisters',
            'mmu_dynamic',
            'arm_shared_domains',
            'hal',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'callback_simple',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception_ipc',
            'extension',
            'filesystem_standard',
            'mutex_hybrid',
            'scheduler_standard',
            'intlock',
            'interrupt_dynamic',
            'ipc_extended',
            'kdb_standard',
            'kernel_standard',
            'libokl4-lock',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'multi_pd',
            'mutex_dynamic',
            'notify_standard',
            'notify_service',
            'objmanager_simple',
            'open_wait',
            'plugin_standard',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc_support',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'useraccess_dynamic',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4extras.production.arm926ej_s_r0p3_vfp.types
        Autogen.types = \
            okl4types.okl4extras.production.arm926ej_s_r0p3_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4extras.production.arm926ej_s_r0p3_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4extras.production.arm926ej_s_r0p3_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4extras.production.arm926ej_s_r0p3_vfp.rights
        Autogen.rights = \
            okl4types.okl4extras.production.arm926ej_s_r0p3_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_dynamic
        Autogen.mmu = \
            pyokl4.okl4.mmu_dynamic
        modules.append(Autogen.mmu)

        import pyokl4.okl4.arm_vfp
        Autogen.arm_vfp = \
            pyokl4.okl4.arm_vfp
        modules.append(Autogen.arm_vfp)

        import pyokl4.okl4.buffer_area
        Autogen.buffer_area = \
            pyokl4.okl4.buffer_area
        modules.append(Autogen.buffer_area)

        import pyokl4.okl4.cache_control
        Autogen.cache_control = \
            pyokl4.okl4.cache_control
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.critsect
        Autogen.critsect = \
            pyokl4.okl4.critsect
        modules.append(Autogen.critsect)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.intlock
        Autogen.intlock = \
            pyokl4.okl4.intlock
        modules.append(Autogen.intlock)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.ipc_extended
        Autogen.ipc = \
            pyokl4.okl4.ipc_extended
        modules.append(Autogen.ipc)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.message_queue
        Autogen.message_queue = \
            pyokl4.okl4.message_queue
        modules.append(Autogen.message_queue)

        import pyokl4.okl4.mutex_dynamic
        Autogen.mutex = \
            pyokl4.okl4.mutex_dynamic
        modules.append(Autogen.mutex)

        import pyokl4.okl4.notify_service
        Autogen.notify_service = \
            pyokl4.okl4.notify_service
        modules.append(Autogen.notify_service)

        import pyokl4.okl4.objmanager_simple
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_simple
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.segment
        Autogen.segment = \
            pyokl4.okl4.segment
        modules.append(Autogen.segment)

        import pyokl4.okl4.switch_table
        Autogen.switch_table = \
            pyokl4.okl4.switch_table
        modules.append(Autogen.switch_table)

        import pyokl4.okl4.tracebuffer
        Autogen.tracebuffer = \
            pyokl4.okl4.tracebuffer
        modules.append(Autogen.tracebuffer)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv5.okl4.mmu_dynamic
        Autogen.mmu_arch = \
            pyokl4.arch.armv5.okl4.mmu_dynamic
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv5.okl4.arm_shared_domains
        Autogen.arm_shared_domains_arch = \
            pyokl4.arch.armv5.okl4.arm_shared_domains
        modules.append(Autogen.arm_shared_domains_arch)

        import pyokl4.arch.armv5.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv5.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4extras.production.arm926ej_s_r0p3_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4extras.production.arm926ej_s_r0p3_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4extras.production.arm926ej_s_r0p3_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4extras.production.arm926ej_s_r0p3_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4extras.production.arm926ej_s_r0p3_vfp.objmanager_simple
        Autogen.objmanager_autogen = \
           okl4types.okl4extras.production.arm926ej_s_r0p3_vfp.objmanager_simple
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4extras' and arch == 'armv6' and \
            cpu_desc == 'arm1136jf_s_r0p4_vfp' and debug == 'production':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'mmu',
            'cpuregisters',
            'hal',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception',
            'protected_library',
            'filesystem',
            'mutex_hybrid',
            'scheduler',
            'intlock',
            'interrupt',
            'ipc',
            'kdb',
            'kernel',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'PD',
            'mutex',
            'notify',
            'notify_service',
            'objmanager',
            'open_wait',
            'plugin',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'useraccess',
        )

        module_names = (
            'allocator',
            'mmu_dynamic',
            'cpuregisters',
            'hal',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception_ipc',
            'extension',
            'filesystem_standard',
            'mutex_hybrid',
            'scheduler_standard',
            'intlock',
            'interrupt_dynamic',
            'ipc_extended',
            'kdb_standard',
            'kernel_standard',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'multi_pd',
            'mutex_dynamic',
            'notify_standard',
            'notify_service',
            'objmanager_simple',
            'open_wait',
            'plugin_standard',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc_support',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'useraccess_dynamic',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4extras.production.arm1136jf_s_r0p4_vfp.types
        Autogen.types = \
            okl4types.okl4extras.production.arm1136jf_s_r0p4_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4extras.production.arm1136jf_s_r0p4_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4extras.production.arm1136jf_s_r0p4_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4extras.production.arm1136jf_s_r0p4_vfp.rights
        Autogen.rights = \
            okl4types.okl4extras.production.arm1136jf_s_r0p4_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_dynamic
        Autogen.mmu = \
            pyokl4.okl4.mmu_dynamic
        modules.append(Autogen.mmu)

        import pyokl4.okl4.buffer_area
        Autogen.buffer_area = \
            pyokl4.okl4.buffer_area
        modules.append(Autogen.buffer_area)

        import pyokl4.okl4.cache_control
        Autogen.cache_control = \
            pyokl4.okl4.cache_control
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.critsect
        Autogen.critsect = \
            pyokl4.okl4.critsect
        modules.append(Autogen.critsect)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.intlock
        Autogen.intlock = \
            pyokl4.okl4.intlock
        modules.append(Autogen.intlock)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.ipc_extended
        Autogen.ipc = \
            pyokl4.okl4.ipc_extended
        modules.append(Autogen.ipc)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.message_queue
        Autogen.message_queue = \
            pyokl4.okl4.message_queue
        modules.append(Autogen.message_queue)

        import pyokl4.okl4.mutex_dynamic
        Autogen.mutex = \
            pyokl4.okl4.mutex_dynamic
        modules.append(Autogen.mutex)

        import pyokl4.okl4.notify_service
        Autogen.notify_service = \
            pyokl4.okl4.notify_service
        modules.append(Autogen.notify_service)

        import pyokl4.okl4.objmanager_simple
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_simple
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.segment
        Autogen.segment = \
            pyokl4.okl4.segment
        modules.append(Autogen.segment)

        import pyokl4.okl4.switch_table
        Autogen.switch_table = \
            pyokl4.okl4.switch_table
        modules.append(Autogen.switch_table)

        import pyokl4.okl4.tracebuffer
        Autogen.tracebuffer = \
            pyokl4.okl4.tracebuffer
        modules.append(Autogen.tracebuffer)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv6.okl4.mmu_dynamic
        Autogen.mmu_arch = \
            pyokl4.arch.armv6.okl4.mmu_dynamic
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv6.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv6.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4extras.production.arm1136jf_s_r0p4_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4extras.production.arm1136jf_s_r0p4_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4extras.production.arm1136jf_s_r0p4_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4extras.production.arm1136jf_s_r0p4_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4extras.production.arm1136jf_s_r0p4_vfp.objmanager_simple
        Autogen.objmanager_autogen = \
           okl4types.okl4extras.production.arm1136jf_s_r0p4_vfp.objmanager_simple
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4extras' and arch == 'armv6' and \
            cpu_desc == 'arm1136jf_r1p4' and debug == 'production':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'mmu',
            'cpuregisters',
            'hal',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception',
            'protected_library',
            'filesystem',
            'mutex_hybrid',
            'scheduler',
            'intlock',
            'interrupt',
            'ipc',
            'kdb',
            'kernel',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'PD',
            'mutex',
            'notify',
            'notify_service',
            'objmanager',
            'open_wait',
            'plugin',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'useraccess',
        )

        module_names = (
            'allocator',
            'mmu_dynamic',
            'cpuregisters',
            'hal',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception_ipc',
            'extension',
            'filesystem_standard',
            'mutex_hybrid',
            'scheduler_standard',
            'intlock',
            'interrupt_dynamic',
            'ipc_extended',
            'kdb_standard',
            'kernel_standard',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'multi_pd',
            'mutex_dynamic',
            'notify_standard',
            'notify_service',
            'objmanager_simple',
            'open_wait',
            'plugin_standard',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc_support',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'useraccess_dynamic',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4extras.production.arm1136jf_r1p4.types
        Autogen.types = \
            okl4types.okl4extras.production.arm1136jf_r1p4.types
        modules.append(Autogen.types)

        import okl4types.okl4extras.production.arm1136jf_r1p4.machine
        Autogen.machine_info = \
            okl4types.okl4extras.production.arm1136jf_r1p4.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4extras.production.arm1136jf_r1p4.rights
        Autogen.rights = \
            okl4types.okl4extras.production.arm1136jf_r1p4.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_dynamic
        Autogen.mmu = \
            pyokl4.okl4.mmu_dynamic
        modules.append(Autogen.mmu)

        import pyokl4.okl4.buffer_area
        Autogen.buffer_area = \
            pyokl4.okl4.buffer_area
        modules.append(Autogen.buffer_area)

        import pyokl4.okl4.cache_control
        Autogen.cache_control = \
            pyokl4.okl4.cache_control
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.critsect
        Autogen.critsect = \
            pyokl4.okl4.critsect
        modules.append(Autogen.critsect)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.intlock
        Autogen.intlock = \
            pyokl4.okl4.intlock
        modules.append(Autogen.intlock)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.ipc_extended
        Autogen.ipc = \
            pyokl4.okl4.ipc_extended
        modules.append(Autogen.ipc)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.message_queue
        Autogen.message_queue = \
            pyokl4.okl4.message_queue
        modules.append(Autogen.message_queue)

        import pyokl4.okl4.mutex_dynamic
        Autogen.mutex = \
            pyokl4.okl4.mutex_dynamic
        modules.append(Autogen.mutex)

        import pyokl4.okl4.notify_service
        Autogen.notify_service = \
            pyokl4.okl4.notify_service
        modules.append(Autogen.notify_service)

        import pyokl4.okl4.objmanager_simple
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_simple
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.segment
        Autogen.segment = \
            pyokl4.okl4.segment
        modules.append(Autogen.segment)

        import pyokl4.okl4.switch_table
        Autogen.switch_table = \
            pyokl4.okl4.switch_table
        modules.append(Autogen.switch_table)

        import pyokl4.okl4.tracebuffer
        Autogen.tracebuffer = \
            pyokl4.okl4.tracebuffer
        modules.append(Autogen.tracebuffer)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv6.okl4.mmu_dynamic
        Autogen.mmu_arch = \
            pyokl4.arch.armv6.okl4.mmu_dynamic
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv6.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv6.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4extras.production.arm1136jf_r1p4.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4extras.production.arm1136jf_r1p4.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4extras.production.arm1136jf_r1p4.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4extras.production.arm1136jf_r1p4.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4extras.production.arm1136jf_r1p4.objmanager_simple
        Autogen.objmanager_autogen = \
           okl4types.okl4extras.production.arm1136jf_r1p4.objmanager_simple
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4extras' and arch == 'armv6' and \
            cpu_desc == 'arm1136jf_s_r1p5_vfp' and debug == 'production':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'mmu',
            'cpuregisters',
            'hal',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception',
            'protected_library',
            'filesystem',
            'mutex_hybrid',
            'scheduler',
            'intlock',
            'interrupt',
            'ipc',
            'kdb',
            'kernel',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'PD',
            'mutex',
            'notify',
            'notify_service',
            'objmanager',
            'open_wait',
            'plugin',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'useraccess',
        )

        module_names = (
            'allocator',
            'mmu_dynamic',
            'cpuregisters',
            'hal',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception_ipc',
            'extension',
            'filesystem_standard',
            'mutex_hybrid',
            'scheduler_standard',
            'intlock',
            'interrupt_dynamic',
            'ipc_extended',
            'kdb_standard',
            'kernel_standard',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'multi_pd',
            'mutex_dynamic',
            'notify_standard',
            'notify_service',
            'objmanager_simple',
            'open_wait',
            'plugin_standard',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc_support',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'useraccess_dynamic',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4extras.production.arm1136jf_s_r1p5_vfp.types
        Autogen.types = \
            okl4types.okl4extras.production.arm1136jf_s_r1p5_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4extras.production.arm1136jf_s_r1p5_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4extras.production.arm1136jf_s_r1p5_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4extras.production.arm1136jf_s_r1p5_vfp.rights
        Autogen.rights = \
            okl4types.okl4extras.production.arm1136jf_s_r1p5_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_dynamic
        Autogen.mmu = \
            pyokl4.okl4.mmu_dynamic
        modules.append(Autogen.mmu)

        import pyokl4.okl4.buffer_area
        Autogen.buffer_area = \
            pyokl4.okl4.buffer_area
        modules.append(Autogen.buffer_area)

        import pyokl4.okl4.cache_control
        Autogen.cache_control = \
            pyokl4.okl4.cache_control
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.critsect
        Autogen.critsect = \
            pyokl4.okl4.critsect
        modules.append(Autogen.critsect)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.intlock
        Autogen.intlock = \
            pyokl4.okl4.intlock
        modules.append(Autogen.intlock)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.ipc_extended
        Autogen.ipc = \
            pyokl4.okl4.ipc_extended
        modules.append(Autogen.ipc)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.message_queue
        Autogen.message_queue = \
            pyokl4.okl4.message_queue
        modules.append(Autogen.message_queue)

        import pyokl4.okl4.mutex_dynamic
        Autogen.mutex = \
            pyokl4.okl4.mutex_dynamic
        modules.append(Autogen.mutex)

        import pyokl4.okl4.notify_service
        Autogen.notify_service = \
            pyokl4.okl4.notify_service
        modules.append(Autogen.notify_service)

        import pyokl4.okl4.objmanager_simple
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_simple
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.segment
        Autogen.segment = \
            pyokl4.okl4.segment
        modules.append(Autogen.segment)

        import pyokl4.okl4.switch_table
        Autogen.switch_table = \
            pyokl4.okl4.switch_table
        modules.append(Autogen.switch_table)

        import pyokl4.okl4.tracebuffer
        Autogen.tracebuffer = \
            pyokl4.okl4.tracebuffer
        modules.append(Autogen.tracebuffer)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv6.okl4.mmu_dynamic
        Autogen.mmu_arch = \
            pyokl4.arch.armv6.okl4.mmu_dynamic
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv6.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv6.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4extras.production.arm1136jf_s_r1p5_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4extras.production.arm1136jf_s_r1p5_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4extras.production.arm1136jf_s_r1p5_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4extras.production.arm1136jf_s_r1p5_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4extras.production.arm1136jf_s_r1p5_vfp.objmanager_simple
        Autogen.objmanager_autogen = \
           okl4types.okl4extras.production.arm1136jf_s_r1p5_vfp.objmanager_simple
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4extras' and arch == 'armv7' and \
            cpu_desc == 'cortex_a5_r0p0_vfp' and debug == 'production':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'mmu',
            'cpuregisters',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'hal',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception',
            'protected_library',
            'filesystem',
            'mutex_hybrid',
            'scheduler',
            'intlock',
            'interrupt',
            'ipc',
            'kdb',
            'kernel',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'PD',
            'mutex',
            'notify',
            'notify_service',
            'objmanager',
            'open_wait',
            'plugin',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess',
            'vector_page_faults',
        )

        module_names = (
            'allocator',
            'mmu_dynamic',
            'cpuregisters',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'hal',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception_ipc',
            'extension',
            'filesystem_standard',
            'mutex_hybrid',
            'scheduler_standard',
            'intlock',
            'interrupt_dynamic',
            'ipc_extended',
            'kdb_standard',
            'kernel_standard',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'multi_pd',
            'mutex_dynamic',
            'notify_standard',
            'notify_service',
            'objmanager_simple',
            'open_wait',
            'plugin_standard',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc_support',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess_dynamic',
            'vector_page_faults',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4extras.production.cortex_a5_r0p0_vfp.types
        Autogen.types = \
            okl4types.okl4extras.production.cortex_a5_r0p0_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4extras.production.cortex_a5_r0p0_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4extras.production.cortex_a5_r0p0_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4extras.production.cortex_a5_r0p0_vfp.rights
        Autogen.rights = \
            okl4types.okl4extras.production.cortex_a5_r0p0_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_dynamic
        Autogen.mmu = \
            pyokl4.okl4.mmu_dynamic
        modules.append(Autogen.mmu)

        import pyokl4.okl4.arm_vfp
        Autogen.arm_vfp = \
            pyokl4.okl4.arm_vfp
        modules.append(Autogen.arm_vfp)

        import pyokl4.okl4.buffer_area
        Autogen.buffer_area = \
            pyokl4.okl4.buffer_area
        modules.append(Autogen.buffer_area)

        import pyokl4.okl4.cache_control
        Autogen.cache_control = \
            pyokl4.okl4.cache_control
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.critsect
        Autogen.critsect = \
            pyokl4.okl4.critsect
        modules.append(Autogen.critsect)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.intlock
        Autogen.intlock = \
            pyokl4.okl4.intlock
        modules.append(Autogen.intlock)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.ipc_extended
        Autogen.ipc = \
            pyokl4.okl4.ipc_extended
        modules.append(Autogen.ipc)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.message_queue
        Autogen.message_queue = \
            pyokl4.okl4.message_queue
        modules.append(Autogen.message_queue)

        import pyokl4.okl4.mutex_dynamic
        Autogen.mutex = \
            pyokl4.okl4.mutex_dynamic
        modules.append(Autogen.mutex)

        import pyokl4.okl4.notify_service
        Autogen.notify_service = \
            pyokl4.okl4.notify_service
        modules.append(Autogen.notify_service)

        import pyokl4.okl4.objmanager_simple
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_simple
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.segment
        Autogen.segment = \
            pyokl4.okl4.segment
        modules.append(Autogen.segment)

        import pyokl4.okl4.switch_table
        Autogen.switch_table = \
            pyokl4.okl4.switch_table
        modules.append(Autogen.switch_table)

        import pyokl4.okl4.tracebuffer
        Autogen.tracebuffer = \
            pyokl4.okl4.tracebuffer
        modules.append(Autogen.tracebuffer)

        import pyokl4.okl4.trustzone
        Autogen.trustzone = \
            pyokl4.okl4.trustzone
        modules.append(Autogen.trustzone)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv7.okl4.mmu_dynamic
        Autogen.mmu_arch = \
            pyokl4.arch.armv7.okl4.mmu_dynamic
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv7.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv7.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4extras.production.cortex_a5_r0p0_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4extras.production.cortex_a5_r0p0_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4extras.production.cortex_a5_r0p0_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4extras.production.cortex_a5_r0p0_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4extras.production.cortex_a5_r0p0_vfp.objmanager_simple
        Autogen.objmanager_autogen = \
           okl4types.okl4extras.production.cortex_a5_r0p0_vfp.objmanager_simple
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4extras' and arch == 'armv7' and \
            cpu_desc == 'cortex_a8_r1p2_vfp' and debug == 'production':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'mmu',
            'cpuregisters',
            'hal',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception',
            'protected_library',
            'filesystem',
            'mutex_hybrid',
            'scheduler',
            'intlock',
            'interrupt',
            'ipc',
            'kdb',
            'kernel',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'PD',
            'mutex',
            'notify',
            'notify_service',
            'objmanager',
            'open_wait',
            'plugin',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess',
            'vector_page_faults',
        )

        module_names = (
            'allocator',
            'mmu_dynamic',
            'cpuregisters',
            'hal',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception_ipc',
            'extension',
            'filesystem_standard',
            'mutex_hybrid',
            'scheduler_standard',
            'intlock',
            'interrupt_dynamic',
            'ipc_extended',
            'kdb_standard',
            'kernel_standard',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'multi_pd',
            'mutex_dynamic',
            'notify_standard',
            'notify_service',
            'objmanager_simple',
            'open_wait',
            'plugin_standard',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc_support',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess_dynamic',
            'vector_page_faults',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4extras.production.cortex_a8_r1p2_vfp.types
        Autogen.types = \
            okl4types.okl4extras.production.cortex_a8_r1p2_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4extras.production.cortex_a8_r1p2_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4extras.production.cortex_a8_r1p2_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4extras.production.cortex_a8_r1p2_vfp.rights
        Autogen.rights = \
            okl4types.okl4extras.production.cortex_a8_r1p2_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_dynamic
        Autogen.mmu = \
            pyokl4.okl4.mmu_dynamic
        modules.append(Autogen.mmu)

        import pyokl4.okl4.arm_vfp
        Autogen.arm_vfp = \
            pyokl4.okl4.arm_vfp
        modules.append(Autogen.arm_vfp)

        import pyokl4.okl4.buffer_area
        Autogen.buffer_area = \
            pyokl4.okl4.buffer_area
        modules.append(Autogen.buffer_area)

        import pyokl4.okl4.cache_control
        Autogen.cache_control = \
            pyokl4.okl4.cache_control
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.critsect
        Autogen.critsect = \
            pyokl4.okl4.critsect
        modules.append(Autogen.critsect)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.intlock
        Autogen.intlock = \
            pyokl4.okl4.intlock
        modules.append(Autogen.intlock)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.ipc_extended
        Autogen.ipc = \
            pyokl4.okl4.ipc_extended
        modules.append(Autogen.ipc)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.message_queue
        Autogen.message_queue = \
            pyokl4.okl4.message_queue
        modules.append(Autogen.message_queue)

        import pyokl4.okl4.mutex_dynamic
        Autogen.mutex = \
            pyokl4.okl4.mutex_dynamic
        modules.append(Autogen.mutex)

        import pyokl4.okl4.notify_service
        Autogen.notify_service = \
            pyokl4.okl4.notify_service
        modules.append(Autogen.notify_service)

        import pyokl4.okl4.objmanager_simple
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_simple
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.segment
        Autogen.segment = \
            pyokl4.okl4.segment
        modules.append(Autogen.segment)

        import pyokl4.okl4.switch_table
        Autogen.switch_table = \
            pyokl4.okl4.switch_table
        modules.append(Autogen.switch_table)

        import pyokl4.okl4.tracebuffer
        Autogen.tracebuffer = \
            pyokl4.okl4.tracebuffer
        modules.append(Autogen.tracebuffer)

        import pyokl4.okl4.trustzone
        Autogen.trustzone = \
            pyokl4.okl4.trustzone
        modules.append(Autogen.trustzone)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv7.okl4.mmu_dynamic
        Autogen.mmu_arch = \
            pyokl4.arch.armv7.okl4.mmu_dynamic
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv7.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv7.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4extras.production.cortex_a8_r1p2_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4extras.production.cortex_a8_r1p2_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4extras.production.cortex_a8_r1p2_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4extras.production.cortex_a8_r1p2_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4extras.production.cortex_a8_r1p2_vfp.objmanager_simple
        Autogen.objmanager_autogen = \
           okl4types.okl4extras.production.cortex_a8_r1p2_vfp.objmanager_simple
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4extras' and arch == 'armv5' and \
            cpu_desc == 'arm926ej_s_r0p3_vfp' and debug == 'debug':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'cpuregisters',
            'mmu',
            'arm_shared_domains',
            'hal',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'callback',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception',
            'protected_library',
            'filesystem',
            'mutex_hybrid',
            'scheduler',
            'intlock',
            'interrupt',
            'ipc',
            'kdb_interactive',
            'kdb',
            'kernel',
            'libokl4-lock',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'PD',
            'mutex',
            'notify',
            'notify_service',
            'objmanager',
            'open_wait',
            'plugin',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'useraccess',
        )

        module_names = (
            'allocator',
            'cpuregisters',
            'mmu_dynamic',
            'arm_shared_domains',
            'hal',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'callback_simple',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception_ipc',
            'extension',
            'filesystem_standard',
            'mutex_hybrid',
            'scheduler_standard',
            'intlock',
            'interrupt_dynamic',
            'ipc_extended',
            'kdb_interactive',
            'kdb_standard',
            'kernel_standard',
            'libokl4-lock',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'multi_pd',
            'mutex_dynamic',
            'notify_standard',
            'notify_service',
            'objmanager_simple',
            'open_wait',
            'plugin_standard',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc_support',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'useraccess_dynamic',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4extras.debug.arm926ej_s_r0p3_vfp.types
        Autogen.types = \
            okl4types.okl4extras.debug.arm926ej_s_r0p3_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4extras.debug.arm926ej_s_r0p3_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4extras.debug.arm926ej_s_r0p3_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4extras.debug.arm926ej_s_r0p3_vfp.rights
        Autogen.rights = \
            okl4types.okl4extras.debug.arm926ej_s_r0p3_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_dynamic
        Autogen.mmu = \
            pyokl4.okl4.mmu_dynamic
        modules.append(Autogen.mmu)

        import pyokl4.okl4.arm_vfp
        Autogen.arm_vfp = \
            pyokl4.okl4.arm_vfp
        modules.append(Autogen.arm_vfp)

        import pyokl4.okl4.buffer_area
        Autogen.buffer_area = \
            pyokl4.okl4.buffer_area
        modules.append(Autogen.buffer_area)

        import pyokl4.okl4.cache_control
        Autogen.cache_control = \
            pyokl4.okl4.cache_control
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.critsect
        Autogen.critsect = \
            pyokl4.okl4.critsect
        modules.append(Autogen.critsect)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.intlock
        Autogen.intlock = \
            pyokl4.okl4.intlock
        modules.append(Autogen.intlock)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.ipc_extended
        Autogen.ipc = \
            pyokl4.okl4.ipc_extended
        modules.append(Autogen.ipc)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.message_queue
        Autogen.message_queue = \
            pyokl4.okl4.message_queue
        modules.append(Autogen.message_queue)

        import pyokl4.okl4.mutex_dynamic
        Autogen.mutex = \
            pyokl4.okl4.mutex_dynamic
        modules.append(Autogen.mutex)

        import pyokl4.okl4.notify_service
        Autogen.notify_service = \
            pyokl4.okl4.notify_service
        modules.append(Autogen.notify_service)

        import pyokl4.okl4.objmanager_simple
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_simple
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.segment
        Autogen.segment = \
            pyokl4.okl4.segment
        modules.append(Autogen.segment)

        import pyokl4.okl4.switch_table
        Autogen.switch_table = \
            pyokl4.okl4.switch_table
        modules.append(Autogen.switch_table)

        import pyokl4.okl4.tracebuffer
        Autogen.tracebuffer = \
            pyokl4.okl4.tracebuffer
        modules.append(Autogen.tracebuffer)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv5.okl4.mmu_dynamic
        Autogen.mmu_arch = \
            pyokl4.arch.armv5.okl4.mmu_dynamic
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv5.okl4.arm_shared_domains
        Autogen.arm_shared_domains_arch = \
            pyokl4.arch.armv5.okl4.arm_shared_domains
        modules.append(Autogen.arm_shared_domains_arch)

        import pyokl4.arch.armv5.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv5.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4extras.debug.arm926ej_s_r0p3_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4extras.debug.arm926ej_s_r0p3_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4extras.debug.arm926ej_s_r0p3_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4extras.debug.arm926ej_s_r0p3_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4extras.debug.arm926ej_s_r0p3_vfp.objmanager_simple
        Autogen.objmanager_autogen = \
           okl4types.okl4extras.debug.arm926ej_s_r0p3_vfp.objmanager_simple
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4extras' and arch == 'armv6' and \
            cpu_desc == 'arm1136jf_s_r0p4_vfp' and debug == 'debug':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'mmu',
            'cpuregisters',
            'hal',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception',
            'protected_library',
            'filesystem',
            'mutex_hybrid',
            'scheduler',
            'intlock',
            'interrupt',
            'ipc',
            'kdb_interactive',
            'kdb',
            'kernel',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'PD',
            'mutex',
            'notify',
            'notify_service',
            'objmanager',
            'open_wait',
            'plugin',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'useraccess',
        )

        module_names = (
            'allocator',
            'mmu_dynamic',
            'cpuregisters',
            'hal',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception_ipc',
            'extension',
            'filesystem_standard',
            'mutex_hybrid',
            'scheduler_standard',
            'intlock',
            'interrupt_dynamic',
            'ipc_extended',
            'kdb_interactive',
            'kdb_standard',
            'kernel_standard',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'multi_pd',
            'mutex_dynamic',
            'notify_standard',
            'notify_service',
            'objmanager_simple',
            'open_wait',
            'plugin_standard',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc_support',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'useraccess_dynamic',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4extras.debug.arm1136jf_s_r0p4_vfp.types
        Autogen.types = \
            okl4types.okl4extras.debug.arm1136jf_s_r0p4_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4extras.debug.arm1136jf_s_r0p4_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4extras.debug.arm1136jf_s_r0p4_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4extras.debug.arm1136jf_s_r0p4_vfp.rights
        Autogen.rights = \
            okl4types.okl4extras.debug.arm1136jf_s_r0p4_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_dynamic
        Autogen.mmu = \
            pyokl4.okl4.mmu_dynamic
        modules.append(Autogen.mmu)

        import pyokl4.okl4.buffer_area
        Autogen.buffer_area = \
            pyokl4.okl4.buffer_area
        modules.append(Autogen.buffer_area)

        import pyokl4.okl4.cache_control
        Autogen.cache_control = \
            pyokl4.okl4.cache_control
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.critsect
        Autogen.critsect = \
            pyokl4.okl4.critsect
        modules.append(Autogen.critsect)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.intlock
        Autogen.intlock = \
            pyokl4.okl4.intlock
        modules.append(Autogen.intlock)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.ipc_extended
        Autogen.ipc = \
            pyokl4.okl4.ipc_extended
        modules.append(Autogen.ipc)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.message_queue
        Autogen.message_queue = \
            pyokl4.okl4.message_queue
        modules.append(Autogen.message_queue)

        import pyokl4.okl4.mutex_dynamic
        Autogen.mutex = \
            pyokl4.okl4.mutex_dynamic
        modules.append(Autogen.mutex)

        import pyokl4.okl4.notify_service
        Autogen.notify_service = \
            pyokl4.okl4.notify_service
        modules.append(Autogen.notify_service)

        import pyokl4.okl4.objmanager_simple
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_simple
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.segment
        Autogen.segment = \
            pyokl4.okl4.segment
        modules.append(Autogen.segment)

        import pyokl4.okl4.switch_table
        Autogen.switch_table = \
            pyokl4.okl4.switch_table
        modules.append(Autogen.switch_table)

        import pyokl4.okl4.tracebuffer
        Autogen.tracebuffer = \
            pyokl4.okl4.tracebuffer
        modules.append(Autogen.tracebuffer)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv6.okl4.mmu_dynamic
        Autogen.mmu_arch = \
            pyokl4.arch.armv6.okl4.mmu_dynamic
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv6.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv6.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4extras.debug.arm1136jf_s_r0p4_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4extras.debug.arm1136jf_s_r0p4_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4extras.debug.arm1136jf_s_r0p4_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4extras.debug.arm1136jf_s_r0p4_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4extras.debug.arm1136jf_s_r0p4_vfp.objmanager_simple
        Autogen.objmanager_autogen = \
           okl4types.okl4extras.debug.arm1136jf_s_r0p4_vfp.objmanager_simple
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4extras' and arch == 'armv6' and \
            cpu_desc == 'arm1136jf_r1p4_vfp' and debug == 'debug':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'mmu',
            'cpuregisters',
            'hal',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception',
            'protected_library',
            'filesystem',
            'mutex_hybrid',
            'scheduler',
            'intlock',
            'interrupt',
            'ipc',
            'kdb_interactive',
            'kdb',
            'kernel',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'PD',
            'mutex',
            'notify',
            'notify_service',
            'objmanager',
            'open_wait',
            'plugin',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'useraccess',
        )

        module_names = (
            'allocator',
            'mmu_dynamic',
            'cpuregisters',
            'hal',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception_ipc',
            'extension',
            'filesystem_standard',
            'mutex_hybrid',
            'scheduler_standard',
            'intlock',
            'interrupt_dynamic',
            'ipc_extended',
            'kdb_interactive',
            'kdb_standard',
            'kernel_standard',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'multi_pd',
            'mutex_dynamic',
            'notify_standard',
            'notify_service',
            'objmanager_simple',
            'open_wait',
            'plugin_standard',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc_support',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'useraccess_dynamic',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4extras.debug.arm1136jf_r1p4_vfp.types
        Autogen.types = \
            okl4types.okl4extras.debug.arm1136jf_r1p4_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4extras.debug.arm1136jf_r1p4_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4extras.debug.arm1136jf_r1p4_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4extras.debug.arm1136jf_r1p4_vfp.rights
        Autogen.rights = \
            okl4types.okl4extras.debug.arm1136jf_r1p4_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_dynamic
        Autogen.mmu = \
            pyokl4.okl4.mmu_dynamic
        modules.append(Autogen.mmu)

        import pyokl4.okl4.buffer_area
        Autogen.buffer_area = \
            pyokl4.okl4.buffer_area
        modules.append(Autogen.buffer_area)

        import pyokl4.okl4.cache_control
        Autogen.cache_control = \
            pyokl4.okl4.cache_control
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.critsect
        Autogen.critsect = \
            pyokl4.okl4.critsect
        modules.append(Autogen.critsect)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.intlock
        Autogen.intlock = \
            pyokl4.okl4.intlock
        modules.append(Autogen.intlock)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.ipc_extended
        Autogen.ipc = \
            pyokl4.okl4.ipc_extended
        modules.append(Autogen.ipc)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.message_queue
        Autogen.message_queue = \
            pyokl4.okl4.message_queue
        modules.append(Autogen.message_queue)

        import pyokl4.okl4.mutex_dynamic
        Autogen.mutex = \
            pyokl4.okl4.mutex_dynamic
        modules.append(Autogen.mutex)

        import pyokl4.okl4.notify_service
        Autogen.notify_service = \
            pyokl4.okl4.notify_service
        modules.append(Autogen.notify_service)

        import pyokl4.okl4.objmanager_simple
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_simple
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.segment
        Autogen.segment = \
            pyokl4.okl4.segment
        modules.append(Autogen.segment)

        import pyokl4.okl4.switch_table
        Autogen.switch_table = \
            pyokl4.okl4.switch_table
        modules.append(Autogen.switch_table)

        import pyokl4.okl4.tracebuffer
        Autogen.tracebuffer = \
            pyokl4.okl4.tracebuffer
        modules.append(Autogen.tracebuffer)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv6.okl4.mmu_dynamic
        Autogen.mmu_arch = \
            pyokl4.arch.armv6.okl4.mmu_dynamic
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv6.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv6.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4extras.debug.arm1136jf_r1p4_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4extras.debug.arm1136jf_r1p4_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4extras.debug.arm1136jf_r1p4_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4extras.debug.arm1136jf_r1p4_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4extras.debug.arm1136jf_r1p4_vfp.objmanager_simple
        Autogen.objmanager_autogen = \
           okl4types.okl4extras.debug.arm1136jf_r1p4_vfp.objmanager_simple
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4extras' and arch == 'armv6' and \
            cpu_desc == 'arm1136jf_s_r1p5_vfp' and debug == 'debug':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'mmu',
            'cpuregisters',
            'hal',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception',
            'protected_library',
            'filesystem',
            'mutex_hybrid',
            'scheduler',
            'intlock',
            'interrupt',
            'ipc',
            'kdb_interactive',
            'kdb',
            'kernel',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'PD',
            'mutex',
            'notify',
            'notify_service',
            'objmanager',
            'open_wait',
            'plugin',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'useraccess',
        )

        module_names = (
            'allocator',
            'mmu_dynamic',
            'cpuregisters',
            'hal',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception_ipc',
            'extension',
            'filesystem_standard',
            'mutex_hybrid',
            'scheduler_standard',
            'intlock',
            'interrupt_dynamic',
            'ipc_extended',
            'kdb_interactive',
            'kdb_standard',
            'kernel_standard',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'multi_pd',
            'mutex_dynamic',
            'notify_standard',
            'notify_service',
            'objmanager_simple',
            'open_wait',
            'plugin_standard',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc_support',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'useraccess_dynamic',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4extras.debug.arm1136jf_s_r1p5_vfp.types
        Autogen.types = \
            okl4types.okl4extras.debug.arm1136jf_s_r1p5_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4extras.debug.arm1136jf_s_r1p5_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4extras.debug.arm1136jf_s_r1p5_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4extras.debug.arm1136jf_s_r1p5_vfp.rights
        Autogen.rights = \
            okl4types.okl4extras.debug.arm1136jf_s_r1p5_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_dynamic
        Autogen.mmu = \
            pyokl4.okl4.mmu_dynamic
        modules.append(Autogen.mmu)

        import pyokl4.okl4.buffer_area
        Autogen.buffer_area = \
            pyokl4.okl4.buffer_area
        modules.append(Autogen.buffer_area)

        import pyokl4.okl4.cache_control
        Autogen.cache_control = \
            pyokl4.okl4.cache_control
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.critsect
        Autogen.critsect = \
            pyokl4.okl4.critsect
        modules.append(Autogen.critsect)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.intlock
        Autogen.intlock = \
            pyokl4.okl4.intlock
        modules.append(Autogen.intlock)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.ipc_extended
        Autogen.ipc = \
            pyokl4.okl4.ipc_extended
        modules.append(Autogen.ipc)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.message_queue
        Autogen.message_queue = \
            pyokl4.okl4.message_queue
        modules.append(Autogen.message_queue)

        import pyokl4.okl4.mutex_dynamic
        Autogen.mutex = \
            pyokl4.okl4.mutex_dynamic
        modules.append(Autogen.mutex)

        import pyokl4.okl4.notify_service
        Autogen.notify_service = \
            pyokl4.okl4.notify_service
        modules.append(Autogen.notify_service)

        import pyokl4.okl4.objmanager_simple
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_simple
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.segment
        Autogen.segment = \
            pyokl4.okl4.segment
        modules.append(Autogen.segment)

        import pyokl4.okl4.switch_table
        Autogen.switch_table = \
            pyokl4.okl4.switch_table
        modules.append(Autogen.switch_table)

        import pyokl4.okl4.tracebuffer
        Autogen.tracebuffer = \
            pyokl4.okl4.tracebuffer
        modules.append(Autogen.tracebuffer)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv6.okl4.mmu_dynamic
        Autogen.mmu_arch = \
            pyokl4.arch.armv6.okl4.mmu_dynamic
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv6.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv6.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4extras.debug.arm1136jf_s_r1p5_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4extras.debug.arm1136jf_s_r1p5_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4extras.debug.arm1136jf_s_r1p5_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4extras.debug.arm1136jf_s_r1p5_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4extras.debug.arm1136jf_s_r1p5_vfp.objmanager_simple
        Autogen.objmanager_autogen = \
           okl4types.okl4extras.debug.arm1136jf_s_r1p5_vfp.objmanager_simple
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4extras' and arch == 'armv7' and \
            cpu_desc == 'cortex_a5_r0p0_vfp' and debug == 'debug':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'mmu',
            'cpuregisters',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'hal',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception',
            'protected_library',
            'filesystem',
            'mutex_hybrid',
            'scheduler',
            'intlock',
            'interrupt',
            'ipc',
            'kdb_interactive',
            'kdb',
            'kernel',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'PD',
            'mutex',
            'notify',
            'notify_service',
            'objmanager',
            'open_wait',
            'plugin',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess',
            'vector_page_faults',
        )

        module_names = (
            'allocator',
            'mmu_dynamic',
            'cpuregisters',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'hal',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception_ipc',
            'extension',
            'filesystem_standard',
            'mutex_hybrid',
            'scheduler_standard',
            'intlock',
            'interrupt_dynamic',
            'ipc_extended',
            'kdb_interactive',
            'kdb_standard',
            'kernel_standard',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'multi_pd',
            'mutex_dynamic',
            'notify_standard',
            'notify_service',
            'objmanager_simple',
            'open_wait',
            'plugin_standard',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc_support',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess_dynamic',
            'vector_page_faults',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4extras.debug.cortex_a5_r0p0_vfp.types
        Autogen.types = \
            okl4types.okl4extras.debug.cortex_a5_r0p0_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4extras.debug.cortex_a5_r0p0_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4extras.debug.cortex_a5_r0p0_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4extras.debug.cortex_a5_r0p0_vfp.rights
        Autogen.rights = \
            okl4types.okl4extras.debug.cortex_a5_r0p0_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_dynamic
        Autogen.mmu = \
            pyokl4.okl4.mmu_dynamic
        modules.append(Autogen.mmu)

        import pyokl4.okl4.arm_vfp
        Autogen.arm_vfp = \
            pyokl4.okl4.arm_vfp
        modules.append(Autogen.arm_vfp)

        import pyokl4.okl4.buffer_area
        Autogen.buffer_area = \
            pyokl4.okl4.buffer_area
        modules.append(Autogen.buffer_area)

        import pyokl4.okl4.cache_control
        Autogen.cache_control = \
            pyokl4.okl4.cache_control
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.critsect
        Autogen.critsect = \
            pyokl4.okl4.critsect
        modules.append(Autogen.critsect)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.intlock
        Autogen.intlock = \
            pyokl4.okl4.intlock
        modules.append(Autogen.intlock)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.ipc_extended
        Autogen.ipc = \
            pyokl4.okl4.ipc_extended
        modules.append(Autogen.ipc)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.message_queue
        Autogen.message_queue = \
            pyokl4.okl4.message_queue
        modules.append(Autogen.message_queue)

        import pyokl4.okl4.mutex_dynamic
        Autogen.mutex = \
            pyokl4.okl4.mutex_dynamic
        modules.append(Autogen.mutex)

        import pyokl4.okl4.notify_service
        Autogen.notify_service = \
            pyokl4.okl4.notify_service
        modules.append(Autogen.notify_service)

        import pyokl4.okl4.objmanager_simple
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_simple
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.segment
        Autogen.segment = \
            pyokl4.okl4.segment
        modules.append(Autogen.segment)

        import pyokl4.okl4.switch_table
        Autogen.switch_table = \
            pyokl4.okl4.switch_table
        modules.append(Autogen.switch_table)

        import pyokl4.okl4.tracebuffer
        Autogen.tracebuffer = \
            pyokl4.okl4.tracebuffer
        modules.append(Autogen.tracebuffer)

        import pyokl4.okl4.trustzone
        Autogen.trustzone = \
            pyokl4.okl4.trustzone
        modules.append(Autogen.trustzone)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv7.okl4.mmu_dynamic
        Autogen.mmu_arch = \
            pyokl4.arch.armv7.okl4.mmu_dynamic
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv7.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv7.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4extras.debug.cortex_a5_r0p0_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4extras.debug.cortex_a5_r0p0_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4extras.debug.cortex_a5_r0p0_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4extras.debug.cortex_a5_r0p0_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4extras.debug.cortex_a5_r0p0_vfp.objmanager_simple
        Autogen.objmanager_autogen = \
           okl4types.okl4extras.debug.cortex_a5_r0p0_vfp.objmanager_simple
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4extras' and arch == 'armv7' and \
            cpu_desc == 'cortex_a5_r0p0' and debug == 'debug':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'mmu',
            'cpuregisters',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'hal',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception',
            'protected_library',
            'filesystem',
            'mutex_hybrid',
            'scheduler',
            'intlock',
            'interrupt',
            'ipc',
            'kdb_interactive',
            'kdb',
            'kernel',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'PD',
            'mutex',
            'notify',
            'notify_service',
            'objmanager',
            'open_wait',
            'plugin',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess',
            'vector_page_faults',
        )

        module_names = (
            'allocator',
            'mmu_dynamic',
            'cpuregisters',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'hal',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception_ipc',
            'extension',
            'filesystem_standard',
            'mutex_hybrid',
            'scheduler_standard',
            'intlock',
            'interrupt_dynamic',
            'ipc_extended',
            'kdb_interactive',
            'kdb_standard',
            'kernel_standard',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'multi_pd',
            'mutex_dynamic',
            'notify_standard',
            'notify_service',
            'objmanager_simple',
            'open_wait',
            'plugin_standard',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc_support',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess_dynamic',
            'vector_page_faults',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4extras.debug.cortex_a5_r0p0.types
        Autogen.types = \
            okl4types.okl4extras.debug.cortex_a5_r0p0.types
        modules.append(Autogen.types)

        import okl4types.okl4extras.debug.cortex_a5_r0p0.machine
        Autogen.machine_info = \
            okl4types.okl4extras.debug.cortex_a5_r0p0.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4extras.debug.cortex_a5_r0p0.rights
        Autogen.rights = \
            okl4types.okl4extras.debug.cortex_a5_r0p0.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_dynamic
        Autogen.mmu = \
            pyokl4.okl4.mmu_dynamic
        modules.append(Autogen.mmu)

        import pyokl4.okl4.buffer_area
        Autogen.buffer_area = \
            pyokl4.okl4.buffer_area
        modules.append(Autogen.buffer_area)

        import pyokl4.okl4.cache_control
        Autogen.cache_control = \
            pyokl4.okl4.cache_control
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.critsect
        Autogen.critsect = \
            pyokl4.okl4.critsect
        modules.append(Autogen.critsect)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.intlock
        Autogen.intlock = \
            pyokl4.okl4.intlock
        modules.append(Autogen.intlock)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.ipc_extended
        Autogen.ipc = \
            pyokl4.okl4.ipc_extended
        modules.append(Autogen.ipc)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.message_queue
        Autogen.message_queue = \
            pyokl4.okl4.message_queue
        modules.append(Autogen.message_queue)

        import pyokl4.okl4.mutex_dynamic
        Autogen.mutex = \
            pyokl4.okl4.mutex_dynamic
        modules.append(Autogen.mutex)

        import pyokl4.okl4.notify_service
        Autogen.notify_service = \
            pyokl4.okl4.notify_service
        modules.append(Autogen.notify_service)

        import pyokl4.okl4.objmanager_simple
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_simple
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.segment
        Autogen.segment = \
            pyokl4.okl4.segment
        modules.append(Autogen.segment)

        import pyokl4.okl4.switch_table
        Autogen.switch_table = \
            pyokl4.okl4.switch_table
        modules.append(Autogen.switch_table)

        import pyokl4.okl4.tracebuffer
        Autogen.tracebuffer = \
            pyokl4.okl4.tracebuffer
        modules.append(Autogen.tracebuffer)

        import pyokl4.okl4.trustzone
        Autogen.trustzone = \
            pyokl4.okl4.trustzone
        modules.append(Autogen.trustzone)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv7.okl4.mmu_dynamic
        Autogen.mmu_arch = \
            pyokl4.arch.armv7.okl4.mmu_dynamic
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv7.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv7.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4extras.debug.cortex_a5_r0p0.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4extras.debug.cortex_a5_r0p0.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4extras.debug.cortex_a5_r0p0.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4extras.debug.cortex_a5_r0p0.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4extras.debug.cortex_a5_r0p0.objmanager_simple
        Autogen.objmanager_autogen = \
           okl4types.okl4extras.debug.cortex_a5_r0p0.objmanager_simple
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4extras' and arch == 'armv7' and \
            cpu_desc == 'cortex_a7_r0p0' and debug == 'debug':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'mmu',
            'cpuregisters',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'hal',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception',
            'protected_library',
            'filesystem',
            'mutex_hybrid',
            'scheduler',
            'intlock',
            'interrupt',
            'ipc',
            'kdb_interactive',
            'kdb',
            'kernel',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'PD',
            'mutex',
            'notify',
            'notify_service',
            'objmanager',
            'open_wait',
            'plugin',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess',
            'vector_page_faults',
        )

        module_names = (
            'allocator',
            'mmu_dynamic',
            'cpuregisters',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'hal',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception_ipc',
            'extension',
            'filesystem_standard',
            'mutex_hybrid',
            'scheduler_standard',
            'intlock',
            'interrupt_dynamic',
            'ipc_extended',
            'kdb_interactive',
            'kdb_standard',
            'kernel_standard',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'multi_pd',
            'mutex_dynamic',
            'notify_standard',
            'notify_service',
            'objmanager_simple',
            'open_wait',
            'plugin_standard',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc_support',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess_dynamic',
            'vector_page_faults',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4extras.debug.cortex_a7_r0p0.types
        Autogen.types = \
            okl4types.okl4extras.debug.cortex_a7_r0p0.types
        modules.append(Autogen.types)

        import okl4types.okl4extras.debug.cortex_a7_r0p0.machine
        Autogen.machine_info = \
            okl4types.okl4extras.debug.cortex_a7_r0p0.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4extras.debug.cortex_a7_r0p0.rights
        Autogen.rights = \
            okl4types.okl4extras.debug.cortex_a7_r0p0.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_dynamic
        Autogen.mmu = \
            pyokl4.okl4.mmu_dynamic
        modules.append(Autogen.mmu)

        import pyokl4.okl4.buffer_area
        Autogen.buffer_area = \
            pyokl4.okl4.buffer_area
        modules.append(Autogen.buffer_area)

        import pyokl4.okl4.cache_control
        Autogen.cache_control = \
            pyokl4.okl4.cache_control
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.critsect
        Autogen.critsect = \
            pyokl4.okl4.critsect
        modules.append(Autogen.critsect)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.intlock
        Autogen.intlock = \
            pyokl4.okl4.intlock
        modules.append(Autogen.intlock)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.ipc_extended
        Autogen.ipc = \
            pyokl4.okl4.ipc_extended
        modules.append(Autogen.ipc)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.message_queue
        Autogen.message_queue = \
            pyokl4.okl4.message_queue
        modules.append(Autogen.message_queue)

        import pyokl4.okl4.mutex_dynamic
        Autogen.mutex = \
            pyokl4.okl4.mutex_dynamic
        modules.append(Autogen.mutex)

        import pyokl4.okl4.notify_service
        Autogen.notify_service = \
            pyokl4.okl4.notify_service
        modules.append(Autogen.notify_service)

        import pyokl4.okl4.objmanager_simple
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_simple
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.segment
        Autogen.segment = \
            pyokl4.okl4.segment
        modules.append(Autogen.segment)

        import pyokl4.okl4.switch_table
        Autogen.switch_table = \
            pyokl4.okl4.switch_table
        modules.append(Autogen.switch_table)

        import pyokl4.okl4.tracebuffer
        Autogen.tracebuffer = \
            pyokl4.okl4.tracebuffer
        modules.append(Autogen.tracebuffer)

        import pyokl4.okl4.trustzone
        Autogen.trustzone = \
            pyokl4.okl4.trustzone
        modules.append(Autogen.trustzone)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv7.okl4.mmu_dynamic
        Autogen.mmu_arch = \
            pyokl4.arch.armv7.okl4.mmu_dynamic
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv7.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv7.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4extras.debug.cortex_a7_r0p0.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4extras.debug.cortex_a7_r0p0.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4extras.debug.cortex_a7_r0p0.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4extras.debug.cortex_a7_r0p0.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4extras.debug.cortex_a7_r0p0.objmanager_simple
        Autogen.objmanager_autogen = \
           okl4types.okl4extras.debug.cortex_a7_r0p0.objmanager_simple
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4extras' and arch == 'armv7' and \
            cpu_desc == 'cortex_a8_r1p2_vfp' and debug == 'debug':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'mmu',
            'cpuregisters',
            'hal',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception',
            'protected_library',
            'filesystem',
            'mutex_hybrid',
            'scheduler',
            'intlock',
            'interrupt',
            'ipc',
            'kdb_interactive',
            'kdb',
            'kernel',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'PD',
            'mutex',
            'notify',
            'notify_service',
            'objmanager',
            'open_wait',
            'plugin',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess',
            'vector_page_faults',
        )

        module_names = (
            'allocator',
            'mmu_dynamic',
            'cpuregisters',
            'hal',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception_ipc',
            'extension',
            'filesystem_standard',
            'mutex_hybrid',
            'scheduler_standard',
            'intlock',
            'interrupt_dynamic',
            'ipc_extended',
            'kdb_interactive',
            'kdb_standard',
            'kernel_standard',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'multi_pd',
            'mutex_dynamic',
            'notify_standard',
            'notify_service',
            'objmanager_simple',
            'open_wait',
            'plugin_standard',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc_support',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess_dynamic',
            'vector_page_faults',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4extras.debug.cortex_a8_r1p2_vfp.types
        Autogen.types = \
            okl4types.okl4extras.debug.cortex_a8_r1p2_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4extras.debug.cortex_a8_r1p2_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4extras.debug.cortex_a8_r1p2_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4extras.debug.cortex_a8_r1p2_vfp.rights
        Autogen.rights = \
            okl4types.okl4extras.debug.cortex_a8_r1p2_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_dynamic
        Autogen.mmu = \
            pyokl4.okl4.mmu_dynamic
        modules.append(Autogen.mmu)

        import pyokl4.okl4.arm_vfp
        Autogen.arm_vfp = \
            pyokl4.okl4.arm_vfp
        modules.append(Autogen.arm_vfp)

        import pyokl4.okl4.buffer_area
        Autogen.buffer_area = \
            pyokl4.okl4.buffer_area
        modules.append(Autogen.buffer_area)

        import pyokl4.okl4.cache_control
        Autogen.cache_control = \
            pyokl4.okl4.cache_control
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.critsect
        Autogen.critsect = \
            pyokl4.okl4.critsect
        modules.append(Autogen.critsect)

        import pyokl4.okl4.scheduler_standard
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_standard
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.intlock
        Autogen.intlock = \
            pyokl4.okl4.intlock
        modules.append(Autogen.intlock)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.ipc_extended
        Autogen.ipc = \
            pyokl4.okl4.ipc_extended
        modules.append(Autogen.ipc)

        import pyokl4.okl4.kernel_standard
        Autogen.kernel = \
            pyokl4.okl4.kernel_standard
        modules.append(Autogen.kernel)

        import pyokl4.okl4.message_queue
        Autogen.message_queue = \
            pyokl4.okl4.message_queue
        modules.append(Autogen.message_queue)

        import pyokl4.okl4.mutex_dynamic
        Autogen.mutex = \
            pyokl4.okl4.mutex_dynamic
        modules.append(Autogen.mutex)

        import pyokl4.okl4.notify_service
        Autogen.notify_service = \
            pyokl4.okl4.notify_service
        modules.append(Autogen.notify_service)

        import pyokl4.okl4.objmanager_simple
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_simple
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.segment
        Autogen.segment = \
            pyokl4.okl4.segment
        modules.append(Autogen.segment)

        import pyokl4.okl4.switch_table
        Autogen.switch_table = \
            pyokl4.okl4.switch_table
        modules.append(Autogen.switch_table)

        import pyokl4.okl4.tracebuffer
        Autogen.tracebuffer = \
            pyokl4.okl4.tracebuffer
        modules.append(Autogen.tracebuffer)

        import pyokl4.okl4.trustzone
        Autogen.trustzone = \
            pyokl4.okl4.trustzone
        modules.append(Autogen.trustzone)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv7.okl4.mmu_dynamic
        Autogen.mmu_arch = \
            pyokl4.arch.armv7.okl4.mmu_dynamic
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv7.okl4.kernel_standard
        Autogen.kernel_arch = \
            pyokl4.arch.armv7.okl4.kernel_standard
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4extras.debug.cortex_a8_r1p2_vfp.scheduler_standard
        Autogen.scheduler_autogen = \
           okl4types.okl4extras.debug.cortex_a8_r1p2_vfp.scheduler_standard
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4extras.debug.cortex_a8_r1p2_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4extras.debug.cortex_a8_r1p2_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4extras.debug.cortex_a8_r1p2_vfp.objmanager_simple
        Autogen.objmanager_autogen = \
           okl4types.okl4extras.debug.cortex_a8_r1p2_vfp.objmanager_simple
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4smp' and arch == 'armv7' and \
            cpu_desc == 'cortex_a9mp_r0p1_vfp' and debug == 'production':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'mmu',
            'cpuregisters',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'hal',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception',
            'protected_library',
            'filesystem',
            'mutex_hybrid',
            'scheduler',
            'intlock',
            'interrupt',
            'ipc',
            'kdb',
            'kernel',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'PD',
            'mutex',
            'notify',
            'notify_service',
            'objmanager',
            'open_wait',
            'plugin',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess',
            'vector_page_faults',
        )

        module_names = (
            'allocator',
            'mmu_dynamic',
            'cpuregisters',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'hal',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception_ipc',
            'extension',
            'filesystem_standard',
            'mutex_hybrid',
            'scheduler_smp',
            'intlock',
            'interrupt_dynamic',
            'ipc_extended',
            'kdb_standard',
            'kernel_smp',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'multi_pd',
            'mutex_dynamic',
            'notify_standard',
            'notify_service',
            'objmanager_simple',
            'open_wait',
            'plugin_standard',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc_support',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess_dynamic',
            'vector_page_faults',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4smp.production.cortex_a9mp_r0p1_vfp.types
        Autogen.types = \
            okl4types.okl4smp.production.cortex_a9mp_r0p1_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4smp.production.cortex_a9mp_r0p1_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4smp.production.cortex_a9mp_r0p1_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4smp.production.cortex_a9mp_r0p1_vfp.rights
        Autogen.rights = \
            okl4types.okl4smp.production.cortex_a9mp_r0p1_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_dynamic
        Autogen.mmu = \
            pyokl4.okl4.mmu_dynamic
        modules.append(Autogen.mmu)

        import pyokl4.okl4.arm_vfp
        Autogen.arm_vfp = \
            pyokl4.okl4.arm_vfp
        modules.append(Autogen.arm_vfp)

        import pyokl4.okl4.buffer_area
        Autogen.buffer_area = \
            pyokl4.okl4.buffer_area
        modules.append(Autogen.buffer_area)

        import pyokl4.okl4.cache_control
        Autogen.cache_control = \
            pyokl4.okl4.cache_control
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.critsect
        Autogen.critsect = \
            pyokl4.okl4.critsect
        modules.append(Autogen.critsect)

        import pyokl4.okl4.scheduler_smp
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_smp
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.intlock
        Autogen.intlock = \
            pyokl4.okl4.intlock
        modules.append(Autogen.intlock)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.ipc_extended
        Autogen.ipc = \
            pyokl4.okl4.ipc_extended
        modules.append(Autogen.ipc)

        import pyokl4.okl4.kernel_smp
        Autogen.kernel = \
            pyokl4.okl4.kernel_smp
        modules.append(Autogen.kernel)

        import pyokl4.okl4.message_queue
        Autogen.message_queue = \
            pyokl4.okl4.message_queue
        modules.append(Autogen.message_queue)

        import pyokl4.okl4.mutex_dynamic
        Autogen.mutex = \
            pyokl4.okl4.mutex_dynamic
        modules.append(Autogen.mutex)

        import pyokl4.okl4.notify_service
        Autogen.notify_service = \
            pyokl4.okl4.notify_service
        modules.append(Autogen.notify_service)

        import pyokl4.okl4.objmanager_simple
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_simple
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.segment
        Autogen.segment = \
            pyokl4.okl4.segment
        modules.append(Autogen.segment)

        import pyokl4.okl4.switch_table
        Autogen.switch_table = \
            pyokl4.okl4.switch_table
        modules.append(Autogen.switch_table)

        import pyokl4.okl4.tracebuffer
        Autogen.tracebuffer = \
            pyokl4.okl4.tracebuffer
        modules.append(Autogen.tracebuffer)

        import pyokl4.okl4.trustzone
        Autogen.trustzone = \
            pyokl4.okl4.trustzone
        modules.append(Autogen.trustzone)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv7.okl4.mmu_dynamic
        Autogen.mmu_arch = \
            pyokl4.arch.armv7.okl4.mmu_dynamic
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv7.okl4.kernel_smp
        Autogen.kernel_arch = \
            pyokl4.arch.armv7.okl4.kernel_smp
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4smp.production.cortex_a9mp_r0p1_vfp.scheduler_smp
        Autogen.scheduler_autogen = \
           okl4types.okl4smp.production.cortex_a9mp_r0p1_vfp.scheduler_smp
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4smp.production.cortex_a9mp_r0p1_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4smp.production.cortex_a9mp_r0p1_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4smp.production.cortex_a9mp_r0p1_vfp.objmanager_simple
        Autogen.objmanager_autogen = \
           okl4types.okl4smp.production.cortex_a9mp_r0p1_vfp.objmanager_simple
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4smp' and arch == 'armv7' and \
            cpu_desc == 'cortex_a9mp_r0p1_vfp' and debug == 'debug':
        modules = []

        # Composition details.
        feature_names = (
            'allocator',
            'mmu',
            'cpuregisters',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'hal',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception',
            'protected_library',
            'filesystem',
            'mutex_hybrid',
            'scheduler',
            'intlock',
            'interrupt',
            'ipc',
            'kdb_interactive',
            'kdb',
            'kernel',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'PD',
            'mutex',
            'notify',
            'notify_service',
            'objmanager',
            'open_wait',
            'plugin',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess',
            'vector_page_faults',
        )

        module_names = (
            'allocator',
            'mmu_dynamic',
            'cpuregisters',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'hal',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception_ipc',
            'extension',
            'filesystem_standard',
            'mutex_hybrid',
            'scheduler_smp',
            'intlock',
            'interrupt_dynamic',
            'ipc_extended',
            'kdb_interactive',
            'kdb_standard',
            'kernel_smp',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'multi_pd',
            'mutex_dynamic',
            'notify_standard',
            'notify_service',
            'objmanager_simple',
            'open_wait',
            'plugin_standard',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc_support',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess_dynamic',
            'vector_page_faults',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4smp.debug.cortex_a9mp_r0p1_vfp.types
        Autogen.types = \
            okl4types.okl4smp.debug.cortex_a9mp_r0p1_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4smp.debug.cortex_a9mp_r0p1_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4smp.debug.cortex_a9mp_r0p1_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4smp.debug.cortex_a9mp_r0p1_vfp.rights
        Autogen.rights = \
            okl4types.okl4smp.debug.cortex_a9mp_r0p1_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.mmu_dynamic
        Autogen.mmu = \
            pyokl4.okl4.mmu_dynamic
        modules.append(Autogen.mmu)

        import pyokl4.okl4.arm_vfp
        Autogen.arm_vfp = \
            pyokl4.okl4.arm_vfp
        modules.append(Autogen.arm_vfp)

        import pyokl4.okl4.buffer_area
        Autogen.buffer_area = \
            pyokl4.okl4.buffer_area
        modules.append(Autogen.buffer_area)

        import pyokl4.okl4.cache_control
        Autogen.cache_control = \
            pyokl4.okl4.cache_control
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.critsect
        Autogen.critsect = \
            pyokl4.okl4.critsect
        modules.append(Autogen.critsect)

        import pyokl4.okl4.scheduler_smp
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_smp
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.intlock
        Autogen.intlock = \
            pyokl4.okl4.intlock
        modules.append(Autogen.intlock)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.ipc_extended
        Autogen.ipc = \
            pyokl4.okl4.ipc_extended
        modules.append(Autogen.ipc)

        import pyokl4.okl4.kernel_smp
        Autogen.kernel = \
            pyokl4.okl4.kernel_smp
        modules.append(Autogen.kernel)

        import pyokl4.okl4.message_queue
        Autogen.message_queue = \
            pyokl4.okl4.message_queue
        modules.append(Autogen.message_queue)

        import pyokl4.okl4.mutex_dynamic
        Autogen.mutex = \
            pyokl4.okl4.mutex_dynamic
        modules.append(Autogen.mutex)

        import pyokl4.okl4.notify_service
        Autogen.notify_service = \
            pyokl4.okl4.notify_service
        modules.append(Autogen.notify_service)

        import pyokl4.okl4.objmanager_simple
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_simple
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.segment
        Autogen.segment = \
            pyokl4.okl4.segment
        modules.append(Autogen.segment)

        import pyokl4.okl4.switch_table
        Autogen.switch_table = \
            pyokl4.okl4.switch_table
        modules.append(Autogen.switch_table)

        import pyokl4.okl4.tracebuffer
        Autogen.tracebuffer = \
            pyokl4.okl4.tracebuffer
        modules.append(Autogen.tracebuffer)

        import pyokl4.okl4.trustzone
        Autogen.trustzone = \
            pyokl4.okl4.trustzone
        modules.append(Autogen.trustzone)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv7.okl4.mmu_dynamic
        Autogen.mmu_arch = \
            pyokl4.arch.armv7.okl4.mmu_dynamic
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv7.okl4.kernel_smp
        Autogen.kernel_arch = \
            pyokl4.arch.armv7.okl4.kernel_smp
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4smp.debug.cortex_a9mp_r0p1_vfp.scheduler_smp
        Autogen.scheduler_autogen = \
           okl4types.okl4smp.debug.cortex_a9mp_r0p1_vfp.scheduler_smp
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4smp.debug.cortex_a9mp_r0p1_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4smp.debug.cortex_a9mp_r0p1_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4smp.debug.cortex_a9mp_r0p1_vfp.objmanager_simple
        Autogen.objmanager_autogen = \
           okl4types.okl4smp.debug.cortex_a9mp_r0p1_vfp.objmanager_simple
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4smpaffinity' and arch == 'armv7' and \
            cpu_desc == 'cortex_a9mp_r0p1_vfp' and debug == 'production':
        modules = []

        # Composition details.
        feature_names = (
            'scheduler',
            'allocator',
            'mmu',
            'cpuregisters',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'hal',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception',
            'protected_library',
            'filesystem',
            'mutex_hybrid',
            'intlock',
            'interrupt',
            'ipc',
            'kdb',
            'kernel',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'PD',
            'mutex',
            'notify',
            'notify_service',
            'objmanager',
            'open_wait',
            'plugin',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess',
            'vector_page_faults',
        )

        module_names = (
            'scheduler_smp',
            'allocator',
            'mmu_dynamic',
            'cpuregisters',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'hal',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception_ipc',
            'extension',
            'filesystem_standard',
            'mutex_hybrid',
            'intlock',
            'interrupt_dynamic',
            'ipc_extended',
            'kdb_standard',
            'kernel_smp',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'multi_pd',
            'mutex_dynamic',
            'notify_standard',
            'notify_service',
            'objmanager_simple',
            'open_wait',
            'plugin_standard',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc_support',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess_dynamic',
            'vector_page_faults',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4smpaffinity.production.cortex_a9mp_r0p1_vfp.types
        Autogen.types = \
            okl4types.okl4smpaffinity.production.cortex_a9mp_r0p1_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4smpaffinity.production.cortex_a9mp_r0p1_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4smpaffinity.production.cortex_a9mp_r0p1_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4smpaffinity.production.cortex_a9mp_r0p1_vfp.rights
        Autogen.rights = \
            okl4types.okl4smpaffinity.production.cortex_a9mp_r0p1_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.scheduler_smp
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_smp
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.mmu_dynamic
        Autogen.mmu = \
            pyokl4.okl4.mmu_dynamic
        modules.append(Autogen.mmu)

        import pyokl4.okl4.arm_vfp
        Autogen.arm_vfp = \
            pyokl4.okl4.arm_vfp
        modules.append(Autogen.arm_vfp)

        import pyokl4.okl4.buffer_area
        Autogen.buffer_area = \
            pyokl4.okl4.buffer_area
        modules.append(Autogen.buffer_area)

        import pyokl4.okl4.cache_control
        Autogen.cache_control = \
            pyokl4.okl4.cache_control
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.critsect
        Autogen.critsect = \
            pyokl4.okl4.critsect
        modules.append(Autogen.critsect)

        import pyokl4.okl4.intlock
        Autogen.intlock = \
            pyokl4.okl4.intlock
        modules.append(Autogen.intlock)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.ipc_extended
        Autogen.ipc = \
            pyokl4.okl4.ipc_extended
        modules.append(Autogen.ipc)

        import pyokl4.okl4.kernel_smp
        Autogen.kernel = \
            pyokl4.okl4.kernel_smp
        modules.append(Autogen.kernel)

        import pyokl4.okl4.message_queue
        Autogen.message_queue = \
            pyokl4.okl4.message_queue
        modules.append(Autogen.message_queue)

        import pyokl4.okl4.mutex_dynamic
        Autogen.mutex = \
            pyokl4.okl4.mutex_dynamic
        modules.append(Autogen.mutex)

        import pyokl4.okl4.notify_service
        Autogen.notify_service = \
            pyokl4.okl4.notify_service
        modules.append(Autogen.notify_service)

        import pyokl4.okl4.objmanager_simple
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_simple
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.segment
        Autogen.segment = \
            pyokl4.okl4.segment
        modules.append(Autogen.segment)

        import pyokl4.okl4.switch_table
        Autogen.switch_table = \
            pyokl4.okl4.switch_table
        modules.append(Autogen.switch_table)

        import pyokl4.okl4.tracebuffer
        Autogen.tracebuffer = \
            pyokl4.okl4.tracebuffer
        modules.append(Autogen.tracebuffer)

        import pyokl4.okl4.trustzone
        Autogen.trustzone = \
            pyokl4.okl4.trustzone
        modules.append(Autogen.trustzone)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv7.okl4.mmu_dynamic
        Autogen.mmu_arch = \
            pyokl4.arch.armv7.okl4.mmu_dynamic
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv7.okl4.kernel_smp
        Autogen.kernel_arch = \
            pyokl4.arch.armv7.okl4.kernel_smp
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4smpaffinity.production.cortex_a9mp_r0p1_vfp.scheduler_smp
        Autogen.scheduler_autogen = \
           okl4types.okl4smpaffinity.production.cortex_a9mp_r0p1_vfp.scheduler_smp
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4smpaffinity.production.cortex_a9mp_r0p1_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4smpaffinity.production.cortex_a9mp_r0p1_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4smpaffinity.production.cortex_a9mp_r0p1_vfp.objmanager_simple
        Autogen.objmanager_autogen = \
           okl4types.okl4smpaffinity.production.cortex_a9mp_r0p1_vfp.objmanager_simple
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    elif product == 'okl4smpaffinity' and arch == 'armv7' and \
            cpu_desc == 'cortex_a9mp_r0p1_vfp' and debug == 'debug':
        modules = []

        # Composition details.
        feature_names = (
            'scheduler',
            'allocator',
            'mmu',
            'cpuregisters',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'hal',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception',
            'protected_library',
            'filesystem',
            'mutex_hybrid',
            'intlock',
            'interrupt',
            'ipc',
            'kdb_interactive',
            'kdb',
            'kernel',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'PD',
            'mutex',
            'notify',
            'notify_service',
            'objmanager',
            'open_wait',
            'plugin',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess',
            'vector_page_faults',
        )

        module_names = (
            'scheduler_smp',
            'allocator',
            'mmu_dynamic',
            'cpuregisters',
            'arm_vfp',
            'atomic_ops',
            'barrier',
            'buffer_area',
            'cache_control',
            'capability_transfer',
            'cell_environment',
            'clock',
            'condition_variables',
            'hal',
            'critsect',
            'elf_dso',
            'ElfFile',
            'exception_ipc',
            'extension',
            'filesystem_standard',
            'mutex_hybrid',
            'intlock',
            'interrupt_dynamic',
            'ipc_extended',
            'kdb_interactive',
            'kdb_standard',
            'kernel_smp',
            'libokl4-lock',
            'lookup_range',
            'memalloc',
            'memory_pools',
            'message',
            'message_payload',
            'message_queue',
            'multi_pd',
            'mutex_dynamic',
            'notify_standard',
            'notify_service',
            'objmanager_simple',
            'open_wait',
            'plugin_standard',
            'posix',
            'preemption',
            'processor_control',
            'program',
            'rvct_out_of_line_syscalls',
            'realms',
            'relocation',
            'sched_profile',
            'scheduler_metrics',
            'segment',
            'semaphore',
            'soc_support',
            'suspend',
            'switch_table_legacy',
            'switch_table',
            'tls',
            'thread',
            'tracebuffer',
            'trustzone',
            'useraccess_dynamic',
            'vector_page_faults',
        )

        Autogen.features = set(feature_names)
        Autogen.modules = set(module_names)

        # Autogeneration imports.
        import okl4types.okl4smpaffinity.debug.cortex_a9mp_r0p1_vfp.types
        Autogen.types = \
            okl4types.okl4smpaffinity.debug.cortex_a9mp_r0p1_vfp.types
        modules.append(Autogen.types)

        import okl4types.okl4smpaffinity.debug.cortex_a9mp_r0p1_vfp.machine
        Autogen.machine_info = \
            okl4types.okl4smpaffinity.debug.cortex_a9mp_r0p1_vfp.machine
        modules.append(Autogen.machine_info)

        import okl4types.okl4smpaffinity.debug.cortex_a9mp_r0p1_vfp.rights
        Autogen.rights = \
            okl4types.okl4smpaffinity.debug.cortex_a9mp_r0p1_vfp.rights
        modules.append(Autogen.rights)

        # Import kernel modules.
        import pyokl4.okl4.scheduler_smp
        Autogen.scheduler = \
            pyokl4.okl4.scheduler_smp
        modules.append(Autogen.scheduler)

        import pyokl4.okl4.mmu_dynamic
        Autogen.mmu = \
            pyokl4.okl4.mmu_dynamic
        modules.append(Autogen.mmu)

        import pyokl4.okl4.arm_vfp
        Autogen.arm_vfp = \
            pyokl4.okl4.arm_vfp
        modules.append(Autogen.arm_vfp)

        import pyokl4.okl4.buffer_area
        Autogen.buffer_area = \
            pyokl4.okl4.buffer_area
        modules.append(Autogen.buffer_area)

        import pyokl4.okl4.cache_control
        Autogen.cache_control = \
            pyokl4.okl4.cache_control
        modules.append(Autogen.cache_control)

        import pyokl4.okl4.critsect
        Autogen.critsect = \
            pyokl4.okl4.critsect
        modules.append(Autogen.critsect)

        import pyokl4.okl4.intlock
        Autogen.intlock = \
            pyokl4.okl4.intlock
        modules.append(Autogen.intlock)

        import pyokl4.okl4.interrupt_dynamic
        Autogen.interrupt = \
            pyokl4.okl4.interrupt_dynamic
        modules.append(Autogen.interrupt)

        import pyokl4.okl4.ipc_extended
        Autogen.ipc = \
            pyokl4.okl4.ipc_extended
        modules.append(Autogen.ipc)

        import pyokl4.okl4.kernel_smp
        Autogen.kernel = \
            pyokl4.okl4.kernel_smp
        modules.append(Autogen.kernel)

        import pyokl4.okl4.message_queue
        Autogen.message_queue = \
            pyokl4.okl4.message_queue
        modules.append(Autogen.message_queue)

        import pyokl4.okl4.mutex_dynamic
        Autogen.mutex = \
            pyokl4.okl4.mutex_dynamic
        modules.append(Autogen.mutex)

        import pyokl4.okl4.notify_service
        Autogen.notify_service = \
            pyokl4.okl4.notify_service
        modules.append(Autogen.notify_service)

        import pyokl4.okl4.objmanager_simple
        Autogen.objmanager = \
            pyokl4.okl4.objmanager_simple
        modules.append(Autogen.objmanager)

        import pyokl4.okl4.segment
        Autogen.segment = \
            pyokl4.okl4.segment
        modules.append(Autogen.segment)

        import pyokl4.okl4.switch_table
        Autogen.switch_table = \
            pyokl4.okl4.switch_table
        modules.append(Autogen.switch_table)

        import pyokl4.okl4.tracebuffer
        Autogen.tracebuffer = \
            pyokl4.okl4.tracebuffer
        modules.append(Autogen.tracebuffer)

        import pyokl4.okl4.trustzone
        Autogen.trustzone = \
            pyokl4.okl4.trustzone
        modules.append(Autogen.trustzone)


        # Import libokl4 modules.
        import pyokl4.libokl4.realms
        Autogen.realms = \
            pyokl4.libokl4.realms
        modules.append(Autogen.realms)


        # Import arch-specific modules.
        import pyokl4.arch.armv7.okl4.mmu_dynamic
        Autogen.mmu_arch = \
            pyokl4.arch.armv7.okl4.mmu_dynamic
        modules.append(Autogen.mmu_arch)

        import pyokl4.arch.armv7.okl4.kernel_smp
        Autogen.kernel_arch = \
            pyokl4.arch.armv7.okl4.kernel_smp
        modules.append(Autogen.kernel_arch)


        # Import autogenerated modules.
        import okl4types.okl4smpaffinity.debug.cortex_a9mp_r0p1_vfp.scheduler_smp
        Autogen.scheduler_autogen = \
           okl4types.okl4smpaffinity.debug.cortex_a9mp_r0p1_vfp.scheduler_smp
        modules.append(Autogen.scheduler_autogen)

        import okl4types.okl4smpaffinity.debug.cortex_a9mp_r0p1_vfp.kdb_standard
        Autogen.kdb_autogen = \
           okl4types.okl4smpaffinity.debug.cortex_a9mp_r0p1_vfp.kdb_standard
        modules.append(Autogen.kdb_autogen)

        import okl4types.okl4smpaffinity.debug.cortex_a9mp_r0p1_vfp.objmanager_simple
        Autogen.objmanager_autogen = \
           okl4types.okl4smpaffinity.debug.cortex_a9mp_r0p1_vfp.objmanager_simple
        modules.append(Autogen.objmanager_autogen)


        # Backwards compatability 'syscall' interface.
        Autogen.syscalls = Autogen.objmanager_autogen.syscalls

        # Initialise all modules
        for mod in modules:
            if hasattr(mod, "init"):
                mod.init()

    else:
        raise Exception, "Unsupported product/cpu/debug: %s/%s/%s" % \
              (product, cpu_desc, debug)
