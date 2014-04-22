"""
 memtree.py

Description: Data structure of node to store symbol related information
             Related functions to set and get the node data

 Copyright (c) 2016 - 2017 by QUALCOMM, Incorporated.  
             All Rights Reserved.

========================================================================

 EDIT HISTORY FOR MODULE

 $Header: memtree.py

 when            who        what, where, why
 --------      ---------  ------------------------------------------------
 July 7, 2016                 First Draft
 ==========================================================================
 """

class MemTreeNode:

	def __init__(self,name):
		self.name = name
		self.childs = dict()
		## Feilds to Show up the Data
		# Feilds are :- # TEXT, RO, DATA, BSS, TOTAL, FatalCount, F3Count
		self.NodeData  = [0,0,0,0,0,0,0,0]
		self.isLeaf = True
		##dictionary to store the overlaid section
		self.overlaid_info=dict()
		self.overlaid_max_value=int(0)
	
	def get_overlaid_dict(self):
		return self.overlaid_info

	def getNodeName(self):
		return str(self.name)

	def addChild(self,child):
		self.childs.append(child)
		if len(self.childs) > 0:
			self.isLeaf = False
	
	def getChilds(self):
		return self.childs

	def getNodeData(self,type):
		if type=="TEXT":
			index=0
		elif type=="RO":
			index=1
		elif type=="DATA":
			index=2
		elif type=="BSS":
			index=3
		elif type=="TOTAL":
			index=4
		elif type=="FatalCount":
			index=5
		elif type=="F3Count":
			index=6
		else:
			index=7
		return self.NodeData[index]


	def setNodeData(self,type,value):
		if type=="TEXT":
			index=0
		elif type=="RO":
			index=1
		elif type=="DATA":
			index=2
		elif type=="BSS":
			index=3
		elif type=="TOTAL":
			index=4
		elif type=="FatalCount":
			index=5
		elif type=="F3Count":
			index=6
		else:
			index=7
		self.NodeData[index]=value


	def display(self):
		for i in range(8):
			print self.NodeData[i], "     ",
		print "\n"