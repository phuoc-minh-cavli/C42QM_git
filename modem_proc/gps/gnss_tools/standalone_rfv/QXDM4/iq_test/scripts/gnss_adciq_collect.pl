# 
# $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss_tools/standalone_rfv/QXDM4/iq_test/scripts/gnss_adciq_collect.pl#1 $ $DateTime: 2020/07/21 21:23:44 $ $Author: pwbldsvc $
#*****************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc. 
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
#*****************************************************************************
#
# Description:
# This script is typically used by a user to run an ADCIQ test. It takes the  
# QXDM COM port as an input parameter. It converts the QXDM ISF file to DLF,
# converts the DLF to a binary file using IqPacketParser and finally tries
# to plot the IQ test results using gnss_IqTest_IqFileProc.exe.
# 
# Dependencies:
# IqPacketParser.exe
# gnss_IqTest_IqFileProc.exe
# Apex to convert ISF to DLF
#*****************************************************************************

use vars qw(%opts);
use Cwd;
use IqTest;
use Win32::OLE;
use Win32::OLE::Variant;
use Getopt::Long;
use HelperFunctions4;
use File::Spec;

# Global variables
my $QXDM;
my $QXDM2;

my %eventHash = (EVENT_GPS_LM_IQ_TEST_COMPLETE        => 1251);

%opts = ();
GetOptions( \%opts, 'help');

&usage() if ($opts{'help'});
my $port_num = int($ARGV[0]);

my $RC = Initialize();

if ($RC == false)
{
  return;
}

# Get QXDM version
my $Version = $QXDM->{AppVersion};
printf "QXDM Version: $Version\n";

$QXDM->{COMPort} = $port_num;

# Wait until phone is connected
while( $QXDM->{IsPhoneConnected} == 0 ){
   print "Unable to Connect to the Phone. Retrying.\n";
   sleep(1);
}

# Get file name from script folder path
my $isf_filename = GenerateFileName( "", "" );

(my $isf_volume, my $isf_directories, my $isf_file) = File::Spec->splitpath($isf_filename);
$isf_filename = $isf_file;

my $isf_handle = $QXDM->CreateItemStore($isf_filename . ".isf");

$req = new IqTest;
$req->{"cmd"}                  = 75;
$req->{"subsys_id"}            = 13;
$req->{"sub_cmd"}              = 0x65;
$req->{"gps_cmd"}              = 22;
$req->{"version"}              = 54;
$req->{"testcontrol"}          = 0;   # 0/1 => Start/Stop Iq Test 
$req->{"flag"}                 = 0x9; # VALID VALUES for "Flag" field:-
                                      # Bit0 - Enable IQ Logging
                                      # Bit1 - Enable FFT Logging (on-board FFT)
                                      # Bit2 - Enable Hanning window for on-board FFT
                                      # Bit3 - 1 => Disable "Dropped Log Recovery" feature
                                      #        0 => Enable "Dropped Log Recovery" feature
$req->{"retransmitpktnum"} = 0;  # Unused
$req->{"rfstatusloggingratehz"} = 4;  # Unused


$req->{"collectmode"} = 16;     # VALID VALUES for "CollectMode":-
                               # 0   WBIQ BP1 and BP2 simultaneous (2.046 MHz sample rate)
                               # 1   WBIQ BP1 (2.046 MHz sample rate)
                               # 2   WBIQ BP2 (2.046 MHz sample rate)
                               # 3   WBIQ BP3 (1.022 MHz sample rate)
                               # 4   FBIQ BP4 (20.46 MHz sample rate)
                               # 5   NBIQ BP1 Autocenter  (1 kHz sample rate) 
                               # 6   NBIQ BP2 Autocenter  (1 kHz sample rate) 
                               # 7   NBIQ BP3 Autocenter  (1 kHz sample rate) 
                               # 8   NBIQ BP1 Manual  (1 kHz sample rate)
                               # 9   NBIQ BP2 Manual  (1 kHz sample rate)
                               # 10  NBIQ BP3 Manual  (1 kHz sample rate)
                               # 11  NBIQ BP4 Manual  (1 kHz sample rate)
                               # 12  (84 div by 5) MHz (Sample Logging ADC1)
                               #     Each 32-bit word has 3-bit I and 3-bit Q from ADC1
                               #     LSB is the most recently captured sample.
                               # 13  (84 div by 5) MHz (Sample Logging ADC2)
                               #     Each 32-bit word has 3-bit I and 3-bit Q from ADC1
                               #     LSB is the most recently captured sample.
                               # 14  (84 div by 4) MHz (Sample Logging ADC1 + ADC2)
                               #     Each 32-bit word has 2-bit I and 2-bit Q from ADC1 and 2-bit I and 2-bit Q from ADC2.
                               #     Format: ADC1:ADC2 (I:Q).
                               #     LSB is the most recently captured sample.
                               # 15  BP3 FBIQ test (10.22 MHz sample rate)
                               #     2 bytes per sample (8 bit I and 8 bit Q)
                               # 16  ADC I/Q at 81 MHz (or 82.28, depends on Nav Main frequency)
                               # 17  WBIQ_BP2 at 4MHz (Beidou)
                               # 18  WBIQ_BP1 SubChannel3 at 4MHz (Galileo)


