"""
 reportgenerator.py

Description: creates HTML report using the symbols information
             after post prcessing , output data is a tree 
             uses memtree for storing info in tree
 Copyright (c) 2016 - 2017 by QUALCOMM, Incorporated.  
             All Rights Reserved.

========================================================================

 EDIT HISTORY FOR MODULE

 $Header: reportgenerator.py

   when            who        what, where, why
  --------       ---------  ------------------------------------------------
 7 July 2016                  First Draft
    
 ==========================================================================
 """


import memtree
import dataformat
import htmlheader
import os

class reportgenerator:
	
	def __init__(self,addr_to_dataformat):
		self.data_dict=addr_to_dataformat
		##Create a foder Name as "ROOT" it is the super folder under which all nodes are added
		self.root=memtree.MemTreeNode("ROOT")
		self.creatememtree(self.data_dict)
		#self.display_tree(self.root)
		self.Generate_MemTree_HTML(self.root,"ROOT")

	def creatememtree(self,data_dict):
		##parse the dictionary generated in after the map and elf processing
		for address in data_dict:
			data_obj=data_dict[address]
			size=data_obj.get_size()
			symbol_name=data_obj.get_symbol_name()
			path=data_obj.get_path()+"/"+symbol_name
			section_type = str(data_obj.get_section_type()+"")
			overlaid_type=data_obj.f_isSectionOverlaid()
			if overlaid_type==0:
				self.add_node_to_tree(section_type,size,path)
				self.add_node_to_tree("TOTAL",size,path)
			else:
				#print "calling overlaid"
				self.add_node_to_tree_overlaid(section_type,size,path,overlaid_type)
	
	##function in case of section is overlapped
	def add_node_to_tree_overlaid(self,section_type,size,path,overlaid_type):
		folder_names=path.split("/")
		current_obj=self.root
		if overlaid_type in current_obj.get_overlaid_dict():
			if int(current_obj.get_overlaid_dict()[overlaid_type]) < int(size.strip("L"),16):
				previous_size=current_obj.get_overlaid_dict()[overlaid_type]
				current_obj.setNodeData(section_type,current_obj.getNodeData(section_type)-int(previous_size)+int(size.strip("L"),16))
				current_obj.setNodeData("TOTAL",current_obj.getNodeData("TOTAL")-int(previous_size)+int(size.strip("L"),16))
				current_obj.overlaid_info[overlaid_type]=int(size.strip("L"),16)
		else:
			current_obj.overlaid_info[overlaid_type]=int(size.strip("L"),16)
			current_obj.setNodeData(section_type,current_obj.getNodeData(section_type)+int(size.strip("L"),16))
			current_obj.setNodeData("TOTAL",current_obj.getNodeData("TOTAL")+int(size.strip("L"),16))
		#print current_obj.display()
		for names in folder_names:
			if names in current_obj.childs:
				if overlaid_type in current_obj.childs[names].get_overlaid_dict():
					if int(current_obj.childs[names].overlaid_info[overlaid_type]) < int(size.strip("L"),16):
						#print int(current_obj.childs[names].overlaid_info[overlaid_type]) , "<" , int(size.strip("L"),16)
						previous_size=current_obj.childs[names].overlaid_info[overlaid_type]
						# print "previous_size  ", previous_size
						# print "Node Name :- ",names ,"			",current_obj.childs[names].display()
						current_obj.childs[names].setNodeData(section_type,current_obj.childs[names].getNodeData(section_type)-int(previous_size)+int(size.strip("L"),16))
						current_obj.childs[names].setNodeData("TOTAL",current_obj.childs[names].getNodeData("TOTAL")-int(previous_size)+int(size.strip("L"),16))
						current_obj.childs[names].overlaid_info[overlaid_type]=int(size.strip("L"),16)
						# print "in if block:- " , current_obj.childs[names].display()
				else:
					current_obj.childs[names].get_overlaid_dict()[overlaid_type]=int(size.strip("L"),16)
					current_obj.childs[names].setNodeData(section_type,current_obj.childs[names].getNodeData(section_type)+int(size.strip("L"),16))
					current_obj.childs[names].setNodeData("TOTAL",current_obj.childs[names].getNodeData("TOTAL")+int(size.strip("L"),16))
				current_obj=current_obj.childs[names]
			else:
				# print "dict entry not found"
				current_obj.isLeaf=False
				newobj=memtree.MemTreeNode(names)
				newobj.setNodeData(section_type,int(size.strip("L"),16))
				newobj.setNodeData("TOTAL",int(size.strip("L"),16))
				newobj.get_overlaid_dict()[overlaid_type]=int(size.strip("L"),16)
				current_obj.childs[names]=newobj
				# print "Node Name :- ",names ,"			",current_obj.childs[names].display()
				current_obj=newobj
	##Function to add node to tree , without overlapped symbols
	def add_node_to_tree(self,section_type,size,path):
		folder_names=path.split("/")
		#print folder_names
		current_obj=self.root
		current_obj.setNodeData(section_type,current_obj.getNodeData(section_type)+int(size.strip("L"),16))
		for names in folder_names:
			if names in current_obj.childs:
				current_obj.childs[names].setNodeData(section_type,current_obj.childs[names].getNodeData(section_type)+int(size.strip("L"),16))
				current_obj=current_obj.childs[names]
			else:
				current_obj.isLeaf=False
				newobj=memtree.MemTreeNode(names)
				newobj.setNodeData(section_type,int(size.strip("L"),16))
				current_obj.childs[names]=newobj
				current_obj=newobj
	
	##Function to generate the HTML Report of the results , parsing into memory Tree generated
	def Generate_MemTree_HTML(self,obj,path):
		#print path
		if obj.isLeaf==True:
			return
		if obj.name=="ROOT":
			htmlfilename="index.html"
		else:
			if path and not os.path.exists(path):  
				os.makedirs(path)
			path=path+"\\"+obj.name
			htmlfilename=path+".html"
		htmlfile = open(htmlfilename,"w")
		htmlfile.write(htmlheader.htmlcssheader)
		htmlfile.write(htmlheader.htmldirtable)
		for child in obj.childs:
			htmlfile.write("<tr>")
			linkdisp=obj.childs[child].name
			linkpath=str(path.split("\\")[-1])+"\\"+linkdisp+".html"
			#print "child name:--",linkdisp,"  key:-",child
			if obj.childs[child].isLeaf:
				htmldirtd = """<td>{name}</td>""".format(name = linkdisp )
			else:
				htmldirtd = """<td><a href="{link}">{name}</a></td>""".format(link = linkpath,name = linkdisp )
			htmlfile.write(htmldirtd)
			for i in range(0,8):
				htmlsizetd = """<td align="right">{size}</td>""".format(size = obj.childs[child].NodeData[i])
				htmlfile.write(htmlsizetd)
			htmlfile.write("</tr>\n")
			htmlfile.write("</tbody>\n<tfoot><tr>")
		
		for child in obj.childs:
			self.Generate_MemTree_HTML(obj.childs[child],path)

	##Display Function,not used in QCMAT.Used for debugging
	def display_tree(self,mem_obj):
		mem_obj.display()
		if mem_obj.isLeaf==True:
			return
		print "\n|"
		print "|"
		print "V"
		for i in mem_obj.childs:
			self.display_tree(mem_obj.childs[i])

def main():
	dicti=dict()
	str="0xc1453260				TEXT	.text_sw	text	_ZL11exc_cleanup19_Unwind_Reason_CodeP17_Unwind_Exception	0xc1453260	0x88L	hexagon/releases/tools/7.3.07/Tools/target/hexagon/lib/v61/libstdc++.a(xthrow.o)"
	str=str.split()
	obj=dataformat.object_info(str[1],str[2],str[3],str[4],str[5],str[6],str[7])
	dicti[str[0]]=obj
	reportgenerator(dicti)

if __name__ == "__main__":
	main()