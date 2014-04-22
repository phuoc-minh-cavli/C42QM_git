#!/usr/bin/perl
#******************************************************************************
#* $Header: //components/rel/mcfg.mpss/10.1.1/mcfg_gen/scripts/MCFGStructureItem.pm#1 $
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

package MCFGStructureItem;
use Moose; # automatically turns on strict & warnings
use MCFGStructureElement;
use MCFGStructureTRL;
use McfgGenUtils;

use File::Spec;
use File::Basename;
use Data::Dumper;
use JSON::XS;


#====== Attribute Declarations ======

extends 'MCFGStructureElement';
has 'category' => ( is => 'rw', isa => 'Str', default => 'MCFG' ); 
has 'id' => ( is => 'rw', isa => 'Str', default => 'none' ); 

#====== Package Variable Declarations ======
my %attrib_filter_map = (
  'efs'          => [qw|attrib_blacklist type sizeOf|],  # for now, type and sizeOf will still come from spreadsheet instead of metaData
  'efs_item'     => [qw|attrib_blacklist type sizeOf|],  # for now, type and sizeOf will still come from spreadsheet instead of metaData
  'scalar'       => [qw|attrib_blacklist type sizeOf|],  # for now, type and sizeOf will still come from spreadsheet instead of metaData
  'prl'          => [qw|id attrib_blacklist type sizeOf|],  # for now, type and sizeOf will still come from spreadsheet instead of metaData
  'group'        => [qw|id attrib_blacklist type sizeOf|],  # for now, type and sizeOf will still come from spreadsheet instead of metaData
  'efs_dir'      => [qw|id attrib_blacklist type sizeOf|],  # for now, type and sizeOf will still come from spreadsheet instead of metaData
  'config'       => [qw|id attrib_blacklist type sizeOf|],  # for now, type and sizeOf will still come from spreadsheet instead of metaData
  'active_config'=> [qw|id attrib_blacklist type sizeOf|],  # for now, type and sizeOf will still come from spreadsheet instead of metaData
  'trl'          => [qw|id attrib_blacklist type sizeOf|],  # for now, type and sizeOf will still come from spreadsheet instead of metaData
);

my $DB_PARSER = 'modem_proc/mcfg/mcfg_gen/scripts/read_apex_xml_dump_nv_xml.pl';
#====== Method Declarations ======

# get_attributes()      Lists the available attributes for an object
# @param: none          Only object reference is passed in as part of call
# @return: @att_list    Array of attributes for an object
sub get_attributes
{
  my ($self) = shift;
  my @list2 = __PACKAGE__->meta->get_attribute_list;
  my @att_list = $self->SUPER::get_attributes();
  push @att_list, $self->meta->get_attribute_list;

  return @att_list;
}
  
#====== Function Declarations ======

# NormalizeStr()  Uses MetaData structure definitions to collect TLV IDs from item hash
# @param: $str         hash containig TRL data (typically from spreadsheet)
# @return: $str         Assigns metaData structures corresponding to TRL TLV types found to a new item key
sub NormalizeStr
{
  my ($str) = @_; 

  if (defined ($str))
  {
    #remove double quotes
    $str =~ s|"|'|g;
    #replace & with and 
    $str =~ s|&| and |g;
  }

  return $str;
}

# AttachTrlIMetaData()  Uses MetaData structure definitions to collect TLV IDs from item hash
# @param: $item         hash containig TRL data (typically from spreadsheet)
# @return: None         Assigns metaData structures corresponding to TRL TLV types found to a new item key
sub AttachTrlIMetaData
{
  my ($item) = @_;
  my @tlv_id_arr = MCFGStructureTRL::getTlvIdList($item);
  my @trlMetaData = ();

  foreach my $id (@tlv_id_arr)
  {
    my $idStructure = MCFGStructureTRL::getTlvId($id);
    #print Dumper($idStructure);
    if (!defined($idStructure))
    {
      print "No TRL record structure data for id: '$id'\n";
      print "Moving to next id\n";
    }
    else
    {
      for my $idElement (@$idStructure)
      {
       # print Dumper($idElement);
        push (@trlMetaData, MCFGStructureElement->new({
                              name =>        $idElement->{name},
                              description => $idElement->{description},
                              comment =>     $idElement->{comment},
                              type =>        $idElement->{type},
                              sizeOf =>      $idElement->{sizeOf},
                            }));
      }
    }
  }

  $item->{ElemMetaData} = \@trlMetaData;
  $item->{MetaData} = MCFGStructureItem->new( 
                          name => 'MCFG TRL record',
                          description => 'Configuration Framework Data', 
                          comment => '',
                          category => 'MCFG',
                          attrib_blacklist => $attrib_filter_map{$item->{ItemType}},  # for now, type and sizeOf will still come from spreadsheet instead of metaData
                      );
}

sub AttachGroupMetaData
{
  my ($item) = @_;

  $item->{MetaData} = MCFGStructureItem->new( 
                          name => basename($item->{Filename}),
                          description => 'MCFG Group XML', 
                          comment => '',
                          category => 'MCFG',
                          attrib_blacklist => $attrib_filter_map{$item->{ItemType}},  # for now, type and sizeOf will still come from spreadsheet instead of metaData
                      );
}

