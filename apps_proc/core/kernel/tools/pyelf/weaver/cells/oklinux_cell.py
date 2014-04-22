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
Support class for an oklinux (!vlinux) cell.
"""

from weaver.ezxml import str_attr, bool_attr, long_attr, ParsedElement, \
        update_element, Element
from weaver.memobjs_xml import Memsection_el, Stack_el
from weaver.segments_xml import Patch_el, Segment_el
from weaver.cells.collection.thread import Thread_el
from weaver.cells.collection.irq import IRQ_el
from weaver.cells.collection.mutex import Mutex_el
from weaver.cells.collection.device import UseDevice_el
from weaver.cells.collection.pd import Program_el, PD_el
from weaver.cells.collection.attachment import Attachment_el
from weaver.cells.collection.tzmonitor import TZMonitor_el
from weaver.cells.cell import Cell, Cell_el

# If the virq module is not present, treat as nops.
try:
    from weaver.cells.collection.virq import VIRQ_el, VInterruptLine_el
except:
    VIRQ_el = Element('virq')
    VInterruptLine_el = Element('virtual_interrupt_line')

OKLinuxCell_el = update_element(Cell_el,
                              Segment_el, Patch_el, Thread_el, Attachment_el,
                              Memsection_el, Mutex_el, IRQ_el, VIRQ_el,
                              UseDevice_el, VInterruptLine_el,
                              TZMonitor_el, Stack_el,
                              file = (str_attr, 'required'),
                              caps = (long_attr, 'optional'),
                              threads = (long_attr, 'optional'),
                              priority = (long_attr, 'optional'),
                              init = (str_attr, 'optional'))
OKLinuxCell_el.remove_element(Program_el)
OKLinuxCell_el.remove_element(PD_el)
OKLinuxCell_el.set_name("linux")

class OKLinuxCell(Cell):
    """
    Cell for the linux kernel.
    """

    element = OKLinuxCell_el

    def __init__(self):
        Cell.__init__(self)
        self.vmalloc_ms = None

    def collect_xml(self, el, ignore_name, namespace, machine, pools, kernel,
                    image):
        """Handle an simple program cell Compound Object"""

        # Split our combined cell tag into a cell and program tag
        program_el = ParsedElement('program')
        program_el.name = 'MAIN'
        program_el.file = el.file
        program_el.manager = True
        program_el._path = el._path
        # We need to designate that this is a linux program so don't try and
        # do any of our initail start up / lock weaving
        program_el.linux = True

        # Grab any program specific tag from the cell's XML
        for tag in ('caps', 'threads',
                    'priority', 'init'):
            if hasattr(el, tag):
                setattr(program_el, tag, getattr(el, tag))

        # Grab any relevant children from the cell's XML
        for tag in ('segment', 'patch', 'thread', 'attached', 'memsection',
                    'mutex', 'irq', 'virq', 'virtual_interrupt_line',
                    'use_device', 'stack', 'platform_control',
                    'processor_control'):
            program_el.add_children(el.find_children(tag))

        el.add_children([program_el])

        # Call Cell's collect method
        Cell.collect_xml(self, el, ignore_name, namespace, machine, pools,
                         kernel, image)

    def generate_init(self, machine, image):
        """
        Generate the bootinfo script for the cell, placing it into a
        segment.
        """
        #self.set_stack(kernel)
        #self.get_pool_freelists(pools, kernel)

        #self.bootinfo.generate_bootinfo(self, machine, self.elf, kernel)

        # self.env.generate_init(machine, pools, kernel, image)
        # Pretend the cell env is the bootinfo!
        #kernel.set_bootinfo(phys = self.env.ms.segment.paddr,
        #                    virt = self.env.ms.segment.vaddr,
        #                    size = self.env.ms.segment.get_memsz())
        self.set_free_pools(image, machine)
        self.env.generate_init(image)

    #pylint: disable-msg=R0913
    def generate_dynamic_segments(self, machine, image):
        """Generate environment and other dynamic cell buffers."""
        Cell.generate_dynamic_segments(self, machine, image)

OKLinuxCell.register()
