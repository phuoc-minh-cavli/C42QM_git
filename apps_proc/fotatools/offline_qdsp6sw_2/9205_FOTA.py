#! /usr/bin/python

import os
import sys
import struct
import hashlib
import argparse
import operator

from os.path import join as pj
from os.path import split as ps
from os.path import exists as pe
from os.path import abspath as ap

CLADE1ENABLED = True
CLADE2ENABLED = False

# Data needed for CLADE1 compressed section
Clade1SegSections = [ 
    ".clade.comp",
    ".clade.exception_low_large",
    ".clade.exception_low_small",
    ".clade.exception_high",
    ".clade.dict",
    ".clade.metadata"
    ]

# Data needed for CLADE2 compressed section
Clade2SegSections = [
    ".clade2.comp",
    ".clade2.metadata",
    ".clade2.16B_free_list",
    ".clade2.32B_free_list",
    ".clade2.48B_free_list",
    ".clade2.64B_free_list",
    ".clade2.regs",
    ]

# Data needed for CLADE1 uncompressed section
CLADE1_Uncompressed_Key = [
    ".region_high_clade",
    ".region_low_clade"
    ]

# Data needed for CLADE2 uncompressed section
CLADE2_Uncompressed_Key = [
    ".region_clade2",
    ]

# CLADE2 engine configuration
CLADE2_Config = [
    "anchor1",
    "anchor2",
    "anchor3",
    "anchor4",
    "freelist16_off",
    "freelist32_off",
    "freelist48_off",
    "freelist64_off",
    "num_entries_16",
    "num_entries_32",
    "num_entries_48",
    "num_entries_64",
    "free_list_entries_16",
    "free_list_entries_32",
    "free_list_entries_48",
    "free_list_entries_64"
    ]

# Final metadata 
# meta 1 contains offset/size tuples per item
META_1 = Clade1SegSections
# Meta 2 is comprised of singleton data
META_2 = CLADE1_Uncompressed_Key 

# We also need some data for dynamic sections
extpl_section = [
    "__swapped_segments_extpl_nb1_rwdata_end__", 
    "__swapped_segments_extpl_nb1_text_start__",
    "__extpl_rx_swap_pool_end__", 
    "__extpl_rx_swap_pool_start__",
    "__clade_region_high_pd0_start__",
    "__clade_region_high_dynamic_start__",
    "__clade_region_high_dynamic_end__"
    ]

def get_meta_counts(metadata, counts):
  meta_arr = bytearray(metadata)
  bytecount = 0
  bitBuffer = 0
  for byte in meta_arr:
    bitBuffer=(byte<<(bytecount*8))|bitBuffer
    bytecount+=1
    if bytecount == 3:
      if (bitBuffer&0x3FFFFF) != 0x3FFFFF:
        size = byte >> 6
        counts[size+1] = counts.get(size+1, 0) + 1
      bytecount = 0
      bitBuffer = 0