# AttachStorageItemMetaData()  Adds metaData for a storage item to the $item hash reference
# @param: $item         hash containing storage item data (typically from spreadsheet)
# @return: None         Assigns metaData structures corresponding to information pulled from database file
sub AttachStorageItemMetaData
{
  my ($item) = shift;
  my $status = 0;
  my $itemID = defined($item->{ID})? $item->{ID} 
                                   : $item->{Filepath};
  if (! $itemID)
  {
    die "No item identifier listed for setting. This means either NV# or EFS target path is missing for this item. Quitting application \n";
  }

  #print Dumper($item);
  # If a version is needed it will be the first element. No harm passing 1st element if version is not needed (for now)
  my $struct = getItemStructure({ itemID => $itemID, version => $item->{Elements}[0]->{Value} });
  #print Dumper($struct);
  if (! $struct)
  {
    print "Appropriate structure NOT found for item '$itemID'. This may be OK if item is not part of QXDM database\n";
    $item->{MetaData}->attrib_blacklist($attrib_filter_map{$item->{ItemType}});
    $item->{MetaData}->name(basename($itemID));
    $item->{MetaData}->category('Unknown');
  }
  else
  {
    ($item->{MetaData}, $item->{ElemMetaData}) = parseItemStructure($struct, $item->{ItemType});
    $status = 1;
  }

  return $status;
}

# chdirRoot()     Determines MPSS build root 
# @param: $dir    Path to a file located within mcfg folder (either build_mcfgs.exe or this filepath
# @return: $dir   returns directory path of build root (up to but not including modem_proc folder)
sub chdirRoot
{
  my ($dir) = @_;
  my $node = '';

  my @dirs = File::Spec->splitdir( $dir );
  until ($node eq 'modem_proc') 
  {
    $node = pop @dirs;
  }

  if (scalar(@dirs) < 1)
  {
    die "No modem_proc directory found when searching for build root\n";
  }
  else
  {
    $dir = File::Spec->catdir(@dirs);
    return $dir;
  }
}

# GetScr()          Builds the path to the script used for parsing QXDM database
# @param: none
# @return: $script  complete path to script
sub GetScr
{
  #Expects script to be in same location as current file
  my $dir = File::Spec->rel2abs(dirname(__FILE__)); #__FILE__ reports build_mcfg.exe intead of pm module within executable
  my $build_root = chdirRoot($dir);
  my $script = File::Spec->catfile($build_root, $DB_PARSER);
  $script = File::Spec->canonpath($script);
  $script =~ s/^[A-Za-z]:\\<//; # Compiled exe file prefixes syscall with "X:\<"
  return $script;
}

