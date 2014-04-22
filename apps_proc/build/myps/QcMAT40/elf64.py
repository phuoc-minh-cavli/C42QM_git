"""
 elf.py

Description: This File parse the elf file and map address to section type.
             Functionality taken from ELF Manipulator.

 Copyright (c) 2016 - 2017 by QUALCOMM, Incorporated.  
             All Rights Reserved.

========================================================================

 EDIT HISTORY FOR MODULE

 $Header: elf.py

 when            who        what, where, why
 --------      ---------  ------------------------------------------------
 July 7, 2016                 First Draft
 ==========================================================================
 """


import time
import sys
import struct


class elfDataTypes:
	## Unsigned program address
	Elf32_Addr = 8
	## Unsigned medium integer
	Elf32_Half = 2
	## Unsigned file offset
	Elf32_Off = 8
	## Signed large integer
	Elf32_Sword = 4
	## Unsigned large integer
	Elf32_Word = 4
	## Unsigned small integer
	unsigned_char = 1
	## EI_NIDENT imported from other data type just to make it in same class 
	EI_NIDENT = 16
	## EI_PAD imported from other data type just to make it in same class 
	## Start of padding bytes
	EI_PAD = 9
	## EI_PAD imported from other data type just to make it in same class 
	## Invalid data encoding
	ELFDATANONE     = 0
	## EI_PAD imported from other data type just to make it in same class 
	## Little Endian
	ELFDATA2LSB     = 1
	## EI_PAD imported from other data type just to make it in same class 
	## Big Endian
	ELFDATA2MSB     = 2

EI_DATA = elfDataTypes.ELFDATA2LSB


class elfLibError(Exception):
	def __init__ (self, expr):
		self.expr = expr


class sectionTypes:
	## Inactive
	SHT_NULL			= 0
	## Information defined by the program
	SHT_PROGBITS		= 1
	## Symbol table
	SHT_SYMTAB		  = 2
	## String table
	SHT_STRTAB		  = 3
	## Relocation entries with explicit addends
	SHT_RELA			= 4
	## Symbol hash table
	SHT_HASH			= 5
	## Information for dynamic linking
	SHT_DYNAMIC		 = 6
	## "Special" marks for the file
	SHT_NOTE			= 7
	## No space, but otherwise SHT_PROGBITS
	SHT_NOBITS		  = 8
	## Relocation entries without explicit addends
	SHT_REL			 = 9
	## Reserved, unspecified semantics
	SHT_SHLIB		   = 10
	## Symbol table
	SHT_DYNSYM		  = 11
	## Array of pointers to init functions
	SHT_INIT_ARRAY	  = 14
	## Array of pointers to termination functions
	SHT_FINI_ARRAY	  = 15
	## Array of pointers to preinit functions
	SHT_PREINIT_ARRAY   = 16
	## Section group; related sections
	SHT_GROUP		   = 17
	## Required if any of the section header indexes referenced by SHT_SYMTAB
	#  contain the escape value SHN_XINDEX
	SHT_SYMTAB_SHNDX	= 18
	## Lower bound for reserved OS-specific semantics
	SHT_LOOS			= 0x60000000
	## Upper bound for reserved OS-specific semantics
	SHT_HIOS			= 0x6fffffff
	## Lower bound for reserved processor-specific semantics
	SHT_LOPROC		  = 0x70000000
	## Upper bound for reserved processor-specific semantics
	SHT_HIPROC		  = 0x7fffffff
	## Lower bound of reserved indexes for application programs
	SHT_LOUSER		  = 0x80000000
	## Upper bound of reserved indexes for application programs
	SHT_HIUSER		  = 0xffffffff

def getDataFromELF(elf, offset, size):
    try :
        elfFileHandle = open(elf, 'rb')
    except IOError:
        raiseElfLibWarning("Failed to open " + elf)
    else:
        elfFileHandle.seek(offset)
        data = elfFileHandle.read(size)
        elfFileHandle.close()
    return data

# @brief    Function used to raise an error
# @param    errorMessage  Error message to print
def raiseElfLibError(errorMessage):
	try:
		raise elfLibError(errorMessage)
	except elfLibError as e:
		print "Exception raised during ELF manipulation: " + e.expr
		raise
		
