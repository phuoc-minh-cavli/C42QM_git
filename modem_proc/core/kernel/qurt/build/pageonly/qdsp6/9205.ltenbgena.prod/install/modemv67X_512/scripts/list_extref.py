#=============================================================================
#
#                                    list_extref.py
#
# GENERAL DESCRIPTION
#
# EXTERNAL FUNCTIONS
#        None.
#
# INITIALIZATION AND SEQUENCING REQUIREMENTS
#        None.
#
#             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
#=============================================================================
import os, sys, re, subprocess, optparse
from lib.qurt import *

#globals
libs=[]
undefdict = dict()
defdict_obj = dict()
defdict_arch = dict()
defdict_sect = dict() 
defdict_arch_obj = dict()   
size_obj = dict()
size_sym = dict()
size_arch = dict()
size_data = dict() #(.data .data.*), (.sdata .sdata.*)
size_text = dict() #(.text .text.*)
size_bss = dict() #(.bss .bss.* ) (.sbss .sbss.*)
size_common = dict() #(COMMON) (.scommon .scommon.*)
size_rodata = dict() #(.rodata .rodata.*)
size_other = dict() # Any unaccounted section
symbol_list=[]
csv_list = []

start_addr=0
end_addr=0
Total_size=0
elf_symbol_list=[]
isymbol_list=[]
diff_size=0
section_align_size=0
Symbol_align=0
total_section_size=0
unused_lib_size=[]
aliased_symbol_size=0
pd_summary = ""


#
# Most of the below class and methods are taken from
# lib/build_qurt_config.py and modified to suit the needs here
#
class find_external_references_relocation(object):
    def __init__(self, infilename):
        self.infilename = infilename
    def section_at(self, index):
        #
        #  Return the section header for (index).
        #  Clips (index) so that we don't fault -- if index is greater than the
        #   number of section headers, return section header for 0.
        #
        if index >= self.ehdr.e_shnum:
            index = 0
        return self.shdr[index]
    def run(self):
        import StringIO
        self.infile = open(self.infilename, 'rb')
        self.ehdr = Elf32Header(self.infile)
        self.infile.seek(self.ehdr.e_shoff)
        self.shdr = [Elf32SectionHeader(self.infile, loc=loc) for loc in range(self.ehdr.e_shnum)]
        for s in self.shdr:
            s.f = self.infile
        strtab_section = self.shdr[self.ehdr.e_shstrndx]
        strtab = strtab_section.contents().read()
        for s in self.shdr:
            s.name = strtab[s.sh_name:].split('\0')[0]
        #
        # Set "ring" levels for each section,
        #  with the idea that relocations from an inner ring to an
        #  outer ring are worth reporting as a potential problem.
        #
        # Rings:
        #   0 : *island
        #   1 : All other sections
        #
        for s in self.shdr[:]:
            s.ring = 1
            if (s.sh_flags & s.SHF_ALLOC) != 0:
				if s.name.endswith('island'):
					s.ring = 0
        strtab_section.sh_offset = 0
        strtab_section.f = StringIO.StringIO(strtab)

        #
        # Find the symbol table in the object file.
        # Requirements:
        #  * Must be one section header of type SHT_SYMTAB
        #  * Must have the name .symtab
        #  * Must indicate a symbol table entry size of 16 bytes
        #  * Section size must be a multiple of 16 bytes
        #
        symtabs = [s for s in self.shdr if s.sh_type == s.SHT_SYMTAB]
        if len(symtabs) != 1:
            raise Exception('Object must have one SYMTAB section')
        symtab = symtabs[0]
        if symtab.name != '.symtab':
            raise Exception('SYMTAB section must be named .symtab')
        if symtab.sh_entsize != 16:
            raise Exception('SYMTAB entries must be 16 bytes each')
        if symtab.sh_size % 16 != 0:
            raise Exception('SYMTAB size must be a multiple of 16 bytes')
        #
        # Go read all of the symbol table entries.
        #
        symreader = symtab.contents()
        self.syms = [Elf32Sym(symreader) for _ in range(symtab.sh_size/16)]
        strtab = self.shdr[symtab.sh_link].contents().read()
        for s in self.syms:
            if s.st_name:
                s.name = strtab[s.st_name:].split('\0')[0]
            else:
                s.name = ''
        for s in self.syms:
            if s.st_shndx > 0 and s.st_shndx < self.ehdr.e_shnum:
                s.section_name = self.shdr[s.st_shndx].name
            else:
                s.section_name = ''

        #
        # Sort all symbols to get the based on the offsets
        #
        offsyms = [self.syms[0]]
        offsyms.extend(sorted(self.syms[1:], key=lambda s: s.st_value))
        
        # 
        # Create a dictionary where key is st_shndx and value is a tuple of [offset, symbol name]
        # Note that before going through the symbol table its been sorted based on the offsets. 
        #
        dsyms = {}
        for s in offsyms:
            st_type = s.st_info & 0x0f
            if st_type == 1 or st_type == 2: # Add it to the dictonary only if the symbol type is OBJECT or FUNC
#            if s.st_shndx < self.ehdr.e_shnum:
                try:
                    dsyms[s.st_shndx].append([s.st_value, s.name])
                except KeyError:
                    dsyms[s.st_shndx] = [[s.st_value, s.name]]
