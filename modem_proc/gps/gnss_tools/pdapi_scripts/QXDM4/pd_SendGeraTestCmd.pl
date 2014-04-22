##########################################################################
# $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss_tools/pdapi_scripts/QXDM4/pd_SendGeraTestCmd.pl#1 $
# $DateTime: 2020/07/21 21:23:44 $
# $Author: pwbldsvc $
#
# This example demonstrates using a DM Command to set GFLog control
#
# Usage: perl pd_GFLogOnOff.pl 11 8 1 (11 is the COM Port the FFA is connected
#        to, 8 is the Client Type and '1' to enable GFLog data)
#
##########################################################################

use vars qw(%opts);

use Win32::OLE;
use Win32::OLE::Variant;

use Getopt::Long;
use pd_SendGeraTestCmd;
use pd_StartDiag;
use pd_EndDiag;
use constant true                => 1;

%opts = ();
GetOptions (\%opts, 'help');

&usage() if ($opts{'help'});


######################################################################
# Create QXDM instance
######################################################################

# my $qxdm_app = new Win32::OLE 'QXDM.Application';
# if (!$qxdm_app)
# {
  # print "ERROR: Unable to invoke the QXDM application.\n";
  # die;
# }
my $IQXDM = new Win32::OLE 'QXDM.QXDMAutoApplication';
if (!$IQXDM)
{
  print "ERROR: Unable to invoke the QXDM application.\n";
  die;
}
			$qxdm_app = $IQXDM->GetAutomationWindow();
			$qxdm_app2 = $qxdm_app;
		$qxdm_app->SetVisible(true);

#####################################################################
# Prompt for user input if arguments are not provided
#####################################################################

$comport = $ARGV[0];
$client_type = $ARGV[1];
$gera_test_command = $ARGV[2];
check_client_type();