def raiseElfLibWarning(warningMessage):
	try:
		sys.stderr.write("%s\n" % warningMessage)
	except elfLibError as e:
		print "Warning raised during ELF manipulation: " + e.expr
		raise

def convertFromELF(buffer, amount, offset=0, encoding=0):
	if encoding == 0:
		# Set the endianness according to global EI_DATA variable
		if EI_DATA == elfDataTypes.ELFDATA2LSB:
			endianness = "<"
		elif EI_DATA == elfDataTypes.ELFDATA2MSB:
			endianness = ">"
		else:
			raiseElfLibWarning("Invalid data encoding specified")
	else:
		endianness = encoding
	# Set the format according to the amount
	if (amount == elfDataTypes.Elf32_Addr or amount==elfDataTypes.Elf32_Word): #Elf32_Off, Elf32_Sword, Elf32_Word
		format = "L"
	elif (amount == elfDataTypes.Elf32_Half):
		format = "H"
	elif (amount == elfDataTypes.unsigned_char):
		format = "B"
	else:
		raiseElfLibWarning("Unexpected amount specified for convertFromELF")
	# Unpack buffer with specified endianness and format at given offset
	return struct.unpack_from(endianness + format, buffer, offset)[0]


class elfStruct(object):
    """description of class"""
    ##
    # @brief    Special member function that is automatically invoked for a new
    #           instance. Initializes and verifies data struct.
    # @param    elf     A full path to the input ELF file.
    # @param    offset  The offset to start reading the ELF file from.
    # @param    size    The amount to read in from the ELF file.
    # @param    idx     The index of the structure (default '0')
    # @param    verbose Enabled verbose printing for debugging (default '0')
    def __init__ (self, elf, offset, size, idx=0, verbose=0):
        ## Input ELF file
        self.elf = elf
        ## Section or program index
        self.idx = idx
        ## Verbosity levels
        self.verbose = False
        ## Offset that will be used internally
        self.structOffset = 0
        ## Structure containing the data
        self.struct = getDataFromELF(self.elf, offset, size)
        self.setStructMemberSizes()
        self.setAndValidateStructMembers()

    ##
    # @brief    Member function to set struct member sizes
    # @pre      Subclass implementation
    def setStructMemberSizes(self):
        raiseElfLibError("Subclass must implement elfStruct::setStructMemberSizes")

    ##
    # @brief    Member function to get struct member size
    # @param    member  Member to get the size of
    # @pre      Struct member sizes have been set using setStructMemberSizes
    def getStructMemberSize(self, member):
        # print "member",member
        if member in self.halfMembers:
            size = elfDataTypes.Elf32_Half
        elif member in self.longMembers:
            size = elfDataTypes.Elf32_Addr # == Off == Sword == Word
        elif member in self.byteMembers:
            size = elfDataTypes.unsigned_char
        elif member in self.longMembers_word:
           size = elfDataTypes.Elf32_Word
        else:
            raiseElfLibError(self.__class__.__name__ + "." +
                                    member + " does not exist.")
        return size

    ##
    # @brief    Member function to get struct member from ELF
    # @param    member  Member to get the value for
    def getStructMember(self, member):
        size = self.getStructMemberSize(member)
        data = convertFromELF(    self.struct,
                                        size,
                                        self.structOffset)
        self.structOffset += size
        # Print member and value if verbose is enabled
        if (self.verbose):
            print (self.__class__.__name__ + "." + member).ljust(35) + " = " + str(data)
        return data

    ##
    # @brief    Member function to extract and validate data struct
    # @pre      Subclass implementation
    def setAndValidateStructMembers(self):
        raiseElfLibError("Subclass must implement elfStruct::setAndValidateStructMembers")