$req->{"glo_r1_chan"} = 0;     # -7~6 allowed
$req->{"glo_hw_chan"} = 1;     # 1-12 allowed

$req->{"num1ksamples"} = 256;   # 1-1024 allowed for all WB. 0-1024 allowed for NB. 0=>Collect Forever
$req->{"integrations"} = 20;    # Greater than 0
$req->{"centerfreqnbhz"} = 100000;      # -1 MHz to +1 MHz for BP1 and BP2
                                        # -500 kHz to +500 kHz for BP3
                                        # -10 MHz to +10 MHz for BP4

$req->{"adciqsatthresh"} = 9950;      # Default saturation threshold - (unit of 0.01%). 9950 indicates a saturation
                                        # threshold of 99.5%					
$req->{"adciqwidebandjammerstartfreqhz"} = 1555000000; # 1555 MHz Start
$req->{"adciqwidebandjammerstopfreqhz"} = 1573000000; # 1573 MHz Stop

my $req_pkt = $req->SetCaptureIqTestCmd();

my $mode_event = EVENT_GPS_LM_IQ_TEST_COMPLETE;

# Send string to stdout before sending cmd. This is to reduce latency
printf("    QXDM ===>  wait_event $mode_event \n");

my $timeout = 300000;

if($req->{"collectmode"} == 5 || $req->{"collectmode"} == 11)
{
  $timeout = (20 + $req->{"num1ksamples"} * $req->{"integrations"}) * 1000;
}

AddItems();

# Send command.
my $rsp_pkt_var = $QXDM->SendDmIcdPacketEx( $req_pkt, 3000);

my $rsp_len_var = length($rsp_pkt_var);

printf( "IQ Collect Response: " );
my @results = unpack("C$rsp_len_var", $rsp_pkt_var);
for( $i=0; $i<$rsp_len_var; $i++ )
{
	printf( "%d ", $results[$i] );
}
printf( "\n\n" );

eventQXDM($mode_event, $timeout);

SaveISF( );

if(-e "C:\\Program Files (x86)\\Qualcomm\\APEX 6.x\\Bin\\Apex.exe")
{
  print("\"C:\\Program Files (x86)\\Qualcomm\\APEX 6.x\\Bin\\Apex.exe\" -dlf " . $isf_filename . ".isf ");
  system("\"C:\\Program Files (x86)\\Qualcomm\\APEX 6.x\\Bin\\Apex.exe\" -dlf " . $isf_filename . ".isf ");
}
elsif (-e "C:\\Program Files\\Qualcomm\\QXDM\\bin\\ISFConverter.exe")
{
   print("\"C:\\Program Files\\Qualcomm\\QXDM\\bin\\ISFConverter.exe\" /pc " . $isf_filename . ".isf " . $isf_filename . ".dlf");
   system("\"C:\\Program Files\\Qualcomm\\QXDM\\bin\\ISFConverter.exe\" /pc " . $isf_filename . ".isf " . $isf_filename . ".dlf");
}
elsif (-e "C:\\Program Files (x86)\\Qualcomm\\QXDM\\bin\\ISFConverter.exe")
{
   print("\"C:\\Program Files (x86)\\Qualcomm\\QXDM\\bin\\ISFConverter.exe\" /pc " . $isf_filename . ".isf " . $isf_filename . ".dlf");
   system("\"C:\\Program Files (x86)\\Qualcomm\\QXDM\\bin\\ISFConverter.exe\" /pc " . $isf_filename . ".isf " . $isf_filename . ".dlf");
}
else
{
  # need the ISFConverter that comes with QXDM, print error if QXDM is not installed
  exit ("\n\tYou need to install QXDM to run this script\n");
}

