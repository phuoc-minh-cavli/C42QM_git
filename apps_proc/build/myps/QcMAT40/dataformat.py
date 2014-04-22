"""
 dataformat.py

Description: data structure to store the symbol related information.
            information stored:- address , symbol name ,size , section name
            section type , file name.
 Copyright (c) 2016 - 2017 by QUALCOMM, Incorporated.  
             All Rights Reserved.

========================================================================

 EDIT HISTORY FOR MODULE

 $Header: dataformat.py

   when            who        what, where, why
  --------       ---------  ------------------------------------------------
 7 July 2016                  First Draft
    
 ==========================================================================
 """

class object_info:
	#constructor
	def __init__(self, section_name, symbol_section, symbol_name, address, size, path , section_index):
		self.section_name=section_name
		self.symbol_section=symbol_section
		self.symbol_name=symbol_name
		self.address=address
		self.size=size
		self.path=path
		self.section_index=section_index

		#To display the data of object associated as key
	def display(self):
		return str(self.path+" "+self.address[2:]+" "+self.symbol_name+" "+str(self.size)+" "+self.section_name+" g O "+str(self.section_index)+"\n")

	#To modify the symbol name of previously occured symbol
	def set_symbol_name(self,value):
		self.symbol_name=value

	#To set the size of symbol , size is calculated by subtraction from the coming symbol @ using back processing
	def set_size(self,value):
		self.size=value

	def get_section_index(self):
		return self.section_index

	##to get the section_name
	def get_section_name(self):
		return self.section_name

	##to get the symbol_section
	def get_symbol_section(self):
		return self.symbol_section

	##to get the symbol_name
	def get_symbol_name(self):
		return self.symbol_name

	##to get the address
	def get_address(self):
		return self.address

	##to get the size
	def get_size(self):
		return self.size

	##to get the symbol path
	def get_path(self):
		return self.path