class CLADE_Extractor:
  def __init__(self, orig_elf_path, final_mbn_path, paging_mbn_path, out_dir):
    self.out_dir = out_dir
    self.origelf_path = ap(orig_elf_path)
    self.finalmbn_path = ap(final_mbn_path)
    self.pagingmbn_path = ap(paging_mbn_path)
    self.elf_handle = open(self.origelf_path, 'rb')
    self.mbn_handle = open(self.finalmbn_path, 'rb')
    self.elf_obj = None
    self.mbn_obj = None
    self.clade1_comp = []
    self.clade2_comp = []
    self.clade1_uncomp = []
    self.clade2_uncomp = []
    self.mbn_offsets = {}
    self.extpl_meta = {}
    self.clade2config = {}

    print "Extracting Clade info from following debug elf location: {}".format(self.origelf_path)

    if not pe(out_dir):
      try:
        os.mkdir(out_dir)
        print "Created folder for data dumps: {}".format(out_dir)
      except OSError:  
        print "Creation of the directory failed: {}".format(out_dir)

    else:
      print "Using existing folder for data dumps: {}".format(out_dir)

    if not is_elf(self.elf_handle):
      print 'Missing elf headers from original elf!'
      return 0

    if not is_elf(self.mbn_handle):
      print 'Missing elf headers from final mbn!'
      return 0

    self.get_elf_data()

  # def __del__(self):
  #   try:
  #     self.elf_handle.close()
  #     self.mbn_handle.close()

  def get_elf_data(self):
    self.elf_obj = elfFileClass.elfFile(ap(self.origelf_path))
    self.mbn_obj = elfFileClass.elfFile(ap(self.finalmbn_path))

  def extract_clade_info(self):
    self.get_clade1_compdata()
    #self.get_clade2_compdata()
    self.get_clade1_uncompdata()
    #self.get_clade2_uncompdata()
    # self.get_clade2_config()

  def get_clade1_compdata(self):
    print "function get_clade1_compdata:---"
    for section_name in Clade1SegSections:
      s_data = get_section_data(self.elf_obj, section_name)
      if s_data == None:
        raise Exception("CLADE1 data missing from elf: {}".format(section_name))
      self.clade1_comp.append(s_data)
      print s_data

  # def get_clade2_compdata(self):
    # for section_name in Clade2SegSections:
      # s_data = get_section_data(self.elf_obj, section_name)
      # if s_data == None:
        # raise Exception("CLADE2 data missing from elf: {}".format(section_name))
      # self.clade2_comp.append(s_data)
      # if section_name == '.clade2.16B_free_list':
        # self.clade2config['free_list_entries_16'] = s_data[2]/64*23
      # if section_name == '.clade2.32B_free_list':
        # self.clade2config['free_list_entries_32'] = s_data[2]/64*23
      # if section_name == '.clade2.48B_free_list':
        # self.clade2config['free_list_entries_48'] = s_data[2]/64*23
      # if section_name == '.clade2.64B_free_list':
        # self.clade2config['free_list_entries_64'] = s_data[2]/64*23
      # print s_data

  def get_clade1_uncompdata(self):
    print "Inside get_clade1_uncompdata:----"
    for section_name_key in CLADE1_Uncompressed_Key:
      section_hdr = find_section_with_substring(self.elf_obj, section_name_key)
      if section_hdr == None:
        raise Exception("Could not find section using key: {}".format(section_name_key))
      name = section_hdr.sh_nameStr
      offset = section_hdr.sh_offset
      size = section_hdr.sh_size
      sect_index = section_hdr.idx
      seg_index = get_segidx_from_sectidx(self.elf_obj, sect_index)
      self.clade1_uncomp.append((name, offset, size, sect_index, seg_index))
      print self.clade1_uncomp

  # def get_clade2_uncompdata(self):
    # for section_name_key in CLADE2_Uncompressed_Key:
      # section_hdr = find_section_with_substring(self.elf_obj, section_name_key)
      # if section_hdr == None:
        # raise Exception("Could not find section using key: {}".format(section_name_key))
      # name = section_hdr.sh_nameStr
      # offset = section_hdr.sh_offset
      # size = section_hdr.sh_size
      # sect_index = section_hdr.idx
      # seg_index = get_segidx_from_sectidx(self.elf_obj, sect_index)
      # self.clade2_uncomp.append((name, offset, size, sect_index, seg_index))
      # print self.clade2_uncomp

  # def get_clade2_config(self):
    # meta_sh = self.elf_obj.getSectionByName('.clade2.metadata')
    # self.elf_handle.seek(meta_sh.sh_offset)
    # clade2meta = self.elf_handle.read(meta_sh.sh_size)

    # decode meta
    # meta_counts={1:0,2:0,3:0,4:0}
    # get_meta_counts(clade2meta, meta_counts)
    # print meta_counts

    # regs_sh = self.elf_obj.getSectionByName('.clade2.regs')
    # self.elf_handle.seek(regs_sh.sh_offset)
    # clade2regs = self.elf_handle.read(regs_sh.sh_size)
    # regsdata = struct.unpack("{}I".format(regs_sh.sh_size/4), clade2regs)

    # regs_dict = {}
    # for i in xrange(0, len(regsdata), 2):
      # regs_dict[int(regsdata[i])] = int(regsdata[i+1])

    # print regsdata
    # self.clade2config['anchor1'] = regs_dict[0x100]
    # self.clade2config['anchor2'] = regs_dict[0x104]
    # self.clade2config['anchor3'] = regs_dict[0x108]
    # self.clade2config['anchor4'] = regs_dict[0x10C]
    # self.clade2config["freelist16_off"] = regs_dict[0x48]
    # self.clade2config["freelist32_off"] = regs_dict[0x58]
    # self.clade2config["freelist48_off"] = regs_dict[0x68]
    # self.clade2config["freelist64_off"] = regs_dict[0x78]
    # self.clade2config["num_entries_16"] = \
      # meta_counts[1] + (regs_dict[0x48] >> 6)*23 + (regs_dict[0x48]&63)
    # self.clade2config["num_entries_32"] = \
      # meta_counts[2] + (regs_dict[0x58] >> 6)*23 + (regs_dict[0x58]&63)
    # self.clade2config["num_entries_48"] = \
      # meta_counts[3] + (regs_dict[0x68] >> 6)*23 + (regs_dict[0x68]&63)
    # self.clade2config["num_entries_64"] = \
      # meta_counts[4] + (regs_dict[0x78] >> 6)*23 + (regs_dict[0x78]&63)

  def find_mbn_offsets(self):
    print "==========================================Inside find_mbn_offsets=========================================="
    self.clade1_comp = sorted(self.clade1_comp, key=operator.itemgetter(1))
    print self.clade1_comp
    # find exception high offset in mbn - should be lowest offset of all clade sections
    segidx = self.clade1_comp[0][4]
    contents, phdr = get_segment_contents(self.elf_handle, self.elf_obj, segidx)
    hi_ex_seg_offset = self.get_mbn_seg_offset(contents, self.clade1_comp[0][2])
    # self.mbn_offsets[self.clade1_comp[0][0]] = hi_ex_seg_offset

    # save offset for .clade.comp in original elf
    orig_clade_offset = self.clade1_comp[1][1]

    # find clade compressed offset in mbn
    segidx = self.clade1_comp[1][4]
    contents, phdr = get_segment_contents(self.elf_handle, self.elf_obj, segidx)
    mbn_clade_offset = self.get_mbn_seg_offset(contents)
    
    for name, offset, size, sect_index, seg_index in self.clade1_comp:
      if name == ".clade.exception_high":
        self.mbn_offsets[name] = (hi_ex_seg_offset, size)
      else:
        self.mbn_offsets[name] = (offset - orig_clade_offset + mbn_clade_offset, size)

    # self.clade2_comp = sorted(self.clade2_comp, key=operator.itemgetter(1))
    # orig_clade2_offset = self.clade2_comp[0][1]

    # get clade2 compressed contents from orig elf, find offset in mbn
    # segidx = self.clade2_comp[0][4]
    # contents, phdr = get_segment_contents(self.elf_handle, self.elf_obj, segidx)
    # mbn_clade2_offset = self.get_mbn_seg_offset(contents)

    # for name, offset, size, sect_index, seg_index in self.clade2_comp:
      # self.mbn_offsets[name] = (offset - orig_clade2_offset + mbn_clade2_offset, size)

    for name, offset, size, sect_index, seg_index in self.clade1_uncomp:
      contents, phdr = get_segment_contents(self.elf_handle, self.elf_obj, seg_index)
      for n in CLADE1_Uncompressed_Key:
        if n in name:
          name = n
      self.mbn_offsets[name] = len(contents)

    # for name, offset, size, sect_index, seg_index in self.clade2_uncomp:
      # self.mbn_offsets[name] = size

    # for name, val in self.clade2config.items():
      # self.mbn_offsets[name] = val

    # print "finding extpl offset/size"
    # try:
      # nb1end = self.elf_obj.getSymbolByName(extpl_section[0]).st_value
      # nb1start = self.elf_obj.getSymbolByName(extpl_section[1]).st_value
      # swapend = self.elf_obj.getSymbolByName(extpl_section[2]).st_value
      # swapstart = self.elf_obj.getSymbolByName(extpl_section[3]).st_value
      # cladehighstart = self.elf_obj.getSymbolByName(extpl_section[4]).st_value
      # cladedynamicstart = self.elf_obj.getSymbolByName(extpl_section[5]).st_value
      # cladedynamicend = self.elf_obj.getSymbolByName(extpl_section[6]).st_value
    # except Exception as e:
      # raise (e)

    # extpl_dynclade_off = nb1end - nb1start
    # extpl_dynclade_sz = swapend - swapstart

    # dyncladeoffset = self.mbn_offsets['.clade.comp'][0] + self.mbn_offsets['.clade.comp'][1] - extpl_dynclade_sz
    # compextploffset = cladedynamicstart - cladehighstart

    # self.extpl_meta['size'] = extpl_dynclade_sz
    # self.extpl_meta['static_offset'] = dyncladeoffset
    # self.extpl_meta['paging_offset'] = extpl_dynclade_off

    # dumping copy of dynamic clade data
    # with open(self.out_dir + 'clade_extpl.bin', 'wb') as out:
      # self.mbn_handle.seek(dyncladeoffset)
      # out.write(self.mbn_handle.read(extpl_dynclade_sz))

    with open(self.out_dir + 'offsets.txt', 'w') as out:
      out.write("CLADE1 OFFSET: {}\n".format(self.mbn_offsets['.clade.comp'][0]))
      # out.write("CLADE2 OFFSET: {}\n".format(self.mbn_offsets['.clade2.comp'][0]))
      # out.write("EXTPL STATIC OFFSET: {}\n".format(self.extpl_meta['static_offset']))
      # out.write("EXTPL PAGING OFFSET: {}\n".format(self.extpl_meta['paging_offset']))
      # out.write("EXTPL CLADE1COMP OFFSET: {}\n".format(compextploffset))
      # out.write("EXTPL SIZE: {}\n".format(self.extpl_meta['size']))

    # print "mbn offsets:"
    # for k, v in sorted(self.mbn_offsets.items()):
    #   print k, v

  def extract_dynamic_CLADE_meta(self):
    if len(self.extpl_meta.keys()) < 3:
      raise Exception("Missing extpl meta")

    # get checksum for data portion in static image
    self.mbn_handle.seek(self.extpl_meta['static_offset'])
    s_chksm = hashlib.sha256(self.mbn_handle.read(self.extpl_meta['size'])).hexdigest()

    # compare checksum for data in dynamic image
    paging_bin = os.path.dirname(self.finalmbn_path) + '/qdsp6sw_paging.bin'
    if not pe(paging_bin):
      raise Exception("Cannot find external image: {}".format(paging_bin))

    with open(paging_bin, 'rb') as fh:
      fh.seek(self.extpl_meta['paging_offset'])
      d_chksm = hashlib.sha256(fh.read(self.extpl_meta['size'])).hexdigest()
    assert (s_chksm == d_chksm)
    print "Dynamic CLADE Meta verified"

    with open(self.out_dir + 'extpl_meta.bin', 'wb') as out:
      out.write(struct.pack('i', self.extpl_meta['size']))
      out.write(struct.pack('i', self.extpl_meta['static_offset']))
      out.write(struct.pack('i', self.extpl_meta['paging_offset']))

    print "Wrote dynamic CLADE meta to {}".format(self.out_dir + 'extpl_meta.bin')

  def get_mbn_seg_offset(self, contents, section_sz=None):
    for ph in self.mbn_obj.programHeaderTable:
      start = ph.p_offset
      size = ph.p_filesz
      if size == len(contents):
        if section_sz:
          size = min(size, section_sz)
        self.mbn_handle.seek(start);
        if (hashlib.sha256(contents[0:size]).hexdigest() == hashlib.sha256(self.mbn_handle.read(size)).hexdigest()):
          return ph.p_offset      

  def get_mbn_offsets(self):
    if len(self.mbn_offsets) < 8:
      raise Exception("Metadata table missing information (expecting at least 16 datapoints)")
    return self.mbn_offsets

  def dump_clade_meta(self):
    print "============================================dump_clade_meta============================================"
    if len(self.mbn_offsets) < 8:
      raise Exception("Metadata table missing information (expecting at least 16 datapoints)")

    with open(self.out_dir + 'fota_meta.bin', 'wb') as out:
      print "META_1"
      for item in META_1:
        print item, self.mbn_offsets[item][0]
        print item, self.mbn_offsets[item][1]
        out.write(struct.pack('I', self.mbn_offsets[item][0]))
        out.write(struct.pack('I', self.mbn_offsets[item][1]))
      print "META_2"
      for item in META_2:
        print item, self.mbn_offsets[item]
        out.write(struct.pack('I', self.mbn_offsets[item]))

    print "Wroteout FOTA metadata to {}".format(self.out_dir + 'fota_meta.bin')

  def dump_clade1(self):
    print "inside dump_clade1:-------"
    # dump out dictionary, compressed segment, and uncompressed high/low regions
    # high exception exists in TCM segment instead of clade compressed segment but ignored for FOTA
    seg_idx = -1
    for s in self.clade1_comp:
      print s
      if s[0] == ".clade.dict":
        seg_idx = s[4]
        with open(self.out_dir + 'dict.bin', 'wb') as out:
          offset = s[1]
          size = s[2]
          self.elf_handle.seek(offset)
          out.write(self.elf_handle.read(size))

    if seg_idx == -1:
      raise Exception("Could not find clade1 dictionary info")

    phdr = self.elf_obj.getSegmentByIndex(seg_idx)
    if phdr == elfConstants.RC_ERROR:
      raise Exception('Could not retrieve segment index {} from orig elf'.format(seg_idx))
    self.elf_handle.seek(phdr.p_offset)
    contents = self.elf_handle.read(phdr.p_filesz)
    checksum = hashlib.sha256(contents).hexdigest()
    with open(self.out_dir + 'clade1comp.bin', 'wb') as out:
      out.write(contents)

    with open(self.out_dir + 'clade1comp.checksum', 'wb') as out:
      out.write(checksum)

    print "Wroteout clade1 compressed contents to {}".format(self.out_dir + 'clade1comp.bin')

    # find clade1 uncompressed segments
    uncompseg = {}
    for s in self.clade1_uncomp:
      seg_idx = s[4]
      uncompseg[seg_idx] = 0

    for count, seg_idx in enumerate(sorted(uncompseg.keys())):
      phdr = self.elf_obj.getSegmentByIndex(seg_idx)
      if phdr == elfConstants.RC_ERROR:
        raise Exception('Could not retrieve segment index {} from orig elf'.format(seg_idx))
      self.elf_handle.seek(phdr.p_offset)
      if count == 0:
        with open(self.out_dir + 'high.bin', 'wb') as out:
          self.elf_handle.seek(phdr.p_offset)
          out.write(self.elf_handle.read(phdr.p_filesz))
      elif count == 1:
        with open(self.out_dir + 'low.bin', 'wb') as out:
          self.elf_handle.seek(phdr.p_offset)
          out.write(self.elf_handle.read(phdr.p_filesz))
      else:
        raise Exception('More than 2 uncompressed segments found')

    print "Wroteout clade1 uncompressed contents to high.bin and low.bin in {} directory".format(self.out_dir)


  def dump_clade2(self):

    clade2_uncomp_idx = self.clade2_uncomp[0][4]
    contents, phdr = get_segment_contents(self.elf_handle, self.elf_obj, clade2_uncomp_idx)
    with open(self.out_dir + 'clade2uncomp.bin', 'wb') as out:
      out.write(contents)
    print "Wrote clade2 uncompressed data to {}".format(self.out_dir + 'clade2uncomp.bin')

    clade2_comp_idx = self.clade2_comp[0][4]
    contents, phdr = get_segment_contents(self.elf_handle, self.elf_obj, clade2_comp_idx)
    checksum = hashlib.sha256(contents).hexdigest()
    with open(self.out_dir + 'clade2comp.bin', 'wb') as out:
      out.write(contents)

    with open(self.out_dir + 'clade2comp.checksum', 'wb') as out:
      out.write(checksum)

    print "Wrote clade2 compressed data to {}".format(self.out_dir + 'clade2comp.bin')

  # Correct way to call is from the new build, and pass in meta 
  # and dump directory from the old build
  def make_fota_image(self, clade1, clade2, dump_dir):
    if not pe(ap(dump_dir)):
      raise Exception("Could not find old dump directory: {}".format(dump_dir))

    if not pe(dump_dir + '/clade1comp.bin'):
      raise Exception("Could not find {}/clade1comp.bin: {}".format(dump_dir))

    # if not pe(dump_dir + '/clade2comp.bin'):
      # raise Exception("Could not find {}/clade2comp.bin: {}".format(dump_dir))

    # if not pe(dump_dir + '/clade_extpl.bin'):
      # raise Exception("Could not find {}/clade_extpl.bin: {}".format(dump_dir))

    clade1end = 0
    clade2end = 0
    for name, data in self.mbn_offsets.items():
      if name == '.clade.comp':
        clade1start = data[0]
        continue
      if name == '.clade2.comp':
        clade2start = data[0]
        continue
      if '.clade.' in name:
        end = data[0] + data[1]
        clade1end = max(clade1end, end)
        continue
      if '.clade2.' in name:
        end = data[0] + data[1]
        clade2end = max(clade2end, end)
        continue

    clade1size = clade1end - clade1start
    # clade2size = clade2end - clade2start

    # Writeout fota static image here
    with open(self.out_dir + '9205_fota_image_POI.mbn', 'wb') as out:
      self.mbn_handle.seek(0)
      out.write(self.mbn_handle.read(clade1start))

      if clade1:
        filesize = os.stat(dump_dir + '/clade1comp.bin').st_size
        IOsize = min(filesize, clade1size)
        with open(dump_dir + '/clade1comp.bin', 'rb') as clade1:
          out.write(clade1.read(IOsize))
          # print "current outsize is {}".format(out.tell())

        if clade1size > IOsize: # pad with zeros if old section is smaller than new section
          out.write(b'\x00' * (clade1size - IOsize))
      else:
        out.write(self.mbn_handle.read(clade1size))

      # self.mbn_handle.seek(clade1end)
      # out.write(self.mbn_handle.read(clade2start - clade1end))

      # if clade2:
        # filesize = os.stat(dump_dir + '/clade2comp.bin').st_size
        # IOsize = min(filesize, clade2size)
        # with open(dump_dir + '/clade2comp.bin', 'rb') as clade2:
          # out.write(clade2.read(IOsize))
          # print "current outsize is {}".format(out.tell())

        # if clade2size > IOsize: # zero padding for clade2
          # out.write(b'\x00' * (clade2size - IOsize))
      # else:
        # out.write(self.mbn_handle.read(clade2size))

      self.mbn_handle.seek(clade1end)
      out.write(self.mbn_handle.read())

    print "Wrote FOTA optimized static image to {}".format(self.out_dir + '9205_fota_image.mbn')

    # Writeout fota dynamic paging image here
    # with open(self.out_dir + '9205_fota_image_paging.bin', 'wb') as out, open(self.pagingmbn_path, 'rb') as orig:
      # extplsize = self.extpl_meta['size']
      # filesize = os.stat(dump_dir + '/clade_extpl.bin').st_size
      # IOsize = min(filesize, extplsize)
      # out.write(orig.read(self.extpl_meta['paging_offset']))
      # with open(dump_dir + '/clade_extpl.bin', 'rb') as extpl:
        # out.write(extpl.read(IOsize))
      # if extplsize > IOsize:
        # out.write(b'\x00' * (extplsize - IOsize))

      # orig.seek(self.extpl_meta['paging_offset'] + self.extpl_meta['size'])
      # out.write(orig.read())

    # print "Wrote FOTA optimized paging image to {}".format(self.out_dir + '9205_fota_image_paging.bin')