#                print [s.name, hex(s.st_value), s.st_size, s.st_info, s.st_shndx, s.st_other]
 
        #
        # For each RELA section, read in all RELA elements,
        #  and fix their symbol table indexes.  Store back
        #  the modified info as a pseudo-file.
        #
        undefined_syms = {}
        for s in self.shdr:
            if s.sh_type == s.SHT_RELA:
                relareader = s.contents()
                relas = [Elf32Rela(relareader) for _ in range(s.sh_size/s.sh_entsize)]
                allsyms = set()
                for r in relas:
                    this_section = self.shdr[s.sh_info]
                    that_sym = self.syms[r.get_sym()]
                    that_section = self.section_at(that_sym.st_shndx)
                    if that_section.ring > this_section.ring:
                        if that_sym.name == '':      
                            # If the symbol name doesn't exits, record the section name instead
                            that_sym.name = that_section.name
                        # dsym is dictionary of symbols which has tuple of offset from the section and symbol name. 
                        # In the below loop, for the section in question, we will find out to which symbol does the
                        # r.r_offset belong to.
                        for dsym in dsyms[s.sh_info]:
                            if dsym[0] > r.r_offset:
                                break
                            this_sym = dsym

                        objstr = defdict_arch[this_sym[1]]+':'+defdict_obj[this_sym[1]]
                        referencers = set()
                        if undefined_syms.has_key(that_sym.name):
                            referencers = set(undefined_syms[that_sym.name])
                        referencers.add((this_sym[1],objstr))
                        undefined_syms[that_sym.name] = referencers

                        allsyms.add(that_sym.name)

                        referencers = set()
                        if undefdict.has_key(that_sym.name):
                            referencers = set(undefdict[that_sym.name])
                        referencers.add(objstr)
                        undefdict[that_sym.name]=referencers

#                        referencers = []
#                        if undefdict.has_key(that_sym.name):
#                            referencers = list(undefdict[that_sym.name])
#                        referencers.append(objstr)
#                        undefdict[that_sym.name]=referencers

        undefined_syms_set = set(undefdict.keys())
        print "################################################################################"
        print "# Undefined Island Symbols                                                     #"
        print "################################################################################"
        for s in (undefined_syms_set - ignore_syms):
            print "Symbol: %s" % s
            print "Callers: "
            for t in undefined_syms[s]:
                caller_str = "         %s" % t[0]
                if t[0] in defdict_arch:
                    caller_str += " in %s" % t[1]
                print caller_str
            print "--------------------------------------------------------------------------------"
        print "Total Undefined: %d" % len(undefined_syms_set - ignore_syms)

#CompareSymbols compares each symbol it got from island_analysis.txt and compares it with the symbol list from elf
def CompareSymbols():
    global elf_symbol_list
    global diff_size
    global unused_lib_size
    prev_lib=''
    prev_size=0
    current_size=0
    lib_size=0
    count=0
    elf_symbol_list=dict(elf_symbol_list)
    print'##############################################################'
    print'                  UNUSED SYMBOLS                              '
    print'##############################################################'
    for isym in symbol_list:
        if (isym[0] not in elf_symbol_list):
            print [isym[0],isym[1],isym[2]]
            diff_size+=isym[1]
            current_size = isym[1]
            if(prev_lib == isym[2]):
                lib_size = lib_size + current_size
            else:
                if count > 0 :
                    unused_lib_size.append([prev_lib,lib_size])
                    lib_size=0
                lib_size = current_size
            count=1
            prev_lib = isym[2]
            prev_size = current_size
    unused_lib_size.append([prev_lib,lib_size])            #for last lib
    print '\n'
    print "Total size of unused symbols: %u" % (diff_size)
    print '\n'

    unused_lib_size.sort(key=lambda x: x[1],reverse=1) #sort by size
    print "*************** Unused symbols per Lib ***************"
    for lib in unused_lib_size:
        if re.search(r'(\w+[.]\w+)', lib[0]) is not None:
            m = re.search(r'(\w+[.]\w+)', lib[0])
            lib[0] = m.group(1)
        print  "Unused symbols in \"%s\": %d bytes" % (lib[0],lib[1])
    print '\n'


