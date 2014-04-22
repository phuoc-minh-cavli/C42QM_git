# 
#
#*****************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc. 
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# This program is confidential and a trade secret of Qualcomm, Inc.  The
# receipt or possession of this program does not convey any rights to reproduce 
# or disclose its contents or to manufacture, use or sell anything that this 
# program describes in whole or in part, without the express written consent of
# Qualcomm, Inc.  The recipient and/or possessor of this program shall not 
# reproduce or adapt or disclose or use this program except as expressly 
# allowed by a written authorization from Qualcomm, Inc.
#
# *****************************************************************************
#
# Description:
# This is a support file for gnss_wbiq_collect.pl, gnss_nbiq_collect.pl and
# gnss_adciq_collect.pl. This should be in the same directory as those files.
#
#*****************************************************************************


package IqTest;

use Win32::OLE;
use Win32::OLE::Variant;

my %fields = (
       # 24 bytes in total
       cmd => undef, # 1 byte
       subsys_id => undef,  # 1 byte
       sub_cmd => undef, # 2 bytes
       gps_cmd => undef, # 1 byte
       version => undef, # 1 byte
       testcontrol => undef, # 1 byte
       flag => undef, # 2 bytes
       retransmitpktnum => undef, # 2 bytes
       rfstatusloggingratehz => undef, # 1 byte
       collectmode => undef, # 1 byte
       glo_r1_chan => undef, # 1 byte
       glo_hw_chan => undef, # 1 byte
       num1ksamples => undef, # 2 bytes
       integrations => undef, # 1 byte
       centerfreqnbhz => undef, # 4 bytes
       adciqsatthresh => undef, # 4 bytes
       adciqwidebandjammerstartfreqhz => undef, # 4 bytes
       adciqwidebandjammerstopfreqhz => undef, # 4 bytes
);

# METHOD:
#   new
#
# DESCRIPTION:
#   Constructor
#
# RETURN VALUE:
#   Instance
#
sub new
{
   my $that = shift;
   my $class = ref($that) || $that;
   my $self = { %fields,
   };
   bless $self, $class;
   return $self;
}

# METHOD:
#   AUTOLOAD
#
# DESCRIPTION:
#   Performs object validataion.
#
# RETURN VALUE:
#   reference object
#
sub AUTOLOAD
{
   my $self = shift;
   my $type = ref($that) || die "$self is not an object";
   my $name = $AUTOLOAD;
   $name =~ s/.*://;   #strip fully-qualified portion
   unless( exists $self->{$name} ) {
      die "Can't access '$name' field in object of class $type";
   }
   if( @_) {
      return $self->{$name} = shift;
   }else{
      return $self->{$name};
   }
}

sub uint32toBytes {
   my ($arg) = @_;
   my($byte1, $byte2, $byte3, $byte4);

   # printf ("%d", $arg);
    
   $byte1 = $arg & 0xFF; $arg = $arg >> 8;
   $byte2 = $arg & 0xFF; $arg = $arg >> 8;
   $byte3 = $arg & 0xFF; $arg = $arg >> 8;
   $byte4 = $arg & 0xFF;

   # printf ("  %d %d %d %d\n", $byte1, $byte2, $byte3, $byte4);

   return($byte1, $byte2, $byte3, $byte4);

}

# METHOD:
#   SetCaptureCmd
#
# DESCRIPTION:
#   Initialize HS bitmap capture command packet.
#
# RETURN VALUE:
#   None
#
sub SetCaptureIqTestCmd
{
   my $self = shift;

   my $length = 34;
   my $type = ref($self) || die "$self is not an object";

   my $req_pkt = Variant(VT_ARRAY | VT_UI1, $length);

   $req_pkt->Put(0, $self->{"cmd"});
   $req_pkt->Put(1, $self->{"subsys_id"});
   $req_pkt->Put(2, 0xFF & $self->{"sub_cmd"});

   my $upper = 0xFF & ($self->{"sub_cmd"} >> 8);
   $req_pkt->Put(3, $upper);
   $req_pkt->Put(4, $self->{"gps_cmd"});
   $req_pkt->Put(5, $self->{"version"});
   $req_pkt->Put(6, $self->{"testcontrol"});
   $req_pkt->Put(7, $self->{"flag"});
   
   my $upper = 0xFF & ($self->{"flag"} >> 8);
   $req_pkt->Put(8, $upper);   
   
   $req_pkt->Put(9, $self->{"retransmitpktnum"});
   
   my $upper = 0xFF & ($self->{"retransmitpktnum"} >> 8);
   $req_pkt->Put(10, $upper);

   $req_pkt->Put(11, $self->{"rfstatusloggingratehz"});
   $req_pkt->Put(12, $self->{"collectmode"});
   $req_pkt->Put(13, $self->{"glo_r1_chan"});
   $req_pkt->Put(14, $self->{"glo_hw_chan"});
   $req_pkt->Put(15, $self->{"num1ksamples"});
   
   my $upper = 0xFF & ($self->{"num1ksamples"} >> 8);
   $req_pkt->Put(16, $upper);
   $req_pkt->Put(17, $self->{"integrations"});
   
   my @argsTopass = uint32toBytes( $self->{"centerfreqnbhz"});

   my $i = 18;
   foreach (@argsTopass)
   {
      $req_pkt->Put($i, $_);
      $i++;
   }

   my @argsTopass = uint32toBytes( $self->{"adciqsatthresh"});

   my $i = 22;
   foreach (@argsTopass)
   {
      $req_pkt->Put($i, $_);
      $i++;
   }
   
   my @argsTopass = uint32toBytes( $self->{"adciqwidebandjammerstartfreqhz"});

   my $i = 26;
   foreach (@argsTopass)
   {
      $req_pkt->Put($i, $_);
      $i++;
   }

   my @argsTopass = uint32toBytes( $self->{"adciqwidebandjammerstopfreqhz"});

   my $i = 30;
   foreach (@argsTopass)
   {
      $req_pkt->Put($i, $_);
      $i++;
   }

   return $req_pkt;
}
