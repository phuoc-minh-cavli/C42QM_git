#!/usr/bin/env python
#-------------------------------------------------------------------------------
# QDSS Ramdump Extractor
#      Extracts ETB and ETR data from a RAM dump
#
# Copyright (c) 2014 Qualcomm Technologies, Inc.
#         All rights reserved.
# Qualcomm  Technologies Confidential and Proprietary
#-------------------------------------------------------------------------------

import os
import sys
import  struct

#This buffer is shared between DIAG and QDSS.
#RAM dump extractor searches this space to find the QDSS trace buffer base
TRACE_BUFFER_BASE =  0x06E80000
TRACE_BUFFER_LIMIT = 0x07E00000

#constants
SYS_DBG_MAGIC_NUMBER = 0x42445953
QDSS_MAGIC_NUMBER = 0x5D1DB1BF
QDSS_PAGE_SIZE = 0x1000
QDSS_ETB_MAX_SIZE = 0x10000
QDSS_DCC_SRAM_SIZE = 0x800

log_level = 0

class QDSS_Ramdump_Extractor:

    def __enter__(self):
        self.ddr_dump_file = open(self.ddr_dump_file_name,'rb')
        self.init_offsets()
        return self

    def __exit__(self, type, value, traceback):
        self.ddr_dump_file.close()

    def __init__(self,ddr_dump_file_name):
        self.ddr_dump_file_name = ddr_dump_file_name
        self.has_read_etb_regs  = False
        self.has_read_etr_regs  = False
        self.has_read_dcc_regs  = False
        self.has_read_dcc_sram = False


    def find_qdss_buf_base(self):
        self.qdss_buf_base_addr = None
        for addr in range(TRACE_BUFFER_BASE,TRACE_BUFFER_LIMIT,QDSS_PAGE_SIZE):
            if self.has_qdss_magic(addr):
                self.qdss_buf_base_addr = addr
                break
        return self.qdss_buf_base_addr
    

    def init_offsets(self):
        if (not self.find_qdss_buf_base()):
            raise Exception('QDSS trace buffer not initialized')

        print("QDSS trace buffer base :  0x%08X" % self.qdss_buf_base_addr)

        hdr_buf=self.read_mem(self.qdss_buf_base_addr,36)
        hdr=(magic,
             version,
             self.dump_table_offset,
             self.etb_regs_offset,
             self.etr_regs_offset,
             self.etb_data_offset,
             self.etr_data_offset,
             self.dcc_regs_offset,
             self.dcc_sram_offset) = struct.unpack('<LLLLLLLLL',hdr_buf)

        self.dump_table_base_addr = (self.qdss_buf_base_addr +
                                     self.dump_table_offset)

        (self.etb_regs_base_addr,
         self.etr_regs_base_addr,
         self.etb_data_base_addr,
         self.etr_data_base_addr,
         self.dcc_regs_base_addr,
         self.dcc_sram_base_addr) = [x+self.qdss_buf_base_addr for x in hdr[3:]]

    def read_mem(self,addr,size):
        self.ddr_dump_file.seek(addr)
        return self.ddr_dump_file.read(size)

    def has_qdss_magic(self,addr):
        (magic,)=struct.unpack('<L',self.read_mem(addr,4))
        return (magic == QDSS_MAGIC_NUMBER)

    def has_sys_dbg_magic(self,addr):
        (magic,)=struct.unpack('<L',self.read_mem(addr+4,4))
        return (magic == SYS_DBG_MAGIC_NUMBER)        

    def read_etb_regs(self):
        if (self.has_read_etb_regs):
            return
        self.has_read_etb_regs = True
        if (not self.has_sys_dbg_magic(self.etb_regs_base_addr)):
            raise Exception("No valid ETB register dump")

        self.etb_regs_buf=self.read_mem(self.etb_regs_base_addr+QDSS_PAGE_SIZE,
                                            QDSS_PAGE_SIZE)

    def dump_etb_regs(self,filename):
        self.read_etb_regs()
        if (len(self.etb_regs_buf) > 0):
            with open(filename,'wb') as w:
                w.write(self.etb_regs_buf)
        else:
            print("No ETB register info")


    def read_etr_regs(self):
        if (self.has_read_etr_regs):
            return
        self.has_read_etr_regs = True
        if (not self.has_sys_dbg_magic(self.etr_regs_base_addr)):
            raise Exception("No valid ETR register dump")

        self.etr_regs_buf=self.read_mem(self.etr_regs_base_addr+QDSS_PAGE_SIZE,
                                            QDSS_PAGE_SIZE)

    def read_dcc_regs(self):
        if (self.has_read_dcc_regs):
            return True
        if (not self.has_sys_dbg_magic(self.dcc_regs_base_addr)):
            print("No valid DCC register dump")
            return False

        self.has_read_dcc_regs = True
        self.dcc_regs_buf=self.read_mem(self.dcc_regs_base_addr+QDSS_PAGE_SIZE,
                                            QDSS_PAGE_SIZE)
        return True

    def read_dcc_sram(self):
        if (self.has_read_dcc_sram):
            return True
        if (not self.has_sys_dbg_magic(self.dcc_sram_base_addr)):
            print("No valid DCC SRAM dump")
            return False

        self.has_read_dcc_sram = True
        self.dcc_sram_buf=self.read_mem(self.dcc_sram_base_addr+QDSS_PAGE_SIZE,
                                        QDSS_DCC_SRAM_SIZE)
        return True

    def dump_etr_regs(self,filename):
        self.read_etr_regs()
        if (len(self.etr_regs_buf) > 0):
            with open(filename,'wb') as w:
                w.write(self.etr_regs_buf)
        else:
            print("No ETR register info")

    def dump_dcc_regs(self,filename):
        if self.read_dcc_regs():
            if (len(self.dcc_regs_buf) > 0):
                with open(filename,'wb') as w:
                    w.write(self.dcc_regs_buf)
            else:
                print("No DCC register info")        

    def dump_dcc_sram(self,filename):
        if self.read_dcc_sram():
            if (len(self.dcc_sram_buf) > 0):
                with open(filename,'wb') as w:
                    w.write(self.dcc_sram_buf)
            else:
                print("No DCC SRAM info")        

    def read_uint32(self,buf,offset):
        return struct.unpack_from('<L',buf,offset)[0]

    def dump_etb_data(self,filename):
        if (not self.has_sys_dbg_magic(self.etb_data_base_addr)):
            return False
        
        self.read_etb_regs()

        etb_status = self.read_uint32(self.etb_regs_buf,0xC)
        etb_buffer_size = self.read_uint32(self.etb_regs_buf,0x4)
        etb_buffer_size = etb_buffer_size * 4 # convert to bytes
        etb_write_ptr = self.read_uint32(self.etb_regs_buf,0x18)

        
        if (etb_buffer_size > QDSS_ETB_MAX_SIZE):
            etb_buffer_size = QDSS_ETB_MAX_SIZE

        if (etb_status & 0x1):
            size_to_read = etb_buffer_size
        else:
            size_to_read = etb_write_ptr % etb_buffer_size
        etb_data = self.read_mem(self.etb_data_base_addr+QDSS_PAGE_SIZE,
                                 size_to_read)

        with open(filename,'wb') as w:
            w.write(etb_data)
            total_size = len(etb_data)
            if (total_size):
                print("Extracted ETB dump (%d bytes) : %s " % (total_size,os.path.abspath(filename)))
                return True
        return False

            
    def dump_etr_data(self,filename):
        self.read_etr_regs()

        etr_buffer_size = self.read_uint32(self.etr_regs_buf,0x4)
        etr_buffer_size = etr_buffer_size * 4 # convert to bytes

        etr_status = self. read_uint32(self.etr_regs_buf,0xC)
        etr_write_ptr = self.read_uint32(self.etr_regs_buf,0x18)
        etr_buffer_base = self.read_uint32(self.etr_regs_buf,0x118)

        if (log_level > 0):
            print() 
            print("ETR buffer base: 0x%08X" % etr_buffer_base)
            print("ETR buffer size: 0x%08X" % etr_buffer_size)
            print("ETR write ptr: 0x%08X" % etr_write_ptr)
            print("ETR status: 0x%08X" % etr_status)
            print() 
        
        write_ptr_offset=etr_write_ptr - etr_buffer_base
        self.ddr_dump_file.seek(self.etr_data_base_addr)
        trace_buf_0=self.ddr_dump_file.read(write_ptr_offset)
        trace_buf_1=self.ddr_dump_file.read(etr_buffer_size - write_ptr_offset)

        with open(filename,'wb') as w:
            if (etr_status & 0x1):
                #wrapped around
                w.write(trace_buf_1)
                w.write(trace_buf_0)
                total_size=len(trace_buf_0)+len(trace_buf_1)
            else:
                w.write(trace_buf_0)
                total_size=len(trace_buf_0)
            if (total_size):
                print("Extracted ETR dump (%d bytes) : %s " % (total_size,os.path.abspath(filename)))
                return True
        return False