# ParseElf is used to extract start and end addresses of island section
# It basically looks for anything ending with island and stores the first and last address
# It also extracts all the symbols from island section
def ParseElf(elfout):
    global start_addr
    global end_addr
    global elf_symbol_list
    global Total_size
    global aliased_symbol_size
    global section_align_size
    global total_section_size
    global Symbol_align
    section_addresses = []
    in_island_sections = 0
    prev_addr=0
    prev_size=0
    current_addr=0
    prev_name=''
    total_sec_symbol_size=0
    sec_symbol_list=[]
    
    for line in elfout.splitlines():
        secpattern = re.compile('\s{2}(.{4})\s+(.*\.\S*)+\s*(.*)\s+\s{7}(\S{8})\s+(\S*)\s+(\S*)\s+(\S*)\s+(\S*)\s+(\S*)\s+(\S*)\s+(\S*)\s')
        sympattern = re.compile('\s*(.*):\s+(\S{8})\s+\s*(\S*)\s+(\S*)\s+\s*(\S*)\s+\s*(\S*)\s+\s*(\S*)\s+(\S*)')

        secmatch = secpattern.match(line)
        symmatch = sympattern.match(line)
        if(secmatch):
            if('island' in secmatch.group(2)):
                if not in_island_sections:
                    start_addr = secmatch.group(4)
                    in_island_sections=1
                    prev_size=int(start_addr,16) #take care of first section
                else:
                    end_addr = int(secmatch.group(4),16)+int(secmatch.group(6),16)
                    end_addr = hex(end_addr)[2:-1]
                if((prev_addr+prev_size) < int(str(secmatch.group(4)),16)):
                    section_align_size += int(str(secmatch.group(4)),16) - (prev_addr+prev_size)
                prev_addr = int(secmatch.group(4),16)
                prev_size = int(secmatch.group(6),16)
                total_section_size += int(str(secmatch.group(6)),16)
                #store all the section addresses in form - Section start address, section end address, section size
                section_addresses.append([secmatch.group(4),hex(int(secmatch.group(4),16)+int(secmatch.group(6),16))[2:-1],int(secmatch.group(6),16)]) 
        elif(symmatch):
            if(('FILE' not in symmatch.group(4)) and ('SECTION' not in symmatch.group(4))):
                 if((symmatch.group(2) >= start_addr) and (symmatch.group(2) < end_addr)):
                     #store all the island symbols in form - Symbol name, symbol address, symbol size.
                     elf_symbol_list.append((symmatch.group(8),(symmatch.group(2),symmatch.group(3))))
                     if symmatch.group(3).startswith('0x'):
                         Total_size+=int(str(symmatch.group(3)),16)
                     else:
                         Total_size+=int(str(symmatch.group(3)))
    elf_symbol_list.sort(key=lambda x: x[1]) #sort by address
    print "#############################################"
    print "             Aliased Symbols                 "
    print "#############################################"
    for sym in elf_symbol_list:
        current_addr = sym[1][0]
        if(current_addr == prev_addr):
            Total_size -= int(sym[1][1],10)  #adjust the total size by substracting aliazed island symbol size
            if(prev_name == sym[0]): #This is to take care of symbol names that repeat in symbol table.These dont seem to be aliased symbols e.g. qurt_mutex_unlock
                prev_name = sym[0]
                continue
            aliased_symbol_size += int(sym[1][1],10)
            print sym[0]+' '+sym[1][1]  #prinout the aliased symbol name and size
        prev_addr = current_addr
        prev_name = sym[0]
    print '\n'
    print "Total Aliased symbol size: %u" % (aliased_symbol_size)
    print '\n'

    # We can have symbol alignments which can cause the total section size to be higher than sumation of individual symbol sizes within that section.
    # Below code will find out the symbol alignments per section and addup the total alignment for all island sections
    for section in section_addresses:
        Get_SymbolAlignment(section[0],section[1],section[2])

# Get_SymbolAlignment gets the symbol alignments per section and adds it up to give total symbol alignments for all island sections
def Get_SymbolAlignment(start, end, sec_size):
    global Symbol_align
    sec_symbol_list = []
    total_sec_symbol_size = 0
    current_addr = 0
    prev_addr = 0
    prev_name = ''

    #We already have the island symbol list. Use that to generate a list of symbols within the section. Start and end addresses for section are passed to this function
    for sym in elf_symbol_list:
         if((sym[1][0] >= start) and (sym[1][0] < end)):
            sec_symbol_list.append(sym)
            if sym[1][1].startswith('0x'):
                total_sec_symbol_size+=int(str(sym[1][1]),16)
            else:
                total_sec_symbol_size+=int(str(sym[1][1]),10)

    # Now we have a list of symbols for passed section. Find out the aliased symbols, count those out.
    for sym in sec_symbol_list:
        current_addr = sym[1][0]
        if(current_addr == prev_addr):
            if sym[1][1].startswith('0x'):
                total_sec_symbol_size -= int(str(sym[1][1]),16)
            else:
                total_sec_symbol_size -= int(str(sym[1][1]),10)
            if(prev_name == sym[0]): #This is to take care of symbol names that repeat in symbol table.These dont seem to be aliased symbols e.g. qurt_mutex_unlock
                prev_name = sym[0]
                continue
        prev_addr = current_addr
        prev_name = sym[0]
        
    #after removing aliased symbols from section, the total size of symbols within a section could be less than the actual section size that linker reports
    # the difference between two is due to symbol alignment.
    if(total_sec_symbol_size != sec_size):
        Symbol_align += sec_size-total_sec_symbol_size

