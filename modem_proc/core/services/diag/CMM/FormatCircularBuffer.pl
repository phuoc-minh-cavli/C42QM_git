#========================================================================
#
# FormatCircularBuffer.pl
#
# This perl file parses the Data Blocks in the Circular buffer and generates a QMDL file.
# The input to this perl file are as below:
# 1. Circular Buffer Dump
# 2. QMDL File output Path
# 3. Circular Buffer Head
# 4. Circular Buffer Tail
# Output of the perl script is the QMDL file with circular buffer data.
#
#
# Copyright (c) 2016 by QUALCOMM Technologies, Inc
#=======================================================================

#==========================================================================
#
#                       EDIT HISTORY FOR MODULE
#
#when       who     what, where, why
#--------   ---     ------------------------------------------------------------
#01/13/16   gn      Created.

#===========================================================================*

my $circular_data_out;
my $circular_data_in ;
my $head ;
my $tail ;
my $i=0,$item;
my $buffer = "";
my $pattern = "";
my $hashfile,$hashfile1,$hashfile2;
# first_block is 1 if we are checking the first block from the tail in the circular buffer
my $first_block=1;  

my @hashes,@hashbytes;
my @header = (0x1A,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00);

#First argument represents the Circular buffer dump extracted in the CMM file
if (defined($ARGV[0])) 
{
    $circular_data_in = $ARGV[0];
} 
else 
{ 
    die "circular buffer data input file not specified.\n";
}

#The output QMDL file path
if (defined($ARGV[1])) 
{
    $circular_data_out = $ARGV[1];	
} 
else 
{ 
    die "circular buffer data QMDL output file not specified.\n";
}

#Circular Buffer Head
if (defined($ARGV[2])) 
{
    $head = oct $ARGV[2];
} 
else 
{ 
    die "Circular Buffer head not specified.\n";
}

#Circular Buffer Tail
if (defined($ARGV[3])) 
{
    $tail = oct $ARGV[3];
} 
else 
{ 
    die "Circular Buffer Tail not specified.\n";
}

open (INFILE,$circular_data_in) or die "Not able to open the $circular_data_in file. \n";
binmode(INFILE);

open (OUTFILE, ">", $circular_data_out) or die "Not able to open the file for writing. \n";
binmode (OUTFILE);


#QMDL2 Header
# Field                 Length (in bytes)     Description 
# HeaderLength             4                  Number of bytes reserved for header
# Version                  1                  Version value to set
# HDLC DataType            1                  0 - indicates hdlc encoding removed
#                                             1-indicates hdlc encoding enabled
# GuidListEntryCount       4                  Number of Guids available to read
# GUID array               16*                An array of available GUIDs.This can expand upto 
#											  the maximum number of GUIDlist entries reserved in diag.

#Printing Header to QMDL file 
#Sample Header : 1A 00 00 00         01          00      01 00 00 00    B9 E2 67 63 1B 62 E8 7E F6 F9 A1 D5 DA 48 4D 92
#                Header Length(4) Version(1) HDLCtype(1) GUIDcount(4)                  GUIDarray    

for ($i=0; $i < 10; $i++){ print OUTFILE (chr $header[$i]);}

#QShrink Hash file
#This script only supports one guid currently and would need to be expanded to support multiple guids for devices with Multi PD

if (defined($ARGV[4]) && defined($ARGV[5])) 
{
    $hashfile1 = $ARGV[4];
	$hashfile1 =~ s/0x//;
	$hashfile2 = $ARGV[5];
	$hashfile2 =~ s/0x//;
	$hashfile = join("",$hashfile1, $hashfile2);
	#printf "Input hash file is $hashfile";
	
	#Splitting the hash string into 2character array 
	@hashes = split //, $hashfile; 
	$i=0;
	foreach (@hashes) {$hashbytes[$i/2].=$hashes[$i];$i+=1;}
	$i=0;
	# @hashbytes = (B9,E2,67,63,1B,62,E8,7E,F6,F9,A1,D5,DA,48,4D,92)
	
	# Printing the GUID to qmdl file 
	foreach (@hashbytes) {print OUTFILE (chr hex $hashbytes[$i]); $i+=1; }
	
} else 
{ 
    die "\nqbd file not specified, exiting.";
}

