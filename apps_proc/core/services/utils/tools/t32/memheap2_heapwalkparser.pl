################################################################################
#                          H E A P P A R S E R                                 #
#                                                                              #
#     This tool generates a report with all the information (useful)           #
# from the heapwalker output file. The CRC information handling has been       #
# disabled for the sake speed of execution .                                   #
#                                                                              #
# Heap Walker Output Processor                                                 #
# 1. Parses the heap walker output and computes                                #
#    a. Summary of heap state
#       o Number of free blocks                                                #
#       o Number of used blocks                                                #
#       o Total Free Space                
#       o Maximum possible Allocation Size
#       o Fragmentation Factor            
#       o Total Used Bytes                
#       o % of Allocated Space Wasted     
#    b. Blocks Summary
#       o Cumulative free blocks List                                          #
#       o Used Blocks List                                                     #
#    c. Free fragments list                                                    #
#    d. Allocations Summary for hash:lineno combo [Max. Allocated First]       #
#    e. Some notes on understanding the results                                #
#                                                                              #  
# Usage                                                                        #
# perl heapwalkparser.pl -i heapmem.txt -o heapsum.txt                         #
#                                                                              #
#   Copyright (c) 2006,2007 by QUALCOMM Incorporated. All Rights Reserved.     #
#                                                                              #
################################################################################

################################################################################
#                            EDIT HISTORY FOR FILE                             #
#   This section contains comments describing changes made to the module.      #
#   Notice that changes are listed in reverse chronological order.             #
#                                                                              #
# $Header: //components/rel/core.tx/6.0/services/utils/tools/t32/memheap2_heapwalkparser.pl#1 $
# $Author: pwbldsvc $
# $DateTime: 2020/01/30 22:49:35 $
#                                                                              #
# ver    when     who  what, where, why                                        #
# ----  -------   ---  --------------------------------------------------------#
#  v1   10/01/07  MK   Created this file                                       #
#                                                                              #
################################################################################

#some Constants
use constant FREEFLAG=>"0XFF";
use constant SIZEOFMEMBLK=>16;  # Equals sizeof(mem_block_header_type)
use constant VERSTRING=>"00.00.01";
use constant CMMSTRING=>"00.00.01";
#commandlinearguments
my $heapwalkerfile = "";
my $outputfile = "";



my %hashCostSummarytcb = ();
my %hashCostSummarycaller = ();
my %usedHeapSummary = ();
my %freeHeapSummary = ();
my %fragSummary     = ();
my $HeapMemBuffer;
my $HeapVariable;
my $CMMVersion;
my $TotalNoOfFragments = 0;
my $TotalNoOfBlocksUsed = 0;
my $TotalNoOfBlocksFree = 0;
my $TotalFreeMem = 0;
my $MaxFreeMemChunk = 0;
my $TotalWstdBytesinHeap = 0;
my $TotalAllocatedBytes = 0;

# Call the main subroutine here
main();

sub main {
   processCmdLine();
   open(OUTPUTFILE,">$outputfile") || die "Unable to create $outputfile\n";
   processHeapTable($heapwalkerfile);   
   processUsedFragmentsTcb();
   processUsedFragmentsCallerPtr();
   select(OUTPUTFILE);
   printSummary(\*OUTPUTFILE);
}