class Elf32_Ehdr(elfStruct):
    """
    #define EI_NIDENT   16
    typedef struct {
        unsigned_char   e_ident[EINIDENT];
        Elf32_Half      e_type;
        Elf32_Half      e_machine;
        Elf32_Word      e_version;
        Elf32_Addr      e_entry;
        Elf32_Off       e_phoff;
        Elf32_Off       e_shoff;
        Elf32_Word      e_flags;
        Elf32_Half      e_ehsize;
        Elf32_Half      e_phentsize;
        Elf32_Half      e_phnum;
        Elf32_Half      e_shentsize;
        Elf32_Half      e_shnum;
        Elf32_Half      e_shstrndx;
    } Elf32_Ehdr;
    """    

    ## Member function to set struct member sizes
    def setStructMemberSizes(self):
        ## List containing all members of type Elf32_Half
        self.halfMembers = [
                            "e_type",
                            "e_machine",
                            "e_ehsize",
                            "e_phentsize",
                            "e_phnum",
                            "e_shentsize",
                            "e_shnum",
                            "e_shstrndx"
                            ]
        ## List containing all members of type Elf32_(Addr|Off|Sword|Word)
        self.longMembers = [
                            "e_entry",
                            "e_phoff",
                            "e_shoff"
                            ]
        ## List containing all members of type unsigned_char
        self.byteMembers = [
                            "e_ident[EI_MAG0]",
                            "e_ident[EI_MAG1]",
                            "e_ident[EI_MAG2]",
                            "e_ident[EI_MAG3]",
                            "e_ident[EI_CLASS]",
                            "e_ident[EI_DATA]",
                            "e_ident[EI_VERSION]",
                            "e_ident[EI_OSABI]",
                            "e_ident[EI_ABIVERSION]",
                            "e_ident[EI_PAD]"
                            ]
        self.longMembers_word = [
                            "e_flags",
                            "e_version",
                            ]
    ## Member function to extract and validate data struct
    def setAndValidateStructMembers(self):
        if (self.verbose):
            print "============================================================"
            print " ** VERBOSE DEBUG MSG ** ELF HEADER (Elf32_Ehdr)"
            print "============================================================"

        ##  The initial bytes mark the file as an object file and provide
        #   machine-independent data with which to decode and interpret
        #   the file's contents.
        self.e_ident = []

        # Magic number identifying the file as an ELF object file
        self.e_ident.append(self.getStructMember("e_ident[EI_MAG0]"))
        self.e_ident.append(self.getStructMember("e_ident[EI_MAG1]"))
        self.e_ident.append(self.getStructMember("e_ident[EI_MAG2]"))
        self.e_ident.append(self.getStructMember("e_ident[EI_MAG3]"))
        self.e_ident.append(self.getStructMember("e_ident[EI_CLASS]"))
        # Data encoding should be LITTLE-ENDIAN
        # Example:
        #            ---- ----
        # 0x0102    | 02 | 01 |
        #            ---- ----
        self.e_ident.append(self.getStructMember("e_ident[EI_DATA]"))

        # Version should be current
        self.e_ident.append(self.getStructMember("e_ident[EI_VERSION]"))
        # OSABI should be UNIX - System V
        self.e_ident.append(self.getStructMember("e_ident[EI_OSABI]"))
        # ABI version should be 0
        self.e_ident.append(self.getStructMember("e_ident[EI_ABIVERSION]"))
        # Remaining bytes of the ELF identifier are don't cares (zeros)
        self.structOffset += elfDataTypes.EI_NIDENT - elfDataTypes.EI_PAD

        ##  This member identifies the object file type. Should be executable.
        self.e_type = self.getStructMember("e_type")
        ##  This member's value specifies the required architecture for an
        #   individual file. Should be Qualcomm Hexagon.
        self.e_machine = self.getStructMember("e_machine")
        #print 'self.e_machine:', self.e_machine
        ##  This member identifies the object file version.
        self.e_version = self.getStructMember("e_version")

        ##  This member gives the virtual address to which the system first
        #   transfers control, thus starting the process. If the file has no
        #   associated entry point, this member holds zero.
        self.e_entry = self.getStructMember("e_entry")

        ##  This member holds the program header table's file offset in bytes.
        #   If the file has no program header table, this member holds zero.
        self.e_phoff = self.getStructMember("e_phoff")

        ##  This member holds the section header table's file offset in bytes.
        #   If the file has no section header table, this member holds zero.
        self.e_shoff = self.getStructMember("e_shoff")

        ##  This member holds processor-specific flags associated with the file.
        #   Flag names take the form EF_machine_flag. Flags should indicate that
        #   both the version of the Hexagon processor that the object file was
        #   created for AND the highest version of the Hexagon processor that
        #   the object file contains for is V5
        self.e_flags = self.getStructMember("e_flags")

        ##  This member holds the ELF header's size in bytes.
        self.e_ehsize = self.getStructMember("e_ehsize")

        ##  This member holds the size in bytes of one entry in the file's
        #   program header table; all entries are the same size.
        self.e_phentsize = self.getStructMember("e_phentsize")

        ##  This member holds the number of entries in the program header table.
        #   Thus the product of e_phentsize and e_phnum gives the table's size
        #   in bytes. If a file has no program header table, e_phnum holds the
        #   value zero.
        self.e_phnum = self.getStructMember("e_phnum")

        ##  This member holds a section header's size in bytes. A section header
        #   is one entry in the section header table; all entries are the same
        #   size.
        self.e_shentsize = self.getStructMember("e_shentsize")

        ##  This member holds the number of entries in the section header table.
        #   Thus the product of e_shentsize and e_shnum gives the section header
        #   table's size in bytes. If a file has no section header table,
        #   e_shnum holds the value zero.
        self.e_shnum = self.getStructMember("e_shnum")

        ##  This member holds the section header table index of the entry
        #   associated with the section name string table. If the file has no
        #   section name string table, this member holds the value SHN_UNDEF.
        self.e_shstrndx = self.getStructMember("e_shstrndx")