#Print out island analysis summary in csv format
def IslandCSVout(csv_out):
    import sys
    global pd_summary
    core_root = 0
    user_PD = 0
    pd_print_message = ""
    tools_root = 0
    root_total = 0
    start = 0
    libs = dict(unused_lib_size)
    discrepancy_total =0
    core_discrepancy = 0
    pd_discrepancy = 0
    tools_discrepancy = 0
    check = 0
    alignment = 0
    alignment += Symbol_align
    tools_discrepancy += aliased_symbol_size
    # Open analysis file.
    try:
        csv_fh=open(csv_out[0], 'w+')
        try:                    
            csv_fh.write('Area,Subsystem,Functionality,Object File,Size(KB),Discrepancy(bytes) \n')
            for line in csv_list:
               check = 0
               if re.search(r'.*_proc[/\\]?([.a-zA-Z0-9_]*?)[/\\]([.a-zA-Z0-9_]*?)[/\\]([.a-zA-Z0-9_]*?)[/\\].*[/\\]([.a-zA-Z0-9_]*?): ([.0-9]*?) KB', line) is not None: 
                  m = re.search(r'.*_proc[/\\]?([.a-zA-Z0-9_]*?)[/\\]([.a-zA-Z0-9_]*?)[/\\]([.a-zA-Z0-9_]*?)[/\\].*[/\\]([.a-zA-Z0-9_]*?): ([.0-9]*?) KB', line)
                  if m.group(3) == 'build':
                      if(m.group(4) in libs):
                          csv_fh.write(m.group(1) + ',' + m.group(2) + ',' + '--' + ',' + m.group(4) + ',' + m.group(5) + ',' + str(float(libs.get(m.group(4)))/1024) + '\n')
                          check = 1
                      else:
                          csv_fh.write(m.group(1) + ',' + m.group(2) + ',' + '--' + ',' + m.group(4) + ',' + m.group(5) + '\n')                         
                  else:
                     if(m.group(4) in libs):
                         csv_fh.write( m.group(1) + ',' + m.group(2) + ',' + m.group(3) + ',' + m.group(4) + ',' + m.group(5) + ',' + str(float(libs.get(m.group(4)))/1024) + '\n')
                         check = 1
                     else:
                         csv_fh.write( m.group(1) + ',' + m.group(2) + ',' + m.group(3) + ',' + m.group(4) + ',' + m.group(5) + '\n')
                  if m.group(1) == 'core':
                     try:               
                         core_root += float(m.group(5))
                         if check:
                             core_discrepancy += float(libs.get(m.group(4)))
                     except ValueError:
                         csv_fh.write("Could not convert data to an float. \n")
                  else:
                      try:
                          pd_print_message = str(m.group(1))
                          user_PD += float(m.group(5))
                          if check:
                              pd_discrepancy += float(libs.get(m.group(4)))
                      except ValueError:
                          csv_fh.write("Could not convert data to an float. \n")
               if re.search(r'.*Tools.*[/\\]([.a-zA-Z0-9_]*?): ([.0-9]*?) KB', line) is not None: 
                  m = re.search(r'.*Tools.*[/\\]([.a-zA-Z0-9_]*?): ([.0-9]*?) KB', line)
                  if(m.group(1) in libs):
                      csv_fh.write('Hexagon Tools,--,--,'+m.group(1)+','+ m.group(2) + ',' + str(float(libs.get(m.group(1)))/1024) + '\n')
                      check = 1
                  else:
                      csv_fh.write('Hexagon Tools,--,--,'+m.group(1)+','+ m.group(2) + '\n')
                  try:
                      tools_root += float(m.group(2))
                      if check:
                          tools_discrepancy += float(libs.get(m.group(1)))
                  except ValueError:
                      csv_fh.write("Could not convert data to float. \n")
               if re.search(r'Total: ([.0-9]*?) KB',line) is not None:
                  m = re.search(r'Total: ([.0-9]*?) KB', line)
                  try:
                      root_total =float(m.group(1))
                  except ValueError:
                      csv_fh.write("Could not convert data to an float. \n")
            core_discrepancy = float(format(core_discrepancy/1024, '.2f'))
            pd_discrepancy = float(format(pd_discrepancy/1024, '.2f'))
            tools_discrepancy = float(format(tools_discrepancy/1024, '.2f'))
            alignment = float(format(alignment/1024, '.2f'))
            if user_PD > 0:
                pd_summary += 'Island ' + pd_print_message +  ' PD Summary:' + '\n'
            else:
                pd_summary += 'Island root PD Summary:' + '\n'
            csv_fh.write(',,,CoreBSP,' + str(core_root) + ',' +  str(core_discrepancy) + '\n')
            pd_summary += 'CoreBSP:' + str(core_root - core_discrepancy) + '\n'
            csv_fh.write(',,,' + pd_print_message + ',' + str(user_PD) + ',' +  str(pd_discrepancy) + '\n')
            if pd_print_message is not None:
                pd_summary += pd_print_message + ':' + str(user_PD - pd_discrepancy) + '\n'
            csv_fh.write(',,,Hexagon Tools,' +str(tools_root) + ',' +  str(tools_discrepancy) + '\n')
            pd_summary += 'Hexagon Tools:' + str(tools_root - tools_discrepancy) + '\n'
            csv_fh.write(',,,Total,' + str(core_root + user_PD + tools_root) + ',' +  str(core_discrepancy + pd_discrepancy + tools_discrepancy) + '\n')
            csv_fh.write(',,,Alignment,' + ',' + ',' +  str(alignment) + '\n')
            csv_fh.write(',,,Total Adjusted size,' + str(core_root + user_PD + tools_root + alignment/1024 - (core_discrepancy + pd_discrepancy + tools_discrepancy)) + '\n')
            pd_summary += 'Total Adjusted size:' + str(core_root + user_PD + tools_root + alignment - (core_discrepancy + pd_discrepancy + tools_discrepancy)) + '\n'
        finally:
            csv_fh.close()
    except IOError:
        print "The file does not exist, exiting gracefully"

def generate_discrepancy_report(file , csv_out, exestr):
    #extract symbols and sections from elf
    command=[exestr, "-s", "-W", "-S", str(file[0])]
    p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

    elfout, err = p.communicate()
    if p.returncode != 0:
        raise RuntimeError("%r failed, status code %s stdout %r stderr %r" % (command, p.returncode, Symbols, err))
    ParseElf(elfout)
    if symbol_list:
        CompareSymbols()

    if csv_out:
        IslandCSVout(csv_out)
    print "#############################################"
    print "             Summary                 "
    print "#############################################"
    print '\n'
    print "Data based on section headers:"
    print "Total Island size: %u" % total_section_size
    print '\n'
    print "Data based on symbol table (should be close to section header):"
    print "Total Island symbol sizes in ELF (A): %u" % (Total_size)#+section_align_size)
    print "Symbol Alignment (B): %u" % Symbol_align
    print "Total size (A+B) = %u" % (Total_size+Symbol_align)
    print '\n'


