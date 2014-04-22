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
Support class for a cell that contains a single okl4 program.
"""
from weaver.segments_xml import start_to_value
from weaver.memobjs_xml import Stack_el, collect_memobj_attrs
from weaver.ezxml import Element, str_attr, long_attr, ParsedElement
from weaver.cap import ThreadRights, SegmentRights, PhysAddrRights

Thread_el = Element('thread', Stack_el,
                    name = (str_attr, 'required'),
                    start = (str_attr, 'required'),
                    init = (str_attr, 'optional'),
                    priority = (long_attr, 'optional'),
                    timeslice = (long_attr, 'optional'),
                    affinity = (long_attr, 'optional'),
                    physpool = (str_attr, 'optional'),
                    virtpool = (str_attr, 'optional'))

DEFAULT_STACK_SIZE = 0x1000

def collect_thread(cell, elf, el, image_coln, namespace, image,
                   machine, mmu_context, manager_domain, env,
                   segs, env_kdomain, env_kmmu_context, pd_name, manager,
                   entry = None, main_thread = False, friendly_name = None):
    """Collect the attributes of a thread element."""

    if entry is None:
        entry = el.start

    init_func = getattr(el, 'init', None)
    if init_func is None:
        if main_thread:
            init_func = '__sys_entry'
        else:
            init_func = '__sys_thread_entry'

    start = start_to_value('_start', elf)
    entry = start_to_value(entry, elf)
    init_func = start_to_value(init_func, elf)

    # Get the current values on the sched params stack.
    (def_prio, def_timeslice, def_affinity) = \
               image.get_default_sched_params()

    priority = getattr(el, 'priority', def_prio)
    timeslice = getattr(el, 'timeslice', def_timeslice)
    affinity = getattr(el, 'affinity', def_affinity)
    physpool = getattr(el, 'physpool', None)
    virtpool = getattr(el, 'virtpool', None)

    # New namespace for objects living in the thread.
    if (main_thread):
        thread_name = 'main'
    else:
        thread_name = el.name

    # Ensure friendly name is set. This will be weaved out as the kdb name
    if (not friendly_name):
        if main_thread:
            friendly_name = "%s_%s" % (cell.name, "main")
        else:
            friendly_name = thread_name

    thread_namespace = namespace.add_namespace(thread_name, "thread")

    # Push the overriding pools for the thread.
    image.push_attrs(virtual = virtpool, physical = physpool)

    # Collect the stack.  Is there no element, create a fake one for
    # the collection code to use.
    stack_el = el.find_child('stack')
    if stack_el is None:
        stack_el = ParsedElement('stack')

    stack_attr = collect_memobj_attrs(stack_el, thread_namespace,
                                      image, machine,
                                      representing="thread stack")

    if stack_attr.size is None:
        stack_attr.size = DEFAULT_STACK_SIZE

    # Allocate the stack with guard pages to  detect overruns.
    stack_ms = image_coln.add_memsection(stack_attr, machine, guard_pages=True)
    sas_attrs = image_coln.sas_attrs(stack_ms.attrs)

    # Create the mmu thread and assign the entry point
    kern_thread = mmu_context.register_thread(thread_name, start,
                                              entry, init_func,
                                              sas_attrs, priority,
                                              timeslice, affinity,
                                              main_thread, friendly_name)

    # Add the standard rights.
    ThreadRights.add_standard_rights(kern_thread, thread_namespace)

    # Add reloc information for this thread if we do in fact end up relocating
    # the segments it is located in
    image.add_reloc_thread(kern_thread, segs)

    # Map the stack.
    stack_seg = manager_domain.register_segment(sas_attrs)
    mmu_context.register_mapping(stack_seg, sas_attrs)
    SegmentRights.add_standard_rights(stack_seg, stack_attr.ns_node)
    PhysAddrRights.add_standard_rights(stack_seg, stack_attr.ns_node)

    # Weave a kthread for this thread
    if main_thread:
        kthread_name = pd_name + '_KTHREAD'
    else:
        kthread_name = pd_name + '_' + thread_name + '_KTHREAD'

    env_kthread = env.add_kthread_entry(kthread_name, kern_thread, env_kdomain,
                                        env_kmmu_context)

    # Add an env entry for the main thread of the main program, assume this
    # will always be called main.
    if thread_name == 'main' and manager:
        # FIXME: Jira ticket OLD-1790 - jsok.
        if hasattr(cell, 'register_realm_mapping'):
            if hasattr(image_coln, "env_realm"):
                env_realm = image_coln.env_realm
            else:
                env_realm = None
        cell.register_realm_mapping('MAIN_STACK', stack_seg,
                                    env_realm, mmu_context)

    image.pop_attrs()

    return kern_thread, env_kthread