class Elf32_Shdr(elfStruct):
    """
    typedef struct {
        Elf32_Word      sh_name;
        Elf32_Word      sh_type;
        Elf32_Word      sh_flags;
        Elf32_Addr      sh_addr;
        Elf32_Off       sh_offset;
        Elf32_Word      sh_size;
        Elf32_Word      sh_link;
        Elf32_Word      sh_info;
        Elf32_Word      sh_addralign;
        Elf32_Word      sh_entsize;
    } Elf32_Shdr;
    """

    ## Member function to set struct member sizes
    def setStructMemberSizes(self):
        ## List containing all members of type Elf32_Half
        self.halfMembers = []
        ## List containing all members of type Elf32_(Addr|Off|Sword|Word)
        self.longMembers = [
                            "sh_flags",
                            "sh_addr",
                            "sh_size",
                            "sh_offset",
                            "sh_addralign",
                            "sh_entsize"
                            ]
        ## List containing all members of type unsigned_char
        self.byteMembers = []
        self.longMembers_word=[
                               "sh_link",
                               "sh_info",
                               "sh_type",
                               "sh_name"
                              ]

    ## Member function to extract and validate data struct
    def setAndValidateStructMembers(self):
        if (self.verbose):
            print "============================================================"
            print " ** VERBOSE DEBUG MSG ** SECTION HEADER (Elf32_Shdr)"
            print "============================================================"

        ##  This member specifies the name of the section. Its value is an index
        #   into the section header string table section, giving the location of
        #   a null terminated string.
        self.sh_name = self.getStructMember("sh_name")

        ##  This member categorizes the section's contents and semantics.
        self.sh_type = self.getStructMember("sh_type")
        if (self.sh_type not in [sectionTypes.SHT_NULL,
                                 sectionTypes.SHT_PROGBITS,
                                 sectionTypes.SHT_NOBITS,
                                 sectionTypes.SHT_STRTAB,
                                 sectionTypes.SHT_SYMTAB
                                ]):
            #utils.raiseElfLibError("Elf32_Shdr.sh_type failed validation.")
            if self.verbose:
              print "Skip Elf32_Shdr.sh_type validation for dynamic loading"

        ##  Sections support 1-bit flags that describe miscellaneous attributes.
        self.sh_flags = self.getStructMember("sh_flags")

        ##  If the section will appear in the memory image of a process, this
        #   member gives the address at which the section's first byte should
        #   reside. Otherwise, the member contains 0.
        self.sh_addr = self.getStructMember("sh_addr")

        ##  This member's value gives the byte offset from the beginning of the
        #   file to the first byte in the section. One section type, SHT_NOBITS,
        #   occupies no space in the file, and its sh_offset member locates the
        #   conceptual placement in the file.
        self.sh_offset = self.getStructMember("sh_offset")

        ##  This member gives the section's size in bytes. Unless the section
        #   type is SHT_NOBITS, the section occupies sh_size bytes in the file.
        #   A section of type SHT_NOBITS may have a non-zero size, but it
        #   occupies no space in the file.
        self.sh_size = self.getStructMember("sh_size")

        ##  This member holds a section header table index link, whose
        #   interpretation depends on the section type.
        self.sh_link = self.getStructMember("sh_link")
        # Print out detailed interpretation
        if (self.verbose):
            if (self.sh_type == sectionTypes.SHT_DYNAMIC):
                print "\tThe section header index of the string table used by entries in the section"
            elif (self.sh_type == sectionTypes.SHT_HASH):
                print "\tThe section header index of the symbol table to which the hash table applies"
            elif (  self.sh_type == sectionTypes.SHT_REL or
                    self.sh_type == sectionTypes.SHT_RELA):
                print "\tThe section header index of the associated symbol table"
            elif (  self.sh_type == sectionTypes.SHT_SYMTAB or
                    self.sh_type == sectionTypes.SHT_SYMTAB):
                print "\tThe section header index of the associated string table"
            else:
                print "\tSHN_UNDEF"

        ##  This member holds extra information, whose interpretation depends on
        #   the section type.
        self.sh_info = self.getStructMember("sh_info")
        # Print out detailed interpretation
        if (self.verbose):
            if (self.sh_type == sectionTypes.SHT_DYNAMIC):
                print "\t0"
            elif (self.sh_type == sectionTypes.SHT_HASH):
                print "\t0"
            elif (  self.sh_type == sectionTypes.SHT_REL or
                    self.sh_type == sectionTypes.SHT_RELA):
                print "\tThe section header index of the section to which the relocation applies"
            elif (  self.sh_type == sectionTypes.SHT_SYMTAB or
                    self.sh_type == sectionTypes.SHT_DYNSYM):
                print "\tOne greater than the symbol table index of the last local symbol (binding STB_LOCAL)"
            else:
                print "\t0"

        ##  Some sections have address alignment constraints. For example, if a
        #   section holds a doubleword, the system must ensure doubleword
        #   alignment for the entire section. That is, the value of sh_addr must
        #   be congruent to 0, modulo the value of sh_addralign. Currently, only
        #   0 and positive integral powers of two are allowed. Values 0 and 1
        #   mean the section has no alignment constraints.
        self.sh_addralign = self.getStructMember("sh_addralign")
        ##  Some sections hold a table of fixed-size entries, such as a symbol
        #   table. For such a section, this member gives the size in bytes of
        #   each entry. The member contains 0 if the section does not hold a
        #   table of fixed-size entries.
        self.sh_entsize = self.getStructMember("sh_entsize")

        ##  This member contains the contents of the section.
        #if self.sh_type != sectionTypes.SHT_NOBITS:
        #if (self.sh_type in [sectionTypes.SHT_STRTAB, sectionTypes.SHT_SYMTAB]):
         #   self.contents = utils.getDataFromELF(   self.elf,
          #                                          self.sh_offset,
           #                                         self.sh_size)


    def getSectionAttributeType(self, sectionType, sectionFlag):
        if sectionTypes.SHT_NOBITS == sectionType:
            return "ELF_SECTION_BSS"
        if "EXEC" in sectionFlag:
            return "ELF_SECTION_TEXT"
        elif "WRITE" in sectionFlag:
            return "ELF_SECTION_DATA"
        elif "STRINGS" in sectionFlag or "ALLOC" in sectionFlag:
            return "ELF_SECTION_RO"

    ##
    # @brief    Member function to get section attribute based on flags
    # @param    attribute   Attribute to get
    # @returns  Non-zero if the attribute is enabled. Zero otherwise.
    def getSectionAttribute(self, attribute):

        #flag = attribute & - attribute
        #attribute &= ~flag

        #NOTE: Some of the following are unlikely to exist but just simply providing for completeness.
        switcher = {
            1: "WRITE",
            2: "ALLOC",
            3: "WRITE, ALLOC",
            4: "EXEC",
            5: "WRITE, EXEC",
            6: "ALLOC, EXEC",
            7: "WRITE, ALLOC, EXEC",
            20: "EXEC, MERGE",
            22: "ALLOC, EXEC, MERGE",
            23: "WRITE, ALLOC, EXEC, MERGE",
            32: "STRINGS",
            33: "WRITE, STRINGS",
            34: "ALLOC, STRINGS",
            35: "WRITE, ALLOC, STRINGS",
            36: "EXEC, STRINGS",
            37: "WRITE, EXEC, STRINGS",
            38: "ALLOC, EXEC, STRINGS",
            39: "WRITE, ALLOC, EXEC, STRINGS",
            48: "MERGE, STRINGS",
            49: "WRITE, MERGE, STRINGS",
            50: "ALLOC, MERGE, STRINGS",
            64: "INFOLINK",
            128: "LINK ORDER",
            256: "NON-CONFORMING",
            512: "GROUP",
            1024: "TLS"
            """ Many more configurations can come here....for now using above....if required, we can add later...."""
            }

        if int(attribute) in switcher:
            return switcher.get(int(attribute))
        else:
            return "UNKNOWN"