def parseArchive(archive_str, archive):
    global symbol_list
    #get archive match, save archive
    archpattern = re.compile('^(.*):\s')
    archmatch = archpattern.match(archive_str)

    object = None
    current_section = ""
    size_arch[archive] = 0
    size_data[archive] = 0 #(.data .data.*), (.sdata .sdata.*)
    size_text[archive] = 0 #(.text .text.*)
    size_bss[archive] = 0 #(.bss .bss.*) (.sbss .sbss.*)
    size_common[archive] = 0 #(COMMON) (.scommon .scommon.*)
    size_rodata[archive] = 0 #(.rodata .rodata.*)
    size_other[archive] = 0 # Any unaccounted section

    for archline in archive_str.split("\n"):
        # Strip out ".hidden" modifiers
        archline = archline.replace(' .hidden','')
        # handle path and parenthesis from object file string 
        archline = archline.replace('(','/',1)
        archline = archline.replace(')','',1)
        archline = os.path.basename(archline)
        objpattern = re.compile('^(.*\.[a-zA-Z]*):\s')
        sympattern = re.compile('(\S{8}){1}\s+(\S*\s+\S*)\s+?(\S+){1}\s+(\S{8}){1}\s+(\S+){1}\s*')
        objmatch = objpattern.match(archline)
        symmatch = sympattern.match(archline)
        if objmatch:
            current_section = ""
            object = objmatch.group(1)
            size_obj[object] = 0
            size_data[object] = 0 #(.data* .sdata*)
            size_text[object] = 0 #(.text*)
            size_bss[object] = 0 #(.bss .bss.*) (.sbss .sbss.*)
            size_common[object] = 0 #(COMMON) (.scommon .scommon.*)
            size_rodata[object] = 0 #(.rodata*)
            size_other[object] = 0 # Any unaccounted section
        elif symmatch and object:
            if symmatch.group(3) != "*UND*":
                current_section = symmatch.group(3)
            if (((str(archive) in libs) or 
                (str(archive)+":"+object in libs) or 
                (str(archive)+":"+object+"("+current_section+")" in libs) or
                (object+"("+current_section+")" in libs) or
                (str(archive)+"("+current_section+")" in libs) or
                (object in libs) or
                ("("+symmatch.group(3)+")" in libs))):
                if ((symmatch.group(5) != symmatch.group(3)) and ("df" not in str(symmatch.group(2)))):
                    if ((symmatch.group(3) == "*UND*")):
                        referencers = set()
                        objstr = object
                        if archive:
                            objstr = archive + ":" + object
                        if not lcsfile:
                        if undefdict.has_key(symmatch.group(5)):
                            referencers = set(undefdict[symmatch.group(5)])
                        referencers.add(objstr)
                        undefdict[symmatch.group(5)]=referencers
                    else:
                        defdict_obj[symmatch.group(5)]=object
                        defdict_sect[symmatch.group(5)]=current_section
                        if archive:
                            defdict_arch[symmatch.group(5)]=archive
                            
                            if archive not in defdict_arch_obj:
                                defdict_arch_obj[archive] = list()
                            if object not in defdict_arch_obj.get(archive):
                                defdict_arch_obj[archive].append(object)
                            
                            if (('.data' in current_section)or('.sdata' in current_section)):
                                size_of_symbol = int(symmatch.group(4),16)
                                size_data[archive] = size_data[archive] + size_of_symbol
                            elif('.text' in current_section):
                                size_of_symbol = int(symmatch.group(4),16)
                                size_text[archive] = size_text[archive] + size_of_symbol
                            elif(('.bss' in current_section)or('.sbss' in current_section)):
                                size_of_symbol = int(symmatch.group(4),16)
                                size_bss[archive] = size_bss[archive] + size_of_symbol
                            elif('COM' in current_section):
                                size_of_symbol = int(symmatch.group(1).split()[0],16)
                                size_common[archive] = size_common[archive] + size_of_symbol
                            elif('.rodata' in current_section):
                                size_of_symbol = int(symmatch.group(4),16)
                                size_rodata[archive] = size_rodata[archive] + size_of_symbol
                            else:
                                size_of_symbol = int(symmatch.group(4),16)
                                size_other[archive] = size_other[archive] + size_of_symbol

                            size_arch[archive] = size_arch[archive] + size_of_symbol
                        else:
                            if (('.data' in current_section)or('.sdata' in current_section)):
                                size_of_symbol = int(symmatch.group(4),16)
                                size_data[object] = size_data[object] + size_of_symbol
                            elif('.text' in current_section):
                                size_of_symbol = int(symmatch.group(4),16)
                                size_text[object] = size_text[object] + size_of_symbol
                            elif(('.bss' in current_section)or('.sbss' in current_section)):
                                size_of_symbol = int(symmatch.group(4),16)
                                size_bss[object] = size_bss[object] + size_of_symbol
                            elif('COM' in current_section):
                                size_of_symbol = int(symmatch.group(1).split()[0],16)
                                size_common[object] = size_common[object] + size_of_symbol
                            elif('.rodata' in current_section):
                                size_of_symbol = int(symmatch.group(4),16)
                                size_rodata[object] = size_rodata[object] + size_of_symbol
                            else:
                                size_of_symbol = int(symmatch.group(4),16)
                                size_other[object] = size_other[object] + size_of_symbol

                            size_obj[object] = size_obj[object] + size_of_symbol
                        size_sym[symmatch.group(5)] = size_of_symbol

