######################################################################
# $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss_tools/pdapi_scripts/QXDM4/pd_GFLogOnOff.pm#1 $
# $DateTime: 2020/07/21 21:23:44 $
#
# Define class in Perl to copy display to EFS.
#
######################################################################

package pd_GFLogOnOff;

use Win32::OLE;
use Win32::OLE::Variant;

# CGPS_GEOFENCE_IN_MEM_LOG_CMD request packet field. Or response??
my %fields = (
  cmd_code           => undef, # (1 byte)
  subsys_id          => undef, # (1 byte)
  subsys_cmd_code    => undef, # (2 bytes)
  cgps_cmd_code      => undef, # (1 byte)
  version            => undef, # (1 byte)
  client_id          => undef, # (4 bytes)
  geofence_command   => undef, # (4 bytes)
  log_size           => undef, # (4 bytes)
  period_s           => undef, # (4 bytes)
);


######################################################################
# METHOD:
#   new
#
# DESCRIPTION:
#   Constructor
#
# RETURN VALUE:
#   Instance
#
######################################################################
sub new
{
  my $that = shift;
  my $class = ref($that) || $that;
  my $self = { %fields };
  bless $self, $class;
  return $self;
}

######################################################################
# METHOD:
#   AUTOLOAD
#
# DESCRIPTION:
#   Performs object validation
#
# RETURN VALUE:
#   reference object
#
######################################################################
sub AUTOLOAD
{
  my $self = shift;
  my $type = ref($that) || die "$self is not an object";
  my $name = $AUTOLOAD;
  $name =~ s/.*://;   # strip fully-qualified portion
  unless (exists $self->{$name})
  {
    die "Can't access '$name' field in object of class $type";
  }
  if (@_)
  {
    return $self->{$name} = shift;
  }
  else
  {
    return $self->{$name};
  }
}

######################################################################
# METHOD:
#   uint32toBytes
#
# DESCRIPTION:
#   Subroutine to convert 32-bit integer to array of 4 bytes
#
# RETURN VALUE:
#   Array of 4 individual bytes representing uint32
#
######################################################################
sub uint32toBytes
{
  my ($arg) = @_;
  my ($byte1, $byte2, $byte3, $byte4);

  # not sure of perl's bitwise operators - change code later
  $byte1 = $arg%256; $arg = $arg/256;
  $byte2 = $arg%256; $arg = $arg/256;
  $byte3 = $arg%256; $arg = $arg/256;
  $byte4 = $arg%256;

  return ($byte1, $byte2, $byte3, $byte4);
}

######################################################################
# METHOD:
#   SetCaptureCmd
#
# DESCRIPTION:
#   Initialize DM CGPS_SET_PARAM_CMD capture request packet
#
# RETURN VALUE:
#   Request packet
#
######################################################################
sub SetCaptureCmd
{
  my $self = shift;

  my $length = 22;
  my $type = ref($self) || die "$self is not an object";

  my $req_pkt = Variant(VT_ARRAY | VT_UI1, $length);

  $req_pkt->Put(0,  $self->{"cmd_code"});
  $req_pkt->Put(1,  $self->{"subsys_id"});
  $req_pkt->Put(2,  $self->{"subsys_cmd_code"} & 0xFF);
  $req_pkt->Put(3, ($self->{"subsys_cmd_code"} >> 8) & 0xFF);
  $req_pkt->Put(4,  $self->{"cgps_cmd_code"});
  $req_pkt->Put(5,  $self->{"version"});

  my @arg_bytes = uint32toBytes($self->{"client_id"});
  my $i = 6;
  foreach (@arg_bytes)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }
  
  my @arg_bytes = uint32toBytes($self->{"geofence_command"});
  my $i = 10;
  foreach (@arg_bytes)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }
  
  my @arg_bytes = uint32toBytes($self->{"log_size"});
  my $i = 14;
  foreach (@arg_bytes)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  my @arg_bytes = uint32toBytes($self->{"period_s"});
  my $i = 18;
  foreach (@arg_bytes)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  return $req_pkt;
}

######################################################################
# METHOD:
#   ParseData
#
# DESCRIPTION:
#   Subroutine to parse the response packet
#
# RETURN VALUE:
#   None
#
######################################################################
sub ParseData
{
  my $self = shift;
  my $type = ref($self) || die "$self is not an object";

  my $data = shift;
  my (@results) = unpack("CCSCCLLL", $data);

  $self->{"cmd_code"}        = $results[0];
  $self->{"subsys_id"}       = $results[1];
  $self->{"subsys_cmd_code"} = $results[2];
  $self->{"cgps_cmd_code"}   = $results[3];
  $self->{"version"}         = $results[4];
  $self->{"result"}          = $results[7];

  printf("cgps_cmd_code = %d, result = %d\n", $results[3], $results[7]);
}


######################################################################
# METHOD:
#   GetResult
#
# DESCRIPTION:
#   Subroutine to retrieve result from the response packet
#
# RETURN VALUE:
#   Result
#
######################################################################
sub GetResult
{
  my $self = shift;
  my $type = ref($self) || die "$self is not an object";

  if ($self->{"cmd_code"} != 75)
  {
    return -1;
  }

  return $self->{"result"};
}

######################################################################
# METHOD:
#   GetResultText
#
# DESCRIPTION:
#   Subroutine to retrieve result from the response packet in text form
#
# RETURN VALUE:
#   Result text string
#
######################################################################
sub GetResultText
{
  my $self = shift;
  my $type = ref($self) || die "$self is not an object";

  if ($self->{"cmd_code"} != 75)
  {
    return "Unknown";
  }

  if( $self->{"result"} eq 1 )
  {
    return "Unsupported";
  }
  elsif( $self->{"result"} eq 2 )
  {
    return "Activated";
  }
  elsif( $self->{"result"} eq 3 )
  {
    return "Deactivated";
  }
  elsif( $self->{"result"} eq 4 )
  {
    return "Already Activated";
  }
  elsif( $self->{"result"} eq 5 )
  {
    return "Already Deactivated";
  }

  return "Not sure...";
}

