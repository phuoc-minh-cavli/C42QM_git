#!/usr/bin/perl
#******************************************************************************
#* $Header: //components/rel/mcfg.mpss/10.1.1/mcfg_gen/scripts/MCFGStructureElement.pm#1 $
#* $DateTime: 2020/02/19 23:07:48 $
#*
#* 
#******************************************************************************
#*
#* Copyright (c) 2016 Qualcomm Technologies, Inc.
#* All rights reserved.
#* Qualcomm Technologies, Inc. Confidential and Proprietary.
#*
#******************************************************************************

package MCFGStructureElement;
use Moose; # automatically turns on strict & warnings

#====== Attribute Declarations ======

has 'name' => ( is => 'rw', isa => 'Str', default => 'Unidentified', );
has 'description' => ( is => 'rw', isa => 'Str|Undef', default => '', );
has 'comment' => ( is => 'rw', isa => 'Str|Undef', default => '', );
has 'type' => ( is => 'rw', isa => 'Str|Undef', ); 
has 'sizeOf' => ( is => 'rw', isa => 'Int|Undef', );

# This filter may be used to determine which metaData attributes are not listed in the XML
has 'attrib_blacklist' => (
  is => 'rw',
  isa => 'ArrayRef',
  default => sub { [qw|attrib_blacklist type sizeOf|] },  # for now, type and sizeOf will still come from spreadsheet instead of metaData
  #builder => '_build_attrib_blacklist', # will be used if determining default values becomes more complicated 
);

#====== Method Declarations ======

#sub _build_attrib_blacklist
#{
#  my $self = shift;
#  my @attributes = @_;
#
#  if (@attributes)
#  {
#    push @
#  }
#  return [qw|attrib_blacklist type sizeOf @attributes|];
#}

sub get_attributes
{
  my ($self) = shift;

  return __PACKAGE__->meta->get_attribute_list;
}

no Moose;
# Increases speed of Moose by making object construction/destruction inlined
#__PACKAGE__->meta->make_immutable;
1;