# this function is used to manipulate and read elf debug information
def import_build_scripts(path, compressor_root=None):     
  anchor_str = 'modem_proc'
  split_dir = ap(path).split(os.sep)

  anchor_dir = None
  for search_dir in (split_dir,):
    if anchor_str in search_dir:
      anchor_dir = os.sep.join(search_dir[0:search_dir.index(anchor_str) + 1])
      break

  if not anchor_dir:
    raise Exception('The specified path does not contain modem_proc')
  
  print "anchor_dir = ", anchor_dir
  sys.path.insert( 0, ap(pj(anchor_dir, 'perf', 'scripts', 'elfManipulator',)))

  compressorDirs = [
    ap(pj(anchor_dir, "core/kernel/dlpager/compressor")),
    ap(pj(anchor_dir, "perf/q6zip/core/compressor")),
    ap(pj(anchor_dir, "perf/scripts/elfManipulator/include")),
  ]

  try:
    from include import elfFileClass
    from include import elfConstants
    
    globals().update(
      elfFileClass=elfFileClass,
      elfConstants=elfConstants)

  except ImportError:
    sys.stderr.write(' '.join([
      'Could not import ELF manipulator. Please place this script somewhere',
      'along <build_root>/modem_proc/build/ms.\n']))
    raise

def find_section_with_substring(elf, key):
    sht = elf.sectionHeaderTable
    for sh in sht:
      if key in sh.sh_nameStr:
        return sh
    return None