#Start processing from Tail
seek(INFILE, $tail,0);

# Process the Header of the first Data block
ProcessHeader();   
	
#Read length bytes from the input file 
while(read(INFILE, $buffer, ($length)) !=0 )
{
   # Copy all the data from the input file to qmdl file
   foreach (split(//,$buffer)) 
   {
      $item = sprintf("%02x", ord($_));  
      print OUTFILE (chr hex $item);
   }
   
   # Adjust for padding
   $pad=(($length+3)& (~3)) - $length;
   seek(INFILE, $pad,1);
   
   $file_pos = tell(INFILE);
   
   #Check for the wrap case
	if ($file_pos == $head+$pad)
   {
        exit;
   }

# Process the Header of the next Data block
ProcessHeader(); 

}

	 
# Block structure : Header + Data 
#
# Header :
# Pattern : DEAD : 2 Bytes
# Length of block : 2 Bytes
#
# Data (4byte aligned) (Non HDLC encoded)
# Starting character    : 7E
# Version               : 01
# Length 			    : Length of payload (2 Bytes)
# Payload               : Data
# Terminating character : 7E

#Sample Data in the block
# ________address|________0________4________8________C_0123456789ABCDEF
     # D:8AF15A90|>189EDEAD 0013017E 00110099 0D650400 ....~.........e.
     # D:8AF15AA0| 0000013B 00005C6F 7E004D92 0013017E ;...o\...M.~~...
     # D:8AF15AB0| 00110099 0D6A1800 0000013B 00005C6E ......j.;...n\..
     # D:8AF15AC0| 7E004D92 0013017E 00110099 0D6D8000 .M.~~.........m.

# Subroutine to process the Header of the Data block 
sub ProcessHeader{

use constant DIAGBUF_PATTERN1 => 0xDE;
use constant DIAGBUF_PATTERN2 => 0xAD;

 #Read the pattern of the Block
   if(read(INFILE, $pattern,1) !=0)
   {
      $code =sprintf("%02x",ord($pattern));
      $code_hex2 = $code;
	  
	  read(INFILE, $pattern,1);
	  $code =sprintf("%02x",ord($pattern));
      $code_hex1 = $code;
	        
      if ((hex($code_hex1) != DIAGBUF_PATTERN1) && (hex($code_hex2) != DIAGBUF_PATTERN2))
      {
		 if($first_block==1)
		 {
			#Buffer header corrupted, Should never happen ideally
			die ("Invalid Circular Buffer pattern for the packet: $code_hex \n");
		 }
		 else
		 {
			#Reached end of buffer need to wrap
			$file_pos=0;
			seek(INFILE,$file_pos,0);
			ProcessHeader();
			return;
		 }
      } 
   }
   
   #Done processing first block header, set first_block to 0
   $first_block=0;
   
   #Read the lower byte of the length
   if(read(INFILE, $length,1)!=0)
   {
     $length1 = sprintf("%02x", ord($length));
   }
   else
   {
      die ("Unable to read the lower byte of the Length\n");
   }
   
   #Read the upper byte of the length   
   if(read(INFILE, $length,1)!=0)
   {
      $length2 = sprintf("%02x", ord($length));     
   }
   else
   {
      die ("Unable to read the Upper byte of the Length\n");
   }
   
   #Calculate the length of the Block
   $length=join("",$length2, $length1);
   $length=sprintf("%d", hex($length));     
   
} 
close (INFILE) or die "Not able to close the file: $infile \n";
close (OUTFILE) or die "Not able to close the file: $outfile \n";