print("\n\n");

 print("..\\iq_pkt_parser\\IqPacketParser.exe " . $isf_filename . ".dlf  " . "IQ_". $isf_filename . ".bin FFT_" . $isf_filename . ".bin");
system("..\\iq_pkt_parser\\IqPacketParser.exe " . $isf_filename . ".dlf  " . "IQ_". $isf_filename . ".bin FFT_" . $isf_filename . ".bin");

print("\n");

system( "if exist IQ_" . $isf_filename . ".bin  copy IQ_". $isf_filename . ".bin gnss_iq_collect.bin");
system( "if exist FFT_" . $isf_filename . ".bin  copy FFT_". $isf_filename . ".bin gnss_fft_collect.bin");

my $iqtestproccmd = "gnss_IqTest_IqFileProc.exe 141 ";

if($req->{"collectmode"} == 3)
{
  $iqtestproccmd = $iqtestproccmd . '50';
}
elsif($req->{"collectmode"} == 18)
{
  $iqtestproccmd = $iqtestproccmd . '764';
}

if(-e "gnss_IqTest_IqFileProc.exe")
{
  system( "if exist IQ_" . $isf_filename . ".bin ". $iqtestproccmd);
}

$QXDM = NULL;


# METHOD:
#   usage
#
# DESCRIPTION:
#   Display usage information.
#
# RETURN VALUE:
#   None
#

# Save the curent ISF
sub SaveISF
{
   my $cwd = getcwd();
   $QXDM->SaveItemStore( $cwd . "\\" . $isf_filename . ".isf" );
   $RC = true;
   return $RC;
}

sub usage
{
   my $usage = <<END;

GPS ADCIQ Collect

Syntax:   perl gnss_adciq_collect.pl <Options>

Options:  -help               Display this help message.
          Option 1:           <QPST Connection Port Number>

 
Example:  perl gnss_adciq_collect.pl 10

END

   print "$usage";
   exit (0);
}

sub eventQXDM {
   my($timeEvent, $timeOut) = @_;

   $_ = $timeEvent;
   return if ($configAttributes{"offline"});

   my $rspPkt = $QXDM->WaitEvent($eventHash{$timeEvent}, $timeOut);
   my $rspLen = length($rspPkt);

   if ((defined $rspLen) && ($rspLen ne ""))
   {
      print "IQ test complete event received\n";
      sleep 2;
   }
   else 
   {
      print "Error: Empty response\n";
   }
}


# Add items to the client
sub AddItems
{
   # Clearing item view to delete the existing logs
   # Without this it might take a long time to process IQ test logs
   $RC = $QXDM->ClearViewItems("Item View");
   if($RC == false)
   {
     print "Clear item view failed\n";
   }
   # Adding a small delay for clear to go through
   sleep 2;
   
   # Get a QXDM client
   my $Handle = $QXDM2->RegisterClient( "IqTest Parser", 1 );
   if ($Handle == 0xFFFFFFFF)
   {
      print "\nUnable to register a client\n";
      return;
   }

   my $Client = $QXDM2->ConfigureClientByKeys( $Handle );
   if ($Client == false)
   {
      print "Unable to configure client by keys\n";
      $QXDM2->UnregisterClient( $Handle );
      return;
   }
   
   $Client->AddLog(0x138A);
   $Client->AddEvent(1251);
   $Client->CommitConfig();
   print "\nConfigureClientByKeys succeeded\n";
}

# Initialize application
sub Initialize
{
   # Assume failure
   my $RC = false;

   # Create QXDM object
   $QXDM = QXDMInitializeInterface2();

   $QXDM2 = $QXDM;
   if ($QXDM2 == null)
   {
      print "\nQXDM does not support required interface";
      return $RC;
   }

   # Success
   $RC = true;
   return $RC;
}