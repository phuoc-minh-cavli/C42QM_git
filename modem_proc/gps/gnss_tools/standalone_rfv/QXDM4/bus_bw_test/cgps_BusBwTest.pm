# 
#$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss_tools/standalone_rfv/QXDM4/bus_bw_test/cgps_BusBwTest.pm#1 $ $DateTime: 2020/07/21 21:23:44 $ $Author: pwbldsvc $
#*****************************************************************************
#  Copyright (C) 2004 Qualcomm, Inc.
#
#                  Qualcomm, Inc.
#                  675 Campbell Technology Parkway
#                  Campbell, CA  95008
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
# This is a support file for cgps_BusBwTest.pl.
# It should be located in the same directory as that file.
#
#*****************************************************************************


package bus_bw_test;

use Win32::OLE;
use Win32::OLE::Variant;

my %fields = (
       cmd => undef,
       id => undef,
       sub_cmd => undef,
       gps_cmd => undef,
       version => undef,
       start_stop_flag => undef,
       test_type => undef,
       task_cnt => undef,
       integ_length => undef,
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
#   SetCaptureBusBwCmd
#
# DESCRIPTION:
#   Initialize HS bitmap capture command packet.
#
# RETURN VALUE:
#   None
#
sub SetCaptureBusBwCmd
{
   my $self = shift;

   my $length = 12;
   my $type = ref($self) || die "$self is not an object";

   my $req_pkt = Variant(VT_ARRAY | VT_UI1, $length);

   $req_pkt->Put(0, $self->{"cmd"});
   $req_pkt->Put(1, $self->{"subsys_id"});
   $req_pkt->Put(2, 0xFF & $self->{"sub_cmd"});

   my $upper = 0xFF & ($self->{"sub_cmd"} >> 8);
   $req_pkt->Put(3, $upper);
   $req_pkt->Put(4, $self->{"gps_cmd"});
   $req_pkt->Put(5, $self->{"version"});
   $req_pkt->Put(6, $self->{"start_stop_flag"});
   $req_pkt->Put(7, $self->{"test_type"});
   $req_pkt->Put(8, $self->{"task_cnt"});
   $req_pkt->Put(9, 0xFF & $self->{"integ_length"});

   my $upper = 0xFF & ($self->{"integ_length"} >> 8);
   $req_pkt->Put(10, $upper);

   $req_pkt->Put(11, 0);
   $req_pkt->Put(12, 0);
   
   return $req_pkt;
}
