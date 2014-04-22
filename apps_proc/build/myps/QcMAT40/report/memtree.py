"""
 memtree.py

Description: Memory Tree data structure used for command line report.

 Copyright (c) 2015 - 2016 by QUALCOMM, Incorporated.  
             All Rights Reserved.

========================================================================

 EDIT HISTORY FOR MODULE

 $Header: //components/rel/build.tn/4.0/myps/QcMAT40/report/memtree.py#1 $
 

 when            who        what, where, why
 --------      ---------  ------------------------------------------------
 Mar 28, 2016   rmandal   qcmatreport - initial version for command-line csv,html report
    
 ==========================================================================
 """
 
class MemTreeNode:
    def __init__(self,name):
        self.name = name
        self.childs = list()
        # TEXT, RO, DATA, BSS, TOTAL, FatalCount, F3Count
        self.leftData  = [0,0,0,0,0,0,0]
        self.rightData = [0,0,0,0,0,0,0]
        self.diffData  = [0,0,0,0,0,0,0]
        self.isLeaf = True
        self.hasLeft = False
        self.hasRight = False
        
    def addChild(self,child):
        self.childs.append(child)
        if len(self.childs) > 0:
            self.isLeaf = False
            
    def getChilds(self):
        return self.childs
        
    def setLeftSizeAt(self,index,size):
        self.leftData[index] = int(size)
        self.hasLeft = True
    
    def setRightSizeAt(self,index,size):
        self.rightData[index] = int(size)
        self.hasRight = True
            
    def getLeftSizeAt(self,index):
        return self.leftData[index]
    
    def getRightSizeAt(self,index):
        return self.rightData[index]
    
    def getDiffSizeAt(self,index):
        return self.diffData[index]
    
    def getLeftSizes(self):
        return self.leftData
    
    def getLeftSizesString(self):
        return "" + ",".join(repr(e) for e in ((self.leftData)))

    def getRightSizes(self):
        return self.rightData
    
    def getRightSizesString(self):
        return "" + ",".join(repr(e) for e in ((self.rightData)))
        
    def updateDiffSizes(self):
        if self.hasLeft and self.hasRight:
            for i in range(0,7):
                self.diffData[i] = self.rightData[i] - self.leftData[i]
        elif self.hasLeft:
            for i in range(0,7):
                self.diffData[i] = -self.leftData[i]
        elif self.hasRight:
            for i in range(0,7):
                self.diffData[i] = self.rightData[i]
    
    def getDiffSizes(self):
        return self.diffData
        
    def getDiffSizesString(self):
        return "" + ",".join(repr(e) for e in ((self.diffData)))
    
    def getFullName(self):
        return self.name
    
    def getLastName(self):
        return self.name.split('/')[-1]
    
    def getLastTwoName(self):
        return self.name.split('/')[-2] + '/' + self.name.split('/')[-1]