def get_section_data(elf, section_name):
  section = elf.getSectionByName(section_name)
  if section == elfConstants.RC_ERROR:
    print "Could not find section in elf: {}".format(section_name)
    return None
  offset = section.sh_offset
  size = section.sh_size
  sect_index = section.idx
  seg_index = get_segidx_from_sectidx(elf, sect_index)
  print "\tFunction get_section_data returning----section_name: ",section_name," Offset:",offset," Size: ",size," sect_index: ",sect_index," seg_index: ",seg_index
  return (section_name, offset, size, sect_index, seg_index)

def is_elf(file):
  """Verifies a file as being an ELF file"""
  file.seek(0)
  magic = file.read(4)
  file.seek(0)
  return magic == '\x7fELF'

def get_segidx_from_sectidx(elf, section_index):
  for seg_idx, sec_indices in elf.sectionToSegmentDict.items():
    if section_index in sec_indices:
      return seg_idx
  raise Exception("Could not find segment index from section index %d" % section_index)

def get_segment_contents(file, elf, segment_index):
  phdr = elf.getSegmentByIndex(segment_index)
  if phdr == elfConstants.RC_ERROR:
    raise Exception('Could not retrieve segment index {} from elf'.format(CLADE_seg_index))
  file.seek(phdr.p_offset)
  contents = file.read(phdr.p_filesz)
  return contents, phdr