sub processCmdLine { 
   my $TotalArgSig = 0;
   my $i=0;

   for( $i=0; $i<$#ARGV+1; $i++)
   {

           my $arg = $ARGV[$i];
           if ($arg eq "-i" || $arg eq "-I")
           {
                   $i++;
                   $TotalArgSig = $TotalArgSig & 0x1 ;
                   $heapwalkerfile = $ARGV[$i];
                   if(!(-e $heapwalkerfile))
                   {
                      die "$heapwalkerfile does not exist!!!\n";
                   }
           }
           elsif ($arg eq "-o" || $arg eq "-O")
           {
                   $i++;
                   $TotalArgSig = $TotalArgSig & 0x2;
                   $outputfile = $ARGV[$i];
           }
           else
           {
                   print_usage();
                   exit 0;
           }
   }
}

sub processHeapTable {
   my $heapfilename = shift;
   my $block_count = 0;   
   my $temp_string = "";   
   my ($ptr,$caller_ptr, $caller_symb,$tcb_ptr,$tcb_symbol,$size);   


   open (HFH, "<$heapfilename") || die "Unable to open $heapfilename";
   <HFH>;
   <HFH>;
   while(<HFH>)
   {

#Block No       Address    DebugInfo  Allocated   Extra  Free  Last    
#                                       Bytes     Bytes  Flag  Flag  
#=============================================================================== 
# 
#   1    0x01F48A30      0x43f67582   1127       4304    0x0  0x00   

      #print "$_";	 
	  if(/(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)/)
	  {
		 #print "$_";
		 #print "\n3";
		 $ptr =$1;
		 $size = $2;
		 $caller_ptr = $3;
		 #it can be 4 or 5, depending on next one
		 my $param4=$4;
		 my $temp =hex($5);
		 $caller_symb = $5;
		 $tcb_ptr    = $6;  
		 $tcb_symbol = $7;
		 if($temp != 0)
		 {
			$tcb_symbol = $tcb_ptr;
			$tcb_ptr=$caller_symb;
			$caller_symb = $param4;		
		 }		 	          
		
		# Used Block  
		$TotalNoOfBlocksUsed++; # Starts with 1...
		$usedHeapSummary{$TotalNoOfBlocksUsed}{PTR} = $ptr; 
		$usedHeapSummary{$TotalNoOfBlocksUsed}{CALLER_PTR} = $caller_ptr;                   
		$usedHeapSummary{$TotalNoOfBlocksUsed}{CALLER_SYMBOL} = $caller_symb;           
		$usedHeapSummary{$TotalNoOfBlocksUsed}{TCB_PTR} = $tcb_ptr;      
		$usedHeapSummary{$TotalNoOfBlocksUsed}{TCB_SYMBOL} = $tcb_symbol;
		$usedHeapSummary{$TotalNoOfBlocksUsed}{ALLOCBYTES} = hex($size);
	  }      
	  elsif(/(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)/)
	  {
		 #print "$_";
		 #print "\n3";
		 $ptr =$1;
		 $size = $2;
		 $caller_ptr = $3;
		 $caller_symb = $4;
		 $tcb_ptr    = $5;  
		 $tcb_symbol = $6;
		          
		
			# Used Block  
			$TotalNoOfBlocksUsed++; # Starts with 1...
			$usedHeapSummary{$TotalNoOfBlocksUsed}{PTR} = $ptr; 
			$usedHeapSummary{$TotalNoOfBlocksUsed}{CALLER_PTR} = $caller_ptr;                   
			$usedHeapSummary{$TotalNoOfBlocksUsed}{CALLER_SYMBOL} = $caller_symb;           
			$usedHeapSummary{$TotalNoOfBlocksUsed}{TCB_PTR} = $tcb_ptr;      
			$usedHeapSummary{$TotalNoOfBlocksUsed}{TCB_SYMBOL} = $tcb_symbol;
			$usedHeapSummary{$TotalNoOfBlocksUsed}{ALLOCBYTES} = hex($size);
	  }      
	 
	
	   
	   
   }
   close(HFH);
 
}

sub printSummary {
   my $OUTPUTFILEHANDLE = shift;
   print $OUTPUTFILEHANDLE "             Heap Walker Results Version  ".VERSTRING."\n";
   print $OUTPUTFILEHANDLE "       =================================================\n\n";
   printOverallSummary($OUTPUTFILEHANDLE); 
   printCostSummaryTcb($OUTPUTFILEHANDLE);  
   printCostSummaryCaller($OUTPUTFILEHANDLE);
}

sub printOverallSummary {
   my $OUTPUTFILEHANDLE = shift;
   
print $OUTPUTFILEHANDLE "Overall Summary\n";
print $OUTPUTFILEHANDLE "===============\n";
print $OUTPUTFILEHANDLE " \n";
print $OUTPUTFILEHANDLE "    Total No. Of Used Blocks              : $TotalNoOfBlocksUsed\n";
print $OUTPUTFILEHANDLE "    Total Used Bytes                      : $TotalAllocatedBytes\n";
print $OUTPUTFILEHANDLE "\n\n";

}

sub processUsedFragmentsTcb {
   my $keyCost;
foreach  (sort keys %usedHeapSummary) {
      $keyCost = $usedHeapSummary{$_}{TCB_PTR}; 
      if(!defined($hashCostSummarytcb{$keyCost}))
         {
            # Currently we just show the allocated bytes and NOT the bytes 
            # requested... that would be alloc - 12( from struct) - EXtra bytes
            # for each fragment
            $hashCostSummarytcb{$keyCost}{ALLOCBYTES} = 0;
            $hashCostSummarytcb{$keyCost}{MAXALLOC} = 0;
            $hashCostSummarytcb{$keyCost}{MINALLOC} = 999999999;            
            $hashCostSummarytcb{$keyCost}{TOTALLOCS} = 0;
			$hashCostSummarytcb{$keyCost}{TCB_SYMBOL}="";
         }
      $hashCostSummarytcb{$keyCost}{ALLOCBYTES} += $usedHeapSummary{$_}{ALLOCBYTES};
      if($hashCostSummarytcb{$keyCost}{MAXALLOC} < $usedHeapSummary{$_}{ALLOCBYTES})
      {
         $hashCostSummarytcb{$keyCost}{MAXALLOC} = $usedHeapSummary{$_}{ALLOCBYTES};
      }
      if($hashCostSummarytcb{$keyCost}{MINALLOC} > $usedHeapSummary{$_}{ALLOCBYTES})
      {
         $hashCostSummarytcb{$keyCost}{MINALLOC} = $usedHeapSummary{$_}{ALLOCBYTES};
      }      
      $hashCostSummarytcb{$keyCost}{TOTALLOCS} += 1;	 
      $TotalAllocatedBytes += $usedHeapSummary{$_}{ALLOCBYTES};
	  $hashCostSummarytcb{$keyCost}{TCB_SYMBOL}=$usedHeapSummary{$_}{TCB_SYMBOL};
   }
}

sub processUsedFragmentsCallerPtr {
   my $keyCost;
foreach  (sort keys %usedHeapSummary) {
      $keyCost = $usedHeapSummary{$_}{CALLER_PTR}; 
      if(!defined($hashCostSummarycaller{$keyCost}))
         {
            # Currently we just show the allocated bytes and NOT the bytes 
            # requested... that would be alloc - 12( from struct) - EXtra bytes
            # for each fragment
            $hashCostSummarycaller{$keyCost}{ALLOCBYTES} = 0;
            $hashCostSummarycaller{$keyCost}{MAXALLOC} = 0;
            $hashCostSummarycaller{$keyCost}{MINALLOC} = 999999999;            
            $hashCostSummarycaller{$keyCost}{TOTALLOCS} = 0;
			$hashCostSummarycaller{$keyCost}{CALLER_SYMBOL}="";
         }
      $hashCostSummarycaller{$keyCost}{ALLOCBYTES} += $usedHeapSummary{$_}{ALLOCBYTES};
      if($hashCostSummarycaller{$keyCost}{MAXALLOC} < $usedHeapSummary{$_}{ALLOCBYTES})
      {
         $hashCostSummarycaller{$keyCost}{MAXALLOC} = $usedHeapSummary{$_}{ALLOCBYTES};
      }
      if($hashCostSummarycaller{$keyCost}{MINALLOC} > $usedHeapSummary{$_}{ALLOCBYTES})
      {
         $hashCostSummarycaller{$keyCost}{MINALLOC} = $usedHeapSummary{$_}{ALLOCBYTES};
      }      
      $hashCostSummarycaller{$keyCost}{TOTALLOCS} += 1;		  
	   $i=rindex($usedHeapSummary{$_}{CALLER_SYMBOL},"\\");
	   $hashCostSummarycaller{$keyCost}{CALLER_SYMBOL}=substr($usedHeapSummary{$_}{CALLER_SYMBOL},$i+1);   	    
   }
}

# Sort the costliest to the economic by the hashno+lineno combo
sub printCostSummaryTcb {
   my $OUTPUTFILEHANDLE = shift;
   my ($tcb, $tcb_symbol,   $NumAllctns, $MaxAllctd, $MinAllctd, $TotAllctd, $PercentAllocated);   
   print $OUTPUTFILEHANDLE "Allocations Summary for Tcb (Sorted in total allocated bytes)\n";
   print $OUTPUTFILEHANDLE "============================================================================\n\n";
   format HeaderFormat = 
@>>>>>>>>>>   @>>>>>>>>>>>>>>>>>	@>>>>>>   @>>>>>   @>>>>>>  @>>>>>   @>>>>>>>>>
$tcb,  $tcb_symbol, $NumAllctns, $MaxAllctd, $MinAllctd, $TotAllctd, $PercentAllocated
.
  $~ = HeaderFormat;
  $tcb = "TCB";
  $tcb_symbol="TCB";
  $NumAllctns = "Num. of";
  $MaxAllctd = "Max.";
  $MinAllctd = "Min.";
  $TotAllctd = "Total";  
  $PercentAllocated= "Percent";
  write();

  $tcb = "Pointer";
  $tcb_symbol="Symbol";
  $NumAllctns = "Alctns";
  $MaxAllctd = "Alctd";
  $MinAllctd = "Alctd";
  $TotAllctd = "Alctd";
  $PercentAllocated= "Alctd";
  write();
  print $OUTPUTFILEHANDLE "-------------------------------------------------------------------------------------------------\n";

  format SummaryFormat = 
@>>>>>>>>>>	@>>>>>>>>>>>>>>>>>	@######  @######   @###### @######   @###.##
$tcb, ,  $tcb_symbol,   $NumAllctns, $MaxAllctd, $MinAllctd, $TotAllctd, $PercentAllocated
.

   $~ = SummaryFormat;
   foreach (sort { $hashCostSummarytcb{$b}{ALLOCBYTES} <=> $hashCostSummarytcb{$a}{ALLOCBYTES}  } keys %hashCostSummarytcb) {
      $tcb = $_;
	  $tcb_symbol = $hashCostSummarytcb{$_}{TCB_SYMBOL};
      $NumAllctns = $hashCostSummarytcb{$_}{TOTALLOCS};
      $MaxAllctd = $hashCostSummarytcb{$_}{MAXALLOC};
      $MinAllctd = $hashCostSummarytcb{$_}{MINALLOC};
      $TotAllctd = $hashCostSummarytcb{$_}{ALLOCBYTES}; 
	  $PercentAllocated  = ($hashCostSummarytcb{$_}{ALLOCBYTES} * 100)/$TotalAllocatedBytes;
      write();
   }
   print $OUTPUTFILEHANDLE "\n\n";
}

sub printCostSummaryCaller {
   my $OUTPUTFILEHANDLE = shift;
   my ($caller, $caller_symbol,   $NumAllctns, $MaxAllctd, $MinAllctd, $TotAllctd, $PercentAllocated);   
   print $OUTPUTFILEHANDLE "Allocations Summary for Caller (Sorted in total allocated bytes)\n";
   print $OUTPUTFILEHANDLE "============================================================================\n\n";
   format HeaderFormat1 = 
@>>>>>>>>>>   @>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>	@>>>>>>   @>>>>>   @>>>>>>  @>>>>>   @>>>>>>>>>
$caller,  $caller_symbol, $NumAllctns, $MaxAllctd, $MinAllctd, $TotAllctd, $PercentAllocated
.
  $~ = HeaderFormat1;
  $caller = "Caller";
  $caller_symbol="Caller";
  $NumAllctns = "Num. of";
  $MaxAllctd = "Max.";
  $MinAllctd = "Min.";
  $TotAllctd = "Total";  
  $PercentAllocated= "Percent";
  write();

  $caller = "Pointer";
  $caller_symbol="Symbol";
  $NumAllctns = "Alctns";
  $MaxAllctd = "Alctd";
  $MinAllctd = "Alctd";
  $TotAllctd = "Alctd";
  $PercentAllocated= "Alctd";
  write();
  print $OUTPUTFILEHANDLE "--------------------------------------------------------------------------------------------------\n";

  format SummaryFormat1 = 
@>>>>>>>>>>	@>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>	@######  @######   @###### @######   @###.##
$tcb, ,  $tcb_symbol,   $NumAllctns, $MaxAllctd, $MinAllctd, $TotAllctd, $PercentAllocated
.

   $~ = SummaryFormat1;
   foreach (sort { $hashCostSummarycaller{$b}{ALLOCBYTES} <=> $hashCostSummarycaller{$a}{ALLOCBYTES}  } keys %hashCostSummarycaller) {
      $tcb = $_;
	  $tcb_symbol = $hashCostSummarycaller{$_}{CALLER_SYMBOL};
      $NumAllctns = $hashCostSummarycaller{$_}{TOTALLOCS};
      $MaxAllctd = $hashCostSummarycaller{$_}{MAXALLOC};
      $MinAllctd = $hashCostSummarycaller{$_}{MINALLOC};
      $TotAllctd = $hashCostSummarycaller{$_}{ALLOCBYTES}; 
	  $PercentAllocated  = ($hashCostSummarycaller{$_}{ALLOCBYTES} * 100)/$TotalAllocatedBytes;
      write();
   }
   print $OUTPUTFILEHANDLE "\n\n";
}