if __name__ == '__main__':
    try:
        ddr_dump_file=sys.argv[1] if (len(sys.argv)>=2) else 'DDRCS0.BIN'
        ddr_dump_file=os.path.abspath(ddr_dump_file)
        if (not os.path.exists(ddr_dump_file)):
            print("Cannot find find file %s" % ddr_dump_file)
            print("Usage: python qdss_rde.py [DDRCS0_BIN]")
            exit(1)
        dir_path=os.path.split(ddr_dump_file)[0]
        with  QDSS_Ramdump_Extractor(ddr_dump_file) as rde:
            rde.dump_etb_regs(os.path.join(dir_path,'etb_registers.bin'))
            rde.dump_etr_regs(os.path.join(dir_path,'etr_registers.bin'))
            rde.dump_dcc_regs(os.path.join(dir_path,'dcc_registers.bin'))
            rde.dump_dcc_sram(os.path.join(dir_path,'dcc_sram.bin'))
            got_etb_dump = rde.dump_etb_data(os.path.join(dir_path,'etb.bin'))
            got_etr_dump = rde.dump_etr_data(os.path.join(dir_path,'etr.bin'))
            if (not (got_etb_dump or got_etr_dump)):
                raise Exception('No valid ETB or ETR dump')
    except Exception as e:
        print(e)