def get_seg_extension(elf_file, elf_obj, contents, section_sz=None):
  for ph in elf_obj.programHeaderTable:
    start = ph.p_offset
    size = ph.p_filesz
    if size == len(contents):
      if section_sz:
        size = min(size, section_sz)
      elf_file.seek(start);
      if (hashlib.sha256(contents[0:size]).hexdigest() == hashlib.sha256(elf_file.read(size)).hexdigest()):
        return ph.idx
  return None

def print_meta(meta):
  return 0

def clade_dict_writeout(data, cladeoffset, elf):
  with open(elf, 'rb') as elf_fh, open('dict.bin', 'wb') as dict_fh:
    offset = data[0] + cladeoffset
    size = data[1]
    elf_fh.seek(offset)
    dict_fh.write(elf_fh.read(size))

__desc__ = ''' Create FOTA optimized build using before/after images for minimal delta '''
__footer__ = ''' After running script, the following files can be used to verify FOTA files '''


def main(argv):

  parser = argparse.ArgumentParser(
    description=__desc__,
    epilog=__footer__)
  parser.add_argument('old_orig_elf', action='store', type = str, help="original elf (orig.XXX.XXX.elf) for old build")
  parser.add_argument('old_mbn_dir', action='store', type = str, help="directory where final modem images exist (qdsp6sw.mbn & qdsp6sw_paging.mbn) for old build")
  parser.add_argument('new_orig_elf', action='store', type = str, help="original elf (orig.XXX.XXX.elf) for new build")
  parser.add_argument('new_mbn_dir', action='store', type = str, help="directory where final modem images exist (qdsp6sw.mbn & qdsp6sw_paging.mbn) for new build")
  parser.add_argument('build_path', action='store', type = str, help="build directory (<build_flavor>/modem_proc) used to detect build settings")

  args = parser.parse_args()

  if '-h' in argv or '--help' in argv:
    print "Help option:"
    parser.print_help()
    return 0

  if len(argv) != 5:
    print "Incorrect Arguments supplied, got {}, expected 5:".format(len(argv))
    parser.print_help()
    return 0

  if not pe(args.build_path):
    sys.stderr.write('Could not find specified build path %s\n' % args.build_path)
    return 1

  if not pe(args.old_mbn_dir + '/qdsp6sw_2.mbn'):
    sys.stderr.write('Could not find final images in build path %s\n' % old_mbn_dir)
    return 1

  if not pe(args.new_mbn_dir + '/qdsp6sw_2.mbn'):
    sys.stderr.write('Could not find final images in build path %s\n' % new_mbn_dir)
    return 1

  import_build_scripts(args.build_path)

  oldbuild = CLADE_Extractor(args.old_orig_elf, args.old_mbn_dir + '/qdsp6sw_2.mbn', args.old_mbn_dir + '', "old/")
  oldbuild.extract_clade_info()
  oldbuild.dump_clade1()
  # oldbuild.dump_clade2()
  oldbuild.find_mbn_offsets()
  # oldbuild.extract_dynamic_CLADE_meta()
  oldbuild.dump_clade_meta()
  # oldbuild.make_fota_image(CLADE1ENABLED, CLADE2ENABLED, oldbuild.get_mbn_offsets(), "old")

  newbuild = CLADE_Extractor(args.new_orig_elf, args.new_mbn_dir + '/qdsp6sw_2.mbn', args.new_mbn_dir + '', "new/")
  newbuild.extract_clade_info()
  newbuild.dump_clade1()
  # newbuild.dump_clade2()
  newbuild.find_mbn_offsets()
  # newbuild.extract_dynamic_CLADE_meta()
  newbuild.dump_clade_meta()

  newbuild.make_fota_image(CLADE1ENABLED, CLADE2ENABLED, "old")

  return 0

if __name__ == "__main__":
  sys.exit(main(sys.argv[1:]))
