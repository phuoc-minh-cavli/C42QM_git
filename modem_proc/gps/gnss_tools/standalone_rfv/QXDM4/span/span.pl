# 
# $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss_tools/standalone_rfv/QXDM4/span/span.pl#1 $
# $DateTime: 2020/07/21 21:23:44 $
# $Author: pwbldsvc $
#*****************************************************************************
#  Copyright (c) 2012, 2013 by QUALCOMM INCORPORATED. All Rights Reserved.
#  Copyright (c) 2015 Qualcomm Technologies, Inc.
#  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 
#
# Export of this technology or software is regulated by the U.S. Government.
# Diversion contrary to U.S. law prohibited.
#
# *****************************************************************************
# Description:
# This file starts QXDM, sets up a connection to the phone, and sends a command
# to control the GNSS On-The-Fly Spectrum Analyzer(OTFSA).
#
# Usage:
# perl span.pl 10
# 
#*****************************************************************************


use vars qw(%opts);
use Win32::OLE;
use Win32::OLE::Variant;
use SpAn;
use HelperFunctions4;
use Getopt::Long;

my %eventHash = (EVENT_GNSS_SPECTRUM_ANALYZER_STATUS        => 2243);

# Global variables
my $QXDM;
my $QXDM2;

# Launch QXDM
my $RC = Initialize();

if ($RC == false)
{
  #return;
}

# Get QXDM version
my $Version = $QXDM->{AppVersion};
printf "\nQXDM Version: $Version\n";

# Add items to the client
AddItems();


%opts = ();
GetOptions( \%opts, 'help');
my $port_num = int($ARGV[0]);

&usage() if ($opts{'help'});

# Specify QXDM COM port 
$QXDM->{COMPort} = $port_num;

# Wait until phone is connected
while( $QXDM->{IsPhoneConnected} == 0 ){
   print "Unable to Connect to the Phone. Retrying.\n";
   sleep(1);
}

# Set the timeout to 2 seconds.
my $timeout = 2000; # Timeout for receiving ACK for injected command

# Build a diagnostic request for On-The-Fly_Spectrum_Analyzer command
$req = new SpAn;
$req->{"cmd"}                  = 75;
$req->{"subsys_id"}            = 13;
$req->{"sub_cmd"}              = 0x65;
$req->{"gps_cmd"}              = 42;
$req->{"version"}              = 3;
$req->{"cmd_control_flag"}     = 0; # 0-Run once;1 - Run continuously
$req->{"scan_start_stop"}      = 1; # 0-Stop Scan; 1- Start scan
$req->{"gps_jobs_enabled"}     = 1; # bit0 - GPS fast scan; bit1 - GPS Deep scan
$req->{"glo_jobs_enabled"}     = 0; # bit0 - GLO fast scan; bit1 - GLO Deep scan
$req->{"probe_point"}          = 1;
$req->{"track_mode"}           = 0;
$req->{"notch_filt_reset"}     = 1;
$req->{"bds_jobs_enabled"}     = 0; # bit0 - BDS fast scan; bit7 - BDS Shallow scan
$req->{"gal_jobs_enabled"}     = 0; # bit0 - GAL fast scan; bit7 - GAL Shallow scan

# Enable logging or else the script wont receive the event.
#$QXDM->SendScript("logging on");
# Initialize the request as a Variant.
my $req_pkt = $req->SpanCmd();

# Display command request in Text window.

# Send string to stdout before sending cmd. This is to reduce latency
#printf("    QXDM ===>  wait_event $mode_event \n");
# Send command.
my $rsp_pkt_var = $QXDM->SendDmIcdPacketEx( $req_pkt, $timeout);
#eventQXDM($mode_event, 6000);	

my $rsp_len_var = length($rsp_pkt_var);

printf( "GNSS Spectrum Analyzer Command: " );
my @results = unpack("C$rsp_len_var", $rsp_pkt_var);
for( $i=0; $i<$rsp_len_var; $i++ )
{
	printf( "%d ", $results[$i] );
}
printf( "\n\n" );

if( ($rsp_len_var == 6) && ($results[2] == 0x65) &&
	($results[3] == 0) && ($results[4] == 42) )
{
	printf( "GNSS Spectrum Analyzer cmd accepted!\n" );
#	eventQXDM(EVENT_GPS_LM_ENTER_SA_RF_VERIF, 6000);
}
else
{
	printf( "GNSS Spectrum Analyzer cmd rejected!\n" );
}

$QXDM = NULL;
$QXDM2->UnregisterClient( $Handle );

# METHOD:
#   usage
#
# DESCRIPTION:
#   Display usage information.
#
# RETURN VALUE:
#   None
#
sub usage
{
   my $usage = <<END;

GNSS Spectrum Analyzer command

Syntax:   perl span.pl <Options>

Options:  -help               Display this help message.
          Option 1:           <QPST Connection Port Number>
 
Example:  perl span.pl 10

END

   print "$usage";
   exit (0);
}

###############################################################
# Subroutine to wait for an event for a specified timeout
################################################################
sub eventQXDM {
   my($timeEvent, $timeOut) = @_;

   $_ = $timeEvent;
   return if ($configAttributes{"offline"});

   unless(exists $eventHash{$timeEvent}) {
      printf( "Invalid event <$timeEvent>\n");
      return;
   }
   #else
   my $rspPkt = $QXDM->WaitEvent($eventHash{$timeEvent}, $timeOut);
   my $rspLen = length($rspPkt);

#print "rspPkt $rspPkt $rspLen\n";

   if ((defined $rspLen) && ($rspLen ne ""))
   {
      print "$timeEvent received\n";
      return 0;
   }
   else 
   {
      return;
   }
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
   my $Handle = $QXDM2->RegisterQueueClient( 256 );
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
   # Register for ENTER and EXIT events
   my ($key, $event_val) = each(%eventHash);
   $Client->AddEvent( $event_val );
   my ($key, $event_val) = each(%eventHash);
   $Client->AddEvent( $event_val );


   $Client->CommitConfig();

}