# Allow user input for comport
if (not defined ($comport)) {
    print("\nUsage: perl pd_SendGeraTestCmd.pl <COM Port> <Test Client Type> <GERA cmd type> <GERA client id> <Ex enable client mask >\n\n");
    print ("Please enter the COM port the FFA is connected to\n");
    $count = 1;
    chomp ($comport = <STDIN>);
    print("\n");
}
# Allow user input for client type
if (not defined ($client_type)) {
    if ($count == 0) {
        print("\nUsage: perl pd_SendGeraTestCmd.pl <COM Port> <Test Client Type> <GERA cmd type> <GERA client id> <Ex enable client mask >\n\n");
    }
    #Prompt user to enter Client Type
    print ("Please enter the Client type
    0 - BROWSER         
    1 - UI                 
    2 - PDA            
    3 - DM                
    4 - NI        
    5 - BREW            
    6 - JAVA            
    7 - TEST           
    8 - TEST2
    9 - OEM\n");
    chomp ($client_type = <STDIN>);
    check_client_type();
    print("\n");
}

######################################################################
# Open QXDM Connection by setting the COM port from command line.
######################################################################

# COM Port value should be supplied as first argument to this script
$qxdm_app->{COMPort} = $comport; # Default is COM1

# Wait until phone is connected
while ($qxdm_app->{IsPhoneConnected} == 0)
{
  print "Unable to Connect to the Phone. Retrying...\n";
  sleep(1);
}

######################################################################
# Global variables initialized here
######################################################################

my @client_ids;
# Set the timeout to 2 seconds.
my $timeout = 2000;

print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
print("Initialize, Register, & Activate the client\n");
print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

#######################################################################
# Build a diagnostic request for CGPS_INIT_CMD to initialize the client
#######################################################################

  $req_apicmd1 = new pd_StartDiag;
  $req_apicmd1->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd1->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd1->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd1->{"cgps_cmd_code"}   = 0;   # CGPS Cmd Code
  $req_apicmd1->{"version"}         = 1;   # Version Number
  $req_apicmd1->{"client_id"}       = $client_type; # Client Type

  ######################################################################
  # Construct the DM CGPS_INIT_CMD capture request packet
  ######################################################################

  my $req_apicmd_pkt = $req_apicmd1->SetCaptureCmd();
  printf ("Sending CGPS_INIT_CMD: Initialize client type %d\n", $clientType);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_INIT_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd1->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd1->GetStatus();
  $client_id = $req_apicmd1->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

  sleep(2);

############################################################################
# Build a diagnostic request for CGPS_REGISTER_PD_CMD to register the client
# Registers the client to receive registered PD events
############################################################################

  $req_apicmd2 = new pd_StartDiag;
  $req_apicmd2->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd2->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd2->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd2->{"cgps_cmd_code"}   = 1;   # CGPS Cmd Code
  $req_apicmd2->{"version"}         = 1;   # Version Number
  $req_apicmd2->{"client_id"}       = $client_id;  # Client ID from Init
  $req_apicmd2->{"reg_type"}        = 0;   # Register

  ######################################################################
  # Construct the DM CGPS_REGISTER_PD_CMD capture request packet
  ######################################################################

  my $req_apicmd_pkt = $req_apicmd2->SetCaptureCmd();
  printf ("Sending CGPS_REGISTER_PD_CMD: Register client ID %d\n", $client_id);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_REGISTER_PD_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd2->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd2->GetStatus();
  $client_id = $req_apicmd2->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

  sleep(2);

#########################################################################
# Build a diagnostic request for CGPS_ACTIVATE_CMD to activate the client
# Activates the client so that it can be notified of PD events
#########################################################################

  $req_apicmd3 = new pd_StartDiag;
  $req_apicmd3->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd3->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd3->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd3->{"cgps_cmd_code"}   = 7;   # CGPS Cmd Code
  $req_apicmd3->{"version"}         = 1;   # Version Number
  $req_apicmd3->{"client_id"}       = $client_id;  # Client ID from Init

  ######################################################################
  # Construct the DM CGPS_ACTIVATE_CMD capture request packet
  ######################################################################

  my $req_apicmd_pkt = $req_apicmd3->SetCaptureCmd();
  printf ("Sending CGPS_ACTIVATE_CMD: Activate client ID %d\n", $client_id);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_ACTIVATE_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd3->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd3->GetStatus();
  $client_id = $req_apicmd3->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

# Run infinitely until user decides to EXIT 
while (1)
{
  print ("Please enter the GERA test cmd type
  0 - Exit GERA test commands
  1 - GERA TEST command to register client
  2 - GERA TEST command to deregister client
  3 - GERA TEST command to request for client enable
  4 - GERA TEST command to request for client disable
  6 - GERA TEST command for GERA reset
  7 - GERA TEST command to query GERA for client state \n");
  chomp ($gera_test_command = <STDIN>);    	
  print("\n");
  print("\nThe current client IDs supported by GERA
  0 - E911 client
  1 - APMP TTR client
  2 - DPO client
  3 - Standalone client
  4 - MSA client
  5 - MSB client \n");

  if($gera_test_command  == 0){
      last;
    }
  if($gera_test_command  == 1) {
    print("\nEnter the GERA client ID to register with GERA.\n");
    chomp($gera_client_id = <STDIN>);
  }
  if($gera_test_command  == 2) {
    print("\nEnter the GERA client ID to deregister with GERA.\n");
    chomp($gera_client_id = <STDIN>);
  }
  if($gera_test_command  == 3) {
    print("\nEnter the GERA client ID to request for client enable from GERA.\n");
    chomp($gera_client_id = <STDIN>);

    print("\nEnter the dependent client mask . Current masks supported 
    0x1 - E911 client mask
    0x2 - APMP TTR client mask
    0x4 - DPO client mask
    0x8 - Standalone client mask
    0x10 - MSA client mask
    0x20 - MSB client mask \n");
    chomp($ex_enable_client_mask = <STDIN>);
  }
  if($gera_test_command  == 4) {
    print("\nEnter the GERA client ID to request for client disable from GERA . \n");
    chomp($gera_client_id = <STDIN>);
  }

######################################################################
# Build a diagnostic request for CGPS_GERA_TEST_CMD
######################################################################

  $req_SendGeraTestCmd = new pd_SendGeraTestCmd;
  $req_SendGeraTestCmd->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_SendGeraTestCmd->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_SendGeraTestCmd->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_SendGeraTestCmd->{"cgps_cmd_code"}   = 208; # CGPS Cmd Code defined for CGPS_GERA_TEST_CMD in aries_gpsdiag.h
  $req_SendGeraTestCmd->{"version"}         = 1;   # Version Number
  $req_SendGeraTestCmd->{"client_id"}       = $client_id;   # Client id
  $req_SendGeraTestCmd->{"gera_test_command"} = $gera_test_command;   # GERA test command type
  $req_SendGeraTestCmd->{"gera_client_id"}         = $gera_client_id;   # GERA client ID
  $req_SendGeraTestCmd->{"ex_enable_client_mask"}  = $ex_enable_client_mask;    # Extended enable client mask

  
  ######################################################################
  # Construct the DM CGPS_GERA_TEST_CMD capture request packet
  ######################################################################

  my $req_SendGeraTestCmd_pkt = $req_SendGeraTestCmd->SetCaptureCmd();
  printf ("Sending CGPS_GERA_TEST_CMD: %d\n", $gera_test_command);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_GeraTestCmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_SendGeraTestCmd_pkt, $timeout);

  my $rsp_GeraTestCmd_len = length($rsp_GeraTestCmd_pkt);
  printf ("DM CGPS_GERA_TEST_CMD Response Packet Length: %d\n", $rsp_GeraTestCmd_len);

  $req_SendGeraTestCmd->ParseData($rsp_GeraTestCmd_pkt);

  $result     = $req_SendGeraTestCmd->GetResult();
  $result_txt = $req_SendGeraTestCmd->GetResultText();
  printf ("Result = %d = %s\n\n", $result, $result_txt);
}# end while(1)
print("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
print("DeActivate, DeRegister, & Release the client\n");
print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

#############################################################################
# Build a diagnostic request for CGPS_DEACTIVATE_CMD to deactivate the client
#############################################################################

  $req_apicmd1 = new pd_EndDiag;
  $req_apicmd1->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd1->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd1->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd1->{"cgps_cmd_code"}   = 8;   # CGPS Cmd Code
  $req_apicmd1->{"version"}         = 1;   # Version Number
  $req_apicmd1->{"client_id"}       = $client_id; # Client Type

  ######################################################################
  # Construct the DM CGPS_DEACTIVATE_CMD capture request packet
  ######################################################################

  my $req_apicmd_pkt = $req_apicmd1->SetCaptureCmd();
  printf ("Sending CGPS_DEACTIVATE_CMD: Deactivate client type %d\n", $clientType);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_DEACTIVATE_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd1->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd1->GetStatus();
  $client_id = $req_apicmd1->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

  sleep(2);

##############################################################################
# Build a diagnostic request for CGPS_REGISTER_PD_CMD to Deregister the client
##############################################################################

  $req_apicmd2 = new pd_EndDiag;
  $req_apicmd2->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd2->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd2->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd2->{"cgps_cmd_code"}   = 1;   # CGPS Cmd Code
  $req_apicmd2->{"version"}         = 1;   # Version Number
  $req_apicmd2->{"client_id"}       = $client_id;  # Client ID from Deactivate
  $req_apicmd2->{"reg_type"}        = 1;   # Deregister

  ######################################################################
  # Construct the DM CGPS_REGISTER_PD_CMD capture request packet
  ######################################################################

  my $req_apicmd_pkt = $req_apicmd2->SetCaptureCmd();
  printf ("Sending CGPS_REGISTER_PD_CMD: DeRegister client ID %d\n", $client_id);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_REGISTER_PD_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd2->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd2->GetStatus();
  $client_id = $req_apicmd2->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

  sleep(2);

#######################################################################
# Build a diagnostic request for CGPS_RELEASE_CMD to Release the client
#######################################################################

  $req_apicmd3 = new pd_EndDiag;
  $req_apicmd3->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd3->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd3->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd3->{"cgps_cmd_code"}   = 9;   # CGPS Cmd Code
  $req_apicmd3->{"version"}         = 1;   # Version Number
  $req_apicmd3->{"client_id"}       = $client_id;  # Client ID from Deactivate

  ######################################################################
  # Construct the DM CGPS_RELEASE_CMD capture request packet
  ######################################################################

  my $req_apicmd_pkt = $req_apicmd3->SetCaptureCmd();
  printf ("Sending CGPS_RELEASE_CMD: Release client ID %d\n", $client_id);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_RELEASE_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd3->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd3->GetStatus();
  $client_id = $req_apicmd3->GetClientId();
  printf ("Status = %d, Client Id = %d\n", $status, $client_id);
  sleep(2);

  $qxdm_app = NULL;

sub check_client_type()
{
  while ($client_type > 9)
  {
    print("\nClient Type out of range. Please enter a value between 0-9
    0 - BROWSER         
    1 - UI                 
    2 - PDA            
    3 - DM                
    4 - NI        
    5 - BREW            
    6 - JAVA            
    7 - TEST           
    8 - TEST2
    9 - OEM\n");
    chomp ($client_type = <STDIN>);
  }
  return;
}

######################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#   Display usage information.
#
# RETURN VALUE:
#   None
#
######################################################################
sub usage
{
  my $usage = <<END;

SendGeraTestCmd

Syntax:   pd_SendGeraTestCmd [options]

Options:  -help               Display this help message.

Example:  perl pd_SendGeraTestCmd.pl <COM Port> <Test Client Type> <GERA cmd type> <GERA client id> <Ex enable client mask >

END

  print "$usage";
  exit (0);
}

