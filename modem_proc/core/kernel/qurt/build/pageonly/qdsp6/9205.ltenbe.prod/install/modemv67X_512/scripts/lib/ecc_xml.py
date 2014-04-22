##############################################################################
# Australian Public Licence B (OZPLB)
# 
# Version 1-0
# 
# Copyright (c) 2007, Open Kernel Labs, Inc.
# 
# All rights reserved. 
# 
# Developed by: Embedded, Real-time and Operating Systems Program (ERTOS)
#               National ICT Australia
#               http://www.ertos.nicta.com.au
# 
# Permission is granted by National ICT Australia, free of charge, to
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
#     * Neither the name of National ICT Australia, nor the names of its
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
# conditions, or imposes obligations or liability on National ICT
# Australia or one of its contributors in respect of the Software that
# cannot be wholly or partly excluded, restricted or modified, the
# liability of National ICT Australia or the contributor is limited, to
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
Processing of the machine XML element.
"""

from ezxml import Element, long_attr, str_attr, size_attr
import re

L1_icache_ecc_el = Element("l1_icache_ecc",
                 enabled = (str_attr, "required"),
                 interrupt = (str_attr, "optional"),
                 threshold = (str_attr, "optional"))
L1_dcache_ecc_el = Element("l1_dcache_ecc",
                 enabled = (str_attr, "required"),
                 interrupt = (str_attr, "optional"),
                 threshold = (str_attr, "optional"))
L2_cache_ecc_el = Element("l2_cache_ecc",
                 enabled = (str_attr, "required"),
                 interrupt = (str_attr, "optional"),
                 threshold = (str_attr, "optional"))
Vtcm_ecc_el = Element("vtcm_ecc",
                 enabled = (str_attr, "required"),
                 interrupt = (str_attr, "optional"),
                 threshold = (str_attr, "optional"))
Sec_interrupt_el = Element("sec_interrupt",
                 num = (str_attr, "required"))

Memory_ecc_el = Element("memory_ecc",
                       L1_icache_ecc_el,
                       L1_dcache_ecc_el,
                       L2_cache_ecc_el,
                       Vtcm_ecc_el,
                       Sec_interrupt_el)


def collect_memory_ecc_element(cfg):
    """Collect the attributes of the memory ECC element."""

    l1i_ecc_enabled = 0
    l1i_ecc_interrupt = 0
    l1i_ecc_threshold = 0
    l1d_ecc_enabled = 0
    l1d_ecc_interrupt = 0
    l1d_ecc_threshold = 0
    l2_ecc_enabled = 0
    l2_ecc_interrupt = 0
    l2_ecc_threshold = 0
    vtcm_ecc_enabled = 0
    vtcm_ecc_interrupt = 0
    vtcm_ecc_threshold = 0
    sec_interrupt = 0
    
    machine_el = cfg.find_child("machine")
    if not machine_el:
        return

    memory_ecc_el = machine_el.find_child("memory_ecc")
    if not memory_ecc_el:
        cfg['ECCL1IENABLE']    = repr(l1i_ecc_enabled)
        cfg['ECCL1IINTERRUPT'] = repr(l1i_ecc_interrupt)
        cfg['ECCL1ITHRESHOLD'] = repr(l1i_ecc_threshold)
        cfg['ECCL1DENABLE']    = repr(l1d_ecc_enabled)
        cfg['ECCL1DINTERRUPT'] = repr(l1d_ecc_interrupt)
        cfg['ECCL1DTHRESHOLD'] = repr(l1d_ecc_threshold)
        cfg['ECCL2ENABLE']     = repr(l2_ecc_enabled)
        cfg['ECCL2INTERRUPT']  = repr(l2_ecc_interrupt)
        cfg['ECCL2THRESHOLD']  = repr(l2_ecc_threshold)
        cfg['ECCVTCMENABLE']   = repr(vtcm_ecc_enabled)
        cfg['ECCVTCMINTERRUPT']= repr(vtcm_ecc_interrupt)
        cfg['ECCVTCMTHRESHOLD']= repr(vtcm_ecc_threshold)
        cfg['ECCSECINTNUM']= repr(sec_interrupt)
        return

    l1_icache_ecc_el = memory_ecc_el.find_child("l1_icache_ecc")
    if l1_icache_ecc_el is not None:
        if l1_icache_ecc_el.enabled is not None: 
            if l1_icache_ecc_el.enabled== 'true': 
                l1i_ecc_enabled = 1
        if l1_icache_ecc_el.interrupt is not None: 
            if l1_icache_ecc_el.interrupt== 'true': 
                l1i_ecc_interrupt = 1
                l1i_ecc_threshold = 1
        if l1_icache_ecc_el.threshold is not None: 
                l1i_ecc_threshold = int(l1_icache_ecc_el.threshold)
          
    cfg['ECCL1IENABLE']    = repr(l1i_ecc_enabled)
    cfg['ECCL1IINTERRUPT'] = repr(l1i_ecc_interrupt)
    cfg['ECCL1ITHRESHOLD'] = repr(l1i_ecc_threshold)

    
    l1_dcache_ecc_el = memory_ecc_el.find_child("l1_dcache_ecc")
    if l1_dcache_ecc_el is not None:
        if l1_dcache_ecc_el.enabled is not None: 
            if l1_dcache_ecc_el.enabled== 'true': 
                l1d_ecc_enabled = 1
        if l1_dcache_ecc_el.interrupt is not None: 
            if l1_dcache_ecc_el.interrupt== 'true': 
                l1d_ecc_interrupt = 1
                l1d_ecc_threshold = 1
        if l1_dcache_ecc_el.threshold is not None: 
                l1d_ecc_threshold = int(l1_dcache_ecc_el.threshold)
          
    cfg['ECCL1DENABLE']    = repr(l1d_ecc_enabled)
    cfg['ECCL1DINTERRUPT'] = repr(l1d_ecc_interrupt)
    cfg['ECCL1DTHRESHOLD'] = repr(l1d_ecc_threshold)
   
    l2_cache_ecc_el = memory_ecc_el.find_child("l2_cache_ecc")
    if l2_cache_ecc_el is not None:
        if l2_cache_ecc_el.enabled is not None: 
            if l2_cache_ecc_el.enabled== 'true': 
                l2_ecc_enabled = 1
        if l2_cache_ecc_el.interrupt is not None: 
            if l2_cache_ecc_el.interrupt== 'true': 
                l2_ecc_interrupt = 1
                l2_ecc_threshold = 1
        if l2_cache_ecc_el.threshold is not None: 
                l2_ecc_threshold = int(l2_cache_ecc_el.threshold)
          
    cfg['ECCL2ENABLE']    = repr(l2_ecc_enabled)
    cfg['ECCL2INTERRUPT'] = repr(l2_ecc_interrupt)
    cfg['ECCL2THRESHOLD'] = repr(l2_ecc_threshold)

    vtcm_cache_ecc_el = memory_ecc_el.find_child("vtcm_ecc")
    if vtcm_cache_ecc_el is not None:
        if vtcm_cache_ecc_el.enabled is not None: 
            if vtcm_cache_ecc_el.enabled== 'true': 
                vtcm_ecc_enabled = 1
        if vtcm_cache_ecc_el.interrupt is not None: 
            if vtcm_cache_ecc_el.interrupt== 'true': 
                vtcm_ecc_interrupt = 1
                vtcm_ecc_threshold = 1
        if vtcm_cache_ecc_el.threshold is not None: 
                vtcm_ecc_threshold = int(vtcm_cache_ecc_el.threshold)
          
    cfg['ECCVTCMENABLE']    = repr(vtcm_ecc_enabled)
    cfg['ECCVTCMINTERRUPT'] = repr(vtcm_ecc_interrupt)
    cfg['ECCVTCMTHRESHOLD'] = repr(vtcm_ecc_threshold)

    sec_interrupt_el = memory_ecc_el.find_child("sec_interrupt")
    if sec_interrupt_el is not None:
        if sec_interrupt_el.num is not None: 
            sec_interrupt = int(sec_interrupt_el.num)
          
    cfg['ECCSECINTNUM']= repr(sec_interrupt)