class elfFile:

	def __init__ (self, elf,debug_enable):
		#x=time.time()
		self.elf = elf
		self.section_overlap=dict()
		self.addr_to_sectype=dict()
		self.elfHeader = self.__getElfHeaderFromElf()
		self.sectionHeaderTable = self.__getSectionHeaderTable()
		#print "total time taken:  " , time.time()-x
		

	def __getElfHeaderFromElf(self):
		size = (elfDataTypes.EI_NIDENT* elfDataTypes.unsigned_char
				+ 8 * elfDataTypes.Elf32_Half
				+ 2 * elfDataTypes.Elf32_Word
				+ 1 * elfDataTypes.Elf32_Addr
				+ 2 * elfDataTypes.Elf32_Off
				)
		# print size
		return Elf32_Ehdr(self.elf, 0, size)


	def __getSectionHeaderTable(self):
        # Retrieve required information from ELF header and iterate through
        # each section header
		offset = self.elfHeader.e_shoff
		size = self.elfHeader.e_shentsize
		sectionHeaderTable = []
		for sh in range(self.elfHeader.e_shnum):
			sectionHeaderTable.append(Elf32_Shdr(self.elf,offset,size,sh))
			offset += size
		for i in range(self.elfHeader.e_shnum):
			key=hex(sectionHeaderTable[i].sh_addr).strip("L")
			type=str(sectionHeaderTable[i].getSectionAttributeType(sectionHeaderTable[i].sh_type,sectionHeaderTable[i].getSectionAttribute(sectionHeaderTable[i].sh_flags & 0x3FF)))
			name        =str(sectionHeaderTable[i].sh_name)
			flags       =str(sectionHeaderTable[i].sh_flags)
			addr        =str(hex(sectionHeaderTable[i].sh_addr).strip("L"))
			offset      =str(sectionHeaderTable[i].sh_offset)
			size        =str(sectionHeaderTable[i].sh_size)
			link        =str(sectionHeaderTable[i].sh_link)
			info        =str(sectionHeaderTable[i].sh_info)
			addralign   =str(sectionHeaderTable[i].sh_addralign)
			entsize     =str(sectionHeaderTable[i].sh_entsize)
			if key in self.addr_to_sectype:
				self.addr_to_sectype[key][-1]="True"
				key=key+"___"+str(size)
				self.addr_to_sectype[key]=[name,flags,addr,size,entsize,type,link,info,addralign,"True"]
			else:
				self.addr_to_sectype[key]=[name,flags,addr,size,entsize,type,link,info,addralign,"False"]
		# Return the section header table as an array
		# for i in self.addr_to_sectype:
			# print "jsumit:- ",i , self.addr_to_sectype[i]
		return sectionHeaderTable

	def get_addr_to_sectype_dict(self):
		return self.addr_to_sectype
		
	def get_section_overlapp_dict(self):
		return self.section_overlap