# Main function that prints object references
if __name__ == "__main__":
    libname = ''
    usage = "usage: %prog [options]\nExample:\npython scripts\\footprint\\%prog -l ..\\install\\ADSPv5MP\\lib -l ..\\build\\ADSPv5MP\\libs\\qurt\\src -l ..\\build\\ADSPv5MP\\kernel\\asm -l build\\ADSPv5MP\\qurt_island_t1 -l ..\\build\\ADSPv5MP\\libs\\timer\\asm -l ..\\build\\ADSPv5MP\\libs\\qurt\\asm -l C:\\Qualcomm\\HEXAGON_Tools\\5.0.07\\dinkumware\\lib -s island.ispec -i island_ignore.txt -a island_api.txt"
    parser = optparse.OptionParser(usage=usage)
    parser.add_option('-s', '--spec', dest="spec", action="append", help="spec file to identify libs and objects")
    parser.add_option('-o', '--libs', dest="libs", action="append", default=[], help="specify an object or library")
    parser.add_option('-l', '--location', dest="locations", action="append", default=[], help="location to to find objects and libraries from command line and spec file (optional)")
    parser.add_option('-i', '--ignore', dest="ignore", action="append", default=[], help="symbols to ignore, or file with symbols to ignore (1 per line)")
    parser.add_option('-f', '--feedback', dest="feedback", action="store", default="", help="generate files for feedback, defined.txt and undefined.txt in the directory specified")
    parser.add_option('-t', '--tools', dest="toolsver", action="store", default="5.1.04", help="hexagon version number to find objdump (optional)")
    parser.add_option('-a', '--api', dest="api", action="append", help="list of API that should be defined in Objects and libraries that are specified (optional)")
    parser.add_option('-v', '--verbose', dest="verbose", action="store_true", default=False, help="print everything (optional)")
    parser.add_option('-e', '--executable', dest="exestr", action="store", default="", help="explicit objdump executable")
    parser.add_option('-d', '--debug', dest="debug", action="store_true", default=False, help="experimental")
    parser.add_option('-x', '--elf', dest="elffile", action="append", default=[], help="path for elf")
    parser.add_option('-c', '--csv', dest="csvfile", action="append", default=[], help="Location to save csv with libs summary")
    parser.add_option('-T', '--lcs', dest="lcsfile", action="append", default=[], help="Location LCS file")
    parser.add_option('-m', '--machine', dest="machine", action="append", default=[], help="hexagon version")
    parser.add_option('-L', '--linkerflags', dest="linkflags", action="append", default=[], help="linker flags")
    parser.add_option('-r', '--reflibs', dest="reflibs", action="append", default=[], help="reference libraries/objects to determine dependencies recursively")
    (options, args) = parser.parse_args(sys.argv)

    #setup parameters
    ispec = options.spec
    libs = options.libs
    locations = options.locations
    ignore = options.ignore
    default_tools_version = options.toolsver
    feedback_dir = options.feedback
    api = options.api
    verbose = options.verbose
    exestr = options.exestr
    debug = options.debug
    elffile = options.elffile
    csvfile = options.csvfile
    lcsfile = options.lcsfile
    machine = options.machine
    reflib = options.reflibs
    linkflags = options.linkflags

    
    ret_val = 0
    #where to find objdump
    if exestr == "":
        if os.name == 'posix':
            default_tools_root = os.path.join('/pkg/qct/software/hexagon/releases/tools',default_tools_version)
        elif os.name == 'nt':
            default_tools_root = os.path.join(os.path.realpath('C:/Qualcomm/HEXAGON_Tools'),default_tools_version)
        if int(default_tools_version.split(".")[0]) >= 7:
            exestr = os.path.join(default_tools_root,"Tools/bin/hexagon-llvm-objdump")
        else:
            exestr = os.path.join(default_tools_root,"gnu/bin/hexagon-objdump")
    else:
        exestr = os.path.realpath(exestr)

    import lib.qurt
    trial_link = lib.qurt.Elf32TrialLink()

    #read in any and all ispec files, add libs to libs list
    if ispec:
        for specfile in ispec:
            if os.path.isfile(specfile):
                f = file(specfile) 
                for line in f: 
                    trial_link.add_ispec(line)
                    comment_pattern = re.compile('^#')
                    lib_pattern = re.compile('\**(.*)(\s+)\**(.*)(\s+)\**(.*)\s*')
                    line = line.strip() 
                    comment_match = comment_pattern.match(line)
                    lib_match = lib_pattern.match(line)
                    if comment_match:
                        continue
                    elif lib_match:
                        libstr = ""
                        if lib_match.group(1) != '':
                            if lib_match.group(3) != '':
                                libstr = lib_match.group(1)+':'+lib_match.group(3)
                            else:
                                libstr = lib_match.group(1)
                        elif lib_match.group(3) != '':
                            libstr = lib_match.group(3)
                        if lib_match.group(5) != '':
                            libstr += "("+lib_match.group(5)+")"
                        libs.append(libstr)
                f.close()
            else:
                print "ispec file does not exist: "+specfile

    # This is a hack for now. This has to be done since 
    # on external builds, cust_config doesn't get passed anywhere
    # as part of the spec. Instead, we directly include sections of it
    # in LCS file. So island_analysis.txt doesn't include config.lib
    # However, to make accurate analyisis of external references, we
    # add it to libfiles.
    libqurt_path = []
    for s in libs:
        if "libqurt.a" in s :
            libqurt_path = s
    if 'config.lib' not in libs and 'cust_config.o' not in libs:
        if libqurt_path:
            libqurt_path = libqurt_path.split(':')[0]
            config_lib_path = libqurt_path.rsplit('/', 4)[0]
            config_lib1 = os.path.join(config_lib_path,"config","config.lib")+':'+'cust_config.o'+'(.data.ukernel.island)'
            libs.append(config_lib1)
            config_lib1 = os.path.join(config_lib_path,"config","config.lib")+':'+'cust_config.o'+'(.rodata.ukernel.island)'
            libs.append(config_lib1)
            config_lib1 = os.path.join(config_lib_path,"config","config.lib")+':'+'cust_config.o'+'(.bss.ukernel.island)'
            libs.append(config_lib1)
            config_lib1 = os.path.join(config_lib_path,"config","config.lib")+':'+'cust_config.o'+'(.data.qurtos.island)'
            libs.append(config_lib1)

    #if api list specified, read in list of API that needs to be supported in island
    apis = set()
    if api:
        for apifile in api:
            if os.path.isfile(apifile):
                f = file(apifile) 
                for line in f: 
                    apis.add(line.strip())
                    trial_link.add_apis(line.strip())
                f.close() 
            else:
                apis.add(apifile.strip())
                trial_link.add_apis(apifile.strip())

    ignore_syms = set()
    #if ignore list is specified, read symbols which will be defined to be ignored.
    if ignore:
        for ignorefile in ignore:
            if os.path.isfile(ignorefile):
                f = file(ignorefile) 
                for line in f:
                    if line.startswith('#'):
                        continue
                    defdict_obj[line.strip()]="ignorelist"
                    ignore_syms.add(line.strip())
                    trial_link.add_ignorelist(line.strip())
                f.close() 
            else:
                defdict_obj[ignorefile]="ignorelist"

    i=0
    libfiles=set()
    while(i<len(libs)):
        sectiontemp = libs[i].split('(',1)
        section=None
        if len(sectiontemp)>1:
            section=sectiontemp[1]
        temp = sectiontemp[0].split(":")
        obj=''
        #if first is a drive letter
        if len(temp[0]) == 1:
            file = temp[0]+":"+temp[1]
            if len(temp) > 2:
                obj = temp[2]
        else:
            file = temp[0]
            if len(temp) > 1:
                obj = temp[1]
        #find path
        if not os.path.isfile(file):
            for loc in locations:
                found = False
                fullpathlib = os.path.join(loc,file)
                if os.path.isfile(fullpathlib):
                    if obj:
                        libs[i]=fullpathlib+":"+obj
                    else:
                        libs[i]=fullpathlib
                    libfiles.add(fullpathlib)
                    found = True
                    break
            if section:
                libs[i] += "("+section

            #if not found:
                #print "File could not be found: "+file
        else:
            libfiles.add(file)
        i+=1

    libfiles = set(libfiles)