# getItemStructure()    Gathers structure data from QXDM database via system call (currently to an external script)
# @param: $options      Hash key, value options passed to system call. these include item#, version of item, and output format 
# @return: $struct      Hash representation of the storage item structure returned by system call to database file
sub getItemStructure
{
   my $options = shift;
   my $itemID = defined($options->{itemID})? $options->{itemID}: undef;
   my $output = defined($options->{output})? $options->{output}: 'json';
   my $version = defined($options->{version})? $options->{version}: 0;

   my $struct = undef;
   
   if (IsBlank($version))
   {
     $version = 0;
   }

   if ($itemID)
   {
     my $script = GetScr();

     #print "script: $script\n";
     my @scr_call = ('perl ', $script, '--item ', $itemID, '--op ', $output, '--version ', $version);

     print "\n--syscall: " . join(" ", @scr_call) . "\n";
     $struct = `@scr_call 2>&1`; 

     #Purposely quit if modules for db parser are missing
     (my $missing_module) = $struct =~ m/(Can't locate .*\@INC)/g;
     die "Error in call to db parser: $missing_module" if ($missing_module);

     # Script returns nothing if item is not in DB
     if ($struct)
     {
       $struct = decode_json($struct);
     }
     else
     {
       print "No structure returned for item '$itemID'\n";
     }

   }
   else
   {
     print "No item passed in getItemStructure\n";
   }

   return $struct;
}

# parseStructMembers()            Parses structure hash representing a storage items members to gather metaData
# @param: $ElemMetaData, $Member  Reference to an array that will contain metaData for each structure element of a storage item
# @return: none                   Adds metadata to the metaData array
sub parseStructMembers
{
  my ($ElemMetaData, $Member, $StructArrIndex) = @_;

  #print Dumper($Member);
  if (!defined($Member))
  {
    return;
  }
  foreach my $index (@$Member)
  {
    # Recursively parse through structures to keep list of elementData in correct order
    if (!scalar (%$index))
    {
      #die "No indices in struct member. Verify database supports NV version listed\n";
      print "No indices found during struct parsing. Verify database supports NV version listed\n";
      return;
    }
    elsif (defined($index->{Struct}))
    {
      # Repeatedly parse a structure if it is an array (i.e. sizeOf > 1)
      my $StructArrSize = $index->{sizeOf};

      # sizeOf can represent different things for different structures, conditions below
      # help determine when sizeOf actually represents the size of a struct array
      if ($StructArrSize > 1 && $StructArrSize < 500 && $index->{name} !~ /^id: \d+/)
      {
        for (my $i = 0; $i < $StructArrSize; $i++)
        {
          parseStructMembers($ElemMetaData, $index->{Struct}->{Member}, $i);
        }
      }
      else
      {
        parseStructMembers($ElemMetaData, $index->{Struct}->{Member});
      }
    }
    else # $member[$index] is not a structure
    {
      my $element;
      # blobs (Reserved bytes) currently do no report the correct size, type, name, or description
      # TODO: See if apex_xml_dump_nv_xml can correct this
      if ($index->{type} eq 'blob')
      {
        # size will be taken from corresponding element TLV in item hash until TODO item above is complete
        $element = MCFGStructureElement->new( name => 'ReservedBytes',
                                              description => 'Bytes reserved for future versions',
                                              type => 'uint8' );
      }
      else
      {
        if (defined($StructArrIndex))
        {
          $element = MCFGStructureElement->new( name => NormalizeStr($index->{name} . "[$StructArrIndex]"), 
                                                type => $index->{type}, 
                                                sizeOf => $index->{sizeOf}, 
                                                description => NormalizeStr($index->{description}) );
        }
        else
        {
          $element = MCFGStructureElement->new( name => NormalizeStr($index->{name}), 
                                                type => $index->{type}, 
                                                sizeOf => $index->{sizeOf}, 
                                                description => NormalizeStr($index->{description}) );
        }
      }
      push @$ElemMetaData, $element;
    }
  }
}

# parseItemStructure()  Parses structure hash representing a storage item to gather metaData
# @param: $struct       Hash representing storage item structure data
# @return: $ItemMetaData, $ElemMetaData      References to metaData for the item and its associated member elements
sub parseItemStructure
{
  my ($struct, $itemType) = @_;
  my $ItemMetaData = MCFGStructureItem->new( id => NormalizeStr($struct->{id}), 
                                             name => basename($struct->{name}), 
                                             category => $struct->{category}, 
                                             description => NormalizeStr($struct->{description}), 
                                             attrib_blacklist => $attrib_filter_map{$itemType},  # for now, type and sizeOf will still come from spreadsheet instead of metaData
                                           );
  my $ElemMetaData = [];
  parseStructMembers($ElemMetaData, $struct->{Member}, $struct->{sizeOf});
  # my ($pkg, $file, $line, $subrout) = caller; print $subrout . ": \n"; print Dumper($ElemMetaData);

  return ($ItemMetaData, $ElemMetaData);
}

# InitializeMetaData()  Initialize metaData for each element of the storage item. Usedful when item or its struct version is not present in database
# @param: $item         hash containing storage item data (typically from spreadsheet)
# @return: None         Assigns default metaData to the item hash
sub InitializeMetaData
{
  my ($item) = shift;
  my $ItemMetaData = MCFGStructureItem->new();
  my @ElemMetaData = ();

  # create MetaData object for each element in item
  foreach my $el (@{ $item->{Elements} })
  {
    # Note that subscription is currently a member, but will 
    # later be removed and listed as NV attribute instead
    push (@ElemMetaData, MCFGStructureElement->new());
  }
  $item->{MetaData} = $ItemMetaData;
  $item->{ElemMetaData} = \@ElemMetaData;

  return $item;
}

# Purpose: Obtain an item's structure and return
# a collection of objects related to that 
# Param: Reference to item object
sub AttachItemMetaData
{
  my ($item) = shift;

  #print Dumper($item);
  $item = InitializeMetaData($item);
  ## Handlers will populate metaData fields with more accurate information
  my $handler  = \&AttachStorageItemMetaData;
  my $handlers = {
  #  "efs"      => \&AttachStorageItemMetaData, # Currently uses AttachStorageItemMetaData 
  #  "efs_item" => \&AttachStorageItemMetaData, # Currently uses AttachStorageItemMetaData
  #  "efs_dir"  => \&AttachEFSDirMetaData,      # Currently uses AttachStorageItemMetaData
  #  "prl"      => \&AttachPrlMetaData,         # Currently uses AttachStorageItemMetaData 
     "trl"      => \&AttachTrlIMetaData,
     "group"    => \&AttachGroupMetaData,       
  };
   
  my $metaType = $item->{ItemType};
  if(defined $handlers->{$metaType}) {
     $handler = $handlers->{$metaType};
  }
  
  my $status = &{$handler}($item);

  return $item;
}

#====== Functions used for inspecting objects ========
#my $meta = __PACKAGE__->meta;
#for ($meta->get_all_attributes)
#{
#  print $_;
#}
#
#print "method:\n";
#for ($meta->get_all_methods)
#{
#  print Dumper($_);
#}
#AttachItemMetaData;
#my $struct = getItemStructure({itemID => 70291, version => 0, output => 'json' });
#print Dumper($struct);
#print parseItemStructure($struct, 'efs');

#Increases speed of Moose by making object construction/destruction inlined
#__PACKAGE__->meta->make_immutable;
no Moose;
1;