#    if 'config.lib' not in libfiles:
#        libqurt_path = [s for s in libfiles if "libqurt.a" in s]
#        config_lib_path = libqurt_path[0].rsplit(os.sep, 4)[0]
#        config_lib_path = os.path.join(config_lib_path,"config","config.lib")
#        libfiles.add(config_lib_path)
		
    for libfile in libfiles:
        for libname, objname, fi in lib.qurt.QurtElfContainer(libfile):
            trial_link.add_object(lib.qurt.Elf32Object(fi, libname=libname, objname=objname))

        command=[exestr, "-t", libfile]
        p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        out, err = p.communicate()
        parseArchive(out, libfile);

    reflibs = set()
    if reflib:
        for reflibfile in reflib:
            if os.path.isfile(reflibfile):
                f = file(reflibfile) 
                for line in f:
                    if line.startswith('#'):
                        continue
                    reflibs.add(line.strip())
                f.close() 
            else:
                reflibs.add(reflibfile.strip())

    if (len(libs) < 1) and (len(reflibs) < 1):
        print "No Libs or Objects have been specified."
        exit(-1)
    for reflib in reflibs:
        for libname, objname, fi in lib.qurt.QurtElfContainer(reflib):
            trial_link.add_object(lib.qurt.Elf32Object(fi, libname=libname, objname=objname))

    minspec_sections =[]
    minspec_symbols =[]
    undef_syms = []
    if api:
        total_original_size, minspec_symbols, minspec_sections, undef_syms = trial_link.run()
	
    total_new_spec_size = 0
    for sym in minspec_symbols:
        total_new_spec_size += int(sym[1])

    if minspec_sections:
        print "################################################################################"
        print "# Minimum Spec                                                                  #"
        print "################################################################################"
        for libs in minspec_sections:
            for objs in minspec_sections[libs]:
                for sections in minspec_sections[libs][objs]:
                    lib_str = '*'
                    obj_str = '*'
                    sec_str = '*'
                    if libs:
                        lib_str = str(libs)
                    if objs:
                        obj_str = str(objs)
                    if sections:
                        sec_str = str(sections)
                       
                    print lib_str+" "+obj_str+" "+sec_str
                    
        print "\n"
        print "################################################################################"
        print "# Total Original size of the spec: %d bytes                                    #" % total_original_size
        print "# Size added due to new spec: %d bytes                                         #" % total_new_spec_size
        print "################################################################################"
        print "\n"
        
        undef_syms=set(undef_syms)
        print "################################################################################"
        print "#  Symbols referenced but not found in libraries (%d)                          #" %len(undef_syms)
        print "################################################################################"
        for sym in undef_syms:
            print sym
        print "\n"
        minspec_symbols=set(minspec_symbols)
        print "################################################################################"
        print "#  New Symbols being added (%d)                                                #" %len(minspec_symbols)
        print "################################################################################"
        for sym in minspec_symbols:
            print sym[0]  
        print "\n"
    undefined = set(undefdict.keys())
    defined = set(defdict_obj.keys())

    if verbose:
        print "################################################################################"
        print "# Defined Symbols                                                              #"
        print "################################################################################"
        
        for sym in sorted(defined):
            callers_str = "Caller:"
            if undefdict.has_key(sym):
                first = True
                for obj in undefdict[sym]:
                    if first:
                        callers_str += "\t"+obj
                        first = False
                    else:
                        callers_str += "\n\t"+obj
            else:
                callers_str += " Local or None"
            symsize = 0
            if sym in size_sym:
                symsize = size_sym[sym]
            print "Symbol:\t"+sym + " (" + str(symsize) +"bytes)"
            def_str = "Defined: "
            if sym in defdict_arch:
                def_str = def_str + defdict_arch[sym]
                libname = os.path.split(defdict_arch[sym])
                symbol_list.append([sym,symsize,libname[1]])
            print def_str + ":" + defdict_obj[sym]
            print callers_str
            print "--------------------------------------------------------------------------------"
        print "\nTotal: "+str(len(defined))+"\n"

        objset = set(defdict_obj.values())
        for file in sorted(size_arch, key=size_arch.get, reverse=True):
            print "\n%-69.69s: %.2f KB" % (str(file)[-69:], float(size_arch[file])/1024)
            if file in defdict_arch_obj:
                #for obj in defdict_arch_obj[file]:
                for obj in sorted(defdict_arch_obj[file]):
                    if obj in objset:
                        print "    " + obj + ":"
                        for sym in defdict_obj:
                            if defdict_obj[sym] == obj:
                                print "        %-56.56s: %s bytes" % (sym[-56:], str(size_sym[sym]))

    if feedback_dir != "":
        def_out = os.path.join(feedback_dir,"defined.txt")
        undef_out = os.path.join(feedback_dir,"undefined.txt")
        def_fh=open(def_out, 'w+')
        undef_fh=open(undef_out, 'w+')
        for sym in (defined):
            def_fh.write(sym+"\n")
        for sym in (undefined - defined):
            undef_fh.write(sym+"\n")

    undef_total=len(undefined - defined)
    if lcsfile and machine:
        import shutil
        compiler = exestr.rsplit(os.sep, 1)[0]
        compiler = os.path.join(compiler,"hexagon-link ")
        lcs_abspath = os.path.abspath(lcsfile[0])
        prelink_lcs = os.path.join(os.path.split(lcs_abspath)[0],"prelink.lcs")
        shutil.copy(lcs_abspath,prelink_lcs)

        prelink_output = prelink_lcs.rsplit(os.sep, 1)[0]
        prelink_output = os.path.join(prelink_output, "island_prelink_out.elf")

        compiler += " -Map "+str(prelink_output)+".map "
        compiler += " -g -G0 -r -nostdlib --whole-archive --allow-multiple-definition -d -m"+machine[0]+" "

        for flag in linkflags:
            compiler += "-L "+flag+" "
        compiler += " --script="+prelink_lcs+ " "
        compiler += " --start-group "
        for libfile in libfiles:
            compiler += libfile + " "
        for reflib in reflibs:
            compiler += reflib + " "
        compiler += " --end-group "

        compiler += " -o "+str(prelink_output)				
        print compiler

        extref = os.system(compiler)
        find_external_references_relocation(prelink_output).run()
    else:
    if undef_total > 0:
        print "################################################################################"
        print "# Undefined Island Symbols                                                     #"
        print "################################################################################"
        for sym in (undefined - defined):
            callers_str = "Caller:"
            first = True
            for obj in undefdict[sym]:
                if first:
                    callers_str += "\t"+obj
                    first = False
                else:
                    callers_str += "\n\t"+obj
            print "Symbol:\t"+sym
            print callers_str
            print "--------------------------------------------------------------------------------"
        print "\nTotal Undefined: "+str(undef_total)+"\n"


	print "\n"
    print "################################################################################"
    print "# Island Footprint                                                             #"
    print "################################################################################"
    total=0
    all_sizes = size_arch
#    for obj in sorted(size_obj, key=size_obj.get, reverse=True):
#        if os.path.isfile(obj):
#            all_sizes[obj] = size_obj[obj]
    for file in sorted(all_sizes, key=all_sizes.get, reverse=True):
        total+=all_sizes[file]
        print "\n\n"
        print "%s: %.2f KB" % (file, float(all_sizes[file])/1024)
        csv_list.append("%s: %.2f KB \n" % (file, float(all_sizes[file])/1024))
        print "%   -56.56s: %.2f KB" % (str("(.data* .sdata*)")[-56:], float(size_data[file])/1024)
        print "%   -56.56s: %.2f KB" % (str("(.text*)")[-56:], float(size_text[file])/1024)
        print "%   -56.56s: %.2f KB" % (str("(.bss* .sbss* )")[-56:], float( size_bss[file])/1024)
        print "%   -56.56s: %.2f KB" % (str("(COMMON .scommon*)")[-56:], float( size_common[file])/1024)
        print "%   -56.56s: %.2f KB" % (str("(.rodata*)")[-56:], float(size_rodata[file])/1024)
        print "%   -56.56s: %.2f KB" % (str("(other sections)")[-56:], float(size_other[file])/1024)

    print "\nTotal: %.2f KB\n" % (float(total)/1024)

    symbol_list.sort(key=lambda x: x[2]) #sort by libnames
    if elffile:
        if exestr == "":
            if os.name == 'posix':
                default_tools_root = os.path.join('/pkg/qct/software/hexagon/releases/tools',default_tools_version)
            elif os.name == 'nt':
                default_tools_root = os.path.join(os.path.realpath('C:/Qualcomm/HEXAGON_Tools'),default_tools_version)
            exestr = os.path.join(default_tools_root,"gnu/bin/hexagon-readelf")
        else:
            exestr = os.path.realpath(exestr)
        exestr = exestr.split('hexagon-')
        exestr = os.path.join(exestr[0],'hexagon-readelf')
        generate_discrepancy_report(elffile, csvfile, exestr)

#    We could simply have returned sys.exit(undef_total). This works on windows machine. But on linux machines it doesn't give the actual value. It seems to be
#   passing some garbage value. So get around this, we either return 0 or 1 for windows machine and 0 or 256 for linux machines. 
    if(undef_total > 0):
        sys.exit(1)
    else:
        sys.exit(0)
